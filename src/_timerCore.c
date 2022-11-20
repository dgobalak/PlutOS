#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>
#include <stdbool.h>

extern thread_handle_t taskListHead = NULL; // Head of the task list
extern thread_handle_t taskListTail = NULL; // Tail of the task list
extern int taskCount = 0; // Number of tasks in the task linked list

extern thread_handle_t currentTaskHandle = NULL; // Handle for current running task
extern thread_handle_t idleTaskHandle = NULL; // Handle for the idle task

void SysTick_Handler(void) {
	updateTimers();
}

void updateTimers(void) {
	bool switchRequired = false; // Only set if a thread has a deadline earlier than the current thread

	// Iterate through all threads
	thread_handle_t handle = taskListHead;
	for (int i = 0; i < taskCount; i++) {
		if (handle == NULL)
			break;

		// Don't need to do anything for IDLE thread
		if (handle == idleTaskHandle) {
			handle = handle->next;
			continue;
		}

		if (handle->state == SLEEPING) {
			handle->sleepTimeRemaining--;

			if (handle->sleepTimeRemaining != 0) {
				handle = handle->next;
				continue;
			}

			// Thread is ready to wake up; sleep time is zero
			handle->state = ACTIVE;

		} else if (handle->state == ACTIVE || handle->state == RUNNING) {		
			if (handle->deadlineCounter > 0)
				handle->deadlineCounter--;
			
			if (handle->deadlineCounter == 0)
				printf("Deadline not met for Thread %d\n", id); // Deadline missed
		}
		
		// Check if thread has an earlier deadline than the current thread
		if (handle->deadlineCounter < currentTaskHandle->deadlineCounter)
			switchRequired = true;

		handle = handle->next;
	}

	// If a thread woke up with a deadline earlier than the current thread, switch to that thread
	if (switchRequired)
		osYieldPreemptive();
}
