#ifndef _KERNEL_CORE_H
#define _KERNEL_CORE_H

#include <stdint.h>
#include <stdbool.h>

//initializes memory structures and interrupts necessary to run the kernel
void kernelInit(void);

//called by the kernel to schedule which threads to run
void osYield(void);

// Sets the value of PSP to threadStack and ensures that the microcontroller
// is using that value by changing the CONTROL register.
void setThreadingWithPSP(uint32_t* threadStack);

//starts the kernel if threads have been created. Returns false otherwise
bool osKernelStart(void);

//a C function to help us to switch PSP so we don't have to do this in assembly
int task_switch(void);

#endif //_KERNEL_CORE_H
