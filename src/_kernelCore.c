#include "_kernelCore.h"
#include "osDefs.h"

#include <LPC17xx.h>
#include <stdbool.h>

int osCurrentTask = 0; // Index for current running task

osthread_t osThreads[MAX_THREADS]; // Array of all threads

int threadNums; // number of created threads
int osNumThreadsRunning; // number of running threads

uint32_t mspAddr; //the initial address of the MSP

// Sets priority of PenSV interrupt and detects/stores initial MSP location
void kernelInit(void) {
	// Set PendSV priority
	SHPR3 |= 0xFF << 16;
	
	// Initialize the address of the MSP
	uint32_t* MSP_Original = 0;
	mspAddr = *MSP_Original;
}

// Yield current task (Switch to next available task)
void osYield(void) {
	
	if(osCurrentTask >= 0) {
		osThreads[osCurrentTask].state = WAITING;	
		osThreads[osCurrentTask].threadStack = (uint32_t*)(__get_PSP() - 16*4); //we are about to push 16 uint32_t's
	}
	
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
	return false;
}

void setThreadingWithPSP(uint32_t* threadStack)
{
	__set_PSP((uint32_t)threadStack);
	__set_CONTROL(1<<1);
	
}

int switchTask(void){
	// TODO: Make use of the priority when choosing next task
	osCurrentTask = (osCurrentTask+1)%(threadNums);
	osThreads[osCurrentTask].state = ACTIVE;
	
	__set_PSP((uint32_t)osThreads[osCurrentTask].threadStack); //set the new PSP
	return 1; //You are free to use this return value in your assembly eventually. It will be placed in r0, so be sure
	//to access it before overwriting r0
}
