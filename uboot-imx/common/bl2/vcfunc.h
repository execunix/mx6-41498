/*
 * Copyright (C) 2007 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __vcfunc_h__
#define __vcfunc_h__

#include <lcd.h>

#define DISP_BASE	(unsigned int*)DISP_MEM_PHYBASE

#define FONT_W		8
#define FONT_H		16
#define VSCR_W		(FONT_W*80) // 640
#define VSCR_H		(FONT_H*25) // 400
#define VSCR_X		((LCD_W-VSCR_W)/2)
#define VSCR_Y		((LCD_H-VSCR_H)/2)

extern unsigned short rom8x16_bits[];
extern unsigned int back_color;
extern unsigned int text_color;

void vcdelay(int ms);
void set_cursor(int x, int y);
void get_cursor(int* x, int* y);
void draw_colorbar();
void fill_area(int x, int y, unsigned int w, unsigned int h, unsigned int color);
void copy_area(int x, int y, unsigned int w, unsigned int h, int dx, int dy);
int vcprintf(const char* fmt, ...);
int vcputch(int ch);
int vcinit(int on);

#endif//__vcfunc_h__
