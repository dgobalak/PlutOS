#ifndef OS_MUTEX_H
#define OS_MUTEX_H

#include "osDefs.h"

#include <stdint.h>
#include <stdbool.h>

typedef int32_t mutex_id_t;

typedef enum {
    TAKEN = 0,
    AVAILABLE = 1
} mutex_status_t;

typedef struct {
    mutex_id_t id;
    mutex_status_t status;
    thread_id_t owner;
    thread_id_t waitingThreads[MAX_WAITING_THREADS];
    thread_id_t firstWaitingThread;
    thread_id_t lastWaitingThread;
    uint32_t waitingThreadsCount;
} osmutex_t;

typedef osmutex_t *osmutex_handle_t;

/**
 * @brief Initializes a mutex
 * 
 * @param mutex Pointer to the mutex to initialize
 * @return mutex_id_t The ID of the mutex; -1 if the mutex could not be initialized
 */
mutex_id_t osMutexCreate(osmutex_t * mutex);

/**
 * @brief Acquire a mutex
 * 
 * @param id The ID of the mutex to acquire
 * @param timeout The timeout in milliseconds to wait for the mutex to become available
 * @param poll If true, the function will wait forever for the mutex to become available
 * @return true If the mutex was successfully locked; false otherwise
 */
bool osMutexAcquire(mutex_id_t id, ms_time_t timeout, bool poll);

/**
 * @brief Release a mutex
 * 
 * @param id The ID of the mutex to release
 * @return true If the mutex was successfully unlocked; false otherwise
 */
bool osMutexRelease(mutex_id_t id);

#endif // OS_MUTEX_H
