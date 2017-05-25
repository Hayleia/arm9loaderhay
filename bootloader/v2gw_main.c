#include <stdio.h>
#include <stdlib.h>
#include "sdmmc.h"
#include "ff.h"
#include "hid.h"
#include "helpers.h"
#include "constants.h"

#define PAYLOAD_ADDRESS		0x23F00000
#define PAYLOAD_SIZE		0x00100000
#define A11_PAYLOAD_LOC     0x1FFF8000  //keep in mind this needs to be changed in the ld script for screen_init too

//used to detect if we are cold booting
#define CFG_BOOTENV			0x10010000
#define COLDBOOT			0

static inline void jump()
{
	((void (*)())PAYLOAD_ADDRESS)();
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
	u32 padInput;
	padInput = GetInput();

	FATFS fs;
	if(f_mount(&fs, "0:", 0) == FR_OK)
	{
		if (tryLoadFile("/luma/gateway.bin")) {
			/*
			// no dragon version
			if ((padInput & KEY_L)==0) { // if L is held, then we must see the menu
				setScreenState(false);
			}
			*/

			//gateway (BootCtr9)
			*(volatile uint32_t*)0x80FFFC0 = 0x18300000;	// framebuffer 1 top left
			*(volatile uint32_t*)0x80FFFC4 = 0x18300000;	// framebuffer 2 top left
			*(volatile uint32_t*)0x80FFFC8 = 0x18300000;	// framebuffer 1 top right
			*(volatile uint32_t*)0x80FFFCC = 0x18300000;	// framebuffer 2 top right
			*(volatile uint32_t*)0x80FFFD0 = 0x18346500;	// framebuffer 1 bottom
			*(volatile uint32_t*)0x80FFFD4 = 0x18346500;	// framebuffer 2 bottom
			*(volatile uint32_t*)0x80FFFD8 = 1;	// framebuffer select top
			*(volatile uint32_t*)0x80FFFDC = 1;	// framebuffer select bottom

			jump();
		}
	}
	return 0;
}
