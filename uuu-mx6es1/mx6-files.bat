@REM 
@REM deploy.bat
@REM 

set date22=%date:-=%
set cp_cmd=copy /y
set cp_dst="files"
set cp_src="Z:\hdd1\imx-41498"

%cp_cmd% %cp_src%\uboot-imx\u-boot.imx %cp_dst%\u-boot.imx
%cp_cmd% %cp_src%\uboot-imx\u-boot-mfg.imx %cp_dst%\u-boot-mfg.imx
%cp_cmd% %cp_src%\linux-imx\arch\arm\boot\zImage %cp_dst%\zImage
%cp_cmd% %cp_src%\linux-imx\arch\arm\boot\zImage-mfg %cp_dst%\zImage-mfg
%cp_cmd% %cp_src%\linux-imx\arch\arm\boot\dts\imx6q-sabreauto-es1.dtb %cp_dst%\imx6q-es1.dtb
%cp_cmd% %cp_src%\linux-imx\arch\arm\boot\dts\imx6q-sabreauto-es1-mfg.dtb %cp_dst%\imx6q-es1-mfg.dtb
%cp_cmd% Z:\hdd1\imx-yocto-bsp\build-fb\tmp\deploy\images\imx6qsabreauto\fsl-image-mfgtool-initramfs-imx6qsabreauto.cpio.gz.u-boot %cp_dst%\initramfs-mfg.cpio.gz.u-boot
@REM copy /y Z:\hdd1\imx-yocto-bsp\build-fb\tmp\deploy\images\imx6qsabreauto\fsl-image-validation-imx-imx6qsabreauto.tar.bz2 files\rootfs.tar.bz2
@REM copy /y Z:\hdd1\imx6\initramfs\initramfs-es1.cpio.gz.u-boot %cp_dst%\initramfs-es1.cpio.gz.u-boot
@REM copy /y Z:\hdd1\imx6\rootfs\rootfs-es1.tar.bz2 files\rootfs-es1.tar.bz2

ping -n 1 127.0.0.1
cscript mx6-gendat.vbs

pause
