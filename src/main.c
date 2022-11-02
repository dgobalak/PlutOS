#include <LPC17xx.h>
#include <stdio.h>

#include "uart.h"
#include "_threadsCore.h"
#include "_kernelCore.h"

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task1(void* args) {
	while(1) {
		printf("In Task 1\n");
		osYield();
	}
}

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task2(void* args) {
	while(1) {
		printf("In Task 2\n");
		osSleep(1);
	}
}

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task3(void* args) {
	while(1) {
		printf("In Task 3\n");
	}
}

int main(void) {
	// Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
	
	// Initialize the kernel.
	kernelInit();
	
	// Create threads; the priority values are not used yet
	osNewThread(task1, LOWEST_PRIORITY);
	osNewThread(task2, LOWEST_PRIORITY);
	osNewThread(task3, LOWEST_PRIORITY);

	// Start kernel and start running first thread
	osKernelStart();
	
	while(1);
}
