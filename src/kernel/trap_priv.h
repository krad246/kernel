/*
 * trap_priv.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef TRAP_PRIV_H_
#define TRAP_PRIV_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "c_traps.h"
#include "trap_defs.h"
#include "attributes.h"
#include "utils.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
typedef cpu_trap_args_t     k_trap_args_t;
typedef cpu_trap_callback_t k_trap_callback_t;

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
STATIC cpu_trap_table_t g_kern_trap_table;

#endif