#ifndef MEMMODEL_H_
#define MEMMODEL_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#ifndef __ASSEMBLER__
	#include <stdint.h>
#endif

/*******************************************************************************
 * defines
 ******************************************************************************/
#ifdef __ASSEMBLER__
	#ifdef __MSP430X_LARGE__
		#define PUSHM 	pushm.a
		#define POPM 	popm.a

		#define PUSH 	push.a 
		#define POP 	pop.a


		#define MOV 	mova 
		#define MOVX 	movx 

		#define ADD 	adda
		#define SUB 	suba
		#define CMP		cmpa

		#define CALL 	calla 
		#define RET 	reta
		#define BR 		bra

	#else
		#define PUSHM 	pushm.w
		#define POPM 	popm.w

		#define PUSH 	push.w 
		#define POP 	pop.w


		#define MOV 	mov.w 

		#define ADD 	add.w
		#define SUB 	sub.w
		#define CMP		cmp.w

		#define CALL 	call 
		#define RET 	ret
		#define BR	 	br
	#endif
#endif

/*******************************************************************************
 * data structures
 ******************************************************************************/
#ifndef __ASSEMBLER__
	#if defined(THREAD_C_)
		typedef uintptr_t cpu_reg_t;

		/* Software context for the CPU */
		typedef struct PACKED
		{
			cpu_reg_t r4;
			cpu_reg_t r5;
			cpu_reg_t r6;
			cpu_reg_t r7;
			cpu_reg_t r8;
			cpu_reg_t r9;
			cpu_reg_t r10;
			cpu_reg_t r11;
			cpu_reg_t r12;
			cpu_reg_t r13;
			cpu_reg_t r14;
			cpu_reg_t r15;
		} cpu_sw_context_t;

		/* Hardware-stacked context for the CPU */
		typedef struct PACKED
		{
			uint32_t value;
			uint16_t words[2];

			/**
			 * CPUX Trapframe (20-bit)
			 * - SP + 2: PC[15:0]
			 * - SP: PC[19:16], SR
			 *
			 * CPU Trapframe (16-bit)
			 * - SP + 2: PC[15:0]
			 * - SP: SR
			 */

			#if defined(__MSP430X_LARGE__)

			struct {
				unsigned int sr : 12;
				unsigned int pc_high : 4;
				unsigned int pc_low : 16;
			};

			#else

			struct {
				unsigned int sr : 16;
				unsigned int pc : 16;
			};

			#endif
		} cpu_hw_context_t;
	#endif
#endif

#endif