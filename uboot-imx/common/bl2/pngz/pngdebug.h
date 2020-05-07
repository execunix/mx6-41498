
/* pngdebug.h - Debugging macros for libpng, also used in pngtest.c
 *
 * Copyright (c) 1998-2011 Glenn Randers-Pehrson
 * (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
 * (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 *
 * Last changed in libpng 1.5.0 [January 6, 2011]
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h
 */

/* Define PNG_DEBUG at compile time for debugging information.  Higher
 * numbers for PNG_DEBUG mean more debugging information.  This has
 * only been added since version 0.95 so it is not implemented throughout
 * libpng yet, but more support will be added as needed.
 *
 * png_debug[1-2]?(level, message ,arg{0-2})
 *   Expands to a statement (either a simple expression or a compound
 *   do..while(0) statement) that outputs a message with parameter
 *   substitution if PNG_DEBUG is defined to 2 or more.  If PNG_DEBUG
 *   is undefined, 0 or 1 every png_debug expands to a simple expression
 *   (actually ((void)0)).
 *
 *   level: level of detail of message, starting at 0.  A level 'n'
 *          message is preceded by 'n' tab characters (not implemented
 *          on Microsoft compilers unless PNG_DEBUG_FILE is also
 *          defined, to allow debug DLL compilation with no standard IO).
 *   message: a printf(3) style text string.  A trailing '\n' is added
 *            to the message.
 *   arg: 0 to 2 arguments for printf(3) style substitution in message.
 */
#ifndef PNGDEBUG_H
#define PNGDEBUG_H

/* These settings control the formatting of messages in png.c and pngerror.c */
/* Moved to pngdebug.h at 1.5.0 */
#  ifndef PNG_LITERAL_SHARP
#    define PNG_LITERAL_SHARP 0x23
#  endif
#  ifndef PNG_LITERAL_LEFT_SQUARE_BRACKET
#    define PNG_LITERAL_LEFT_SQUARE_BRACKET 0x5b
#  endif
#  ifndef PNG_LITERAL_RIGHT_SQUARE_BRACKET
#    define PNG_LITERAL_RIGHT_SQUARE_BRACKET 0x5d
#  endif
#  ifndef PNG_STRING_NEWLINE
#    define PNG_STRING_NEWLINE "\n"
#  endif

#if defined(PNG_DEBUG) && (PNG_DEBUG > 0)
#define png_debug(l,m)  printf("png_debug_%d: " m "\n", l)
#define png_debug1(l,m,p1)  printf("png_debug_%d: " m "\n", l, p1)
#define png_debug2(l,m,p1,p2)  printf("png_debug_%d: " m "\n", l, p1, p2)
#else
#define png_debug(l,m)  ((void)0)
#define png_debug1(l,m,p1)  ((void)0)
#define png_debug2(l,m,p1,p2)  ((void)0)
#endif

#endif /* PNGDEBUG_H */
