/*
 * critical_priv.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef CRITICAL_PRIV_H_
#define CRITICAL_PRIV_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "attributes.h"

#ifndef TEST
#include "msp430.h"
#else
#include "msp430_dummy.h"
#endif

#include "c_traps.h"

/*******************************************************************************
 * public functions
 ******************************************************************************/
EXTERN void cpu_enter_critical(void);
EXTERN void cpu_exit_critical(void);

EXTERN bool cpu_in_critical(void);

#endif /* CRITICAL_PRIV_H_ */
