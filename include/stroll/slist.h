/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
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
	/**
	 * @internal
	 *
	 * Node following this node into a stroll_slist.
	 *
	 * Points to `NULL` if this node ends a stroll_slist.
	 */
	struct stroll_slist_node * next;
};

/**
 * stroll_slist_node constant initializer.
 */
#define STROLL_SLIST_NODE_INIT \
	{ .next = NULL }

/**
 * Singly linked list
 *
 * @see stroll_slist_node
 */
struct stroll_slist {
	/**
	 * @internal
	 *
	 * Head node allowing to locate first node of this stroll_slist.
	 */
	struct stroll_slist_node   head;
	/**
	 * @internal
	 *
	 * Last entry node ending this stroll_slist.
	 */
	struct stroll_slist_node * tail;
};

/**
 * stroll_slist constant initializer.
 *
 * @param[out] _list stroll_slist variable to initialize.
 *
 * @see stroll_slist
 */
#define STROLL_SLIST_INIT(_list) \
	{ \
		.head = STROLL_SLIST_NODE_INIT, \
		.tail = &(_list).head \
	}

/**
 * Initialize a stroll_slist
 *
 * @param[out] list stroll_slist to initialize.
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
 * @param[in] list stroll_slist to test.
 *
 * @retval true  empty
 * @retval false not empty
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
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
 * @param[in] list stroll_slist to get head from.
 *
 * Return a pointer to the stroll_slist internal node that points to first
 * user / data stroll_slist_node.
 *
 * @return Pointer to stroll_list's head node.
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
 * @param[in] node Node which to get the successor from.
 *
 * @return Pointer to following node.
 *
 * @see
 * - STROLL_SLIST_NODE_INIT()
 * - stroll_slist_next_entry()
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
 * @param[in] list stroll_slist to get the first node from.
 *
 * @retval Pointer to first list's node.
 * @retval NULL means list is empty.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is undefined when called on an empty stroll_slist.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_head()
 * - stroll_slist_first_entry()
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
 * @param[in] list stroll_slist to get the last node from.
 *
 * @retval Pointer to last list's node.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - The returned value points to list's head node when @p list is empty.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_head()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_first()
 * - stroll_slist_last_entry()
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
 * @param[inout] list     stroll_slist to add node to.
 * @param[inout] previous Node preceding the one to add.
 * @param[out]   node     Node to add.
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_remove()
 * - stroll_slist_move()
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
 * @param[inout] list     stroll_slist to remove node from.
 * @param[inout] previous Node preceding the one to remove.
 * @param[in]    node     Node to remove.
 *
 * @p list **MUST** contain @p previous and @p node nodes.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is undefined when called on an empty stroll_slist.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_append()
 * - stroll_slist_move()
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
 * @param[inout] list     stroll_slist to insert node into
 * @param[inout] at       list's node to insert node after
 * @param[inout] previous Node preceding the node to move
 * @param[inout] node     Node to move
 *
 * @p list **MUST** contain @p at, @p previous and @p node nodes.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is undefined when called on an empty stroll_slist.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_empty()
 * - stroll_slist_append()
 * - stroll_slist_remove()
 */
extern void stroll_slist_move(struct stroll_slist *                 list,
                              struct stroll_slist_node * __restrict at,
                              struct stroll_slist_node * __restrict previous,
                              struct stroll_slist_node * __restrict node)
	__stroll_nonull(1, 2, 3, 4) __stroll_nothrow __leaf;

/**
 * Add a node to the begining of a stroll_slist.
 *
 * @param[inout] list stroll_slist to add node to.
 * @param[out]   node Node to enqueue.
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_dqueue_front()
 * - stroll_slist_nqueue_back()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_slist_nqueue_front(struct stroll_slist *                 list,
                          struct stroll_slist_node * __restrict node)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(!list->head.next || list->tail);
	stroll_slist_assert_api(node);

	stroll_slist_append(list, &list->head, node);
}

/**
 * Add a node to the end of a stroll_slist.
 *
 * @param[inout] list stroll_slist to add node to.
 * @param[out]   node Node to enqueue.
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_dqueue_front()
 * - stroll_slist_nqueue_front()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_slist_nqueue_back(struct stroll_slist *                 list,
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
 * Remove a node from the begining of a stroll_slist and return it.
 *
 * @param[inout] list stroll_slist to dequeue node from.
 *
 * @return Pointer to dequeued node.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is undefined when called on an empty stroll_slist.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_nqueue_front()
 * - stroll_slist_nqueue_back()
 */
static inline __stroll_nonull(1) __stroll_nothrow
struct stroll_slist_node *
stroll_slist_dqueue_front(struct stroll_slist * __restrict list)
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
 * @param[inout] list  stroll_slist to remove nodes from.
 * @param[inout] first Node preceding the first node of the portion to remove.
 * @param[inout] last  Last node of portion to remove.
 *
 * @p list **MUST** contain the [@p first,@p last] range of nodes.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is undefined when called on an empty stroll_slist.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_embed()
 * - stroll_slist_splice()
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
	stroll_slist_assert_api(first != last);

	first->next = last->next;

	if (!last->next)
		list->tail = first;
}

/**
 * Insert a portion of nodes into a stroll_slist.
 *
 * @param[inout] list  stroll_slist to insert nodes into.
 * @param[inout] at    Node after which portion is inserted.
 * @param[in]    first First node of portion to insert.
 * @param[inout] last  Last node of portion to insert.
 *
 * @p list **MUST** contain the @p at node.
 *
 * The stroll_list within which the [@p first,@p last] range of nodes is
 * contained is not updated to reflect link changes. If required, use
 * stroll_slist_splice() instead.
 *
 * @warning
 * Result is undefined if @p list has not been previously initialized.
 *
 * @see
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_withdraw()
 * - stroll_slist_splice()
 */
static inline __stroll_nonull(1, 2, 3, 4) __stroll_nothrow
void
stroll_slist_embed(struct stroll_slist *                 list,
                   struct stroll_slist_node * __restrict at,
                   struct stroll_slist_node *            first,
                   struct stroll_slist_node *            last)
{
	stroll_slist_assert_api(list);
	stroll_slist_assert_api(!list->head.next || list->tail);
	stroll_slist_assert_api(at);
	stroll_slist_assert_api(first);
	stroll_slist_assert_api(last);
	stroll_slist_assert_api(at != first);
	stroll_slist_assert_api(at != last);

	last->next = at->next;
	if (!last->next)
		list->tail = last;

	at->next = first;
}

/**
 * Extract source list portion and move it to result list at specified location.
 *
 * @param[inout] result stroll_slist to insert nodes into.
 * @param[inout] at     result's node to insert nodes after.
 * @param[inout] source stroll_slist to extract nodes from.
 * @param[inout] first  Node preceding the nodes portion to move.
 * @param[inout] last   Last portions's node to move.
 *
 * @p source **MUST** contain the [@p first,@p last] range of nodes.
 *
 * @warning
 * - Result is undefined if @p result and / or @p source have not been previously
 *   initialized.
 * - Behavior is undefined when called on an empty @p source stroll_slist.
 *
 * @see
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_withdraw()
 * - stroll_slist_embed()
 */
extern void
stroll_slist_splice(struct stroll_slist *      __restrict result,
                    struct stroll_slist_node * __restrict at,
                    struct stroll_slist *      __restrict source,
                    struct stroll_slist_node * __restrict first,
                    struct stroll_slist_node * __restrict last)
	__stroll_nonull(1, 2, 3, 4, 5) __stroll_nothrow __leaf;

/**
 * Return type casted pointer to entry containing specified node.
 *
 * @param[in] _node   stroll_slist node to retrieve container from.
 * @param     _type   Type of container
 * @param     _member Member field of container structure pointing to @p _node.
 *
 * @return Pointer to type casted entry.
 */
#define stroll_slist_entry(_node, _type, _member) \
	containerof(_node, _type, _member)

/**
 * Return type casted pointer to entry following specified entry.
 *
 * @param[in] _entry  Entry containing stroll_slist node.
 * @param     _member Member field of container structure holding the
 *                    stroll_slist_node node.
 *
 * @return Pointer to following entry.
 *
 * @see
 * - stroll_slist_next()
 */
#define stroll_slist_next_entry(_entry, _member) \
	stroll_slist_entry(stroll_slist_next(&(_entry)->_member), \
	                   typeof(*(_entry)), \
	                   _member)

/**
 * Return type casted pointer to entry containing first node of specified
 * stroll_slist.
 *
 * @param[in] _list   stroll_slist to get the first node from.
 * @param     _type   Type of container.
 * @param     _member Member field of container structure @p _type holding the
 *                    stroll_slist_node node.
 *
 * @return Pointer to type casted entry.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is undefined when called on an empty stroll_slist.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_first()
 */
#define stroll_slist_first_entry(_list, _type, _member) \
	stroll_slist_entry(stroll_slist_first(_list), _type, _member)

/**
 * Return type casted pointer to entry containing last node of specified
 * stroll_slist.
 *
 * @param[in] _list   stroll_slist to get the last node from.
 * @param     _type   Type of container.
 * @param     _member Member field of container structure @p _type holding the
 *                    stroll_slist_node node.
 *
 * @return Pointer to type casted entry.
 *
 * @warning
 * - Result is undefined if @p list has not been previously initialized.
 * - Behavior is undefined when called on an empty stroll_slist.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_last()
 */
#define stroll_slist_last_entry(_list, _type, _member) \
	stroll_slist_entry(stroll_slist_last(_list), _type, _member)

/**
 * Iterate over stroll_slist nodes.
 *
 * @param[in]  _list stroll_slist to iterate over.
 * @param[out] _node Pointer to current node.
 *
 * @warning
 * - Result is undefined if @p _list has not been previously initialized.
 * - Behavior is undefined when @p _node is removed while iterating over
 *   @p _list.
 *
 * @see
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_foreach_entry()
 */
#define stroll_slist_foreach_node(_list, _node) \
	for (_node = (_list)->head.next; _node; _node = _node->next)

/**
 * Iterate over stroll_slist node container entries.
 *
 * @param[in]  _list   stroll_slist to iterate over.
 * @param[out] _entry  Pointer to entry containing @p _list's current node.
 * @param      _member Member field of container structure holding the
 *                     stroll_slist_node node.
 *
 * @warning
 * - Result is undefined if @p _list has not been previously initialized.
 * - Behavior is undefined when @p _entry is removed while iterating over
 *   @p _list.
 * - Behavior is undefined when called on an empty stroll_slist.
 *
 * @see
 * - stroll_slist_empty()
 * - stroll_slist_init()
 * - STROLL_SLIST_INIT()
 * - stroll_slist_foreach()
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
typedef int (stroll_slist_cmp_fn)
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
 * - Behavior is undefined when called on an empty stroll_slist.
 * - Implemented for reference only: **DO NOT USE IT**. Refer to
 *   @rstlnk{Sorting lists} for more informations related to algorithm
 *   selection.
 */
extern void
stroll_slist_bubble_sort(struct stroll_slist * __restrict list,
                         stroll_slist_cmp_fn *            compare,
                         void *                           data)
	__stroll_nonull(1, 2);

#endif /* defined(CONFIG_STROLL_SLIST_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_SLIST_SELECT_SORT)

/**
 * Sort specified list according to the selection sort scheme.
 *
 * @param[inout] list    stroll_slist to sort.
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
 * - Behavior is undefined when called on an empty stroll_slist.
 * - Implemented for reference only: **DO NOT USE IT**. Refer to
 *   @rstlnk{Sorting lists} for more informations related to algorithm
 *   selection.
 */
extern void
stroll_slist_select_sort(struct stroll_slist * __restrict list,
                         stroll_slist_cmp_fn *            compare,
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
 * Behavior is undefined when called on an empty stroll_slist.
 */
extern void
stroll_slist_insert_sort(struct stroll_slist * __restrict list,
                         stroll_slist_cmp_fn *            compare,
                         void *                           data)
	__stroll_nonull(1, 2);

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
 * - Behavior is undefined when called on an empty @p result stroll_slist.
 * - Behavior is undefined when called on an empty @p source stroll_slist.
 */
extern void
stroll_slist_merge_presort(struct stroll_slist * __restrict result,
                           struct stroll_slist * __restrict source,
                           stroll_slist_cmp_fn *            compare,
                           void *                           data)
	__stroll_nonull(1, 2, 3);

/**
 * Sort specified list according to an hybrid merge sort scheme.
 *
 * @param[inout] list     stroll_slist to sort.
 * @param[in]    compare  @p list nodes comparison function.
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
 * - Behavior is undefined when called on an empty stroll_slist.
 * - Behavior is undefined when called with @p nodes_nr < 1.
 */
extern void
stroll_slist_merge_sort(struct stroll_slist * __restrict list,
                        stroll_slist_cmp_fn *            compare,
                        void *                           data)
	__stroll_nonull(1, 2);

#endif /* defined(CONFIG_STROLL_SLIST_MERGE_SORT) */

#endif /* _STROLL_SLIST_H */
