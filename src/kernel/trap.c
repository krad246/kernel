/*
 * trap.c
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#define TRAP_C_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include "trap_priv.h"
#include "trap.h"

/*******************************************************************************
 * file-scope globals
 ******************************************************************************/
STATIC cpu_trap_table_t g_kern_trap_table;

/*******************************************************************************
 * private functions
 ******************************************************************************/
STATIC bool k_is_legal_trap(unsigned int tnum)
{
	return IN_RANGE(K_MIN_VALID_TRAP_NUM, tnum, K_MAX_VALID_TRAP_NUM);
}

STATIC bool k_is_rsvd_trap(unsigned int tnum)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(g_kern_rsvd_traps); i++)
	{
		if (g_kern_rsvd_traps[i] == tnum)
		{
			return true;
		}
	}

	return false;
}

/*******************************************************************************
 * public functions
 ******************************************************************************/
int k_traps_init(void)
{
	return cpu_traps_init((void *)&g_kern_trap_table);
}

int k_trap_connect(unsigned int tnum, k_trap_callback_t callback, k_trap_args_t *args)
{
	if (!k_is_legal_trap(tnum))
	{
		errno = EINVAL;
		return K_STATUS_ERROR;
	}

	if (k_is_rsvd_trap(tnum))
	{
		errno = EACCES;
		return K_STATUS_ERROR;
	}

	return cpu_install_handler(tnum, callback, args);
}

int k_trap_disconnect(unsigned int tnum)
{
	if (!k_is_legal_trap(tnum))
	{
		return K_STATUS_ERROR;
	}

	if (k_is_rsvd_trap(tnum))
	{
		return K_STATUS_ERROR;
	}

	return cpu_remove_handler(tnum);
}

bool k_trap_in_progress(void)
{
	return cpu_trap_in_progress();
}
