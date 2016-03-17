#pragma once

#include "common.h"

typedef void (*arm11Function)(void); 
void setStartBrightness(u32 _brightness);
bool screenInit();
void screenShutdown();
void __attribute__((naked)) disable_lcds();
void __attribute__ ((naked)) enable_lcd();