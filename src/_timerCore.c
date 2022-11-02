#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>

extern volatile bool osYieldMutex;
extern volatile int osCurrentTask; // Index for current running task
extern volatile osthread_t osThreads[MAX_THREADS]; // Array of all threads
extern volatile int threadNums; // number of created threads

static uint32_t ticks_elapsed = 0;

void SysTick_Handler(void) {
	ticks_elapsed++;
	updateTimers();
	if (osYieldMutex && !osThreads[osCurrentTask].timeRunning) {
		osYieldFromSysTick();
	}
}

void updateTimers(void) {
	// Update time running
	if (osThreads[osCurrentTask].timeRunning)
		osThreads[osCurrentTask].timeRunning--;
	
	// Update time sleeping
	for (thread_id_t id; id < threadNums; id++) {
		if (osThreads[id].state == SLEEPING) {
				osThreads[id].timeSleeping--;
			if (osThreads[id].timeSleeping < 1) {
				printf(""); // DO NOT REMOVE
				osThreads[id].state = ACTIVE;
			}
		}
	}
}
