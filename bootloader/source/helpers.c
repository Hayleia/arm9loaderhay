#include "helpers.h"
#include "constants.h"
#include "screen.h"

/* File Helpers */

char workingDir[64]={0};


/* Screen Helpers */

void ClearScreen(unsigned char *screen, int color)
{
	int i=(SCREEN_HEIGHT * SCREEN_WIDTH);
	unsigned char *screenPos = screen;
	while(i--)
	{
		*(screenPos++) = color >> 16;  // B
		*(screenPos++) = color >> 8;   // G
		*(screenPos++) = color & 0xFF; // R
	}
}

void clearScreens()
{
	ClearScreen(TOP_SCREENL,0);
	ClearScreen(TOP_SCREENR,0);
	ClearScreen(BOT_SCREEN,0);
}

void setScreenState(bool enableScreen)
{
	if(enableScreen)
		initScreen();
	else
		shutdownScreen();
}

void initScreen()
{
	if(screenInit())
	{
		clearScreens();
	}
}

void shutdownScreen()
{
	screenShutdown();
}

void setBrightness(u8 brightness)
{
	changeBrightness(brightness);
}
