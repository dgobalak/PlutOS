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
		printf("1\n");
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
		printf("2\n");
		osYield();
	}
}

/**
 * @brief Test thread function that prints the value
 * of a counter and then yields
 * 
 * @param args Thread arguments
 */
void task3(void* args) {
	int z = 0;
	while(1) {
		z++;
		printf("3\n");
		osYield();
	}
}

int main(void) {
	// Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
	
	// Initialize the kernel.
	kernelInit();
	
	// Create threads; the priority values are not used yet
	osNewThread(task1, NON_PERIODIC, LOWEST_PRIORITY);
	osNewThread(task2, NON_PERIODIC, LOWEST_PRIORITY);
	osNewThread(task3, NON_PERIODIC, LOWEST_PRIORITY);

	// Start kernel and start running first thread
	osKernelStart();
	
	while(1);
}
