# ES-ESC-FW
Firmware for the EarthSense ESC Based on VESC4, edited from Benjamin Vedder's VESC firmware located here:
[https://github.com/vedderb/bldc](https://github.com/vedderb/bldc)

## Modifications from VESC Firmware
* Added support for AS5048B I2C encoder
* Removed ability to set CAN ID manually, they are set by two digital IO from the EarthSense System Board

## How to Build
* Download [ChibiStudio](https://www.chibios.org/dokuwiki/doku.php?id=chibios:products:chibistudio:start)
* Clone this repository
* Import this repo folder into ChibiStudio
  * File > New > Makefile Project with existing code
* Press Build Button at top and ensure you get a build success message once it finishes.

## How to Flash
* Either build using above method or get the approved binary for production from production manager
* Download [ST-Link Utility](https://www.st.com/en/development-tools/stsw-link004.html)
* Plug in ST-Link adapter to computer and connect it to a powered vesc
* Using ST-Link Utility, click Target > Program and verify
* Browse for the binary and upload
WARNING: Do not upload without browsing for the binary every time you open this window unless the file has not changed between uploads. The file gets cached and will not update to the new version just by overwriting the file on the filesystem.
