/*
 * c_traps_priv.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef C_TRAPS_PRIV_H_
#define C_TRAPS_PRIV_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <msp430.h>

#include <stddef.h>
#include <string.h>

#include "c_traps.h"
#include "critical.h"

#include "memmodel.h"

/*******************************************************************************
 * defines
 ******************************************************************************/

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
STATIC volatile cpu_trap_table_t *g_cpu_trap_table_p = NULL;
STATIC volatile bool g_cpu_in_trap = false;

EXTERN volatile void *g_cpu_trap_sp;
EXTERN volatile unsigned char g_cpu_trap_stack[CPU_TRAP_STACK_SIZE];

#endif /* C_TRAPS_PRIV_H_ */
