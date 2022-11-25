#include <LPC17xx.h>
#include <stdio.h>

#include "uart.h"
#include "_threadsCore.h"
#include "_kernelCore.h"
#include "osMutex.h"

static osmutex_t mutex;
static mutex_handle_t mutexHandle = &mutex;

static osmutex_t counterMutex;
static mutex_handle_t counterMutexHandle = &counterMutex;

static osmutex_t ledMutex;
static mutex_handle_t ledMutexHandle = &ledMutex;

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task1(void* args) {
	while(1) {
		osMutexAcquire(mutexHandle, 10, true);
		for (int i = 0; i < 12000; i++) {
			printf("1\n");
			osYield();
		}
		osMutexRelease(mutexHandle);
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
		osMutexAcquire(mutexHandle, 10, true);
		for (int i = 0; i < 12000; i++) {
			printf("2\n");
			osYield();
		}
		osMutexRelease(mutexHandle);
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
		osMutexAcquire(mutexHandle, 10, true);
		for (int i = 0; i < 12000; i++) {
			printf("Task 3\n");
			osYield();
		}
		osMutexRelease(mutexHandle);
		osYield();
	}
}

void turnOnLED(unsigned int x) {
	unsigned int gpio1Mask;
	
	LPC_GPIO1->FIOCLR |= 0xBU << 28;
	LPC_GPIO2->FIOCLR |= 0x3E;
		
	gpio1Mask = x & 3;
	gpio1Mask |= (x & 4) << 1;
	LPC_GPIO1->FIOSET |= gpio1Mask << 28;
	LPC_GPIO2->FIOSET |= (x & 0xF8) >> 1;
	
	for(int i =0; i<100000; i++);
}

unsigned int t12Counter = 0;

/**
 * @brief Test thread function
 * 
 * @param args Thread arguments
 */
void task12(void* args) {
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
void task13(void* args) {	
	while(1) {
		osMutexAcquire(counterMutexHandle, 10, true);
		osMutexAcquire(ledMutexHandle, 10, true);

		turnOnLED(t12Counter%47U);
		
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
void task14(void* args) {
	while(1) {
		osMutexAcquire(ledMutexHandle, 10, true);
		turnOnLED(0x71U);
		osMutexRelease(ledMutexHandle);
		
		osYield();
	}
}


// #define LAB4_TEST1
// #define LAB4_TEST2
// #define LAB4_TEST3

#define LAB5_TEST1
// #define LAB5_TEST2

int main(void) {	
	// Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
	
	printf("-------------------------------------\n");
	
	LPC_GPIO1->FIODIR |= 0xBU<<28;
	LPC_GPIO2->FIODIR |= 0x7CU;
	
	// Initialize the kernel.
	kernelInit();
	
	osMutexCreate(mutexHandle);
	osMutexCreate(counterMutexHandle);
	osMutexCreate(ledMutexHandle);
	
#ifdef LAB5_TEST1
	osNewThread(task1, 100);
	osNewThread(task2, 100);
	osNewThread(task3, 100);
#elif defined(LAB5_TEST2)
	osNewThread(task12, 40);
	osNewThread(task13, 40);
	osNewThread(task14, 40);
#endif
	
	// Start kernel and start running first thread
	osKernelStart();
	
	while(1);
}
