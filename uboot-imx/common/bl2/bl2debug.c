/*
 * Copyright (C) 2007 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <stdio.h>
#include "bl2debug.h"

static int
debug_handler(const char *str, int len)
{
	puts(str);
	return len;
}
