#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#include <stdint.h>

typedef struct thread_struct{
	uint32_t* threadStack;
	void (*threadFunc)(void* args);
} pluto_thread_t;

#endif // OS_CONFIG_H