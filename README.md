# arm9loaderhay - Arm9LoaderHax with bootloader for Nintendo 3DS


## Explanation

This is a bootloader that is much less convenient than others but that's exactly the goal actually.
This allows you to have a configuration you know but without random people being able to land in GodMode9 by chance and wipe your whole sysNAND.
If you aren't paranoid, this most likely isn't for you, you can use more convenient alternatives.

It works on both New and OLD 3DS.

There is also stuff about arm9loaderhax here since it was forked from BootCtr9 but I didn't touch any of this, so if you're interested in what it does and how to use it, please check out BootCtr9.

## Usage

Put the arm9loaderhax.bin file at the root of your SD card.
Create an arm9loaderhay folder at the root of your SD card.

The arm9loaderhay folder can contain a default.bin file, which would be the default payload, and a default_bl file that indicates if the default payload needs backlighting.

Then, the arm9loaderhay folder can contain subfolders with the base name YXLRdulrTEBA. Replace any of these characters with "-" to get a valid name. The name of the folder describes the pad state to consider booting the payload in that folder.
The meaning of YXLRBA is obvious. The D-Pad is represented with dulr (in lowercase to avoid conflict with LR) and TE is for START and SELECT (they both start with an S so they are described with their second letter).
For example, "----------BA" and "--L--u------" are valid names, describing "B+A" and "L+Up".
Each of these folders
* needs an arm9loaderhax.bin file which will obviously be the payload to be launched when the pad state matches the folder's name
* can contain a "backlight" file (with any content) that indicates if that payload needs backlight or not
* can contain a file named "password" that contains the password for the present payload. Use the same convention as for the folder's name. For example, if your password is Left,Right,L,START, write lrLT. Notice that any password longer than 10 characters will be trimmed down to 10. Also notice that any unrecognized character will mark the end or your password. "ABXYwatLR" for exemple is equivalent to "AXBY". The last thing to notice is that you can't use the same key twice in a row, so ABA is valid but AAB isn't.
* The folder can contain anything else, like a file named "CtrBootManager" if you want to remind yourself that the "arm9loaderhax.bin" payload present in that folder is CtrBootManager's

## Credits

*Copyright 2016, Jason Dellaluce*


*Licensed under GPLv2 or any later version, refer to the license.txt file included.*

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
