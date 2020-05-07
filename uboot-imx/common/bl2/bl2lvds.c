/* U-Boot BL2
 * Copyright (C) 2019 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <stdarg.h>
#include <mapmem.h>
#include <asm/mach-imx/video.h>
#include <video.h>
#include <video_fb.h>
#include <mmc.h>
#include "bl2.h"

DECLARE_GLOBAL_DATA_PTR;

extern int bl2_load_png(void *dp, int dw, int dh, void *sp);

// video=mxcfb0:dev=ldb,if=RGB24,fbpix=RGB32,bpp=32
struct display_info_t const displays[] = {
	{
		.bus	= -1,
		.addr	= 0,
		.pixfmt	= IPU_PIX_FMT_RGB24,
		.detect	= NULL,
		.enable	= NULL,
		.mode	= {
			.name           = "lvds",
			.refresh        = 60,
			.xres           = 1920,
			.yres           = 720,
			.pixclock       = 11338,
			.left_margin    = 100,
			.right_margin   = 40,
			.upper_margin   = 30,
			.lower_margin   = 3,
			.hsync_len      = 10,
			.vsync_len      = 2,
			.sync           = FB_SYNC_EXT,
			.vmode          = FB_VMODE_NONINTERLACED
		}
	},
};
size_t display_count = ARRAY_SIZE(displays);

static GraphicDevice *gdev;
static void *fbaddr;
static ulong fbsize;
static u32 eorx, fgx, bgx;	/* color pats */
static int do_flush_cache;

static const int video_font_draw_table32[16][4] = {
	{0x00000000, 0x00000000, 0x00000000, 0x00000000},
	{0x00000000, 0x00000000, 0x00000000, 0x00ffffff},
	{0x00000000, 0x00000000, 0x00ffffff, 0x00000000},
	{0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff},
	{0x00000000, 0x00ffffff, 0x00000000, 0x00000000},
	{0x00000000, 0x00ffffff, 0x00000000, 0x00ffffff},
	{0x00000000, 0x00ffffff, 0x00ffffff, 0x00000000},
	{0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff},
	{0x00ffffff, 0x00000000, 0x00000000, 0x00000000},
	{0x00ffffff, 0x00000000, 0x00000000, 0x00ffffff},
	{0x00ffffff, 0x00000000, 0x00ffffff, 0x00000000},
	{0x00ffffff, 0x00000000, 0x00ffffff, 0x00ffffff},
	{0x00ffffff, 0x00ffffff, 0x00000000, 0x00000000},
	{0x00ffffff, 0x00ffffff, 0x00000000, 0x00ffffff},
	{0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00000000},
	{0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff}
};

static void memsetl(int *p, int c, int v)
{
	while (c--)
		*(p++) = v;
}

static void memcpyl(int *d, int *s, int c)
{
	while (c--)
		*(d++) = *(s++);
}

int load_bootlogo(bl2_headers_t *bh, void* fbaddr)
{
	struct mmc *mmc = find_mmc_device(1);
	uint blk_cnt, blk, err;
	void *dst;

	// do_mmc_rescan
	//mmc->has_init = 0; // force_init
	if (mmc_init(mmc)) {
		debug("bl2: mmc init failed\n");
		return -1;
	}

	// switch part
	blk_dselect_hwpart(&mmc->block_dev, MMC_PART_BOOT1);
	debug("bl2: b_max=%d bl_len=%d\n", mmc->cfg->b_max, mmc->read_bl_len);

	blk = BLK_START_BOOTLOGO;
	dst = (void *)LOAD_ADDR_BOOTLOGO;
	debug("bl2: %08lx %s\n", (ulong)dst, "bootlogo");
	blk_cnt = ALIGN(bh->bootlogosize, mmc->read_bl_len) / mmc->read_bl_len;
	err = mmc->block_dev.block_read(&mmc->block_dev, blk, blk_cnt, dst);
	if (err != blk_cnt) {
		debug("bl2: %s read failed\n", "bootlogo");
		return -1;
	}

	if (bl2_load_png(fbaddr,
			 displays[0].mode.xres,
			 displays[0].mode.yres,
			 dst) == 0) {
		uint size = displays[0].mode.xres * displays[0].mode.yres* 4;
		flush_cache((ulong)fbaddr, roundup(size, ARCH_DMA_MINALIGN));
		return 0;
	}

	return -1;
}

int bl2_video_init(void)
{
	ipuv3_fb_init(&displays[0].mode, displays[0].di ? 1 : 0, displays[0].pixfmt);
	if (displays[0].enable)
		displays[0].enable(&displays[0]);

	printf("Display: %s (%ux%u)\n",
	       displays[0].mode.name,
	       displays[0].mode.xres,
	       displays[0].mode.yres);

	gdev = video_hw_init();
	if (gdev == NULL)
		return -1;

	fbsize = gdev->winSizeY * gdev->winSizeX * gdev->gdfBytesPP;
	fbaddr = (void *)gdev->frameAdrs;
	do_flush_cache = dcache_status();

	bgx = 0x33333333;
	fgx = 0x000000cc;
	eorx = fgx ^ bgx;

	//memsetl(fbaddr, fbsize / sizeof(int), fgx);

	if (do_flush_cache)
		flush_cache((ulong)fbaddr, fbsize);

	return 0;
}
