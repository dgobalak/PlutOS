#include "_threadHandler.h"
#include <LPC17xx.h>

int task_switch(void){
	//set the new PSP
	__set_PSP((uint32_t)osThreads[osCurrentTask].threadStack);
	return 1; //You are free to use this return value in your
	//assembly eventually. It will be placed in r0, so be sure to
	//access it before overwriting r0
}