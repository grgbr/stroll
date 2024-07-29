/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/priv/lcrs.h"

static void
stroll_lcrs_substitute_siblings(
	const struct stroll_lcrs_node * __restrict node,
	struct stroll_lcrs_node * __restrict       start,
	struct stroll_lcrs_node * __restrict       substitute)
{
	stroll_lcrs_assert_intern(node);
	stroll_lcrs_assert_intern(start);
	stroll_lcrs_assert_intern(substitute);

	stroll_lcrs_previous(node, start)->sibling = substitute;
}

static void
stroll_lcrs_swap_ref(struct stroll_lcrs_node ** __restrict first,
                     struct stroll_lcrs_node ** __restrict second)
{
	stroll_lcrs_assert_intern(first);
	stroll_lcrs_assert_intern(second);
	stroll_lcrs_assert_intern(first != second);

	struct stroll_lcrs_node * tmp = *first;

	*first = *second;
	*second = tmp;
}

struct stroll_lcrs_node *
stroll_lcrs_swap_down(struct stroll_lcrs_node * __restrict node,
                      struct stroll_lcrs_node * __restrict child)
{
	stroll_lcrs_assert_intern(stroll_lcrs_has_child(node));
	stroll_lcrs_assert_intern(stroll_lcrs_parent(child) == node);
	stroll_lcrs_assert_intern(node != child);

	struct stroll_lcrs_node *tmp = node->youngest;

	if (stroll_lcrs_has_child(child)) {
		/*
		 * "child" has children: update their parent pointer to make it
		 * point to "node", the future child.
		 */
		stroll_lcrs_assign_parent(child->youngest, node);
		node->youngest = child->youngest;
	}
	else
		node->youngest = stroll_lcrs_mktail(node);

	/* make "node" a child of "child". */
	if (child != tmp) {
		/*
		 * "child" is not the youngest amongst its siblings: update its
		 * preceding sibling pointer.
		 */
		stroll_lcrs_substitute_siblings(child, tmp, node);
		child->youngest = tmp;
	}
	else
		child->youngest = node;

	/* Update "child" siblings parent pointer to "child". */
	stroll_lcrs_assign_parent(child, child);

	/*
	 * keep a reference to "node"'s parent since altering sibling field
	 * below will prevent us from finding "node"'s parent.
	 */
	tmp = stroll_lcrs_parent(node);

	/* swap "node" and "child" next sibling pointer. */
	stroll_lcrs_swap_ref(&child->sibling, &node->sibling);

	if (!tmp)
		return NULL;

	/* "node" has a parent: make "child" a child of it. */
	if (node != tmp->youngest)
		/*
		 * "node" is not the youngest amongst its siblings: update its
		 * preceding sibling pointer.
		 */
		stroll_lcrs_substitute_siblings(node, tmp->youngest, child);
	else
		tmp->youngest = child;

	return tmp;
}
