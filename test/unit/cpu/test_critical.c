/*
 * test_critical.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#include <unity.h>

#include "mock_attributes.h"
#include "mock_msp430_dummy.h"
#include "mock_cpu.h"

#include "critical.h"

extern volatile int g_cpu_crit_sect_count;

void setUp()
{
	g_cpu_crit_sect_count = 0;
}

void tearDown()
{
}

void test_cpu_enter_critical(void)
{
	_get_interrupt_state_ExpectAndReturn(CPU_TRAPS_ENABLED);
	_disable_interrupts_Expect();
	cpu_enter_critical();
	TEST_ASSERT_EQUAL(1, g_cpu_crit_sect_count);

	_get_interrupt_state_ExpectAndReturn(0);
	cpu_enter_critical();
	TEST_ASSERT_EQUAL(2, g_cpu_crit_sect_count);
}

void test_cpu_exit_critical(void)
{
	test_cpu_enter_critical();

	cpu_exit_critical();
	TEST_ASSERT_EQUAL(1, g_cpu_crit_sect_count);

	_enable_interrupts_Expect();
	cpu_exit_critical();
	TEST_ASSERT_EQUAL(1, g_cpu_crit_sect_count);
}