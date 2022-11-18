#ifndef _KERNEL_CORE_H
#define _KERNEL_CORE_H

#include "osDefs.h"

#include <stdint.h>
#include <stdbool.h>

#define NUM_REGS_TO_PUSH 8U // Number of registers to push to the stack

#define THREADING_MODE 1<<1
#define SET_PENDSVET 1<<28

/**
 * @brief Initializes memory structures and interrupts necessary to run the kernel
 * 
 */
void kernelInit(void);

/**
 * @brief Pend the PendSV exception
 */
void pendPendSV(void);

/**
 * @brief Remove current task from RUNNING state and adjust its stack pointer
 */
void yieldCurrentTask(uint8_t stackDiff);

/**
 * @brief Called to yield current task (Switch to next available task)
 * 
 */
void osYield(void);

/**
 * @brief Called to yield current task preemptively (Don't reset deadline)
 * 
 */
void osYieldPreemptive(void);

/**
 * @brief Make the current task sleep for `sleepTime` ms
 */
void osSleep(ms_time_t sleepTime);

/**
 * @brief Determine the next task to be run
 * @note If no user task is ACTIVE, then the idle task runs
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
