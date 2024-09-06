#!/bin/bash
#fix for pcan analyser 
sudo ip link set can0 name can3

sudo busybox devmem 0x0c303018 w 0x458
sudo busybox devmem 0x0c303010 w 0x400

sudo insmod /lib/modules/5.10.120-tegra/kernel/net/can/can.ko
sudo insmod /lib/modules/5.10.120-tegra/kernel/net/can/can-raw.ko
sudo modprobe mttcan

#pulling mcp2515 enabled iqr disabled
#sudo insmod /lib/modules/5.10.120-tegra/kernel/drivers/net/can/spi/mcp251x.ko
sudo insmod ../.module/mcp251x.ko



sudo ip link set can0 up type can bitrate 500000
sudo ip link set can1 up type can bitrate 500000
sudo ip link set can2 up type can bitrate 500000

sudo ip link set can3 up type can bitrate 500000



exit 0
