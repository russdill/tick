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

#ifndef __I2C_H__
#define __I2C_H__

#define I2C_M_NOSTART	0x01
#define I2C_M_RD	0x02
#define I2C_M_RECV_LEN	0x04

struct i2c_msg {
	unsigned char addr;
	unsigned char flags;
	unsigned char len;
	unsigned char *buf;
};

#define I2C_SMBUS_BLOCK_MAX 32

union i2c_smbus_data {
	unsigned char byte;
	unsigned short word;
	unsigned char block[I2C_SMBUS_BLOCK_MAX + 2];
};

#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

#define I2C_SMBUS_QUICK			0
#define I2C_SMBUS_BYTE			1
#define I2C_SMBUS_BYTE_DATA		2
#define I2C_SMBUS_WORD_DATA		3
#define I2C_SMBUS_BLOCK_DATA		4
#define I2C_SMBUS_I2C_BLOCK_DATA	5

struct adapter;

struct adapter_ops {
	int (*attach)(struct adapter*, unsigned short dir);
	void (*detach)(struct adapter *);
	int (*gpio_output)(struct adapter*, int);
	int (*gpio_input)(struct adapter*, int);
	int (*gpio_high)(struct adapter*, int);
	int (*gpio_low)(struct adapter*, int);
	int (*gpio_get)(struct adapter*, int);
	int (*i2c_start)(struct adapter*);
	int (*i2c_repstart)(struct adapter*);
	int (*i2c_stop)(struct adapter*);
	int (*i2c_inb)(struct adapter*, unsigned char*);
	int (*i2c_outb)(struct adapter*, unsigned char);
	int (*i2c_acknak)(struct adapter*, int);
	int (*flush)(struct adapter*);
	int sda_out;
	int sda_in;
	int scl;
	int reset;
	int user;
	int power;
	int i2c_en;
	int interface;
	int speed_hz;
	const char *desc;
};

struct adapter {
	int i2c_error;
	int hw_error;
	struct adapter_ops *ops;
	struct ftdi_context *ftdic;
	int sda_out;
	int sda_in;
	int scl;
	int speed_hz;
	void *priv;
};

struct adapter *adapter_attach(struct ftdi_context *ftdic,
	struct adapter_ops *ops, int speed_hz);
void adapter_detach(struct adapter *adap);

int arg_to_gpio(struct adapter *adap, const char *arg);
int adapter_flush(struct adapter *adap);

int adapter_gpio_output(struct adapter *adap, int gpio);
int adapter_gpio_input(struct adapter *adap, int gpio);
int adapter_gpio_high(struct adapter *adap, int gpio);
int adapter_gpio_low(struct adapter *adap, int gpio);
int adapter_gpio_get(struct adapter *adap, int gpio);

int adapter_hw_error(struct adapter *adap);

int i2c_xfer(struct adapter *adap, struct i2c_msg msgs[], int count);
int i2c_master_send(struct adapter *adap, unsigned char addr, const char *buf,
			int count);
int i2c_master_recv(struct adapter *adap, unsigned char addr, char *buf,
			int count);

int i2c_smbus_xfer(struct adapter *adap, unsigned char addr,
			unsigned short flags, char rdwr,
			unsigned char command, int size,
			union i2c_smbus_data *data);

int i2c_probe_func_quick_read(struct adapter *adap, unsigned short addr);
int i2c_smbus_read_byte(struct adapter *adap, unsigned char addr);
int i2c_smbus_write_byte(struct adapter *adap, unsigned char addr,
			unsigned char value);
int i2c_smbus_read_byte_data(struct adapter *adap, unsigned char addr,
			unsigned char command);
int i2c_smbus_write_byte_data(struct adapter *adap, unsigned char addr,
			unsigned char command, unsigned char value);
int i2c_smbus_read_word_data(struct adapter *adap, unsigned char addr,
			unsigned char command);
int i2c_smbus_write_word_data(struct adapter *adap, unsigned char addr,
			unsigned char command, unsigned short value);
int i2c_smbus_read_block_data(struct adapter *adap, unsigned char addr,
				unsigned char command, unsigned char *values);
int i2c_smbus_write_block_data(struct adapter *adap, unsigned char addr,
				unsigned char command, unsigned char length,
				unsigned char *values);
int i2c_smbus_read_i2c_block_data(struct adapter *adap, unsigned char addr,
				unsigned char command, unsigned char length,
				unsigned char *values);
int i2c_smbus_write_i2c_block_data(struct adapter *adap, unsigned char addr,
				unsigned char command, unsigned char length,
				unsigned char *values);

#endif
