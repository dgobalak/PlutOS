//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

//this file sets up the UART
#include "uart.h"

//This file is our threading library
#include "_threadsCore.h"

//Include the kernel
#include "_kernelCore.h"

int x = 0;
int y = 0;

void task1(void* args)
{
	while(1)
	{
		x++;
	  printf("In task 1. x is: %d\n",x);
		osYield();
	}
}
void task2(void* args)
{
	while(1)
	{
		y++;
		printf("In task 2. y is: %d\n",y);
		osYield();
	}
}

void osIdleTask(void* args)
{
	while(1)
	{
		printf("in task 0\n");
		osYield();
	}
	
}

//This is C. The expected function heading is int main(void)
int main( void ) 
{
	//Always call this function at the start. It sets up various peripherals, the clock etc. If you don't call this
	//you may see some weird behaviour
	SystemInit();

	//Printf now goes to the UART, so be sure to have PuTTY open and connected
	printf("Hello, world!\r\n");
	
	//We need to get our stack location. In Lab Project 2 this will be done 
	//in a Kernel function instead
	uint32_t* initMSP = getMSPInitialLocation();
	
	//Let's see what it is
	printf("MSP Initially: %x\n",(uint32_t)initMSP);

	
	//Initialize the kernel. We'll need to do this every lab project
	kernelInit();
	
	//set up my threads
	osNewThread(osIdleTask);
	osNewThread(task1);
	osNewThread(task2);
	
	//Now start the kernel, which will run our first thread
	osKernelStart();
	
	//Your code should always terminate in an endless loop if it is done. If you don't
	//the processor will enter a hardfault and will be weird about it the whole time
	while(1);
}
