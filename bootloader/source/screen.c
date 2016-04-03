#include "screen.h"
#include "i2c.h"
#include "constants.h"

#include "../../arm11bg/source/arm11bg/constants.h"

extern u8 arm11bg_bin[];
extern u32 arm11bg_bin_size;
#define A11_PAYLOAD_LOC     0x1FFF4C80  //keep in mind this needs to be changed in the ld script for screen_init too

//default -1 arm11 done
typedef struct {
    vu32 a11ControllValue;
    vu32 version;
    vu32 brightness;
    vu32 fbTopLeft;
    vu32 fbTopRigth;
    vu32 fbBottom;
    volatile int setBrightness;
    volatile int enableLCD;
} a11Commands;

//got code for disabeling from CakesForeveryWan
volatile u32 *a11_entry = (volatile u32 *)0x1FFFFFF8;
volatile a11Commands* arm11_commands=(volatile a11Commands*)ARM11COMMAND_ADDRESS;

void __attribute__((naked)) arm11tmp()
{
    *a11_entry = 0;  // Don't wait for us  
    while (!*a11_entry);
    ((void (*)())*a11_entry)();  
}

void startArm11BackgroundProcess()
{
    /*if(arm11_commands->a11ControllValue!=0xDEADBEEF)
    {*/
        *a11_entry=(u32)arm11tmp;
        for(volatile unsigned int i = 0; i < 0xF; ++i); 
        memcpy((void*)A11_PAYLOAD_LOC, arm11bg_bin, arm11bg_bin_size);
        *a11_entry = (u32)A11_PAYLOAD_LOC;
        for(volatile unsigned int i = 0; i < 0xF; ++i); 
        
    //}
}

void changeBrightness(u32 _brightness)
{
    if(arm11_commands->a11ControllValue!=0xDEADBEEF)
            startArm11BackgroundProcess();
    arm11_commands->brightness=_brightness;
    arm11_commands->setBrightness=1;
}

bool screenInit()
{
    //Check if it's a no-screen-init A9LH boot via PDN_GPU_CNT  
    if (*(u8*)0x10141200 == 0x1)
    {
        if(arm11_commands->a11ControllValue!=0xDEADBEEF)
            startArm11BackgroundProcess();
        arm11_commands->enableLCD=1;
        for(volatile unsigned int i = 0; i < 0xF; ++i);
        i2cWriteRegister(3, 0x22, 0x2A); // 0x2A -> boot into firm with no backlight
        
        *(volatile u32*)0x80FFFC0 = arm11_commands->fbTopLeft;    // framebuffer 1 top left
        *(volatile u32*)0x80FFFC4 = arm11_commands->fbTopLeft;    // framebuffer 2 top left
        *(volatile u32*)0x80FFFC8 = arm11_commands->fbTopRigth;    // framebuffer 1 top right
        *(volatile u32*)0x80FFFCC = arm11_commands->fbTopRigth;    // framebuffer 2 top right
        *(volatile u32*)0x80FFFD0 = arm11_commands->fbBottom;    // framebuffer 1 bottom
        *(volatile u32*)0x80FFFD4 = arm11_commands->fbBottom;    // framebuffer 2 bottom
        *(volatile u32*)0x80FFFD8 = 1;    // framebuffer select top
        *(volatile u32*)0x80FFFDC = 1;    // framebuffer select bottom

        //cakehax  
        *(volatile u32*)0x23FFFE00 = arm11_commands->fbTopLeft;
        *(volatile u32*)0x23FFFE04 = arm11_commands->fbTopRigth;
        *(volatile u32*)0x23FFFE08 = arm11_commands->fbBottom;
        return true;
    }
    return false;
}

void screenShutdown()
{
    if(*(u8*)0x10141200 != 0x1)
    {
        if(arm11_commands->a11ControllValue!=0xDEADBEEF)
            startArm11BackgroundProcess();
        arm11_commands->enableLCD=0;
        for(volatile unsigned int i = 0; i < 0xF; ++i);
    }
}
