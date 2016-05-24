#include "splash.h"
#include "draw.h"
#include "log.h"
#include "helpers.h"
#include "sdmmc.h"
#include "ff.h"
#include "constants.h"
#include "hid.h"
#include "timer.h"

#include "screen.h"
#include "../../arm11bg/source/arm11bg/constants.h"

// Define A11 Commands Struct
ARM11_COMMANDS

bool drawBootSplash(loaderConfiguration* loaderConfig)
{
    if(!isColdboot()&&!loaderConfig->enableSoftbootSplash)
        return false;

    if(loaderConfig->bootsplash)
    {
        switch(loaderConfig->bootsplash)
        {/* 0 - disabled, 1 - splash screen, 2 - entry info, 3 - both */ 
            case 1:
            case 3:
                if(splash_image(loaderConfig->bootsplash_image)!=0)
                    splash_ascii(NULL);
                break;
            case 4:
                if(splash_anim(loaderConfig->bootsplash_image)!=0)
                    splash_ascii(NULL);
                break;
            case 2:
                splash_ascii(NULL);
                break;
        }

        debug("Splash sucessfully loaded");
        return true;
    }
    else
        return false;
}

bool drawSplash(configuration* app)
{
    if(!isColdboot()&&!app->enableSoftbootSplash)
        return false;
    
    if(app->splash)
    {
        switch(app->splash)
        {/* 0 - disabled, 1 - splash screen, 2 - entry info, 3 - both */ 
            case 1:
                if(splash_image(app->splash_image)!=0)
                    splash_ascii(NULL);
                break;
            case 2:
                splash_ascii(app->path);
                break;
            case 3:
                if(splash_image(app->splash_image)!=0)
                    splash_ascii(app->path);
                break;
            case 4:
                if(splash_anim(app->splash_image)!=0)
                    splash_ascii(app->path);
                break;
        }
        if(app->splash!=4)
        {
            debug("Showing Splash for %llu ms",app->splashDelay);
            for(volatile u64 i=0;i<0xEFF*app->splashDelay;i++);
        }
        return true;
    }
    else
        return false;
}

int splash_ascii(const char* payloadName)
{
    // print BootCtr9 logo
    // http://patorjk.com/software/taag/#p=display&f=Bigfig&t=BootCtr
    ClearScreen(TOP_SCREENL,0);
    ClearScreen(TOP_SCREENR,0);
    if(payloadName!=NULL)
    {
        if(strlen(payloadName)>1)
        {
            DrawStringF(5,5,ASCII_ART_TEMPLATE_EXTENDET, VERSION_STRING, payloadName);
            return 0;
        }
    }
    DrawStringF(5,5,ASCII_ART_TEMPLATE, VERSION_STRING);
    return 0;
}

int splash_image(char *splash_path)
{
    // load image in memory, doing proper error checking
    FIL splash_file;
    unsigned int br;
    if(!strlen(splash_path))
    {
        debug("Splash image not set, use default screen");
        return -1;
    }
    if(f_open(&splash_file, splash_path, FA_READ | FA_OPEN_EXISTING) != FR_OK)
    {
        debug("Couldn't open splash image %s.", splash_path);
        return -1;
    }

    //load splash to templocation in memory to prevent visible drawing
    f_read(&splash_file, (void*)(TMPSPLASHADDRESS), 0x00600000, &br);

    // copy splash image to framebuffers(in case 3d is enabled)
    memcpy((void*)TOP_SCREENL,(void*)TMPSPLASHADDRESS,br);
    memcpy((void*)TOP_SCREENR,(void*)TMPSPLASHADDRESS,br);

    return 0;    
}

int splash_anim(char *splash_path)
{
    u8 frameRate= 0x0F;
    u32 topAnimationFileSize=0;
    u32 frameRateConfigurationFileSize=0;
    char frameRateConfigurationPath[64]={0};

    if(!strlen(splash_path))
    {
        debug("Splash image not set, using default screen instead");
        return -1;
    }

    topAnimationFileSize=getFileSize(splash_path);
    if(topAnimationFileSize<TOP_FB_SIZE)
    {
        debug("Bootanimation includes less than one Frame\n, using default screen instead");
        return -1;
    }

    FIL FrameRateConfigurationFile;
    FIL topScreenAnimationFile;
    unsigned int br;

    sprintf(frameRateConfigurationPath,"%s.cfg",splash_path);
    frameRateConfigurationFileSize=getFileSize(frameRateConfigurationPath);
    if(frameRateConfigurationFileSize)
    {
        f_open(&FrameRateConfigurationFile, frameRateConfigurationPath, FA_READ);
        f_read(&FrameRateConfigurationFile, &frameRate, 1, &br);
        f_close(&FrameRateConfigurationFile);

        if(!frameRate)
            frameRate=0x0F;
    }

    u32 topScreenFrames=topAnimationFileSize / TOP_FB_SIZE;
    
    f_open(&topScreenAnimationFile, splash_path, FA_READ);

    ClearScreen(TOP_SCREENL,0);
    ClearScreen(TOP_SCREENR,0);

    //set arm11 thread to draw mode to let it write the readed Framebuffers
    setMode(MODE_DRAW);
    a11Commands* arm11_commands=(a11Commands*)ARM11COMMAND_ADDRESS;

    // calculatet number of Timer cicles per second using the 1024 Prescaler
    // and divide it throug framerate to get the timer Value for the next frame
    u32 nextFrameTimerValue=TIMERFREQUENCY/1024/frameRate;
    vu16* timerValue=timerGetValueAddress(0);

    *timerValue=0;
    //starts timer and let it use the 1024 prescaler and the count up
    //more informations about the timer: https://www.3dbrew.org/wiki/TIMER_Registers#TIMER_CNT
    timerStart(0,PRESCALER_1024);
    u32 frame = topScreenFrames;
    while(frame--) 
    {   
        if (GetInput() == (KEY_SELECT|KEY_START))
            frame = 0;

        //Read to temporary buffer , wait for the next frame and let arm11 write it to the Frame buffer to minimize Tearing
        f_read(&topScreenAnimationFile, (void*)TMPSPLASHADDRESS, TOP_FB_SIZE, &br);
        
        while(*timerValue<nextFrameTimerValue);
        *timerValue=0;
        
        arm11_commands->fbTopLeft=TMPSPLASHADDRESS;

    }
    timerStop(0);

    f_close(&topScreenAnimationFile);

    //set arm11 mode back to the main mode
    setMode(MODE_MAIN);

    return 0;
}
