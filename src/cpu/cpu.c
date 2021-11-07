/*
 * c_traps.c
 *
 *  Created on: Jun 3, 2021
 *      Author: krad2
 */

#define CPU_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "cpu.h"
#include "cpu_priv.h"

/*******************************************************************************
 * defines
 ******************************************************************************/

/*******************************************************************************
 * private functions
 ******************************************************************************/
STATIC void cpu_spin(void)
{
	__eint();
	while (FOREVER);
}

/*******************************************************************************
 * public functions
 ******************************************************************************/
WEAK void cpu_idle(cpu_lpm_t level)
{
	switch (level)
	{
		case CPU_LPM0: { __low_power_mode_0(); } break;
		case CPU_LPM1: { __low_power_mode_1(); } break;
		case CPU_LPM2: { __low_power_mode_2(); } break;
		case CPU_LPM3: { __low_power_mode_3(); } break;
		case CPU_LPM4: { __low_power_mode_4(); } break;
		default: { cpu_spin(); } break;
	}
}

WEAK void cpu_yield(void)
{
	//        t
}

WEAK void cpu_restart(void)
{
	//        wdog_violate();
}