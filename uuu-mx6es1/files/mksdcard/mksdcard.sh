#!/bin/sh

# wait for the SD/MMC device node ready
while [ ! -e $1 ]
do
sleep 1
echo wait for $1 appear
done

# call sfdisk to create partition table
# destroy the partition table
node=$1
dd if=/dev/zero of=${node} bs=1024 count=1

sfdisk --force ${node} << EOF
10M,40M,0c
60M,,83
EOF
