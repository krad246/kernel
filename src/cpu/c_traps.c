/*
 * c_traps.c
 *
 *  Created on: Jun 3, 2021
 *      Author: krad2
 */

#define C_TRAPS_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "c_traps.h"
#include "c_traps_priv.h"

/*******************************************************************************
 * defines
 ******************************************************************************/

/*******************************************************************************
 * public functions
 ******************************************************************************/
cpu_status_code_t cpu_traps_init(void *table_p)
{
	if (table_p == NULL)
	{
		errno = EINVAL;
		return CPU_STATUS_ERROR;
	}

	cpu_enter_critical();

	g_cpu_in_trap = false;

	g_cpu_trap_table_p = (cpu_trap_table_t *)table_p;
	memset(g_cpu_trap_table_p, 0, sizeof(*g_cpu_trap_table_p));

	/* initialize the trap stack and the stack pointer for it */
	memset(g_cpu_trap_stack, CPU_MEM_UNUSED_PTRN, CPU_TRAP_STACK_SIZE);
	g_cpu_trap_sp = g_cpu_trap_stack + CPU_TRAP_STACK_SIZE;

	/* 
	 * allocate a slot in the trap stack for the panic handler
	 * in the event we try to illegally return out of the interrupt
	 * context.
	 */
	g_cpu_trap_sp -= sizeof(cpu_hw_context_t);
	cpu_init_hw_stackframe(g_cpu_trap_sp, cpu_panic, ~CPU_TRAPS_ENABLED);

	cpu_exit_critical();

	return CPU_STATUS_OK;
}

cpu_status_code_t cpu_install_handler(unsigned int tnum, cpu_trap_callback_t callback, cpu_trap_args_t *args)
{
	if (g_cpu_trap_table_p == NULL)
	{
		errno = EINVAL;
		return CPU_STATUS_ERROR;
	}

	cpu_enter_critical();

	cpu_trap_handler_t *old_handler = &(*g_cpu_trap_table_p)[tnum];
	old_handler->callback = callback;
	old_handler->args = args;

	cpu_exit_critical();

	return CPU_STATUS_OK;
}

cpu_status_code_t cpu_remove_handler(unsigned int tnum)
{
	return cpu_install_handler(tnum, NULL, NULL);
}

bool cpu_trap_in_progress(void)
{
	bool in_trap;
	
	cpu_enter_critical();
	in_trap = g_cpu_in_trap;
	cpu_exit_critical();

	return in_trap;
}

WEAK bool cpu_context_switch_requested(void)
{
	return false;
}

WEAK void cpu_context_switch(void)
{
}

void cpu_trap(unsigned int tnum)
{
	g_cpu_in_trap = true;

	if (g_cpu_trap_table_p != NULL)
	{
		volatile cpu_trap_handler_t *handler = &(*g_cpu_trap_table_p)[tnum];
		if (handler->callback != NULL)
		{
			handler->callback(tnum, handler->args);
		}
	}

	g_cpu_in_trap = false;

	if (cpu_context_switch_requested())
	{
		cpu_context_switch();
	}
}

WEAK NORETURN void cpu_panic(unsigned int tnum)
{
	while (FOREVER);
}
