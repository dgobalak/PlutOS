	AREA handle_pend,CODE,READONLY
	EXTERN switchTask
	GLOBAL PendSV_Handler
	PRESERVE8
PendSV_Handler	
	MRS r0, PSP
	STMDB r0!, {r4-r11}
	BL switchTask
	MRS r0, PSP
	;Move the return constant into LR
	MOV LR,#0xFFFFFFFD
	LDMIA r0!, {r4-r11}
	MSR PSP, r0
	
	;Return from this function
	BX LR

	END