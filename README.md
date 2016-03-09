# BootCTR9 - arm9loaderhax with bootloader for 3DS

## What this is

This is my port of BootCTR9 to arm9loaderhax. There are two way to start it, the modded version of arm9loaderhax, or the bootloader loader arm9loaderhax.bin.
The arm9loaderhaxmod is based on **delibles** implementation of the arm9loaderhax exploit, documented [here](http://3dbrew.org/wiki/3DS_System_Flaws) and also presented [in this conference](https://media.ccc.de/v/32c3-7240-console_hacking), which provides ARM9 code execution directly at the console boot, exploiting a vulnerability present in 9.6+ version of New3DS arm9loader.


It works on both New and OLD 3DS.

The arm9loaderhax exploit was found by **plutoo** and **yellows8**, i do not own the idea.
The original [BootCTR](https://github.com/m45t3r/BootCtr) was developed by **m45t3r**

## Usage

### Arm9loaderhax mod

While also having support to load a "arm9loaderhax.bin" as fallback, it normaly loads the "arm9bootloader.bin" from you SDs "arm9loaderhax" folder, or directly out of your root directory. It will try it in the following order:
"arm9loaderhax/arm9bootloader.bin" -> "arm9bootloader.bin" -> "arm9loaderhax.bin"

The **arm9bootloader.bin** will be loaded to address 0x24F00000, while the **arm9loaderhax.bin** arm9 payload will be loaded at address 0x23F00000.

This means that it offers a BRAHMA-like setup, and as such has compatibility with every payload BRAHMA can run; you can also perform a firmlaunch by writing the ARM11 kernel entrypoint at address 0x1FFFFFF8.

Screen initialization was implemented by [**dark-samus**' pull request](https://github.com/delebile/arm9loaderhax/pull/9) (thanks!).

### Bootloader loader

This version is for all people who installed the normal arm9loaderhax. Its an arm9loaderhax.bin which is doing the same as the arm9loaderhax mod, exept it doesn't support the arm9loaderhax.bin fallback loading(since this would result in a recursiv try to load itself) and doesn't do the screen initialization. Simply copy it to the root of your sd-card and it will be usable

### BootCTR9

After you set up one of the above loaders, you can start setting up BootCTR9. For this simply copy the "arm9bootloader.bin" and a "boot_config.ini" to your sd-cards "arm9loaderhax" or root folder.
While beeing backwardcompatible to BootCTRs "boot_config.ini", some additional settings got added.
These are the following for the payload configurations:
* screenEnabled
    * This configures, if the screen shoud be enabled before jumping to the payload(default is 1)
    * If its set to 0 while the screen was initalized before, the screen will be disabled before jumping to the payload

Also there is a new configuration section named "BOOTCTR9".
This section includes some BootCTR9 only configurations:
* key_delay
    * This sets the time in ms BootCTR9 waits for a key
* boot_splash
    * This defines the splash type of the bootsplash(showen while waiting for the key)
        * 0 -> Disabled
        * 1 -> Splash Image
        * 2 -> Ascii Splash
* boot_splash_image
    * This defines the path, with filename, of the bootsplash.
* fileLog
    * This configure if the logging to a log file is enabled
        * 0 -> disabled
        * 1 -> enabled
* screenLog
    * This is the same as fileLog, but the log is showen on the bottom screen, instead of written to a file
* screenEnabled
    * This configures if the screen should be enabled directly at boot


## How do I update it?

An updater is currently in developement. You can test [**dark-samus' one**](https://gbatemp.net/attachments/installer-zip.40329/), but it's currently unfinished and unstable. 

## Installation arm9loaderhax mod

The setup will provide a 3dsx installer that will work on system version 9.0-9.2, which should make the things easier.

If, for any reason, you desire to inject the files manually remember to:

* Inject **firm0.bin** in the NAND FIRM0 partition, encrypted with your xor stream.
* Inject **firm1.bin** in the NAND FIRM1 partition, encrypted with your xor stream.
* Inject **sector.bin** at sector 0x96, in the plaintext output you'll have.
* Inject **stage0x5C000.bin** at sector 0x5C000, in his plaintext form. It is an unused region for now, you'll not ruin your nand.


## Setup

The setup is absolutely user-unfriendly because of the files that needs to be provided in compilation.

In order to make this work you need to creat a directory in the setup root named **data_input**, and put the following files in it :

* **new3ds10.firm** : New 3DS NATIVE_FIRM from system version 10.2. You can dump and decrypt it from both the CDN, NAND, the proper **0004013820000002.cia**, or in any way get it leaked.


	SHA-256 : d253c1cc0a5ffac6b383dac1827cfb3b2d3d566c6a1a8e5254e389c2950623e5
     
* **new3ds90.firm** : Same as before, but for New 3DS NATIVE_FIRM from system version 9.0-9.2.


	SHA-256 : d7be76e1813f398dcea85572d0c058f7954761a1d5ea03b5eb5047ac63ac5d6b

* **secret_sector.bin** : The New 3DS secret 0x96 sector. Many ways to have it has been made public, if you are not good enough to dump it yourself (or you use an OLD 3DS) you can always search for it in the net.


	SHA-256 : 82f2730d2c2da3f30165f987fdccac5cbab24b4e5f65c981cd7be6f438e6d9d3
* **otp.bin** : A dump of your console OTP data from region 0x10012000-0x10012100; in order to dump it you must downgrade to a system version below 3.0, or exploit the New3DS-only vulnerability (This will just give you the hash of it, so you'll need to change the python script).
**it is console unique, and as such it cannot be shared from other people, because it will cause a brick!**


Once you have all of the required files, the compilation needs:

* devkitARM r45
* Python 2.7 with pycrypto
* libctru at least on v1.0.0

## Credits

Copyright 2016, Jason Dellaluce

sdmmc.c & sdmmc.h originally written by Normatt

Licensed under GPLv2 or any later version, refer to the license.txt file included.

* Smealum and contributors for libctru
* Normmatt for sdmmc.c and .h, and also for .ld files and the log from XDS by ichfly that provided us some of the information needed to get screen init
* Christophe Devine for the SHA codes
* Archshift for i2c.c and .h
* Megazig for crypto.c and .h
* Patois for original BRAHMA code
* Smealum, Derrek, Plutoo for publishing the exploit
* Yellows8 and Plutoo as ideators of it
* [3dbrew community](http://3dbrew.org/)
* bilis/b1l1s for his screen init code, and work on inegrating it into stage 2
* dark_samus for work on integrating screen init into stage 2
