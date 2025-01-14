/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Weak heap interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      02 Jun 2024
 * @copyright Copyright (C) 2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_FWHEAP_H
#define _STROLL_FWHEAP_H

#include <stroll/array.h>
#include <stroll/fbmap.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_fwheap_assert_api(_expr) \
	stroll_assert("stroll:fwheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_fwheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#define _STROLL_FWHEAP_RBITS_SIZE(_nr) \
	(STROLL_FBMAP_WORD_NR(_nr) * sizeof(unsigned long))

static inline __stroll_const __stroll_nothrow
unsigned int
_stroll_fwheap_rbits_size(unsigned int nr)
{
	stroll_fwheap_assert_api(nr);

	return stroll_fbmap_word_nr(nr) * sizeof(unsigned long);
}

static inline __stroll_nonull(1) __stroll_const __stroll_nothrow
const void *
_stroll_fwheap_peek(const void * __restrict array)
{
	stroll_fwheap_assert_api(array);

	return array;
}

extern void
_stroll_fwheap_insert(const void * __restrict    elem,
                      void * __restrict          array,
                      unsigned long * __restrict rbits,
                      unsigned int               nr,
                      size_t                     size,
                      stroll_array_cmp_fn *      compare,
                      void *                     data)
	__stroll_nonull(1, 2, 3, 6);

extern void
_stroll_fwheap_extract(void * __restrict          elem,
                       void * __restrict          array,
                       unsigned long * __restrict rbits,
                       unsigned int               nr,
                       size_t                     size,
                       stroll_array_cmp_fn *      compare,
                       void *                     data)
	__stroll_nonull(1, 2, 3, 6);

extern void
_stroll_fwheap_build(void * __restrict          array,
                     unsigned long * __restrict rbits,
                     unsigned int               nr,
                     size_t                     size,
                     stroll_array_cmp_fn *      compare,
                     void *                     data)
	__stroll_nonull(1, 2, 5);

extern unsigned long *
_stroll_fwheap_alloc_rbits(unsigned int nr)
	__stroll_nothrow __leaf __warn_result;

static inline __stroll_nothrow
void
_stroll_fwheap_free_rbits(unsigned long * rbits)
{
	free(rbits);
}

/**
 * Fixed sized array based weak heap.
 *
 * An opaque structure holding an array based weak heap which maximum size is
 * fixed at instantiation time.
 */
struct stroll_fwheap {
	/**
	 * @internal
	 *
	 * Count of elements stored into the heap array.
	 */
	unsigned int          cnt;
	/**
	 * @internal
	 *
	 * Maximum number of elements this heap array may hold.
	 */
	unsigned int          nr;
	/**
	 * @internal
	 *
	 * Size of a single heap array element in bytes.
	 */
	size_t                size;
	/**
	 * @internal
	 *
	 * Internal array of "reverse bits" used to find so-called
	 * "distinguished ancestors" during weak heap operations.
	 */
	void *                rbits;
	/**
	 * @internal
	 *
	 * Memory area where heap array elements are stored.
	 */
	void *                elems;
	/**
	 * @internal
	 *
	 * Heap array element comparison function hook.
	 */
	stroll_array_cmp_fn * compare;
};

#define stroll_fwheap_assert_heap_api(_heap) \
	stroll_fwheap_assert_api(_heap); \
	stroll_fwheap_assert_api((_heap)->nr); \
	stroll_fwheap_assert_api((_heap)->cnt <= heap->nr); \
	stroll_fwheap_assert_api((_heap)->size); \
	stroll_fwheap_assert_api((_heap)->rbits); \
	stroll_fwheap_assert_api((_heap)->elems); \
	stroll_fwheap_assert_api((_heap)->compare)

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_fwheap_count(const struct stroll_fwheap * __restrict heap)
{
	stroll_fwheap_assert_heap_api(heap);

	return heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_fwheap_nr(const struct stroll_fwheap * __restrict heap)
{
	stroll_fwheap_assert_heap_api(heap);

	return heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_fwheap_isempty(const struct stroll_fwheap * __restrict heap)
{
	stroll_fwheap_assert_heap_api(heap);

	return !heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_fwheap_isfull(const struct stroll_fwheap * __restrict heap)
{
	stroll_fwheap_assert_heap_api(heap);

	return heap->cnt == heap->nr;
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fwheap_clear(struct stroll_fwheap * __restrict heap)
{
	stroll_fwheap_assert_heap_api(heap);

	heap->cnt = 0;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
const void *
stroll_fwheap_peek(const struct stroll_fwheap * __restrict heap)
{
	stroll_fwheap_assert_heap_api(heap);
	stroll_fwheap_assert_api(heap->cnt);

	return _stroll_fwheap_peek(heap->elems);
}

extern void
stroll_fwheap_insert(struct stroll_fwheap * __restrict heap,
                     const void * __restrict           elem,
                     void *                            data)
	__stroll_nonull(1, 2);

extern void
stroll_fwheap_extract(struct stroll_fwheap * __restrict heap,
                      void * __restrict                 elem,
                      void *                            data)
	__stroll_nonull(1, 2);

extern void
stroll_fwheap_build(struct stroll_fwheap * __restrict heap,
                    unsigned int                      count,
                    void *                            data)
	__stroll_nonull(1);

extern int
stroll_fwheap_init(struct stroll_fwheap * __restrict heap,
                   void * __restrict                 array,
                   unsigned int                      nr,
                   size_t                            size,
                   stroll_array_cmp_fn *             compare)
	__stroll_nonull(1, 2, 5) __stroll_nothrow __leaf;

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fwheap_fini(struct stroll_fwheap * __restrict heap)
{
	stroll_fwheap_assert_heap_api(heap);

	_stroll_fwheap_free_rbits(heap->rbits);
}

extern struct stroll_fwheap *
stroll_fwheap_create(void * __restrict     array,
                     unsigned int          nr,
                     size_t                size,
                     stroll_array_cmp_fn * compare)
	__stroll_nonull(1, 4) __stroll_nothrow __leaf __warn_result;

extern void
stroll_fwheap_destroy(struct stroll_fwheap * heap)
	__stroll_nonull(1) __stroll_nothrow __leaf;

#endif /* _STROLL_FWHEAP_H */
