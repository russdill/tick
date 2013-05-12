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
#include <ctype.h>
#include <stdlib.h>
#include <ftdi.h>

#include "i2c.h"

struct adapter *adapter_attach(struct ftdi_context *ftdic,
		struct adapter_ops *ops, int speed_hz)
{
	struct adapter *adap;
	unsigned short dir = 0;
	int ret;

	dir |= (1 << ops->reset) | (1 << ops->power);
	dir |= (1 << ops->user) | (1 << ops->i2c_en);
	dir |= (1 << ops->scl) | (1 << ops->sda_out);
	dir &= ~(1 << ops->sda_in);

	adap = calloc(1, sizeof(struct adapter));
	adap->ftdic = ftdic;
	adap->ops = ops;

	adap->speed_hz = ops->speed_hz;
	if (speed_hz > 0 && speed_hz < adap->speed_hz)
		adap->speed_hz = speed_hz;

	ret = ops->attach(adap, dir);
	if (ret < 0) {
		free(adap);
		return NULL;
	}

	return adap;
}

void adapter_detach(struct adapter *adap)
{
	adap->ops->detach(adap);
	free(adap);
}

int arg_to_gpio(struct adapter *adap, const char *arg)
{
	int ret = -1;
	switch (tolower(arg[0])) {
	case 'r':
		ret = adap->ops->reset;
		break;
	case 'u':
		ret = adap->ops->user;
		break;
	case 'p':
		ret = adap->ops->power;
		break;
	default:
		ret = atoi(arg);
	}
	return ret;
}

int adapter_flush(struct adapter *adap)
{
	return adap->ops->flush(adap);
}

int adapter_hw_error(struct adapter *adap)
{
	return adap->hw_error;
}

int adapter_gpio_output(struct adapter *adap, int gpio)
{
	return adap->ops->gpio_output(adap, gpio);
}

int adapter_gpio_input(struct adapter *adap, int gpio)
{
	return adap->ops->gpio_input(adap, gpio);
}

int adapter_gpio_high(struct adapter *adap, int gpio)
{
	return adap->ops->gpio_high(adap, gpio);
}

int adapter_gpio_low(struct adapter *adap, int gpio)
{
	return adap->ops->gpio_low(adap, gpio);
}

int adapter_gpio_get(struct adapter *adap, int gpio)
{
	return adap->ops->gpio_get(adap, gpio);
}

int i2c_xfer(struct adapter *adap, struct i2c_msg msgs[], int count)
{
	int i;
	int ret = 0;

	adap->ops->i2c_start(adap);
	for (i = 0; !adap->i2c_error && i < count; i++) {
		int flags = msgs[i].flags;
		if (!(flags & I2C_M_NOSTART)) {
			if (i)
				adap->ops->i2c_repstart(adap);
			adap->ops->i2c_outb(adap, msgs[i].addr << 1 | !!(flags & I2C_M_RD));
			adap->ops->flush(adap);
		}
		if (flags & I2C_M_RD) {
			int n;
			int count = msgs[i].len;

			for (n = 0; !adap->i2c_error && n < count; n++) {
				adap->ops->i2c_inb(adap, msgs[i].buf + n);
				if (!n && (flags & I2C_M_RECV_LEN)) {
					unsigned char b;
					adap->ops->flush(adap);
					b = msgs[i].buf[0];
					if (adap->i2c_error ||
					    b > I2C_SMBUS_BLOCK_MAX) {
						adap->ops->i2c_acknak(adap, 0);
						ret = -1;
						break;
					}
					count += b;
					msgs[i].len += b;
				}
				adap->ops->i2c_acknak(adap, count - n > 1);
			}
			ret = n;
		} else {
			int n;

			for (n = 0; !adap->i2c_error && n < msgs[i].len; n++)
				adap->ops->i2c_outb(adap, msgs[i].buf[n]);

			ret = n;
		}
	}

	adap->ops->i2c_stop(adap);
	adap->ops->flush(adap);

	return adap->i2c_error ? -1 : ret;
}

int i2c_master_send(struct adapter *adap, unsigned char addr, const char *buf,
			int count)
{
	struct i2c_msg msg = {
		.addr = addr,
		.len = count,
		.buf = (unsigned char *) buf,
	};
	return i2c_xfer(adap, &msg, 1);
}

int i2c_master_recv(struct adapter *adap, unsigned char addr, char *buf,
		int count)
{
	struct i2c_msg msg = {
		.addr = addr,
		.flags = I2C_M_RD,
		.len = count,
		.buf = (unsigned char *) buf,
	};
	return i2c_xfer(adap, &msg, 1);
}

int i2c_smbus_xfer(struct adapter *adap, unsigned char addr,
			unsigned short flags, char rdwr,
			unsigned char command, int size,
			union i2c_smbus_data *data)
{
	unsigned char msgbuf0[I2C_SMBUS_BLOCK_MAX+3];
	unsigned char msgbuf1[I2C_SMBUS_BLOCK_MAX+2];
	int num = rdwr == I2C_SMBUS_READ ? 2 : 1;
	int ret;
	struct i2c_msg msgs[] = {
		{ .addr = addr, .flags = flags, .len = 1, .buf = msgbuf0 },
		{ .addr = addr, .flags = flags | I2C_M_RD, .len = 0, .buf = msgbuf1 },
	};

	msgbuf0[0] = command;
	switch (size) {
	case I2C_SMBUS_QUICK:
		msgs[0].len = 0;
		msgs[0].flags |= rdwr == I2C_SMBUS_READ ? I2C_M_RD : 0;
		num = 1;
		break;
	case I2C_SMBUS_BYTE:
		if (rdwr == I2C_SMBUS_READ) {
			msgs[0].flags |= I2C_M_RD;
			num = 1;
		}
		break;
	case I2C_SMBUS_BYTE_DATA:
		if (rdwr == I2C_SMBUS_READ)
			msgs[1].len = 1;
		else {
			msgs[0].len = 2;
			msgbuf0[1] = data->byte;
		}
		break;
	case I2C_SMBUS_WORD_DATA:
		if (rdwr == I2C_SMBUS_READ)
			msgs[1].len = 2;
		else {
			msgs[0].len = 3;
			msgbuf0[1] = data->word >> 8;
			msgbuf0[2] = data->word & 0xff;
		}
		break;
	case I2C_SMBUS_BLOCK_DATA:
		if (rdwr == I2C_SMBUS_READ) {
			msgs[1].flags |= I2C_M_RECV_LEN;
			msgs[1].len = 1;
		} else {
			msgs[0].len = data->block[0] + 2;
			if (msgs[0].len > I2C_SMBUS_BLOCK_MAX + 2)
				return -1;
			memcpy(msgbuf0 + 1, data->block, msgs[0].len - 1);
		}
		break;
	case I2C_SMBUS_I2C_BLOCK_DATA:
		if (rdwr == I2C_SMBUS_READ) {
			msgs[1].len = data->block[0];
		} else {
			msgs[0].len = data->block[0] + 1;
			if (msgs[0].len > I2C_SMBUS_BLOCK_MAX + 1)
				return -1;
			memcpy(msgbuf0 + 1, data->block + 1, data->block[0]);
		}
		break;
	default:
		return -1;
	}

	ret = i2c_xfer(adap, msgs, num);
	if (ret < 0)
		return ret;

	if (rdwr == I2C_SMBUS_READ) {
		switch (size) {
		case I2C_SMBUS_BYTE:
			data->byte = msgbuf0[0];
			break;
		case I2C_SMBUS_BYTE_DATA:
			data->byte = msgbuf1[0];
			break;
		case I2C_SMBUS_WORD_DATA:
			data->word = msgbuf1[1] | (msgbuf1[0] << 8);
			break;
		case I2C_SMBUS_I2C_BLOCK_DATA:
			memcpy(data->block + 1, msgbuf1, data->block[0]);
			break;
		case I2C_SMBUS_BLOCK_DATA:
			memcpy(data->block, msgbuf1, msgbuf1[0] + 1);
			break;
		}
	}

	return 0;
}

int i2c_probe_func_quick_read(struct adapter *adap, unsigned short addr)
{
	return i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_READ, 0,
        			I2C_SMBUS_QUICK, NULL) >= 0;
}

int i2c_smbus_read_byte(struct adapter *adap, unsigned char addr)
{
	union i2c_smbus_data data;
	int ret;
	ret = i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE,
			&data);
	return ret < 0 ? ret : data.byte;
}

int i2c_smbus_write_byte(struct adapter *adap, unsigned char addr,
			unsigned char value)
{
	return i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_WRITE, value,
			I2C_SMBUS_BYTE, NULL);
}

int i2c_smbus_read_byte_data(struct adapter *adap, unsigned char addr,
			unsigned char command)
{
	union i2c_smbus_data data;
	int ret;
	ret = i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_READ, command,
			I2C_SMBUS_BYTE_DATA, &data);
	return ret < 0 ? ret : data.byte;
}

int i2c_smbus_write_byte_data(struct adapter *adap, unsigned char addr,
			unsigned char command, unsigned char value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_WRITE, command,
			I2C_SMBUS_BYTE_DATA, &data);
}

int i2c_smbus_read_word_data(struct adapter *adap, unsigned char addr,
			unsigned char command)
{
	union i2c_smbus_data data;
	int ret;
	ret = i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_READ, command,
			I2C_SMBUS_WORD_DATA, &data);
	return ret < 0 ? ret : data.word;
}

int i2c_smbus_write_word_data(struct adapter *adap, unsigned char addr,
			unsigned char command, unsigned short value)
{
	union i2c_smbus_data data;
	data.word = value;
	return i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_WRITE, command,
			I2C_SMBUS_WORD_DATA, &data);
}

int i2c_smbus_read_block_data(struct adapter *adap, unsigned char addr,
				unsigned char command, unsigned char *values)
{
	union i2c_smbus_data data;
	int ret;
	ret = i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_READ, command,
			I2C_SMBUS_BLOCK_DATA, &data);
	if (ret)
		return ret;
	memcpy(values, data.block + 1, data.block[0]);
	return data.block[0];
}

int i2c_smbus_write_block_data(struct adapter *adap, unsigned char addr,
				unsigned char command, unsigned char length,
				unsigned char *values)
{
	union i2c_smbus_data data;
	if (length > I2C_SMBUS_BLOCK_MAX)
		length = I2C_SMBUS_BLOCK_MAX;
	data.block[0] = length;
	memcpy(data.block + 1, values, length);
	return i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_WRITE, command,
			I2C_SMBUS_BLOCK_DATA, &data);
}

int i2c_smbus_read_i2c_block_data(struct adapter *adap, unsigned char addr,
				unsigned char command, unsigned char length,
				unsigned char *values)
{
	union i2c_smbus_data data;
	int ret;
	if (length > I2C_SMBUS_BLOCK_MAX)
		length = I2C_SMBUS_BLOCK_MAX;
	data.block[0] = length;
	ret = i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_READ, command,
			I2C_SMBUS_I2C_BLOCK_DATA, &data);
	if (ret)
		return ret;
	memcpy(values, data.block + 1, data.block[0]);
	return data.block[0];
}

int i2c_smbus_write_i2c_block_data(struct adapter *adap, unsigned char addr,
				unsigned char command, unsigned char length,
				unsigned char *values)
{
	union i2c_smbus_data data;
	if (length > I2C_SMBUS_BLOCK_MAX)
		length = I2C_SMBUS_BLOCK_MAX;
	data.block[0] = length;
	memcpy(data.block + 1, values, length);
	return i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_WRITE, command,
			I2C_SMBUS_I2C_BLOCK_DATA, &data);
}
