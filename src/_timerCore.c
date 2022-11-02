#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>

extern bool osYieldMutex;
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
	// Update time running
	if (osThreads[osCurrentTask].timeRunning != 0) {
		osThreads[osCurrentTask].timeRunning--;
	}
	
	// Update time sleeping
	for (thread_id_t id = 0; id < threadNums; id++) {
		if (osThreads[id].state == SLEEPING) {
			osThreads[id].timeSleeping--;
			if (osThreads[id].timeSleeping < 1) {
				osThreads[id].state = ACTIVE;
			}
		}
	}
}
