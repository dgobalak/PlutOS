#ifndef _THREADS_CORE_H
#define _THREADS_CORE_H

#include <LPC17xx.h>
#include <stdint.h>

#define MAX_POOL_SIZE 0x2000
#define STACK_SIZE 512

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation(void);

//Returns the address of a new PSP with offset of “offset” bytes from MSP. 
uint32_t* getNewThreadStack(uint32_t offset);

//Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register
void setThreadingWithPSP(uint32_t* threadStack);

#endif // _THREADS_CORE_H
