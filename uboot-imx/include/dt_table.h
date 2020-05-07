/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _DT_TABLE_H_
#define _DT_TABLE_H_

#include <linux/types.h>

/* https://android.googlesource.com/platform/system/libufdt/+/master/utils/README.md */

#define DT_TABLE_MAGIC	0xd7b7ab1e

struct dt_table_header {
	__be32 magic;
	__be32 total_size;
	__be32 header_size;
	__be32 dt_entry_size;
	__be32 dt_entry_count;
	__be32 dt_entries_offset;
	__be32 page_size;
	__be32 version;
};

struct dt_table_entry {
	__be32 dt_size;
	__be32 dt_offset;
	__be32 id;
	__be32 rev;
	__be32 custom[4];
};
#endif /* _DT_TABLE_H_ */
