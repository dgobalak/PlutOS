#include "_kernelCore.h"
#include "_threadsCore.h"
#include "svc_handler.h"
#include "osDefs.h"

#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// GLOBAL VARIABLES
thread_handle_t taskListHead = NULL; // Head of the task list
thread_handle_t taskListTail = NULL; // Tail of the task list
int taskCount = 0; // Number of tasks in the task linked list

thread_handle_t currentTaskHandle = NULL; // Handle for current running task

static osthread_t idleTask; // Idle task
thread_handle_t idleTaskHandle = NULL; // Handle for the idle task

uint32_t mspAddr; // The initial address of the MSP

/**
 * @brief Idle task that prints a message and then yields
 * @note It exists so that there's at least 1 function that
 * can be run when there are no other threads to run
 * 
 * @param args Thread arguments
 */
static void osIdleTaskFunc(void* args) {
	while(1);
}

void kernelInit(void) {
	SHPR3 |= SYSTICK_PRIORITY << 24; // Set SysTick priority
	SHPR3 |= PENDSV_PRIORITY << 16; // Set PendSV priority
	SHPR2 |= SVC_PRIORITY << 24; // Set SVC priority
	
	// Initialize the address of the MSP
	uint32_t* MSP_Original = MSP_STORAGE_ADDR;
	mspAddr = *MSP_Original;
	
	// Configure the SysTick interrupt
	SysTick_Config(SYSTICK_TICKS);
	
	// Create the idle thread
	idleTaskHandle = osNewThread(&idleTask, osIdleTaskFunc, IDLE_THREAD_DEADLINE);
}

void osSched(void) {
	// Find the task with the earliest deadline, but not the idle task
	ms_time_t earliestDeadline = UINT32_MAX;
	thread_handle_t earliestHandle = NULL;
	thread_handle_t handle = currentTaskHandle;

	for (int i = 0; i < taskCount + 1; i++) {
		if (handle == idleTaskHandle)
			continue;
		
		if (handle->state != ACTIVE)
			continue;
		
		if (handle->deadline < earliestDeadline || earliestHandle == NULL) {
			earliestDeadline = handle->deadline;
			earliestHandle = handle;
		}
	
		if (handle->next) {
			handle = handle->next;
		} else {
			handle = taskListHead;
		}
	}

	currentTaskHandle = earliestHandle; // NULL if no tasks are active

	// If no ACTIVE task is found, switch to the idle task
	if (earliestHandle == NULL) {
		currentTaskHandle = idleTaskHandle;
		
	// Set the chosen thread's state to RUNNING
	currentTaskHandle->state = RUNNING;
}

void pendPendSV(void) {
	// Pend the PendSV interrupt
	ICSR |= SET_PENDSVET; // Set PENDSVSET to 1 to make PendSV exception pending
	__asm("isb");
}

void yieldCurrentTask(uint8_t stackDiff) {
	if (currentTaskHandle != NULL) {
		// Yield the current RUNNING task
		// If the thread was already set to the SLEEPING state by osSleep, keep it as SLEEPING
		// Otherwise, set as ACTIVE
		currentTaskHandle->state = (currentTaskHandle->state == SLEEPING) ? SLEEPING : ACTIVE;
		currentTaskHandle->threadStack = (uint32_t*)(__get_PSP() - stackDiff*sizeof(uint32_t)); // We are about to push `stackDiff` uint32_t's
	}
}

void osYield(void) {
	__ASM(SVC_YIELD_SWITCH_CMD); // Make SVC call to yield
}

void osYieldPreemptive(void) {
	__ASM(SVC_YIELD_SWITCH_PREEMPTIVE_CMD); // Make SVC call to yield preemptively
}

void osSleep(ms_time_t sleepTime) {
	// Set current task to the SLEEPING state
	currentTaskHandle->sleepTimeRemaining = sleepTime;
	currentTaskHandle->state = SLEEPING;
	osYield();
}

bool osKernelStart(void) {
	if (taskCount > 0) {
		currentTaskHandle = NULL;
		__set_CONTROL(THREADING_MODE);
		__set_PSP((uint32_t)idleTaskHandle.threadStack);
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
	__set_PSP((uint32_t)currentTaskHandle->threadStack); // Set the new PSP
	return 1; 
}
