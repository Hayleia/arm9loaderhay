#include "timer.h"

vu16* timerGetValueAddress(u32 timer);
u32 timerStart(u32 timer, u32 prescaler);
u32 timerStop(u32 timer);

vu16* timerGetConfigAddress(u32 timer)
{
	if(timer<TIMERCOUNT)
	{
		return (vu16*)TIMERFIRSTCONFIG + 4*timer;
	}
	return 0;
}

vu16* timerGetValueAddress(u32 timer)
{
	if(timer<TIMERCOUNT)
	{
		return (vu16*)TIMERFIRSTVALUE + 4*timer;
	}
	return 0;
}

u32 timerStart(u32 timer, u32 prescaler)
{
	vu16* timeraddress=timerGetConfigAddress(timer);
	if(timeraddress!=0)
	{
		u16 configValue=0b10000100|prescaler;
		*timeraddress=configValue;
		return 0;
	}
	else
		return 1;
}

u32 timerStop(u32 timer)
{
	vu16* timeraddress=timerGetConfigAddress(timer);
	if(timeraddress!=0)
	{
		*timeraddress=0;
		return 0;
	}
	else
		return 1;
}