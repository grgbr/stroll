/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Binomial heap interface (doubly linked multi-way trees based)
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      5 Dec 2017
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_DBNHEAP_H
#define _STROLL_DBNHEAP_H

#include <stroll/dlist.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_dbnheap_assert_api(_expr) \
	stroll_assert("stroll:dbnheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_dbnheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

struct stroll_dbnheap_node;

typedef int (stroll_dbnheap_cmp_fn)
            (const struct stroll_dbnheap_node * __restrict,
             const struct stroll_dbnheap_node * __restrict,
             void *)
	__stroll_nonull(1, 2) __warn_result;

struct stroll_dbnheap_base {
	struct stroll_dlist_node roots;
};

#define STROLL_DBNHEAP_BASE_INIT(_heap) \
	{ .roots = STROLL_DLIST_INIT((_heap).roots) }

struct stroll_dbnheap_node {
	unsigned int                 order;
	struct stroll_dlist_node     siblings;
	struct stroll_dbnheap_node * parent;
	struct stroll_dlist_node     children;
};

#define stroll_dbnheap_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_dbnheap_base_isempty(const struct stroll_dbnheap_base * __restrict heap)
{
	stroll_dbnheap_assert_api(heap);

	return stroll_dlist_empty(&heap->roots);
}

extern struct stroll_dbnheap_node *
stroll_dbnheap_base_peek(const struct stroll_dbnheap_base * __restrict heap,
                         stroll_dbnheap_cmp_fn *                       compare,
                         void *                                        data)
	__stroll_nonull(1, 2) __stroll_pure __warn_result;

extern void
stroll_dbnheap_base_insert(struct stroll_dbnheap_base * __restrict heap,
                           struct stroll_dbnheap_node * __restrict node,
                           stroll_dbnheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_dbnheap_base_merge(struct stroll_dbnheap_base * __restrict result,
                          struct stroll_dbnheap_base * __restrict source,
                          stroll_dbnheap_cmp_fn *                 compare,
                          void *                                  data)
	__stroll_nonull(1, 2, 3);

extern struct stroll_dbnheap_node *
stroll_dbnheap_base_extract(struct stroll_dbnheap_base * __restrict heap,
                            stroll_dbnheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2) __warn_result;

extern void
stroll_dbnheap_base_remove(struct stroll_dbnheap_base * __restrict heap,
                           struct stroll_dbnheap_node * __restrict node,
                           stroll_dbnheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_dbnheap_base_promote(struct stroll_dbnheap_base * __restrict heap,
                            struct stroll_dbnheap_node * __restrict node,
                            stroll_dbnheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_dbnheap_base_demote(struct stroll_dbnheap_base * __restrict heap,
                           struct stroll_dbnheap_node * __restrict node,
                           stroll_dbnheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dbnheap_base_setup(struct stroll_dbnheap_base * __restrict heap)
{
	stroll_dbnheap_assert_api(heap);

	stroll_dlist_init(&heap->roots);
}

struct stroll_dbnheap {
	unsigned int               cnt;
	unsigned int               nr;
	struct stroll_dbnheap_base base;
	stroll_dbnheap_cmp_fn *    compare;
};

#define stroll_dbnheap_assert_heap_api(_heap) \
	stroll_dbnheap_assert_api(_heap); \
	stroll_dbnheap_assert_api((_heap)->nr); \
	stroll_dbnheap_assert_api((_heap)->cnt <= (_heap)->nr); \
	stroll_dbnheap_assert_api(!!(_heap)->cnt ^ \
	                          stroll_dbnheap_base_isempty(&(_heap)->base)); \
	stroll_dbnheap_assert_api((_heap)->compare)

#define STROLL_DBNHEAP_INIT(_heap, _nr, _compare) \
	{ \
		.cnt     = 0, \
		.nr      = _nr, \
		.base    = STROLL_DBNHEAP_BASE_INIT((_heap).base), \
		.compare = _compare \
	}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_dbnheap_count(const struct stroll_dbnheap * __restrict heap)
{
	stroll_dbnheap_assert_heap_api(heap);

	return heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_dbnheap_nr(const struct stroll_dbnheap * __restrict heap)
{
	stroll_dbnheap_assert_heap_api(heap);

	return heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_dbnheap_isempty(const struct stroll_dbnheap * __restrict heap)
{
	stroll_dbnheap_assert_heap_api(heap);

	return stroll_dbnheap_base_isempty(&heap->base);
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_dbnheap_isfull(const struct stroll_dbnheap * __restrict heap)
{
	stroll_dbnheap_assert_heap_api(heap);

	return heap->cnt == heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_dbnheap_node *
stroll_dbnheap_peek(const struct stroll_dbnheap * __restrict heap,
                    void *                                   data)
{
	stroll_dbnheap_assert_heap_api(heap);

	return stroll_dbnheap_base_peek(&heap->base, heap->compare, data);
}

extern void
stroll_dbnheap_insert(struct stroll_dbnheap * __restrict      heap,
                      struct stroll_dbnheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

extern void
stroll_dbnheap_merge(struct stroll_dbnheap * __restrict result,
                     struct stroll_dbnheap * __restrict source,
                     void *                             data)
	__stroll_nonull(1, 2);

extern struct stroll_dbnheap_node *
stroll_dbnheap_extract(struct stroll_dbnheap * __restrict heap, void * data)
	__stroll_nonull(1);

extern void
stroll_dbnheap_remove(struct stroll_dbnheap * __restrict      heap,
                      struct stroll_dbnheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

static inline __stroll_nonull(1, 2)
void
stroll_dbnheap_promote(struct stroll_dbnheap * __restrict      heap,
                       struct stroll_dbnheap_node * __restrict node,
                       void *                                  data)
{
	stroll_dbnheap_assert_heap_api(heap);
	stroll_dbnheap_assert_api(heap->cnt);

	stroll_dbnheap_base_promote(&heap->base, node, heap->compare, data);
}

static inline __stroll_nonull(1, 2)
void
stroll_dbnheap_demote(struct stroll_dbnheap * __restrict      heap,
                      struct stroll_dbnheap_node * __restrict node,
                      void *                                  data)
{
	stroll_dbnheap_assert_heap_api(heap);
	stroll_dbnheap_assert_api(heap->cnt);

	stroll_dbnheap_base_demote(&heap->base, node, heap->compare, data);
}

extern void
stroll_dbnheap_setup(struct stroll_dbnheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_dbnheap_cmp_fn *            compare)
	__stroll_nonull(1, 3) __stroll_nothrow __leaf;

extern struct stroll_dbnheap *
stroll_dbnheap_create(unsigned int nr, stroll_dbnheap_cmp_fn * compare)
	__stroll_nonull(2) __stroll_nothrow __leaf __warn_result;

static inline __stroll_nothrow
void
stroll_dbnheap_destroy(struct stroll_dbnheap * heap)
{
	free(heap);
}

#endif /* _STROLL_DBNHEAP_H */
