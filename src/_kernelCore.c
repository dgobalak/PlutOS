#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>

int osCurrentTask = -1; // Index for current running task

osthread_t osThreads[MAX_THREADS]; // Array of all threads

int threadNums; // number of created threads

bool osYieldMutex = true;

uint32_t mspAddr; //the initial address of the MSP

/**
 * @brief Idle task that prints a message and then yields
 * @note It exists so that there's at least 1 function that
 * can be run when there are no other threads to run
 * 
 * @param args Thread arguments
 */
static void osIdleTask(void* args) {
	while(1) {
		printf("In Idle Task\n");
		osYield();
	}
}

void kernelInit(void) {
	// Set PendSV to the weakest priority we can set
	SHPR3 |= 0xFF << 16;
	
	// Initialize the address of the MSP
	uint32_t* MSP_Original = 0;
	mspAddr = *MSP_Original;
	
	SysTick_Config(SYSTICK_TICKS);
	
	// Create the idle thread
	osNewThread(osIdleTask, LOWEST_PRIORITY);
}

void osSched(void) {
	// Choose next thread to run
	// TODO: Make use of the priority when choosing next task
	// For now, just do round robin scheduling with no priorities
	
	thread_id_t id = 0;
	do {
		osCurrentTask = (osCurrentTask+1)%(threadNums);
		id++;
		
		if (id >= threadNums) {
			break;
		}
		
	}	while ((osThreads[osCurrentTask].state != ACTIVE) || (osCurrentTask == IDLE_THREAD_ID));
	
	if (osThreads[osCurrentTask].state != ACTIVE) {
			osCurrentTask = IDLE_THREAD_ID;
	}
	
	printf(""); // DO NOT REMOVE. THIS PRINT PREVENTS A TIMING ISSUE
	
	// Configure thread
	osThreads[osCurrentTask].state = RUNNING;
	osThreads[osCurrentTask].timeRunning = MAX_THREAD_RUNTIME_MS;
}

static void pendPendSV(void) {
	// Pend the PendSV interrupt
	ICSR |= 1<<28; // Set PENDSVSET to 1 to make PendSV exception pending
	__asm("isb");
}

static void yieldCurrentTask(uint8_t stackDiff) {
	if (osCurrentTask >= 0) {
		// Yield the current task (It could be in the RUNNING state or in the SLEEPING state)
		osThreads[osCurrentTask].state = (osThreads[osCurrentTask].state == SLEEPING) ? SLEEPING : ACTIVE;
		osThreads[osCurrentTask].threadStack = (uint32_t*)(__get_PSP() - stackDiff*4); //we are about to push `stackDiff` uint32_t's
	}
}

void osYield(void) {
	osYieldMutex = false;

	yieldCurrentTask(16);
	
	osSched(); // Choose next task
	
	osYieldMutex = true;
	
	pendPendSV();
}

void osYieldFromSysTick(void) {
	yieldCurrentTask(8);
			
	osSched(); // Choose next task

	pendPendSV();
}

void osSleep(ms_time_t sleepTime) {
	osThreads[osCurrentTask].timeSleeping = sleepTime;
	osThreads[osCurrentTask].state = SLEEPING;
	osYield();
}

bool osKernelStart() {
	if(threadNums > 0) {
		osCurrentTask = -1;
		__set_CONTROL(1<<1);
		__set_PSP((uint32_t)osThreads[0].threadStack);
		osYield();
	}
	// If we get here, we failed to start the kernel
	return false;
}

void setThreadingWithPSP(uint32_t* threadStack) {
	__set_PSP((uint32_t)threadStack); // Set PSP to threadStack
	__set_CONTROL(1<<1); // Switch to threading mode
	
}

int switchTask(void){
	__set_PSP((uint32_t)osThreads[osCurrentTask].threadStack); //set the new PSP

	// The return value can be accessed in the assembly code. Access it from r0 before overwriting r0
	return 1; 
}
