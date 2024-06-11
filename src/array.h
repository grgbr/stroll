/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _STROLL_ARRAY_INTERN_H
#define _STROLL_ARRAY_INTERN_H

#include "stroll/array.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_array_assert_api(_expr) \
	stroll_assert("stroll:array", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_array_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_array_assert_intern(_expr) \
	stroll_assert("stroll:array", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_array_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

static inline __stroll_nonull(1) __stroll_const __stroll_nothrow
bool
stroll_array_aligned(const void * __restrict array, size_t size, size_t align)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(align);

	return (size == align) && stroll_aligned((unsigned long)array, align);
}

#define STROLL_ARRAY_DEFINE_SWAP(_func, _type) \
	static inline __stroll_nonull(1, 2) \
	void \
	_func(_type * __restrict first, _type * __restrict second) \
	{ \
		stroll_array_assert_intern(first); \
		stroll_array_assert_intern(second); \
		\
		_type tmp; \
		\
		tmp = *first; \
		*first = *second; \
		*second = tmp; \
	}

STROLL_ARRAY_DEFINE_SWAP(stroll_array_swap32, uint32_t)

STROLL_ARRAY_DEFINE_SWAP(stroll_array_swap64, uint64_t)

static inline __stroll_nonull(1, 2)
void
stroll_array_swap(void * __restrict first,
                  void * __restrict second,
                  size_t            size)
{
	stroll_array_assert_intern(first);
	stroll_array_assert_intern(second);
	stroll_array_assert_intern(size);

	char tmp[size];

	memcpy(tmp, first, size);
	memcpy(first, second, size);
	memcpy(second, tmp, size);
}

#endif /* _STROLL_ARRAY_INTERN_H */
