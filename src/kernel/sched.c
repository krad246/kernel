/*
 * sched.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#if 0

typedef struct k_sched
{
	/* TODO: keep a standardized interface here */
	int (*init)(void);

	int (*create)(k_thrd_t *job, unsigned int);

	int (*start)(unsigned int);

	k_thrd_t *(*next)(k_thrd_t *job);

	int (*pause)(unsigned int);

	int (*resume)(unsigned int);

	k_thrd_t *(*yield)(k_thrd_t *job);

	int (*kill)(unsigned int);
} k_sched_t;
#endif
