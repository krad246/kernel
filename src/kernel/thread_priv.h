/*
 * thread_priv.h
 *
 *  Created on: Jun 7, 2021
 *      Author: krad2
 */

#ifndef THREAD_PRIV_H_
#define THREAD_PRIV_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "attributes.h"
#include "c_traps.h"
#include "memmodel.h"
#include "mem.h"
#include "sched.h"
#include "utils.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
#define K_THREAD_STACK_BOTTOM(this) ((this)->stack_mem + (this)->stack_mem_size)
#define K_MEM_UNUSED_PTRN (0x5A)

/*******************************************************************************
 * data structures
 ******************************************************************************/

#endif /* THREAD_PRIV_H_ */
