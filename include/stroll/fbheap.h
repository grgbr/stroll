/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Binary heap interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      02 Jun 2024
 * @copyright Copyright (C) 2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_FBHEAP_H
#define _STROLL_FBHEAP_H

#include <stroll/array.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_fbheap_assert_api(_expr) \
	stroll_assert("stroll:fbheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_fbheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static inline __stroll_nonull(1) __stroll_const __stroll_nothrow
const void *
_stroll_fbheap_peek(const void * __restrict array)
{
	stroll_fbheap_assert_api(array);

	return array;
}

extern void
_stroll_fbheap_insert(const void * __restrict elem,
                      void * __restrict       array,
                      unsigned int            nr,
                      size_t                  size,
                      stroll_array_cmp_fn *   compare,
                      void *                  data)
	__stroll_nonull(1, 2, 5);

extern void
_stroll_fbheap_extract(void * __restrict     elem,
                       void * __restrict     array,
                       unsigned int          nr,
                       size_t                size,
                       stroll_array_cmp_fn * compare,
                       void *                data)
	__stroll_nonull(1, 2, 5);

extern void
_stroll_fbheap_build(void * __restrict     array,
                     unsigned int          nr,
                     size_t                size,
                     stroll_array_cmp_fn * compare,
                     void *                data)
	__stroll_nonull(1, 4);

struct stroll_fbheap {
	unsigned int          cnt;
	unsigned int          nr;
	size_t                size;
	void *                elems;
	stroll_array_cmp_fn * compare;
};

#define stroll_fbheap_assert_heap_api(_heap) \
	stroll_fbheap_assert_api(_heap); \
	stroll_fbheap_assert_api((_heap)->nr); \
	stroll_fbheap_assert_api((_heap)->cnt <= heap->nr); \
	stroll_fbheap_assert_api((_heap)->size); \
	stroll_fbheap_assert_api((_heap)->elems); \
	stroll_fbheap_assert_api((_heap)->compare)

#define STROLL_FBHEAP_INIT(_array, _nr, _size, _compare) \
	{ \
		.cnt     = 0, \
		.nr      = _nr, \
		.size    = _size, \
		.elems   = _array, \
		.compare = _compare \
	}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_fbheap_count(const struct stroll_fbheap * __restrict heap)
{
	stroll_fbheap_assert_heap_api(heap);

	return heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_fbheap_nr(const struct stroll_fbheap * __restrict heap)
{
	stroll_fbheap_assert_heap_api(heap);

	return heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_fbheap_isempty(const struct stroll_fbheap * __restrict heap)
{
	stroll_fbheap_assert_heap_api(heap);

	return !heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_fbheap_isfull(const struct stroll_fbheap * __restrict heap)
{
	stroll_fbheap_assert_heap_api(heap);

	return heap->cnt == heap->nr;
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbheap_clear(struct stroll_fbheap * __restrict heap)
{
	stroll_fbheap_assert_heap_api(heap);

	heap->cnt = 0;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
const void *
stroll_fbheap_peek(const struct stroll_fbheap * __restrict heap)
{
	stroll_fbheap_assert_heap_api(heap);
	stroll_fbheap_assert_api(heap->cnt);

	return _stroll_fbheap_peek(heap->elems);
}

extern void
stroll_fbheap_insert(struct stroll_fbheap * __restrict heap,
                     const void * __restrict           elem,
                     void *                            data)
	__stroll_nonull(1, 2);

extern void
stroll_fbheap_extract(struct stroll_fbheap * __restrict heap,
                      void * __restrict                 elem,
                      void *                            data)
	__stroll_nonull(1, 2);

extern void
stroll_fbheap_build(struct stroll_fbheap * __restrict heap,
                    unsigned int                      count,
                    void *                            data)
	__stroll_nonull(1);

extern void
stroll_fbheap_setup(struct stroll_fbheap * __restrict heap,
                    void * __restrict                 array,
                    unsigned int                      nr,
                    size_t                            size,
                    stroll_array_cmp_fn *             compare)
	__stroll_nonull(1, 2, 5) __stroll_nothrow __leaf;

extern struct stroll_fbheap *
stroll_fbheap_create(void * __restrict     array,
                     unsigned int          nr,
                     size_t                size,
                     stroll_array_cmp_fn * compare)
	__stroll_nonull(1, 4) __stroll_nothrow __leaf __warn_result;

static inline __stroll_nothrow
void
stroll_fbheap_destroy(struct stroll_fbheap * heap)
{
	free(heap);
}

#endif /* _STROLL_FBHEAP_H */
