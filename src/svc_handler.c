#include "svc_handler.h"
#include "_kernelCore.h"

#include <stdint.h>

void svcYield(void) {
	yieldCurrentTask(NUM_REGS_TO_PUSH);
	osSched(); // Choose next task			
	pendPendSV();
}

void SVC_Handler_Main(uint32_t *svc_args) {
	char call = ((char*)svc_args[6])[-2];
	
	switch(call) {
		case SVC_YIELD_SWITCH:
			svcYield();
			break;
	}

}
