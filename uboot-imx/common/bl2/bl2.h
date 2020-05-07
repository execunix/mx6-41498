/* U-Boot BL2
 * Copyright (C) 2019 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _BL2_H_
#define _BL2_H_

#include <linux/sizes.h>
#include <linux/compiler.h>
#include <lmb.h>

#define MMC_PART_USER0          0
#define MMC_PART_BOOT1          1
#define MMC_PART_BOOT2          2
#define MMC_PART_RPMB3          3

#define BLK_START_BOOTCFG       1792 // 64k
#define BLK_START_BOOTENV       1920 // 64k
#define BLK_START_DTB           2048 // 256k
#define BLK_START_ZIMAGE        2560 // 8M
#define BLK_START_BOOTLOGO      18944 // 4M
#define BLK_START_INITRD        2048 // BOOT2 16M - tbd

#define LOAD_ADDR_DTB           0x18000000
#define LOAD_ADDR_INITRD        0x12c00000
#define LOAD_ADDR_ZIMAGE        0x12000000
#define LOAD_ADDR_BOOTLOGO      0x1e800000

#define BL2_FB_ADDR             0x1f000000

typedef struct bl2_headers {
	ulong ep_addr;
	ulong rd_addr;
	ulong fdt_blob;
	ulong zi_start;
	ulong zi_end;
	ulong rd_start;
	ulong rd_end;
	ulong ft_addr;
	ulong ft_len;
	char bootcfg[8];
	uint dtbsize;
	uint zimagesize;
	uint bootlogosize;
} bl2_headers_t;

extern bl2_headers_t bl2hdr;

extern int load_bootlogo(bl2_headers_t *bh, void* fbaddr);
extern void bl2_mmc_init(bl2_headers_t *bh);
extern void bl2_mmc_boot(bl2_headers_t *bh);
extern void bl2_mfg_boot(bl2_headers_t *bh);
extern void bl2_boot(bl2_headers_t *bh);
extern void bl2_main(void);

extern int bl2_video_init(void);

#endif /* _BL2_H_ */
