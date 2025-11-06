/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Allocator interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      6 Nov 2025
 * @copyright Copyright (C) 2017-2025 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_ALLOC_H
#define _STROLL_ALLOC_H

#include <stroll/cdefs.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_alloc_assert_api(_expr) \
	stroll_assert("stroll:alloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_alloc_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

struct stroll_alloc;

typedef void *
        stroll_alloc_fn(struct stroll_alloc * __restrict);

typedef void
        stroll_free_fn(struct stroll_alloc * __restrict, void * __restrict);

typedef void
        stroll_fini_fn(struct stroll_alloc * __restrict);

struct stroll_alloc_ops {
	stroll_alloc_fn * alloc;
	stroll_free_fn *  free;
	stroll_fini_fn *  fini;
};

#define stroll_alloc_assert_ops_api(_ops) \
	stroll_alloc_assert_api(_ops); \
	stroll_alloc_assert_api((_ops)->alloc); \
	stroll_alloc_assert_api((_ops)->free); \
	stroll_alloc_assert_api((_ops)->fini)

struct stroll_alloc {
	const struct stroll_alloc_ops * ops;
};

static inline __stroll_nonull(1)
void
stroll_free(struct stroll_alloc * __restrict allocator,
            void * __restrict                chunk)
{
	stroll_alloc_assert_api(allocator);
	stroll_alloc_assert_ops_api(allocator->ops);

	allocator->ops->free(allocator, chunk);
}

static inline __stroll_nonull(1) __warn_result
void *
stroll_alloc(struct stroll_alloc * __restrict allocator)
{
	stroll_alloc_assert_api(allocator);
	stroll_alloc_assert_ops_api(allocator->ops);

	return allocator->ops->alloc(allocator);
}

static inline __stroll_nonull(1)
void
stroll_alloc_fini(struct stroll_alloc * __restrict allocator)
{
	stroll_alloc_assert_api(allocator);
	stroll_alloc_assert_ops_api(allocator->ops);

	allocator->ops->fini(allocator);
}

extern void
stroll_alloc_destroy(struct stroll_alloc * __restrict allocator)
	__stroll_nonull(1);

#endif /* _STROLL_ALLOC_H */
