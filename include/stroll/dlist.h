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

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_dlist_assert_intern(_expr) \
	stroll_assert("stroll:dlist", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_dlist_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Doubly linked list node
 *
 * Describes a single entry linked into a doubly linked list.
 */
struct stroll_dlist_node {
	/**
	 * @internal
	 *
	 * Node following this node into this stroll_dlist_node.
	 */
	struct stroll_dlist_node * next;
	/**
	 * @internal
	 *
	 * Node preceding this node into this stroll_dlist_node.
	 */
	struct stroll_dlist_node * prev;
};

/**
 * stroll_dlist_node constant initializer.
 *
 * @param _node stroll_dlist_node variable to initialize.
 *
 * @see stroll_dlist_init()
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
 *
 * @see STROLL_DLIST_INIT()
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
 * @param[in] node stroll_dlist_node to test.
 *
 * @retval true  empty
 * @retval false not empty
 *
 * @warning
 * Result is undefined if @p node has not been previously initialized.
 *
 * @see stroll_dlist_init()
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
 * @param[in] node Node which to get the follower from.
 *
 * @return Pointer to following node.
 *
 * @warning
 * Result is undefined if @p node has not been previously initialized.
 *
 * @see
 * - stroll_dlist_prev()
 * - stroll_dlist_next_entry()
 * - stroll_dlist_init()
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
 * @param[in] node Node which to get the predecessor from.
 *
 * @return Pointer to preceding node.
 *
 * @warning
 * Result is undefined if @p node has not been previously initialized.
 *
 * @see
 * - stroll_dlist_next()
 * - stroll_dlist_prev_entry()
 * - stroll_dlist_init()
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_dlist_node *
stroll_dlist_prev(const struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(node);

	return node->prev;
}

/**
 * @internal
 *
 * Insert a stroll_dlist_node in between specified adjacent nodes.
 *
 * @param[inout] prev Node to append @p node after.
 * @param[out]   node Node to inject.
 * @param[inout] next Node to insert @p node before.
 *
 * @warning
 * - Result is undefined if @p prev has not been previously initialized.
 * - Result is undefined if @p next has not been previously initialized.
 *
 * @see stroll_dlist_init()
 */
static inline __stroll_nonull(1, 2, 3) __stroll_nothrow
void
stroll_dlist_inject(struct stroll_dlist_node *            prev,
                    struct stroll_dlist_node * __restrict node,
                    struct stroll_dlist_node *            next)
{
	stroll_dlist_assert_intern(node);
	stroll_dlist_assert_intern(prev);
	stroll_dlist_assert_intern(next);
	stroll_dlist_assert_intern(node != prev);
	stroll_dlist_assert_intern(node != next);

	prev->next = node;

	node->next = next;
	node->prev = prev;

	next->prev = node;
}

/**
 * Insert a stroll_dlist_node before specified node.
 *
 * @param[inout] at   Node to insert @p node before.
 * @param[out]   node Node to insert.
 *
 * @warning
 * - Result is undefined if @p at has not been previously initialized.
 *
 * @see
 * - stroll_dlist_append()
 * - stroll_dlist_init()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_insert(struct stroll_dlist_node * __restrict at,
                    struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(node != at);

	stroll_dlist_inject(at->prev, node, at);
}

/**
 * Append a stroll_dlist_node after specified node.
 *
 * @param[inout] at   Node to append @p node after.
 * @param[out]   node Node to append.
 *
 * @warning
 * - Result is undefined if @p at has not been previously initialized.
 *
 * @see
 * - stroll_dlist_insert()
 * - stroll_dlist_init()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void 
stroll_dlist_append(struct stroll_dlist_node * __restrict at,
                    struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(node != at);

	stroll_dlist_inject(at, node, at->next);
}

/**
 * Enqueue a stroll_dlist_node at the head of specified list.
 *
 * @param[inout] list Dummy head node designating the list.
 * @param[out]   node Node to enqueue.
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_dlist_nqueue_back()
 * - stroll_dlist_init()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_nqueue_front(struct stroll_dlist_node * __restrict list,
                          struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(list);
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(node != list);

	stroll_dlist_append(list, node);
}

/**
 * Enqueue a stroll_dlist_node at the tail of specified list.
 *
 * @param[inout] list Dummy head node designating the list.
 * @param[out]   node Node to enqueue.
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_dlist_nqueue_front()
 * - stroll_dlist_init()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_dlist_nqueue_back(struct stroll_dlist_node * __restrict list,
                         struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(list);
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(node != list);

	stroll_dlist_insert(list, node);
}

/**
 * Remove a stroll_dlist_node from a doubly linked list.
 *
 * @param[inout] node Node to remove.
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_dlist_remove_init()
 * - stroll_dlist_init()
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
 * @param[inout] node Node to remove.
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_dlist_remove()
 * - stroll_dlist_init()
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dlist_remove_init(struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(node);

	stroll_dlist_remove(node);
	stroll_dlist_init(node);
}

/**
 * Dequeue a stroll_dlist_node from the head of specified list.
 *
 * @param[inout] list Dummy head node designating the list.
 *
 * @return Pointer to dequeued node.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is unpredictable if @p list is empty.
 *
 * @see
 * - stroll_dlist_dqueue_front_init()
 * - stroll_dlist_dqueue_back()
 * - stroll_dlist_init()
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
 * @param[inout] list Dummy head node designating the list.
 *
 * @return Pointer to dequeued node.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is unpredictable if @p list is empty.
 *
 * @see
 * - stroll_dlist_dqueue_front()
 * - stroll_dlist_dqueue_back_init()
 * - stroll_dlist_init()
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
 * @param[inout] list Dummy head node designating the list.
 *
 * @return Pointer to dequeued node.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is unpredictable if @p list is empty.
 *
 * @see
 * - stroll_dlist_dqueue_back_init()
 * - stroll_dlist_dqueue_front()
 * - stroll_dlist_init()
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
 * @param[inout] list Dummy head node designating the list.
 *
 * @return Pointer to dequeued node.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is unpredictable if @p list is empty.
 *
 * @see
 * - stroll_dlist_dqueue_back()
 * - stroll_dlist_dqueue_front_init()
 * - stroll_dlist_init()
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
 * Replace an old stroll_dlist_node entry by a new one.
 *
 * @param[in]  old  Node to be replaced.
 * @param[out] node New node to replace @p old with.
 *
 * @warning
 * - Result is undefined if @p old node has not been previously initialized.
 * - Behavior is unpredictable if @p old node is empty.
 * - Behavior is unpredictable when @p old and @p node both point to the same
 *   node.
 *
 * @see
 * - stroll_dlist_move_after()
 * - stroll_dlist_move_before()
 * - stroll_dlist_replace_init()
 * - stroll_dlist_init()
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
 * Replace then reinitialize an old stroll_dlist_node entry by a new one.
 *
 * @param[inout] old  Node to be replaced.
 * @param[out]   node New node to replace @p old with.
 *
 * @warning
 * - Result is undefined if @p old node has not been previously initialized.
 * - Behavior is unpredictable if @p old node is empty.
 * - Behavior is unpredictable when @p old and @p node both point to the same
 *   node.
 *
 * @see
 * - stroll_dlist_replace()
 * - stroll_dlist_init()
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_dlist_replace_init(struct stroll_dlist_node * __restrict old,
                          struct stroll_dlist_node * __restrict node)
{
	stroll_dlist_assert_api(!stroll_dlist_empty(old));
	stroll_dlist_assert_api(node);
	stroll_dlist_assert_api(old != node);

	stroll_dlist_replace(old, node);
	stroll_dlist_init(old);
}

/**
 * Move a stroll_dlist_node entry from one location before another.
 *
 * @param[inout] at   Node before which to insert @p node.
 * @param[inout] node Node to move.
 *
 * @warning
 * - Result is undefined if @p at node has not been previously initialized.
 * - Behavior is unpredictable when @p at and @p node both point to the same
 *   node.
 *
 * @see
 * - stroll_dlist_replace()
 * - stroll_dlist_move_after()
 * - stroll_dlist_init()
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
 * Move a stroll_dlist_node entry from one location after another.
 *
 * @param[inout] at   Node after which to append @p node.
 * @param[inout] node Node to move.
 *
 * @warning
 * - Result is undefined if @p at node has not been previously initialized.
 * - Behavior is unpredictable when @p at and @p node both point to the same
 *   node.
 *
 * @see
 * - stroll_dlist_replace()
 * - stroll_dlist_move_after()
 * - stroll_dlist_init()
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
 * Extract / remove a portion of nodes from a doubly linked list.
 *
 * @param[inout] first First node of portion to remove.
 * @param[inout] last  Last node of portion to remove.
 *
 * The [@p first,@p last] sequence of nodes *MUST* be part of the same doubly
 * linked list.
 *
 * @warning
 * - Result is undefined if @p first node has not been previously initialized.
 * - Result is undefined if @p last node has not been previously initialized.
 *
 * @see
 * - stroll_dlist_embed_after()
 * - stroll_dlist_embed_before()
 * - stroll_dlist_splice_after()
 * - stroll_dlist_splice_before()
 * - stroll_dlist_init()
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
 * Insert a portion of nodes into a doubly linked list before specified
 * location.
 *
 * @param[inout] at    Node before which portion is inserted.
 * @param[inout] first First node of portion to insert.
 * @param[inout] last  Last node of portion to insert.
 *
 * The doubly linked list within which the [@p first,@p last] sequence of nodes
 * is contained is not updated to reflect link changes. If required, use
 * stroll_dlist_splice_before() instead.
 *
 * @warning
 * Result is undefined if @p at node has not been previously initialized.
 *
 * @see
 * - stroll_dlist_embed_after()
 * - stroll_dlist_withdraw()
 * - stroll_dlist_splice_after()
 */
static inline __stroll_nonull(1, 2, 3) __stroll_nothrow
void
stroll_dlist_embed_before(struct stroll_dlist_node * __restrict at,
                          struct stroll_dlist_node *            first,
                          struct stroll_dlist_node *            last)
{
	stroll_dlist_assert_api(at);
	stroll_dlist_assert_api(first);
	stroll_dlist_assert_api(last);
	stroll_dlist_assert_api(at != first);
	stroll_dlist_assert_api(at != last);

	struct stroll_dlist_node * prev = at->prev;

	first->prev = prev;
	prev->next = first;

	last->next = at;
	at->prev = last;
}

/**
 * Insert a portion of nodes into a doubly linked list after specified
 * location.
 *
 * @param[inout] at    Node after which portion is inserted.
 * @param[inout] first First node of portion to insert.
 * @param[inout] last  Last node of portion to insert.
 *
 * The doubly linked list within which the [@p first,@p last] sequence of nodes
 * is contained is not updated to reflect link changes. If required, use
 * stroll_dlist_splice_after() instead.
 *
 * @warning
 * Result is undefined if @p at node has not been previously initialized.
 *
 * @see
 * - stroll_dlist_embed_before()
 * - stroll_dlist_withdraw()
 * - stroll_dlist_splice_before()
 * - stroll_dlist_init()
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
 * Extract a portion of nodes from a doubly linked list and move it to result
 * list before specified location.
 *
 * @param[inout] at    Node to insert nodes before.
 * @param[inout] first First node of portion to move.
 * @param[inout] last  Last node of portion to move.
 *
 * The [@p first,@p last] sequence of nodes *MUST* be part of the same doubly
 * linked list.
 *
 * @warning
 * - Result is undefined if @p at node has not been previously initialized.
 * - Result is undefined if @p first node is empty or has not been initialized.
 * - Result is undefined if @p last node is empty or has not been initialized.
 *
 * @see
 * - stroll_dlist_embed_before()
 * - stroll_dlist_withdraw()
 * - stroll_dlist_splice_after()
 * - stroll_dlist_init()
 */
extern void
stroll_dlist_splice_before(struct stroll_dlist_node * __restrict at,
                           struct stroll_dlist_node *            first,
                           struct stroll_dlist_node *            last)
	__stroll_nonull(1, 2, 3) __stroll_nothrow;

/**
 * Extract a portion of nodes from a doubly linked list and move it to result
 * list after specified location.
 *
 * @param[inout] at    Node to insert nodes after.
 * @param[inout] first First node of portion to move.
 * @param[inout] last  Last node of portion to move.
 *
 * The [@p first,@p last] sequence of nodes *MUST* be part of the same doubly
 * linked list.
 *
 * @warning
 * - Result is undefined if @p at node has not been previously initialized.
 * - Result is undefined if @p first node is empty or has not been initialized.
 * - Result is undefined if @p last node is empty or has not been initialized.
 *
 * @see
 * - stroll_dlist_embed_after()
 * - stroll_dlist_withdraw()
 * - stroll_dlist_splice_behore()
 * - stroll_dlist_init()
 */
extern void
stroll_dlist_splice_after(struct stroll_dlist_node * __restrict at,
                          struct stroll_dlist_node *            first,
                          struct stroll_dlist_node *            last)
	__stroll_nonull(1, 2, 3) __stroll_nothrow;

/**
 * Return type casted pointer to entry containing specified node.
 *
 * @param[in] _node   stroll_dlist_node to retrieve container from.
 * @param     _type   Type of container
 * @param     _member Member field of container structure holding the
 *                    stroll_dlist_node @p _node.
 *
 * @return Pointer to type casted entry.
 */
#define stroll_dlist_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

/**
 * Return type casted pointer to entry following specified entry.
 *
 * @param[in] _entry  Entry containing stroll_dlist_node.
 * @param     _member Member field of container structure holding the
 *                    stroll_dlist_node @p _node.
 *
 * @return Pointer to following entry.
 *
 * @warning
 * Result is undefined if `(_entry)->_member` has not been previously
 * initialized.
 *
 * @see
 * - stroll_dlist_next()
 * - stroll_dlist_prev_entry()
 * - stroll_dlist_init()
 */
#define stroll_dlist_next_entry(_entry, _member) \
	stroll_dlist_entry(stroll_dlist_next(&(_entry)->_member), \
	                   typeof(*(_entry)), \
	                   _member)

/**
 * Return type casted pointer to entry preceding specified entry.
 *
 * @param[in] _entry  Entry containing stroll_dlist_node.
 * @param     _member Member field of container structure holding the
 *                    stroll_dlist_node @p _node.
 *
 * @return Pointer to preceding entry.
 *
 * @warning
 * Result is undefined if `(_entry)->_member` has not been previously
 * initialized.
 *
 * @see
 * - stroll_dlist_prev()
 * - stroll_dlist_next_entry()
 * - stroll_dlist_init()
 */
#define stroll_dlist_prev_entry(_entry, _member) \
	dlist_entry(stroll_dlist_prev(&(_entry)->_member), \
	            typeof(*(_entry)), \
	            _member)

/**
 * Iterate over doubly linked list nodes.
 *
 * @param[in]  _head Dummy head stroll_dlist_node designating the list to
 *                   iterate over.
 * @param[out] _node Pointer to current node.
 *
 * @warning
 * - Result is undefined if @p _head has not been previously initialized.
 * - Behavior is undefined when @p _node is modified while iterating over
 *   @p _head.
 *
 * @see
 * - stroll_dlist_foreach_node_safe()
 * - stroll_dlist_foreach_prev_node()
 * - stroll_dlist_foreach_entry()
 * - stroll_dlist_init()
 */
#define stroll_dlist_foreach_node(_head, _node) \
	for (_node = stroll_dlist_next(_head);  \
	     _node != (_head); \
	     _node = stroll_dlist_next(_node))

#define stroll_dlist_continue_node(_head, _node) \
	for (_node = stroll_dlist_next(_node); \
	     _node != (_head); \
	     _node = stroll_dlist_next(_node))

/**
 * Iterate over doubly linked list nodes safely.
 *
 * @param[in]  _head Dummy head stroll_dlist_node designating the list to
 *                   iterate over.
 * @param[out] _node Pointer to current node.
 * @param[out] _tmp  Pointer to another stroll_dlist_node used for temporary
 *                   storage.
 *
 * Allows to iterate over a stroll_dlist_node based list safely against
 * @p node removal.
 * 
 * @warning
 * Result is undefined if @p _head has not been previously initialized.
 *
 * @see
 * - stroll_dlist_foreach_node()
 * - stroll_dlist_foreach_entry_safe()
 * - stroll_dlist_init()
 */
#define stroll_dlist_foreach_node_safe(_head, _node, _tmp) \
	for (_node = stroll_dlist_next(_head), \
	     _tmp = stroll_dlist_next(_node);  \
	     _node != (_head); \
	     _node = _tmp, \
	     _tmp = stroll_dlist_next(_tmp))

#define stroll_dlist_continue_node_safe(_head, _node, _tmp) \
	for (_node = stroll_dlist_next(_node), \
	     _tmp = stroll_dlist_next(_node);  \
	     _node != (_head); \
	     _node = _tmp, \
	     _tmp = stroll_dlist_next(_tmp))

/**
 * Iterate backward over doubly linked list nodes.
 *
 * @param[in]  _head Dummy head stroll_dlist_node designating the list to
 *                   iterate over.
 * @param[out] _node Pointer to current node.
 *
 * @warning
 * - Result is undefined if @p _head has not been previously initialized.
 * - Behavior is undefined when @p _node is modified while iterating over
 *   @p _head.
 *
 * @see
 * - stroll_dlist_foreach_node()
 * - stroll_dlist_foreach_prev_entry()
 * - stroll_dlist_init()
 */
#define stroll_dlist_foreach_prev_node(_head, _node) \
	for (_node = stroll_dlist_prev(_head); \
	     _node != (_head); \
	     _node = stroll_dlist_prev(_node))

/**
 * Iterate backward over doubly linked list nodes safely.
 *
 * @param[in]  _head Dummy head stroll_dlist_node designating the list to
 *                   iterate over.
 * @param[out] _node Pointer to current node.
 * @param[out] _tmp  Pointer to another stroll_dlist_node used for temporary
 *                   storage.
 *
 * Allows to iterate backward over a stroll_dlist_node based list safely against
 * @p node removal.
 *
 * @warning
 * Result is undefined if @p _head has not been previously initialized.
 *
 * @see
 * - stroll_dlist_foreach_prev_node()
 * - stroll_dlist_foreach_prev_entry_safe()
 * - stroll_dlist_init()
 */
#define stroll_dlist_foreach_prev_node_safe(_head, _node, _tmp) \
	for (_node = stroll_dlist_prev(_head), \
	     _tmp = stroll_dlist_prev(_node); \
	     _node != (_head); \
	     _node = _tmp, \
	     _tmp = stroll_dlist_prev(_tmp))

/**
 * Iterate over doubly linked list entries.
 *
 * @param[in]  _head   Dummy head stroll_dlist_node designating the list to
 *                     iterate over.
 * @param[out] _entry  Pointer to entry containing @p _head's current node.
 * @param      _member Member field of container structure holding the
 *                     stroll_dlist_node @p _node.
 *
 * @warning
 * - Result is undefined if @p _head has not been previously initialized.
 * - Behavior is undefined when @p _node is modified while iterating over
 *   @p _head.
 *
 * @see
 * - stroll_dlist_foreach_node()
 * - stroll_dlist_init()
 */
#define stroll_dlist_foreach_entry(_head, _entry, _member) \
	for (_entry = stroll_dlist_entry((_head)->next, \
	                                 typeof(*(_entry)), _member); \
	     &(_entry)->_member != (_head); \
	     _entry = stroll_dlist_next_entry(_entry, _member))

#define stroll_dlist_continue_entry(_head, _entry, _member) \
	for (_entry = stroll_dlist_next_entry(_entry, _member); \
	     &(_entry)->_member != (_head); \
	     _entry = stroll_dlist_next_entry(_entry, _member))

/**
 * Iterate over doubly linked list entries safely.
 *
 * @param[in]  _head   Dummy head stroll_dlist_node designating the list to
 *                     iterate over.
 * @param[out] _entry  Pointer to entry containing @p _head's current node.
 * @param      _member Member field of container structure holding the
 *                     stroll_dlist_node @p _node.
 * @param[out] _tmp    Pointer to same type as @p _entry used for temporary
 *                     storage.
 *
 * Allows to iterate over stroll_dlist_node based list entries safely against
 * @p node removal.
 *
 * @warning
 * Result is undefined if @p _head has not been previously initialized.
 *
 * @see
 * - stroll_dlist_foreach_entry()
 * - stroll_dlist_foreach_node_safe()
 * - stroll_dlist_init()
 */
#define stroll_dlist_foreach_entry_safe(_head, _entry, _member, _tmp) \
	for (_entry = stroll_dlist_entry((_head)->next, \
	                                 typeof(*(_entry)), \
	                                 _member), \
	     _tmp = stroll_dlist_next_entry(_entry, _member); \
	     &(_entry)->_member != (_head); \
	     _entry = _tmp, \
	     _tmp = stroll_dlist_next_entry(_tmp, _member))

#define stroll_dlist_continue_entry_safe(_head, _entry, _member, _tmp) \
	for (_entry = stroll_dlist_next_entry(_entry, _member), \
	     _tmp = stroll_dlist_next_entry(_entry, _member); \
	     &(_entry)->_member != (_head); \
	     _entry = _tmp, \
	     _tmp = stroll_dlist_next_entry(_tmp, _member))

/**
 * Iterate backward over doubly linked list entries.
 *
 * @param[in]  _head   Dummy head stroll_dlist_node designating the list to
 *                     iterate over.
 * @param[out] _entry  Pointer to entry containing @p _head's current node.
 * @param      _member Member field of container structure holding the
 *                     stroll_dlist_node @p _node.
 *
 * @warning
 * - Result is undefined if @p _head has not been previously initialized.
 * - Behavior is undefined when @p _node is modified while iterating over
 *   @p _head.
 *
 * @see
 * - stroll_dlist_foreach_entry()
 * - stroll_dlist_foreach_prev_node()
 * - stroll_dlist_init()
 */
#define stroll_dlist_foreach_prev_entry(_head, _entry, _member) \
	for (_entry = stroll_dlist_entry((_head)->prev, \
	                                 typeof(*(_entry)), _member); \
	     &(_entry)->_member != (_head); \
	     _entry = stroll_dlist_prev_entry(_entry, _member))

/**
 * Iterate backward over doubly linked list entries safely.
 *
 * @param[in]  _head   Dummy head stroll_dlist_node designating the list to
 *                     iterate over.
 * @param[out] _entry  Pointer to entry containing @p _head's current node.
 * @param      _member Member field of container structure holding the
 *                     stroll_dlist_node @p _node.
 * @param[out] _tmp    Pointer to same type as @p _entry used for temporary
 *                     storage.
 *
 * Allows to iterate backward over stroll_dlist_node based list entries safely
 * against @p node removal.
 *
 * @warning
 * Result is undefined if @p _head has not been previously initialized.
 *
 * @see
 * - stroll_dlist_foreach_prev_entry()
 * - stroll_dlist_foreach_prev_node_safe()
 * - stroll_dlist_init()
 */
#define stroll_dlist_foreach_prev_entry_safe(_head, _entry, _member, _tmp) \
	for (_entry = stroll_dlist_entry((_head)->prev, \
	                                 typeof(*(_entry)), _member), \
	     _tmp = _entry; \
	     &(_entry)->_member != (_head); \
	     _entry = _tmp, \
	     _tmp = stroll_dlist_prev_entry(_tmp, _member))

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

#endif /* defined(CONFIG_STROLL_DLIST_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_DLIST_SELECT_SORT)

/**
 * Sort specified list according to the selection sort scheme.
 *
 * @param[inout] head    head of list to sort.
 * @param[in]    compare @p list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 *
 * Sort the @p list using the @p compare comparison function according to
 * the @rstlnk{List selection sort} algorithm.
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
 * - Behavior is undefined when called on an empty stroll_dlist_node.
 * - Implemented for reference only: **DO NOT USE IT**. Refer to
 *   @rstlnk{Sorting lists} for more informations related to algorithm
 *   selection.
 */
extern void
stroll_dlist_select_sort(struct stroll_dlist_node * __restrict head,
                         stroll_dlist_cmp_fn *                 compare,
                         void *                                data)
	__stroll_nonull(1, 2);

#endif /* defined(CONFIG_STROLL_DLIST_SELECT_SORT) */

#if defined(CONFIG_STROLL_DLIST_INSERT_SORT)

/**
 * Sort specified list according to the insertion sort scheme.
 *
 * @param[inout] head    head of list to sort.
 * @param[in]    compare @p list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 *
 * Sort the @p list using the @p compare comparison function according to
 * the @rstlnk{List insertion sort} algorithm.
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
 * Behavior is undefined when called on an empty stroll_dlist_node.
 */
extern void
stroll_dlist_insert_sort(struct stroll_dlist_node * __restrict head,
                         stroll_dlist_cmp_fn *                 compare,
                         void *                                data)
	__stroll_nonull(1, 2);

#endif /* defined(CONFIG_STROLL_DLIST_INSERT_SORT) */

#if defined(CONFIG_STROLL_DLIST_MERGE_SORT)

/**
 * Sort 2 presorted stroll_dlist_node doubly linked lists into a single one.
 *
 * @param[inout] result  Head of list within which both lists will be sorted
 *                       into.
 * @param[inout] source  Head of list to sort within @p result.
 * @param[in]    compare stroll_dlist_node nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 *
 * Merge the already sorted @p result and @p source lists using the @p compare
 * comparison function. Sorted result is stored into the @p result list.
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
 * - Behavior is undefined when called on an empty @p result stroll_dlist_node
 *   head.
 * - Behavior is undefined when called on an empty @p source stroll_dlist_node
 *   head.
 */
extern void
stroll_dlist_merge_presort(struct stroll_dlist_node * __restrict result,
                           struct stroll_dlist_node * __restrict source,
                           stroll_dlist_cmp_fn *                 compare,
                           void *                                data)
	__stroll_nonull(1, 2, 3);

/**
 * Sort specified doubly linked list according to an hybrid merge sort scheme.
 *
 * @param[inout] head     Head of list to sort.
 * @param[in]    compare  @p head list nodes comparison function.
 * @param[inout] data     Optional arbitrary user data.
 *
 * Sort the @p list using the @p compare comparison function according to
 * the @rstlnk{List merge sort} algorithm.
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
 * - Behavior is undefined when called on an empty stroll_dlist_node head.
 */
extern void
stroll_dlist_merge_sort(struct stroll_dlist_node * __restrict head,
                        stroll_dlist_cmp_fn *                 compare,
                        void *                                data)
	__stroll_nonull(1, 2);

#endif /* defined(CONFIG_STROLL_DLIST_MERGE_SORT) */

#endif /* _STROLL_DLIST_H */
