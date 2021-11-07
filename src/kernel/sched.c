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
 * file-scope globals
 ******************************************************************************/
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

STATIC volatile k_thread_id_t g_kern_curr_pid = 1;

/*******************************************************************************
 * private functions
 ******************************************************************************/

STATIC k_status_code_t k_sched_load_policies(void)
{
	k_sched_lock();
	g_kern_sched.known_policies[K_SCHED_RR] = g_kern_rr_policy;
	k_sched_unlock();

	return K_STATUS_OK;
}

/*******************************************************************************
 * public functions
 ******************************************************************************/
k_status_code_t k_sched_init(k_sched_policy_t policy)
{
	if (policy >= K_SCHED_N_POLICY)
	{
		errno = EINVAL;
		return K_STATUS_ERROR;
	}

	k_sched_lock();

	k_sched_load_policies();

	g_kern_sched.policy = g_kern_sched.known_policies[policy];
	if (g_kern_sched.policy == NULL)
	{
		errno = ENOSYS;
		return K_STATUS_ERROR;
	}

	g_kern_sched.policy->init();
	// k_idle_thread_create(k_idle);

	k_sched_unlock();

	return K_STATUS_OK;
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
	g_kern_sched.next_thread = (volatile k_thread_t *)g_kern_sched.policy->next((k_thread_t *)g_kern_sched.curr_thread);
}

void k_sched_job_yield(void)
{
	g_kern_sched.next_thread = (volatile k_thread_t *)g_kern_sched.policy->yield((k_thread_t *)g_kern_sched.curr_thread);
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

k_thread_id_t k_sched_alloc_pid(void)
{
	unsigned int pid;

	k_sched_lock();
	pid = g_kern_curr_pid;
	k_sched_unlock();

	k_sched_lock();
	g_kern_curr_pid++;
	k_sched_unlock();

	return pid;
}

void k_sched_lock(void)
{
	cpu_enter_critical();
}

void k_sched_unlock(void)
{
	cpu_exit_critical();
}


// add a query function that calls rr.c
