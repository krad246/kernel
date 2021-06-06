;-------------------------------------------------------------------------------
; MS-TOS MSP430 boot code
;-------------------------------------------------------------------------------
            
		#include "msp430.h"
		#include "memmodel.h"

;-------------------------------------------------------------------------------
            
		.text
		.global _start					
		.global main

;-------------------------------------------------------------------------------

_start:      
		MOV  	#__stack, sp					; initialize stack pointer

		;-------------------

		dint									; enter critical section
		nop

		;-------------------

		MOV		#(WDTPW | WDTHOLD), &WDTCTL 	; freeze watchdog
		
		;-------------------

		MOV		#__bssstart, r12
		clr.w	r13
		MOV		#__bsssize, r14

	.ifdef __MSP430X_LARGE__
		clr.w	r15
	.endif

		CALL	#memset							; zero-init .bss

		;-------------------

		MOV		#__datastart, r12
		MOV		#__romdatastart, r13

		CMP		r12, r13						; skip if no memory
												; needs to be moved
		jeq		_call_main
		MOV		#__romdatacopysize, r14
	
	.ifdef __MSP430X_LARGE__
		clr.w	r15
	.endif

		CALL	#memmove						; copy rom to ram

		;-------------------

_call_main:
		clr.w	R12									
		CALL	#main

_main_exit:
		bis.w	#(LPM3_bits | GIE),	sr
		nop

		jmp		_main_exit

;-------------------------------------------------------------------------------
;           stack pointer definition
;-------------------------------------------------------------------------------

        .sect 	.stack

;-------------------------------------------------------------------------------
;           interrupt vectors
;-------------------------------------------------------------------------------
        .sect   .resetvec, "ax", @progbits            
        .word  	_start

		.end
