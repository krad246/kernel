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

STATIC void k_init_thread_context(k_thread_context_t *ctx, void *pc, void *arg)
{
    k_init_hw_stackframe(&ctx->hw_ctx, pc);
    k_init_sw_stackframe(&ctx->sw_ctx, arg);
}

STATIC int k_init_thread_stack(k_thread_t *this, void *pc, void *arg)
{
    if ((this == NULL) || (pc == NULL))
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
#define K_STACK_SIZE_MIN     (sizeof(k_thread_context_t))

/*******************************************************************************
 * public functions
 ******************************************************************************/
k_status_code_t k_thread_create(unsigned int priority, size_t stack_size, k_thread_id_t *id)
{
    if ((priority == 0) || (id == NULL))
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
    k_thread_t *the_stack = malloc(stack_size);
    memset(the_stack, K_MEM_UNUSED_PTRN, stack_size);
    the_thread->stack_mem = the_stack;


    /* connect the thread to the scheduler */
    if (k_sched_init_thread_metadata(&the_thread->state) < 0)
    {
        return -1;
    }

    k_sched_mark_job_created(the_thread->state);

    /* assign the ID */
    *id = g_kern_assign_id;
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

    k_thread_t *the_thread = k_sched_lkup_by_id(id);
    if (the_thread == NULL)
    {
        return -1;
    }

    cpu_enter_critical();

    the_thread->entry = entry;
    k_init_thread_stack(the_thread, entry, args);
    k_sched_mark_job_started(the_thread->state);

    cpu_exit_critical();

    return 0;
}

k_status_code_t k_thread_suspend(k_thread_id_t id)
{
    k_thread_t *the_thread = k_sched_lkup_by_id(id);
    if (the_thread == NULL)
    {
        return -1;
    }

    cpu_enter_critical();

    k_sched_mark_job_suspended(the_thread->state);
    k_yield();

    cpu_exit_critical();

    return 0;
}

k_status_code_t k_thread_resume(k_thread_id_t id)
{
    k_thread_t *the_thread = k_sched_lkup_by_id(id);
    if (the_thread == NULL)
    {
        return -1;
    }

    cpu_enter_critical();

    k_sched_mark_job_running(the_thread->state);

    cpu_exit_critical();

    return 0;
}

k_status_code_t k_thread_restart(k_thread_id_t id, k_thread_arg_t *args)
{
    k_thread_t *the_thread = k_sched_lkup_by_id(id);
    if (the_thread == NULL)
    {
        return -1;
    }

    cpu_enter_critical();

    k_init_thread_stack(the_thread, the_thread->entry, args);

    cpu_exit_critical();

    return 0;
}

k_status_code_t k_thread_kill(k_thread_id_t id)
{
    k_thread_t *the_thread = k_sched_lkup_by_id(id);
    if (the_thread == NULL)
    {
        return -1;
    }

    cpu_enter_critical();

	// find next thread
	k_thread_t *next = k_sched_next(the_thread);
	// cpu_stash_sp = next->sp;
    k_sched_job_delete(the_thread->state);

    /* deallocate data */
	free(the_thread->state);
	free(the_thread->stack_mem);
	free(the_thread);

    cpu_exit_critical();

    return 0;
}

void k_thread_exit(int ret)
{
    k_thread_kill(K_THREAD_SELF);

    // TODO: bring the return value into the thread
}

k_thread_id_t k_thread_current(void)
{

}
