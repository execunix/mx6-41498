/*
 * Copyright (C) 2007 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <windows.h>
#include <stdarg.h>
#include "vcfunc.h"

//
// virtual screen functions
//

unsigned int back_color = 0;
unsigned int text_color = 0;
static int cur_x = 0;
static int cur_y = 0;
static int vc_on = 0;

void vcdelay(int ms)
{
	volatile int i;
	for (i = 0; i < ms; i++) {
		volatile int n = 1100000/16;
		while (0 < --n);
	}
}

void set_cursor(int x, int y)
{
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x > 79)
		x = 79;
	if (y > 24)
		y = 24;
	cur_x = x;
	cur_y = y;
}

void get_cursor(int* x, int* y)
{
	*x = cur_x;
	*y = cur_y;
}

void draw_colorbar()
{
#if 0
	do { // ram test
		const unsigned int* start = (unsigned int*)0x48000000;
		const unsigned int* end   = (unsigned int*)0x60000000;
		volatile unsigned int* p;
		unsigned int val = 0x55555555;
		KITLOutputDebugString("ram test 0x%x:0x%x\r\n", start, end);
		while (1) {
			for (p = start; p < end; p++) {
				*p = val;
				if (0 == (0xFFFFF&(int)p))
					KITLOutputDebugString("0x%x W\r\n", p);
			}
			for (p = start; p < end; p++) {
				if (*p != val)
					KITLOutputDebugString("ram test error : addr=0x%x val=0x%x\r\n", p, *p);
				if (0 == (0xFFFFF&(int)p))
					KITLOutputDebugString("0x%x R\r\n", p);
			}
			val = ~val;
			if (val == 0x55555555)
				break;
		}
	} while (0);
#endif
	do {
#if 1 // draw color bar
		unsigned int i, j, k = 0;
		unsigned int* p1;
		unsigned int* p2;
		i = 0;
		p1 = DISP_BASE;
		p2 = p1 + (LCD_H/8)*LCD_W;
		for (; i < 1*LCD_H/8; i++) {
			for (j = 0, k = LCD_W - 1; j < LCD_W; j++, k--) {
				*p1++ = ((0x100*k/LCD_W)<<0); // B
				*p2++ = ((0x100*j/LCD_W)<<0); // B
			}
		}
		i = 2*LCD_H/8;
		p1 += (LCD_H/8)*LCD_W;
		p2 += (LCD_H/8)*LCD_W;
		for (; i < 3*LCD_H/8; i++) {
			for (j = 0, k = LCD_W - 1; j < LCD_W; j++, k--) {
				*p1++ = ((0x100*k/LCD_W)<<8); // G
				*p2++ = ((0x100*j/LCD_W)<<8); // G
			}
		}
		i = 4*LCD_H/8;
		p1 += (LCD_H/8)*LCD_W;
		p2 += (LCD_H/8)*LCD_W;
		for (; i < 5*LCD_H/8; i++) {
			for (j = 0, k = LCD_W - 1; j < LCD_W; j++, k--) {
				*p1++ = ((0x100*k/LCD_W)<<16); // R
				*p2++ = ((0x100*j/LCD_W)<<16); // R
			}
		}
		i = 6*LCD_H/8;
		p1 += (LCD_H/8)*LCD_W;
		p2 += (LCD_H/8)*LCD_W;
		for (; i < 7*LCD_H/8; i++) {
			for (j = 0, k = LCD_W - 1; j < LCD_W; j++, k--) {
				unsigned int ck = (0x100*k/LCD_W);
				unsigned int cj = (0x100*j/LCD_W);
				*p1++ = (ck<<16)|(ck<<8)|ck; // Gray
				*p2++ = (cj<<16)|(cj<<8)|cj; // Gray
			}
		}
#else // draw grid point
		unsigned int x, y;
		unsigned int* p;
		for (y = 0; y < LCD_H; y += 2) {
			p = DISP_BASE + LCD_W*y;
			for (x = 0; x < LCD_W; x += 2) {
				*p = 0xffffff;
				p += 2;
			}
		}
#endif
		fill_area(0, LCD_H-8, 8, 8, 0xffffff);
		fill_area(1, LCD_H-7, 6, 6, 0x000000);
		fill_area(2, LCD_H-6, 4, 4, 0xffffff);
		fill_area(LCD_W-8, 0, 8, 8, 0xffffff);
		fill_area(LCD_W-7, 1, 6, 6, 0x000000);
		fill_area(LCD_W-6, 2, 4, 4, 0xffffff);
	} while (0);
}

void fill_area(int x, int y, unsigned int w, unsigned int h, unsigned int color)
{
	unsigned int* dp;
	int l = x;
	int t = y;
	int r = x + w;
	int b = y + h;
	if (l < 0)
		l = 0;
	if (t < 0)
		t = 0;
	if (r > LCD_W)
		r = LCD_W;
	if (b > LCD_H)
		b = LCD_H;
	dp = DISP_BASE + LCD_W*t;
	while (t < b) {
		unsigned int* p = dp + l;
		unsigned int* q = dp + r;
		while (p < q)
			*p++ = color;
		dp += LCD_W;
		t++;
	}
}

void copy_area(int x, int y, unsigned int w, unsigned int h, int dx, int dy)
{
	unsigned int* sp;
	unsigned int* dp;
#if 0 // not req workaround
	if (x < 0) { dx -= x; w += x; x = 0; }
	if (y < 0) { dy -= y; h += y; y = 0; }
	if ((x + w) > /*src*/LCD_W) w = /*src*/LCD_W - x;
	if ((y + h) > /*src*/LCD_H) h = /*src*/LCD_H - y;
	if (dx < 0) { x -= dx; w += dx; dx = 0; }
	if (dy < 0) { y -= dy; h += dy; dy = 0; }
	if ((dx + w) > /*dst*/LCD_W) w = /*dst*/LCD_W - dx;
	if ((dy + h) > /*dst*/LCD_H) h = /*dst*/LCD_H - dy;
	if ((int)w <= 0 || (int)h <= 0) return;
#endif
	int sp_next = /*src*/LCD_W - w;
	int dp_next = /*dst*/LCD_W - w;
	sp = /*src*/DISP_BASE + /*src*/LCD_W * y + x;
	dp = /*dst*/DISP_BASE + /*dst*/LCD_W * dy + dx;
	while (h--) {
		int width = w;
		while (width--) {
			*dp++ = *sp++;
		}
		sp += sp_next;
		dp += dp_next;
	}
}

static void draw_char(int x, int y, int c)
{
	unsigned short* sp = rom8x16_bits + (FONT_H*c);
	unsigned int* dp = DISP_BASE + (LCD_W*y + x);
	for (y = 0; y < FONT_H; y++) {
		unsigned short rom = *sp++;
		rom = rom >> 7;
		for (x = 7; x >= 0; x--) {
			rom = rom >> 1;
			if (rom & 1) {
				dp[x] = text_color;
			} else {
				dp[x] = back_color;
			}
		}
		dp += LCD_W;
	}
}

static void draw_cursor(int bit_not)
{
	if (0 <= cur_x && cur_x < 80 &&
		0 <= cur_y && cur_y < 25) {
		if (bit_not) {
			int x = VSCR_X + FONT_W*cur_x;
			int y = VSCR_Y + FONT_H*cur_y;
			unsigned int* dp = DISP_BASE + LCD_W*y;
			unsigned int h = FONT_H;
			while (h > 0) {
				unsigned int* p = dp + x;
				unsigned int* q = p + FONT_W;
				while (p < q) {
					unsigned int color = *p;
					*p++ = color ^ 0x999999;
				}
				dp += LCD_W;
				h--;
			}
		} else { // erase to back_color
			fill_area(VSCR_X+FONT_W*cur_x, VSCR_Y+FONT_H*cur_y, FONT_W, FONT_H, back_color);
		}
	}
}

static int lcd_out_cnt = 0;

static void lcd_out(int c)
{
	// 80x25 screen 640x400 pixel
	int i;
	lcd_out_cnt++;
	if (c == '\b') {
		if (cur_x > 0) {
			draw_cursor(0);
			cur_x--;
			draw_cursor(1);
		}
		return;
	}
	if (c == '\r') {
		draw_cursor(1);
		cur_x = 0;
		draw_cursor(1);
		return;
	}
	if (c == '\n') {
		draw_cursor(1);
		cur_x = 0;
		if (cur_y < 24) {
			cur_y++;
			draw_cursor(1);
			return;
		}
		copy_area(VSCR_X, VSCR_Y+FONT_H, VSCR_W, FONT_H*24, VSCR_X, VSCR_Y); // scroll
		fill_area(VSCR_X, VSCR_Y+FONT_H*24, VSCR_W, FONT_H, back_color); // erase
		draw_cursor(1);
		return;
	}
	if (c == '\t') {
		int new_x = (cur_x+8)/8*8;
		int space = new_x - cur_x;
		for (i = 0; i < space; i++) {
			lcd_out_cnt--;
			lcd_out(' ');
		}
		return;
	}
	draw_char(VSCR_X+FONT_W*cur_x, VSCR_Y+FONT_H*cur_y, c);
	cur_x++;
	if (cur_x < 80) {
		draw_cursor(1);
	} else {
		lcd_out_cnt--;
		lcd_out('\n');
	}
}

/////////////////////////////////////////////////////////////////////////////
// impl. vcvprintf

static size_t strnlen(const char* str, size_t maxlen)
{
	size_t len = 0;
	while (str[len] && len < maxlen)
		len++;
	return len;
}

#undef isdigit
#define isdigit(d)	((d) >= '0' && (d) <= '9')

static int skip_atoi(const char** s)
{
	int i = 0;
	while (isdigit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SMALL	32		/* Must be 32 == 0x20 */
#define SPECIAL	64		/* 0x */

static void number(unsigned long long num, int base, int size, int precision, int type)
{
	/* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
	static const char digits[] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

	char tmp[66];
	char sign;
	char locase;
	int need_pfx = ((type & SPECIAL) && base != 10);
	int i;

	/* locase = 0 or 0x20. ORing digits or letters with 'locase'
	 * produces same digits or (maybe lowercased) letters */
	locase = (type & SMALL);
	if (type & LEFT)
		type &= ~ZEROPAD;
	sign = 0;
	if (type & SIGN) {
		if ((signed long long)num < 0) {
			sign = '-';
			num = - (signed long long)num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (need_pfx) {
		size--;
		if (base == 16)
			size--;
	}

	/* generate full string in tmp[], in reverse order */
	i = 0;
	if (num == 0) {
		tmp[i++] = '0';
	} else {
		do { /* Generic code, for any base */
			tmp[i++] = (digits[num % base] | locase);
			num = num / base;
		} while (num != 0);
	}

	/* printing 100 using %2d gives "100", not "00" */
	if (i > precision)
		precision = i;
	/* leading space padding */
	size -= precision;
	if (!(type & (ZEROPAD+LEFT))) {
		while (--size >= 0) {
			lcd_out(' ');
		}
	}
	/* sign */
	if (sign) {
		lcd_out(sign);
	}
	/* "0x" / "0" prefix */
	if (need_pfx) {
		lcd_out('0');
		if (base == 16) {
			lcd_out('X' | locase);
		}
	}
	/* zero or space padding */
	if (!(type & LEFT)) {
		char c = (type & ZEROPAD) ? '0' : ' ';
		while (--size >= 0) {
			lcd_out(c);
		}
	}
	/* hmm even more zero padding? */
	while (i <= --precision) {
		lcd_out('0');
	}
	/* actual digits of result */
	while (--i >= 0) {
		lcd_out(tmp[i]);
	}
	/* trailing space padding */
	while (--size >= 0) {
		lcd_out(' ');
	}
}

static void string(char* s, int field_width, int precision, int flags)
{
	int len, i;
	if ((unsigned long)s < 1/*PAGE_SIZE*/)
		s = "<NULL>";
	len = (int)strnlen(s, precision);
	if (!(flags & LEFT)) {
		while (len < field_width--) {
			lcd_out(' ');
		}
	}
	for (i = 0; i < len; ++i) {
		lcd_out(*s++);
	}
	while (len < field_width--) {
		lcd_out(' ');
	}
}

static int vcvprintf(const char* fmt, va_list args)
{
	unsigned long long num;
	int base;
	char c;
	int flags; /* flags to number() */
	int field_width; /* width of output field */
	int precision; /* min. # of digits for integers; max number of chars for from string */
	int qualifier; /* 'h', 'l', or 'L' for integer fields */

	lcd_out_cnt = 0;

	for (; *fmt; ++fmt) {
		if (*fmt != '%') {
			lcd_out(*fmt);
			continue;
		}

		/* process flags */
		flags = 0;
repeat:
		++fmt; /* this also skips first '%' */
		switch (*fmt) {
			case '-': flags |= LEFT; goto repeat;
			case '+': flags |= PLUS; goto repeat;
			case ' ': flags |= SPACE; goto repeat;
			case '#': flags |= SPECIAL; goto repeat;
			case '0': flags |= ZEROPAD; goto repeat;
		}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' ||
			*fmt == 'Z' || *fmt == 'z' || *fmt == 't') {
			qualifier = *fmt;
			++fmt;
			if (qualifier == 'l' && *fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
		}

		/* default base */
		base = 10;

		switch (*fmt) {
			case 'A':
				back_color = (unsigned int)va_arg(args, unsigned int);
				continue;

			case 'C':
				text_color = (unsigned int)va_arg(args, unsigned int);
				continue;

			case 'c':
				if (!(flags & LEFT)) {
					while (--field_width > 0) {
						lcd_out(' ');
					}
				}
				c = (unsigned char)va_arg(args, int);
				lcd_out(c);
				while (--field_width > 0) {
					lcd_out(' ');
				}
				continue;

			case 's':
				string(va_arg(args, char*), field_width, precision, flags);
				continue;

			case 'p':
				field_width = 8;
				precision = -1;
				//flags |= SMALL;
				flags |= ZEROPAD;
				number((unsigned long long)va_arg(args, void*), 16, field_width, precision, flags);
				continue;

			case '%':
				lcd_out('%');
				continue;

				/* integer number formats - set up the flags and "break" */
			case 'o':
				base = 8;
				break;

			case 'B': // wince
				precision = 2;
				base = 16;
				break;

			case 'H': // wince
				precision = 4;
				base = 16;
				break;

			case 'x':
				flags |= SMALL;
			case 'X':
				if (*fmt == 'X')
					precision = 8; // wince
				base = 16;
				break;

			case 'd':
			case 'i':
				flags |= SIGN;
			case 'u':
				break;

			default:
				lcd_out('%');
				if (*fmt) {
					lcd_out(*fmt);
				} else {
					--fmt;
				}
				continue;
		}
		if (qualifier == 'L')
			num = va_arg(args, long long);
		else if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
			if (flags & SIGN)
				num = (signed long)num;
		} else if (qualifier == 'Z' || qualifier == 'z') {
			num = va_arg(args, size_t);
		} else if (qualifier == 't') {
			num = va_arg(args, ptrdiff_t);
		} else if (qualifier == 'h') {
			num = (unsigned short)va_arg(args, int);
			if (flags & SIGN)
				num = (signed short)num;
		} else {
			num = va_arg(args, unsigned int);
			if (flags & SIGN)
				num = (signed int)num;
		}
		number(num, base, field_width, precision, flags);
	}
	return lcd_out_cnt;
}

int vcprintf(const char* fmt, ...)
{
	if (vc_on) {
		int r;
		va_list arg;
		va_start(arg, fmt);
		r = vcvprintf(fmt, arg);
		va_end(arg);
		return r;
	}
	return 0;
}

int vcputch(int ch)
{
	if (vc_on) {
		lcd_out(ch);
		return ch;
	}
	return 0;
}

int vcinit(int on)
{
	vc_on = on; // tbd
	if (vc_on) {
		back_color = 0x00003f;
		text_color = 0x999999;
		cur_x = 0;
		cur_y = 0;
		// 80x25 screen 640x400 pixel
		fill_area(VSCR_X-16, VSCR_Y-12, VSCR_W+32, VSCR_H+24, 0xffffffff);
		fill_area(VSCR_X-12, VSCR_Y- 8, VSCR_W+24, VSCR_H+16, back_color);
		draw_cursor(1);
		//vcprintf("Copyright (C) 2006-2013 JC Hyun Systems, Inc.  All rights reserved.\n");
		//vcprintf("Virtual Screen Start...0x%p\n", DISP_BASE);
	}
	return 0;
}

