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

#include <stdio.h>
#include <ftdi.h>
#include <string.h>

#include "i2c.h"

#define ADDR 0x45

extern struct adapter_ops v1_ops;
extern struct adapter_ops v2_ops;

static void print_usage(const char *argv0)
{
	printf(
"Read power settings: %s [<devaddr>]\n"
"Set/clear gpio:      %s [<devaddr>] <set|clear> <u[ser]|p[ower]|r[eset]>\n"
"\n",
		argv0, argv0);
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	int ret;
	struct ftdi_context ftdic;
	struct adapter *adap;
	struct adapter_ops *ops;

	ops = &v2_ops;

	if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
		print_usage(argv[0]);

	if (ftdi_init(&ftdic) < 0) {
		fprintf(stderr, "ftdi_init failed\n");
		return EXIT_FAILURE;
	}

	ftdi_set_interface(&ftdic, ops->interface);

	if (argc > 1 && strchr(argv[1],':')) {
		ret = ftdi_usb_open_string(&ftdic, argv[1]);
		argv++;
		argc--;
	} else
		ret = ftdi_usb_open_desc(&ftdic, 0x0403, 0x6010, ops->desc, NULL);

	if (ret < 0) {
		fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(&ftdic));
		return EXIT_FAILURE;
	}

	adap = adapter_attach(&ftdic, ops, 0);
	if (!adap) {
		perror("attach");
		return EXIT_FAILURE;
	}

	if (argc < 2) {
		short shunt_v;
		int shunt_v_sh;
		unsigned int bus_v;
		double shunt;
		double bus;
		double r = 0.050;
		double current;
		double power;
		unsigned char cmdbuf[] = {1, 2};
		unsigned char msgbuf[2][2];
		struct i2c_msg msgs[] = {
			{ .addr = ADDR, .len = 1, .buf = cmdbuf },
			{ .addr = ADDR, .flags = I2C_M_RD, .len = 2, .buf = msgbuf[0] },
			{ .addr = ADDR, .len = 1, .buf = cmdbuf + 1 },
			{ .addr = ADDR, .flags = I2C_M_RD, .len = 2, .buf = msgbuf[1] },
		};

again:
		ret = i2c_xfer(adap, msgs, 4);
		if (ret < 0) {
			fprintf(stderr, "i2c_xfer error: %d\n", ret);
			goto out;
		}

		shunt_v_sh = msgbuf[0][1] | (msgbuf[0][0] << 8);
		bus_v = msgbuf[1][1] | (msgbuf[1][0] << 8);

		shunt_v = shunt_v_sh;

		if (!((bus_v >> 1) & 1))
			/* Hack for v1 communication failures */
			goto again;

		if (bus_v & 1) {
			fprintf(stderr, "Overflow\n");
			goto out;
		}

		shunt = shunt_v / 100000.0;
		bus = (bus_v >> 3) * 4.0 / 1000.0;
		printf("shunt %fV, bus %fV\n", shunt, bus);
		current = shunt / r;
		power = current * bus;
		printf("%fV * %fmA = %fmW\n", bus, current * 1000, power * 1000);

	} else if (!strcmp(argv[1], "set")) {
		adapter_gpio_high(adap, arg_to_gpio(adap, argv[2]));
		adapter_flush(adap);
	} else if (!strcmp(argv[1], "clear")) {
		adapter_gpio_low(adap, arg_to_gpio(adap, argv[2]));
		adapter_flush(adap);
	} else {
		char *end;
		unsigned char reg;
		unsigned short data;
		reg = strtoul(argv[1], &end, 0);
		if (argc < 3) {
			data = i2c_smbus_read_word_data(adap, ADDR, reg);
			printf("hw_error: %d, 0x%04x\n", adapter_hw_error(adap), data);
		} else {
			unsigned short data;
			data = strtoul(argv[2], &end, 0);
			ret = i2c_smbus_write_word_data(adap, ADDR, reg, data);
			printf("hw_error: %d, 0x%04x\n", adapter_hw_error(adap), ret);
		}
	}

out:
	adapter_detach(adap);
	if ((ret = ftdi_usb_close(&ftdic)) < 0) {
		fprintf(stderr, "unable to close ftdi device: %d (%s)\n", ret, ftdi_get_error_string(&ftdic));
		return EXIT_FAILURE;
	}

	ftdi_deinit(&ftdic);

	return EXIT_SUCCESS;
}
