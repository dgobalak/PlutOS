#ifndef OS_MUTEX_H
#define OS_MUTEX_H

#include "osDefs.h"

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief The status of a mutex
 */
typedef enum {
    TAKEN = 0,  // Mutex is taken
    AVAILABLE = 1 // Mutex is available
} mutex_status_t;

/**
 * @brief The mutex structure
 * 
 * @param status The status of the mutex
 * @param owner The ID of the thread that owns the mutex
 * @param waitingThreads The IDs of the threads that are waiting on the mutex
 * @param waitingThreadsCount The number of threads that are waiting on the mutex
 * @param firstWaitingThread The index of the first waiting thread in the waitingThreads array
 * @param lastWaitingThread The index of the last waiting thread in the waitingThreads array
 * @param prev The previous mutex handle in the linked list
 * @param next The next mutex handle in the linked list
 */
typedef struct {
    mutex_status_t status;
    thread_id_t owner;
    thread_id_t waitingThreads[MAX_WAITING_THREADS];
    int firstWaitingThread;
    int lastWaitingThread;
    uint32_t waitingThreadsCount;
    mutex_handle_t prev;
    mutex_handle_t next;
} osmutex_t;

// The mutex handle type
typedef osmutex_t *mutex_handle_t;

/**
 * @brief Initializes a mutex
 * 
 * @param mutex The mutex handle
 * @return true if the mutex was initialized successfully, false otherwise
 * @note The mutex must be allocated before calling this function
 */
bool osMutexCreate(mutex_handle_t mutex);

/**
 * @brief Acquire a mutex
 * 
 * @param handle The handle of the mutex to acquire
 * @param timeout The timeout in milliseconds to wait for the mutex to become available
 * @param osWaitForever If true, the function will wait forever for the mutex to become available
 * @return true if the mutex was successfully locked; false otherwise
 */
bool osMutexAcquire(mutex_handle_t handle, ms_time_t timeout, bool osWaitForever);

/**
 * @brief Release a mutex
 * 
 * @param handle The handle of the mutex to release
 * @return true if the mutex was successfully unlocked; false otherwise
 */
bool osMutexRelease(mutex_handle_t handle);

#endif // OS_MUTEX_H
