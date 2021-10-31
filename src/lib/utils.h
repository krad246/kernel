/*
 * utils.h
 *
 *  Created on: Jun 4, 2021
 *      Author: krad2
 */

#ifndef UTILS_H_
#define UTILS_H_

/*******************************************************************************
 * includes
 ******************************************************************************/
#include <stddef.h>

/*******************************************************************************
 * Arithmetic macros
 ******************************************************************************/
#define IN_RANGE(low, val, high) (((low) <= (val)) && ((val) <= (high)))
#define IS_BETWEEN(low, val, high) (IN_RANGE(low, val, high))

#define IS_ODD(x) ((x)&1)
#define IS_EVEN(x) (!IS_EVEN(x))
#define IS_POW2(x) (((x) & ((x)-1)) && (x))

#define MAXIMUM(a, b)          \
	(                          \
		{                      \
			DECLTYPE(a)        \
			_a = (a);          \
			DECLTYPE(b)        \
			_b = (b);          \
			_a > _b ? _a : _b; \
		})

#define MINIMUM(a, b)          \
	(                          \
		{                      \
			DECLTYPE(a)        \
			_a = (a);          \
			DECLTYPE(b)        \
			_b = (b);          \
			_a < _b ? _a : _b; \
		})

#define BIT_WIDTH(t) (sizeof((t)) * 8)
#define BIT(x, i)					((( DECLTYPE(x) )) 1) << (i))
#define TOP_BIT(x) (BIT(x, BIT_WIDTH(i) - 1))

/*******************************************************************************
 * Memory macros
 ******************************************************************************/
#define IS_POW2_ALIGNED(size, pow2) (((size) & ((pow2)-1)) == 0)
#define IS_MEM_ALIGNED(p, size) (IS_POW2_ALIGNED((uintptr_t)(p), size))
#define ALIGN_SIZE(size, pow2) (((size) + (pow2)-1) & (~((pow2)-1)))

/*******************************************************************************
 * Type system macros
 ******************************************************************************/
#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr[0])))
#define DECLTYPE(x) typeof((x))
#define FIELD_SIZEOF(t, field) (sizeof(((t *)0)->field))
#define CONTAINER_OF(ptr, type, member)                        \
	(                                                          \
		{                                                      \
			const typeof(((type *)0)->member) *__mptr = (ptr); \
			(type *)((char *)__mptr - offsetof(type, member)); \
		})

/*******************************************************************************
 * Stringification macros
 ******************************************************************************/
#define _JOIN(a, b) a##b
#define JOIN(a, b) _JOIN(a, b)

#define _STR(x) #x
#define STR(x) _STR(x)

#endif /* UTILS_H_ */
