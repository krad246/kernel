/*
 * test_c_traps.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#include <unity.h>

#define C_TRAPS_C_

#include "mock_attributes.h"
#include "mock_msp430_dummy.h"
#include "mock_memmodel.h"
#include "mock_critical.h"

#include "c_traps.h"

extern volatile cpu_trap_table_t *g_cpu_trap_table_p;
extern volatile bool g_cpu_in_trap;

extern volatile void *g_cpu_trap_sp;
extern volatile uint8_t g_cpu_trap_stack[CPU_TRAP_STACK_SIZE];

cpu_trap_table_t dummy_traps = { 0 };

void setUp()
{
	memset(&dummy_traps, 0, sizeof(dummy_traps));
}

void tearDown()
{
}

void _init_trap_table(void)
{
	size_t trap_stack_unused = CPU_TRAP_STACK_SIZE - sizeof(cpu_hw_context_t);

	cpu_enter_critical_Expect();
	cpu_init_hw_stackframe_Expect(g_cpu_trap_stack + trap_stack_unused, cpu_panic, ~CPU_TRAPS_ENABLED);
	cpu_exit_critical_Expect();
	TEST_ASSERT_EQUAL(CPU_STATUS_OK, cpu_traps_init(&dummy_traps));

	/* test that the interrupt stack pointer was primed correctly */
	TEST_ASSERT_EQUAL(g_cpu_trap_stack + trap_stack_unused, g_cpu_trap_sp);

	/* test that most bytes in the trap stack are set to unused */
	TEST_ASSERT_EACH_EQUAL_CHAR(CPU_MEM_UNUSED_PTRN, g_cpu_trap_stack, trap_stack_unused);
}

void test_cpu_traps_init(void)
{

	/* error condition */
	TEST_ASSERT_EQUAL(CPU_STATUS_ERROR, cpu_traps_init(NULL));
	TEST_ASSERT_EQUAL(EINVAL, errno);

	/* nominal initialization */
	_init_trap_table();
}

void test_cpu_install_and_remove_handler(void)
{
	g_cpu_trap_table_p = NULL;
	TEST_ASSERT_EQUAL(CPU_STATUS_ERROR, cpu_install_handler(0, 0, 0));

	_init_trap_table();

	unsigned int inum = 1;
	cpu_trap_callback_t fn = cpu_panic;
	void *args = 0x1234;

	cpu_enter_critical_Expect();
	cpu_exit_critical_Expect();
	TEST_ASSERT_EQUAL(CPU_STATUS_OK, cpu_install_handler(inum, fn, args));
	TEST_ASSERT_EQUAL(dummy_traps[inum].callback, fn);
	TEST_ASSERT_EQUAL(dummy_traps[inum].args, args);

	cpu_enter_critical_Expect();
	cpu_exit_critical_Expect();
	TEST_ASSERT_EQUAL(CPU_STATUS_OK, cpu_remove_handler(inum));
	TEST_ASSERT_EQUAL(dummy_traps[inum].callback, NULL);
	TEST_ASSERT_EQUAL(dummy_traps[inum].args, NULL);
}

typedef struct
{
	bool in;
	unsigned int inum;
} test_handler_trap_args_t;

void _test_handler(unsigned int tnum, cpu_trap_args_t *args)
{
	test_handler_trap_args_t *trap_args = args;
	TEST_ASSERT_EQUAL(trap_args->inum, tnum);
	TEST_ASSERT_EQUAL(trap_args->in, false);

	trap_args->in = true;
}

void test_cpu_trap_handler(void)
{
	_init_trap_table();

	test_handler_trap_args_t trap_args;
	unsigned int inum = 1;
	trap_args.in = false;
	trap_args.inum = inum;

	cpu_trap_callback_t fn = _test_handler;
	void *args = &trap_args;

	cpu_enter_critical_Expect();
	cpu_exit_critical_Expect();
	TEST_ASSERT_EQUAL(CPU_STATUS_OK, cpu_install_handler(inum, fn, args));
	TEST_ASSERT_EQUAL(dummy_traps[inum].callback, fn);
	TEST_ASSERT_EQUAL(dummy_traps[inum].args, args);

	cpu_trap(inum);
}