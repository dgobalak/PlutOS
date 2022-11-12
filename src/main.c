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
		osSleep(2);
	}
}

/**
 * @brief Periodic test thread function
 * 
 * @param args Thread arguments
 */
void task3(void* args) {
	while(1) {
		printf("In Task 3\n");
	}
}

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task4(void* args) {
	while(1) {
		printf("In Task 4\n");
		osSleep(2);
	}
}

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task5(void* args) {
	while(1) {
		printf("In Task 5\n");
		osSleep(7);
	}
}

#define LAB3_TEST1

int main(void) {
	// Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
	
	// Initialize the kernel.
	kernelInit();
	
#ifdef LAB3_TEST1
	// Create threads; the priority values are not used yet
	osNewThread(task1, 15);
	osNewThread(task2, 15);
	osNewPeriodicThread(task3, 15, 3);
#else
	osNewThread(task4, LOWEST_PRIORITY);
	osNewThread(task5, LOWEST_PRIORITY);
#endif
	
	// Start kernel and start running first thread
	osKernelStart();
	
	while(1);
}
