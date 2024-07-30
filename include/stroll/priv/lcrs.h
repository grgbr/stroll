/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Left child right sibling interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      22 Nov 2017
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_LCRS_H
#define _STROLL_LCRS_H

#include <stroll/cdefs.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_lcrs_assert_intern(_expr) \
	stroll_assert("stroll:lcrs", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_lcrs_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

struct stroll_lcrs_node {
	struct stroll_lcrs_node * sibling;
	struct stroll_lcrs_node * youngest;
} __align(sizeof(uintptr_t));

#define stroll_lcrs_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

typedef int (stroll_lcrs_cmp_fn)(const struct stroll_lcrs_node * __restrict,
                                 const struct stroll_lcrs_node * __restrict,
                                 void *);

#define STROLL_LCRS_TAIL ((uintptr_t)1U)

#define STROLL_LCRS_INIT(_node) \
	{ \
		.sibling  = (struct stroll_lcrs_node *)(STROLL_LCRS_TAIL), \
		.youngest = (struct stroll_lcrs_node *)((uintptr_t)&(_node) | \
		                                        STROLL_LCRS_TAIL) \
	}

static inline
bool
stroll_lcrs_istail(const struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(node);

	return ((uintptr_t)node & STROLL_LCRS_TAIL);
}

static inline
struct stroll_lcrs_node *
stroll_lcrs_mktail(const struct stroll_lcrs_node * __restrict node)
{
	return (struct stroll_lcrs_node *)((uintptr_t)node | STROLL_LCRS_TAIL);
}

static inline
struct stroll_lcrs_node *
stroll_lcrs_untail(struct stroll_lcrs_node * __restrict node)
{
	return (struct stroll_lcrs_node *)((uintptr_t)node & ~STROLL_LCRS_TAIL);
}

static inline
struct stroll_lcrs_node *
stroll_lcrs_next(const struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	return node->sibling;
}

static inline
struct stroll_lcrs_node **
stroll_lcrs_next_ref(struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	return &node->sibling;
}

static inline
void
stroll_lcrs_assign_next(struct stroll_lcrs_node * __restrict node,
                        struct stroll_lcrs_node * __restrict sibling)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	node->sibling = (struct stroll_lcrs_node *)sibling;
}

static inline
struct stroll_lcrs_node *
stroll_lcrs_previous(const struct stroll_lcrs_node * __restrict node,
                     const struct stroll_lcrs_node * __restrict start)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(start));
	stroll_lcrs_assert_intern(start != node);

	while (start->sibling != node) {
		stroll_lcrs_assert_intern(!stroll_lcrs_istail(start->sibling));
		start = start->sibling;
	}

STROLL_IGNORE_WARN("-Wcast-qual")
	return (struct stroll_lcrs_node *)start;
STROLL_RESTORE_WARN
}

static inline
struct stroll_lcrs_node **
stroll_lcrs_previous_ref(const struct stroll_lcrs_node  * __restrict node,
                         struct stroll_lcrs_node ** __restrict       start)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(*start));

	while (*start != node)
		start = stroll_lcrs_next_ref(*start);

	return (struct stroll_lcrs_node **)start;
}

static inline
bool
stroll_lcrs_has_child(const struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	return node->youngest != stroll_lcrs_mktail(node);
}

static inline
struct stroll_lcrs_node *
stroll_lcrs_youngest(const struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	return node->youngest;
}

static inline struct stroll_lcrs_node **
stroll_lcrs_youngest_ref(struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	return &node->youngest;
}

static inline
struct stroll_lcrs_node *
stroll_lcrs_eldest(const struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	while (!stroll_lcrs_istail(node->sibling))
		node = node->sibling;

STROLL_IGNORE_WARN("-Wcast-qual")
	return (struct stroll_lcrs_node *)node;
STROLL_RESTORE_WARN
}

static inline
bool
stroll_lcrs_has_parent(const struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	return node->sibling != stroll_lcrs_mktail(NULL);
}

static inline
struct stroll_lcrs_node *
stroll_lcrs_parent(const struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	return stroll_lcrs_untail(stroll_lcrs_eldest(node)->sibling);
}

static inline
void
stroll_lcrs_assign_parent(struct stroll_lcrs_node * node,
                          struct stroll_lcrs_node * parent)
{
	stroll_lcrs_eldest(node)->sibling = stroll_lcrs_mktail(parent);
}

static inline
void
stroll_lcrs_assign_youngest(struct stroll_lcrs_node * __restrict node,
                            struct stroll_lcrs_node * __restrict youngest)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(node));

	node->youngest = youngest;
}

static inline
void
stroll_lcrs_join(struct stroll_lcrs_node * __restrict tree,
                 struct stroll_lcrs_node * __restrict parent)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(tree));
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(parent));

	tree->sibling = parent->youngest;
	stroll_lcrs_assign_youngest(parent, tree);
}

static inline
void
stroll_lcrs_split(const struct stroll_lcrs_node * __restrict tree,
                  struct stroll_lcrs_node ** __restrict      previous)
{
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(tree));
	stroll_lcrs_assert_intern(previous);
	stroll_lcrs_assert_intern(!stroll_lcrs_istail(*previous));
	stroll_lcrs_assert_intern(stroll_lcrs_parent(tree) ==
	                          stroll_lcrs_parent(*previous));

	while (*previous != tree)
		previous = &(*previous)->sibling;

	*previous = tree->sibling;
}

extern struct stroll_lcrs_node *
stroll_lcrs_swap_down(struct stroll_lcrs_node * __restrict node,
                      struct stroll_lcrs_node * __restrict child);

#define stroll_lcrs_foreach_sibling(_youngest, _sibling) \
	for ((_sibling) = (_youngest); \
	     !stroll_lcrs_istail(_sibling); \
	     (_sibling) = (_sibling)->sibling)

#define stroll_lcrs_foreach_child(_node, _child) \
	lcrs_foreach_sibling((_node)->youngest, _child)

#define stroll_lcrs_foreach_sibling_safe(_youngest, _sibling, _tmp) \
	for ((_sibling) = (_youngest), (_tmp) = (_sibling)->sibling; \
	     !stroll_lcrs_istail(_sibling); \
	     (_sibling) = (_tmp), (_tmp) = (_sibling)->sibling)

#define stroll_lcrs_foreach_child_safe(_node, _child, _tmp) \
	lcrs_foreach_sibling_safe((_node)->youngest, _child, _tmp)

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_lcrs_init(struct stroll_lcrs_node * __restrict node)
{
	stroll_lcrs_assert_intern(node);

	node->sibling = stroll_lcrs_mktail(NULL);
	node->youngest = stroll_lcrs_mktail(node);
}

#endif /* _STROLL_LCRS_H */
