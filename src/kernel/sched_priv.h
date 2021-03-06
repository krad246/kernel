/*
 * sched_priv.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef SCHED_PRIV_H_
#define SCHED_PRIV_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "sched.h"
#include "idle.h"
#include "thread.h"
#include "sched_policy.h"

/*******************************************************************************
 * data structures
 ******************************************************************************/
typedef struct
{
	volatile k_thread_t *curr_thread;
	volatile k_thread_t *next_thread;
	k_thread_t *idle;

	k_sched_interface_t *policy;
	k_sched_interface_t *known_policies[K_SCHED_N_POLICY];
} k_sched_t;

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
EXTERN k_sched_interface_t *g_kern_rr_policy;

#endif
