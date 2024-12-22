/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Paring heap interface (half-ordered binary tree based)
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      09 Sep 2024
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_HPRHEAP_H
#define _STROLL_HPRHEAP_H

#include <stroll/cdefs.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_hprheap_assert_api(_expr) \
	stroll_assert("stroll:hprheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_hprheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

struct stroll_hprheap_node;

typedef int (stroll_hprheap_cmp_fn)
            (const struct stroll_hprheap_node * __restrict,
             const struct stroll_hprheap_node * __restrict,
             void *)
	__stroll_nonull(1, 2) __warn_result;

struct stroll_hprheap_base {
	struct stroll_hprheap_node * root;
};

#define STROLL_HPRHEAP_BASE_INIT \
	{ .root = NULL }

struct stroll_hprheap_node {
	struct stroll_hprheap_node * children[2];
};

#define stroll_hprheap_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_hprheap_base_isempty(const struct stroll_hprheap_base * __restrict heap)
{
	stroll_hprheap_assert_api(heap);

	return !heap->root;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_hprheap_node *
stroll_hprheap_base_peek(const struct stroll_hprheap_base * __restrict heap)
{
	stroll_hprheap_assert_api(heap);
	stroll_hprheap_assert_api(heap->root);

	return heap->root;
}

extern void
stroll_hprheap_base_insert(struct stroll_hprheap_base * __restrict heap,
                           struct stroll_hprheap_node * __restrict node,
                           stroll_hprheap_cmp_fn   *               compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_hprheap_base_merge(struct stroll_hprheap_base * __restrict       result,
                          const struct stroll_hprheap_base * __restrict source,
                          stroll_hprheap_cmp_fn *                       compare,
                          void *                                        data)
	__stroll_nonull(1, 3);

extern struct stroll_hprheap_node *
stroll_hprheap_base_extract(struct stroll_hprheap_base * __restrict heap,
                            stroll_hprheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2);

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_hprheap_base_setup(struct stroll_hprheap_base * __restrict heap)
{
	stroll_hprheap_assert_api(heap);

	heap->root = NULL;
}

struct stroll_hprheap {
	unsigned int               cnt;
	unsigned int               nr;
	struct stroll_hprheap_base base;
	stroll_hprheap_cmp_fn *    compare;
};

#define stroll_hprheap_assert_heap_api(_heap) \
	stroll_hprheap_assert_api(_heap); \
	stroll_hprheap_assert_api((_heap)->nr); \
	stroll_hprheap_assert_api((_heap)->cnt <= (_heap)->nr); \
	stroll_hprheap_assert_api(!!(_heap)->cnt ^ \
	                          stroll_hprheap_base_isempty(&(_heap)->base)); \
	stroll_hprheap_assert_api((_heap)->compare)

#define STROLL_HPRHEAP_INIT(_nr, _compare) \
	{ \
		.cnt     = 0, \
		.nr      = _nr, \
		.base    = STROLL_HPRHEAP_BASE_INIT, \
		.compare = _compare \
	}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_hprheap_count(const struct stroll_hprheap * __restrict heap)
{
	stroll_hprheap_assert_heap_api(heap);

	return heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_hprheap_nr(const struct stroll_hprheap * __restrict heap)
{
	stroll_hprheap_assert_heap_api(heap);

	return heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_hprheap_isempty(const struct stroll_hprheap * __restrict heap)
{
	stroll_hprheap_assert_heap_api(heap);

	return stroll_hprheap_base_isempty(&heap->base);
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_hprheap_isfull(const struct stroll_hprheap * __restrict heap)
{
	stroll_hprheap_assert_heap_api(heap);

	return heap->cnt == heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_hprheap_node *
stroll_hprheap_peek(const struct stroll_hprheap * __restrict heap)
{
	stroll_hprheap_assert_heap_api(heap);

	return stroll_hprheap_base_peek(&heap->base);
}

extern void
stroll_hprheap_insert(struct stroll_hprheap * __restrict      heap,
                      struct stroll_hprheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

extern void
stroll_hprheap_merge(struct stroll_hprheap * __restrict       result,
                     const struct stroll_hprheap * __restrict source,
                     void *                                   data)
	__stroll_nonull(1, 2);

extern struct stroll_hprheap_node *
stroll_hprheap_extract(struct stroll_hprheap * __restrict heap, void * data)
	__stroll_nonull(1);

extern void
stroll_hprheap_setup(struct stroll_hprheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_hprheap_cmp_fn *            compare)
	__stroll_nonull(1, 3) __stroll_nothrow __leaf;

extern struct stroll_hprheap *
stroll_hprheap_create(unsigned int nr, stroll_hprheap_cmp_fn * compare)
	__stroll_nonull(2) __stroll_nothrow __leaf __warn_result;

static inline __stroll_nothrow
void
stroll_hprheap_destroy(struct stroll_hprheap * heap)
{
	free(heap);
}

#endif /* _STROLL_HPRHEAP_H */
