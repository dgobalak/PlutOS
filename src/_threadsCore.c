#include "_threadsCore.h"
#include "osDefs.h"

#include <stdint.h>
#include <stdio.h>

// GLOBAL VARIABLES
extern thread_handle_t taskListHead = NULL; // Head of the task list
extern thread_handle_t taskListTail = NULL; // Tail of the task list
extern int taskCount = 0; // Number of tasks in the task linked list

extern thread_handle_t currentTaskHandle = NULL; // Handle for current running task

extern uint32_t mspAddr; // The address of the MSP

uint32_t* getMSPInitialLocation(void) {
	uint32_t* MSP = MSP_STORAGE_ADDR; // Initial MSP is stored at address 0
	return (uint32_t*)*MSP;
}

uint32_t* getNewThreadStack(uint32_t offset) {
	static uint8_t numStacks = 0;

	// Check if we have reached the maximum total stack size
	if (MAX_POOL_SIZE < (numStacks+1)*(THREAD_STACK_SIZE))
		return NULL;
	
	// If offset isn't a multiple of 8, it's a multiple of 4
	if(offset % 8 != 0)
		offset = offset + sizeof(uint32_t); // The stack will either be divisible by 8 or 4
	
	// Decrement MSP by offset because ARM Cortex's stack grows down
	uint32_t* PSP = (uint32_t *)((uint32_t)getMSPInitialLocation() - offset);
	numStacks++;
	return PSP;
}

thread_id_t osNewPeriodicThread(thread_handle_t handle, void (*taskFunc)(void*args), ms_time_t deadline, ms_time_t period) {
	thread_handle_t newHandle = osNewThread(handle, taskFunc, deadline);
	
	if (newHandle != NULL)) {
		// Configure attributes specific to periodic threads
		newHandle->period = period;
		newHandle->isPeriodic = true;
	}
	
	return newHandle;
}
	
thread_handle_t osNewThread(thread_handle_t handle, void (*taskFunc)(void*args), ms_time_t deadline) {
	if (handle == NULL)
		return NULL;

	if (taskFunc == NULL)
		return NULL;

	// Add the thread to the linked list
	taskListTail->next = handle;
	handle->prev = taskListTail;
	taskListTail = handle;

	taskListTail->threadStack = getNewThreadStack(MSR_STACK_SIZE + taskCount*THREAD_STACK_SIZE); // (uint32_t*)((mspAddr - MSR_STACK_SIZE) - (totalThreads)*THREAD_STACK_SIZE);
	taskListTail->threadFunc = taskFunc;
	taskListTail->state = ACTIVE;
	taskListTail->sleepTimeRemaining = 0;
	taskListTail->deadline = deadline;
	taskListTail->deadlineCounter = deadline;
	taskListTail->period = 0;
	taskListTail->isPeriodic = false;
	
	// Now we need to set up the stack; it should 'look' like the thread is already running
	
	// First is xpsr, the status register. If bit 24 is not set and we are in thread mode we get a hard fault, so we just make sure it's set
	*(--taskListTail->threadStack) = 1<<24;
	
	// Next is the program counter, which is set to whatever the function we are running will be
	*(--taskListTail->threadStack) = (uint32_t)taskFunc;
	
	// Next is a set of important registers. These values are meaningless but we are setting them to be nonzero so that the 
	// compiler doesn't optimize out these lines
	*(--taskListTail->threadStack) = 0xE; //LR
	*(--taskListTail->threadStack) = 0xC; //R12
	*(--taskListTail->threadStack) = 0x3; //R3
	*(--taskListTail->threadStack) = 0x2; //R2
	*(--taskListTail->threadStack) = 0x1; //R1
	*(--taskListTail->threadStack) = 0x0; // R0
	
	
	// Now we have registers R11 to R4, which again are just set to random values so that we know for sure that they exist
	*(--taskListTail->threadStack) = 0xB; //R11
	*(--taskListTail->threadStack) = 0xA; //R10
	*(--taskListTail->threadStack) = 0x9; //R9
	*(--taskListTail->threadStack) = 0x8; //R8
	*(--taskListTail->threadStack) = 0x7; //R7
	*(--taskListTail->threadStack) = 0x6; //R6
	*(--taskListTail->threadStack) = 0x5; //R5
	*(--taskListTail->threadStack) = 0x4; //R4
		
		
	// Now the stack is set up, the thread's SP is correct, since we've been decrementing it.
	taskCount++;
	return handle; // Return the thread index
}
