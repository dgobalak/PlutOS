#include "_threadsCore.h"
#include "osDefs.h"

#include <stdint.h>
#include <stdio.h>

extern int osCurrentTask; // Current task ID
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads

extern int threadNums; // number of created threads
extern uint32_t mspAddr; //the address of the MSP

uint32_t* getMSPInitialLocation(void) {
	uint32_t* MSP = 0; // Initial MSP is stored at address 0
	return (uint32_t*)*MSP;
}

uint32_t* getNewThreadStack(uint32_t offset) {
	static uint8_t numStacks = 0;
	
	// Check if offset is divisible by stack size
	if (offset % THREAD_STACK_SIZE != 0)
		return NULL;

	// Check if we have reached the maximum total stack size
	if (MAX_POOL_SIZE < (numStacks+1)*(THREAD_STACK_SIZE))
		return NULL;
	
	// Since the offset is a multiple of the default stack size (512), 
	// it's also a multiple of 8
	if(offset % 8 != 0)
		offset = offset + sizeof(uint32_t); // The stack will either be divisible by 8 or 4
	
	// Decrement MSP by offset because ARM Cortex's stack grows down
	uint32_t* PSP = (uint32_t *)((uint32_t)getMSPInitialLocation() - offset);
	numStacks++;
	return PSP;
}

thread_id_t osNewThread(void (*taskFunc)(void*args), thread_type_t type, thread_priority_t priority) {
	// TODO: Make use of priority
	if(threadNums < MAX_THREADS) {
		// Configure the thread in the array
		osThreads[threadNums].threadStack = getNewThreadStack(MSR_STACK_SIZE + threadNums*THREAD_STACK_SIZE);//(uint32_t*)((mspAddr - MSR_STACK_SIZE) - (threadNums)*THREAD_STACK_SIZE);
		osThreads[threadNums].threadFunc = taskFunc;
		osThreads[threadNums].state = ACTIVE;
		osThreads[threadNums].type = type;
		osThreads[threadNums].timeRunning = 0;
		osThreads[threadNums].timeSleeping = 0;
		osThreads[threadNums].priority = priority;
		
		
		// Now we need to set up the stack; it should 'look' like the thread is already running
		
		// First is xpsr, the status register. If bit 24 is not set and we are in thread mode we get a hard fault, so we just make sure it's set
		*(--osThreads[threadNums].threadStack) = 1<<24;
		
		// Next is the program counter, which is set to whatever the function we are running will be
		*(--osThreads[threadNums].threadStack) = (uint32_t)taskFunc;
		
		// Next is a set of important registers. These values are meaningless but we are setting them to be nonzero so that the 
		// compiler doesn't optimize out these lines
		*(--osThreads[threadNums].threadStack) = 0xE; //LR
		*(--osThreads[threadNums].threadStack) = 0xC; //R12
		*(--osThreads[threadNums].threadStack) = 0x3; //R3
		*(--osThreads[threadNums].threadStack) = 0x2; //R2
		*(--osThreads[threadNums].threadStack) = 0x1; //R1
		*(--osThreads[threadNums].threadStack) = 0x0; // R0
		
		
		// Now we have registers R11 to R4, which again are just set to random values so that we know for sure that they exist
		*(--osThreads[threadNums].threadStack) = 0xB; //R11
		*(--osThreads[threadNums].threadStack) = 0xA; //R10
		*(--osThreads[threadNums].threadStack) = 0x9; //R9
		*(--osThreads[threadNums].threadStack) = 0x8; //R8
		*(--osThreads[threadNums].threadStack) = 0x7; //R7
		*(--osThreads[threadNums].threadStack) = 0x6; //R6
		*(--osThreads[threadNums].threadStack) = 0x5; //R5
		*(--osThreads[threadNums].threadStack) = 0x4; //R4
		
		
		// Now the stack is set up, the thread's SP is correct, since we've been decrementing it.
		threadNums++;
		return (thread_id_t)(threadNums - 1); // Return the thread index
	}
	return -1;
}
