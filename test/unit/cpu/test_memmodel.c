/*
 * test_memmodel.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#include <unity.h>

#define C_TRAPS_C_
#include "mock_msp430_dummy.h"

#include "memmodel.h"

void setUp()
{
}

void tearDown()
{
}

void test_cpu_init_hw_stackframe(void)
{
	cpu_hw_context_t context;

	#ifdef __MSP430X_LARGE__
		void *pc = 0xabcde;
		bool trap_en = true;
		cpu_init_hw_stackframe(&context, pc, trap_en);

		TEST_ASSERT_EQUAL((pc & 0xF0000) >> 4 | GIE , context.pc_plus_sr);
		TEST_ASSERT_EQUAL(pc & 0xFFFF , context.pc_plus_sr);

		trap_en = false;
		cpu_init_hw_stackframe(&context, pc, trap_en);

		TEST_ASSERT_EQUAL((pc & 0xF0000) >> 4 , context.pc_plus_sr);
		TEST_ASSERT_EQUAL(pc & 0xFFFF , context.pc_plus_sr);
	#else
		void *pc = 0xbcde;
		bool trap_en = true;
		cpu_init_hw_stackframe(&context, pc, trap_en);

		TEST_ASSERT_EQUAL(pc, context.pc);
		TEST_ASSERT_EQUAL(GIE, context.sr);

		trap_en = false;
		cpu_init_hw_stackframe(&context, pc, trap_en);

		TEST_ASSERT_EQUAL(pc, context.pc);
		TEST_ASSERT_EQUAL(0, context.sr);
	#endif
}

void test_cpu_init_sw_stackframe(void)
{
	cpu_sw_context_t context;
	void *arg = 0xabcde;

	cpu_init_sw_stackframe(&context, NULL);
	TEST_ASSERT_EACH_EQUAL_CHAR(0, &context, sizeof(context));

	cpu_init_sw_stackframe(&context, arg);
	TEST_ASSERT_EQUAL(arg, context.r12);
}