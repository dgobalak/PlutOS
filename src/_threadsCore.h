#ifndef _THREADS_CORE_H
#define _THREADS_CORE_H

#include "osDefs.h"

#include <LPC17xx.h>
#include <stdint.h>

#define MSP_STORAGE_ADDR 0x00

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
 * @brief Creates a new periodic thread
 * 
 * @param threadBuffer The buffer to store the thread
 * @param taskFunc The function that the thread will run
 * @param deadline The deadline of the thread
 * @param period The period of the thread
 * @return int The thread ID of the new thread. Returns -1 if error occurs
 */
thread_handle_t osNewPeriodicThread(thread_handle_t threadBuffer, void (*taskFunc)(void*args), ms_time_t deadline, ms_time_t period);

/**
 * @brief Creates a new thread
 * 
 * @param threadBuffer The buffer to store the thread
 * @param taskFunc The function that the thread will run
 * @param deadline The deadline of the thread
 * @return int The thread ID of the new thread. Returns -1 if error occurs
 */
thread_handle_t osNewThread(thread_handle_t threadBuffer, void (*taskFunc)(void*args), ms_time_t deadline);

#endif // _THREADS_CORE_H
