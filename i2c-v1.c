/*

Copyright (C) 2013 Russ Dill <Russ.Dill@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ftdi.h>

#include "i2c.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

//#define CHART

struct v1_priv {
	unsigned char dir;
	unsigned char gpio;

	int wr_bytes;
	unsigned char *wr_buffer;

	unsigned char *rd_buffer;
	unsigned char nack;
	int in_bit;
	int in_byte;

	unsigned char *dest[4096];
	int dest_n;

	int hw_error;
	int chunk;
};

#ifdef CHART
static int scls[4096];
static int sdas[4096];
static int scl_count;
static int sda_count;
#endif

static int v1_attach(struct adapter *adap, unsigned short dir)
{
	struct v1_priv *priv;
	int ret;
	unsigned char initial = 0;

	ftdi_usb_purge_buffers(adap->ftdic);

	ret = ftdi_set_bitmode(adap->ftdic, dir, BITMODE_SYNCBB);
	if (ret < 0)
		return ret;

	ret = ftdi_read_pins(adap->ftdic, &initial);
	if (ret < 0)
		return ret;

	/* 4 bits per clock cycle */
	/* may need other component of calculation */
	ret = ftdi_set_baudrate(adap->ftdic, adap->speed_hz * 4);
	if (ret < 0)
		return ret;

	ftdi_usb_purge_buffers(adap->ftdic);

	initial |= (1 << adap->ops->scl) | (1 << adap->ops->sda_out);
	ret = ftdi_write_data(adap->ftdic, &initial, 1);
	if (ret < 0)
		return ret;
	ret = ftdi_read_data(adap->ftdic, &initial, 1);
	if (ret < 0)
		return ret;

	priv = calloc(1, sizeof(struct v1_priv));
	adap->priv = priv;
	priv->dir = dir;
	priv->chunk = adap->ftdic->max_packet_size - 2;
	priv->gpio = initial;

	priv->wr_buffer = malloc(priv->chunk);
	priv->rd_buffer = malloc(priv->chunk);

	return 0;
}

static void v1_detach(struct adapter *adap)
{
	struct v1_priv *priv = adap->priv;
#ifdef CHART
	int i;
	printf("IDX: ");
	for (i = 0; i < sda_count; i += 4)
		printf("%03d,", i/4);
	printf("\nSDA: ");
	for (i = 0; i < sda_count; i++)
		printf("%c", sdas[i]);
	printf("\nSCL: ");
	for (i = 0; i < scl_count; i++)
		printf("%c", scls[i]);
	putchar('\n');
#endif
	free(priv->wr_buffer);
	free(priv->rd_buffer);
	free(priv);
}

static int v1_gpio_output(struct adapter *adap, int gpio)
{
	struct v1_priv *priv = adap->priv;

	if (!(priv->dir & (1 << gpio)) && !adap->hw_error) {
		int ret;
		priv->dir |= 1 << gpio;
		ret = ftdi_set_bitmode(adap->ftdic, priv->dir, BITMODE_SYNCBB);
		if (ret < 0)
			adap->hw_error = ret;
	}

	return adap->hw_error;
}

static int v1_gpio_input(struct adapter *adap, int gpio)
{
	struct v1_priv *priv = adap->priv;

	if ((priv->dir & (1 << gpio)) && !adap->hw_error) {
		int ret;
		priv->dir &= ~(1 << gpio);
		ret = ftdi_set_bitmode(adap->ftdic, priv->dir, BITMODE_SYNCBB);
		if (ret < 0)
			adap->hw_error = ret;
	}

	return adap->hw_error;
}

static int v1_gpio_state(struct adapter *adap, int gpio, int state)
{
	struct v1_priv *priv = adap->priv;
	int ret;
	unsigned char tmp;

	if (adap->hw_error)
		return adap->hw_error;

	priv->gpio &= ~(1 << gpio);
	priv->gpio |= (!state) << gpio;

	ret = ftdi_write_data(adap->ftdic, &priv->gpio, 1);
	if (ret >= 0)
		ret = ftdi_read_data(adap->ftdic, &tmp, 1);

	if (ret < 0)
		adap->hw_error = ret;

	return adap->hw_error;
}

static int v1_gpio_high(struct adapter *adap, int gpio)
{
	return v1_gpio_state(adap, gpio, 1);
}

static int v1_gpio_low(struct adapter *adap, int gpio)
{
	return v1_gpio_state(adap, gpio, 0);
}

static int v1_gpio_get(struct adapter *adap, int gpio)
{
	struct v1_priv *priv = adap->priv;
	int ret;
	unsigned char tmp;

	if (adap->hw_error)
		return adap->hw_error;

	ret = ftdi_write_data(adap->ftdic, &priv->gpio, 1);
	if (ret >= 0)
		ret = ftdi_read_data(adap->ftdic, &tmp, 1);

	if (ret < 0)
		adap->hw_error = ret;

	return !!(tmp & (1 << gpio));
}

static void process_input(struct adapter *adap)
{
	struct v1_priv *priv = adap->priv;
	int ret;
	int i;
	int len;

	if (!priv->dest_n || adap->hw_error)
		return;

	len = priv->dest_n < priv->chunk ? priv->dest_n : priv->chunk;
	ret = ftdi_read_data(adap->ftdic, priv->rd_buffer, len);
	if (ret < 0) {
		adap->hw_error = ret;
		adap->i2c_error = 1;
		return;
	}

#ifdef CHART
	scls[scl_count++] = ' ';
	sdas[sda_count++] = ' ';
#endif

	for (i = 0; i < ret; i++) {
		int sda = !!(priv->rd_buffer[i] & (1 << adap->ops->sda_in));
#ifdef CHART
		int scl = !!(priv->rd_buffer[i] & (1 << adap->ops->scl));
		scls[scl_count++] = scl ? '#' : '_';
		sdas[sda_count++] = sda ? '#' : '_';
#endif
		if (priv->dest[i] == &priv->nack) {
			if (sda)
				/* NACK */
				adap->i2c_error = 1;
		} else if (priv->dest[i] && !adap->i2c_error) {
			priv->in_byte = (priv->in_byte << 1) | sda;
			priv->in_bit++;
			if (priv->in_bit == 8) {
				*priv->dest[i] = priv->in_byte;
				priv->in_byte = priv->in_bit = 0;
			}
		}
	}

	priv->dest_n -= ret;
	if (priv->dest_n)
		memmove(priv->dest, priv->dest + ret, priv->dest_n);
}

static void flush_output(struct adapter *adap)
{
	struct v1_priv *priv = adap->priv;
	int ret;

	if (!priv->wr_bytes || adap->hw_error)
		return;

	ret = ftdi_write_data(adap->ftdic, priv->wr_buffer, priv->wr_bytes);
	if (ret < 0) {
		adap->hw_error = ret;
		adap->i2c_error = 1;
		return;
	}
	priv->wr_bytes = 0;
}

static int flush_all(struct adapter *adap)
{
	struct v1_priv *priv = adap->priv;
	flush_output(adap);
	while (priv->dest_n && !adap->hw_error)
		process_input(adap);
	return 0;
}

static void append_data_clock(struct adapter *adap, unsigned char data,
			unsigned char clock, unsigned char *dest)
{
	struct v1_priv *priv = adap->priv;

	if (adap->hw_error)
		return;

	if (priv->dest_n == ARRAY_SIZE(priv->dest))
		flush_all(adap);

	priv->gpio &= ~((1 << adap->ops->sda_out) | (1 << adap->ops->scl));
	priv->gpio |= data ? (1 << adap->ops->sda_out) : 0;
	priv->gpio |= clock ? (1 << adap->ops->scl) : 0;
	priv->dest[priv->dest_n++] = dest;
	priv->wr_buffer[priv->wr_bytes++] = priv->gpio;

	if (priv->wr_bytes == priv->chunk)
		flush_output(adap);
}

static int v1_start(struct adapter *adap)
{
	struct v1_priv *priv = adap->priv;

	priv->in_bit = priv->in_byte = adap->i2c_error = 0;
	append_data_clock(adap, 0, 1, NULL);
	append_data_clock(adap, 0, 1, NULL);
	append_data_clock(adap, 0, 0, NULL);
	return 0;
}


static int v1_repstart(struct adapter *adap)
{
	append_data_clock(adap, 1, 0, NULL);
	append_data_clock(adap, 1, 1, NULL);
	append_data_clock(adap, 0, 1, NULL);
	append_data_clock(adap, 0, 0, NULL);
	return 0;
}

static int v1_stop(struct adapter *adap)
{
	append_data_clock(adap, 0, 0, NULL);
	append_data_clock(adap, 0, 1, NULL);
	append_data_clock(adap, 0, 1, NULL);
	append_data_clock(adap, 1, 1, NULL);
	append_data_clock(adap, 1, 1, NULL);
	flush_all(adap);
	return 0;
}

static int v1_outb(struct adapter *adap, unsigned char c)
{
	struct v1_priv *priv = adap->priv;
	int i;
	int sb;

	for (i = 7; i >= 0; i--) {
		sb = (c >> i) & 1;
		append_data_clock(adap, sb, 0, NULL);
		append_data_clock(adap, sb, 1, NULL);
		append_data_clock(adap, sb, 1, NULL);
		append_data_clock(adap, sb, 0, NULL);
	}
	append_data_clock(adap, 1, 0, NULL);
	append_data_clock(adap, 1, 1, NULL);
	append_data_clock(adap, 1, 1, NULL);
	append_data_clock(adap, 1, 0, &priv->nack);
	return 0;
}

static int v1_inb(struct adapter *adap, unsigned char *dest)
{
	int i;

	for (i = 0; i < 8; i++) {
		append_data_clock(adap, 1, 0, NULL);
		append_data_clock(adap, 1, 1, NULL);
		append_data_clock(adap, 1, 1, NULL);
		append_data_clock(adap, 1, 0, dest);
	}
	return 0;
}

static int v1_acknak(struct adapter *adap, int val)
{
	append_data_clock(adap, !val, 0, NULL);
	append_data_clock(adap, !val, 1, NULL);
	append_data_clock(adap, !val, 1, NULL);
	append_data_clock(adap, !val, 0, NULL);
	return 0;
}

struct adapter_ops v1_ops = {
	.attach		= v1_attach,
	.detach		= v1_detach,
	.gpio_output	= v1_gpio_output,
	.gpio_input	= v1_gpio_input,
	.gpio_high	= v1_gpio_high,
	.gpio_low	= v1_gpio_low,
	.gpio_get	= v1_gpio_get,
	.i2c_start	= v1_start,
	.i2c_repstart	= v1_repstart,
	.i2c_stop	= v1_stop,
	.i2c_inb	= v1_inb,
	.i2c_outb	= v1_outb,
	.i2c_acknak	= v1_acknak,
	.flush		= flush_all,
        .sda_out        = 1,
        .sda_in         = 2,
        .scl            = 0,
        .reset          = 8,
        .user           = 9,
        .power          = 15,
        .i2c_en         = 14,
        .interface      = INTERFACE_A,
	.speed_hz	= 50,
	.desc		= "Dual RS232-HS",
};
