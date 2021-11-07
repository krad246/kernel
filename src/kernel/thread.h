/*
 * thread.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef THREAD_H_
#define THREAD_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stddef.h>
#include "attributes.h"
#include "memmodel.h"
#include "k_status.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
#define K_THREAD_PRIORITY(x) ((x) & 0xFF)
#define K_THREAD_SELF ({ k_thread_current(); })

typedef void k_thread_arg_t;
typedef k_status_code_t (*k_thread_entry_t)(k_thread_arg_t *);

typedef unsigned int k_thread_id_t;

#define K_STACK_SIZE_MIN (CPU_STACK_SIZE_MIN)
#define K_STACK_SIZE_TINY (2 * K_STACK_SIZE_MIN)
#define K_STACK_SIZE_SMALL (2 * K_STACK_SIZE_TINY)
#define K_STACK_SIZE_MEDIUM (2 * K_STACK_SIZE_SMALL)
#define K_STACK_SIZE_LARGE (2 * K_STACK_SIZE_MEDIUM)

/*******************************************************************************
 * data structures
 ******************************************************************************/
#if defined(SCHED_C_) || defined(THREAD_C_) || defined(IDLE_C_)
typedef struct PACKED
{
	cpu_sw_context_t sw_ctx;
	cpu_hw_context_t hw_ctx;
} k_thread_context_t;

typedef struct
{
	k_thread_entry_t entry;

	k_thread_context_t *sp;

	void *stack_mem;
	size_t stack_mem_size;

	void *state;

	k_thread_id_t id;
} k_thread_t;
#endif

typedef struct
{

} k_thread_attr_t;

/*******************************************************************************
 * public functions
 ******************************************************************************/
EXTERN k_status_code_t k_thread_create(unsigned int priority, size_t stack_size, k_thread_id_t *id);
EXTERN k_status_code_t k_thread_start(k_thread_id_t id, k_thread_entry_t entry, k_thread_arg_t *args);
EXTERN k_status_code_t k_thread_suspend(k_thread_id_t id);
EXTERN k_status_code_t k_thread_resume(k_thread_id_t id);
EXTERN k_status_code_t k_thread_restart(k_thread_id_t id, k_thread_arg_t *args);
EXTERN k_status_code_t k_thread_kill(k_thread_id_t id);
EXTERN NORETURN void k_thread_exit(k_status_code_t ret);

EXTERN k_thread_id_t k_thread_current(void);

EXTERN k_status_code_t k_thread_get_attr(k_thread_id_t id, k_thread_attr_t *attr);
EXTERN k_status_code_t k_thread_set_attr(k_thread_id_t id, k_thread_attr_t *attr);

#endif
