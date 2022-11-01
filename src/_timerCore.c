#include "_timerCore.h"
#include "_kernelCore.h"
#include "_threadsCore.h"
#include "osDefs.h"

#include <stdio.h>

extern volatile bool osYieldMutex;

static uint32_t ticks_elapsed = 0;

void SysTick_Handler(void) {
	ticks_elapsed++;
	if (osYieldMutex) {
		osYieldFromSysTick();
	}
}

void updateTimers(void) {
}
