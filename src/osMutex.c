#include "osMutex.h"
#include "osDefs.h"
#include "_kernelCore.h"

#include <stdbool.h>

extern thread_handle_t taskListHead = NULL; // Head of the task list
extern thread_handle_t taskListTail = NULL; // Tail of the task list
extern int taskCount = 0; // Number of tasks in the task linked list

extern thread_handle_t currentTaskHandle = NULL; // Handle for current running task

mutex_handle_t osMutexCreate(mutex_handle_t handle) {
    if (handle == NULL)
        return false;

    handle->status = AVAILABLE;
    handle->owner = -1;

    handle->firstWaitingThread = NULL;
    handle->lastWaitingThread = NULL;
    handle->waitingThreadsCount = 0;

    return true;
}

bool osMutexAcquire(mutex_handle_t handle, ms_time_t timeout, bool osWaitForever) {
    if (handle == NULL)
        return false;

    if (handle->status == AVAILABLE) {
        handle->status = TAKEN;
        handle->owner = currentTaskHandle;
        return true;
    }

    // Check if current task already owns the mutex
    if (handle->owner == currentTaskHandle)
        return false;
    
    // Add current task to waiting threads queue
    handle->lastWaitingThread = (handle->lastWaitingThread + 1) % MAX_WAITING_THREADS;
    handle->waitingThreads[handle->lastWaitingThread] = osCurrentTask;
    handle->waitingThreadsCount++;

    osThreads[osCurrentTask]->state = SLEEPING;

    if (osWaitForever) {
        while (handle->owner != osCurrentTask) {
            osYield(); // Is this the intended behavior?
        }
    } else {
        osSleep(timeout);
    }

    // If this line is reached, osMutexRelease should have given this thread the mutex (or the timeout has expired)
    if (handle->owner == osCurrentTask) {
        handle->status = TAKEN;
        return true;
    }

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

    osThreads[handle->owner]->state = ACTIVE;

    return true;
}

bool osMutexDelete(mutex_handle_t handle) {
    if (handle == NULL)
        return false;

    if (handle->status == TAKEN)
        return false;

    *handle = NULL;
    handle = NULL;

    return true;
}