/* U-Boot BL2
 * Copyright (C) 2019 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <mapmem.h>
#include <asm/sections.h>
#include <serial.h>
#include <mmc.h>
#include "bl2.h"

DECLARE_GLOBAL_DATA_PTR;

static int initr_malloc(void)
{
	ulong malloc_start;

#ifdef CONFIG_SYS_MALLOC_F_LEN
	debug("Pre-reloc malloc() used %#lx bytes (%ld KB)\n", gd->malloc_ptr,
	      gd->malloc_ptr / 1024);
#endif
	/* The malloc area is immediately below the monitor copy in DRAM */
	malloc_start = gd->relocaddr - TOTAL_MALLOC_LEN;
	mem_malloc_init((ulong)map_sysmem(malloc_start, TOTAL_MALLOC_LEN),
			TOTAL_MALLOC_LEN);
	return 0;
}

void board_init_r(gd_t *new_gd, ulong dest_addr)
{
	/* tell others: relocation done */
	gd->flags |= GD_FLG_RELOC | GD_FLG_FULL_MALLOC_INIT;

	/* Enable caches */
	enable_caches();

	initr_malloc();

	board_init();	/* Setup chipselects */ // mx6qsabreauto.c

	debug("Now running in RAM - U-Boot at: %08lx\n", gd->relocaddr);

	mxc_serial_initialize();
#if !defined(CONFIG_MX6ES1MFG)
	mmc_initialize(gd->bd);
#endif
	//set_default_env(NULL);
	//jumptable_init();

	interrupt_init();
	enable_interrupts();

	//board_late_init();

	bl2_main();

	/* NOTREACHED - bl2_main() does not return */
	hang();
}
