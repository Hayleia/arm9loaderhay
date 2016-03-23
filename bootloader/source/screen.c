#include "screen.h"
#include "i2c.h"
#include "constants.h"

//default -1 arm11 done
typedef struct {
    volatile u32 a11ControllValue;
    volatile u32 version;
    volatile u32 brightness;
    volatile int setBrightness;
    volatile int enableLCD;
} a11Commands;

//got code for disabeling from CakesForeveryWan
volatile u32 *a11_entry = (volatile u32 *)0x1FFFFFF8;
volatile a11Commands* arm11_commands=(volatile a11Commands*)ARM11COMMAND_ADDRESS;

void __attribute__((naked)) arm11BackgroundProcess();
void startArm11BackgroundProcess()
{
    if(arm11_commands->a11ControllValue!=0xDEADBEEF)
    {
        *a11_entry=(u32)arm11BackgroundProcess;
        for(volatile unsigned int i = 0; i < 0xF; ++i); 
    }
}

void changeBrightness(u32 _brightness)
{
    arm11_commands->brightness=_brightness;
    arm11_commands->setBrightness=1;
}

bool screenInit()
{
    //Check if it's a no-screen-init A9LH boot via PDN_GPU_CNT  
    if (*(u8*)0x10141200 == 0x1)
    {
        arm11_commands->enableLCD=1;
        for(volatile unsigned int i = 0; i < 0xF; ++i);
        i2cWriteRegister(3, 0x22, 0x2A); // 0x2A -> boot into firm with no backlight
        
        *(volatile u32*)0x80FFFC0 = FB_TOP_LEFT;    // framebuffer 1 top left
        *(volatile u32*)0x80FFFC4 = FB_TOP_LEFT;    // framebuffer 2 top left
        *(volatile u32*)0x80FFFC8 = FB_TOP_RIGHT;    // framebuffer 1 top right
        *(volatile u32*)0x80FFFCC = FB_TOP_RIGHT;    // framebuffer 2 top right
        *(volatile u32*)0x80FFFD0 = FB_BOTTOM;    // framebuffer 1 bottom
        *(volatile u32*)0x80FFFD4 = FB_BOTTOM;    // framebuffer 2 bottom
        *(volatile u32*)0x80FFFD8 = 1;    // framebuffer select top
        *(volatile u32*)0x80FFFDC = 1;    // framebuffer select bottom

        //cakehax  
        *(volatile u32*)0x23FFFE00 = FB_TOP_LEFT;
        *(volatile u32*)0x23FFFE04 = FB_TOP_RIGHT;
        *(volatile u32*)0x23FFFE08 = FB_BOTTOM;
        return true;
    }
    return false;
}

void screenShutdown()
{
    if(*(u8*)0x10141200 != 0x1)
    {
        arm11_commands->enableLCD=0;
        for(volatile unsigned int i = 0; i < 0xF; ++i);
    }
}

static inline void disable_lcds()  
{  
	*(volatile u32 *)0x10202A44 = 0;  
	*(volatile u32 *)0x10202244 = 0;  
	*(volatile u32 *)0x1020200C = 0;  
	*(volatile u32 *)0x10202014 = 0;  
} 

static inline void enable_lcd()
{

    *((volatile u32*)0x1FFFFFFD) = 0;
    *((volatile u32*)0x1FFFFFF8) = 0;
   
    // pdn sub_101D98
    *((volatile u32*)0x10141200) = 0x10000; // PDN_GPU_CNT
    // Delay here pls
    for(volatile int i = 0xC; i > 0; i-=2);
    *((volatile u32*)0x10141200) = 0x1007F; // PDN_GPU_CNT
   
    // gsp sub_1021F4 init_screen_maybe
    *((volatile u32*)0x10400004) |= 0x100;
    *((volatile u32*)0x10400030) &= 0xFFFFF0FF;
   
    // sub_107A34
    // Top screen
    *((volatile u32*)0x10400400) = 0x000001c2;
    *((volatile u32*)0x10400404) = 0x000000d1;
    *((volatile u32*)0x10400408) = 0x000001c1;
    *((volatile u32*)0x1040040c) = 0x000001c1;
    *((volatile u32*)0x10400410) = 0x00000000;
    *((volatile u32*)0x10400414) = 0x000000cf;
    *((volatile u32*)0x10400418) = 0x000000d1;
    *((volatile u32*)0x1040041c) = 0x01c501c1;
    *((volatile u32*)0x10400420) = 0x00010000;
    *((volatile u32*)0x10400424) = 0x0000019d;
    *((volatile u32*)0x10400428) = 0x00000002;
    *((volatile u32*)0x1040042c) = 0x00000192;
    *((volatile u32*)0x10400430) = 0x00000192;
    *((volatile u32*)0x10400434) = 0x00000192;
    *((volatile u32*)0x10400438) = 0x00000001;
    *((volatile u32*)0x1040043c) = 0x00000002;
    *((volatile u32*)0x10400440) = 0x01960192;
    *((volatile u32*)0x10400444) = 0x00000000;
    *((volatile u32*)0x10400448) = 0x00000000;
    *((volatile u32*)0x1040044c) = 0x0000ff00;
    *((volatile u32*)0x1040045c) = 0x019000f0;
    *((volatile u32*)0x10400460) = 0x01c100d1;
    *((volatile u32*)0x10400464) = 0x01920002;
    *((volatile u32*)0x10400490) = 0x000003c0;
    *((volatile u32*)0x1040049c) = 0x00000000;
    *((volatile u32*)0x10400468) = 0x18000000;
    *((volatile u32*)0x1040046c) = 0x18000000;
    *((volatile u32*)0x10400494) = 0x18000000;
    *((volatile u32*)0x10400498) = 0x18000000;
    *((volatile u32*)0x10400470) = 0x00080340;
    *((volatile u32*)0x10400480) = 0x00000000;
 
    for(int i = 0; i < 256; i++)
        *((volatile u32*)0x10400484) = 0x10101 * i;
   
    // Bottom screen
    *((volatile u32*)0x10400500) = 0x000001c2;
    *((volatile u32*)0x10400504) = 0x000000d1;
    *((volatile u32*)0x10400508) = 0x000001c1;
    *((volatile u32*)0x1040050c) = 0x000001c1;
    *((volatile u32*)0x10400510) = 0x000000cd;
    *((volatile u32*)0x10400514) = 0x000000cf;
    *((volatile u32*)0x10400518) = 0x000000d1;
    *((volatile u32*)0x1040051c) = 0x01c501c1;
    *((volatile u32*)0x10400520) = 0x00010000;
    *((volatile u32*)0x10400524) = 0x0000019d;
    *((volatile u32*)0x10400528) = 0x00000052;
    *((volatile u32*)0x1040052c) = 0x00000192;
    *((volatile u32*)0x10400530) = 0x00000192;
    *((volatile u32*)0x10400534) = 0x0000004f;
    *((volatile u32*)0x10400538) = 0x00000050;
    *((volatile u32*)0x1040053c) = 0x00000052;
    *((volatile u32*)0x10400540) = 0x01970193;
    *((volatile u32*)0x10400544) = 0x00000000;
    *((volatile u32*)0x10400548) = 0x00000011;
    *((volatile u32*)0x1040054c) = 0x000000ff;
    *((volatile u32*)0x1040055c) = 0x014000f0;
    *((volatile u32*)0x10400560) = 0x01c100d1;
    *((volatile u32*)0x10400564) = 0x01920052;
    *((volatile u32*)0x10400590) = 0x000003c0;
    *((volatile u32*)0x1040059c) = 0x00000000;
    *((volatile u32*)0x10400568) = 0x18000000;
    *((volatile u32*)0x1040056c) = 0x18000000;
    *((volatile u32*)0x10400594) = 0x18000000;
    *((volatile u32*)0x10400598) = 0x18000000;
    *((volatile u32*)0x10400570) = 0x00080301;
    *((volatile u32*)0x10400580) = 0x00000000;
   
    for(int i = 0; i < 256; i++)
        *((volatile u32*)0x10400584) = 0x10101 * i;

    *((volatile u32*)0x10400478) = 0x00000000;
    *((volatile u32*)0x10400578) = 0x00000000;
    *((volatile u32*)0x10400474) = 0x00010501;
    *((volatile u32*)0x10400574) = 0x00010501;

    // sub_10915C
    *((volatile u32*)0x10202014) = 0x00000001;
    *((volatile u32*)0x10202204) = 0x00000000; // color fill disable
    *((volatile u32*)0x10202a04) = 0x00000000; // color fill disable
    *((volatile u32*)0x1020200C) &= 0xFFFEFFFE;// wtf register
   
    *((volatile u32*)0x10202240) = ((a11Commands*)ARM11COMMAND_ADDRESS)->brightness;
    *((volatile u32*)0x10202244) = 0x1023E;
   
    *((volatile u32*)0x10202A40) = ((a11Commands*)ARM11COMMAND_ADDRESS)->brightness;
    *((volatile u32*)0x10202A44) = 0x1023E;
  
    // After hm call cmd 0x00160042 to acquire rights
    // and cmd 00130042 RegisterInterruptRelayQueue
    *((volatile u32*)0x10401000) = 0;
    *((volatile u32*)0x10401080) = 0x12345678;
    *((volatile u32*)0x104010C0) = 0xFFFFFFF0;
    *((volatile u32*)0x104010D0) = 1;
    *((volatile u32*)0x10400400) = 0x000001c2;
    *((volatile u32*)0x10400404) = 0x000000d1;
    *((volatile u32*)0x10400408) = 0x000001c1;
    *((volatile u32*)0x1040040c) = 0x000001c1;
    *((volatile u32*)0x10400410) = 0x00000000;
    *((volatile u32*)0x10400414) = 0x000000cf;
    *((volatile u32*)0x10400418) = 0x000000d1;
    *((volatile u32*)0x1040041c) = 0x01c501c1;
    *((volatile u32*)0x10400420) = 0x00010000;
    *((volatile u32*)0x10400424) = 0x0000019d;
    *((volatile u32*)0x10400428) = 0x00000002;
    *((volatile u32*)0x1040042c) = 0x00000192;
    *((volatile u32*)0x10400430) = 0x00000192;
    *((volatile u32*)0x10400434) = 0x00000192;
    *((volatile u32*)0x10400438) = 0x00000001;
    *((volatile u32*)0x1040043c) = 0x00000002;
    *((volatile u32*)0x10400440) = 0x01960192;
    *((volatile u32*)0x10400444) = 0x00000000;
    *((volatile u32*)0x10400448) = 0x00000000;
    *((volatile u32*)0x1040045c) = 0x019000f0;
    *((volatile u32*)0x10400460) = 0x01c100d1;
    *((volatile u32*)0x10400464) = 0x01920002;
    *((volatile u32*)0x10400470) = 0x00080340;
    *((volatile u32*)0x1040049C) = 0x00000000;
    // (122500 log)
    *((volatile u32*)0x10400500) = 0x000001c2;
    *((volatile u32*)0x10400504) = 0x000000d1;
    *((volatile u32*)0x10400508) = 0x000001c1;
    *((volatile u32*)0x1040050c) = 0x000001c1;
    *((volatile u32*)0x10400510) = 0x000000cd;//diff
    *((volatile u32*)0x10400514) = 0x000000cf;
    *((volatile u32*)0x10400518) = 0x000000d1;
    *((volatile u32*)0x1040051c) = 0x01c501c1;
    *((volatile u32*)0x10400520) = 0x00010000;
    *((volatile u32*)0x10400524) = 0x0000019d;
    *((volatile u32*)0x10400528) = 0x00000052;//diff
    *((volatile u32*)0x1040052c) = 0x00000192;
    *((volatile u32*)0x10400530) = 0x00000192;
    *((volatile u32*)0x10400534) = 0x0000004f;//diff
    *((volatile u32*)0x10400538) = 0x00000050;//diff
    *((volatile u32*)0x1040053c) = 0x00000052;//diff
    *((volatile u32*)0x10400540) = 0x01980194;//diff
    *((volatile u32*)0x10400544) = 0x00000000;
    *((volatile u32*)0x10400548) = 0x00000011;//diff
    *((volatile u32*)0x1040055c) = 0x014000f0;//diff
    *((volatile u32*)0x10400560) = 0x01c100d1;
    *((volatile u32*)0x10400564) = 0x01920052;//diff
    *((volatile u32*)0x1040059C) = 0x00000000;
    //(122860 log)
   
    *((volatile u32*)0x10400468) = FB_TOP_LEFT;
    *((volatile u32*)0x1040046c) = FB_TOP_LEFT;
    *((volatile u32*)0x10400494) = FB_TOP_RIGHT;
    *((volatile u32*)0x10400498) = FB_TOP_RIGHT;
   
    *((volatile u32*)0x10400568) = FB_BOTTOM;
    *((volatile u32*)0x1040056c) = FB_BOTTOM;
   
    *((volatile u32*)0x10400478) = 0x00000001;
    *((volatile u32*)0x10400578) = 0x00000001;
   
    // dma stuffs
    // skipped
   
    // called by hm
    *((volatile u32*)0x10400004) = 0x00070100;
   
    //(123030 log)
    *((volatile u32*)0x1040001C) &= 0xFFFFFFFD;
    *((volatile u32*)0x1040002C) &= 0xFFFFFFFD;
    *((volatile u32*)0x10400050) = 0x22221200;
    *((volatile u32*)0x10400054) = 0xFF2;
   
    //(123097 log)
    *((volatile u32*)0x10400474) = 0x00010501;
    *((volatile u32*)0x10400574) = 0x00010501;
   
    // from hm
    // command list skipped
    *((volatile u32*)0x10400470) = 0x00080341;
    *((volatile u32*)0x10400490) = 0x000002D0;
   
    *((volatile u32*)0x10400570) = 0x00080301;
    *((volatile u32*)0x10400590) = 0x000002D0;
   
    *((volatile u32*)0x10401000) = 0x00000000;
}

static inline void a11setBrightness()
{
    if(*(u8*)0x10141200 != 0x1)
    {
        *((volatile u32*)0x10202240) = ((a11Commands*)ARM11COMMAND_ADDRESS)->brightness;           
        *((volatile u32*)0x10202A40) = ((a11Commands*)ARM11COMMAND_ADDRESS)->brightness;
    }
} 

void __attribute__((naked)) arm11BackgroundProcess()
{
    *a11_entry = 0;  // Don't wait for us  
    a11Commands* arm11commands=(a11Commands*)ARM11COMMAND_ADDRESS;
    arm11commands->a11ControllValue=0xDEADBEEF;
    arm11commands->version=1;
    arm11commands->setBrightness=-1;
    arm11commands->brightness=DEFAULT_BRIGHTNESS;
    arm11commands->enableLCD=-1;
    while (!*a11_entry)
    {
        if(arm11commands->a11ControllValue==0xDEADBEEF)
        {
            if(arm11commands->setBrightness!=-1)
            {
                a11setBrightness();
                arm11commands->setBrightness=-1;
            }
            if(arm11commands->enableLCD!=-1)
            {
                if(arm11commands->enableLCD==0)
                    disable_lcds();
                if(arm11commands->enableLCD==1)
                    enable_lcd();
                arm11commands->enableLCD=-1;
            }
        }
    }
    arm11commands->a11ControllValue=0;
    ((void (*)())*a11_entry)();  
}
