#pragma once

#include "common.h"
#include "config.h"


#define ASCII_ART_TEMPLATE (" _           __       _\n" \ 
                            "|_) _  _ _|_/  _|_ __(_)\n" \ 
                            "|_)(_)(_) |_\\__ |_ |  /\n" \
                            "%s\n" \ 
                            "\n" \ 
                            "is loading...") 
#define ASCII_ART_TEMPLATE_EXTENDET (" _           __       _\n" \ 
                            		 "|_) _  _ _|_/  _|_ __(_)\n" \ 
                            		 "|_)(_)(_) |_\\__ |_ |  /\n" \
                            		 "%s\n" \ 
                            		 "\n" \ 
                            		 "Payload: %s") 
							
bool drawBootSplash(loaderConfiguration* loaderConfig);
bool drawSplash(configuration* app);
int splash_ascii(void); 
int splash_ascii_extendet(const char* payloadName); 
int splash_image(char *splash_path); 
int splash_image_extendet(char *splash_path,const char* payloadName); 
