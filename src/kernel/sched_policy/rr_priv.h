/*
 * rr.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stdlib.h>

#include "attributes.h"
#include "clist.h"
#include "thread.h"
#include "sched.h"
#include "utils.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
#define K_JOB_RUN_METADATA(x) CONTAINER_OF((x), k_job_run_metadata_t, entry)
#define K_JOB_PAUSE_METADATA(x) CONTAINER_OF((x), k_job_pause_metadata_t, entry)
#define K_JOB_CREATE_METADATA(x) CONTAINER_OF((x), k_job_create_metadata_t, entry)
#define K_JOB_METADATA(x) CONTAINER_OF(K_JOB_RUN_METADATA(x), k_job_metadata_t, run)
#define K_JOB_LIST_ENTRY(x) ({ &K_JOB_METADATA(x)->run.entry; })
#define K_JOB_THREAD(x) K_JOB_METADATA(x)->run.thread
#define K_JOB_STATE(x) K_JOB_METADATA(x)->state

typedef enum  
{
	K_JOB_RUNNING,
	K_JOB_PAUSED,
	K_JOB_CREATED,
} k_job_status_t;

/*******************************************************************************
 * data strucutres
 ******************************************************************************/
typedef struct 
{
	k_thread_t	*thread;		/* pointer to owned thread */

	unsigned int init_prio;		/* thread initial priority */

	unsigned int n_slices;		/* working priority data (for priority inheritance protocol and scheduling) */
	unsigned int n_slices_used;

	clist_node_t entry;
} k_job_run_metadata_t;


typedef struct 
{
	k_thread_t	*thread;		/* pointer to owned thread */

	unsigned int init_prio;		/* thread initial priority */

	unsigned int unused[2];		/* dummy data to map struct onto k_job_run_node_t */

	clist_node_t entry;
} k_job_pause_metadata_t;

typedef k_job_pause_metadata_t k_job_create_metadata_t;

typedef struct
{
	k_job_status_t state;

	union 
	{
		k_job_run_metadata_t 		run;
		k_job_create_metadata_t 	create;
		k_job_pause_metadata_t 		pause;
	};

} k_job_metadata_t;

typedef struct
{
	const k_sched_interface_t interface;

	clist_t run_q;
	clist_t pause_q;
	clist_t create_q;
} k_round_robin_policy_t;

/*******************************************************************************
 * forward declarations
 ******************************************************************************/
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

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
STATIC k_round_robin_policy_t g_kern_rr_interface =
{
	.interface = 
	{
		.init = k_rr_init,

		.start = k_rr_job_start,
		.pause = k_rr_job_pause,
		.resume = k_rr_job_resume,
		.restart = k_rr_job_restart,

		.create = k_rr_job_create,
		.kill = k_rr_job_kill,

		.next = k_rr_job_next,
		.yield = k_rr_job_yield,

		.find = k_rr_job_find
	}
};

k_sched_interface_t *g_kern_rr_policy = (k_sched_interface_t *) &g_kern_rr_interface;
