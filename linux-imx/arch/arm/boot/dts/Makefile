# SPDX-License-Identifier: GPL-2.0
ifeq ($(CONFIG_OF),y)

dtb-$(CONFIG_MX6ES1) += \
	imx6q-sabreauto-es1.dtb \
	imx6q-sabreauto-es1-mfg.dtb
dtb-$(CONFIG_SOC_IMX6Q) += \
	imx6q-sabreauto.dtb
endif

dtstree		:= $(srctree)/$(src)
dtb-$(CONFIG_OF_ALL_DTBS) := $(patsubst $(dtstree)/%.dts,%.dtb, $(wildcard $(dtstree)/*.dts))

always		:= $(dtb-y)
clean-files	:= *.dtb
