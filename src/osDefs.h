#ifndef OS_DEFS_H
#define OS_DEFS_H

#include <stdint.h>
#include <stdbool.h>
#include <LPC17xx.h>

#define MAX_THREADS 10 // Maximum number of threads that can be created

#define MAX_POOL_SIZE 0x2000 // Max size of all stacks combined
#define MSR_STACK_SIZE 512 // MSP Stack Size (First thread stack starts at this offset from MSP)
#define THREAD_STACK_SIZE 512 // Size of each thread's stack

#if (THREAD_STACK_SIZE * MAX_THREADS) > MAX_POOL_SIZE
#error Maximum pool size is too small for the number of threads
#endif

#define SHPR3 *(uint32_t*)0xE000ED20 // System Handler Priority Register 3
#define SHPR2 *(uint32_t*)0xE000ED1C // System Handler Priority Register 2

#define ICSR *(uint32_t*)0xE000ED04 // Interrupt Control and State Register

#define SVC_PRIORITY 0xFU
#define PENDSV_PRIORITY 0xFEU
#define SYSTICK_PRIORITY 0xFFU

#define SYSTICK_MS 1 // Number of ms between each SysTick interrupt
#define SYSTICK_SEC (SYSTICK_MS/1000.0) // Number of seconds between each SysTick interrupt
#define SYSTICK_TICKS (SystemCoreClock*SYSTICK_SEC) // Number of cycles for the specified number of ms

// Assuming SYSTICK_MS is 1, this is the max number of ms a thread can run before getting pre-empted
#define MAX_THREAD_RUNTIME_MS 5

// The index of the idle task in the array of tasks
#define IDLE_THREAD_ID 0

// The deadline of the idle thread
#define IDLE_THREAD_DEADLINE UINT32_MAX

// Max number of threads that can be waiting on a mutex
#define MAX_WAITING_THREADS 10

#if MAX_WAITING_THREADS > MAX_THREADS
#error MAX_WAITING_THREADS should not be greater than MAX_THREADS
#endif

/**
 * @brief The state of a thread
 * 
 */
typedef enum thread_state {
	CREATED, // Not used
	ACTIVE, // Task is able to run
	RUNNING, // Task is running
	SLEEPING, // Task is sleeping
	BLOCKED,
	DESTROYED // Not used
} thread_state_t;

/**
 * @brief The ID number of a thread; the index in the thread array
 * 
 */
typedef int32_t thread_id_t;

/**
 * @brief An time in ms
 * 
 */
typedef uint32_t ms_time_t;

/**
 * @brief The thread structure
 * @param threadStack The PSP of the thread
 * @param threadFunc The function that the thread will run
 * @param state The state of the thread
 * @param sleepTimeRemaining The remaining time in ms that the thread must sleep
 * @param deadline The deadline of the thread
 * @param deadlineCounter Decrements from the deadline to 0
 * @param isPeriodic Whether the thread is periodic or not
 * @param period The period of the thread
 */
typedef struct osthread {
	volatile uint32_t * threadStack;
	void (*threadFunc)(void * args);
	volatile thread_state_t state;
	volatile ms_time_t sleepTimeRemaining;
	ms_time_t deadline;
	volatile ms_time_t deadlineCounter;
	volatile bool isPeriodic;
	ms_time_t period;
} osthread_t;

#endif // OS_DEFS_H
