/*
 * thread.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#define THREAD_C_

#include <stddef.h>
#include "attributes.h"
#include "memmodel.h"
#include <stdbool.h>
#include <string.h>
#include <msp430.h>

typedef struct PACKED
{
	cpu_sw_context_t	sw_ctx;
	cpu_hw_context_t	hw_ctx;
} k_thrd_context_t;

typedef struct
{
    k_thrd_context_t    *sp;

    void    			*stack_mem_p;
    size_t  			stack_mem_size;
	
    void				*state_p;
	
	unsigned int		id;
	void    			*notepad_p;
} k_thrd_t;


STATIC void k_init_sw_stackframe(cpu_sw_context_t *ctx, void *task_arg)
{
	memset(ctx, 0, sizeof(cpu_sw_context_t));
	ctx->r12 = (cpu_reg_t) task_arg;
}
#if 0
void k_thrd_exit(int ret);
STATIC void k_init_hw_stackframe(cpu_hw_context_t *ctx, void *pc, bool trap_en)
{
	// TODO: move this code
	// // cpu_init_hw_stackframe(ctx, pc, sr);
	// #define CPU_PC_LOW_BITS(pc)	 ((pc) & 0xFFFF)
	// #define CPU_SR_BITS(sr)		 ((sr) & 0x1FF)
	// #if defined(__MSP430X_LARGE__)
	// 	#define CPU_PC_HIGH_BITS(pc) ((pc) & 0xF0000) >> 16

	// 	unsigned int pc_high = CPU_PC_HIGH_BITS(pc);
	// 	unsigned int pc_low = CPU_PC_LOW_BITS(pc);
	// 	unsigned int sr = CPU_SR_BITS(sr);

	// 	ctx->pc_high = pc_high;
	// 	ctx->pc_low = pc_low;
	// 	ctx->sr = sr;
	// #else
	// 	ctx->pc = CPU_PC_LOW_BITS((uintptr_t) pc);
	// 	ctx->sr = CPU_SR_BITS(sr);
	// #endif
}

STATIC INLINE void *k_thrd_stack_bottom(k_thrd_t *this)
{
	return this->stack_mem_p + this->stack_mem_size;
}


#define K_TRAPS_ENABLED (CPU_TRAPS_ENABLED)
#define CPU_TRAPS_ENABLED (GIE)

STATIC int k_init_thrd_stack(k_thrd_t *this, void *pc, void *arg, bool trap_en)
{
	if ((this == NULL) || (pc == NULL))
	{
		return -1;
	}

	memset(this->stack_mem_p, 0, this->stack_mem_size);

	/* allocate a memory slot for the thread exit handler */
	cpu_hw_context_t *exit_hndlr = k_thrd_stack_bottom(this) - sizeof(cpu_hw_context_t);

	/* allocate memory slots for the thread context */
	k_thrd_context_t *init_ctx = exit_hndlr - sizeof(k_thrd_context_t);

	/* initialize all of the contexts allocated */
	k_init_sw_stackframe(&init_ctx->sw_ctx, arg);
	k_init_hw_stackframe(&init_ctx->hw_ctx, pc, trap_en);
	k_init_hw_stack_frame(&exit_hndlr, k_thrd_exit, ~K_TRAPS_ENABLED);

	return 0;
}

int k_thrd_create(int (*routine)(void *), void *arg, unsigned int priority, size_t stack_size, unsigned int flags)
{
	if ((routine == NULL) || (priority == 0))
	{
		return -1;
	}

	// TODO: enforce minimum stack size

	cpu_enter_critical();

	// allocate kernel memory here
	k_thrd_t *the_thread = NULL; // TODO: replace with kernel allocator
	memset(the_thread, 0, sizeof(*the_thread));

	// allocate kernel stack here
	// the_thread->stack_mem_p = NULL;

	if (k_init_thrd_stack(the_thread, routine, arg, flags & K_TRAPS_ENABLED) < 0)
	{
		cpu_exit_critical();
		return -1;
	}

	// set up scheduling data structures 
	if (k_init_thrd_sched_params(the_thread, priority) < 0)
	{
		cpu_exit_critical();
		// this function will allocate the sched_data_p, initialize it, and connect the underlying data structure back to the thread
		return -1;
	}

	// don't add it to the scheduler yet - add it to ainitial queue
	// return the ID of the created task
}

int k_thrd_start(unsigned int id)
{
	// if the thread is already started (i.e. not on the initial queue, instead either on a run queue or something else) then do nothing
	k_thrd_t *new_job = NULL;

	if (new_job == NULL)
	{
		return -1;
	}

	// call tghe scheduler to add the thread to the list
	// return k_sched_add_job(new_job);
}

void k_thrd_exit(int ret)
{
	// interrupts should be disabled at this point
	if (!cpu_in_critical())
	{
		// k_panic(K_ILL_TRAP);
	}

	// drop the thread from the job list
	// k_sched_rm_job(SELF);

	// free the thread memory
	// free()

	// yield to a new thread
}
#endif