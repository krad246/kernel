/*
 * k_status.h
 *
 *  Created on: Jun 8, 2021
 *      Author: krad2
 */

#ifndef K_STATUS_H_
#define K_STATUS_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <errno.h>
#include "status.h"

/*******************************************************************************
 * defines
 ******************************************************************************/
typedef enum 
{
	K_STATUS_OK = CPU_STATUS_OK,
	K_STATUS_ERROR = CPU_STATUS_ERROR,
} k_status_code_t;

#endif
