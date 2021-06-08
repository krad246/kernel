/*
 * rr.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#define SCHED_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "rr_priv.h"
#include "rr.h"

/*******************************************************************************
 * private functions
 ******************************************************************************/
int k_thread_matches_id(const clist_node_t *node, void *id)
{
	return (int) (K_JOB_THREAD(node)->id == *((k_thread_id_t *) id));
}

k_thread_t *k_rr_queue_search(clist_t *queue, k_thread_id_t id)
{
	clist_node_t *node = clist_foreach(queue, k_thread_matches_id, &id);
	if (node == NULL)
	{
		return NULL;
	}

	return K_JOB_THREAD(node);
}

/*******************************************************************************
 * public functions
 ******************************************************************************/
k_status_code_t k_rr_init(void)
{
	memset(&g_kern_rr_interface.run_q, 0, sizeof(g_kern_rr_interface.run_q));
	memset(&g_kern_rr_interface.pause_q, 0, sizeof(g_kern_rr_interface.pause_q));
	memset(&g_kern_rr_interface.create_q, 0, sizeof(g_kern_rr_interface.create_q));

	/* TODO: allocate all the queue memory up front */

	return 0;
}

k_status_code_t k_rr_job_start(k_thread_t *thread)
{
	clist_node_t *node = thread->state;

	if (K_JOB_STATE(node) == K_JOB_RUNNING)
	{
		return 0;
	}


	if (K_JOB_STATE(node) == K_JOB_PAUSED)
	{
		return -1;
	}

	clist_remove_at(&g_kern_rr_interface.create_q, K_JOB_LIST_ENTRY(node));

	K_JOB_STATE(node) = K_JOB_RUNNING;
	K_JOB_METADATA(node)->run.n_slices = K_JOB_METADATA(node)->run.init_prio;
	K_JOB_METADATA(node)->run.n_slices_used = 0;

	clist_lpush(&g_kern_rr_interface.run_q, K_JOB_LIST_ENTRY(node));

	return 0;
}

k_status_code_t k_rr_job_pause(k_thread_t *thread)
{
	clist_node_t *node = thread->state;

	if (K_JOB_STATE(node) == K_JOB_CREATED)
	{
		return -1;
	}


	if (K_JOB_STATE(node) == K_JOB_PAUSED)
	{
		return 0;
	}

	clist_remove_at(&g_kern_rr_interface.run_q, K_JOB_LIST_ENTRY(node));

	K_JOB_STATE(node) = K_JOB_PAUSED;

	clist_lpush(&g_kern_rr_interface.create_q, K_JOB_LIST_ENTRY(node));

	return 0;
}

k_status_code_t k_rr_job_resume(k_thread_t *thread)
{
	clist_node_t *node = thread->state;

	if (K_JOB_STATE(node) == K_JOB_CREATED)
	{
		return -1;
	}


	if (K_JOB_STATE(node) == K_JOB_RUNNING)
	{
		return 0;
	}

	clist_remove_at(&g_kern_rr_interface.pause_q, K_JOB_LIST_ENTRY(node));

	K_JOB_STATE(node) = K_JOB_RUNNING;

	clist_lpush(&g_kern_rr_interface.run_q, K_JOB_LIST_ENTRY(node));

	return 0;
}

k_status_code_t k_rr_job_restart(k_thread_t *thread)
{
	/* need to reinitialize thread stacks and such too */

	k_rr_job_kill(thread);
	k_rr_job_create(thread, K_JOB_METADATA(thread->state)->create.init_prio);
	k_rr_job_start(thread);

	return 0;
}

k_status_code_t k_rr_job_create(k_thread_t *thread, unsigned int prio)
{
	if (thread->state == NULL)
	{
		thread->state = malloc(sizeof(k_job_metadata_t));
		memset(thread->state, 0, sizeof(k_job_metadata_t));

		thread->state += offsetof(k_job_metadata_t, run.entry);
	}

	clist_node_t *node = thread->state;

	K_JOB_STATE(node) = K_JOB_CREATED;
	K_JOB_METADATA(node)->create.init_prio = prio;
	K_JOB_THREAD(node) = thread;

	clist_lpush(&g_kern_rr_interface.create_q, K_JOB_LIST_ENTRY(node));

	return 0;
}

k_status_code_t k_rr_job_kill(k_thread_t *thread)
{
	clist_node_t *node = thread->state;

	clist_remove(&g_kern_rr_interface.create_q, K_JOB_LIST_ENTRY(node));
	clist_remove(&g_kern_rr_interface.run_q, K_JOB_LIST_ENTRY(node));
	clist_remove(&g_kern_rr_interface.pause_q, K_JOB_LIST_ENTRY(node));

	free(K_JOB_METADATA(node));
	thread->state = NULL;

	return 0;
}

k_thread_t *k_rr_job_next(k_thread_t *thread)
{
	clist_node_t *node = thread->state;

	if (K_JOB_STATE(node) != K_JOB_RUNNING)
	{
		return NULL;
	}

	K_JOB_METADATA(node)->run.n_slices_used++;
	if (K_JOB_METADATA(node)->run.n_slices_used == K_JOB_METADATA(node)->run.n_slices)
	{
		K_JOB_METADATA(node)->run.n_slices_used = 0;
		return K_JOB_THREAD(K_JOB_LIST_ENTRY(node)->next);
	}

	return thread;
}

k_thread_t *k_rr_job_yield(k_thread_t *thread)
{
	return k_rr_job_next(thread);
}

k_thread_t *k_rr_job_find(k_thread_id_t id)
{
	k_thread_t *found;

	if (NULL != (found = k_rr_queue_search(&g_kern_rr_interface.run_q, id)))
	{
		return found;
	}

	if (NULL != (found = k_rr_queue_search(&g_kern_rr_interface.create_q, id)))
	{
		return found;
	}

	if (NULL != (found = k_rr_queue_search(&g_kern_rr_interface.pause_q, id)))
	{
		return found;
	}

	return NULL;
}

