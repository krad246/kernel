/*
 * idle.h
 *
 *  Created on: Jun 8, 2021
 *      Author: krad2
 */

#ifndef IDLE_H_
#define IDLE_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "attributes.h"
#include "sched.h"

/*******************************************************************************
 * globals
 ******************************************************************************/
#if defined(SCHED_C_)
    EXTERN k_status_code_t k_idle(k_thread_arg_t *unused);
#endif

#endif /* IDLE_H_ */
