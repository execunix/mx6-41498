/* U-Boot BL2
 * Copyright (C) 2019 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <stdarg.h>
#include <mapmem.h>
#include <linux/libfdt.h>
#include <bootm.h>
#include "bl2.h"

DECLARE_GLOBAL_DATA_PTR;

#define IH_INITRD_ARCH IH_ARCH_DEFAULT

bl2_headers_t bl2hdr;

static const image_header_t *image_get_ramdisk(ulong rd_addr, uint8_t arch,
						int verify)
{
	const image_header_t *rd_hdr = (const image_header_t *)rd_addr;

	if (!image_check_magic(rd_hdr)) {
		puts("Bad Magic Number\n");
		return NULL;
	}

	if (!image_check_hcrc(rd_hdr)) {
		puts("Bad Header Checksum\n");
		return NULL;
	}

	image_print_contents(rd_hdr);

	if (verify) {
		puts("   Verifying Checksum ... ");
		if (!image_check_dcrc(rd_hdr)) {
			puts("Bad Data CRC\n");
			return NULL;
		}
		puts("OK\n");
	}

	if (!image_check_os(rd_hdr, IH_OS_LINUX) ||
	    !image_check_arch(rd_hdr, arch) ||
	    !image_check_type(rd_hdr, IH_TYPE_RAMDISK)) {
		printf("No Linux %s Ramdisk Image\n", genimg_get_arch_name(arch));
		return NULL;
	}

	return rd_hdr;
}

static int bl2_get_ramdisk(ulong rd_addr, uint8_t arch, ulong *rd_start, ulong *rd_end)
{
	ulong rd_data, rd_len;
	const image_header_t *rd_hdr;
	//void *buf;

	*rd_start = 0;
	*rd_end = 0;

	debug("*  ramdisk: image address = 0x%08lx\n", rd_addr);
	if (rd_addr ==  0) {
		debug("## Skipping init Ramdisk\n");
		return 0;
	}

	/*
	 * Check if there is an initrd image at the
	 * address provided in the second bootm argument
	 * check image type, for FIT images get FIT node.
	 */
	//buf = map_sysmem(rd_addr, 0);
	printf("## Loading init Ramdisk from Legacy Image at %08lx ...\n", rd_addr);

	rd_hdr = image_get_ramdisk(rd_addr, arch, 1/*verify*/);
	if (rd_hdr == NULL) {
		return 1;
	}
	rd_data = image_get_data(rd_hdr);
	rd_len = image_get_data_size(rd_hdr);
	//ulong rd_load = image_get_load(rd_hdr);

	if (!rd_data) {
		debug("## No init Ramdisk\n");
	} else {
		*rd_start = rd_data;
		*rd_end = rd_data + rd_len;
	}
	debug("   ramdisk start = 0x%08lx, ramdisk end = 0x%08lx\n",
			*rd_start, *rd_end);

	return 0;
}

static void bl2_jump_linux(bl2_headers_t *bh)
{
	unsigned long machid = gd->bd->bi_arch_number;
	void (*kernel_entry)(int zero, int arch, uint params);
	unsigned long r2;

	kernel_entry = (void (*)(int, int, uint))LOAD_ADDR_ZIMAGE;

	debug("## Transferring control to Linux (at address %08lx)" \
		"...\n", (ulong) kernel_entry);
	printf("\nStarting kernel ...\n\n");

#ifdef CONFIG_USB_DEVICE
	udc_disconnect();
#endif

	cleanup_before_linux();

	if (bh->ft_len)
		r2 = bh->ft_addr;
	else
		r2 = gd->bd->bi_boot_params;

	kernel_entry(0, machid, r2);
}

void bl2_boot(bl2_headers_t *bh)
{
	int ret;

	bootz_setup(bh->ep_addr, &bh->zi_start, &bh->zi_end);

	// bootm_find_images
	ret = bl2_get_ramdisk(bh->rd_addr, IH_INITRD_ARCH, &bh->rd_start, &bh->rd_end);
	if (ret) {
		puts("Ramdisk image is corrupt or invalid\n");
	} else if (bh->rd_addr) {
		// replace fdt_initrd
		printf("   check device-tree {chosen} node...\n"
		       "      linux,initrd-start = <0x%08lx>\n"
		       "      linux,initrd-end = <0x%08lx>\n",
		       bh->rd_start, bh->rd_end);
	}
	// replace boot_get_fdt
	bh->ft_addr = bh->fdt_blob;
	bh->ft_len = fdt_totalsize(bh->fdt_blob);
	printf("## Flattened Device Tree blob at %08lx size %08lx\n",
	       bh->ft_addr, bh->ft_len);

	/*
	 * We are doing the BOOTM_STATE_LOADOS state ourselves, so must
	 * disable interrupts ourselves
	 */
	disable_interrupts();

	arch_preboot_os();
	bl2_jump_linux(bh);

	debug("\n## boot fail...\n");
}

void bl2_main(void)
{
	memset(&bl2hdr, 0, sizeof(bl2hdr));
#if defined(CONFIG_MX6ES1MFG) && defined(is_boot_from_usb)
	if (is_boot_from_usb()) {
		bl2_mfg_boot(&bl2hdr);
	}
#else
	bl2_mmc_init(&bl2hdr);
#if defined(CONFIG_VIDEO)
	bl2_video_init();
#endif
	bl2_mmc_boot(&bl2hdr);
#endif
}
