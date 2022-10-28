#include "_kernelCore.h"
#include "osDefs.h"

#include <LPC17xx.h>
#include <stdbool.h>

int osCurrentTask = 0; // Index for current running task

osthread_t osThreads[MAX_THREADS]; // Array of all threads

int threadNums; // number of created threads
int osNumThreadsRunning; // number of running threads

uint32_t mspAddr; //the initial address of the MSP

void kernelInit(void) {
	// Set PendSV to the weakest priority we can set
	SHPR3 |= 0xFF << 16;
	
	// Initialize the address of the MSP
	uint32_t* MSP_Original = 0;
	mspAddr = *MSP_Original;
	
	SysTick_Config(TIMESLICE_TICKS);
}

void osSched(void) {
	
	// Choose next thread to run
	// TODO: Make use of the priority when choosing next task
	// For now, just do round robin scheduling with no priorities
	osCurrentTask = (osCurrentTask+1)%(threadNums);
	osThreads[osCurrentTask].state = RUNNING;
}

void osYield(void) {
	if(osCurrentTask >= 0) {
		// Yield the current task
		osThreads[osCurrentTask].state = ACTIVE;	
		osThreads[osCurrentTask].threadStack = (uint32_t*)(__get_PSP() - 16*4); //we are about to push 16 uint32_t's
	}
	
	// Pend the PendSV interrupt
	ICSR |= 1<<28; // Set PENDSVSET to 1 to make PendSV exception pending
	__asm("isb");
}

bool osKernelStart() {
	if(threadNums > 0) {
		osCurrentTask = -1;
		__set_CONTROL(1<<1);
		__set_PSP((uint32_t)osThreads[0].threadStack);
		osYield();
	}
	// If we get here, we failed to start the kernel
	return false;
}

void setThreadingWithPSP(uint32_t* threadStack)
{
	__set_PSP((uint32_t)threadStack); // Set PSP to threadStack
	__set_CONTROL(1<<1); // Switch to threading mode
	
}

int switchTask(void){
	osSched(); // Choose next task
	__set_PSP((uint32_t)osThreads[osCurrentTask].threadStack); //set the new PSP

	// The return value can be accessed in the assembly code. Access it from r0 before overwriting r0
	return 1; 
}
