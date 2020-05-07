#!/bin/sh
#launch_uuc ci_hdrc.0 0

function launch_dhcpd() {
	touch -f /var/lib/misc/udhcpd.leases
	echo start	168.1.1.50 > dhcpd.conf
	echo end	168.1.1.253 >> dhcpd.conf
	echo interface usb0 	>> dhcpd.conf

	ifconfig usb0 168.1.1.1
	udhcpd dhcpd.conf
}

mkdir /sys/kernel/config/usb_gadget/g1
cd /sys/kernel/config/usb_gadget/g1

echo 0x066F > idVendor
echo 0x9CFF > idProduct

mkdir strings/0x409
echo 0000000000000000 > strings/0x409/serialnumber
echo "FSL i.MX Board" > strings/0x409/product
mkdir configs/c.1
echo 5 > configs/c.1/MaxPower

echo ffs.utp0

echo 1 > os_desc/use
echo "MSFT100" > os_desc/qw_sign
echo 0x40 > os_desc/b_vendor_code

mkdir functions/ncm.1
ln -s functions/ncm.1 configs/c.1/
echo ci_hdrc.0 > UDC

echo "Start config network"

ifconfig usb0 up
while true; do
	if [[ `ifconfig usb0` == *inet6* ]]; then
		break;
	fi
	echo "wait usb0 inet6"
	sleep 1
done

launch_dhcpd 0

remote=""
while [[ "$remote" == "" ]];
do
	sleep 2
	echo retry get remote ipaddress
	ping6 -c2 -I usb0 ff02::1

	remote=`ip -6 neighbor show dev usb0`
	remote=(${remote})
	remote=${remote[0]}
done

echo ${remote}
mount -t nfs [${remote}%usb0]:/imx6 /mnt/

