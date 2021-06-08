/*
 * sched.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#define SCHED_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "sched.h"
#include "sched_priv.h"

/*******************************************************************************
 * defines
 ******************************************************************************/

/*******************************************************************************
 * public functions
 ******************************************************************************/
k_status_code_t k_sched_init(k_sched_policy_t policy)
{
	if (policy >= K_SCHED_N_POLICY)
	{
		return -1;
	}

	g_kern_sched.known_policies[K_SCHED_ROUND_ROBIN] = g_kern_rr_policy;

	g_kern_sched.policy = g_kern_sched.known_policies[policy];
	if (g_kern_sched.policy == NULL)
	{
		return -2;
	}

	g_kern_sched.policy->init();
	g_kern_sched.idle = k_idle;

	return 0;
}

k_status_code_t k_sched_job_start(k_thread_t *thread)
{
	return g_kern_sched.policy->start(thread);
}

k_status_code_t k_sched_job_suspend(k_thread_t *thread)
{
    if (thread == g_kern_sched.next_thread)
    {
        k_sched_job_yield();
    }

	return g_kern_sched.policy->pause(thread);
}	

k_status_code_t k_sched_job_resume(k_thread_t *thread)
{
	return g_kern_sched.policy->resume(thread);
}

k_status_code_t k_sched_job_restart(k_thread_t *thread)
{
	return g_kern_sched.policy->restart(thread);
}

k_status_code_t k_sched_job_create(k_thread_t *thread, unsigned int prio)
{
	return g_kern_sched.policy->create(thread, prio);
}

k_status_code_t k_sched_job_delete(k_thread_t *thread)
{
    if (thread == g_kern_sched.next_thread)
    {
        k_sched_job_yield();
    }

	return g_kern_sched.policy->kill(thread);
}

void k_sched_job_next(void)
{
	g_kern_sched.next_thread = (volatile k_thread_t *) g_kern_sched.policy->next((k_thread_t *) g_kern_sched.curr_thread);
}
	
void k_sched_job_yield(void)
{
    g_kern_sched.next_thread = (volatile k_thread_t *) g_kern_sched.policy->yield((k_thread_t *) g_kern_sched.curr_thread);
}

k_thread_t *k_sched_lkup_by_id(k_thread_id_t id)
{
	return g_kern_sched.policy->find(id);
}

volatile k_thread_t *k_sched_current_job(void)
{
	return g_kern_sched.curr_thread;
}

bool k_sched_is_idle(void)
{

}

// add a query function that calls rr.c
