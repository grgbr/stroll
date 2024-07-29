/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Rank pairing heap interface (doubly linked half-ordered trees based)
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      09 Sep 2024
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_DRPHEAP_H
#define _STROLL_DRPHEAP_H

#include <stroll/cdefs.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_drpheap_assert_api(_expr) \
	stroll_assert("stroll:drpheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_drpheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

struct stroll_drpheap_node;

typedef int (stroll_drpheap_cmp_fn)
            (const struct stroll_drpheap_node * __restrict,
             const struct stroll_drpheap_node * __restrict,
             void *)
	__stroll_nonull(1, 2) __warn_result;

struct stroll_drpheap_link {
	struct stroll_drpheap_link * edges[2];
};

struct stroll_drpheap_base {
	struct stroll_drpheap_link roots;
};

#define stroll_drpheap_encode_root_link(_link) \
	((struct stroll_drpheap_link *)((unsigned long)(_link) | 1UL))

#define STROLL_DRPHEAP_BASE_INIT(_heap) \
	{ \
		.roots.edges = { \
			[0] = &(_heap).roots, \
			[1] = stroll_drpheap_encode_root_link(&(_heap).roots) \
		} \
	}

struct stroll_drpheap_node {
	int                          rank;
	struct stroll_drpheap_link * child;
	struct stroll_drpheap_link   link;
};

#define stroll_drpheap_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

#if defined(CONFIG_STROLL_UTEST) || defined(CONFIG_STROLL_PTEST)

extern const struct stroll_drpheap_link * const stroll_drpheap_tail;

#endif /* defined(CONFIG_STROLL_UTEST) || defined(CONFIG_STROLL_PTEST) */

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_drpheap_base_isempty(const struct stroll_drpheap_base * __restrict heap)
{
	stroll_drpheap_assert_api(heap);

	return heap->roots.edges[0] == &heap->roots;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_drpheap_node *
stroll_drpheap_node_from_link(
	const struct stroll_drpheap_link * __restrict link)
{
	return containerof(link, struct stroll_drpheap_node, link);
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_drpheap_node *
stroll_drpheap_base_peek(const struct stroll_drpheap_base * __restrict heap)
{
	stroll_drpheap_assert_api(heap);
	stroll_drpheap_assert_api(heap->roots.edges[0]);
	stroll_drpheap_assert_api(heap->roots.edges[0] != &heap->roots);

	return stroll_drpheap_node_from_link(heap->roots.edges[0]);
}

extern void
stroll_drpheap_base_insert(struct stroll_drpheap_base * __restrict heap,
                           struct stroll_drpheap_node * __restrict node,
                           stroll_drpheap_cmp_fn   *               compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_drpheap_base_merge(struct stroll_drpheap_base * __restrict       result,
                          const struct stroll_drpheap_base * __restrict source,
                          stroll_drpheap_cmp_fn *                       compare,
                          void *                                        data)
	__stroll_nonull(1, 3);

extern struct stroll_drpheap_node *
stroll_drpheap_base_extract(struct stroll_drpheap_base * __restrict heap,
                            stroll_drpheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2);

extern void
stroll_drpheap_base_remove(struct stroll_drpheap_base * __restrict heap,
                           struct stroll_drpheap_node * __restrict node,
                           stroll_drpheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_drpheap_base_promote(struct stroll_drpheap_base * __restrict heap,
                            struct stroll_drpheap_node * __restrict node,
                            stroll_drpheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_drpheap_base_demote(struct stroll_drpheap_base * __restrict heap,
                           struct stroll_drpheap_node * __restrict node,
                           stroll_drpheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_drpheap_base_setup(struct stroll_drpheap_base * __restrict heap)
{
	stroll_drpheap_assert_api(heap);

	heap->roots.edges[0] = &heap->roots;
	heap->roots.edges[1] = stroll_drpheap_encode_root_link(&heap->roots);
}

struct stroll_drpheap {
	unsigned int               cnt;
	unsigned int               nr;
	struct stroll_drpheap_base base;
	stroll_drpheap_cmp_fn *    compare;
};

#define stroll_drpheap_assert_heap_api(_heap) \
	stroll_drpheap_assert_api(_heap); \
	stroll_drpheap_assert_api((_heap)->nr); \
	stroll_drpheap_assert_api((_heap)->cnt <= (_heap)->nr); \
	stroll_drpheap_assert_api(!!(_heap)->cnt ^ \
	                          stroll_drpheap_base_isempty(&(_heap)->base)); \
	stroll_drpheap_assert_api((_heap)->compare)

#define STROLL_DRPHEAP_INIT(_nr, _compare) \
	{ \
		.cnt     = 0, \
		.nr      = _nr, \
		.base    = STROLL_DRPHEAP_BASE_INIT((_heap).base), \
		.compare = _compare \
	}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_drpheap_count(const struct stroll_drpheap * __restrict heap)
{
	stroll_drpheap_assert_heap_api(heap);

	return heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_drpheap_nr(const struct stroll_drpheap * __restrict heap)
{
	stroll_drpheap_assert_heap_api(heap);

	return heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_drpheap_isempty(const struct stroll_drpheap * __restrict heap)
{
	stroll_drpheap_assert_heap_api(heap);

	return stroll_drpheap_base_isempty(&heap->base);
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_drpheap_isfull(const struct stroll_drpheap * __restrict heap)
{
	stroll_drpheap_assert_heap_api(heap);

	return heap->cnt == heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_drpheap_node *
stroll_drpheap_peek(const struct stroll_drpheap * __restrict heap)
{
	stroll_drpheap_assert_heap_api(heap);

	return stroll_drpheap_base_peek(&heap->base);
}

extern void
stroll_drpheap_insert(struct stroll_drpheap * __restrict      heap,
                      struct stroll_drpheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

extern void
stroll_drpheap_merge(struct stroll_drpheap * __restrict       result,
                     const struct stroll_drpheap * __restrict source,
                     void *                                   data)
	__stroll_nonull(1, 2);

extern struct stroll_drpheap_node *
stroll_drpheap_extract(struct stroll_drpheap * __restrict heap, void * data)
	__stroll_nonull(1);

extern void
stroll_drpheap_remove(struct stroll_drpheap * __restrict      heap,
                      struct stroll_drpheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

static inline __stroll_nonull(1, 2)
void
stroll_drpheap_promote(struct stroll_drpheap * __restrict      heap,
                       struct stroll_drpheap_node * __restrict node,
                       void *                                  data)
{
	stroll_drpheap_assert_heap_api(heap);
	stroll_drpheap_assert_api(heap->cnt);

	stroll_drpheap_base_promote(&heap->base, node, heap->compare, data);
}

static inline __stroll_nonull(1, 2)
void
stroll_drpheap_demote(struct stroll_drpheap * __restrict      heap,
                      struct stroll_drpheap_node * __restrict node,
                      void *                                  data)
{
	stroll_drpheap_assert_heap_api(heap);
	stroll_drpheap_assert_api(heap->cnt);

	stroll_drpheap_base_demote(&heap->base, node, heap->compare, data);
}

extern void
stroll_drpheap_setup(struct stroll_drpheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_drpheap_cmp_fn *            compare)
	__stroll_nonull(1, 3) __stroll_nothrow __leaf;

extern struct stroll_drpheap *
stroll_drpheap_create(unsigned int nr, stroll_drpheap_cmp_fn * compare)
	__stroll_nonull(2) __stroll_nothrow __leaf __warn_result;

static inline __stroll_nothrow
void
stroll_drpheap_destroy(struct stroll_drpheap * heap)
{
	free(heap);
}

#endif /* _STROLL_DRPHEAP_H */
