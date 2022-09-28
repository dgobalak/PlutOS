	AREA handle_pend,CODE,READONLY
	GLOBAL PendSV_Handler
	PRESERVE8
PendSV_Handler	
	;Move the return constant into LR
	MOV LR,#0xFFFFFFFD
	
	;Return from this function
	BX LR
	
	END