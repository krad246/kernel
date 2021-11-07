/*
 * attributes.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef ATTRIBUTES_H_
#define ATTRIBUTES_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stdbool.h>

#ifndef TEST

#define EXTERN extern
#define STATIC static
#define INLINE inline

#define NORETURN __attribute__((noreturn))
#define WEAK __attribute__((weak))
#define PACKED __attribute__((packed))
#define NAKED __attribute__((naked))
#define INTERRUPT(x) __attribute__((interrupt(x))) 
#define FOREVER	(1)

#else 

#define EXTERN
#define STATIC
#define INLINE

#define NORETURN
#define WEAK
#define PACKED
#define NAKED

bool forever(void);
#define FOREVER ({  forever(); })

#endif

#endif /* ATTRIBUTES_H_ */
