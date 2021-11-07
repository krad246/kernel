/*
 * c_traps.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef C_TRAPS_H_
#define C_TRAPS_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h> 

#include "attributes.h"
#include "cpu.h"
#include "trap_defs.h"
#include "status.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
#define CPU_TRAP_STACK_SIZE (128)

typedef void cpu_trap_args_t;
typedef void (*cpu_trap_callback_t)(unsigned int, cpu_trap_args_t *);

/*******************************************************************************
 * data structures
 ******************************************************************************/
#if defined(C_TRAPS_C_) || defined(TRAP_C_)
	typedef struct
	{
		volatile cpu_trap_callback_t callback;
		volatile cpu_trap_args_t *args;
	} cpu_trap_handler_t;

	typedef cpu_trap_handler_t cpu_trap_table_t[CPU_N_TRAPS];
	EXTERN volatile uint8_t g_cpu_trap_stack[CPU_TRAP_STACK_SIZE];
#endif

/*******************************************************************************
 * public functions
 ******************************************************************************/
EXTERN cpu_status_code_t cpu_traps_init(void *table_p);

EXTERN cpu_status_code_t cpu_install_handler(unsigned int tnum, cpu_trap_callback_t callback, cpu_trap_args_t *args);
EXTERN cpu_status_code_t cpu_remove_handler(unsigned int tnum);

EXTERN bool cpu_trap_in_progress(void);

WEAK EXTERN bool cpu_context_switch_requested(void);
WEAK EXTERN void cpu_context_switch(void);

WEAK EXTERN void cpu_panic(unsigned int tnum);

#endif /* C_TRAPS_H_ */
