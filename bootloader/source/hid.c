#include "hid.h"
#include "timer.h"

    	
u32 WaitTimeForInput(u32 waittime) {
    u32 pad_state = GetInput();

    u32 timerid=0;
    vu16 nextMS=TIMERFREQUENCY/1024/1000;
    vu16* timerValue=timerGetValueAddress(timerid);

    //starts timer and let it use the 1024 prescaler and the count up
    //more informations about the timer: https://www.3dbrew.org/wiki/TIMER_Registers#TIMER_CNT
    timerStart(timerid,PRESCALER_1024);
    *timerValue=0;

    for(vu32 i=0;i<waittime;i++)
    {   
        while(*timerValue < nextMS)
        {
            pad_state = GetInput();
            if (pad_state!=0)
            {
                i=waittime;
                *timerValue=nextMS;
            }
        }
        //start and stop it to make it work, dont know why its needed
        timerStop(timerid);
        *timerValue=0;
        timerStart(timerid,PRESCALER_1024);
    }
    //stop the timer
    timerStop(timerid);

    return pad_state;
}

u32 InputWait() {
    u32 pad_state_old = HID_STATE;
    while (true) {
        u32 pad_state = HID_STATE;
        if (pad_state ^ pad_state_old)
            return ~pad_state;
    }
}
u32 GetInput() { 
    u32 pad_state = HID_STATE; 
    
    return ~pad_state&0xFFF; 
}
