/*
 * cpu.h
 *
 *  Created on: Jun 8, 2021
 *      Author: krad2
 */

#ifndef CPU_H_
#define CPU_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "attributes.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
#define CPU_FREQ
#define CPU_TRAPS_ENABLED (GIE)

typedef enum
{
	CPU_LPM0,
	CPU_LPM1,
	CPU_LPM2,
	CPU_LPM3,
	CPU_LPM4
} cpu_lpm_t;

/*******************************************************************************
 * public functions
 ******************************************************************************/
EXTERN WEAK void cpu_idle(cpu_lpm_t level);
EXTERN WEAK void cpu_yield(void);
EXTERN WEAK void cpu_restart(void);


#endif /* CPU_H_ */
