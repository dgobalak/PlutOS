#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>

extern bool osYieldMutex; // Mutex to protect yield
extern thread_id_t osCurrentTask; // Index for current running task
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads
extern uint32_t totalThreads; // number of created threads

void SysTick_Handler(void) {
	updateTimers();
	if (osYieldMutex && !osThreads[osCurrentTask].runTimeRemaining) {
		osYieldFromSysTick();
	}
}

void updateTimers(void) {
	// Update runTimeRemaining of current task
	if (osThreads[osCurrentTask].runTimeRemaining != 0) {
		osThreads[osCurrentTask].runTimeRemaining--;
	}
	
	// Update sleepTimeRemaining of all SLEEPING tasks
	for (thread_id_t id = 0; id < totalThreads; id++) {
		if (osThreads[id].state == SLEEPING) {
			osThreads[id].sleepTimeRemaining--;
			if (osThreads[id].sleepTimeRemaining < 1) {
				osThreads[id].state = ACTIVE;
			}
		}
	}
}
