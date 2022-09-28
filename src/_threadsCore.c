#include "_threadsCore.h"
#include "stdio.h"

uint32_t* getMSPInitialLocation(void)
{
	uint32_t* MSP = 0;
	return (uint32_t*)*MSP;
}

uint32_t* getNewThreadStack(uint32_t offset)
{
	if(offset % 8 != 0)
	{
		offset = offset + sizeof(uint32_t);
	}
	
	if(offset < 0x2000) //Using 0x2000 bytes as maxmimum stack size (from starter code)
	{
		uint32_t* PSP = (uint32_t*)(getMSPInitialLocation() - offset/4);
		return PSP;
	}
	else
	{
		//Temporary error message and return
		printf("The offset exceeds the maximum thread stack size.\r\n");
		return NULL;
	}
}

void setThreadingWithPSP(uint32_t* threadStack)
{
	__set_PSP((uint32_t)threadStack);
	__set_CONTROL(1<<1);
	
}
