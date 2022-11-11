#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>

extern thread_id_t osCurrentTask; // Index for current running task
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads
extern uint32_t totalThreads; // number of created threads

void SysTick_Handler(void) {
	updateTimers();
}

void updateTimers(void) {
	// Update sleepTimeRemaining of all SLEEPING tasks, and update deadlines of all ACTIVE tasks
	for (thread_id_t id = 0; id < totalThreads; id++) {
		if (osThreads[id].state == SLEEPING) {
			osThreads[id].sleepTimeRemaining--;
			if (osThreads[id].sleepTimeRemaining < 1) {
				osThreads[id].state = ACTIVE;
				osThreads[id].deadlineCounter = osThreads[id].deadline;
				if (osThreads[id].deadlineCounter < osThreads[osCurrentTask].deadlineCounter)
					osYield();
			}
		} else if (osThreads[id].state == ACTIVE) {
			osThreads[id].deadlineCounter--;
			
			if (osThreads[id].deadlineCounter < 1) {
				printf("Deadline not met for Thread %d", id);
			}
		}	
	}
}
