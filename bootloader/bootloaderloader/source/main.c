#include "common.h"
#include "i2c.h"

#define BOOTLOADER_PAYLOAD_ADDRESS	0x24F00000
#define PAYLOAD_SIZE		0x00100000

extern u8 arm9bootloader_bin[];
extern u32 arm9bootloader_bin_size;

void fastmempy(u32* destination, u32* source, u32 size)
{
	size/=4;
	while(size--)
	{
		*destination=*source;
		destination++;
		source++;
	}
}

int main()
{
	fastmempy((u32*)BOOTLOADER_PAYLOAD_ADDRESS,(u32*)arm9bootloader_bin, arm9bootloader_bin_size);
	((void (*)())BOOTLOADER_PAYLOAD_ADDRESS)();

	i2cWriteRegister(I2C_DEV_MCU, 0x20, (u8)(1<<0));
    return 0;
}
