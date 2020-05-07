#!/bin/sh

mkdir /sys/kernel/config/usb_gadget/g1
cd /sys/kernel/config/usb_gadget/g1

echo 0x1b67 > idVendor
echo 0x400c > idProduct
mkdir strings/0x409
echo "1234567890" > strings/0x409/serialnumber
echo "freescale" > strings/0x409/manufacturer
cat /proc/device-tree/model > strings/0x409/product
mkdir configs/c.1
mkdir configs/c.1/strings/0x409
echo "mass_storage" > configs/c.1/strings/0x409/configuration
mkdir functions/mass_storage.1
mkdir functions/mass_storage.1/partition.1
ln -s functions/mass_storage.1 configs/c.1
echo "/dev/mmcblk2p1" > functions/mass_storage.1/lun.0/file
#echo "/dev/mmcblk2p1" > functions/mass_storage.1/partition.1/file
echo "ci_hdrc.0" > UDC
cd -
