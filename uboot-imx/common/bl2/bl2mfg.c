/* U-Boot BL2
 * Copyright (C) 2019 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include "bl2.h"

void bl2_mfg_boot(bl2_headers_t *bh)
{
	bh->ep_addr = LOAD_ADDR_ZIMAGE;
	bh->rd_addr = LOAD_ADDR_INITRD;
	bh->fdt_blob = LOAD_ADDR_DTB;
	bl2_boot(bh);
}

