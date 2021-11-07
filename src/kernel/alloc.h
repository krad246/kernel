/*
 * alloc.h
 *
 *  Created on: Jun 8, 2021
 *      Author: krad2
 */

#ifndef ALLOC_H_
#define ALLOC_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stdlib.h>
#include <stddef.h>
#include "attributes.h"

/*******************************************************************************
 * globals
 ******************************************************************************/
EXTERN void *k_alloc(size_t bytes);
EXTERN void k_free(void *ptr);

#endif /* ALLOC_H_ */
