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

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_prheap_assert_api(_expr) \
	stroll_assert("stroll:prheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_prheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#define STROLL_PRHEAP_NODE_INIT(_node) \
	STROLL_LCRS_INIT(_node)

#define stroll_prheap_entry(_node, _type, _member) \
	stroll_lcrs_entry(_node, _type, _member)

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_prheap_init_node(struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_init(node);
}

struct stroll_prheap {
	struct stroll_lcrs_node * root;
};

#define STROLL_PRHEAP_INIT \
	{ .root = NULL }

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_prheap_empty(const struct stroll_prheap * __restrict heap)
{
	stroll_prheap_assert_api(heap);

	return !heap->root;
}

static inline
struct stroll_lcrs_node *
stroll_prheap_peek(const struct stroll_prheap * __restrict heap)
{
	stroll_prheap_assert_api(heap);

	return heap->root;
}

extern void
stroll_prheap_insert(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict node,
                     stroll_lcrs_cmp_fn   *               compare,
                     void *                               data);

extern struct stroll_lcrs_node *
stroll_prheap_extract(struct stroll_prheap * __restrict heap,
                      stroll_lcrs_cmp_fn *              compare,
                      void *                            data);

extern void
stroll_prheap_remove(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict node,
                     stroll_lcrs_cmp_fn *                 compare,
                     void *                               data);

extern void
stroll_prheap_merge(struct stroll_prheap * __restrict result,
                    struct stroll_prheap * __restrict source,
                    stroll_lcrs_cmp_fn *              compare,
                    void *                            data);

extern void
stroll_prheap_promote(struct stroll_prheap * __restrict    heap,
                      struct stroll_lcrs_node * __restrict key,
                      stroll_lcrs_cmp_fn *                 compare,
                      void *                               data);

extern void
stroll_prheap_demote(struct stroll_prheap * __restrict    heap,
                     struct stroll_lcrs_node * __restrict key,
                     stroll_lcrs_cmp_fn *                 compare,
                     void *                               data);

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_prheap_init(struct stroll_prheap * __restrict heap)
{
	stroll_prheap_assert_api(heap);

	heap->root = NULL;
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_prheap_fini(struct stroll_prheap * __restrict heap __unused)
{
	stroll_prheap_assert_api(heap);
}

#endif /* _STROLL_PRHEAP_H */
