/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 * Copyright 2012 Linaro Ltd.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <sound/soc.h>

#include "imx-audmux.h"

//#define USE_SOC_OPS

struct imx_tlv320_data {
	struct snd_soc_dai_link dai;
	struct snd_soc_card card;
	unsigned int clk_frequency;
};

#ifdef USE_SOC_OPS
static int imx_tlv320_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct imx_tlv320_data *data = snd_soc_card_get_drvdata(rtd->card);
	struct device *dev = rtd->card->dev;
	int ret;

	dev_dbg(dev, "imx_tlv320_hw_params\n");

	ret = snd_soc_dai_set_sysclk(rtd->codec_dai, 0,
				     data->clk_frequency, SND_SOC_CLOCK_OUT);
	if (ret) {
		dev_err(dev,
			"Failed to set the codec sysclk.\n");
		return ret;
	}

	snd_soc_dai_set_tdm_slot(rtd->cpu_dai, 0x3, 0x3, 2, 0);

	ret = snd_soc_dai_set_sysclk(rtd->cpu_dai, 0/*IMX_SSP_SYS_CLK*/, 0,
				     SND_SOC_CLOCK_IN);
	/* fsl_ssi lacks the set_sysclk ops */
	if (ret && ret != -EINVAL) {
		dev_err(dev,
			"Can't set the IMX_SSP_SYS_CLK CPU system clock.\n");
		return ret;
	}

	return 0;
}

static struct snd_soc_ops imx_tlv320_snd_ops = {
	.hw_params	= imx_tlv320_hw_params,
};
#else
static int imx_tlv320_dai_init(struct snd_soc_pcm_runtime *rtd)
{
	struct imx_tlv320_data *data = snd_soc_card_get_drvdata(rtd->card);
	struct device *dev = rtd->card->dev;
	int ret;

	dev_dbg(dev, "tlv320_dai_init\n");

	ret = snd_soc_dai_set_sysclk(rtd->codec_dai, 0, data->clk_frequency, SND_SOC_CLOCK_IN);
	if (ret) {
		dev_err(dev, "could not set codec driver clock params\n");
		return ret;
	}

	return 0;
}
#endif

static int imx_tlv320_probe(struct platform_device *pdev)
{
//	struct device_node *np = pdev->dev.of_node;
	struct device_node *ssi_np, *codec_np;
	struct platform_device *ssi_pdev;
	struct i2c_client *codec_dev;
	struct imx_tlv320_data *data = NULL;
	int int_port, ext_port;
	int ret;

	int_port = 2;
	ext_port = 4;

	/*
	 * The port numbering in the hardware manual starts at 1, while
	 * the audmux API expects it starts at 0.
	 */
	int_port--;
	ext_port--;
	ret = imx_audmux_v2_configure_port(int_port,
			IMX_AUDMUX_V2_PTCR_SYN |
			IMX_AUDMUX_V2_PTCR_TFSEL(ext_port) |
			IMX_AUDMUX_V2_PTCR_TCSEL(ext_port) |
			IMX_AUDMUX_V2_PTCR_TFSDIR |
			IMX_AUDMUX_V2_PTCR_TCLKDIR,
			IMX_AUDMUX_V2_PDCR_RXDSEL(ext_port));
	if (ret) {
		dev_err(&pdev->dev, "audmux internal port setup failed\n");
		return ret;
	}
	ret = imx_audmux_v2_configure_port(ext_port,
			IMX_AUDMUX_V2_PTCR_SYN,
			IMX_AUDMUX_V2_PDCR_RXDSEL(int_port));
	if (ret) {
		dev_err(&pdev->dev, "audmux external port setup failed\n");
		return ret;
	}

	ssi_np = of_parse_phandle(pdev->dev.of_node, "ssi-controller", 0);
	codec_np = of_parse_phandle(pdev->dev.of_node, "audio-codec", 0);
	if (!ssi_np || !codec_np) {
		dev_err(&pdev->dev, "phandle missing or invalid\n");
		ret = -EINVAL;
		goto fail;
	}

	ssi_pdev = of_find_device_by_node(ssi_np);
	if (!ssi_pdev) {
		dev_err(&pdev->dev, "failed to find SSI platform device\n");
		ret = -EPROBE_DEFER;
		goto fail;
	}
	codec_dev = of_find_i2c_device_by_node(codec_np);
	if (!codec_dev) {
		dev_err(&pdev->dev, "failed to find codec platform device\n");
		return -EPROBE_DEFER;
	}

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data) {
		ret = -ENOMEM;
		goto fail;
	}

	data->clk_frequency = 12000000;

	data->dai.name = "HiFi";
	data->dai.stream_name = "HiFi";
	data->dai.codec_dai_name = "tlv320";
	data->dai.codec_of_node = codec_np;
	data->dai.cpu_of_node = ssi_np;
	data->dai.platform_of_node = ssi_np;
#ifdef USE_SOC_OPS
	data->dai.ops = &imx_tlv320_snd_ops,
#else
	data->dai.init = &imx_tlv320_dai_init;
#endif
	data->dai.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
			    SND_SOC_DAIFMT_CBM_CFM;

	data->card.dev = &pdev->dev;
	ret = snd_soc_of_parse_card_name(&data->card, "model");
	if (ret)
		goto fail;

	data->card.num_links = 1;
	data->card.owner = THIS_MODULE;
	data->card.dai_link = &data->dai;

	platform_set_drvdata(pdev, &data->card);
	snd_soc_card_set_drvdata(&data->card, data);

	ret = devm_snd_soc_register_card(&pdev->dev, &data->card);
	if (ret) {
		dev_err(&pdev->dev, "snd_soc_register_card failed (%d)\n", ret);
		goto fail;
	}

	of_node_put(ssi_np);
	of_node_put(codec_np);

	return 0;

fail:
	of_node_put(ssi_np);
	of_node_put(codec_np);

	return ret;
}

static int imx_tlv320_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id imx_tlv320_dt_ids[] = {
	{ .compatible = "fsl,imx-audio-tlv320", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx_tlv320_dt_ids);

static struct platform_driver imx_tlv320_driver = {
	.driver = {
		.name = "imx-tlv320",
		.of_match_table = imx_tlv320_dt_ids,
	},
	.probe = imx_tlv320_probe,
	.remove = imx_tlv320_remove,
};
module_platform_driver(imx_tlv320_driver);

MODULE_AUTHOR("chpark");
MODULE_DESCRIPTION("Freescale i.MX TLV320 ASoC machine driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:imx-tlv320");
