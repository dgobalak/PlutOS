#include "osMutex.h"
#include "osDefs.h"
#include "_kernelCore.h"

#include <stdbool.h>

static mutex_id_t lastCreatedID = -1;
static osmutex_handle_t osMutexes[MAX_MUTEXES];

extern thread_id_t osCurrentTask; // Current task ID
extern osthread_t osThreads[MAX_THREADS]; // Array of all threads
extern uint32_t totalThreads; // Number of created threads

mutex_id_t osMutexCreate(osmutex_handle_t mutex) {
    if (mutex == NULL)
        return -1;

    if (lastCreatedID == (MAX_MUTEXES - 1))
        return -1;

    lastCreatedID++;
    osMutexes[lastCreatedID] = mutex;

    mutex->id = lastCreatedID;
    mutex->status = AVAILABLE;
    mutex->owner = -1;

    for (int i = 0; i < MAX_WAITING_THREADS; i++) {
        mutex->waitingThreads[i] = -1;
    }

    mutex->firstWaitingThread = -1;
    mutex->lastWaitingThread = -1;

    return mutex->id;
}

bool osMutexAcquire(mutex_id_t id, ms_time_t timeout, bool osWaitForever) {
    if (id < 0 || id > 
    )
        return false;

    osmutex_handle_t mutex = osMutexes[id];

    if (mutex->status == AVAILABLE) {
        mutex->status = TAKEN;
        mutex->owner = osCurrentTask;
        return true;
    }

    // Check if current task already owns the mutex
    if (mutex->owner == osCurrentTask)
        return false;

    // Check if waiting threads array is full
    if (mutex->waitingThreadsCount == MAX_WAITING_THREADS)
        return false;
    
    // Add current task to waiting threads queue
    mutex->lastWaitingThread = (mutex->lastWaitingThread + 1) % MAX_WAITING_THREADS;
    mutex->waitingThreads[mutex->lastWaitingThread] = osCurrentTask;

    threads[osCurrentTask]->state = SLEEPING;

    if (osWaitForever) {
        while (mutex->status != AVAILABLE);
    } else {
        osSleep(timeout);
    }

    // osMutexRelease should have selected the next thread to acquire the mutex
    if (mutex->owner == osCurrentTask) {
        mutex->status = TAKEN;
        mutex->owner = osCurrentTask;
        return true;
    }

    return false;
}

bool osMutexRelease(mutex_id_t id) {
    if (id < 0 || id > lastCreatedID)
        return false;

    osmutex_handle_t mutex = osMutexes[id];

    if (mutex->status == AVAILABLE)
        return false;

    if (mutex->owner != osCurrentTask)
        return false;

    mutex->status = AVAILABLE;
    mutex->owner = mutex->waitingThreads[mutex->firstWaitingThread];
    mutex->firstWaitingThread = (mutex->firstWaitingThread + 1) % MAX_WAITING_THREADS;

    osThreads[mutex->owner]->state = ACTIVE;

    return true;
}