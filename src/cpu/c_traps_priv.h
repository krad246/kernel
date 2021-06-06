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

/*******************************************************************************
 * defines
 ******************************************************************************/

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
STATIC cpu_trap_table_t *g_cpu_trap_table_p = NULL;
STATIC volatile bool g_cpu_in_trap = false;

#endif /* C_TRAPS_PRIV_H_ */
