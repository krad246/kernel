/*
 * critical.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "attributes.h"
#include "msp430.h"
#include "mem.h"

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
STATIC int g_cpu_crit_sect_count = 0;

/*******************************************************************************
 * public functions
 ******************************************************************************/
void cpu_enter_critical(void) {
    if (__get_interrupt_state() & GIE) {
        _disable_interrupts();
    }

    g_cpu_crit_sect_count++;
}

void cpu_exit_critical(void) {
    g_cpu_crit_sect_count--;
    if (g_cpu_crit_sect_count == 0) {
        g_cpu_crit_sect_count = 1;
        _enable_interrupts();
    }
}

bool cpu_in_critical(void)
{
	return !(__get_interrupt_state() & GIE);
}
