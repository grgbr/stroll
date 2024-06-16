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

extern void
stroll_array_swap32(uint32_t * __restrict first, uint32_t * __restrict second)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __export_intern;

extern void
stroll_array_swap64(uint64_t * __restrict first, uint64_t * __restrict second)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __export_intern;

extern void
stroll_array_swap(void * __restrict first,
                  void * __restrict second,
                  size_t            size)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __export_intern;

#endif /* _STROLL_ARRAY_INTERN_H */
