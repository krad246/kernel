/*
 * thread.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#define THREAD_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "thread.h"
#include "thread_priv.h"

/*******************************************************************************
 * defines
 ******************************************************************************/

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
STATIC unsigned int g_kern_assign_id = 0;

/*******************************************************************************
 * private functions
 ******************************************************************************/
STATIC void k_init_sw_stackframe(cpu_sw_context_t *ctx, void *arg)
{
	cpu_init_sw_stackframe(ctx, arg);
}

STATIC void k_init_hw_stackframe(cpu_hw_context_t *ctx, void *pc)
{
	cpu_init_hw_stackframe(ctx, pc, CPU_TRAPS_ENABLED); // TODO: make interrupt setting flexible
}

STATIC int k_init_thread_stack(k_thread_t *this, void *pc, void *arg)
{
	if (pc == NULL)
	{
		return -1;
	}

	memset(this->stack_mem, 0, this->stack_mem_size);

	/* allocate a memory slot for the thread exit handler */
	void *exit_handler = K_THREAD_STACK_BOTTOM(this) - sizeof(cpu_hw_context_t);

	/* allocate memory slots for the thread context */
	k_thread_context_t *init_ctx = exit_handler - sizeof(k_thread_context_t);

	/* initialize all of the contexts allocated */
	k_init_sw_stackframe(&init_ctx->sw_ctx, arg);
	k_init_hw_stackframe(&init_ctx->hw_ctx, pc);
	k_init_hw_stackframe(&exit_handler, k_thread_exit);

	/* point the thread to the context */
	this->sp = init_ctx;

	return 0;
}

// TODO: make this public somewhere

/*******************************************************************************
 * public functions
 ******************************************************************************/
k_status_code_t k_thread_create(unsigned int priority, size_t stack_size, k_thread_id_t *id)
{
	if ((priority == 0))
	{
		return -1;
	}

	if (stack_size <= K_STACK_SIZE_MIN)
	{
		return -1;
	}

	/* need a whole number of words */
	if (!IS_MEM_ALIGNED(stack_size, sizeof(cpu_reg_t)))
	{
		return -2;
	}

	cpu_enter_critical();

	// TODO: no malloc / free

	/* allocate a thread */
	k_thread_t *the_thread = malloc(sizeof(k_thread_t));
	memset(the_thread, 0, sizeof(*the_thread));

	/* allocate its stack */
	void *the_stack = malloc(stack_size);
	memset(the_stack, K_MEM_UNUSED_PTRN, stack_size);
	the_thread->stack_mem = the_stack;
	the_thread->stack_mem_size = stack_size;

	k_sched_job_create(the_thread, priority);

	/* assign the ID */
	if (id != NULL)
	{
		*id = g_kern_assign_id;
	}

	the_thread->id = g_kern_assign_id;
	g_kern_assign_id++;

	cpu_exit_critical();

	return 0;
}

k_status_code_t k_thread_start(k_thread_id_t id, k_thread_entry_t entry, k_thread_arg_t *args)
{
	if (entry == NULL)
	{
		return -1;
	}

	cpu_enter_critical();
	k_thread_t *the_thread = k_sched_lkup_by_id(id);
	cpu_exit_critical();

	if (the_thread == NULL)
	{
		return -1;
	}

	cpu_enter_critical();

	the_thread->entry = entry;
	k_init_thread_stack(the_thread, entry, args);
	k_sched_job_start(the_thread);

	cpu_exit_critical();

	return 0;
}

k_status_code_t k_thread_suspend(k_thread_id_t id)
{
	cpu_enter_critical();
	k_thread_t *the_thread = k_sched_lkup_by_id(id);
	cpu_exit_critical();

	if (the_thread == NULL)
	{
		return -1;
	}

	cpu_enter_critical();

	if (id == K_THREAD_SELF)
	{
		k_sched_job_yield();
	}

	k_sched_job_suspend(the_thread);

	cpu_exit_critical();

	cpu_yield();

	return 0;
}

k_status_code_t k_thread_resume(k_thread_id_t id)
{
	cpu_enter_critical();
	k_thread_t *the_thread = k_sched_lkup_by_id(id);
	cpu_exit_critical();

	if (the_thread == NULL)
	{
		return -1;
	}

	cpu_enter_critical();

	k_sched_job_resume(the_thread);

	cpu_exit_critical();

	return 0;
}

k_status_code_t k_thread_restart(k_thread_id_t id, k_thread_arg_t *args)
{
	cpu_enter_critical();
	k_thread_t *the_thread = k_sched_lkup_by_id(id);
	k_thread_entry_t entry_point = the_thread->entry;
	cpu_exit_critical();

	if (the_thread == NULL)
	{
		return -1;
	}

	cpu_enter_critical();

	k_thread_kill(id);
	k_thread_create(id, the_thread->stack_mem_size, &id);
	k_thread_start(id, entry_point, args);

	cpu_exit_critical();

	return 0;
}

k_status_code_t k_thread_kill(k_thread_id_t id)
{
	cpu_enter_critical();
	k_thread_t *the_thread = k_sched_lkup_by_id(id);
	cpu_exit_critical();

	if (the_thread == NULL)
	{
		return -1;
	}

	cpu_enter_critical();

	if (id != K_THREAD_SELF)
	{
		/* deallocate data */
		free(the_thread);
		free(the_thread->stack_mem);
	}
	else
	{
		k_sched_job_yield();
	}

	k_sched_job_delete(the_thread);

	cpu_exit_critical();

	return 0;
}

NORETURN void k_thread_exit(k_status_code_t ret)
{
	k_thread_kill(K_THREAD_SELF);

	while (1)
		;
}

k_thread_id_t k_thread_current(void)
{
	return k_sched_current_job()->id;
}
