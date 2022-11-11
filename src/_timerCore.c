#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>
#include <stdbool.h>

extern thread_id_t osCurrentTask; // Index for current running task
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads
extern uint32_t totalThreads; // number of created threads

void SysTick_Handler(void) {
	updateTimers();
}

void updateTimers(void) {
	// Update sleep and deadline timers for all threads

	bool switchRequired = false; // Only used if a thread wakes up with a deadline earlier than the current thread

	for (thread_id_t id = 0; id < totalThreads; id++) {
		if (osThreads[id].state == SLEEPING) {
			osThreads[id].sleepTimeRemaining--;

			if (osThreads[id].sleepTimeRemaining == 0) { // Thread is ready to wake up
				osThreads[id].state = ACTIVE;
				osThreads[id].deadlineCounter = osThreads[id].deadline;

				if (osThreads[id].deadlineCounter < osThreads[osCurrentTask].deadlineCounter)
					switchRequired = true;
			}

		} else if (osThreads[id].state == ACTIVE) {
			if (osThreads[id].deadlineCounter > 0)
				osThreads[id].deadlineCounter--;
			
			if (osThreads[id].deadlineCounter == 0) {
				printf("Deadline not met for Thread %d\n", id);
			}
		}	
	}

	if (switchRequired)
		osYield();
}
