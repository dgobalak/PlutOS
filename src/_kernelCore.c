#include "_kernelCore.h"
#include "_threadsCore.h"
#include "svc_handler.h"
#include "osDefs.h"

#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// GLOBAL VARIABLES
osthread_t osThreads[MAX_THREADS]; // Array of all threads
thread_id_t osCurrentTask = -1; // Index for current running task
uint32_t totalThreads = 0; // Number of created threads
uint32_t mspAddr; // The initial address of the MSP

/**
 * @brief Idle task that prints a message and then yields
 * @note It exists so that there's at least 1 function that
 * can be run when there are no other threads to run
 * 
 * @param args Thread arguments
 */
static void osIdleTask(void* args) {
	while(1) {
		// printf("Idle\n");
	}
}

void kernelInit(void) {
	SHPR3 |= SYSTICK_PRIORITY << 24; // Set SysTick priority
	SHPR3 |= PENDSV_PRIORITY << 16; // Set PendSV priority
	SHPR2 |= SVC_PRIORITY << 24; // Set SVC priority
	
	// Initialize the address of the MSP
	uint32_t* MSP_Original = 0;
	mspAddr = *MSP_Original;
	
	// Configure the SysTick interrupt
	SysTick_Config(SYSTICK_TICKS);
	
	// Create the idle thread
	osNewThread(osIdleTask, IDLE_THREAD_DEADLINE);
}

void osSched(void) {
	// Find the task with the earliest deadline, but not the idle task
	ms_time_t earliestDeadline = UINT32_MAX;
	thread_id_t earliestID = -1;
	thread_id_t id = osCurrentTask;

	// `i` goes to totalThreads + 1 so that it loops back to the original thread
	for (int i = 0; i < totalThreads + 1; i++) {
		id = (id + 1) % totalThreads;

		if (id == IDLE_THREAD_ID)
			continue;
		
		if (osThreads[id].state != ACTIVE)
			continue;
		
		if ( (osThreads[id].deadline < earliestDeadline) || (earliestID == -1) ) {
			earliestDeadline = osThreads[id].deadline;
			earliestID = id;
		}
	}
	
	// If no ACTIVE task is found, switch to the idle task
	if (earliestID == -1)
		osCurrentTask = IDLE_THREAD_ID;

	osCurrentTask = earliestID; // -1 if idle thread is the only active task
	
	// Set the chosen thread's state to RUNNING
	osThreads[osCurrentTask].state = RUNNING;
}

void pendPendSV(void) {
	// Pend the PendSV interrupt
	ICSR |= SET_PENDSVET; // Set PENDSVSET to 1 to make PendSV exception pending
	__asm("isb");
}

void yieldCurrentTask(uint8_t stackDiff) {
	if (osCurrentTask >= 0) {
		// Current task is either RUNNING, SLEEPING, OR ACTIVE
		// RUNNING task must be set to ACTIVE
		if (osThreads[osCurrentTask].state == RUNNING)
			osThreads[osCurrentTask].state = ACTIVE;
		
		osThreads[osCurrentTask].threadStack = (uint32_t*)(__get_PSP() - stackDiff*sizeof(uint32_t)); // We are about to push `stackDiff` uint32_t's
	}
}

void osYield(void) {
	__ASM(SVC_YIELD_SWITCH_CMD); // Make SVC call to yield
}

void osYieldNoReset(void) {
	__ASM(SVC_YIELD_SWITCH_NO_RESET_CMD); // Make SVC call to yield without resetting deadlines
}

void osSleep(ms_time_t sleepTime) {
	// Set current task to the SLEEPING state
	osThreads[osCurrentTask].sleepTimeRemaining = sleepTime;
	osThreads[osCurrentTask].state = SLEEPING;
	osYield();
}

void osSleepNoReset(ms_time_t sleepTime) {
	// Set current task to the SLEEPING state without resetting deadlines
	osThreads[osCurrentTask].sleepTimeRemaining = sleepTime;
	osThreads[osCurrentTask].state = SLEEPING;
	osYieldNoReset();
}

bool osKernelStart(void) {
	if (totalThreads > 0) {
		osCurrentTask = -1;
		__set_CONTROL(THREADING_MODE);
		__set_PSP((uint32_t)osThreads[0].threadStack);
		osYield();
	}
	// If we get here, we failed to start the kernel
	return false;
}

void setThreadingWithPSP(uint32_t* threadStack) {
	__set_PSP((uint32_t)threadStack); // Set PSP to threadStack
	__set_CONTROL(THREADING_MODE); // Switch to threading mode
}

int switchTask(void) {
	__set_PSP((uint32_t)osThreads[osCurrentTask].threadStack); // Set the new PSP
	return 1; 
}
