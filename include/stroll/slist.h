/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Singly linked list interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      27 Jun 2017
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_SLIST_H
#define _STROLL_SLIST_H

#include <stroll/cdefs.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_slist_assert_api(_expr) \
	stroll_assert("stroll:slist", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_slist_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Singly linked list node
 *
 * Describes a single entry linked into a stroll_slist.
 */
struct stroll_slist_node {
	/** Node following this node into a stroll_slist. */
	struct stroll_slist_node * next;
};

/**
 * Singly linked list
 */
struct stroll_slist {
	/** stroll_slist head allowing to locate the first linked node. */
	struct stroll_slist_node   head;
	/** Last entry in this stroll_slist. */
	struct stroll_slist_node * tail;
};

/**
 * stroll_slist constant initializer.
 *
 * @param _list stroll_slist variable to initialize.
 */
#define STROLL_SLIST_INIT(_list) \
	{ \
		.head.next = NULL, \
		.tail      = &(_list)->head \
	}

/**
 * Initialize a stroll_slist
 *
 * @param list stroll_slist to initialize.
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_slist_init(struct stroll_slist * __restrict list)
{
	stroll_slist_assert_api(list);

	list->head.next = NULL;
	list->tail = &list->head;
}

/**
 * Test wether a stroll_slist is empty or not.
 *
 * @param list stroll_slist to test.
 *
 * @retval true  empty
 * @retval false not empty
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
bool
stroll_slist_empty(const struct stroll_slist * __restrict list)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(list->tail);

	return list->head.next == NULL;
}

/**
 * Return stroll_slist head.
 *
 * @param list stroll_slist to get head from.
 *
 * @return Pointer to list's head node.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_slist_node *
stroll_slist_head(struct stroll_slist * __restrict list)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(list->tail);

	return &list->head;
}

/**
 * Get node following specified node.
 *
 * @param node Node which to get the successor from.
 *
 * @return Pointer to following node.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_slist_node *
stroll_slist_next(const struct stroll_slist_node * __restrict node)
{
	stroll_slist_assert_api(node);

	return node->next;
}

/**
 * Get first node of specified stroll_slist.
 *
 * @param list stroll_slist to get the first node from.
 *
 * @retval Pointer to first list's node.
 * @retval NULL means list is empty.
 *
 * @warning Behavior is undefined when called on an empty stroll_slist.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_slist_node *
stroll_slist_first(const struct stroll_slist * __restrict list)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));

	return list->head.next;
}

/**
 * Get last node of specified stroll_slist.
 *
 * @param list stroll_slist to get the last node from.
 *
 * @retval Pointer to last list's node.
 * @retval Pointer to list's head node means list is empty.
 *
 * @warning Behavior is undefined when called on an empty stroll_slist.
 *
 * @see stroll_slist_head()
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow
struct stroll_slist_node *
stroll_slist_last(const struct stroll_slist * __restrict list)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));

	return list->tail;
}

/**
 * Add a node into a stroll_slist.
 *
 * @param list     stroll_slist to add node to.
 * @param previous Node preceding the one to add.
 * @param node     Node to add.
 */
static inline __stroll_nonull(1, 2, 3) __stroll_nothrow
void
stroll_slist_append(struct stroll_slist *                 list,
                    struct stroll_slist_node * __restrict previous,
                    struct stroll_slist_node * __restrict node)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(!list->head.next || list->tail);
	stroll_slist_assert_api(previous);
	stroll_slist_assert_api(node);

	if (!previous->next)
		/* Update tail pointer if previous points to last node. */
		list->tail = node;

	node->next = previous->next;
	previous->next = node;
}

/**
 * Remove a node from a stroll_slist.
 *
 * @param list     stroll_slist to remove node from.
 * @param previous Node preceding the one to remove.
 * @param node     Node to remove.
 *
 * @warning Behavior is undefined when called on an empty stroll_slist.
 */
static inline __stroll_nonull(1, 2, 3) __stroll_nothrow
void
stroll_slist_remove(struct stroll_slist *                       list,
                    struct stroll_slist_node * __restrict       previous,
                    const struct stroll_slist_node * __restrict node)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(previous);
	stroll_slist_assert_api(node);
	stroll_slist_assert_api(previous->next == node);

	if (!node->next)
		list->tail = previous;

	previous->next = node->next;
}

/**
 * Move stroll_slist node from one location to another.
 *
 * @param list     stroll_slist to insert node into
 * @param at       list's node to insert node after
 * @param previous Node preceding the node to move
 * @param node     Node to move
 */
extern void stroll_slist_move(struct stroll_slist *                 list,
                              struct stroll_slist_node * __restrict at,
                              struct stroll_slist_node * __restrict previous,
                              struct stroll_slist_node * __restrict node)
	__stroll_nonull(1, 2, 3, 4) __stroll_nothrow __leaf;

/**
 * Add a node to the end of a stroll_slist.
 *
 * @param list stroll_slist to add node to.
 * @param node Node to enqueue.
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_slist_nqueue(struct stroll_slist *                 list,
                    struct stroll_slist_node * __restrict node)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(!list->head.next || list->tail);
	stroll_slist_assert_api(node);

	node->next = NULL;

	list->tail->next = node;
	list->tail = node;
}

/**
 * Remove a node of the begining of a stroll_slist and return it.
 *
 * @param list stroll_slist to dequeue node from.
 *
 * @return Pointer to dequeued node.
 *
 * @warning Behavior is undefined when called on an empty stroll_slist.
 */
static inline __stroll_nonull(1) __stroll_nothrow
struct stroll_slist_node *
stroll_slist_dqueue(struct stroll_slist * __restrict list)
{
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(list->tail);

	struct stroll_slist_node * node = list->head.next;

	list->head.next = node->next;

	if (!node->next)
		list->tail = &list->head;

	return node;
}

/**
 * Extract / remove a portion of nodes from a stroll_slist.
 *
 * @param list  stroll_slist to remove nodes from.
 * @param first Node preceding the first node of the portion to remove.
 * @param last  Last node of portion to remove.
 *
 * @warning Behavior is undefined when called on an empty stroll_slist.
 */
static inline __stroll_nonull(1, 2, 3) __stroll_nothrow
void
stroll_slist_withdraw(struct stroll_slist *                       list,
                      struct stroll_slist_node * __restrict       first,
                      const struct stroll_slist_node * __restrict last)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(!stroll_slist_empty(list));
	stroll_slist_assert_api(first);
	stroll_slist_assert_api(last);

	first->next = last->next;

	if (!last->next)
		list->tail = first;
}

/**
 * Insert a portion of nodes into a stroll_slist.
 *
 * @param list  stroll_slist to insert nodes into.
 * @param at    Node after which portion is inserted.
 * @param first First node of portion to insert.
 * @param last  Last node of portion to insert.
 */
static inline __stroll_nonull(1, 2, 3, 4) __stroll_nothrow
void
stroll_slist_embed(struct stroll_slist *                 list,
                   struct stroll_slist_node * __restrict at,
                   struct stroll_slist_node * __restrict first,
                   struct stroll_slist_node * __restrict last)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(!list->head.next || list->tail);
	stroll_slist_assert_api(at);
	stroll_slist_assert_api(first);
	stroll_slist_assert_api(last);

	last->next = at->next;
	if (!last->next)
		list->tail = last;

	at->next = first;
}

/**
 * Extract source list portion and move it to result list at specified location.
 *
 * @param result stroll_slist to insert nodes into.
 * @param at     result's node to insert nodes after.
 * @param source stroll_slist to extract nodes from.
 * @param first  Node preceding the nodes portion to move.
 * @param last   Last portions's node to move.
 *
 * @warning Behavior is undefined when called on an empty @p source stroll_slist.
 */
extern void
stroll_slist_splice(struct stroll_slist *      __restrict result,
                    struct stroll_slist_node * __restrict at,
                    struct stroll_slist *      __restrict source,
                    struct stroll_slist_node * __restrict first,
                    struct stroll_slist_node * __restrict last)
	__stroll_nonull(1, 2, 3, 4, 5) __stroll_nothrow __leaf;

/**
 * Iterate over stroll_slist nodes.
 *
 * @param _list stroll_slist to iterate over.
 * @param _node Pointer to current node.
 */
#define stroll_slist_foreach_node(_list, _node) \
	for (_node = (_list)->head.next; _node; _node = _node->next)

/**
 * Return type casted pointer to entry containing specified node.
 *
 * @param _node   stroll_slist node to retrieve container from.
 * @param _type   Type of container
 * @param _member Member field of container structure pointing to _node.
 *
 * @return Pointer to type casted entry.
 */
#define stroll_slist_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

/**
 * Return type casted pointer to entry containing first node of specified
 * stroll_slist.
 *
 * @param _list   stroll_slist to get the first node from.
 * @param _type   Type of container.
 * @param _member Member field of container structure pointing to first _list's
 *                node.
 *
 * @return Pointer to type casted entry.
 */
#define stroll_slist_first_entry(_list, _type, _member) \
	stroll_slist_entry(stroll_slist_first(_list), _type, _member)

/**
 * Return type casted pointer to entry containing last node of specified stroll_slist.
 *
 * @param _list   stroll_slist to get the last node from.
 * @param _type   Type of container.
 * @param _member Member field of container structure pointing to last _list's
 *                node.
 *
 * @return Pointer to type casted entry.
 */
#define stroll_slist_last_entry(_list, _type, _member) \
	stroll_slist_entry(stroll_slist_last(_list), _type, _member)


/**
 * Return type casted pointer to entry following specified entry.
 *
 * @param _entry  Entry containing stroll_slist node.
 * @param _member Member field of container structure pointing to stroll_slist
 *                node.
 *
 * @return Pointer to following entry.
 */
#define stroll_slist_next_entry(_entry, _member) \
	stroll_slist_entry(next(&(_entry)->_member), typeof(*(_entry)), _member)

/**
 * Iterate over stroll_slist node container entries.
 *
 * @param _list   stroll_slist to iterate over.
 * @param _entry  Pointer to entry containing @p _list's current node.
 * @param _member Member field of container structure pointing to stroll_slist
 *                node.
 */
#define stroll_slist_foreach_entry(_list, _entry, _member) \
	for (_entry = stroll_slist_entry((_list)->head.next, \
	                                 typeof(*(_entry)), \
	                                 _member); \
	     &(_entry)->_member; \
	     _entry = stroll_slist_next_entry(_entry, _member))

/******************************************************************************
 * Stroll singly linked list sorting
 ******************************************************************************/

/**
 * stroll_slist_node item comparison routine prototype.
 *
 * Use this to implement a function that compares 2 stroll_slist_node nodes
 * given as arguments according to an arbitrary logic.
 *
 * Last argument may point to an optional arbitrary user data used for
 * comparison purposes.
 *
 * The function *MUST* return an integer less than, equal to, or greater than
 * zero if first argument is found, respectively, to be less than, to match, or
 * be greater than the second one.
 */
typedef int (stroll_slist_compare_fn)
            (const struct stroll_slist_node * __restrict,
             const struct stroll_slist_node * __restrict,
             void *)
	__stroll_nonull(1, 2);

#if defined(CONFIG_STROLL_SLIST_BUBBLE_SORT)

/**
 * Sort specified list according to the bubble sort scheme.
 *
 * @param[inout] list    stroll_slist to sort.
 * @param[in]    compare @p list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 */
extern void
stroll_slist_bubble_sort(struct stroll_slist * __restrict list,
                         stroll_slist_compare_fn *        compare,
                         void *                           data)
	__stroll_nonull(1, 2);

#endif /* defined(CONFIG_STROLL_SLIST_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_SLIST_SELECT_SORT)

/**
 * Sort specified list according to the insertion sort scheme.
 *
 * @param[inout] list    stroll_slist to sort.
 * @param[in]    compare @p list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 */
extern void
stroll_slist_select_sort(struct stroll_slist * __restrict list,
                         stroll_slist_compare_fn *        compare,
                         void *                           data)
	__stroll_nonull(1, 2);

#endif /* defined(CONFIG_STROLL_SLIST_SELECT_SORT) */

#if defined(CONFIG_STROLL_SLIST_INSERT_SORT)

/**
 * Sort specified list according to the insertion sort scheme.
 *
 * @param[inout] list    stroll_slist to sort.
 * @param[in]    compare @p list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 */
extern void
stroll_slist_insert_sort(struct stroll_slist * __restrict list,
                         stroll_slist_compare_fn *        compare,
                         void *                           data)
	__stroll_nonull(1, 2);

/**
 * Sort a specified number of nodes from a stroll_slist according to the
 * insertion sort scheme and move sorted nodes into a result stroll_slist.
 *
 * @param[inout] result  Result stroll_slist where sorted nodes are moved.
 * @param[inout] source  Source stroll_slist to sort.
 * @param[in]    count   Maximum number of source nodes to sort.
 * @param[in]    compare stroll_list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 */
extern void
stroll_slist_counted_insert_sort(struct stroll_slist * __restrict result,
                                 struct stroll_slist * __restrict source,
                                 unsigned int                     count,
                                 stroll_slist_compare_fn *        compare,
                                 void *                           data)
	__stroll_nonull(1, 2, 4);

#endif /* defined(CONFIG_STROLL_SLIST_INSERT_SORT) */

#if defined(CONFIG_STROLL_SLIST_MERGE_SORT)
/**
 * Sort 2 presorted stroll_slists into a single one.
 *
 * @param[inout] result  stroll_slist within which both stroll_slists will be
 *                       sorted into.
 * @param[inout] source  Source stroll_slist to sort within @p result.
 * @param[in]    compare stroll_list nodes comparison function.
 * @param[inout] data    Optional arbitrary user data.
 *
 * @warning Behavior is undefined when called on an empty @p source stroll_slist.
 */
extern void
stroll_slist_merge_presort(struct stroll_slist * __restrict result,
                           struct stroll_slist * __restrict source,
                           stroll_slist_compare_fn *        compare,
                           void *                           data)
	__stroll_nonull(1, 2, 3);

/**
 * Sort specified list according to an hybrid sort scheme mixing insertion and
 * merge sort algorithms.
 *
 * @param[inout] list     stroll_slist to sort.
 * @param[in]    run_len  Primary sorting run length in number of nodes.
 * @param[in]    nodes_nr Number of nodes linked into list.
 * @param[in]    compare  @p list nodes comparison function.
 * @param[inout] data     Optional arbitrary user data.
 *
 * Description
 * ----------
 *
 * Use insertion scheme to sort sublists shorter than @p run_len nodes then
 * switch to merge sort strategy for longer ones.
 *
 * Merge sorting implementation is based on an original idea attributed to
 * Jon McCarthy and described
 * [here](http://richardhartersworld.com/cri/2007/schoen.html).
 *
 * The whole point is : avoid excessive scanning of the list during sublist
 * splitting phases by using an additional bounded auxiliary space to store
 * sublist heads.
 * The whole process is performed in an iterative manner.
 *
 * __Time complexity__
 * worst       | average     | best
 * ------------|-------------|-----
 * O(n.log(n)) | O(n.log(n)) | O(n)
 *
 * __Auxiliary space__: allocated on stack
 * worst                | average   | best
 * ---------------------|-----------|--------
 * 27 slists (54 words) | O(log(n)) | 0 words
 *
 * __Stability__: yes
 *
 * @warning Behavior is undefined when called on an empty stroll_slist.
 * @warning Behavior is undefined when called with @p run_len < 1.
 * @warning Behavior is undefined when called with @p nodes_nr < 1.
 */
extern void
stroll_slist_hybrid_merge_sort(struct stroll_slist * __restrict list,
                               unsigned int                     run_len,
                               unsigned int                     nodes_nr,
                               stroll_slist_compare_fn *        compare,
                               void *                           data)
	__stroll_nonull(1, 4);

/**
 * Sort specified list according to an hybrid merge sort scheme.
 *
 * @param[inout] list     stroll_slist to sort.
 * @param[in]    nodes_nr Number of nodes linked into list.
 * @param[in]    compare  @p list nodes comparison function.
 * @param[inout] data     Optional arbitrary user data.
 *
 * Basically a wrapper around stroll_slist_hybrid_merge_sort() implementing a
 * heuristic to automatically compute a proper value for the initial run length.
 *
 * @warning Behavior is undefined when called on an empty stroll_slist.
 * @warning Behavior is undefined when called with @p nodes_nr < 1.
 *
 * @see stroll_slist_hybrid_merge_sort()
 */
extern void
stroll_slist_merge_sort(struct stroll_slist * __restrict list,
                        unsigned int                     nodes_nr,
                        stroll_slist_compare_fn *        compare,
                        void *                           data)
	__stroll_nonull(1, 3);

#endif /* defined(CONFIG_STROLL_SLIST_MERGE_SORT) */

#endif /* _STROLL_SLIST_H */
