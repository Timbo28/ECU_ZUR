#!/bin/bash

# Load PC driver for CAN bus


# set up CAN buses
sudo ip link set can0 up type can bitrate 500000
sudo ip link set can1 up type can bitrate 500000
sudo ip link set can2 up type can bitrate 500000
sudo ip link set can3 up type can bitrate 500000



exit 0
