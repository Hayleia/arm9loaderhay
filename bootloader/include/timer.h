#pragma once

#include "common.h"

#define PRESCALER_1 0
#define PRESCALER_64 1
#define PRESCALER_256 2
#define PRESCALER_1024 3

#define TIMERCOUNT 4
#define TIMERFREQUENCY 67027964

#define TIMERFIRSTVALUE 0x10003000
#define TIMERFIRSTCONFIG 0x10003002

vu16* timerGetConfigAddress(u32 timer);
vu16* timerGetValueAddress(u32 timer);
u32 timerStart(u32 timer, u32 prescaler);
u32 timerStop(u32 timer);