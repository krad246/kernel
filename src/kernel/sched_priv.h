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
#include "thread.h"

/*******************************************************************************
 * data structures
 ******************************************************************************/
typedef struct
{
	volatile k_thread_t *curr_thread;

	k_sched_interface_t *policy;
	k_sched_interface_t *known_policies[K_SCHED_N_POLICY];
} k_sched_t;

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
EXTERN k_sched_interface_t *g_kern_rr_policy;

STATIC k_sched_t g_kern_sched =
{
	.curr_thread = NULL,
	.policy = NULL,
	.known_policies = 
	{
		NULL,
		NULL,
		NULL
	}
};

#endif
