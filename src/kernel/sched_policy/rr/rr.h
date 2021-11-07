/*
 * rr.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef RR_H_
#define RR_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "attributes.h"
#include "clist.h"
#include "sched.h"
#include "k_status.h"
#include "thread.h"

/*******************************************************************************
 * data structures
 ******************************************************************************/
#if defined(SCHED_C_)

typedef struct
{
	k_sched_interface_t interface;

	clist_t run_q;
	clist_t pause_q;
	clist_t create_q;
} k_round_robin_policy_t;

#endif

/*******************************************************************************
 * globals
 ******************************************************************************/
#if defined(SCHED_C_)

EXTERN k_round_robin_policy_t g_kern_rr_interface;
EXTERN k_sched_interface_t *g_kern_rr_policy;

#ifdef TEST
EXTERN k_status_code_t k_rr_init(void);
EXTERN k_status_code_t k_rr_job_start(k_thread_t *);
EXTERN k_status_code_t k_rr_job_pause(k_thread_t *);
EXTERN k_status_code_t k_rr_job_resume(k_thread_t *);
EXTERN k_status_code_t k_rr_job_restart(k_thread_t *);
EXTERN k_status_code_t k_rr_job_create(k_thread_t *, unsigned int prio);
EXTERN k_status_code_t k_rr_job_kill(k_thread_t *);
EXTERN k_thread_t *k_rr_job_next(k_thread_t *);
EXTERN k_thread_t *k_rr_job_yield(k_thread_t *);
EXTERN k_thread_t *k_rr_job_find(k_thread_id_t);
#endif

#endif

#endif