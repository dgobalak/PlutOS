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
	bool switchRequired = false; // Only set if a thread has a deadline earlier than the current thread

	// Iterate through all threads
	for (thread_id_t id = 0; id < totalThreads; id++) {
		// Don't need to do anything for IDLE thread
		if (id == IDLE_THREAD_ID)
			continue;

		if (osThreads[id].state == SLEEPING) {
			osThreads[id].sleepTimeRemaining--;

			if (osThreads[id].sleepTimeRemaining != 0)
				continue;
			
			// Thread is ready to wake up; sleep time is zero
			osThreads[id].state = ACTIVE;

		} else if (osThreads[id].state == ACTIVE || osThreads[id].state == RUNNING || osThreads[id].state == BLOCKED) {		
			if (osThreads[id].deadlineCounter > 0)
				osThreads[id].deadlineCounter--;
			
			if (osThreads[id].deadlineCounter == 0)
				printf("Deadline not met for Thread %d\n", id); // Deadline missed
		}
		
		// Check if thread has an earlier deadline than the current thread
		if (osThreads[id].deadlineCounter < osThreads[osCurrentTask].deadlineCounter)
			switchRequired = true;
	}

	// If a thread woke up with a deadline earlier than the current thread, switch to that thread
	if (switchRequired)
		osYieldNoReset();
}
