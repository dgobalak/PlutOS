	AREA handle_pend,CODE,READONLY
	GLOBAL PendSV_Handler
	PRESERVE8
	EXTERN task_switch
PendSV_Handler	
	MRS r0, PSP
	STMDB r0!, {r4-r11}
	BL task_switch
	MRS r0, PSP
	;Move the return constant into LR
	MOV LR,#0xFFFFFFFD
	LDMIA r0!, {r4-r11}
	MSR PSP, r0
	
	;Return from this function
	BX LR

	END