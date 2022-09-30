#include "_kernelCore.h"

void kernelInit(void) {
	SHPR3 |= 0xFF << 16;
}

void osYield(void) {
	ICSR |= 1<<28;
	__asm("isb");
}
