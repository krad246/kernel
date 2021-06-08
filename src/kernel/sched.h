/*
 * sched.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef SCHED_H_
#define SCHED_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "thread.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
typedef enum
{
	K_SCHED_ROUND_ROBIN,
	K_SCHED_LOTTERY,
	K_SCHED_VTRR,
	K_SCHED_N_POLICY
} k_sched_policy_t;

/*******************************************************************************
 * data structures
 ******************************************************************************/
#if defined(SCHED_C_)
	typedef struct 
	{
		k_status_code_t (*init)(void);

		k_status_code_t (*start)(k_thread_t *);
		k_status_code_t (*pause)(k_thread_t *);
		k_status_code_t (*resume)(k_thread_t *);
		k_status_code_t (*restart)(k_thread_t *);

		k_status_code_t (*create)(k_thread_t *, unsigned int);
		k_status_code_t (*kill)(k_thread_t *);

		k_thread_t *(*next)(k_thread_t *);
		k_thread_t *(*yield)(k_thread_t *);

		k_thread_t *(*find)(k_thread_id_t);
	} k_sched_interface_t;
#endif

/*******************************************************************************
 * public functions
 ******************************************************************************/
#if defined(THREAD_C_)
	EXTERN k_status_code_t k_sched_init(k_sched_policy_t policy);

	EXTERN  k_status_code_t k_sched_job_start(k_thread_t *thread);
	EXTERN  k_status_code_t k_sched_job_suspend(k_thread_t *thread);

	EXTERN k_status_code_t k_sched_job_resume(k_thread_t *thread);

	EXTERN k_status_code_t k_sched_job_restart(k_thread_t *thread);

	EXTERN k_status_code_t k_sched_job_create(k_thread_t *thread, unsigned int prio);
	EXTERN k_status_code_t k_sched_job_delete(k_thread_t *thread);

	EXTERN void k_sched_job_next(void);
	EXTERN void k_sched_job_yield(void);

	EXTERN k_thread_t *k_sched_lkup_by_id(k_thread_id_t id);

	EXTERN volatile k_thread_t *k_sched_current_job(void);
#endif

#endif
