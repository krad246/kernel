/*
 * test_cpu.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#include <unity.h>

#define CPU_C_

#include "mock_attributes.h"
#include "mock_msp430_dummy.h"

#include "cpu.h"


void setUp()
{
}

void tearDown()
{
}

void test_cpu_idle(void)
{
	_bis_SR_register_Expect(LPM0_bits | GIE);
	cpu_idle(CPU_LPM0);

	_bis_SR_register_Expect(LPM1_bits | GIE);
	cpu_idle(CPU_LPM1);

	_bis_SR_register_Expect(LPM2_bits | GIE);
	cpu_idle(CPU_LPM2);

	_bis_SR_register_Expect(LPM3_bits | GIE);
	cpu_idle(CPU_LPM3);

	_bis_SR_register_Expect(LPM4_bits | GIE);
	cpu_idle(CPU_LPM4);

	_enable_interrupts_Expect();
	cpu_idle(CPU_LPM4 + 1);
}

void test_cpu_yield(void)
{
	cpu_yield();
}

void test_cpu_restart(void)
{
	cpu_restart();
}