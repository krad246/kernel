/*
 * test_sched.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#include <unity.h>

#define SCHED_C_

#include "mock_cpu.h"
#include "mock_critical.h"
#include "mock_memmodel.h"
#include "mock_thread.h"
#include "mock_idle.h"
#include "mock_sched_policy.h"
#include "mock_alloc.h"

#include "sched_priv.h"
#include "sched.h"

#include "rr.h"
#include "rr_priv.h"

extern k_sched_t g_kern_sched;

void setUp()
{
	cpu_enter_critical_Ignore();
	cpu_exit_critical_Ignore();
}

void tearDown()
{
}

void *mock_k_alloc(size_t bytes)
{
	return malloc(bytes);
}

void *mock_k_free(void *ptr)
{
	free(ptr);
}

k_status_code_t k_thread_init(k_thread_t *thread, size_t stack_size, k_thread_id_t *id)
{

	if (stack_size <= K_STACK_SIZE_MIN)
	{
		errno = EINVAL;
		return K_STATUS_ERROR;
	}

	/* need a whole number of words */
	if (!IS_MEM_ALIGNED(stack_size, sizeof(cpu_reg_t)))
	{
		errno = EINVAL;
		return K_STATUS_ERROR;
	}

	k_sched_lock();

	if (thread == NULL)
	{
		errno = ENOMEM;
		return K_STATUS_ERROR;
	}
	memset(thread, 0, sizeof(*thread));

	/* allocate its stack */
	void *the_stack = thread;
	if (the_stack == NULL)
	{
		errno = ENOMEM;
		return K_STATUS_ERROR;
	}

	thread->stack_mem = the_stack;
	thread->stack_mem_size = stack_size;

	unsigned int the_id = k_sched_alloc_pid();

	/* assign the ID */
	if (id != NULL)
	{
		*id = the_id;
	}

	thread->id = the_id;

	k_sched_unlock();

	return K_STATUS_OK;
}

void test_sched_init(void)
{
	TEST_ASSERT_EQUAL(K_STATUS_OK, k_sched_init(K_SCHED_RR));
	
	TEST_ASSERT_EQUAL(g_kern_rr_policy, g_kern_sched.policy);
	TEST_ASSERT_EQUAL(g_kern_rr_policy, g_kern_sched.known_policies[K_SCHED_RR]);
	TEST_ASSERT_EQUAL(k_rr_init, g_kern_rr_interface.interface.init);

	#define TEST_RR_INTERFACE_FN(fn) do { TEST_ASSERT_EQUAL(k_rr_job_##fn, g_kern_rr_interface.interface.fn); } while (0)

	TEST_RR_INTERFACE_FN(start);
	TEST_RR_INTERFACE_FN(pause);
	TEST_RR_INTERFACE_FN(resume);
	TEST_RR_INTERFACE_FN(restart);
	TEST_RR_INTERFACE_FN(create);
	TEST_RR_INTERFACE_FN(kill);
	TEST_RR_INTERFACE_FN(next);
	TEST_RR_INTERFACE_FN(yield);
	TEST_RR_INTERFACE_FN(find);

	TEST_ASSERT_EACH_EQUAL_CHAR(0, &g_kern_rr_interface.run_q, sizeof(clist_t));
	TEST_ASSERT_EACH_EQUAL_CHAR(0, &g_kern_rr_interface.pause_q, sizeof(clist_t));
	TEST_ASSERT_EACH_EQUAL_CHAR(0, &g_kern_rr_interface.create_q, sizeof(clist_t));
}
void test_sched_add_job(void)
{
	TEST_ASSERT_EQUAL(K_STATUS_OK, k_sched_init(K_SCHED_RR));

	k_thread_t threads[3];
	clist_node_t *last_head = NULL;
	for (int i = 0; i < ARRAY_SIZE(threads); i++)
	{
		unsigned int the_id;
		k_thread_init(&threads[i], 256, &the_id);
		
		TEST_ASSERT_NOT_NULL(threads[i].stack_mem);
		TEST_ASSERT_NOT_NULL(threads[i].stack_mem_size);
		TEST_ASSERT_NULL(threads[i].state);
		TEST_ASSERT_EQUAL_INT(i + 1, the_id);

		k_alloc_ExpectAndReturn(sizeof(k_job_metadata_t), 0);
		k_alloc_AddCallback(mock_k_alloc);

		last_head = g_kern_rr_interface.create_q.head;
		k_sched_job_create(&threads[i], i);

		TEST_ASSERT_NOT_NULL(threads[i].state);
		TEST_ASSERT_EQUAL_UINT(K_JOB_CREATED, K_JOB_METADATA(threads[i].state)->state);
		TEST_ASSERT_EQUAL_UINT(i, K_JOB_METADATA(threads[i].state)->create.init_prio);
		TEST_ASSERT_EQUAL(&threads[i], K_JOB_THREAD(threads[i].state));

		TEST_ASSERT_EQUAL(i + 1, g_kern_rr_interface.create_q.count);
		TEST_ASSERT_EQUAL(&K_JOB_CREATE_METADATA(threads[i].state)->entry, g_kern_rr_interface.create_q.head);

		if (i == 0)
		{
			TEST_ASSERT_EQUAL(K_JOB_CREATE_METADATA(threads[i].state)->entry.prev, K_JOB_CREATE_METADATA(threads[i].state)->entry.next);
		} else
		{
			TEST_ASSERT_NOT_EQUAL(g_kern_rr_interface.create_q.head, last_head);
			TEST_ASSERT_EQUAL(g_kern_rr_interface.create_q.head->prev->next, g_kern_rr_interface.create_q.head);
			TEST_ASSERT_EQUAL(g_kern_rr_interface.create_q.head->prev, g_kern_rr_interface.create_q.head->prev->prev->next);
		}
	}
}
