/*
 * critical.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "critical.h"
#include "critical_priv.h"

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
STATIC volatile int g_cpu_crit_sect_count = 0;

/*******************************************************************************
 * public functions
 ******************************************************************************/
void cpu_enter_critical(void)
{
	if (!cpu_in_critical())
	{
		_disable_interrupts();
	}

	g_cpu_crit_sect_count++;
}

void cpu_exit_critical(void)
{
	g_cpu_crit_sect_count--;
	if (g_cpu_crit_sect_count == 0)
	{
		g_cpu_crit_sect_count = 1;
		_enable_interrupts();
	}
}

bool cpu_in_critical(void)
{
	return !(__get_interrupt_state() & CPU_TRAPS_ENABLED);
}
