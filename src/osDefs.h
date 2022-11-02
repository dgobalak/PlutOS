#ifndef OS_DEFS_H
#define OS_DEFS_H

#include <stdint.h>
#include <LPC17xx.h>

#define MAX_THREADS 10 // Maximum number of threads that can be created

#define MAX_POOL_SIZE 0x2000 // Max size of all stacks combined
#define MSR_STACK_SIZE 512 // MSP Stack Size (First thread stack starts at this offset from MSP)
#define THREAD_STACK_SIZE 0x200 // Size of each thread's stack

#define SHPR3 *(uint32_t*)0xE000ED20 // System Handler Priority Register 3
#define ICSR *(uint32_t*)0xE000ED04 // Interrupt Control and State Register

#define LOWEST_PRIORITY 0 // Lowest priority for a thread

#define SYSTICK_MS 1
#define SYSTICK_SEC (SYSTICK_MS/1000.0)
#define SYSTICK_TICKS (SystemCoreClock*SYSTICK_SEC) // Systick ever 1ms

#define MAX_THREAD_RUNTIME_MS 5

#define IDLE_THREAD_ID 0

/**
 * @brief The state of a thread
 * 
 */
typedef enum thread_state {
	CREATED,
	ACTIVE,
	RUNNING,
	SLEEPING,
	BLOCKED,
	DESTROYED
} thread_state_t;

/**
 * @brief The priority value of a thread
 * @note Larger number results in a higher priority
 * 
 */
typedef uint32_t thread_priority_t;

/**
 * @brief The ID number of a thread
 * 
 */
typedef int32_t thread_id_t;

/**
 * @brief An elapsed time in ms
 * 
 */
typedef uint32_t ms_time_t;

/**
 * @brief The thread structure
 * @param threadStack The PSP of the thread
 * @param threadFunc The function that the thread will run
 * @param threadState The state of the thread
 * @param timeSinceSwitch The number of ms since the last context switch
 * @param threadPriority The priority of the thread
 */
typedef struct osthread {
	volatile uint32_t * threadStack;
	void (*threadFunc)(void * args);
	volatile thread_state_t state;
	volatile ms_time_t timeRunning;
	volatile ms_time_t timeSleeping;
	thread_priority_t priority; // TODO: Implement priorities
} osthread_t;

#endif // OS_DEFS_H
