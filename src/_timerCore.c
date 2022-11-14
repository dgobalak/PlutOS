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
		if (id == IDLE_THREAD_ID)
			continue;

		if (osThreads[id].state == SLEEPING) {
			osThreads[id].sleepTimeRemaining--;

			if (osThreads[id].sleepTimeRemaining != 0)
				continue;
			
			// Thread is ready to wake up
			osThreads[id].state = ACTIVE;

		} else if (osThreads[id].state == ACTIVE || osThreads[id].state == RUNNING) {		
			if (osThreads[id].deadlineCounter > 0)
				osThreads[id].deadlineCounter--;
			
			if (osThreads[id].deadlineCounter == 0)
				printf("Deadline not met for Thread %d\n", id);
		}	
		if (osThreads[id].deadlineCounter < osThreads[osCurrentTask].deadlineCounter)
			switchRequired = true;
	}

	// If a thread woke up with a deadline earlier than the current thread, switch to that thread
	if (switchRequired) {
		if (osThreads[osCurrentTask].isPeriodic) {
			osThreads[osCurrentTask].sleepTimeRemaining = osThreads[osCurrentTask].period;
			osThreads[osCurrentTask].state = SLEEPING;
		}

		// Reset the deadline counter
		osThreads[osCurrentTask].deadlineCounter = osThreads[osCurrentTask].deadline;
		
		yieldCurrentTask(NUM_REGS_TO_PUSH);
		osSched(); // Choose next task			
		pendPendSV();	}
}
