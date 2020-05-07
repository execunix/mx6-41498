/* U-Boot BL2
 * Copyright (C) 2019 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
//#include <command.h>
#include <image.h>
#include <mmc.h>
#include "bl2.h"

#if 0
#ifdef CONFIG_MX6ES1MFG
const char *bl2bootargs =
	"console=ttymxc3,921600 rdinit=/linuxrc clk_ignore_unused "
#else
const char *bl2bootargs =
	"console=ttymxc3,921600 root=/dev/mmcblk2p2 rootwait rw "
	"vmalloc=256M video=mxcfb0:dev=ldb,if=RGB24,fbpix=RGB32,bpp=32 ";
#endif
#endif

ulong mmc_bread(struct blk_desc *block_dev, lbaint_t start, lbaint_t blkcnt,
		void *dst);

#ifdef CONFIG_MX6ES1BL2
static struct mmc mmc_static;

struct mmc *find_mmc_device(int dev_num)
{
	return &mmc_static;
}

void mmc_do_preinit(void)
{
	struct mmc *m = &mmc_static;

	mmc_start_init(m);
}

struct blk_desc *mmc_get_blk_desc(struct mmc *mmc)
{
	return &mmc->block_dev;
}

static int cur_dev_num = -1;
#if 0
int get_mmc_num(void)
{
	return cur_dev_num;
}
#endif
void mmc_list_init(void)
{
	cur_dev_num = 0;
}

static struct mmc mmc_static = {
	.dsr_imp		= 0,
	.dsr			= 0xffffffff,
	.block_dev = {
		.if_type	= IF_TYPE_MMC,
		.removable	= 0,
		.devnum		= 1,
		.block_read	= mmc_bread,
		.block_write	= 0/*mmc_bwrite*/,
		.block_erase	= 0/*mmc_berase*/,
		.part_type	= 0,
	},
};

struct mmc *mmc_create(const struct mmc_config *cfg, void *priv)
{
	struct mmc *mmc = &mmc_static;

	mmc->cfg = cfg;
	mmc->priv = priv;

	return mmc;
}

void mmc_destroy(struct mmc *mmc)
{
}

int blk_dselect_hwpart(struct blk_desc *desc, int hwpart)
{
	struct mmc *mmc = &mmc_static;
	int ret;

	if (!mmc)
		return -ENODEV;

	if (mmc->block_dev.hwpart == hwpart)
		return 0;

	if (mmc->part_config == MMCPART_NOAVAILABLE)
		return -EMEDIUMTYPE;

	ret = mmc_switch_part(mmc, hwpart);
	if (ret)
		return ret;

	return 0;
}

void print_mmc_devices(char separator)
{
	struct mmc *m = &mmc_static;

	printf("%s: %d (%s)\n", m->cfg->name, m->block_dev.devnum, "eMMC");
}

#endif

void bl2_mmc_init(bl2_headers_t *bh)
{
	struct mmc *mmc = find_mmc_device(1);
	uint blk, err;
	char *blk_buf;
	char *str;

	// do_mmc_rescan
	//mmc->has_init = 0; // force_init
	if (mmc_init(mmc)) {
		debug("bl2: mmc init failed\n");
		return;
	}

	// switch part
	blk_dselect_hwpart(&mmc->block_dev, MMC_PART_BOOT1);
	debug("bl2: b_max=%d bl_len=%d\n", mmc->cfg->b_max, mmc->read_bl_len);

	// temp read 512 bytes
	blk_buf = (char *)LOAD_ADDR_DTB;

	// read bootcfg
	blk = BLK_START_BOOTCFG;
	memset(blk_buf, 0, sizeof(blk_buf));
	err = mmc->block_dev.block_read(&mmc->block_dev, blk, 1, blk_buf);
	if (err != 1) {
		debug("bl2: %s read failed\n", "bootcfg");
		return;
	}
	debug("bootcfg: %s\n", blk_buf);
	if (NULL != (str = strstr(blk_buf + 9, "avbmeta="))) {
		bh->bootcfg[0] = str[ 8];
		bh->bootcfg[1] = str[10];
		bh->bootcfg[2] = str[12];
		bh->bootcfg[3] = str[14];
	}

	// read bootenv
	blk = BLK_START_BOOTENV;
	memset(blk_buf, 0, sizeof(blk_buf));
	err = mmc->block_dev.block_read(&mmc->block_dev, blk, 1, blk_buf);
	if (err != 1) {
		debug("bl2: %s read failed\n", "bootenv");
		return;
	}
	debug("bootenv: %s\n", blk_buf);

	if (strncmp(blk_buf, "<BOOTENV", 8)) {
		debug("bl2: invalid bootenv\n");
		return;
	}
	bh->dtbsize = 256*1024;
	if (NULL != (str = strstr(blk_buf + 9, "dtbsize=")))
		bh->dtbsize = simple_strtoul(str + 8, NULL, 10);

	bh->zimagesize = 8*1024*1024;
	if (NULL != (str = strstr(blk_buf + 9, "zimagesize=")))
		bh->zimagesize = simple_strtoul(str + 11, NULL, 10);

	bh->bootlogosize = 4*1024*1024;
	if (NULL != (str = strstr(blk_buf + 9, "bootlogosize=")))
		bh->bootlogosize = simple_strtoul(str + 13, NULL, 10);

	//debug("bl2: dtbsize=%d zimagesize=%d bootlogosize=%d\n",
	//      bh->dtbsize, bh->zimagesize, bh->bootlogosize);
}

//
// 1. loadimage
// 2. loadfdt
// 3. bootz
//
void bl2_mmc_boot(bl2_headers_t *bh)
{
	struct mmc *mmc = find_mmc_device(1);
	uint blk_cnt, blk, err;
	void *dst;

	// do_mmc_rescan
	//mmc->has_init = 0; // force_init
	if (mmc_init(mmc)) {
		debug("bl2: mmc init failed\n");
		return;
	}

	// switch part
	blk_dselect_hwpart(&mmc->block_dev, MMC_PART_BOOT1);
	debug("bl2: b_max=%d bl_len=%d\n", mmc->cfg->b_max, mmc->read_bl_len);

	blk = BLK_START_DTB;
	dst = (void *)LOAD_ADDR_DTB;
	debug("bl2: %08lx %s\n", (ulong)dst, "dtb");
	blk_cnt = ALIGN(bh->dtbsize, mmc->read_bl_len) / mmc->read_bl_len;
	err = mmc->block_dev.block_read(&mmc->block_dev, blk, blk_cnt, dst);
	if (err != blk_cnt) {
		debug("bl2: %s read failed\n", "dtb");
		return;
	}
	/* Clean d-cache and invalidate i-cache, to
	 * make sure that no stale data is executed. */
	flush_cache((ulong)dst, roundup(bh->dtbsize, ARCH_DMA_MINALIGN));

	blk = BLK_START_ZIMAGE;
	dst = (void *)LOAD_ADDR_ZIMAGE;
	debug("bl2: %08lx %s\n", (ulong)dst, "zimage");
	blk_cnt = ALIGN(bh->zimagesize, mmc->read_bl_len) / mmc->read_bl_len;
	#if 1
	err = mmc->block_dev.block_read(&mmc->block_dev, blk, blk_cnt, dst);
	if (err != blk_cnt) {
		debug("bl2: %s read failed\n", "zimage");
		return;
	}
	#else
	do {
		#define BURST_CNT 512
		blk = BLK_START_ZIMAGE;
		while (blk_cnt > 0) {
			uint cnt = blk_cnt < BURST_CNT ? blk_cnt : BURST_CNT;
			err = mmc->block_dev.block_read(&mmc->block_dev, blk, cnt, dst);
			debug("bl2: read(%d,%d,%08lx)=%d\n", blk, cnt, (ulong)dst, err);
			if (err != cnt) {
				debug("bl2: %s read failed\n", "zimage");
				return;
			}
			dst += (mmc->read_bl_len*BURST_CNT);
			blk += BURST_CNT;
			blk_cnt -= cnt;
		}
	} while (0);
	#endif
	/* Clean d-cache and invalidate i-cache, to
	 * make sure that no stale data is executed. */
	flush_cache((ulong)dst, roundup(bh->zimagesize, ARCH_DMA_MINALIGN));

	// tbd - load initrd

	#if 0
	// switch part
	blk_dselect_hwpart(&mmc->block_dev, MMC_PART_USER0);
	debug("bl2: b_max=%d bl_len=%d\n", mmc->cfg->b_max, mmc->read_bl_len);
	#endif

	#if 1
	bh->ep_addr = LOAD_ADDR_ZIMAGE;
	bh->rd_addr = 0;//LOAD_ADDR_INITRD;
	bh->fdt_blob = LOAD_ADDR_DTB;
	bl2_boot(bh);
	#else
	do {
		int argc = 4;
		char *argv[] = {
			"bootz",
			__stringify(LOAD_ADDR_ZIMAGE),
			"-",
			__stringify(LOAD_ADDR_DTB)
		};
		env_set("bootargs", bl2bootargs);
		debug("bl2: %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
		do_bootz(find_cmd("do_bootz"), 0/*CMD_FLAG_*/, argc, argv);
	} while (0);
	#endif
}
