/*
 * Copyright (C) 2007 C.H Park <execunix@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include "pngpriv.h"

static void pr_error(png_structp png_ptr, png_const_charp error_message)
{
	printf("png ERR: %s\n", error_message);
}

static void pr_warn(png_structp png_ptr, png_const_charp error_message)
{
	printf("png WRN: %s\n", error_message);
}

static void mem_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
	memcpy(data, png_ptr->io_ptr, length);
	png_ptr->io_ptr += length;
}

int bl2_load_png(void *dp, int dw, int dh, void *sp)
{
	int ret = -1;
	png_structp png_ptr;
	png_infop info_ptr;
	png_byte** ptrs = NULL;
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	int interlace_type;
	int compression_type;
	int filter_type;
	png_uint_32 rowbytes;
	png_uint_32 channels;
	unsigned char hdr[8];

	memcpy(hdr, sp, 8);
	if (!(hdr[0] == 0x89 && hdr[1] == 'P' && hdr[2] == 'N' && hdr[3] == 'G')) {
		printf("%s: invalid png format.\n", __func__);
		goto done;
	}
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, pr_error, pr_warn);
	if (!png_ptr) {
		printf("%s: png_create_read_struct fail.\n", __func__);
		goto done;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		printf("%s: png_create_info_struct fail.\n", __func__);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		goto done;
	}

	png_set_read_fn(png_ptr, (png_voidp)sp, mem_read);
	png_set_sig_bytes(png_ptr, 0);

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_type);
	printf("%s: width=%d height=%d bit_depth=%d color_type=%d "
	       "interlace_type=%d compression_type=%d filter_type=%d\n",
	       __func__, width, height, bit_depth, color_type,
	       interlace_type, compression_type, filter_type);

	/* convert palette/gray image to rgb */
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	//	png_set_expand(png_ptr);
	}
	/* expand gray bit depth if needed */
	if (color_type == PNG_COLOR_TYPE_GRAY) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}
	/* transform transparency to alpha */
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
	//	png_set_expand(png_ptr);
	}
	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}
	if (bit_depth < 8) {
		png_set_packing(png_ptr);
	//	png_set_expand(png_ptr);
	}
	/* convert grayscale to RGB */
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
#if 0
	} else
	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
		png_set_bgr(png_ptr);
#endif
	}
	if (interlace_type != PNG_INTERLACE_NONE) {
		png_set_interlace_handling(png_ptr);
	}

	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
#if 0
	double image_gamma;
	if (png_get_gAMA(png_ptr, info_ptr, &image_gamma)) {
		double screen_gamma = 1.0; // 2.2: PC bright, 2.0: PC dark, 1.0 Mac
		png_set_gamma(png_ptr, screen_gamma, image_gamma);
	}
#endif
	/* recheck header after setting EXPAND options */
	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_type);
	if (width != dw || height != dh || bit_depth != 8) {
		png_error(png_ptr, "png bpp error");
	}
	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
		png_set_bgr(png_ptr);
	} else {
		png_error(png_ptr, "png rgb error");
	}

	rowbytes = png_get_rowbytes(png_ptr, info_ptr); // width * channels
	channels = png_get_channels(png_ptr, info_ptr); // bpp = channels*8
	printf("%s: rowbytes=%d, channels=%d\n", __func__, rowbytes, channels);

	ptrs = (png_byte**)malloc(dh*sizeof(png_byte*));
	if (ptrs == NULL) {
		png_error(png_ptr, "malloc ptrs fail");
	}
	for (int h = 0; h < dh; h++) {
		ptrs[h] = (png_byte*)dp + dw*h*sizeof(int);
	}
	png_read_image(png_ptr, ptrs);
	png_read_end(png_ptr, NULL);

	ret = 0;
//fail:
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
done:
	if (ptrs)
		free(ptrs);

	return ret;
}
