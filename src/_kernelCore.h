#ifndef _KERNEL_CORE_H
#define _KERNEL_CORE_H

#include <stdint.h>

#define SHPR3 *(uint32_t*)0xE000ED20
#define ICSR *(uint32_t*)0xE000ED04

//initializes memory structures and interrupts necessary to run the kernel
void kernelInit(void);

//called by the kernel to schedule which threads to run
void osYield(void);


#endif //_KERNEL_CORE_H
