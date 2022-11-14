#include "svc_handler.h"
#include "_kernelCore.h"
#include "osDefs.h"

#include <stdint.h>
#include <stdio.h>

extern thread_id_t osCurrentTask; // Current task ID
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads

void svcYield(void) {
	if (osThreads[osCurrentTask].isPeriodic) {
		// Periodic threads must be put to sleep
		osThreads[osCurrentTask].sleepTimeRemaining = osThreads[osCurrentTask].period;
		osThreads[osCurrentTask].state = SLEEPING;
	}

	// Reset the deadline counter
	osThreads[osCurrentTask].deadlineCounter = osThreads[osCurrentTask].deadline;
	
	// Switch to new thread
	yieldCurrentTask(NUM_REGS_TO_PUSH);
	osSched(); // Choose next task			
	pendPendSV();
}

void SVC_Handler_Main(uint32_t *svc_args) {
	// `call` is whatever was passed in during "SVC #IMMEDIATE" instruction
	char call = ((char*)svc_args[6])[-2];
	
	switch(call) {
		case SVC_YIELD_SWITCH:
			svcYield();
			break;
	}

}
