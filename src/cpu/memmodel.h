/*
 * memmodel.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef MEMMODEL_H_
#define MEMMODEL_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#ifndef __ASSEMBLER__
	#include <stdint.h>
	#include <stdbool.h>
	#include <string.h>

	#include "cpu.h"
	#include "attributes.h"
#endif

/*******************************************************************************
 * defines
 ******************************************************************************/
#ifdef __ASSEMBLER__
	#ifdef __MSP430X_LARGE__
		#define PUSHM pushm.a
		#define POPM popm.a

		#define PUSH push.a
		#define POP pop.a

		#define MOV mova
		#define MOVX movx

		#define ADD adda
		#define SUB suba
		#define CMP cmpa

		#define CALL calla
		#define RET reta
		#define BR bra

	#else
		#define PUSHM pushm.w
		#define POPM popm.w

		#define PUSH push.w
		#define POP pop.w

		#define MOV mov.w

		#define ADD add.w
		#define SUB sub.w
		#define CMP cmp.w

		#define CALL call
		#define RET ret
		#define BR br
	#endif
#endif

/*******************************************************************************
 * data structures
 ******************************************************************************/
#ifndef __ASSEMBLER__

	#define CPU_MEM_UNUSED_PTRN (0x5A)
	#define CPU_STACK_SIZE_MIN (sizeof(cpu_hw_context_t) + sizeof(cpu_sw_context_t))

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

		/**
		 * CPUX Trapframe (20-bit)
		 * - SP + 2: PC[15:0]
		 * - SP: PC[19:16], SR[11:0]
		 *
		 * CPU Trapframe (16-bit)
		 * - SP + 2: PC[15:0]
		 * - SP: SR
		 */

	#ifdef __MSP430X_LARGE__
		uint16_t pc_plus_sr;
		uint16_t pc_low_bits;
	#else
		uint16_t pc;
		uint16_t sr;
	#endif

	} cpu_hw_context_t;

	EXTERN void cpu_init_hw_stackframe(cpu_hw_context_t *ctx, void *pc, bool trap_en);
	EXTERN void cpu_init_sw_stackframe(cpu_sw_context_t *ctx, cpu_reg_t r12);

#else

	#ifdef __MSP430X_LARGE__
		#define CPU_STACK_SIZE_MIN (52)
	#else
		#define CPU_STACK_SIZE_MIN (26)
	#endif

#endif

#endif
