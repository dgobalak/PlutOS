#include <LPC17xx.h>
#include <stdio.h>

#include "uart.h"
#include "_threadsCore.h"
#include "_kernelCore.h"
#include "osMutex.h"

// Mutex to protect printing
static osmutex_t mutex;
static mutex_handle_t mutexHandle = &mutex;

// Mutex to protect access to global counter
static osmutex_t counterMutex;
static mutex_handle_t counterMutexHandle = &counterMutex;

// Mutex to protect the LEDs
static osmutex_t ledMutex;
static mutex_handle_t ledMutexHandle = &ledMutex;

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task1(void* args) {
	while(1) {
		if (osMutexAcquire(mutexHandle, 0, true)) {
			for (int i = 0; i < 5; i++) {
				printf("Task 1\n");
				osYield();
			}
			osMutexRelease(mutexHandle);
		}
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
		if (osMutexAcquire(mutexHandle, 0, true)) {
			for (int i = 0; i < 5; i++) {
				printf("Task 2\n");
				osYield();
			}
			osMutexRelease(mutexHandle);
		}
		osYield();
	}
}

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task3(void* args) {
	while(1) {
		if (osMutexAcquire(mutexHandle, 0, true)) {
			for (int i = 0; i < 5; i++) {
				printf("Task 3\n");
				osYield();
			}
			osMutexRelease(mutexHandle);
		}
		osYield();
	}
}

/**
 * @brief Display a byte on the LEDs
 * 
 * @param x The value to display on the LEDs
 */
void turnOnLED(unsigned int x) {
	unsigned int gpio1Mask;
	
	LPC_GPIO1->FIOCLR |= 0xBU << 28;
	LPC_GPIO2->FIOCLR |= 0x3E;
		
	gpio1Mask = x & 3;
	gpio1Mask |= (x & 4) << 1;
	LPC_GPIO1->FIOSET |= gpio1Mask << 28;
	LPC_GPIO2->FIOSET |= (x & 0xF8) >> 1;
	
	for(int i = 0; i < 5000000; i++);
}

unsigned int t12Counter = 0;

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task4(void* args) {
	while(1) {
		osMutexAcquire(counterMutexHandle, 0, true);
		t12Counter++;
		osMutexRelease(counterMutexHandle);
		
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
		osMutexAcquire(counterMutexHandle, 0, true);
		osMutexAcquire(ledMutexHandle, 0, true);

		turnOnLED(t12Counter%47U);
		printf("%u\n", t12Counter); // Protected by LED mutex

		osMutexRelease(ledMutexHandle);
		osMutexRelease(counterMutexHandle);
		
		osYield();
	}
}

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task6(void* args) {
	while(1) {
		osMutexAcquire(ledMutexHandle, 0, true);

		turnOnLED(0x71U);
		printf("0x%x\n", 0x71U); // Protected by LED mutex

		osMutexRelease(ledMutexHandle);
		
		osYield();
	}
}

// #define LAB5_TEST1
#define LAB5_TEST2
int main(void) {	
	// Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
		
	LPC_GPIO1->FIODIR |= 0xBU<<28;
	LPC_GPIO2->FIODIR |= 0x7CU;
	
	// Initialize the kernel.
	kernelInit();
	
	// Create mutexes
	osMutexCreate(mutexHandle);
	osMutexCreate(counterMutexHandle);
	osMutexCreate(ledMutexHandle);
		
#ifdef LAB5_TEST1
	osNewThread(task1, 10000);
	osNewThread(task2, 10000);
	osNewThread(task3, 10000);
#elif defined(LAB5_TEST2)
	osNewThread(task4, 10000);
	osNewThread(task5, 10000);
	osNewThread(task6, 10000);
#endif
	
	// Start kernel and start running first thread
	osKernelStart();
	
	while(1);
}
