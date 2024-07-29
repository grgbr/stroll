/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Pairing heap interface (half-ordered binary tree with parent based)
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      08 Sep 2024
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_PPRHEAP_H
#define _STROLL_PPRHEAP_H

#include <stroll/cdefs.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_pprheap_assert_api(_expr) \
	stroll_assert("stroll:pprheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_pprheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

struct stroll_pprheap_node;

typedef int (stroll_pprheap_cmp_fn)
            (const struct stroll_pprheap_node * __restrict,
             const struct stroll_pprheap_node * __restrict,
             void *)
	__stroll_nonull(1, 2) __warn_result;

struct stroll_pprheap_base {
	struct stroll_pprheap_node * root;
};

#define STROLL_PPRHEAP_BASE_INIT(_heap) \
	{ .root = NULL }

struct stroll_pprheap_node {
	struct stroll_pprheap_node * children[2];
	struct stroll_pprheap_node * parent;
};

#define stroll_pprheap_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

#if defined(CONFIG_STROLL_UTEST) || defined(CONFIG_STROLL_PTEST)

extern const struct stroll_pprheap_node * const stroll_pprheap_tail;

#endif /* defined(CONFIG_STROLL_UTEST) || defined(CONFIG_STROLL_PTEST) */

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_pprheap_base_isempty(const struct stroll_pprheap_base * __restrict heap)
{
	stroll_pprheap_assert_api(heap);

	return !heap->root;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_pprheap_node *
stroll_pprheap_base_peek(const struct stroll_pprheap_base * __restrict heap)
{
	stroll_pprheap_assert_api(heap);
	stroll_pprheap_assert_api(heap->root);

	return heap->root;
}

extern void
stroll_pprheap_base_insert(struct stroll_pprheap_base * __restrict heap,
                           struct stroll_pprheap_node * __restrict node,
                           stroll_pprheap_cmp_fn   *               compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_pprheap_base_merge(struct stroll_pprheap_base * __restrict       result,
                          const struct stroll_pprheap_base * __restrict source,
                          stroll_pprheap_cmp_fn *                       compare,
                          void *                                        data)
	__stroll_nonull(1, 3);

extern struct stroll_pprheap_node *
stroll_pprheap_base_extract(struct stroll_pprheap_base * __restrict heap,
                            stroll_pprheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2);

extern void
stroll_pprheap_base_remove(struct stroll_pprheap_base * __restrict heap,
                           struct stroll_pprheap_node * __restrict node,
                           stroll_pprheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_pprheap_base_promote(struct stroll_pprheap_base * __restrict heap,
                            struct stroll_pprheap_node * __restrict node,
                            stroll_pprheap_cmp_fn *                 compare,
                            void *                                  data)
	__stroll_nonull(1, 2, 3);

extern void
stroll_pprheap_base_demote(struct stroll_pprheap_base * __restrict heap,
                           struct stroll_pprheap_node * __restrict node,
                           stroll_pprheap_cmp_fn *                 compare,
                           void *                                  data)
	__stroll_nonull(1, 2, 3);

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_pprheap_base_setup(struct stroll_pprheap_base * __restrict heap)
{
	stroll_pprheap_assert_api(heap);

	heap->root = NULL;
}

struct stroll_pprheap {
	unsigned int               cnt;
	unsigned int               nr;
	struct stroll_pprheap_base base;
	stroll_pprheap_cmp_fn *    compare;
};

#define stroll_pprheap_assert_heap_api(_heap) \
	stroll_pprheap_assert_api(_heap); \
	stroll_pprheap_assert_api((_heap)->nr); \
	stroll_pprheap_assert_api((_heap)->cnt <= (_heap)->nr); \
	stroll_pprheap_assert_api(!!(_heap)->cnt ^ \
	                         stroll_pprheap_base_isempty(&(_heap)->base)); \
	stroll_pprheap_assert_api((_heap)->compare)

#define STROLL_PPRHEAP_INIT(_nr, _compare) \
	{ \
		.cnt     = 0, \
		.nr      = _nr, \
		.base    = STROLL_PPRHEAP_BASE_INIT((_heap).base), \
		.compare = _compare \
	}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_pprheap_count(const struct stroll_pprheap * __restrict heap)
{
	stroll_pprheap_assert_heap_api(heap);

	return heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_pprheap_nr(const struct stroll_pprheap * __restrict heap)
{
	stroll_pprheap_assert_heap_api(heap);

	return heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_pprheap_isempty(const struct stroll_pprheap * __restrict heap)
{
	stroll_pprheap_assert_heap_api(heap);

	return stroll_pprheap_base_isempty(&heap->base);
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_pprheap_isfull(const struct stroll_pprheap * __restrict heap)
{
	stroll_pprheap_assert_heap_api(heap);

	return heap->cnt == heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_pprheap_node *
stroll_pprheap_peek(const struct stroll_pprheap * __restrict heap)
{
	stroll_pprheap_assert_heap_api(heap);

	return stroll_pprheap_base_peek(&heap->base);
}

extern void
stroll_pprheap_insert(struct stroll_pprheap * __restrict      heap,
                      struct stroll_pprheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

extern void
stroll_pprheap_merge(struct stroll_pprheap * __restrict       result,
                     const struct stroll_pprheap * __restrict source,
                     void *                                   data)
	__stroll_nonull(1, 2);

extern struct stroll_pprheap_node *
stroll_pprheap_extract(struct stroll_pprheap * __restrict heap, void * data)
	__stroll_nonull(1);

extern void
stroll_pprheap_remove(struct stroll_pprheap * __restrict      heap,
                      struct stroll_pprheap_node * __restrict node,
                      void *                                  data)
	__stroll_nonull(1, 2);

static inline __stroll_nonull(1, 2)
void
stroll_pprheap_promote(struct stroll_pprheap * __restrict      heap,
                       struct stroll_pprheap_node * __restrict node,
                       void *                                  data)
{
	stroll_pprheap_assert_heap_api(heap);
	stroll_pprheap_assert_api(heap->cnt);

	stroll_pprheap_base_promote(&heap->base, node, heap->compare, data);
}

static inline __stroll_nonull(1, 2)
void
stroll_pprheap_demote(struct stroll_pprheap * __restrict      heap,
                      struct stroll_pprheap_node * __restrict node,
                      void *                                  data)
{
	stroll_pprheap_assert_heap_api(heap);
	stroll_pprheap_assert_api(heap->cnt);

	stroll_pprheap_base_demote(&heap->base, node, heap->compare, data);
}

extern void
stroll_pprheap_setup(struct stroll_pprheap * __restrict heap,
                     unsigned int                       nr,
                     stroll_pprheap_cmp_fn *            compare)
	__stroll_nonull(1, 3) __stroll_nothrow __leaf;

extern struct stroll_pprheap *
stroll_pprheap_create(unsigned int nr, stroll_pprheap_cmp_fn * compare)
	__stroll_nonull(2) __stroll_nothrow __leaf __warn_result;

static inline __stroll_nothrow
void
stroll_pprheap_destroy(struct stroll_pprheap * heap)
{
	free(heap);
}

#endif /* _STROLL_PPRHEAP_H */
