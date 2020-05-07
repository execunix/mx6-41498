/*
 * ALSA SoC TLV320DAC3100 codec driver
 *
 * Copyright (C) 2013 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef _TLV320DAC3100_H
#define _TLV320DAC3100_H

#define DAC3100_RATES	SNDRV_PCM_RATE_8000_192000

#define DAC3100_FORMATS	(SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE \
			 | SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S24_LE \
			 | SNDRV_PCM_FMTBIT_S32_LE)


#define DAC3100_STEREO_CLASS_D_BIT	0x1
#define DAC3100_MINIDSP_BIT		0x2

enum dac3100_type {
	AIC3100	= 0,
	AIC3110 = DAC3100_STEREO_CLASS_D_BIT,
	AIC3120 = DAC3100_MINIDSP_BIT,
	AIC3111 = (DAC3100_STEREO_CLASS_D_BIT | DAC3100_MINIDSP_BIT),
};

struct dac3100_pdata {
	enum dac3100_type codec_type;
	unsigned int gpio_reset;
	int micbias_vg;
};

/* Page Control Register */
#define DAC3100_PAGECTL				0x00

/* Page 0 Registers */
/* Software reset register */
#define DAC3100_RESET				0x01
/* OT FLAG register */
#define DAC3100_OT_FLAG				0x03
/* Clock clock Gen muxing, Multiplexers*/
#define DAC3100_CLKMUX				0x04
/* PLL P and R-VAL register */
#define DAC3100_PLLPR				0x05
/* PLL J-VAL register */
#define DAC3100_PLLJ				0x06
/* PLL D-VAL MSB register */
#define DAC3100_PLLDMSB				0x07
/* PLL D-VAL LSB register */
#define DAC3100_PLLDLSB				0x08
/* DAC NDAC_VAL register*/
#define DAC3100_NDAC				0x0B
/* DAC MDAC_VAL register */
#define DAC3100_MDAC				0x0C
/* DAC OSR setting register 1, MSB value */
#define DAC3100_DOSRMSB				0x0D
/* DAC OSR setting register 2, LSB value */
#define DAC3100_DOSRLSB				0x0E
/* Clock setting register 9, Multiplexers */
#define DAC3100_CLKOUTMUX			0x19
/* Clock setting register 10, CLOCKOUT M divider value */
#define DAC3100_CLKOUTMVAL			0x1A
/* Audio Interface Setting Register 1 */
#define DAC3100_IFACE1				0x1B
/* Audio Data Slot Offset Programming */
#define DAC3100_DATA_OFFSET			0x1C
/* Audio Interface Setting Register 2 */
#define DAC3100_IFACE2				0x1D
/* Clock setting register 11, BCLK N Divider */
#define DAC3100_BCLKN				0x1E
/* Audio Interface Setting Register 3, Secondary Audio Interface */
#define DAC3100_IFACESEC1			0x1F
/* Audio Interface Setting Register 4 */
#define DAC3100_IFACESEC2			0x20
/* Audio Interface Setting Register 5 */
#define DAC3100_IFACESEC3			0x21
/* I2C Bus Condition */
#define DAC3100_I2C				0x22
/* DAC Flag Registers */
#define DAC3100_DACFLAG1			0x25
#define DAC3100_DACFLAG2			0x26
/* Sticky Interrupt flag (overflow) */
#define DAC3100_OFFLAG				0x27
/* Sticy DAC Interrupt flags */
#define DAC3100_INTRDACFLAG			0x2C
/* Sticy ADC Interrupt flags */
#define DAC3100_INTRADCFLAG			0x2D
/* DAC Interrupt flags 2 */
#define DAC3100_INTRDACFLAG2			0x2E
/* ADC Interrupt flags 2 */
#define DAC3100_INTRADCFLAG2			0x2F
/* INT1 interrupt control */
#define DAC3100_INT1CTRL			0x30
/* INT2 interrupt control */
#define DAC3100_INT2CTRL			0x31
/* GPIO1 control */
#define DAC3100_GPIO1				0x33
/* DIN (IN Pin) control */
#define DAC3100_DIN				0x36

#define DAC3100_DACPRB				0x3C
/* DAC channel setup register */
#define DAC3100_DACSETUP			0x3F
/* DAC Mute and volume control register */
#define DAC3100_DACMUTE				0x40
/* Left DAC channel digital volume control */
#define DAC3100_LDACVOL				0x41
/* Right DAC channel digital volume control */
#define DAC3100_RDACVOL				0x42
/* Headset detection */
#define DAC3100_HSDETECT			0x43
/* DRC control 1 */
#define DAC3100_DRC1				0x44
/* DRC control 2 */
#define DAC3100_DRC2				0x45
/* DRC control 3 */
#define DAC3100_DRC3				0x46
/* Left beep generator */
#define DAC3100_LBGEN				0x47
/* Right beep generator */
#define DAC3100_RBGEN				0x48


/* Page 1 Registers */
/* Headphone drivers */
#define DAC3100_HPDRIVER			0x9F
/* Class-D Speakear Amplifier */
#define DAC3100_SPKAMP				0xA0
/* HP Output Drivers POP Removal Settings */
#define DAC3100_HPPOP				0xA1
/* Output Driver PGA Ramp-Down Period Control */
#define DAC3100_SPPGARAMP			0xA2
/* DAC_L and DAC_R Output Mixer Routing */
#define DAC3100_DACMIXERROUTE			0xA3
/* Left Analog Vol to HPL */
#define DAC3100_LANALOGHPL			0xA4
/* Right Analog Vol to HPR */
#define DAC3100_RANALOGHPR			0xA5
/* Left Analog Vol to SPL */
#define DAC3100_LANALOGSPL			0xA6
/* HPL Driver */
#define DAC3100_HPLGAIN				0xA8
/* HPR Driver */
#define DAC3100_HPRGAIN				0xA9
/* SPL Driver */
#define DAC3100_SPLGAIN				0xAA
/* HP Driver Control */
#define DAC3100_HPCONTROL			0xAC
/* MIC Bias Control */
#define DAC3100_MICBIAS				0xAE
/* Input CM Settings */
#define DAC3100_MICPGACM			0xB2

/* Bits, masks and shifts */

/* DAC3100_CLKMUX */
#define DAC3100_PLL_CLKIN_MASK			0x0c
#define DAC3100_PLL_CLKIN_SHIFT			2
#define DAC3100_PLL_CLKIN_MCLK			0
#define DAC3100_CODEC_CLKIN_MASK		0x03
#define DAC3100_CODEC_CLKIN_SHIFT		0
#define DAC3100_CODEC_CLKIN_PLL			3
#define DAC3100_CODEC_CLKIN_BCLK		1

/* DAC3100_PLLPR, DAC3100_NDAC, DAC3100_MDAC, DAC3100_NADC, DAC3100_MADC,
   DAC3100_BCLKN */
#define DAC3100_PLL_MASK		0x7f
#define DAC3100_PM_MASK			0x80

/* DAC3100_IFACE1 */
#define DAC3100_WORD_LEN_16BITS		0x00
#define DAC3100_WORD_LEN_20BITS		0x01
#define DAC3100_WORD_LEN_24BITS		0x02
#define DAC3100_WORD_LEN_32BITS		0x03
#define DAC3100_IFACE1_DATALEN_MASK	0x30
#define DAC3100_IFACE1_DATALEN_SHIFT	(4)
#define DAC3100_IFACE1_DATATYPE_MASK	0xC0
#define DAC3100_IFACE1_DATATYPE_SHIFT	(6)
#define DAC3100_I2S_MODE		0x00
#define DAC3100_DSP_MODE		0x01
#define DAC3100_RIGHT_JUSTIFIED_MODE	0x02
#define DAC3100_LEFT_JUSTIFIED_MODE	0x03
#define DAC3100_IFACE1_MASTER_MASK	0x0C
#define DAC3100_BCLK_MASTER		0x08
#define DAC3100_WCLK_MASTER		0x04

/* DAC3100_DATA_OFFSET */
#define DAC3100_DATA_OFFSET_MASK	0xFF

/* DAC3100_IFACE2 */
#define DAC3100_BCLKINV_MASK		0x08
#define DAC3100_BDIVCLK_MASK		0x03
#define DAC3100_DAC2BCLK		0x00
#define DAC3100_DACMOD2BCLK		0x01
#define DAC3100_ADC2BCLK		0x02
#define DAC3100_ADCMOD2BCLK		0x03

/* DAC3100_ADCFLAG */
#define DAC3100_ADCPWRSTATUS_MASK		0x40

/* DAC3100_DACFLAG1 */
#define DAC3100_LDACPWRSTATUS_MASK		0x80
#define DAC3100_RDACPWRSTATUS_MASK		0x08
#define DAC3100_HPLDRVPWRSTATUS_MASK		0x20
#define DAC3100_HPRDRVPWRSTATUS_MASK		0x02
#define DAC3100_SPLDRVPWRSTATUS_MASK		0x10
#define DAC3100_SPRDRVPWRSTATUS_MASK		0x01

/* DAC3100_INTRDACFLAG */
#define DAC3100_HPSCDETECT_MASK			0x80
#define DAC3100_BUTTONPRESS_MASK		0x20
#define DAC3100_HSPLUG_MASK			0x10
#define DAC3100_LDRCTHRES_MASK			0x08
#define DAC3100_RDRCTHRES_MASK			0x04
#define DAC3100_DACSINT_MASK			0x02
#define DAC3100_DACAINT_MASK			0x01

/* DAC3100_INT1CTRL */
#define DAC3100_HSPLUGDET_MASK			0x80
#define DAC3100_BUTTONPRESSDET_MASK		0x40
#define DAC3100_DRCTHRES_MASK			0x20
#define DAC3100_AGCNOISE_MASK			0x10
#define DAC3100_OC_MASK				0x08
#define DAC3100_ENGINE_MASK			0x04

/* DAC3100_DACSETUP */
#define DAC3100_SOFTSTEP_MASK			0x03

/* DAC3100_DACMUTE */
#define DAC3100_DACMUTE_MASK			0x0C

/* DAC3100_MICBIAS */
#define DAC3100_MICBIAS_MASK			0x03
#define DAC3100_MICBIAS_SHIFT			0

#endif	/* _TLV320DAC3100_H */
