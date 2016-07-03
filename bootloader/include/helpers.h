#pragma once

#include "common.h"
#include "sdmmc.h"
#include "ff.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 400

void ClearScreen(unsigned char *screen, int color);
void clearScreens();
void setScreenState(bool enableScreen);
void initScreen();
void shutdownScreen();
void setBrightness(u8 brightness);
