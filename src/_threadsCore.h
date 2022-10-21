#ifndef _THREADS_CORE_H
#define _THREADS_CORE_H

#include <LPC17xx.h>
#include <stdint.h>

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation(void);

//Returns the address of a new PSP with offset of “offset” bytes from MSP. 
uint32_t* getNewThreadStack(uint32_t offset);

//Sets the value of PSP to threadStack and ensures that the microcontroller is using that value by changing the CONTROL register
void setThreadingWithPSP(uint32_t* threadStack);

// Create a new thread
int osNewThread(void (*taskFunc)(void*args));

#endif // _THREADS_CORE_H
