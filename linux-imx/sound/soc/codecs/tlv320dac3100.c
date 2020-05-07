/*
 * ALSA SoC TLV320DAC3100 codec driver
 *
 * Copyright (C) 2014 Texas Instruments, Inc.
 *
 * Author: Jyri Sarha <jsarha@ti.com>
 *
 * Based on ground work by: Ajit Kulkarni <x0175765@ti.com>
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED AS IS AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * The TLV320AIC31xx series of audio codec is a low-power, highly integrated
 * high performance codec which provides a stereo DAC, a mono ADC,
 * and mono/stereo Class-D speaker driver.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "tlv320dac3100.h"

static const struct reg_default dac3100_reg_defaults[] = {
	{ DAC3100_CLKMUX, 0x00 },
	{ DAC3100_PLLPR, 0x11 },
	{ DAC3100_PLLJ, 0x04 },
	{ DAC3100_PLLDMSB, 0x00 },
	{ DAC3100_PLLDLSB, 0x00 },
	{ DAC3100_NDAC, 0x01 },
	{ DAC3100_MDAC, 0x01 },
	{ DAC3100_DOSRMSB, 0x00 },
	{ DAC3100_DOSRLSB, 0x80 },
	{ DAC3100_IFACE1, 0x00 },
	{ DAC3100_DATA_OFFSET, 0x00 },
	{ DAC3100_IFACE2, 0x00 },
	{ DAC3100_BCLKN, 0x01 },
	{ DAC3100_DACSETUP, 0x14 },
	{ DAC3100_DACMUTE, 0x0c },
	{ DAC3100_LDACVOL, 0x00 },
	{ DAC3100_RDACVOL, 0x00 },
	{ DAC3100_HPDRIVER, 0x04 },
	{ DAC3100_SPKAMP, 0x06 },
	{ DAC3100_DACMIXERROUTE, 0x00 },
	{ DAC3100_LANALOGHPL, 0x7f },
	{ DAC3100_RANALOGHPR, 0x7f },
	{ DAC3100_LANALOGSPL, 0x7f },
	{ DAC3100_HPLGAIN, 0x02 },
	{ DAC3100_HPRGAIN, 0x02 },
	{ DAC3100_SPLGAIN, 0x00 },
	{ DAC3100_MICBIAS, 0x00 },
};

static bool dac3100_volatile(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case DAC3100_PAGECTL: /* regmap implementation requires this */
	case DAC3100_RESET: /* always clears after write */
	case DAC3100_OT_FLAG:
	case DAC3100_DACFLAG1:
	case DAC3100_DACFLAG2:
	case DAC3100_OFFLAG: /* Sticky interrupt flags */
	case DAC3100_INTRDACFLAG: /* Sticky interrupt flags */
	case DAC3100_INTRADCFLAG: /* Sticky interrupt flags */
	case DAC3100_INTRDACFLAG2:
	case DAC3100_INTRADCFLAG2:
		return true;
	}
	return false;
}

static bool dac3100_writeable(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case DAC3100_OT_FLAG:
	case DAC3100_DACFLAG1:
	case DAC3100_DACFLAG2:
	case DAC3100_OFFLAG: /* Sticky interrupt flags */
	case DAC3100_INTRDACFLAG: /* Sticky interrupt flags */
	case DAC3100_INTRADCFLAG: /* Sticky interrupt flags */
	case DAC3100_INTRDACFLAG2:
	case DAC3100_INTRADCFLAG2:
		return false;
	}
	return true;
}

static const struct regmap_range_cfg dac3100_ranges[] = {
	{
		.range_min = 0,
		.range_max = 12 * 128,
		.selector_reg = DAC3100_PAGECTL,
		.selector_mask = 0xff,
		.selector_shift = 0,
		.window_start = 0,
		.window_len = 128,
	},
};

static const struct regmap_config dac3100_i2c_regmap = {
	.reg_bits = 8,
	.val_bits = 8,
	.writeable_reg = dac3100_writeable,
	.volatile_reg = dac3100_volatile,
	.reg_defaults = dac3100_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(dac3100_reg_defaults),
	.cache_type = REGCACHE_RBTREE,
	.ranges = dac3100_ranges,
	.num_ranges = ARRAY_SIZE(dac3100_ranges),
	.max_register = 12 * 128,
};

struct dac3100_priv {
	struct snd_soc_codec *codec;
	u8 i2c_regs_status;
	struct device *dev;
	struct regmap *regmap;
	struct dac3100_pdata pdata;
	unsigned int sysclk;
	u8 p_div;
	int rate_div_line;
};

struct dac3100_rate_divs {
	u32 mclk_p;
	u32 rate;
	u8 pll_j;
	u16 pll_d;
	u16 dosr;
	u8 ndac;
	u8 mdac;
	u8 aosr;
	u8 nadc;
	u8 madc;
};

/* ADC dividers can be disabled by cofiguring them to 0 */
static const struct dac3100_rate_divs dac3100_divs[] = {
	/* mclk/p    rate  pll: j     d        dosr ndac mdac  aors nadc madc */
	/* 8k rate */
	{12000000,   8000,	8, 1920,	128,  48,  2,	128,  48,  2},
	{12000000,   8000,	8, 1920,	128,  32,  3,	128,  32,  3},
	/* 11.025k rate */
	{12000000,  11025,	7, 5264,	128,  32,  2,	128,  32,  2},
	{12000000,  11025,	8, 4672,	128,  24,  3,	128,  24,  3},
	/* 16k rate */
	{12000000,  16000,	8, 1920,	128,  24,  2,	128,  24,  2},
	{12000000,  16000,	8, 1920,	128,  16,  3,	128,  16,  3},
	/* 22.05k rate */
	{12000000,  22050,	7, 5264,	128,  16,  2,	128,  16,  2},
	{12000000,  22050,	8, 4672,	128,  12,  3,	128,  12,  3},
	/* 32k rate */
	{12000000,  32000,	8, 1920,	128,  12,  2,	128,  12,  2},
	{12000000,  32000,	8, 1920,	128,   8,  3,	128,   8,  3},
	/* 44.1k rate */
	{12000000,  44100,	7, 5264,	128,   8,  2,	128,   8,  2},
	{12000000,  44100,	8, 4672,	128,   6,  3,	128,   6,  3},
	/* 48k rate */
	{12000000,  48000,	8, 1920,	128,   8,  2,	128,   8,  2},
	{12000000,  48000,	7, 6800,	 96,   5,  4,	 96,   5,  4},
	/* 88.2k rate */
	{12000000,  88200,	7, 5264,	 64,   8,  2,	 64,   8,  2},
	{12000000,  88200,	8, 4672,	 64,   6,  3,	 64,   6,  3},
	/* 96k rate */
	{12000000,  96000,	8, 1920,	 64,   8,  2,	 64,   8,  2},
	{12000000,  96000,	7, 6800,	 48,   5,  4,	 48,   5,  4},
	/* 176.4k rate */
	{12000000, 176400,	7, 5264,	 32,   8,  2,	 32,   8,  2},
	{12000000, 176400,	8, 4672,	 32,   6,  3,	 32,   6,  3},
	/* 192k rate */
	{12000000, 192000,	8, 1920,	 32,   8,  2,	 32,   8,  2},
	{12000000, 192000,	7, 6800,	 24,   5,  4,	 24,   5,  4},
};

static const DECLARE_TLV_DB_SCALE(dac_vol_tlv, -6350, 50, 0);
static const DECLARE_TLV_DB_SCALE(class_D_drv_tlv, 600, 600, 0);
static const DECLARE_TLV_DB_SCALE(sp_vol_tlv, -6350, 50, 0);

/*
 * controls to be exported to the user space
 */
static int tlv320_put_volsw(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_soc_kcontrol_codec(kcontrol);

	// amixer cset name='AMP' 2

	// 1. amixer cset name='Speaker Switch' on
	snd_soc_update_bits(codec, /*0x26*/DAC3100_LANALOGSPL, 0x80, 0x80);
	//snd_soc_write(codec, /*0x26*/DAC3100_LANALOGSPL, 0xff);

	// 2. amixer cset name='Output Left From Left DAC' on
	snd_soc_write(codec, /*0x23*/DAC3100_DACMIXERROUTE, 0x40);

	// 3. amixer cset name='Speaker Driver Playback Switch' 1
	snd_soc_write(codec, /*0x2a*/DAC3100_SPLGAIN, 0x04);

	// 4. amixer cset name='Speaker Driver Playback Volume' 2
	return snd_soc_put_volsw(kcontrol, ucontrol);
}

static const struct snd_kcontrol_new dac3100_snd_controls[] = {
	SOC_DOUBLE_R_S_TLV("DAC", DAC3100_LDACVOL, DAC3100_RDACVOL,
			   0, -127, 48, 7, 0, dac_vol_tlv),
	SOC_SINGLE_EXT_TLV("AMP", DAC3100_SPLGAIN, 3, 3, 0,
			   snd_soc_get_volsw, tlv320_put_volsw, class_D_drv_tlv),
	SOC_SINGLE_TLV("SPK", DAC3100_LANALOGSPL,
		       0, 0x7F, 1, sp_vol_tlv),
};

static int dac3100_wait_bits(struct dac3100_priv *dac3100, unsigned int reg,
			     unsigned int mask, unsigned int wbits, int sleep,
			     int count)
{
	unsigned int bits;
	int counter = count;
	int ret = regmap_read(dac3100->regmap, reg, &bits);

	while ((bits & mask) != wbits && counter && !ret) {
		usleep_range(sleep, sleep * 2);
		ret = regmap_read(dac3100->regmap, reg, &bits);
		counter--;
	}
	if ((bits & mask) != wbits) {
		dev_err(dac3100->dev,
			"%s: Failed! 0x%x was 0x%x expected 0x%x (%d, 0x%x, %d us)\n",
			__func__, reg, bits, wbits, ret, mask,
			(count - counter) * sleep);
		ret = -1;
	}
	return ret;
}

#define WIDGET_BIT(reg, shift) (((shift) << 8) | (reg))

static int dac3100_dapm_power_event(struct snd_soc_dapm_widget *w,
				    struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = snd_soc_dapm_to_codec(w->dapm);
	struct dac3100_priv *dac3100 = snd_soc_codec_get_drvdata(codec);
	unsigned int reg = DAC3100_DACFLAG1;
	unsigned int mask;

	dev_dbg(codec->dev, "dapm: '%s' 0x%02x,%d %d\n",
		w->name, w->reg, w->mask, event);
#if 1
	if (event == SND_SOC_DAPM_POST_PMU) {
		snd_soc_write(codec, /*0x20*/DAC3100_SPKAMP, 0x86);
	} else if (event == SND_SOC_DAPM_POST_PMD) {
		snd_soc_write(codec, /*0x20*/DAC3100_SPKAMP, 0x06);
	}
#endif
	switch (WIDGET_BIT(w->reg, w->shift)) {
	case WIDGET_BIT(DAC3100_DACSETUP, 7):
		mask = DAC3100_LDACPWRSTATUS_MASK;
		break;
	case WIDGET_BIT(DAC3100_DACSETUP, 6):
		mask = DAC3100_RDACPWRSTATUS_MASK;
		break;
	default:
		dev_err(codec->dev, "Unknown widget '%s' calling %s\n",
			w->name, __func__);
		return -EINVAL;
	}

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		return dac3100_wait_bits(dac3100, reg, mask, mask, 5000, 100);
	case SND_SOC_DAPM_POST_PMD:
		return dac3100_wait_bits(dac3100, reg, mask, 0, 5000, 100);
	default:
		dev_dbg(codec->dev,
			"Unhandled dapm widget event %d from %s\n",
			event, w->name);
	}
	return 0;
}

static const struct snd_soc_dapm_widget dac3100_dapm_widgets[] = {
	SND_SOC_DAPM_AIF_IN("DAC", "DAC Playback", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_DAC_E("LDI", "Left Playback",
			   DAC3100_DACSETUP, 7, 0, dac3100_dapm_power_event,
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_OUTPUT("SPK"),
};

static const struct snd_soc_dapm_route
dac3100_audio_map[] = { /* { sink, control, source } */
	{"LDI", NULL, "DAC"},
	{"SPK", NULL, "LDI"},
};

static int dac3100_setup_pll(struct snd_soc_codec *codec,
			     struct snd_pcm_hw_params *params)
{
	struct dac3100_priv *dac3100 = snd_soc_codec_get_drvdata(codec);
	int bclk_score = snd_soc_params_to_frame_size(params);
	int mclk_p = dac3100->sysclk / dac3100->p_div;
	int bclk_n = 0;
	int match = -1;
	int i;

	/* Use PLL as CODEC_CLKIN and DAC_CLK as BDIV_CLKIN */
	snd_soc_update_bits(codec, DAC3100_CLKMUX,
			    DAC3100_CODEC_CLKIN_MASK, DAC3100_CODEC_CLKIN_PLL);
	snd_soc_update_bits(codec, DAC3100_IFACE2,
			    DAC3100_BDIVCLK_MASK, DAC3100_DAC2BCLK);

	for (i = 0; i < ARRAY_SIZE(dac3100_divs); i++) {
		if (dac3100_divs[i].rate == params_rate(params) &&
		    dac3100_divs[i].mclk_p == mclk_p) {
			int s =	(dac3100_divs[i].dosr * dac3100_divs[i].mdac) %
				snd_soc_params_to_frame_size(params);
			int bn = (dac3100_divs[i].dosr * dac3100_divs[i].mdac) /
				snd_soc_params_to_frame_size(params);
			if (s < bclk_score && bn > 0) {
				match = i;
				bclk_n = bn;
				bclk_score = s;
			}
		}
	}

	if (match == -1) {
		dev_err(codec->dev,
			"%s: Sample rate (%u) and format not supported\n",
			__func__, params_rate(params));
		/* See bellow for details how fix this. */
		return -EINVAL;
	}
	if (bclk_score != 0) {
		dev_warn(codec->dev, "Can not produce exact bitclock");
		/* This is fine if using dsp format, but if using i2s
		   there may be trouble. To fix the issue edit the
		   dac3100_divs table for your mclk and sample
		   rate. Details can be found from:
		   http://www.ti.com/lit/ds/symlink/tlv320aic3100.pdf
		   Section: 5.6 CLOCK Generation and PLL
		*/
	}
	i = match;

	/* PLL configuration */
	snd_soc_update_bits(codec, DAC3100_PLLPR, DAC3100_PLL_MASK,
			    (dac3100->p_div << 4) | 0x01);
	snd_soc_write(codec, DAC3100_PLLJ, dac3100_divs[i].pll_j);

	snd_soc_write(codec, DAC3100_PLLDMSB,
		      dac3100_divs[i].pll_d >> 8);
	snd_soc_write(codec, DAC3100_PLLDLSB,
		      dac3100_divs[i].pll_d & 0xff);

	/* DAC dividers configuration */
	snd_soc_update_bits(codec, DAC3100_NDAC, DAC3100_PLL_MASK,
			    dac3100_divs[i].ndac);
	snd_soc_update_bits(codec, DAC3100_MDAC, DAC3100_PLL_MASK,
			    dac3100_divs[i].mdac);

	snd_soc_write(codec, DAC3100_DOSRMSB, dac3100_divs[i].dosr >> 8);
	snd_soc_write(codec, DAC3100_DOSRLSB, dac3100_divs[i].dosr & 0xff);

	/* Bit clock divider configuration. */
	snd_soc_update_bits(codec, DAC3100_BCLKN,
			    DAC3100_PLL_MASK, bclk_n);

	dac3100->rate_div_line = i;

	dev_dbg(codec->dev,
		"pll %d.%04d/%d dosr %d n %d m %d aosr %d n %d m %d bclk_n %d\n",
		dac3100_divs[i].pll_j, dac3100_divs[i].pll_d,
		dac3100->p_div, dac3100_divs[i].dosr,
		dac3100_divs[i].ndac, dac3100_divs[i].mdac,
		dac3100_divs[i].aosr, dac3100_divs[i].nadc,
		dac3100_divs[i].madc, bclk_n);

	return 0;
}

static int dac3100_hw_params(struct snd_pcm_substream *substream,
			     struct snd_pcm_hw_params *params,
			     struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	u8 data = 0;

	dev_dbg(codec->dev, "## %s: width %d rate %d\n",
		__func__, params_width(params),
		params_rate(params));

	switch (params_width(params)) {
	case 16:
		break;
	case 20:
		data = (DAC3100_WORD_LEN_20BITS <<
			DAC3100_IFACE1_DATALEN_SHIFT);
		break;
	case 24:
		data = (DAC3100_WORD_LEN_24BITS <<
			DAC3100_IFACE1_DATALEN_SHIFT);
		break;
	case 32:
		data = (DAC3100_WORD_LEN_32BITS <<
			DAC3100_IFACE1_DATALEN_SHIFT);
		break;
	default:
		dev_err(codec->dev, "%s: Unsupported width %d\n",
			__func__, params_width(params));
		return -EINVAL;
	}

	snd_soc_update_bits(codec, DAC3100_IFACE1,
			    DAC3100_IFACE1_DATALEN_MASK,
			    data);

	return dac3100_setup_pll(codec, params);
}

static int dac3100_dac_mute(struct snd_soc_dai *codec_dai, int mute)
{
	struct snd_soc_codec *codec = codec_dai->codec;

	dev_dbg(codec->dev, "## %s: %d\n", __func__, mute);
	if (mute) {
		snd_soc_update_bits(codec, DAC3100_DACMUTE,
				    DAC3100_DACMUTE_MASK,
				    DAC3100_DACMUTE_MASK);
	} else {
		snd_soc_update_bits(codec, DAC3100_DACMUTE,
				    DAC3100_DACMUTE_MASK, 0x0);
	}

	return 0;
}

static int dac3100_set_dai_fmt(struct snd_soc_dai *codec_dai,
			       unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u8 iface_reg1 = 0;
	u8 iface_reg2 = 0;
	u8 dsp_a_val = 0;

	dev_dbg(codec->dev, "## %s: fmt = 0x%x\n", __func__, fmt);

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		iface_reg1 |= DAC3100_BCLK_MASTER | DAC3100_WCLK_MASTER;
		break;
	default:
		dev_alert(codec->dev, "Invalid DAI master/slave interface\n");
		return -EINVAL;
	}

	/* interface format */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	case SND_SOC_DAIFMT_DSP_A:
		dsp_a_val = 0x1;
	case SND_SOC_DAIFMT_DSP_B:
		/* NOTE: BCLKINV bit value 1 equas NB and 0 equals IB */
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			iface_reg2 |= DAC3100_BCLKINV_MASK;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			break;
		default:
			return -EINVAL;
		}
		iface_reg1 |= (DAC3100_DSP_MODE <<
			       DAC3100_IFACE1_DATATYPE_SHIFT);
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		iface_reg1 |= (DAC3100_RIGHT_JUSTIFIED_MODE <<
			       DAC3100_IFACE1_DATATYPE_SHIFT);
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface_reg1 |= (DAC3100_LEFT_JUSTIFIED_MODE <<
			       DAC3100_IFACE1_DATATYPE_SHIFT);
		break;
	default:
		dev_err(codec->dev, "Invalid DAI interface format\n");
		return -EINVAL;
	}

	snd_soc_update_bits(codec, DAC3100_IFACE1,
			    DAC3100_IFACE1_DATATYPE_MASK |
			    DAC3100_IFACE1_MASTER_MASK,
			    iface_reg1);
	snd_soc_update_bits(codec, DAC3100_DATA_OFFSET,
			    DAC3100_DATA_OFFSET_MASK,
			    dsp_a_val);
	snd_soc_update_bits(codec, DAC3100_IFACE2,
			    DAC3100_BCLKINV_MASK,
			    iface_reg2);

	return 0;
}

static int dac3100_set_dai_sysclk(struct snd_soc_dai *codec_dai,
				  int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct dac3100_priv *dac3100 = snd_soc_codec_get_drvdata(codec);

	dac3100->p_div = 1;

	/* set clock on MCLK, BCLK, or GPIO1 as PLL input */
	snd_soc_update_bits(codec, DAC3100_CLKMUX, DAC3100_PLL_CLKIN_MASK,
			    clk_id << DAC3100_PLL_CLKIN_SHIFT);

	dac3100->sysclk = freq;
	return 0;
}

static void dac3100_clk_on(struct snd_soc_codec *codec)
{
	struct dac3100_priv *dac3100 = snd_soc_codec_get_drvdata(codec);
	u8 mask = DAC3100_PM_MASK;
	u8 on = DAC3100_PM_MASK;

	dev_dbg(codec->dev, "codec clock -> on (rate %d)\n",
		dac3100_divs[dac3100->rate_div_line].rate);
	snd_soc_update_bits(codec, DAC3100_PLLPR, mask, on);
	mdelay(10);
	snd_soc_update_bits(codec, DAC3100_NDAC, mask, on);
	snd_soc_update_bits(codec, DAC3100_MDAC, mask, on);
	snd_soc_update_bits(codec, DAC3100_BCLKN, mask, on);
}

static void dac3100_clk_off(struct snd_soc_codec *codec)
{
	u8 mask = DAC3100_PM_MASK;
	u8 off = 0;

	dev_dbg(codec->dev, "codec clock -> off\n");
	snd_soc_update_bits(codec, DAC3100_BCLKN, mask, off);
	snd_soc_update_bits(codec, DAC3100_MDAC, mask, off);
	snd_soc_update_bits(codec, DAC3100_NDAC, mask, off);
	snd_soc_update_bits(codec, DAC3100_PLLPR, mask, off);
}

static int dac3100_power_on(struct snd_soc_codec *codec)
{
	struct dac3100_priv *dac3100 = snd_soc_codec_get_drvdata(codec);
	int ret = 0;

	dev_dbg(codec->dev, "dac3100_power_on: %p\n", codec);
#if 0 // move to alsa_sound_init
	if (gpio_is_valid(dac3100->pdata.gpio_reset)) {
		gpio_set_value(dac3100->pdata.gpio_reset, 1);
		udelay(100);
	}
#endif
	regcache_cache_only(dac3100->regmap, false);
	ret = regcache_sync(dac3100->regmap);
	if (ret != 0) {
		dev_err(codec->dev,
			"Failed to restore cache: %d\n", ret);
		regcache_cache_only(dac3100->regmap, true);
		return ret;
	}
	return 0;
}

static int dac3100_power_off(struct snd_soc_codec *codec)
{
	struct dac3100_priv *dac3100 = snd_soc_codec_get_drvdata(codec);
	int ret = 0;

	dev_dbg(codec->dev, "dac3100_power_off: %p\n", codec);
	regcache_cache_only(dac3100->regmap, true);

	return ret;
}

static int dac3100_set_bias_level(struct snd_soc_codec *codec,
				  enum snd_soc_bias_level level)
{
	dev_dbg(codec->dev, "## %s: %d -> %d\n", __func__,
		snd_soc_codec_get_bias_level(codec), level);

	switch (level) {
	case SND_SOC_BIAS_ON:
		break;
	case SND_SOC_BIAS_PREPARE:
		if (snd_soc_codec_get_bias_level(codec) == SND_SOC_BIAS_STANDBY)
			dac3100_clk_on(codec);
		break;
	case SND_SOC_BIAS_STANDBY:
		switch (snd_soc_codec_get_bias_level(codec)) {
		case SND_SOC_BIAS_OFF:
			dac3100_power_on(codec);
			break;
		case SND_SOC_BIAS_PREPARE:
			dac3100_clk_off(codec);
			break;
		default:
			BUG();
		}
		break;
	case SND_SOC_BIAS_OFF:
		if (snd_soc_codec_get_bias_level(codec) == SND_SOC_BIAS_STANDBY)
			dac3100_power_off(codec);
		break;
	}

	return 0;
}

static int dac3100_codec_probe(struct snd_soc_codec *codec)
{
	int ret = 0;
	struct dac3100_priv *dac3100 = snd_soc_codec_get_drvdata(codec);

	dev_dbg(dac3100->dev, "## %s\n", __func__);

	dac3100 = snd_soc_codec_get_drvdata(codec);

	dac3100->codec = codec;

	regcache_cache_only(dac3100->regmap, true);
	regcache_mark_dirty(dac3100->regmap);

	return ret;
}

static int dac3100_codec_remove(struct snd_soc_codec *codec)
{
	return 0;
}

static struct snd_soc_codec_driver soc_codec_driver_dac3100 = {
	.probe			= dac3100_codec_probe,
	.remove			= dac3100_codec_remove,
	.set_bias_level		= dac3100_set_bias_level,
	.suspend_bias_off	= true,

	.component_driver = {
		.controls		= dac3100_snd_controls,
		.num_controls		= ARRAY_SIZE(dac3100_snd_controls),
		.dapm_widgets		= dac3100_dapm_widgets,
		.num_dapm_widgets	= ARRAY_SIZE(dac3100_dapm_widgets),
		.dapm_routes		= dac3100_audio_map,
		.num_dapm_routes	= ARRAY_SIZE(dac3100_audio_map),
	},
};

static struct snd_soc_dai_ops dac3100_dai_ops = {
	.hw_params	= dac3100_hw_params,
	.set_sysclk	= dac3100_set_dai_sysclk,
	.set_fmt	= dac3100_set_dai_fmt,
	.digital_mute	= dac3100_dac_mute,
};

static struct snd_soc_dai_driver dac3100_dai_driver[] = {
	{
		.name = "tlv320",
		.playback = {
			.stream_name	 = "Playback",
			.channels_min	 = 2,
			.channels_max	 = 2,
			.rates		 = DAC3100_RATES,
			.formats	 = DAC3100_FORMATS,
		},
		.ops = &dac3100_dai_ops,
		.symmetric_rates = 1,
	}
};

static const struct of_device_id tlv320dac3100_of_match[] = {
	{ .compatible = "ti,tlv320" },
	{},
};
MODULE_DEVICE_TABLE(of, tlv320dac3100_of_match);

static void dac3100_pdata_from_of(struct dac3100_priv *dac3100)
{
	dac3100->pdata.micbias_vg = 1; // MICBIAS_2_0V
	dac3100->pdata.gpio_reset = 8; // MX6QDL_PAD_GPIO_8__GPIO1_IO08
}

static int dac3100_device_init(struct dac3100_priv *dac3100)
{
	int ret;

	dev_set_drvdata(dac3100->dev, dac3100);
	dac3100_pdata_from_of(dac3100);

#if 0 // move to alsa_sound_init
	if (dac3100->pdata.gpio_reset) {
		ret = devm_gpio_request_one(dac3100->dev,
					    dac3100->pdata.gpio_reset,
					    GPIOF_OUT_INIT_HIGH,
					    "tlv320-reset-pin");
		if (ret < 0) {
			dev_err(dac3100->dev, "not able to acquire gpio\n");
			return ret;
		}
	}
#endif

	return 0;
}

static int dac3100_i2c_probe(struct i2c_client *i2c,
			     const struct i2c_device_id *id)
{
	struct dac3100_priv *dac3100;
	int ret;
	const struct regmap_config *regmap_config;

	dev_dbg(&i2c->dev, "## %s: %s codec_type = %d\n", __func__,
		id->name, (int) id->driver_data);

	regmap_config = &dac3100_i2c_regmap;

	dac3100 = devm_kzalloc(&i2c->dev, sizeof(*dac3100), GFP_KERNEL);
	if (dac3100 == NULL)
		return -ENOMEM;

	dac3100->regmap = devm_regmap_init_i2c(i2c, regmap_config);
	if (IS_ERR(dac3100->regmap)) {
		ret = PTR_ERR(dac3100->regmap);
		dev_err(&i2c->dev, "Failed to allocate register map: %d\n",
			ret);
		return ret;
	}
	dac3100->dev = &i2c->dev;

	dac3100->pdata.codec_type = id->driver_data;

	ret = dac3100_device_init(dac3100);
	if (ret)
		return ret;

	return snd_soc_register_codec(&i2c->dev, &soc_codec_driver_dac3100,
				     dac3100_dai_driver,
				     ARRAY_SIZE(dac3100_dai_driver));
}

static int dac3100_i2c_remove(struct i2c_client *i2c)
{
	snd_soc_unregister_codec(&i2c->dev);
	return 0;
}

static const struct i2c_device_id dac3100_i2c_id[] = {
	{ "tlv320", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, dac3100_i2c_id);

static struct i2c_driver dac3100_i2c_driver = {
	.driver = {
		.name	= "tlv320",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(tlv320dac3100_of_match),
	},
	.probe		= dac3100_i2c_probe,
	.remove		= dac3100_i2c_remove,
	.id_table	= dac3100_i2c_id,
};

module_i2c_driver(dac3100_i2c_driver);

MODULE_DESCRIPTION("ASoC TLV320DAC3100 codec driver");
MODULE_AUTHOR("Jyri Sarha");
MODULE_LICENSE("GPL");
