#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>

extern int osCurrentTask;
extern osthread_t osThreads[MAX_THREADS];

extern int threadNums; //number of threads actually created
extern int osNumThreadsRunning; //number of threads that have started runnin
extern uint32_t mspAddr; //the initial address of the MSP

uint32_t* getMSPInitialLocation(void)
{
	uint32_t* MSP = 0;
	return (uint32_t*)*MSP;
}

uint32_t* getNewThreadStack(uint32_t offset)
{
	static uint8_t numStacks = 0;
	
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

//returns the thread ID, or -1 if that is not possible
int osNewThread(void (*taskFunc)(void*args))
{
	if(threadNums < MAX_THREADS)
	{
		osThreads[threadNums].state = WAITING; //tells the OS that it is ready but not yet run
		osThreads[threadNums].threadFunc = taskFunc;
		osThreads[threadNums].threadStack = getNewThreadStack(MSR_STACK_SIZE + threadNums*THREAD_STACK_SIZE);//(uint32_t*)((mspAddr - MSR_STACK_SIZE) - (threadNums)*THREAD_STACK_SIZE);
		
		//Now we need to set up the stack
		
		//First is xpsr, the status register. If bit 24 is not set and we are in thread mode we get a hard fault, so we just make sure it's set
		*(--osThreads[threadNums].threadStack) = 1<<24;
		
		//Next is the program counter, which is set to whatever the function we are running will be
		*(--osThreads[threadNums].threadStack) = (uint32_t)taskFunc;
		
		//Next is a set of important registers. These values are meaningless but we are setting them to be nonzero so that the 
		//compiler doesn't optimize out these lines
		*(--osThreads[threadNums].threadStack) = 0xE; //LR
		*(--osThreads[threadNums].threadStack) = 0xC; //R12
		*(--osThreads[threadNums].threadStack) = 0x3; //R3
		*(--osThreads[threadNums].threadStack) = 0x2; //R2
		*(--osThreads[threadNums].threadStack) = 0x1; //R1
		*(--osThreads[threadNums].threadStack) = 0x0; // R0
		
		
		//Now we have registers R11 to R4, which again are just set to random values so that we know for sure that they exist
		*(--osThreads[threadNums].threadStack) = 0xB; //R11
		*(--osThreads[threadNums].threadStack) = 0xA; //R10
		*(--osThreads[threadNums].threadStack) = 0x9; //R9
		*(--osThreads[threadNums].threadStack) = 0x8; //R8
		*(--osThreads[threadNums].threadStack) = 0x7; //R7
		*(--osThreads[threadNums].threadStack) = 0x6; //R6
		*(--osThreads[threadNums].threadStack) = 0x5; //R5
		*(--osThreads[threadNums].threadStack) = 0x4; //R4
		
		
		//Now the stack is set up, the thread's SP is correct, since we've been decrementing it.
		threadNums++;
		osNumThreadsRunning++;
		return threadNums - 1;
	}
	return -1;
}

