;-------------------------------------------------------------------------------
; includes
;-------------------------------------------------------------------------------

		#include "msp430.h"
		#include "memmodel.h"
		#include "asm.h"

;-------------------------------------------------------------------------------
; defines for context switching
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

cpu_trap_\tnum\()_stash_context:
	CPU_SAVE_CONTEXT
	; MOV sp, g_cpu_trap_sp			; stash the stack pointer somewhere

cpu_trap_\tnum\()_check_stack_switch_needed:
	; MOV g_cpu_trap_stack, r12
	; MOV g_cpu_trap_stack, r13
	; ADD #CPU_TRAP_STACK_SIZE, r13

	; cmp.w sp, r12	; r12 >= sp
	; jc cpu_trap_\tnum\()_post_stack_switch

	; cmp.w r13, sp	; sp >= r13
	; jc cpu_trap_\tnum\()_post_stack_switch

cpu_trap_\tnum\()_switch_stacks:
	; MOV r13, sp					; switch to the base of the ISR stack

cpu_trap_\tnum\()_post_stack_switch:
	MOV #\tnum, r12
	CALL #\fn					; fn(tnum) - executes on the ISR stack

cpu_trap_\tnum\()_restore_context:
	; MOV &g_cpu_trap_sp, sp			; refetch the stack stash
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
