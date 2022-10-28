#ifndef _THREADS_CORE_H
#define _THREADS_CORE_H

#include "osDefs.h"

#include <LPC17xx.h>
#include <stdint.h>

/**
 * @brief Obtains the initial location of MSP by looking it up in the vector table
 * 
 * @return uint32_t* Initial location of MSP
 */
uint32_t* getMSPInitialLocation(void);

/**
 * @brief Get the New Thread Stack
 * 
 * @param offset The offset from MSP
 * @return uint32_t* Address of a new PSP with offset of 'offset' bytes from MSP. 
 * Returns NULL if offset is not divisible by stack size or if the stack pool is full
 */
uint32_t* getNewThreadStack(uint32_t offset);

/**
 * @brief Set the value of PSP to threadStack and ensures that the microcontroller is using 
 * that value by changing the CONTROL register
 * 
 * @param threadStack The new value of PSP
 */
void setThreadingWithPSP(uint32_t* threadStack);

/**
 * @brief Creates a new thread
 * 
 * @param taskFunc The function that the thread will run
 * @param priority The priority of the thread
 * @return thread_id_t The thread ID of the new thread. Returns -1 if error occurs
 */
thread_id_t osNewThread(void (*taskFunc)(void*args), thread_priority_t priority);



#endif // _THREADS_CORE_H
