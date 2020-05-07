#!/bin/sh
#launch_uuc ci_hdrc.0 0

mkdir /sys/kernel/config/usb_gadget/g1
cd /sys/kernel/config/usb_gadget/g1

echo 0x066F > idVendor
echo 0x9BFF > idProduct

mkdir strings/0x409
echo 0000000000000000 > strings/0x409/serialnumber
echo "FSL i.MX Board" > strings/0x409/product
mkdir configs/c.1
echo 5 > configs/c.1/MaxPower

echo ffs.utp0

echo 1 > os_desc/use
echo "MSFT100" > os_desc/qw_sign
echo 0x40 > os_desc/b_vendor_code

mkdir functions/ffs.utp0
mkdir /dev/usb-utp0
mount -t functionfs utp0 /dev/usb-utp0
ln -s functions/ffs.utp0 configs/c.1/
ln -s configs/c.1 os_desc

#mkdir functions/mass_storage.1
#ln -s functions/mass_storage.1 configs/c.1/
#echo /fat > functions/mass_storage.1/lun.0/file

ufb /dev/usb-utp0/ep0 &

echo run utp at /dev/usb-utp0/ep0;
while [ ! -e /dev/usb-utp0/ep1 ]
do
	echo "."
	sleep 1;
done

echo ci_hdrc.0 > UDC

echo uuc /dev/utp
uuc /dev/utp &
