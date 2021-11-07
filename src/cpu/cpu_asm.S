;-------------------------------------------------------------------------------
; includes
;-------------------------------------------------------------------------------

		#include "msp430.h"
		#include "memmodel.h"
		#include "asm.h"

;-------------------------------------------------------------------------------
; defines for trap handling and context switching
;-------------------------------------------------------------------------------
.macro CPU_SAVE_CONTEXT stash
		#if defined(__MSP430_HAS_MSP430XV2_CPU__) || \
			defined(__MSP430_HAS_MSP430X_CPU__)

			PUSHM #12, r15

		#else
			PUSH r15
			PUSH r14
			PUSH r13
			PUSH r12
			PUSH r11
			PUSH r10
			PUSH r9
			PUSH r8
			PUSH r7
			PUSH r6
			PUSH r5
			PUSH r4
		#endif
.endm

.macro CPU_RESTORE_CONTEXT stash
		#if defined(__MSP430_HAS_MSP430XV2_CPU__) || \
			defined(__MSP430_HAS_MSP430X_CPU__)
			POPM #12, r15
		#else
			POP r4
			POP r5
			POP r6
			POP r7
			POP r8
			POP r9
			POP r10
			POP r11
			POP r12
			POP r13
			POP r14
			POP r15
		#endif
.endm

;-------------------------------------------------------------------------------
; defines for cpu interrupt vectoring
;-------------------------------------------------------------------------------
; Stores the interrupt number and vectors execution to a given function
.macro CPU_TRAP_HANDLER fn tnum
	CPU_SAVE_CONTEXT

	MOV g_
	; store off SP in current thread

	MOV g_cpu_trap_stack, sp

	MOV #\tnum, r12
	CALL #\fn					; fn(tnum) - executes on the ISR stack

	; reload SP of new current thread 

	CPU_RESTORE_CONTEXT

cpu_trap_\tnum\()_finish:
	bic.w #LPM3_bits, sr
	nop
	reti

.endm

; Link a default CPU trap handler with an interrupt vector
.macro CPU_TRAP tnum
cpu_trap_\tnum\()_handler:
	CPU_TRAP_HANDLER cpu_trap \tnum
.endm

; Link an illegal CPU trap handler with an interrupt vector
.macro CPU_BAD_TRAP tnum
cpu_trap_\tnum\()_handler:
	CPU_TRAP_HANDLER cpu_panic \tnum
.endm

; allocate space for an interrupt handler
.macro CPU_TRAP_DEFINE tnum
	.sect __interrupt_vector_\tnum, "ax", @progbits
	.word cpu_trap_\tnum\()_handler
.endm
