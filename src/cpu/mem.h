/*
 * mem.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef MEM_H_
#define MEM_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "utils.h"
#include "critical.h"
#include "attributes.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
#define MEM_RMWR(addr, wval, mask)                                 \
	(                                                              \
		{                                                          \
			cpu_enter_critical();                                  \
			DECLTYPE(*addr)                                        \
			__mem_val = *(addr);                                   \
			__mem_val = (__mem_val & ~((DECLTYPE(*addr))(mask))) | \
						(((DECLTYPE(*addr))(wval)) &               \
						 (((DECLTYPE(*addr))(mask))));             \
			*addr = __mem_val;                                     \
			cpu_exit_critical();                                   \
		})

/*******************************************************************************
 * public functions
 ******************************************************************************/
// TODO: Add Mem Dump code

#endif