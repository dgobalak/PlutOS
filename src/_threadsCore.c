#include "_threadsCore.h"
#include "osDefs.h"

#include <stdint.h>
#include <stdio.h>

extern thread_id_t osCurrentTask; // Current task ID
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads

extern uint32_t totalThreads; // Number of created threads
extern uint32_t mspAddr; // The address of the MSP

uint32_t* getMSPInitialLocation(void) {
	uint32_t* MSP = 0; // Initial MSP is stored at address 0
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

thread_id_t osNewThread(void (*taskFunc)(void*args), thread_priority_t priority) {
	// TODO: Make use of priority
	if(totalThreads < MAX_THREADS) {
		// Configure the thread in the array
		osThreads[totalThreads].threadStack = getNewThreadStack(MSR_STACK_SIZE + totalThreads*THREAD_STACK_SIZE);//(uint32_t*)((mspAddr - MSR_STACK_SIZE) - (totalThreads)*THREAD_STACK_SIZE);
		osThreads[totalThreads].threadFunc = taskFunc;
		osThreads[totalThreads].state = ACTIVE;
		osThreads[totalThreads].runTimeRemaining = 0;
		osThreads[totalThreads].sleepTimeRemaining = 0;
		osThreads[totalThreads].priority = priority;
		
		
		// Now we need to set up the stack; it should 'look' like the thread is already running
		
		// First is xpsr, the status register. If bit 24 is not set and we are in thread mode we get a hard fault, so we just make sure it's set
		*(--osThreads[totalThreads].threadStack) = 1<<24;
		
		// Next is the program counter, which is set to whatever the function we are running will be
		*(--osThreads[totalThreads].threadStack) = (uint32_t)taskFunc;
		
		// Next is a set of important registers. These values are meaningless but we are setting them to be nonzero so that the 
		// compiler doesn't optimize out these lines
		*(--osThreads[totalThreads].threadStack) = 0xE; //LR
		*(--osThreads[totalThreads].threadStack) = 0xC; //R12
		*(--osThreads[totalThreads].threadStack) = 0x3; //R3
		*(--osThreads[totalThreads].threadStack) = 0x2; //R2
		*(--osThreads[totalThreads].threadStack) = 0x1; //R1
		*(--osThreads[totalThreads].threadStack) = 0x0; // R0
		
		
		// Now we have registers R11 to R4, which again are just set to random values so that we know for sure that they exist
		*(--osThreads[totalThreads].threadStack) = 0xB; //R11
		*(--osThreads[totalThreads].threadStack) = 0xA; //R10
		*(--osThreads[totalThreads].threadStack) = 0x9; //R9
		*(--osThreads[totalThreads].threadStack) = 0x8; //R8
		*(--osThreads[totalThreads].threadStack) = 0x7; //R7
		*(--osThreads[totalThreads].threadStack) = 0x6; //R6
		*(--osThreads[totalThreads].threadStack) = 0x5; //R5
		*(--osThreads[totalThreads].threadStack) = 0x4; //R4
		
		
		// Now the stack is set up, the thread's SP is correct, since we've been decrementing it.
		totalThreads++;
		return (thread_id_t)(totalThreads - 1); // Return the thread index
	}
	return -1;
}
