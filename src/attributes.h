/*
 * attributes.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef ATTRIBUTES_H_
#define ATTRIBUTES_H_

#define EXTERN extern
#define STATIC static
#define INLINE inline

#define NORETURN __attribute__((noreturn))
#define WEAK __attribute__((weak))
#define PACKED __attribute__((packed))

#define KERNEL_CODE

#endif /* ATTRIBUTES_H_ */
