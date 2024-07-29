/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Pairing heap interface (heap-ordered multi-way tree with parent based)
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      15 Nov 2017
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_DPRHEAP_H
#define _STROLL_DPRHEAP_H

#include <stroll/dlist.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_dprheap_assert_api(_expr) \
	stroll_assert("stroll:dprheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_dprheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

struct stroll_dprheap_node;

typedef int (stroll_dprheap_cmp_fn)
            (const struct stroll_dprheap_node * __restrict,
             const struct stroll_dprheap_node * __restrict,
             void *)
	__stroll_nonull(1, 2) __warn_result;

struct stroll_dprheap_base {
	struct stroll_dprheap_node * root;
};

#define STROLL_DPRHEAP_BASE_INIT(_heap) \
	{ .root = NULL }

struct stroll_dprheap_node {
	struct stroll_dlist_node     siblings;
	struct stroll_dprheap_node * parent;
	struct stroll_dlist_node     children;
};

#define stroll_dprheap_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_dprheap_base_isempty(const struct stroll_dprheap_base * __restrict heap)
{
	stroll_dprheap_assert_api(heap);

	return !heap->root;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_dprheap_node *
stroll_dprheap_base_peek(const struct stroll_dprheap_base * __restrict heap)
{
	stroll_dprheap_assert_api(heap);
	stroll_dprheap_assert_api(heap->root);

	return heap->root;
}

extern void
stroll_dprheap_base_insert(struct stroll_dprheap_base * __restrict heap,
                           struct stroll_dprheap_node * __restrict node,
                           stroll_dprheap_cmp_fn   *               compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_dprheap_base_merge(struct stroll_dprheap_base * __restrict       result,
                          const struct stroll_dprheap_base * __restrict source,
                          stroll_dprheap_cmp_fn *                       compare,
                          void *                                        data)
	__stroll_nonull(1, 3);

extern struct stroll_dprheap_node *
stroll_dprheap_base_extract(struct stroll_dprheap_base * __restrict heap,
                            stroll_dprheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2);

extern void
stroll_dprheap_base_remove(struct stroll_dprheap_base * __restrict heap,
                           struct stroll_dprheap_node * __restrict node,
                           stroll_dprheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_dprheap_base_promote(struct stroll_dprheap_base * __restrict heap,
                            struct stroll_dprheap_node * __restrict node,
                            stroll_dprheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_dprheap_base_demote(struct stroll_dprheap_base * __restrict heap,
                           struct stroll_dprheap_node * __restrict node,
                           stroll_dprheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dprheap_base_setup(struct stroll_dprheap_base * __restrict heap)
{
	stroll_dprheap_assert_api(heap);

	heap->root = NULL;
}

struct stroll_dprheap {
	unsigned int               cnt;
	unsigned int               nr;
	struct stroll_dprheap_base base;
	stroll_dprheap_cmp_fn *    compare;
};

#define stroll_dprheap_assert_heap_api(_heap) \
	stroll_dprheap_assert_api(_heap); \
	stroll_dprheap_assert_api((_heap)->nr); \
	stroll_dprheap_assert_api((_heap)->cnt <= (_heap)->nr); \
	stroll_dprheap_assert_api(!!(_heap)->cnt ^ \
	                         stroll_dprheap_base_isempty(&(_heap)->base)); \
	stroll_dprheap_assert_api((_heap)->compare)

#define STROLL_DPRHEAP_INIT(_nr, _compare) \
	{ \
		.cnt     = 0, \
		.nr      = _nr, \
		.base    = STROLL_DPRHEAP_BASE_INIT((_heap).base), \
		.compare = _compare \
	}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_dprheap_count(const struct stroll_dprheap * __restrict heap)
{
	stroll_dprheap_assert_heap_api(heap);

	return heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_dprheap_nr(const struct stroll_dprheap * __restrict heap)
{
	stroll_dprheap_assert_heap_api(heap);

	return heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_dprheap_isempty(const struct stroll_dprheap * __restrict heap)
{
	stroll_dprheap_assert_heap_api(heap);

	return stroll_dprheap_base_isempty(&heap->base);
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_dprheap_isfull(const struct stroll_dprheap * __restrict heap)
{
	stroll_dprheap_assert_heap_api(heap);

	return heap->cnt == heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_dprheap_node *
stroll_dprheap_peek(const struct stroll_dprheap * __restrict heap)
{
	stroll_dprheap_assert_heap_api(heap);

	return stroll_dprheap_base_peek(&heap->base);
}

extern void
stroll_dprheap_insert(struct stroll_dprheap * __restrict      heap,
                      struct stroll_dprheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

extern void
stroll_dprheap_merge(struct stroll_dprheap * __restrict       result,
                     const struct stroll_dprheap * __restrict source,
                     void *                                   data)
	__stroll_nonull(1, 2);

extern struct stroll_dprheap_node *
stroll_dprheap_extract(struct stroll_dprheap * __restrict heap, void * data)
	__stroll_nonull(1);

extern void
stroll_dprheap_remove(struct stroll_dprheap * __restrict      heap,
                      struct stroll_dprheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

static inline __stroll_nonull(1, 2)
void
stroll_dprheap_promote(struct stroll_dprheap * __restrict      heap,
                       struct stroll_dprheap_node * __restrict node,
                       void *                                  data)
{
	stroll_dprheap_assert_heap_api(heap);
	stroll_dprheap_assert_api(heap->cnt);

	stroll_dprheap_base_promote(&heap->base, node, heap->compare, data);
}

static inline __stroll_nonull(1, 2)
void
stroll_dprheap_demote(struct stroll_dprheap * __restrict      heap,
                      struct stroll_dprheap_node * __restrict node,
                      void *                                  data)
{
	stroll_dprheap_assert_heap_api(heap);
	stroll_dprheap_assert_api(heap->cnt);

	stroll_dprheap_base_demote(&heap->base, node, heap->compare, data);
}

extern void
stroll_dprheap_setup(struct stroll_dprheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_dprheap_cmp_fn *            compare)
	__stroll_nonull(1, 3) __stroll_nothrow __leaf;

extern struct stroll_dprheap *
stroll_dprheap_create(unsigned int nr, stroll_dprheap_cmp_fn * compare)
	__stroll_nonull(2) __stroll_nothrow __leaf __warn_result;

static inline __stroll_nothrow
void
stroll_dprheap_destroy(struct stroll_dprheap * heap)
{
	free(heap);
}

#endif /* _STROLL_DPRHEAP_H */
