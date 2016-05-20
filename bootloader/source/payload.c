#include "payload.h"
#include "log.h"
#include "helpers.h"
#include "constants.h"

void checkPayload(configuration app)
{
	debug("Checking payload");
	if(app.payload==0)
	{	
		panic("Trying to load a 3dsx - this is not supported by arm9");
	}
	
	if(!file_exists(app.path))
	{
        panic("Target payload not found: %s",app.path);
	}
}

int patchPath(u32 size,const char *path)
{
	debug("Fixing payload path");
	const u8 pattern[12] = {'s', 0, 'd', 0, 'm', 0, 'c', 0, ':', 0, '/', 0};
	for(int i=0; i<size;i++)
	{
		if((*(u8 *)(PAYLOAD_ADDRESS+i))==pattern[0] && (*(u8 *)(PAYLOAD_ADDRESS+i+1))==pattern[1])
		{
			if(memcmp(pattern,(char *)(PAYLOAD_ADDRESS+i),12)==0)
			{
				u32 strgsize=strlen(path);
				u32 shift=10;
				if(path[0]!='/')
					shift=12;
				for(int x=0;x<strgsize;x++)
				{
					*(u8 *)(PAYLOAD_ADDRESS+i+shift+(x*2))=path[x];
					*(u8 *)(PAYLOAD_ADDRESS+i+shift+(x*2)+1)=0;
				}
				*(u8 *)(PAYLOAD_ADDRESS+i+shift+(strgsize*2))=0;
				return 0;
			}
		}
	}
	debug("pattern not found");
	return 1;
}
