/*
 * idle.c
 *
 *  Created on: Jun 8, 2021
 *      Author: krad2
 */

#define IDLE_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "thread.h"
#include "cpu.h"

/*******************************************************************************
 * public functions
 ******************************************************************************/
k_status_code_t k_idle(k_thread_arg_t *unused)
{
    while (1)
    {
        cpu_idle();
    }

    return 0;
}