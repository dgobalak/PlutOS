#ifndef OS_DEFS_H
#define OS_DEFS_H

#include <stdint.h>
#include <LPC17xx.h>

#define MAX_THREADS 10 // Maximum number of threads that can be created

#define MAX_POOL_SIZE 0x2000 // Max size of all stacks combined

#define MSR_STACK_SIZE 512
#define THREAD_STACK_SIZE 0x200 // Size of each thread's stack

#define SHPR3 *(uint32_t*)0xE000ED20 // System Handler Priority Register 3
#define ICSR *(uint32_t*)0xE000ED04 // Interrupt Control and State Register

#define LOWEST_PRIORITY 0 // Lowest priority for a thread

/**
 * @brief The state of a thread
 * 
 */
typedef enum thread_state {
	CREATED,
	ACTIVE,
	WAITING,
	DESTROYED
} thread_state_t;

/**
 * @brief The priority value of a thread
 * @note Larger number results in a higher priority
 * 
 */
typedef uint32_t thread_priority_t;

/**
 * @brief The thread structure
 * @param threadStack The PSP of the thread
 * @param threadFunc The function that the thread will run
 * @param threadState The state of the thread
 * @param threadPriority The priority of the thread
 */
typedef struct osthread {
	uint32_t * threadStack;
	void (*threadFunc)(void * args);
	thread_state_t state;
	thread_priority_t priority; // TODO: Implement priorities
} osthread_t;

#endif // OS_DEFS_H
