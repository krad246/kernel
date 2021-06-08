/*
 * cpu.h
 *
 *  Created on: Jun 8, 2021
 *      Author: krad2
 */

#ifndef CPU_H_
#define CPU_H_

#include <msp430.h>
#include "attributes.h"

WEAK void cpu_idle(void)
{
    __low_power_mode_3();
}

WEAK void cpu_yield(void)
{
//        t
}

WEAK void cpu_restart(void)
{
//        wdog_violate();
}

#endif /* CPU_H_ */
