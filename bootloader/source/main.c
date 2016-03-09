#include <stdio.h>
#include <stdlib.h>
#include "fatfs/sdmmc/sdmmc.h"
#include "fatfs/ff.h"
#include "config.h"
#include "hid.h"
#include "log.h"
#include "i2c.h"
#include "screen.h"
#include "draw.h"
#include "splash.h"

#define DEFAULT_SECTION 	"GLOBAL"
#define DEFAULT_PATH 		{0}
#define DEFAULT_KEYDELAY	100 /* ms */
#define DEFAULT_PAYLOAD 	-1 /* <0 - auto, 0 - disable, >0 - enabled */
#define DEFAULT_OFFSET 		0x12000
#define DEFAULT_SPLASH 		0 /* 0 - disabled, 1 - splash screen, 2 - entry info, 3 - both */ 
#define DEFAULT_SPLASH_IMAGE {0} 
#define DEFAULT_SCREEN 		1

#define LOADER_SECTION 			"BOOTCTR9"
#define DEFAULT_SPLASHDELAY 	0
#define DEFAULT_SCREEN_LOADER	1 /* 0 - use payloadConfiguration in the end, 1 - screen init at start */
#define DEFAULT_LOG_FILE 		0
#define DEFAULT_LOG_SCREEN 		0
#define DEFAULT_BOOTSPLASH 		0 /* 0 - disabled, 1 - splash screen, 2 - asciiSplash */ 
#define DEFAULT_BOOTSPLASH_IMAGE {0} 
#define DEFAULT_BRIGHTNESS		0xFF

#define INI_FILE 			"/arm9loaderhax/boot_config.ini"
#define INI_FILE_BOOTCTR 	"/boot_config.ini"


#define PAYLOAD_ADDRESS		0x23F00000
#define PAYLOAD_SIZE		0x00100000

bool file_exists(const char* path) { 
    FIL fd;
    if(f_open(&fd, path, FA_READ | FA_OPEN_EXISTING) == FR_OK) { 
       f_close(&fd); 
       return true; 
    } 
 	return false; 
} 

bool openIniFile(const char* filename, FIL* file)
{
	if(file_exists(filename))
    {
    	if (f_open(file, filename, FA_READ | FA_OPEN_EXISTING) == FR_OK)
        	return true;
    }
    return false;
}

int iniparse(const char* filename, ini_handler handler, void* user)
{
    FIL file;
    int error;
    if(!openIniFile(INI_FILE,&file))
    {
    	if(!openIniFile(INI_FILE_BOOTCTR,&file))
        	return -1; 
    }
    
    f_lseek(&file,0);
    error = ini_parse_stream((ini_reader)f_gets, &file, handler, user);
    f_close(&file);
    return error;
}

void initScreen(bool enableScreen)
{
	if(enableScreen)
	{
		debug("Enabeling screen");
		screenInit();
		clearScreens();
		debug("ScreensEnabled");
	}
}

int main() {
	unsigned int br;
	configuration app =  {
	        .section = DEFAULT_SECTION,
	        .path = DEFAULT_PATH,
	        .splashDelay = DEFAULT_SPLASHDELAY,
	        .payload = DEFAULT_PAYLOAD,
	        .offset = DEFAULT_OFFSET,
	        .splash = DEFAULT_SPLASH, 
            .splash_image = DEFAULT_SPLASH_IMAGE,
	        .screenEnabled = DEFAULT_SCREEN,
    };
    loaderConfiguration loader =  {
	        .section = LOADER_SECTION,
	        .keyDelay = DEFAULT_KEYDELAY,
	        .bootsplash = DEFAULT_BOOTSPLASH, 
            .bootsplash_image = DEFAULT_BOOTSPLASH_IMAGE,
            .fileLog = DEFAULT_LOG_FILE,
            .screenLog = DEFAULT_LOG_SCREEN,
	        .screenEnabled = DEFAULT_SCREEN,
	        .screenBrightness = DEFAULT_BRIGHTNESS,
    };
	FATFS fs;
	FIL payload;

	if(f_mount(&fs, "0:", 1) == FR_OK)
	{
		iniparse(INI_FILE,handlerLoaderConfiguration,&loader);
    	initLog(loader.fileLog, loader.screenLog);
		setStartBrightness(loader.screenBrightness);
    	initScreen(loader.screenEnabled);

    	if(loader.screenEnabled)
    	{
			drawBootSplash(&loader);
    	}

    	debug("Reading GLOBAL section");
    	iniparse(INI_FILE, handler, &app);

    	debug("Wait %llu ms for Input",loader.keyDelay);
		u32 key = WaitTimeForInput(loader.keyDelay);

        // using X-macros to generate each switch-case rules
        // https://en.wikibooks.org/wiki/C_Programming/Preprocessor#X-Macros
        #define KEY(k) \
        if(key & KEY_##k) \
            app.section = "KEY_"#k; \
        else
        #include "keys.def"
            app.section = "DEFAULT";

	    debug("Key checked-selected section: %s",app.section);

	    debug("Reading selected section");
	    int config_err = iniparse(INI_FILE, handler, &app);

	    switch (config_err) {
	        case 0:
	            // section not found, try to load [DEFAULT] section
	            if (strlen(app.path)==0) {
	            	debug("Section not found, trying to load the default section");
	                app.section = "DEFAULT";
	                // don't need to check error again
	                iniparse(INI_FILE, handler, &app);
	                if (!app.path)
	                    panic("Section [DEFAULT] not found or \"path\" not set.");
	            } else if (!file_exists(app.path)) {
	                debug("[ERROR] Target payload not found:\n%s",app.path);
	                panic(app.path);
	            }
	            break;
	        case -1:
	            panic("Config file not found.");//, INI_FILE);
	            break;
	        case -2:
	            // should not happen, however better be safe than sorry
	            panic("Config file is too big.");
	            break;
	        default:
	            panic("Error found in config file");
	            break;
	    }

		debug("Checking payload");
		if(app.payload==0)
		{	
			panic("Trying to load a 3dsx - this is not supported by this version");
		}
		if(!file_exists(app.path))
		{
            debug("[ERROR] Target payload not found:");
            panic(app.path);
		}

		if(drawSplash(&app))
		{
			debug("Splash loaded");
		}

		debug("Loading Payload: %s",app.path);
		if(f_open(&payload, app.path, FA_READ | FA_OPEN_EXISTING) == FR_OK)
		{
			if(app.offset>0)
			{ 	
				debug("Jump to offset: %i",app.offset);
				f_lseek (&payload, app.offset);
			}
			debug("Reading payload");
			f_read(&payload, (void*)PAYLOAD_ADDRESS, PAYLOAD_SIZE, &br);
			debug("Finished reading the payload");

			debug("closing files and unmount sd");
			f_close(&payload);
			closeLogFile();
			f_mount(&fs, "0:", 1);

			debug("Configuring and jumping to the payload");
			if(app.screenEnabled==0)
			{
				screenDeinit();
			}
			else
			{
				screenInit(app.screenEnabled);
			}
			((void (*)())PAYLOAD_ADDRESS)();
		}
	}
	else
	{
		initScreen(true);
		panic("Failed to mount the sd-card or jump to the payload");
	}
	
    
	return 0;
}