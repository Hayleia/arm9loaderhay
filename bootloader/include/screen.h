#include "common.h"

typedef void (*arm11Function)(void); 
void setStartBrightness(u32 _brightness);
void screenInit();
void screenDeinit();
void __attribute__((naked)) disable_lcds();
void __attribute__ ((naked)) enable_lcd();