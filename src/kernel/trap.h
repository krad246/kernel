/*
 * trap.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef TRAP_H_
#define TRAP_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "c_traps.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
typedef cpu_trap_args_t     k_trap_args_t;
typedef cpu_trap_callback_t k_trap_callback_t;

/*******************************************************************************
 * public functions
 ******************************************************************************/
EXTERN int k_traps_init(void);
EXTERN int k_trap_connect(unsigned int tnum, k_trap_callback_t callback, k_trap_args_t *args);
EXTERN int k_trap_disconnect(unsigned int tnum);
EXTERN volatile bool k_trap_in_progress(void);

#endif