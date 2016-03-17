#pragma once

#include "common.h"
#include "config.h"
#include "../source/fatfs/sdmmc/sdmmc.h"
#include "../source/fatfs/ff.h"

bool file_exists(const char* path);
void getWorkingDirectory(char* str);
void checkFolders(const char* fileName, char* str);

void setScreenState(bool enableScreen);
void initScreen();
void shutdownScreen();
void setBrightness(u8 brightness);

bool openIniFile(FIL* file);
int iniparse(ini_handler handler, void* user, FIL* file);

bool isColdboot();
