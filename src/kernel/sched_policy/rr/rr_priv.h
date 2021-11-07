/*
 * rr_priv.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef RR_PRIV_H_
#define RR_PRIV_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stdlib.h>
#include <errno.h>

#include "alloc.h"
#include "clist.h"
#include "attributes.h"
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
	k_thread_t *thread; /* pointer to owned thread */

	unsigned int init_prio; /* thread initial priority */

	unsigned int n_slices; /* working priority data (for priority inheritance protocol and scheduling) */
	unsigned int n_slices_used;

	clist_node_t entry;
} k_job_run_metadata_t;

typedef struct
{
	k_thread_t *thread; /* pointer to owned thread */

	unsigned int init_prio; /* thread initial priority */

	unsigned int unused[2]; /* dummy data to map struct onto k_job_run_node_t */

	clist_node_t entry;
} k_job_pause_metadata_t;

typedef k_job_pause_metadata_t k_job_create_metadata_t;

typedef struct
{
	k_job_status_t state;

	union
	{
		k_job_run_metadata_t run;
		k_job_create_metadata_t create;
		k_job_pause_metadata_t pause;
	};

} k_job_metadata_t;



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

#endif