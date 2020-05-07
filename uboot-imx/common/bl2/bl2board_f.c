/* U-Boot BL2
 * Copyright (C) 2019 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <mapmem.h>
#include <version.h>
#include <asm/sections.h>

DECLARE_GLOBAL_DATA_PTR;

__weak void coloured_LED_init(void) {}
__weak void red_led_on(void) {}
__weak void red_led_off(void) {}
__weak void green_led_on(void) {}
__weak void green_led_off(void) {}
__weak void yellow_led_on(void) {}
__weak void yellow_led_off(void) {}
__weak void blue_led_on(void) {}
__weak void blue_led_off(void) {}

void board_init_f(ulong boot_flags)
{
	gd->flags = boot_flags;
	gd->have_console = 0;

	gd->mon_len = (ulong)&__bss_end - (ulong)_start;

	assert(gd->malloc_base);	/* Set up by crt0.S */
	gd->malloc_limit = CONFIG_SYS_MALLOC_F_LEN;
	gd->malloc_ptr = 0;

	arch_cpu_init();
	board_early_init_f();
	timer_init();
	board_postclk_init();
	get_clocks();

	gd->env_addr	= 0;
	gd->env_valid	= 0;

	gd->baudrate = CONFIG_BAUDRATE;
	serial_init();
	gd->have_console = 1;

	puts("\nU-Boot 4.14.98 - " PLAIN_VERSION
	     " (" U_BOOT_DATE " - " U_BOOT_TIME ")\n");
	debug("U-Boot code: %08lX -> %08lX  BSS: -> %08lX\n",
	      (ulong)CONFIG_SYS_TEXT_BASE, (ulong)&__bss_start, (ulong)&__bss_end);

	print_cpuinfo();
	//checkboard();

	dram_init();

	debug("Monitor len: %08lX\n", gd->mon_len);
	debug("Ram size   : %08lX\n", (ulong)gd->ram_size);
	gd->ram_top = CONFIG_SYS_SDRAM_BASE;
	gd->ram_top += gd->ram_size;
	gd->relocaddr = gd->ram_top;
	debug("Ram top    : %08lX\n", gd->ram_top);
	// reserve_round_4k
	gd->relocaddr &= ~(4096 - 1);

	/* reserve TLB table */
	gd->arch.tlb_size = PGTABLE_SIZE;
	gd->relocaddr -= gd->arch.tlb_size;

	/* round down to next 64 kB limit */
	gd->relocaddr &= ~(0x10000 - 1);

	gd->arch.tlb_addr = gd->relocaddr;
	debug("TLB table from %08lx to %08lx\n", gd->arch.tlb_addr,
	      gd->arch.tlb_addr + gd->arch.tlb_size);

	// reserve_video - skip

	/*
	 * reserve memory for U-Boot code, data & bss
	 * round down to next 4 kB limit
	 */
	gd->relocaddr -= gd->mon_len;
	gd->relocaddr &= ~(4096 - 1);
	debug("Reserving %ldk for U-Boot at: %08lx\n", gd->mon_len >> 10,
	      gd->relocaddr);
	gd->start_addr_sp = gd->relocaddr;

	/* reserve memory for malloc() area */
	gd->start_addr_sp = gd->start_addr_sp - TOTAL_MALLOC_LEN;
	debug("Reserving %dk for malloc() at: %08lx\n",
	      TOTAL_MALLOC_LEN >> 10, gd->start_addr_sp);

	/* (permanently) allocate a Board Info struct */
	if (!gd->bd) {
		gd->start_addr_sp -= sizeof(bd_t);
		gd->bd = (bd_t *)map_sysmem(gd->start_addr_sp, sizeof(bd_t));
		memset(gd->bd, '\0', sizeof(bd_t));
		debug("Reserving %zu Bytes for Board Info at: %08lx\n",
		      sizeof(bd_t), gd->start_addr_sp);
	}
	gd->bd->bi_arch_number = CONFIG_MACH_TYPE; /* board id for Linux */
	gd->start_addr_sp -= sizeof(gd_t);
	gd->new_gd = (gd_t *)map_sysmem(gd->start_addr_sp, sizeof(gd_t));
	debug("Reserving %zu Bytes for Global Data at: %08lx\n",
	      sizeof(gd_t), gd->start_addr_sp);

	// tbd - reserve_fdt

	/* make stack pointer 16-byte aligned */
	gd->start_addr_sp -= 16;
	gd->start_addr_sp &= ~0xf;
	/* setup stack pointer for exceptions */
	gd->irq_sp = gd->start_addr_sp;
	/* leave 3 words for abort-stack, plus 1 for alignment */
	gd->start_addr_sp -= 16;

	/* Ram is board specific, so move it to board code ... */
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = get_effective_memsize();

	do {
		unsigned long long size;
		#ifdef CONFIG_NR_DRAM_BANKS
		int i;
		debug("RAM Configuration:\n");
		for (i = size = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
			size += gd->bd->bi_dram[i].size;
			debug("Bank #%d: %llx ", i,
			      (unsigned long long)(gd->bd->bi_dram[i].start));
			#ifdef DEBUG
			print_size(gd->bd->bi_dram[i].size, "\n");
			#endif
		}
		debug("DRAM:  ");
		#else
		size = gd->ram_size;
		#endif
		print_size(size, "\n");
	} while (0);

	gd->reloc_off = gd->relocaddr - CONFIG_SYS_TEXT_BASE;
	memcpy(gd->new_gd, (char *)gd, sizeof(gd_t));
	debug("Relocation Offset is: %08lx\n", gd->reloc_off);
	debug("Relocating to %08lx, new gd at %08lx, sp at %08lx\n",
	      gd->relocaddr, (ulong)map_to_sysmem(gd->new_gd),
	      gd->start_addr_sp);
}
