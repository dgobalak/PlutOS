#include <LPC17xx.h>
#include <stdio.h>

#include "uart.h"
#include "_threadsCore.h"
#include "_kernelCore.h"

/**
 * @brief Periodic test thread function
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
 * @brief Periodic test thread function
 * 
 * @param args Thread arguments
 */
void task2(void* args) {
	while(1) {
		printf("In Task 2\n");
		osYield();
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
		osYield();
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
		osYield();
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
		osSleep(12);
	}
}

/**
 * @brief Periodic test thread function
 * 
 * @param args Thread arguments
 */
void task6(void* args) {
	while(1) {
		printf("In Task 6\n");
		osYield();
	}
}

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task7(void* args) {
	while(1) {
		printf("In Task 7\n");
		osSleep(2000);
	}
}

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task8(void* args) {
	while(1) {
		printf("In Task 8\n");
		osSleep(3000);
	}
}


#define LAB4_TEST1
// #define LAB4_TEST2
// #define LAB4_TEST3

int main(void) {	
	// Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
	
	printf("Hello, world!\n");
	
	// Initialize the kernel.
	kernelInit();
	
#ifdef LAB4_TEST1
	osNewPeriodicThread(task1, 20, 4); // 1/256Hz = 3.9ms
	osNewPeriodicThread(task2, 20, 10); // 1/100Hz = 10ms
	osNewPeriodicThread(task3, 20, 83); // 1/12Hz = 83.3ms
#elif defined(LAB4_TEST2)
	osNewThread(task4, 10);
	osNewThread(task5, 10);
	osNewPeriodicThread(task6, 10, 5); // 1/200Hz = 5ms
#elif defined(LAB4_TEST3)
	osNewThread(task7, 10);
	osNewThread(task8, 10);
#endif
	
	// Start kernel and start running first thread
	osKernelStart();
	
	while(1);
}
