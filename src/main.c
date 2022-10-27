#include <LPC17xx.h>
#include <stdio.h>

#include "uart.h"
#include "_threadsCore.h"
#include "_kernelCore.h"

/**
 * @brief Test thread function that prints the value
 * of a counter and then yields
 * 
 * @param args Thread arguments
 */
void task1(void* args) {
	int x = 0;
	while(1) {
		x++;
		printf("In task 1. x is: %d\n",x);
		osYield();
	}
}

/**
 * @brief Test thread function that prints the value
 * of a counter and then yields
 * 
 * @param args Thread arguments
 */
void task2(void* args) {
	int y = 0;
	while(1) {
		y++;
		printf("In task 2. y is: %d\n",y);
		osYield();
	}
}

/**
 * @brief Idle task that prints a message and then yields
 * @note It exists so that there's at least 1 function that
 * can be run when there are no other threads to run
 * 
 * @param args Thread arguments
 */
void osIdleTask(void* args) {
	while(1) {
		printf("in task 0\n");
		osYield();
	}
}

int main(void) {
	// Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
	
	// Initialize the kernel.
	kernelInit();
	
	// Create threads; the priority values are not used yet
	osNewThread(osIdleTask, LOWEST_PRIORITY);
	osNewThread(task1, LOWEST_PRIORITY);
	osNewThread(task2, LOWEST_PRIORITY);
	
	// Start kernel and start running first thread
	osKernelStart();
	
	while(1);
}
