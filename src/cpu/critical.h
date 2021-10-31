/*
 * critical.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef CRITICAL_H_
#define CRITICAL_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stdbool.h>
#include "attributes.h"

/*******************************************************************************
 * public functions
 ******************************************************************************/
EXTERN void cpu_enter_critical(void);
EXTERN void cpu_exit_critical(void);

EXTERN bool cpu_in_critical(void);

#endif /* CRITICAL_H_ */
