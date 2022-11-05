#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>

extern bool osYieldMutex; // Mutex to protect yield
extern int osCurrentTask; // Index for current running task
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads
extern int threadNums; // number of created threads

void SysTick_Handler(void) {
	updateTimers();
	if (osYieldMutex && !osThreads[osCurrentTask].timeRunning) {
		osYieldFromSysTick();
	}
}

void updateTimers(void) {
	// Update timeRunning of current task
	if (osThreads[osCurrentTask].timeRunning != 0) {
		osThreads[osCurrentTask].timeRunning--;
	}
	
	// Update sleepTimeRemaining of all SLEEPING tasks
	for (thread_id_t id = 0; id < threadNums; id++) {
		if (osThreads[id].state == SLEEPING) {
			osThreads[id].sleepTimeRemaining--;
			if (osThreads[id].sleepTimeRemaining < 1) {
				osThreads[id].state = ACTIVE;
			}
		}
	}
}
