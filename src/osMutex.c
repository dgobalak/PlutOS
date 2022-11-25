#include "osMutex.h"
#include "osDefs.h"
#include "_kernelCore.h"

#include <stdbool.h>
#include <stdio.h>

extern thread_id_t osCurrentTask; // Current task ID
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads

bool osMutexCreate(mutex_handle_t handle) {
    if (handle == NULL)
        return false;

    handle->status = AVAILABLE;
    handle->owner = -1;

    for (int i = 0; i < MAX_WAITING_THREADS; i++) {
        handle->waitingThreads[i] = -1;
    }

    handle->firstWaitingThread = -1;
    handle->lastWaitingThread = -1;
    handle->waitingThreadsCount = 0;

    return true;
}

bool osMutexAcquire(mutex_handle_t handle, ms_time_t timeout, bool osWaitForever) {
    if (handle == NULL)
        return false;

    if (handle->status == AVAILABLE) {
        handle->status = TAKEN;
        handle->owner = osCurrentTask;
        return true;
    }

    // Check if current task already owns the mutex
    if (handle->owner == osCurrentTask)
        return false;

    // Check if waiting threads array is full
    if (handle->waitingThreadsCount == MAX_WAITING_THREADS)
        return false;
    
    // Add current task to waiting threads queue
    handle->lastWaitingThread = (handle->lastWaitingThread + 1) % MAX_WAITING_THREADS;
    handle->waitingThreads[handle->lastWaitingThread] = osCurrentTask;
    handle->waitingThreadsCount++;

    if (osWaitForever) {
        osThreads[osCurrentTask].state = BLOCKED;
				osYieldNoReset();
    } else {
        osSleepNoReset(timeout);
    }

    // If this line is reached, osMutexRelease should have given this thread the mutex (or the timeout has expired)
    if (handle->owner == osCurrentTask) {
        handle->status = TAKEN;
        return true;
    }
		
		// handle->firstWaitingThread = (handle->firstWaitingThread + 1) % MAX_WAITING_THREADS;
    // handle->waitingThreadsCount--;
    
		return false;
}

bool osMutexRelease(mutex_handle_t handle) {
    if (handle == NULL)
        return false;

    if (handle->status == AVAILABLE)
        return false;

    if (handle->owner != osCurrentTask)
        return false;

    handle->status = AVAILABLE;
    handle->owner = handle->waitingThreads[handle->firstWaitingThread];
    handle->firstWaitingThread = (handle->firstWaitingThread + 1) % MAX_WAITING_THREADS;
    handle->waitingThreadsCount--;

    osThreads[handle->owner].state = ACTIVE;

    return true;
}

bool osMutexDelete(mutex_handle_t handle) {
    if (handle == NULL)
        return false;

    if (handle->status == TAKEN)
        return false;

    // handle = NULL;

    return true;
}
