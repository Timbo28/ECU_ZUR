# Flashing Guide

## Replace files from [Jetson Linux](https://github.zhaw.ch/hpmm/anyvision_orin_nx) with .kernel files

__Important!!__ This device tree files are only needed if you want to use the two MCP2515 chips on the adapter board thit the coresponding Kernal Driver. 

* anyvision
```
anyvision_orin_nx/Linux_for_Tegra/sources/hardware/nvidia/platform/t23x/p3768/kernel-dts/anyvision/tegra234-mb2-bct-anyvision.dtsi
```
* BCT
```
anyvision_orin_nx/Linux_for_Tegra/bootloader/t186ref/BCT/tegra234-mb1-bct-padvoltage-p3767-0000-anyvision.dtsi
anyvision_orin_nx/Linux_for_Tegra/bootloader/t186ref/BCT/tegra234-mb1-bct-pinmux-p3767-0000-anyvision.dtsi
anyvision_orin_nx/Linux_for_Tegra/bootloader/t186ref/BCT/tegra234-mb2-bct-misc-p3767-0000-anyvision.dts
```

## Need to be installed

* docker
```
sudo apt-get install docker.io
```
* busybox
```
sudo apt-get install busybox
```
* can-utils
```
sudo apt-get install can-utils
```
* GCC 13.1
```
# Install the software properties common package
sudo apt install software-properties-common

# Add the GCC Repository
sudo add-apt-repository ppa:ubuntu-toolchain-r/test

# Install the desired GCC version
sudo apt install gcc-13 g++-13

# Configure the default GCC version
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 --slave /usr/bin/g++ g++ /usr/bin/g++-13

# Version check
gcc --version
```
* cmake 3.29.3
```
# Unistall
sudo apt remove cmake

# Go to directory
cd /opt/

# Download
wget https://cmake.org/files/v3.29/cmake-3.29.3-linux-aarch64.sh

# Make it executable
sudo chmod -x /opt/cmake-3.29.3-linux-aarch64.sh

# Installation
sudo bash /opt/cmake-3.29.3-linux-aarch64.sh --skip-license --exclude-subdir --prefix=/usr/local

# Version check
cmake --version
```