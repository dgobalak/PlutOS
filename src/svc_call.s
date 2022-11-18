	AREA handle_pend,CODE,READONLY
	EXTERN switchTask
	GLOBAL PendSV_Handler
	GLOBAL SVC_Handler
	PRESERVE8
PendSV_Handler	
	;Store context
	MRS r0, PSP
	STMDB r0!, {r4-r11}
	
	;Get new thread's stack pointer
	BL switchTask
	MRS r0, PSP
	
	;Move the return constant into LR
	;to go back to thread mode and use PSP
	MOV LR,#0xFFFFFFFD
	
	;Load new context
	LDMIA r0!, {r4-r11}
	MSR PSP, r0
	
	;Return from this function
	BX LR

SVC_Handler
	;Define external handler function
	EXTERN SVC_Handler_Main
		
	;Test if called from thread or handler mode
	;and whether MSP or PSP is used
	TST LR, #4
	
	;Load r0 with either MSP or PSP
	ITE EQ
	MRSEQ r0, MSP
	MRSNE r0, PSP
	
	;Jump to the handler
	B SVC_Handler_Main
	
	END
