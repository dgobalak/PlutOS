#include "_threadsCore.h"
#include "stdio.h"

uint8_t numStacks = 0;

uint32_t* getMSPInitialLocation(void)
{
	uint32_t* MSP = 0;
	return (uint32_t*)*MSP;
}

uint32_t* getNewThreadStack(uint32_t offset)
{
	if (offset % STACK_SIZE != 0)
		return NULL;
	
	if (MAX_POOL_SIZE < (numStacks+1)*(STACK_SIZE))
		return NULL;
			
	
	if(offset % 8 != 0)
		offset = offset + sizeof(uint32_t); // The stack will either be divisible by 8 or 4
	
	
	uint32_t* PSP = (uint32_t *)((uint32_t)getMSPInitialLocation() - offset);
	numStacks++;
	return PSP;
}

void setThreadingWithPSP(uint32_t* threadStack)
{
	__set_PSP((uint32_t)threadStack);
	__set_CONTROL(1<<1);
	
}
