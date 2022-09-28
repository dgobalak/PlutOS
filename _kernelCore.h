#ifndef _KERNEL_CORE_H
#define _KERNEL_CORE_H

//initializes memory structures and interrupts necessary to run the kernel
void kernelInit(void);

//called by the kernel to schedule which threads to run
void osSched(void);


#endif //_KERNEL_CORE_H