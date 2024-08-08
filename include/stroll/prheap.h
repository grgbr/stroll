/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Pairing heap interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      15 Nov 2017
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_PRHEAP_H
#define _STROLL_PRHEAP_H

#include <stroll/priv/lcrs.h>
#include <stdlib.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_prheap_assert_api(_expr) \
	stroll_assert("stroll:prheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_prheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#define _STROLL_PRHEAP_INIT (NULL)

static inline __stroll_pure __stroll_nothrow
bool
_stroll_prheap_isempty(struct stroll_lcrs_node * const * __restrict root)
{
	stroll_prheap_assert_api(root);

	return !*root;
}

static inline __stroll_pure __stroll_nothrow
struct stroll_lcrs_node *
_stroll_prheap_peek(struct stroll_lcrs_node * const * __restrict root)
{
	stroll_prheap_assert_api(root);
	stroll_prheap_assert_api(*root);

STROLL_IGNORE_WARN("-Wcast-qual")
	return *(struct stroll_lcrs_node **)root;
STROLL_RESTORE_WARN
}

extern void
_stroll_prheap_insert(struct stroll_lcrs_node ** __restrict root,
                      struct stroll_lcrs_node * __restrict  node,
                      stroll_lcrs_cmp_fn   *                compare,
                      void *                                data)
	__stroll_nonull(1, 2, 3);

extern void
_stroll_prheap_merge(struct stroll_lcrs_node ** __restrict        result,
                     struct stroll_lcrs_node * const * __restrict source,
                     stroll_lcrs_cmp_fn *                         compare,
                     void *                                       data)
	__stroll_nonull(1, 3);

extern struct stroll_lcrs_node *
_stroll_prheap_extract(struct stroll_lcrs_node ** __restrict root,
                       stroll_lcrs_cmp_fn *                  compare,
                       void *                                data)
	__stroll_nonull(1, 2);

extern void
_stroll_prheap_remove(struct stroll_lcrs_node ** __restrict root,
                      struct stroll_lcrs_node * __restrict  node,
                      stroll_lcrs_cmp_fn *                  compare,
                      void *                                data)
	__stroll_nonull(1, 2, 3);

extern void
_stroll_prheap_promote(struct stroll_lcrs_node ** __restrict root,
                       struct stroll_lcrs_node * __restrict  node,
                       stroll_lcrs_cmp_fn *                  compare,
                       void *                                data)
	__stroll_nonull(1, 2, 3);

extern void
_stroll_prheap_demote(struct stroll_lcrs_node ** __restrict root,
                      struct stroll_lcrs_node * __restrict  node,
                      stroll_lcrs_cmp_fn *                  compare,
                      void *                                data)
	__stroll_nonull(1, 2, 3);

static inline __stroll_nonull(1) __stroll_nothrow
void
_stroll_prheap_setup(struct stroll_lcrs_node ** __restrict root)
{
	stroll_prheap_assert_api(root);

	*root = NULL;
}

struct stroll_prheap {
	unsigned int              cnt;
	unsigned int              nr;
	struct stroll_lcrs_node * root;
	stroll_lcrs_cmp_fn *      compare;
};

#define stroll_prheap_assert_heap_api(_heap) \
	stroll_prheap_assert_api(_heap); \
	stroll_prheap_assert_api((_heap)->nr); \
	stroll_prheap_assert_api((_heap)->cnt <= (_heap)->nr); \
	stroll_prheap_assert_api(!(!!(_heap)->cnt ^ !!(_heap)->root)); \
	stroll_prheap_assert_api((_heap)->compare)

#define STROLL_PRHEAP_INIT(_nr, _compare) \
	{ \
		.cnt     = 0, \
		.nr      = _nr, \
		.root    = NULL, \
		.compare = _compare \
	}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_prheap_count(const struct stroll_prheap * __restrict heap)
{
	stroll_prheap_assert_heap_api(heap);

	return heap->cnt;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
unsigned int
stroll_prheap_nr(const struct stroll_prheap * __restrict heap)
{
	stroll_prheap_assert_heap_api(heap);

	return heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_prheap_isempty(const struct stroll_prheap * __restrict heap)
{
	stroll_prheap_assert_heap_api(heap);

	return _stroll_prheap_isempty(&heap->root);
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_prheap_isfull(const struct stroll_prheap * __restrict heap)
{
	stroll_prheap_assert_heap_api(heap);

	return heap->cnt == heap->nr;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_lcrs_node *
stroll_prheap_peek(const struct stroll_prheap * __restrict heap)
{
	stroll_prheap_assert_heap_api(heap);

	return _stroll_prheap_peek(&heap->root);
}

extern void
stroll_prheap_insert(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict node,
                     void *                               data)
	__stroll_nonull(1, 2);

extern void
stroll_prheap_merge(struct stroll_prheap * __restrict       result,
                    const struct stroll_prheap * __restrict source,
                    void *                                  data)
	__stroll_nonull(1, 2);

extern struct stroll_lcrs_node *
stroll_prheap_extract(struct stroll_prheap * __restrict heap, void * data)
	__stroll_nonull(1);

extern void
stroll_prheap_remove(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict node,
                     void *                               data)
	__stroll_nonull(1, 2);

static inline __stroll_nonull(1, 2)
void
stroll_prheap_promote(struct stroll_prheap * __restrict    heap,
                      struct stroll_lcrs_node * __restrict node,
                      void *                               data)
{
	stroll_prheap_assert_heap_api(heap);

	_stroll_prheap_promote(&heap->root, node, heap->compare, data);
}

static inline __stroll_nonull(1, 2, 3)
void
stroll_prheap_demote(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict node,
                     void *                               data)
{
	stroll_prheap_assert_heap_api(heap);

	_stroll_prheap_demote(&heap->root, node, heap->compare, data);
}

extern void
stroll_prheap_setup(struct stroll_prheap * __restrict heap,
                    unsigned int                      nr,
                    stroll_lcrs_cmp_fn *              compare)
	__stroll_nonull(1, 3) __stroll_nothrow __leaf;

extern struct stroll_prheap *
stroll_prheap_create(unsigned int nr, stroll_lcrs_cmp_fn * compare)
	__stroll_nonull(2) __stroll_nothrow __leaf __warn_result;

static inline __stroll_nothrow
void
stroll_prheap_destroy(struct stroll_prheap * heap)
{
	free(heap);
}

#endif /* _STROLL_PRHEAP_H */
