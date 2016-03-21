#include <stdio.h>
#include <stdlib.h>
#include "fatfs/sdmmc/sdmmc.h"
#include "fatfs/ff.h"
#include "config.h"
#include "hid.h"
#include "log.h"
#include "splash.h"
#include "helpers.h"
#include "constands.h"

int main() {
	FATFS fs;
	FIL configFile;
	FIL payload;
	FIL latestFile;
	char latestFilePath[32]={0};
	char latestSection[15]={0};
	unsigned int br=0;

    loaderConfiguration loader =  {
	        .section = LOADER_SECTION,
	        .keyDelay = DEFAULT_KEYDELAY,
	        .bootsplash = DEFAULT_BOOTSPLASH, 
            .bootsplash_image = DEFAULT_BOOTSPLASH_IMAGE,
            .enableSoftbootSplash = DEFAULT_SOFTBOOT_SPLASH_LOADER,
            .enableAutosoftboot = DEFAULT_AUTOSOFTBOOT,
            .fileLog = DEFAULT_LOG_FILE,
            .screenLog = DEFAULT_LOG_SCREEN,
	        .screenEnabled = DEFAULT_SCREEN,
	        .screenBrightness = DEFAULT_BOOTBRIGHTNESS,
    };

	configuration app =  {
	        .section = DEFAULT_SECTION,
	        .path = DEFAULT_PATH,
	        .splashDelay = DEFAULT_SPLASHDELAY,
	        .payload = DEFAULT_PAYLOAD,
	        .offset = DEFAULT_OFFSET,
	        .splash = DEFAULT_SPLASH, 
            .splash_image = DEFAULT_SPLASH_IMAGE,
            .enableSoftbootSplash = DEFAULT_SOFTBOOT_SPLASH,
	        .screenEnabled = DEFAULT_SCREEN,
	        .screenBrightness = DEFAULT_BRIGHTNESS,
    };

	if(f_mount(&fs, "0:", 1) == FR_OK)
	{
    	if(!openIniFile(&configFile))
    		panic("Config file not found.");

		iniparse(handlerLoaderConfiguration,&loader,&configFile);
    	initLog(loader.fileLog, loader.screenLog);
    	setBrightness(loader.screenBrightness);
    	setScreenState(loader.screenEnabled);

    	if(loader.screenEnabled)
    	{
			drawBootSplash(&loader);
    	}
    	
    	debug("Reading GLOBAL section");
    	iniparse(handler, &app,&configFile);

    	if(loader.enableAutosoftboot>0)
    	{
    		debug("opening last.section file");
			checkFolders(LASTESTSETIONFILE, latestFilePath);
			if(!isColdboot())
			{
				if (f_open(&latestFile, latestFilePath, FA_READ) == FR_OK)
				{
					br=0;
				    debug("it's softreboot, read %s",LASTESTSETIONFILE);
					f_gets(latestSection, 15, &latestFile);
					br=strlen(latestSection);
					app.section=latestSection;
					f_close(&latestFile);
					debug("Lates Section: %s",app.section);
				}
			}
    	}

    	if(isColdboot() || loader.enableAutosoftboot==0 || br<=4)
    	{
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
    	}

	    debug("Reading selected section");
	    int config_err = iniparse(handler, &app, &configFile);

	    switch (config_err) {
	        case 0:
	            // section not found, try to load [DEFAULT] section
	            if (strlen(app.path)==0) {
	            	debug("Section not found, trying to load the [DEFAULT] section");
	                app.section = "DEFAULT";
	                // don't need to check error again
	                iniparse(handler, &app, &configFile);
	                if (!app.path)
	                    panic("Section [DEFAULT] not found or \"path\" not set.");
	            } else if (!file_exists(app.path)) {
	                debug("[ERROR] Target payload not found:\n%s",app.path);
	                panic(app.path);
	            }
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
            panic("[ERROR] Target payload not found: %s",app.path);
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
				f_lseek (&payload, app.offset);
			}
			debug("Reading payload at offset %i",app.offset);
			f_read(&payload, (void*)PAYLOAD_ADDRESS, PAYLOAD_SIZE, &br);
			debug("Finished reading the payload");

		    if(loader.enableAutosoftboot>0&&isColdboot())
		    {
		    	if(f_open(&latestFile, latestFilePath, FA_READ | FA_WRITE | FA_CREATE_ALWAYS )==FR_OK)
		    	{
			    	debug("writing to file: %s",app.section);
					f_puts (app.section, &latestFile);
					f_close(&latestFile);
				}           
			}

			debug("closing files and unmount sd");
			f_close(&payload);
   			f_close(&configFile);
			closeLogFile();
			f_mount(&fs, "0:", 1);

			debug("Configuring and jumping to the payload");
			setScreenState(app.screenEnabled);
    		setBrightness(app.screenBrightness);

			((void (*)())PAYLOAD_ADDRESS)();
		}
	}
	panic("Failed to mount the sd-card or jump to the payload");
    
	return 0;
}
