#include "_kernelCore.h"
#include "osDefs.h"

#include <LPC17xx.h>

// Index for current running task
int osCurrentTask = 0;

osthread_t osThreads[MAX_THREADS]; // Array of all threads

int threadNums = 0; // number of threads created
int osNumThreadsRunning = 0; // number of threads that have started running

uint32_t mspAddr; //the initial address of the MSP

// Sets priority of PenSV interrupt and detects/stores initial MSP location
void kernelInit(void) {
	// Set PendSV priority
	SHPR3 |= 0xFF << 16;
	
	//initialize the address of the MSP
	uint32_t* MSP_Original = 0;
	mspAddr = *MSP_Original;
}

void osYield(void) {
	
	if(osCurrentTask >= 0) {
		osThreads[osCurrentTask].state = WAITING;	
	
		osThreads[osCurrentTask].threadStack = (uint32_t*)(__get_PSP() - 16*4); //we are about to push 16 uint32_t's
	}

	osCurrentTask = (osCurrentTask+1)%(threadNums);
	osThreads[osCurrentTask].state = ACTIVE;
	
	ICSR |= 1<<28;
	__asm("isb");
}

/*
	Starts the threads if threads have been created. Returns false otherwise
	This function will not return under normal circumstances, since its job is to start the 
	threads, which take over. Therefore, if this function ever returns at all something has gone wrong.
*/
bool osKernelStart() {
	if(threadNums > 0)
	{
		osCurrentTask = -1;
		__set_CONTROL(1<<1);
		__set_PSP((uint32_t)osThreads[0].threadStack);
		osYield();
	}
	return 0;
}

void setThreadingWithPSP(uint32_t* threadStack)
{
	__set_PSP((uint32_t)threadStack);
	__set_CONTROL(1<<1);
	
}

/*
	at the moment this just changes the stack from one to the other. I personally found
	this to be easier to do in C. You may want to do more interesting things here.
	For example, when implementing more sophisticated scheduling algorithms, perhaps now is the time
	to figure out which thread should run, and then set PSP to its stack pointer...
*/
int task_switch(void){
		__set_PSP((uint32_t)osThreads[osCurrentTask].threadStack); //set the new PSP
		return 1; //You are free to use this return value in your assembly eventually. It will be placed in r0, so be sure
		//to access it before overwriting r0
}
