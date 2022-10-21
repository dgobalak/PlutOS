#ifndef OS_DEFS_H
#define OS_DEFS_H

#include <stdint.h>
#include <LPC17xx.h>

#define MAX_THREADS 10

#define MAX_POOL_SIZE 0x2000
#define STACK_SIZE 512

#define MSR_STACK_SIZE 512
#define THREAD_STACK_SIZE 0x200

#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0xE000ED04

#define LOWEST_PRIORITY 0

typedef enum thread_state {
	CREATED,
	ACTIVE,
	WAITING,
	DESTROYED
} thread_state_t;

// Larger number results in a higher priority
typedef uint32_t thread_priority_t;

typedef struct osthread {
	uint32_t * threadStack;
	void (*threadFunc)(void * args);
	thread_state_t state;
	thread_priority_t priority; // TODO: Implement priorities
} osthread_t;


#endif // OS_DEFS_H
