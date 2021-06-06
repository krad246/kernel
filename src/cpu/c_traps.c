/*
 * c_traps.c
 *
 *  Created on: Jun 3, 2021
 *      Author: krad2
 */

#define C_TRAPS_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "c_traps.h"
#include "c_traps_priv.h"

/*******************************************************************************
 * defines
 ******************************************************************************/

/*******************************************************************************
 * public functions
 ******************************************************************************/
int cpu_traps_init(void *table_p)
{
    if (table_p == NULL)
    {
        // TODO: change this to error case
        return -1;
    }

    cpu_enter_critical();

    g_cpu_in_trap = false;

    g_cpu_trap_table_p = (cpu_trap_table_t *) table_p;
    memset(g_cpu_trap_table_p, 0, sizeof(*g_cpu_trap_table_p));

    cpu_exit_critical();

    return 0;
}

int cpu_install_handler(unsigned int tnum, cpu_trap_callback_t callback, cpu_trap_args_t *args)
{
    if (g_cpu_trap_table_p == NULL)
    {
        // TODO: change this to error case
        return -1;
    }

    cpu_trap_handler_t *old_handler = &(*g_cpu_trap_table_p)[tnum];

    cpu_enter_critical();

    old_handler->callback = callback;
    old_handler->args = args;

    cpu_exit_critical();

    return 0;
}

int cpu_remove_handler(unsigned int tnum)
{
    return cpu_install_handler(tnum, NULL, NULL);
}

volatile bool cpu_trap_in_progress(void)
{
    return g_cpu_in_trap;
}

WEAK bool cpu_context_switch_requested(void)
{
    return false;
}

WEAK void cpu_context_switch(void)
{

}

void cpu_trap(unsigned int tnum)
{
    g_cpu_in_trap = true;

    if (g_cpu_trap_table_p != NULL)
    {
        cpu_trap_handler_t *handler = &(*g_cpu_trap_table_p)[tnum];
        if (handler->callback != NULL)
        {
            handler->callback(tnum, handler->args);
        }
    }

    g_cpu_in_trap = false;

    if (cpu_context_switch_requested())
    {
        cpu_context_switch();
    }
}

WEAK NORETURN void cpu_panic(unsigned int tnum)
{
    __bis_SR_register(LPM3_bits & ~GIE);
    while (1);
}
