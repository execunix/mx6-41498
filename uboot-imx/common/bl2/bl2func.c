/* U-Boot BL2
 * Copyright (C) 2019 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <stdarg.h>
#include <serial.h>

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_CMD_CONSOLE

void serial_register(struct serial_device *dev)
{
	dev->next = NULL;
}

int serial_init(void)
{
	struct serial_device *dev;

	gd->flags |= GD_FLG_SERIAL_READY;
	dev = default_serial_console();
	return dev->start();
}

void serial_setbrg(void)
{
	default_serial_console()->setbrg();
}

void serial_putc(const char c)
{
	default_serial_console()->putc(c);
}

void serial_puts(const char *s)
{
	default_serial_console()->puts(s);
}

void default_serial_puts(const char *s)
{
	struct serial_device *dev = default_serial_console();
	while (*s)
		dev->putc(*s++);
}

int serial_printf(const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CONFIG_SYS_PBSIZE];

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vscnprintf(printbuffer, sizeof(printbuffer), fmt, args);
	va_end(args);

	serial_puts(printbuffer);
	return i;
}

void putc(const char c)
{
	if (gd->have_console)
		serial_putc(c);
}

void puts(const char *s)
{
	if (gd->have_console)
		serial_puts(s);
}

#if 1
__weak int getc(void)
{
	return 'x';
}

__weak int tstc(void)
{
	return '*';
}

__weak int ctrlc(void)
{
	return 0;
}

__weak int had_ctrlc(void)
{
	return 0;
}

__weak void clear_ctrlc(void)
{
}

__weak int disable_ctrlc(int disable)
{
	return 0;
}

__weak int confirm_yesno(void)
{
	return 0;
}
#endif

#endif // !CONFIG_CMD_CONSOLE
