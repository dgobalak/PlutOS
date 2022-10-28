#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>

extern int osCurrentTask; // Current task ID
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads

void SysTick_Handler(void) {
	// Set current thread to active state
	if(osCurrentTask >= 0) {
		// Yield the current task
		osThreads[osCurrentTask].state = ACTIVE;	
		osThreads[osCurrentTask].threadStack = (uint32_t*)(__get_PSP() - 8*4); //we are about to push 8 uint32_t's
	}
	
	osSched();
	
	// Pend the PendSV interrupt
	ICSR |= 1<<28; // Set PENDSVSET to 1 to make PendSV exception pending
	__asm("isb");
}
