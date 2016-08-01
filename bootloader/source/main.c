#include <stdio.h>
#include <stdlib.h>
#include "sdmmc.h"
#include "ff.h"
#include "hid.h"
#include "helpers.h"
#include "screen.h"

#define PAYLOAD_ADDRESS		0x23F00000
#define PAYLOAD_SIZE		0x00100000
#define A11_PAYLOAD_LOC		0x1FFF8000  //keep in mind this needs to be changed in the ld script for screen_init too

//used to detect if we are cold booting
#define CFG_BOOTENV			0x10010000
#define COLDBOOT			0

static inline void jumpAndSetBrightness(int brightness)
{
	setBrightness(brightness);
	((void (*)())PAYLOAD_ADDRESS)();
}

int shouldEnableBL(char *file)
{
	FIL f;
	if(f_open(&f, file, FA_READ | FA_OPEN_EXISTING) == FR_OK) {
		f_close(&f);
		return 1;
	}
	return 0;
}

int tryLoadFile(char *file)
{
	FIL f;
	UINT br;
	if(f_open(&f, file, FA_READ | FA_OPEN_EXISTING) == FR_OK)
	{
		f_read(&f, (void*)PAYLOAD_ADDRESS, PAYLOAD_SIZE, &br);
		f_close(&f);
		return 1;
	}
	return 0;
}

int main()
{
	setBrightness(0);
	setScreenState(true);

	FATFS fs;
	if(f_mount(&fs, "0:", 0) == FR_OK)
	{
		char ch[] = "ABETrludRLXY";

		//Read pad state
		u16 padInput = (~(*(u16*)0x10146000)) & 0x0FFF;
		FIL f;

		//Deduce base filename
		u16 N = padInput;
		char base[] = "/arm9loaderhay/------------/arm9loaderhax.bin";
		char back[] = "/arm9loaderhay/------------/backlight";
		char pass[] = "/arm9loaderhay/------------/password";
		char spt1[] = "/arm9loaderhay/------------/splash_top_1";
		char spt2[] = "/arm9loaderhay/------------/splash_top_2";

		for (int i=0; i<12; i++) {
			if (N%2) {
				base[26-i] = ch[i];
				back[26-i] = ch[i];
				pass[26-i] = ch[i];
				spt1[26-i] = ch[i];
				spt2[26-i] = ch[i];
			}
			N = N/2;
		}

		if (splash_image(spt1)==0) setBrightness(0x44);

		//Password if needed
		if (f_open(&f, pass, FA_READ | FA_OPEN_EXISTING) == FR_OK) {
			//u32 password[] = {BUTTON_LEFT, BUTTON_RIGHT, BUTTON_DOWN, BUTTON_UP, BUTTON_A, 0};

			u32 password[11]; //password can't be longer than 10, because a max is reasonably needed and no one will go over 10...
			for (int i=0; i<11; i++) password[i] = 0;
			char read[10];
			UINT pass_length;
			f_read(&f, read, 10, &pass_length);
			f_close(&f);

			//convert char like 'A' or 'B' into u32 like 0001 or 0010...
			for (int i=0; i<pass_length; i++) {
				char cur = read[i];
				int index = -1;
				for (int j=0; j<12; j++) if (cur == ch[j]) index = j;
				if (index != -1) password[i] = 1<<index;
			}

			int n = 0;
			int miss = 0;
			u32 pad = 0;
			u32 oldPad;
			while (password[n] != 0) {
				oldPad = pad;
				while (pad == 0 || pad == oldPad) pad = InputWait() & 0x0FFF;

				if (password[n] == pad) {
					n++;
				} else {
					miss++;
					n = 0;
				}
			}
		}

		int brightness = 0; // so if there is no splash and no "backlight", backlight is off
		if (splash_image(spt2)==0) brightness = 0x44;

		if (tryLoadFile(base)) {
			if (shouldEnableBL(back)) brightness = 0x44;
			jumpAndSetBrightness(brightness);
		} else if (tryLoadFile("/arm9loaderhay/default.bin")) {
			if(*(vu8*)CFG_BOOTENV == COLDBOOT) {
				if (shouldEnableBL("/arm9loaderhay/default_bl")) brightness = 0x44;
				jumpAndSetBrightness(brightness);
			} else { //dont enable backlight again on soft reset
				jumpAndSetBrightness(brightness);
			}
		}
	}
	return 0;
}
