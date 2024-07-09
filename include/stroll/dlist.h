/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Doubly linked list interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      29 Jun 2017
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_DLIST_H
#define _STROLL_DLIST_H

#include <stroll/cdefs.h>
#include <stdbool.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_dlist_assert_api(_expr) \
	stroll_assert("stroll:dlist", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_dlist_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Doubly linked list node
 *
 * Describes a single entry linked into a stroll_dlist.
 */
struct stroll_dlist_node {
	/**
	 * @internal
	 *
	 * Node following this node into a dlist.
	 */
	struct stroll_dlist_node * next;
	/**
	 * @internal
	 *
	 * Node preceding this node into a stroll_dlist.
	 */
	struct stroll_dlist_node * prev;
};

/**
 * stroll_dlist_node constant initializer.
 *
 * @param _node stroll_dlist_node variable to initialize.
 */
#define STROLL_DLIST_INIT(_node) \
	{ \
		.next = &(_node), \
		.prev = &(_node) \
	}

/**
 * Initialize a stroll_dlist_node
 *
 * @param[out] node stroll_dlist_node to initialize.
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dlist_init(struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(node);

	node->next = node;
	node->prev = node;
}

/**
 * Test wether a stroll_dlist_node is empty or not.
 *
 * @param node stroll_dlist_node to test.
 *
 * @retval true  empty
 * @retval false not empty
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_dlist_empty(const struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(node);

	return node->next == node;
}

/**
 * Get node following specified node.
 *
 * @param node Node which to get the follower from.
 *
 * @return Pointer to following node.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_dlist_node *
stroll_dlist_next(const struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(node);

	return node->next;
}

/**
 * Get node preceding specified node.
 *
 * @param node Node which to get the predecessor from.
 *
 * @return Pointer to preceding node.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_dlist_node *
stroll_dlist_prev(const struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(node);

	return node->prev;
}

/**
 * Insert a stroll_dlist_node in between specified adjacent nodes.
 *
 * @param prev Node to append @p node after.
 * @param node Node to inject.
 * @param next Node to insert @p node before.
 */
static inline __stroll_nonull(1, 2, 3) __stroll_nothrow
void
stroll_dlist_inject(struct stroll_dlist_node * __restrict prev,
                    struct stroll_dlist_node * __restrict node,
                    struct stroll_dlist_node * __restrict next)
{
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(prev);
	stroll_dlist_assert_api(next);
	stroll_dlist_assert_api(node != prev);
	stroll_dlist_assert_api(node != next);

	prev->next = node;

	node->next = next;
	node->prev = prev;

	next->prev = node;
}

/**
 * Insert a stroll_dlist_node before specified node.
 *
 * @param node Node to insert.
 * @param at   Node to insert @p node before.
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_insert(struct stroll_dlist_node * __restrict at,
                    struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(node);

	stroll_dlist_inject(at->prev, node, at);
}

/**
 * Append a stroll_dlist_node after specified node.
 *
 * @param node Node to append.
 * @param at   Node to append @p node after.
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void 
stroll_dlist_append(struct stroll_dlist_node * __restrict at,
                    struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(node);

	stroll_dlist_inject(at, node, at->next);
}

/**
 * Enqueue a stroll_dlist_node at the head of specified list.
 *
 * @param list Dummy head node designating the list.
 * @param node Node to enqueue.
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_nqueue_front(struct stroll_dlist_node * __restrict list,
                          struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_append(list, node);
}

/**
 * Enqueue a stroll_dlist_node at the tail of specified list.
 *
 * @param list Dummy head node designating the list.
 * @param node Node to enqueue.
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_nqueue_back(struct stroll_dlist_node * __restrict list,
                         struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_insert(list, node);
}

/**
 * Remove a stroll_dlist_node from a doubly linked list.
 *
 * @param node Node to remove.
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dlist_remove(const struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(node);

	struct stroll_dlist_node * next = node->next;
	struct stroll_dlist_node * prev = node->prev;

	prev->next = next;
	next->prev = prev;
}

/**
 * Remove then reinitialize a stroll_dlist_node from a doubly linked list.
 *
 * @param node Node to remove.
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dlist_remove_init(struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_remove(node);
	stroll_dlist_init(node);
}

/**
 * Dequeue a stroll_dlist_node from the head of specified list.
 *
 * @param list Dummy head node designating the list.
 *
 * @return Pointer to dequeued node.
 */
static inline __stroll_nonull(1) __stroll_nothrow
struct stroll_dlist_node *
stroll_dlist_dqueue_front(struct stroll_dlist_node * __restrict list)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(list));

	struct stroll_dlist_node * node = stroll_dlist_next(list);

	stroll_dlist_remove(node);

	return node;
}

/**
 * Dequeue then reinitialize a stroll_dlist_node from the head of specified
 * list.
 *
 * @param list Dummy head node designating the list.
 *
 * @return Pointer to dequeued node.
 */
static inline __stroll_nonull(1) __stroll_nothrow
struct stroll_dlist_node *
stroll_dlist_dqueue_front_init(struct stroll_dlist_node * __restrict list)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(list));

	struct stroll_dlist_node * node = stroll_dlist_dqueue_front(list);

	stroll_dlist_init(node);

	return node;
}

/**
 * Dequeue a stroll_dlist_node from the tail of specified list.
 *
 * @param list Dummy head node designating the list.
 *
 * @return Pointer to dequeued node.
 */
static inline __stroll_nonull(1) __stroll_nothrow
struct stroll_dlist_node *
stroll_dlist_dqueue_back(struct stroll_dlist_node * __restrict list)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(list));

	struct stroll_dlist_node * node = stroll_dlist_prev(list);

	stroll_dlist_remove(node);

	return node;
}

/**
 * Dequeue then reinitialize a stroll_dlist_node from the tail of specified
 * list.
 *
 * @param list Dummy head node designating the list.
 *
 * @return Pointer to dequeued node.
 */
static inline __stroll_nonull(1) __stroll_nothrow
struct stroll_dlist_node *
stroll_dlist_dqueue_back_init(struct stroll_dlist_node * __restrict list)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(list));

	struct stroll_dlist_node * node = stroll_dlist_dqueue_back(list);

	stroll_dlist_init(node);

	return node;
}

/**
 * Replace old entry by new one.
 *
 * @param old  Node to replace.
 * @param node New node to insert.
 *
 * @warning Behavior is unpredictable if @p old node is empty.
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dlist_replace(struct stroll_dlist_node * __restrict old,
                     struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(old));
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(old != node);

	stroll_dlist_inject(old->prev, node, old->next);
}

/**
 * Move entry from one location to another.
 *
 * @param at   Node after which to append @p node.
 * @param node Node to move.
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_move_before(struct stroll_dlist_node * __restrict at,
                         struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(at != node);

	stroll_dlist_remove(node);
	stroll_dlist_inject(at->prev, node, at);
}

/**
 * Move entry from one location to another.
 *
 * @param at   Node after which to append @p node.
 * @param node Node to move.
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_move_after(struct stroll_dlist_node * __restrict at,
                        struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(at != node);

	stroll_dlist_remove(node);
	stroll_dlist_inject(at, node, at->next);
}

/**
 * Extract / remove a portion of nodes from a dlist.
 *
 * @param first First node of portion to remove.
 * @param last  Last node of portion to remove.
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_withdraw(const struct stroll_dlist_node * first,
                      const struct stroll_dlist_node * last)
{
	stroll_dlist_assert_api(first);
	stroll_dlist_assert_api(last);

	struct stroll_dlist_node * prev = first->prev;
	struct stroll_dlist_node * next = last->next;

	prev->next = next;
	next->prev = prev;
}

/**
 * Insert a portion of nodes into a dlist.
 *
 * @param at    Node after which portion is inserted.
 * @param first First node of portion to insert.
 * @param last  Last node of portion to insert.
 */
static inline __stroll_nonull(1, 2, 3) __stroll_nothrow
void
stroll_dlist_embed_after(struct stroll_dlist_node * __restrict at,
                         struct stroll_dlist_node *            first,
                         struct stroll_dlist_node *            last)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(first);
	stroll_dlist_assert_api(last);
	stroll_dlist_assert_api(at != first);
	stroll_dlist_assert_api(at != last);

	struct stroll_dlist_node * next = at->next;

	first->prev = at;
	at->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * Extract source list portion and move it to result list at specified location.
 *
 * @param at    Node to insert nodes after.
 * @param first First node of portion to move.
 * @param last  Last node of portion to move.
 */
extern void
stroll_dlist_splice_after(struct stroll_dlist_node * __restrict at,
                          struct stroll_dlist_node *            first,
                          struct stroll_dlist_node *            last)
	__stroll_nonull(1, 2, 3) __stroll_nothrow;

/**
 * Iterate over a stroll_dlist_node based list .
 *
 * @param _list Dummy head node designating the list to iterate over.
 * @param _node Pointer to current node.
 */
#define stroll_dlist_foreach_node(_list, _node) \
	for (_node = stroll_dlist_next(_list);  \
	     _node != (_list);           \
	     _node = stroll_dlist_next(_node))

/**
 * Return type casted pointer to entry containing specified node.
 *
 * @param _node   stroll_dlist_node to retrieve container from.
 * @param _type   Type of container
 * @param _member Member field of container structure pointing to _node.
 *
 * @return Pointer to type casted entry.
 */
#define stroll_dlist_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

/**
 * Return type casted pointer to entry following specified entry.
 *
 * @param _entry  Entry containing dlist node.
 * @param _member Member field of container structure pointing to dlist node.
 *
 * @return Pointer to following entry.
 */
#define stroll_dlist_next_entry(_entry, _member) \
	stroll_dlist_entry(stroll_dlist_next(&(_entry)->_member), \
	                   typeof(*(_entry)), \
	                   _member)

/**
 * Return type casted pointer to entry preceding specified entry.
 *
 * @param _entry  Entry containing dlist node.
 * @param _member Member field of container structure pointing to dlist node.
 *
 * @return Pointer to preceding entry.
 */
#define stroll_dlist_prev_entry(_entry, _member) \
	dlist_entry(stroll_dlist_prev(&(_entry)->_member), \
	            typeof(*(_entry)), \
	            _member)

/**
 * Iterate over dlist node container entries.
 *
 * @param _list   dlist to iterate over.
 * @param _entry  Pointer to entry containing @p _list's current node.
 * @param _member Member field of container structure pointing to dlist node.
 */
#define stroll_dlist_foreach_entry(_list, _entry, _member) \
	for (_entry = stroll_dlist_entry((_list)->next, \
	                                 typeof(*(_entry)), _member); \
	     &(_entry)->_member != (_list); \
	     _entry = stroll_dlist_next_entry(_entry, _member))

/******************************************************************************
 * Stroll doubly linked list sorting
 ******************************************************************************/

/**
 * stroll_dlist_node item comparison routine prototype.
 *
 * Use this to implement a function that compares 2 stroll_dlist_node nodes
 * given as arguments according to an arbitrary logic.
 *
 * Last argument may point to an optional arbitrary user data used for
 * comparison purposes.
 *
 * The function *MUST* return an integer less than, equal to, or greater than
 * zero if first argument is found, respectively, to be less than, to match, or
 * be greater than the second one.
 */
typedef int (stroll_dlist_cmp_fn)
            (const struct stroll_dlist_node * __restrict,
             const struct stroll_dlist_node * __restrict,
             void *)
	__stroll_nonull(1, 2);

#if defined(CONFIG_STROLL_DLIST_BUBBLE_SORT)

/**
 * Sort specified list according to the bubble sort scheme.
 *
 * @param[inout] head    head of list to sort.
 * @param[in]    compare @p list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 *
 * Sort the @p list using the @p compare comparison function according to
 * the @rstlnk{List bubble sort} algorithm.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p list elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * @note
 * Refer to @rstlnk{Sorting lists} for more informations related to algorithm
 * selection.
 *
 * @warning
 * - Behavior is undefined when called on an empty list.
 * - Implemented for reference only: **DO NOT USE IT**. Refer to
 *   @rstlnk{Sorting lists} for more informations related to algorithm
 *   selection.
 */
extern void
stroll_dlist_bubble_sort(struct stroll_dlist_node * __restrict head,
                         stroll_dlist_cmp_fn *                 compare,
                         void *                                data)
	__stroll_nonull(1, 2);

#endif /* defined(CONFIG_STROLL_SLIST_BUBBLE_SORT) */

#endif /* _STROLL_DLIST_H */
