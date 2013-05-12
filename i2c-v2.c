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

struct v2_priv {
	unsigned short dir;
	unsigned short gpio;

	int wr_bytes;
	unsigned char *wr_buffer;

	unsigned char *rd_buffer;
	unsigned char nack;

	unsigned char *dest[4096];
	int dest_n;

	int chunk;
};

static void append_byte(struct adapter *adap, unsigned char byte);
static void append_read(struct adapter *adap, unsigned char *dest);

static void process_input(struct adapter *adap)
{
	struct v2_priv *priv = adap->priv;
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

	for (i = 0; i < ret; i++) {
		if (adap->i2c_error)
			break;
		*priv->dest[i] = priv->rd_buffer[i];
		if (priv->dest[i] == &priv->nack && (priv->nack & 1)) {
			printf("nack\n");
			adap->i2c_error = 1;
		}
	}

	priv->dest_n -= ret;
	if (priv->dest_n)
		memmove(priv->dest, priv->dest + ret, priv->dest_n);
}

static void flush_output(struct adapter *adap)
{
	struct v2_priv *priv = adap->priv;
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
	struct v2_priv *priv = adap->priv;
	flush_output(adap);
	while (priv->dest_n && !adap->hw_error)
		process_input(adap);
	return 0;
}

static int update_gpio(struct adapter *adap, int high)
{
	struct v2_priv *priv = adap->priv;

	if (adap->hw_error)
		return adap->hw_error;

	if (priv->wr_bytes + 3 > priv->chunk)
		flush_output(adap);

	if (high) {
		priv->wr_buffer[priv->wr_bytes++] = SET_BITS_HIGH;
		priv->wr_buffer[priv->wr_bytes++] = priv->gpio >> 8;
		priv->wr_buffer[priv->wr_bytes++] = priv->dir >> 8;
	} else {
		priv->wr_buffer[priv->wr_bytes++] = SET_BITS_LOW;
		priv->wr_buffer[priv->wr_bytes++] = priv->gpio & 0xff;
		priv->wr_buffer[priv->wr_bytes++] = priv->dir & 0xff;
	}

	return adap->hw_error;
}

static int v2_gpio_output(struct adapter *adap, int gpio)
{
	struct v2_priv *priv = adap->priv;
	priv->dir |= 1 << gpio;
	return update_gpio(adap, gpio > 7);
}

static int v2_gpio_get(struct adapter *adap, int gpio)
{
	char byte;

	if (gpio > 7) {
		append_byte(adap, GET_BITS_HIGH);
		append_read(adap, (unsigned char *) &byte);
		gpio -= 8;
	} else {
		append_byte(adap, GET_BITS_LOW);
		append_read(adap, (unsigned char *) &byte);
	}
	flush_all(adap);

	return !!(byte & (1 << gpio));
}

static int v2_gpio_input(struct adapter *adap, int gpio)
{
	struct v2_priv *priv = adap->priv;
	priv->dir &= ~(1 << gpio);
	return update_gpio(adap, gpio > 7);
}

static int v2_gpio_high(struct adapter *adap, int gpio)
{
	struct v2_priv *priv = adap->priv;
	priv->gpio |= 1 << gpio;
	return update_gpio(adap, gpio > 7);
}

static int v2_gpio_low(struct adapter *adap, int gpio)
{
	struct v2_priv *priv = adap->priv;
	priv->gpio &= ~(1 << gpio);
	return update_gpio(adap, gpio > 7);
}

static void append_read(struct adapter *adap, unsigned char *dest)
{
	struct v2_priv *priv = adap->priv;

	if (adap->hw_error)
		return;

	if (priv->dest_n + 1 > ARRAY_SIZE(priv->dest))
		flush_all(adap);
	priv->dest[priv->dest_n++] = dest;
}

static void append_data_clock(struct adapter *adap, int data, int clock)
{
	struct v2_priv *priv = adap->priv;

	priv->gpio &= ~(1 << adap->ops->scl);
	priv->gpio &= ~(1 << adap->ops->sda_out);
	if (data < 0) {
		priv->dir &= ~(1 << adap->ops->sda_out);
	} else {
		priv->dir |= 1 << adap->ops->sda_out;
		priv->gpio |= !!data ? (1 << adap->ops->sda_out) : 0;
	}
	priv->gpio |= !!clock ? (1 << adap->ops->scl) : 0;

	update_gpio(adap, 0);
}

static void append_command(struct adapter *adap, unsigned char cmd,
				unsigned short data)
{
	struct v2_priv *priv = adap->priv;

	if (adap->hw_error)
		return;

	if (priv->wr_bytes + 3 > priv->chunk)
		flush_output(adap);

	priv->wr_buffer[priv->wr_bytes++] = cmd;
	priv->wr_buffer[priv->wr_bytes++] = data & 0xff;
	priv->wr_buffer[priv->wr_bytes++] = (data >> 8) & 0xff;
}

static void append_command1(struct adapter *adap, unsigned char cmd,
				unsigned short data)
{
	append_command(adap, cmd, data << 8);
}

static void append_byte(struct adapter *adap, unsigned char byte)
{
	struct v2_priv *priv = adap->priv;

	if (adap->hw_error)
		return;

	if (priv->wr_bytes + 1 > priv->chunk)
		flush_output(adap);

	priv->wr_buffer[priv->wr_bytes++] = byte;
}

static void v2_set_rate(struct adapter *adap, unsigned int hz)
{
	unsigned short divisor;
	int numerator = 30000000;

	if (hz >= 60000000)
		append_byte(adap, DIS_DIV_5);
	else {
		append_byte(adap, EN_DIV_5);
		numerator /= 5;
	}
	divisor = numerator / hz - 1;
	append_command(adap, TCK_DIVISOR, divisor);
}

static int v2_start(struct adapter *adap)
{
	adap->i2c_error = 0;
	append_data_clock(adap, 0, 1);
	return 0;
}


static int v2_repstart(struct adapter *adap)
{
	append_data_clock(adap, 0, 0);
	append_data_clock(adap, 1, 1);
	append_data_clock(adap, 0, 1);
	return 0;
}

static int v2_stop(struct adapter *adap)
{
	append_data_clock(adap, 0, 0);
	append_data_clock(adap, 0, 1);
	append_data_clock(adap, 1, 1);
	return 0;
}

static int v2_acknak(struct adapter *adap, int val)
{
	append_command1(adap, MPSSE_DO_WRITE | MPSSE_WRITE_NEG | MPSSE_BITMODE,
								val ? 0 : 0xff);
	return 0;
}

static int v2_outb(struct adapter *adap, unsigned char byte)
{
	struct v2_priv *priv = adap->priv;

	append_data_clock(adap, 0, 0);
	append_command(adap, MPSSE_DO_WRITE | MPSSE_WRITE_NEG, 0);
	append_byte(adap, byte);
	append_data_clock(adap, -1, 0);
	append_command1(adap, MPSSE_DO_READ | MPSSE_BITMODE, SEND_IMMEDIATE);
	append_read(adap, &priv->nack);
	return 0;
}

static int v2_inb(struct adapter *adap, unsigned char *dest)
{
	append_data_clock(adap, -1, 0);
	append_command(adap, MPSSE_DO_READ, 0);
	append_data_clock(adap, 0, 0);
	append_read(adap, dest);
	return 0;
}

static int v2_attach(struct adapter *adap, unsigned short dir)
{
	struct v2_priv *priv;
	int ret;
	unsigned short initial = 0;

	ret = ftdi_set_bitmode(adap->ftdic, dir, BITMODE_MPSSE);
	if (ret < 0) {
		fprintf(stderr, "%s: ftdi_set_bitmode failed\n", __func__);
		return ret;
	}

	ftdi_usb_purge_buffers(adap->ftdic);

	priv = calloc(1, sizeof(struct v2_priv));
	adap->priv = priv;
	priv->dir = dir;
	priv->chunk = adap->ftdic->max_packet_size - 2;

	priv->wr_buffer = malloc(priv->chunk);
	priv->rd_buffer = malloc(priv->chunk);

	append_byte(adap, GET_BITS_LOW);
	append_read(adap, (unsigned char *) &initial);
	append_byte(adap, GET_BITS_HIGH);
	append_read(adap, ((unsigned char *) &initial) + 1);
	flush_all(adap);

	priv->gpio = initial;

	v2_set_rate(adap, adap->speed_hz);

	append_byte(adap, DIS_ADAPTIVE);
	append_byte(adap, EN_3_PHASE);
	append_data_clock(adap, 1, 1);
	flush_all(adap);
	usleep(25000);
	ftdi_usb_purge_buffers(adap->ftdic);

	return 0;
}

static void v2_detach(struct adapter *adap)
{
	struct v2_priv *priv = adap->priv;
	free(priv->wr_buffer);
	free(priv->rd_buffer);
	free(adap->priv);
}

struct adapter_ops v2_ops = {
	.attach		= v2_attach,
	.detach		= v2_detach,
	.gpio_output	= v2_gpio_output,
	.gpio_input	= v2_gpio_input,
	.gpio_high	= v2_gpio_high,
	.gpio_low	= v2_gpio_low,
	.gpio_get	= v2_gpio_get,
	.i2c_start	= v2_start,
	.i2c_repstart	= v2_repstart,
	.i2c_stop	= v2_stop,
	.i2c_inb	= v2_inb,
	.i2c_outb	= v2_outb,
	.i2c_acknak	= v2_acknak,
	.flush		= flush_all,
	.sda_out	= 1,
	.sda_in		= 2,
	.scl		= 0,
	.reset		= 8,
	.user		= 9,
	.power		= 15,
	.i2c_en		= 14,
	.interface	= INTERFACE_A,
	.speed_hz	= 400000,
	.desc		= "BeagleBone/XDS100V2",
};
