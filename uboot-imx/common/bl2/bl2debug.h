/*
 * Copyright (C) 2007 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __fxdebug_h__
#define __fxdebug_h__

#include <stdlib.h>
#include <stdarg.h>

#define debug_printf printf

#define dprint0(...) ((void)0)
#define dprint1(...) debug_printf(__VA_ARGS__)
#ifdef DEBUG
#define dprintf dprint1
#else
#define dprintf dprint0
#endif

#define WCSTR_ARG(contents)     L##contents
#define WCSTR(macro_or_string)  WCSTR_ARG(macro_or_string)
#define __func__                ((const char*)(__FUNCTION__))
#define __funcw__               ((const wchar_t*)WCSTR(__FUNCTION__))

#endif//__fxdebug_h__
