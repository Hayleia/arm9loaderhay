#pragma once

#include "common.h"

typedef void (*arm11Function)(void); 
void startArm11BackgroundProcess();
void setMode(u32 mode);
void changeBrightness(u32 _brightness);
bool screenInit();
void screenShutdown();

//default -1 arm11 done
typedef struct {
    vu32 a11ControllValue;
    vu32 a11threadRunning;
    vu32 version;
    vu32 brightness;
    vu32 fbTopLeft;
    vu32 fbTopRigth;
    vu32 fbBottom;
    vs32 setBrightness;
    vs32 enableLCD;
    vu32 mode;
    vs32 changeMode;
} a11Commands;