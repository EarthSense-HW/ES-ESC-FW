# VESC firmware

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Travis CI Status](https://travis-ci.com/vedderb/bldc.svg?branch=master)](https://travis-ci.com/vedderb/bldc)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/75e90ffbd46841a3a7be2a9f7a94c242)](https://www.codacy.com/app/vedderb/bldc?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=vedderb/bldc&amp;utm_campaign=Badge_Grade)
[![Contributors](https://img.shields.io/github/contributors/vedderb/bldc.svg)](https://github.com/vedderb/bldc/graphs/contributors)
[![Watchers](https://img.shields.io/github/watchers/vedderb/bldc.svg)](https://github.com/vedderb/bldc/watchers)
[![Stars](https://img.shields.io/github/stars/vedderb/bldc.svg)](https://github.com/vedderb/bldc/stargazers)
[![Forks](https://img.shields.io/github/forks/vedderb/bldc.svg)](https://github.com/vedderb/bldc/network/members)

An open source motor controller firmware.

This is the source code for the VESC DC/BLDC/FOC controller. Read more at
[https://vesc-project.com/](https://vesc-project.com/)

## Prerequisites

### On Ubuntu

Install the gcc-arm-embedded toolchain. Recommended version ```gcc-arm-none-eabi-7-2018-q2```  

**Method 1 - Through Official GNU Arm Embedded Toolchain Downloads**
1. Go to [GNU Arm Embedded Toolchain Downloads](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
2. Locate and Download version **gcc-arm-none-eabi-7-2018-q2** for your machine  
   ```GNU Arm Embedded Toolchain: 7-2018-q2-update     June 27, 2018```  
   Linux 64-bit version can be downloaded from [here](https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2?revision=bc2c96c0-14b5-4bb4-9f18-bceb4050fee7?product=GNU%20Arm%20Embedded%20Toolchain,64-bit,,Linux,7-2018-q2-update)  
3. Unpack the archive in the file manager by right-clicking on it and select "extract here"
4. Change directory to the unpacked folder, unpack it in /usr/local by execute the following command
   ```
   cd gcc-arm-none-eabi-7-2018-q2-update-linux  
   sudo cp -RT gcc-arm-none-eabi-7-2018-q2-update/ /usr/local  
   ```

**Method 2 - Through apt install**
```bash
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt update
sudo apt install gcc-arm-embedded
```


**Optional - Add udev rules to use the stlink v2 programmer without being root**
```bash
wget vedder.se/Temp/49-stlinkv2.rules
sudo mv 49-stlinkv2.rules /etc/udev/rules.d/
sudo udevadm trigger
```

## Build
Clone and build the firmware

```bash
git clone https://github.com/vedderb/bldc.git vesc_firmware
cd vesc_firmware
make
```
