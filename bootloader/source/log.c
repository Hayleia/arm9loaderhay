#include "log.h"
#include "draw.h"

#include "fatfs/ff.h"
#include "i2c.h"
#include "hid.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define LOGNAME "/arm9loaderhax/arm9bootloader.log"
#define LOGNAME_BACKUP "/arm9bootloader.log"

static FIL logFile;
bool fileLoggingEnabled		= false;
bool screenLoggingEnabled	= false;

int initLog(bool _fileLoggingEnabled, bool _screenLoggingEnabled)
{
	screenLoggingEnabled = _screenLoggingEnabled;
	if(_fileLoggingEnabled)
		openLogFile();
    
    return 0;
}

void openLogFile()
{	
	if (f_open(&logFile, LOGNAME, FA_READ | FA_WRITE | FA_CREATE_ALWAYS ) != FR_OK)
    {
    	if (f_open(&logFile, LOGNAME_BACKUP, FA_READ | FA_WRITE | FA_CREATE_ALWAYS ) != FR_OK)
    		return;
    }           
	f_puts ("opened logfile\n", &logFile);
    f_sync (&logFile);  
    fileLoggingEnabled=true;
}


void closeLogFile()
{	
	if(fileLoggingEnabled==true)
	{
		f_close(&logFile);
		fileLoggingEnabled=false;
	}
}

void logToFile(char *msg)
{
	if(fileLoggingEnabled)
	{
	    f_puts (msg, &logFile);
	    f_putc ('\n', &logFile);
	    f_sync (&logFile);
	}
}

void logToScreen(char *msg)
{
	if(screenLoggingEnabled)
	{
		drawDebug(msg);
	}
}

void debug(const char *format, ...)
{
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);

    logToFile(str);
    logToScreen(str);
}

void panic(const char *format, ...)
{
	char str[256];
    va_list va;
    screenLoggingEnabled=true;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);


	debug("ERROR: %s",str);
    logToScreen("Press any key to shutdown");
	InputWait();
    shutdown();
}

void shutdown() {
	debug("shutdown");
	closeLogFile();
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (u8)(1<<0));
	while(1);
}