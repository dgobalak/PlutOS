#ifndef _KERNEL_CORE_H
#define _KERNEL_CORE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initializes memory structures and interrupts necessary to run the kernel
 * 
 */
void kernelInit(void);

/**
 * @brief Called by the kernel to yield current task (Switch to next available task)
 * 
 */
void osYield(void);

/**
 * @brief Determine the next task to be run
 *
 */
void osSched(void);

/**
 * @brief Set the value of PSP to threadStack and ensures that the microcontroller is using 
 * that value by changing the CONTROL register
 * 
 * @param threadStack The new value of PSP
 */
void setThreadingWithPSP(uint32_t* threadStack);

/**
 * @brief Starts the kernel if threads have been created. Returns false otherwise
 * 
 * @return Never returns if the kernel has been started correctly, false otherwise
 */
bool osKernelStart(void);

/**
 * @brief Switch the PSP to the threadStack of the next thread to run
 * 
 * @return int For now, always returns 1
 */
int switchTask(void);

#endif //_KERNEL_CORE_H
