/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Hash list interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      22 Sep 2025
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_HLIST_H
#define _STROLL_HLIST_H

#include <stroll/cdefs.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_hlist_assert_api(_expr) \
	stroll_assert("stroll:hlist", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_hlist_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_hlist_assert_intern(_expr) \
	stroll_assert("stroll:hlist", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_hlist_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Hashed list node
 *
 * Describes a single entry node linked into a stroll_hlist hashed list.
 *
 * @see
 * - stroll_hlist_init_node()
 * - stroll_hlist
 */
struct stroll_hlist_node {
	/**
	 * @internal
	 *
	 * Node following this node into a stroll_hlist.
	 */
	struct stroll_hlist_node *  next;
	/**
	 * @internal
	 *
	 * Node preceding this node into a stroll_hlist.
	 */
	struct stroll_hlist_node ** prev;
};

/**
 * stroll_hlist_node constant initializer.
 *
 * @param _hnode stroll_hlist_node variable to initialize.
 *
 * @see stroll_hlist_init_node()
 */
#define STROLL_HLIST_INIT_NODE(_hnode) \
	{ \
		.next = NULL, \
		.prev = &(_hnode)->next \
	}

/**
 * Initialize a stroll_dlist_node
 *
 * @param[out] hnode stroll_hlist_node to initialize.
 *
 * @see STROLL_HLIST_INIT_NODE()
 */
static inline
void
stroll_hlist_init_node(struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(hnode);

	hnode->next = NULL;
	hnode->prev = &hnode->next;
}

/**
 * Test wether a stroll_hlist_node is linked into a stroll_hlist.
 *
 * @param[in] hnode stroll_hlist_node to test.
 *
 * @retval true  linked
 * @retval false not linked
 *
 * @warning
 * Result is undefined if @p hnode has not been previously initialized.
 *
 * @see stroll_hlist_init_node()
 */
static inline
void
stroll_hlist_node_hashed(const struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(hnode);
	stroll_hlist_assert_api((hnode->prev != &hnode->next) || !hnode->next);

	return hnode->prev != &hnode->next;
}

/**
 * Insert a stroll_hlist_node before specified node.
 *
 * @param[inout] at    Node to insert @p hnode before.
 * @param[out]   hnode Node to insert.
 *
 * @warning
 * - Result is undefined if @p at has is not linked into a stroll_hlist.
 *
 * @see
 * - stroll_hlist_append_node()
 * - stroll_hlist_init_node()
 */
static inline
void
stroll_hlist_insert_node(struct stroll_hlist_node * __restrict at,
                         struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(stroll_hlist_node_hashed(at));
	stroll_hlist_assert_api(hnode);

	struct stroll_hlist_node ** prev = at->prev;

	hnode->next = at;
	hnode->prev = prev;
	at->prev = &hnode->next;
	*prev = hnode;
}

static inline
void
_stroll_hlist_append_node(struct stroll_hlist_node ** __restrict prev,
                          struct stroll_hlist_node * __restrict  hnode)
{
	stroll_hlist_assert_intern(prev);
	stroll_hlist_assert_intern(hnode);

	struct stroll_hlist_node * next = *prev;

	hnode->next = next;
	hnode->prev = prev;
	*prev = hnode;
	if (next)
		next->prev = &hnode->next;
}

/**
 * Append a stroll_hlist_node after specified node.
 *
 * @param[inout] at    Node to append @p hnode after.
 * @param[out]   hnode Node to append.
 *
 * @warning
 * - Result is undefined if @p at has not been previously initialized.
 *
 * @see
 * - stroll_hlist_insert_node()
 * - stroll_hlist_init_node()
 */
static inline
void
stroll_hlist_append_node(struct stroll_hlist_node * __restrict at,
                         struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(at);
	stroll_hlist_assert_api((at->prev != &at->next) || !at->next);
	stroll_hlist_assert_api(hnode);

	_stroll_hlist_append_node(&at->next, hnode);
}

/**
 * Remove a stroll_hlist_node from a stroll_hlist.
 *
 * @param[inout] hnode Node to remove.
 *
 * @warning
 * Result is undefined if @p hnode has not been previously initialized.
 *
 * @see
 * - stroll_hlist_remove_node_init()
 * - stroll_hlist_init_node()
 */
static inline
void
stroll_hlist_remove_node(struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(hnode);
	stroll_hlist_assert_api((hnode->prev != &hnode->next) || !hnode->next);

	struct stroll_hlist_node *  next = hnode->next;
	struct stroll_hlist_node ** prev = hnode->prev;

	*prev = next;
	if (next)
		next->prev = prev;
}

/**
 * Remove then reinitialize a stroll_hlist_node from a stroll_hlist.
 *
 * @param[inout] hnode Node to remove.
 *
 * @warning
 * Result is undefined if @p hnode has not been previously initialized.
 *
 * @see
 * - stroll_hlist_remove_node()
 * - stroll_hlist_init_node()
 */
static inline
void
stroll_hlist_remove_node_init(struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(hnode);
	stroll_hlist_assert_api((hnode->prev != &hnode->next) || !hnode->next);

	stroll_hlist_del(hnode);
	stroll_hlist_init_node(hnode);
}

/**
 * Hashed list.
 *
 * A linked list that may be used as a top-level bucket list to implement hash
 * tables.
 * This is implemented as a doubly linked list where stroll_hlist_node hashed
 * node may be inserted at head of list only.
 *
 * @see
 * - stroll_hlist_init()
 * - stroll_hlist_node
 */
struct stroll_hlist {
	/**
	 * @internal
	 *
	 * Leading node of this hashed list.
	 */
	struct hlist_node * head;
};

/**
 * stroll_hlist constant initializer.
 *
 * @param _hlist stroll_hlist variable to initialize.
 *
 * @see stroll_hlist_init()
 */
#define STROLL_HLIST_INIT(_hlist) \
	{ .head = NULL }

/**
 * Iterate over hashed list nodes.
 *
 * @param[in]  _hlist Pointer to stroll_hlist designating the list to iterate
 *                    over.
 * @param[out] _hnode Pointer to current stroll_hlist_node node.
 *
 * @warning
 * - Result is undefined if @p _hlist has not been previously initialized.
 * - Behavior is undefined when @p _hnode is modified while iterating over
 *   @p _hlist.
 *
 * @see
 * - stroll_hlist_foreach_node_safe()
 * - stroll_hlist_foreach_entry()
 * - stroll_hlist_init()
 */
#define stroll_hlist_foreach_node(_hlist, _hnode) \
	for (_hnode = (_hlist)->head; _hnode; _hnode = (_hnode)->next)

/**
 * Iterate safely over hashed list nodes.
 *
 * @param[in]  _hlist Pointer to stroll_hlist designating the list to iterate
 *                    over.
 * @param[out] _hnode Pointer to current stroll_hlist_node node.
 * @param[out] _tmp   Pointer to another stroll_hlist_node used for temporary
 *                    storage.
 *
 * Allows to iterate over stroll_hlist hashed list nodes safely against
 * @p _hnode removal.
 *
 * @warning
 * - Result is undefined if @p _hlist has not been previously initialized.
 *
 * @see
 * - stroll_hlist_foreach_node()
 * - stroll_hlist_foreach_entry_safe()
 * - stroll_hlist_init()
 */
#define stroll_hlist_foreach_node_safe(_hlist, _hnode, _tmp) \
	for (_hnode = (_hlist)->head; \
	     _hnode && ({ _tmp = (_hnode)->next; true; }); \
	     _hnode = _tmp)

/**
 * Continue iteration over hashed list nodes.
 *
 * @param[out] _hnode Pointer to current stroll_hlist_node node.
 *
 * Continue a previous iteration over stroll_hlist hashed list nodes starting
 * at the node following the current position pointed to by @p _hnode.
 *
 * @warning
 * - Behavior is undefined when @p _hnode is modified while iterating.
 *
 * @see
 * - stroll_hlist_foreach_continue_node_safe()
 * - stroll_hlist_foreach_node()
 * - stroll_hlist_foreach_continue_entry()
 */
#define stroll_hlist_foreach_continue_node(_hnode) \
	for (_hnode = (_hnode)->next; _hnode; _hnode = (_hnode)->next)

/**
 * Continue iteration safely over hashed list nodes.
 *
 * @param[out] _hnode Pointer to current stroll_hlist_node node.
 * @param[out] _tmp   Pointer to another stroll_hlist_node used for temporary
 *                    storage.
 *
 * Continue a previous iteration over stroll_hlist hashed list nodes safely
 * against @p _hnode removal.
 * Iteration (re)starts at the node following the current position pointed to by
 * @p _hnode.
 *
 * @see
 * - stroll_hlist_foreach_continue_node()
 * - stroll_hlist_foreach_node()
 * - stroll_hlist_foreach_continue_entry_safe()
 */
#define stroll_hlist_foreach_continue_node_safe(_hnode, _tmp) \
	for (_hnode = (_hnode)->next; \
	     _hnode && ({ _tmp = (_hnode)->next; true; }); \
	     _hnode = _tmp)

/**
 * Return type casted pointer to entry containing specified node.
 *
 * @param[in] _hnode  stroll_hlist_node to retrieve container from.
 * @param     _type   Type of container
 * @param     _member Member field of container structure holding the
 *                    stroll_hlist_node @p _hnode.
 *
 * @return Pointer to type casted entry.
 */
#define stroll_hlist_entry(_hnode, _type, _member) \
	containerof(_hnode, _type, _member)

#define stroll_hlist_entry_or_null(_hnode, _type, _member) \
	({ \
		typeof(_hnode) __hnode = _hnode; \
		__hnode ? stroll_hlist_entry(__hnode, _type, _member) : NULL; \
	 })

/**
 * Iterate over hashed list entries.
 *
 * @param[in]  _hlist  Pointer to stroll_hlist to iterate over.
 * @param[out] _hentry Pointer to entry containing @p _hlist's current node.
 * @param      _member Member field of @p *_hentry structure holding the
 *                     current stroll_hlist_node node.
 *
 * Allows to iterate over stroll_hlist hashed list entries.
 *
 * @warning
 * - Result is undefined if @p _hlist has not been previously initialized.
 * - Behavior is undefined when @p _hentry is modified while iterating over
 *   @p _hlist.
 *
  * @see
 * - stroll_hlist_foreach_node()
 * - stroll_hlist_init()
*/
#define stroll_hlist_foreach_entry(_hlist, _hentry, _member) \
	for (_hentry = stroll_hlist_entry_or_null((_hlist)->head, \
	                                          typeof(*(_hentry)), \
	                                          _member); \
	     _hentry; \
	     _hentry = stroll_hlist_entry_or_null((_hentry)->_member.next, \
	                                          typeof(*(_hentry)), \
	                                          _member))

/**
 * Iterate safely over hashed list entries.
 *
 * @param[in]  _hlist  Pointer to stroll_hlist to iterate over.
 * @param[out] _hentry Pointer to entry containing @p _hlist's current node.
 * @param      _member Member field of @p *_hentry structure holding the
 *                     current stroll_hlist_node node.
 * @param[out] _tmp    Pointer to a stroll_hlist_node used for temporary
 *                     storage.
 *
 * Allows to iterate over stroll_hlist hashed list entries safely against
 * @p _hentry removal.
 *
 * @warning
 * Result is undefined if @p _hlist has not been previously initialized.
 *
 * @see
 * - stroll_hlist_foreach_entry()
 * - stroll_hlist_foreach_node_safe()
 * - stroll_hlist_init()
 */
#define stroll_hlist_foreach_entry_safe(_hlist, _hentry, _member, _tmp) \
	for (_hentry = stroll_hlist_entry_or_null((_hlist)->head, \
	                                          typeof(*(_hentry)), \
	                                          _member); \
	     _hentry && ({ _tmp = (_hentry)->member.next; true; }); \
	     _hentry = stroll_hlist_entry_or_null(_tmp, \
	                                          typeof(*(_hentry)), \
	                                          _member))

/**
 * Continue iteration over hashed list entries.
 *
 * @param[out] _hentry Pointer to entry containing @p _hlist's current node.
 * @param      _member Member field of @p *_hentry structure holding the current
 *                     stroll_hlist_node node.
 * @param[out] _tmp    Pointer to a stroll_hlist_node used for temporary
 *                     storage.
 *
 * Continue a previous iteration over stroll_hlist hashed list entries starting
 * at the entry following the current position pointed to by @p _hentry.
 *
 * @warning
 * - Behavior is undefined when @p _hentry is modified while iterating.
 *
 * @see
 * - stroll_hlist_foreach_continue_entry_safe()
 * - stroll_hlist_foreach_entry()
 * - stroll_hlist_foreach_continue_node()
 */
#define stroll_hlist_foreach_continue_entry(_hentry, _member) \
	for (_hentry = stroll_hlist_entry_or_null((_hentry)->_member.next, \
	                                          typeof(*(_hentry)), \
	                                          _member); \
	     _hentry; \
	     _hentry = stroll_hlist_entry_or_null((_hentry)->_member.next, \
	                                          typeof(*(_hentry)), \
	                                          _member))

/**
 * Continue iteration safely over hashed list entries.
 *
 * @param[out] _hentry Pointer to entry containing @p _hlist's current node.
 * @param      _member Member field of @p *_hentry structure holding the current
 *                     stroll_hlist_node node.
 * @param[out] _tmp    Pointer to another stroll_hlist_node used for temporary
 *                     storage.
 *
 * Continue a previous iteration over stroll_hlist hashed list entries safely
 * against @p _hentry removal.
 * Iteration (re)starts at the entry following the current position pointed to
 * by @p _hentry.
 *
 * @see
 * - stroll_hlist_foreach_continue_entry()
 * - stroll_hlist_foreach_entry()
 * - stroll_hlist_foreach_continue_node_safe()
 */
#define stroll_hlist_foreach_continue_entry_safe(_hentry, _member, _tmp) \
	for (_hentry = stroll_hlist_entry_or_null((_hentry)->_member.next, \
	                                          typeof(*(_hentry)), \
	                                          _member); \
	     _hentry && ({ _tmp = (_hentry)->member.next; true; }); \
	     _hentry = stroll_hlist_entry_or_null(_tmp, \
	                                          typeof(*(_hentry)), \
	                                          _member))

/**
 * Test wether a stroll_hlist is empty or not.
 *
 * @param[in] hlist stroll_hlist to test.
 *
 * @retval true  empty
 * @retval false not empty
 *
 * @warning
 * Result is undefined if @p hlist has not been previously initialized.
 *
 * @see stroll_hlist_init()
 */
static inline
bool
stroll_hlist_empty(const struct stroll_hlist * __restrict hlist)
{
	stroll_hlist_assert_api(hlist);

	return hlist->head == NULL;
}

/**
 * Add / link a stroll_hlist_node into a hashed list.
 *
 * @param[inout] hlist Hashed list to add @p hnode to.
 * @param[out]   hnode Node to add to @p hlist.
 *
 * @warning
 * - Result is undefined if @p hlist has not been previously initialized.
 *
 * @see
 * - stroll_hlist_del()
 * - stroll_hlist_insert_node()
 * - stroll_hlist_init()
 */
static inline
void
stroll_hlist_add(struct stroll_hlist * __restrict      hlist,
                 struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(hlist);
	stroll_hlist_assert_api(hnode);

	_stroll_hlist_append_node(&hlist->head, hnode);
}

/**
 * Remove a stroll_hlist_node from a hashed list.
 *
 * @param[inout] hnode Node to remove.
 *
 * @warning
 * Result is undefined if @p hnode has not been previously initialized.
 *
 * @see
 * - stroll_hlist_del_init()
 * - stroll_hlist_init()
 * - stroll_hlist_remove_node()
 */
static inline
void
stroll_hlist_del(struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(hnode);

	stroll_hlist_remove_node(hnode);
}

/**
 * Remove then reinitialize a stroll_hlist_node from a hashed list.
 *
 * @param[inout] hnode Node to remove.
 *
 * @warning
 * Result is undefined if @p hnode has not been previously initialized.
 *
 * @see
 * - stroll_hlist_del()
 * - stroll_hlist_init()
 * - stroll_hlist_remove_node_init()
 */
static inline
void
stroll_hlist_del_init(struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(hnode);

	stroll_hlist_remove_node_init(hnode);
}

/**
 * Initialize a stroll_hlist
 *
 * @param[out] hlist stroll_hlist to initialize.
 *
 * @see STROLL_HLIST_INIT()
 */
static inline
void
stroll_hlist_init(struct stroll_hlist * __restrict hlist)
{
	stroll_hlist_assert_api(hlist);

	hlist->head = NULL;
}

#if 0

#if (__WORDSIZE != 32) && (__WORDSIZE != 64)
#error "Unsupported machine word size !"
#endif /* !((__WORDSIZE != 32) && (__WORDSIZE != 64)) */

/*
 * This hash multiplies the input by a large odd number and takes the
 * high bits.  Since multiplication propagates changes to the most
 * significant end only, it is essential that the high bits of the
 * product be used for the hash value.
 *
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * Although a random odd number will do, it turns out that the golden
 * ratio phi = (sqrt(5)-1)/2, or its negative, has particularly nice
 * properties.  (See Knuth vol 3, section 6.4, exercise 9.)
 *
 * These are the negative, (1 - phi) = phi**2 = (3 - sqrt(5))/2,
 * which is very slightly easier to multiply by and makes no
 * difference to the hash distribution.
 */
#define STROLL_HASH_GOLDEN_RATIO32 UINT32_C(0x61C88647)
#define STROLL_HASH_GOLDEN_RATIO64 UINT64_C(0x61C8864680B583EB)

static inline
uint32_t
_stroll_hash_uint32(uint32_t key)
{
	return key * STROLL_HASH_GOLDEN_RATIO32;
}

static inline
unsigned int
stroll_hash_uint32(uint32_t key, unsigned int bits)
{
	stroll_hlist_assert_api(bits);
	stroll_hlist_assert_api(bits <= 32);

	/* High bits are more random, so use them. */
	return _stroll_hash_uint32(key) >> (32U - bits);
}

#if __WORDSIZE == 64

static inline
unsigned int
stroll_hash_uint64(uint64_t key, unsigned int bits)
{
	stroll_hlist_assert_api(bits);
	stroll_hlist_assert_api(bits <= 32);

	/*
	 * 64x64-bit multiply is efficient on all 64-bit processors.
	 * High bits are more random, so use them.
	 */
	return (key * STROLL_HASH_GOLDEN_RATIO64) >> (64U - bits);
}

#else /* __WORDSIZE != 64 */

static inline
unsigned int
stroll_hash_uint64(uint64_t key, unsigned int bits)
{
	stroll_hlist_assert_api(bits);
	stroll_hlist_assert_api(bits <= 32);

	/* Hash 64 bits using only 32x32-bit multiply. */
	uint32_t tmp = (uint32_t)key ^ _stroll_hash_uint32(key >> 32);

	return stroll_hash_uint32(tmp, bits);
}

#endif /* __WORDSIZE == 64 */

static inline
unsigned int
stroll_hash_uint(unsigned int key, unsigned int bits)
{
	return stroll_hash_uint32(key, bits);
}

#if __WORDSIZE == 64

static inline
unsigned int
stroll_hash_ulong(unsigned long key, unsigned int bits)
{
	return stroll_hash_uint64(key, bits);
}

#else /* __WORDSIZE != 64 */

static inline
unsigned int
stroll_hash_ulong(unsigned long key, unsigned int bits)
{
	return stroll_hash_uint32(key, bits);
}

#endif /* __WORDSIZE == 64 */

static inline
unsigned int
stroll_hash_ptr(const void * __restrict ptr, unsigned int bits)
{
	return stroll_hash_ulong((unsigned long)ptr, bits);
}

static inline
void
stroll_htable_base_add(struct stroll_hlist * __restrict      htable,
                       struct stroll_hlist_node * __restrict hnode,
                       unsigned int                          bucket)
{
	stroll_hlist_add(&htable[bucket], hnode);
}

void
stroll_htable_base_init(struct stroll_hlist * __restrict htable,
                        unsigned int                     bits)
{
	stroll_hlist_assert_api(htable);
	stroll_hlist_assert_api(bits);
	stroll_hlist_assert_api(bits <= 32);

	unsigned int b;
	unsigned int nr = 1U << bits;

	for (b = 0; b < nr; b++)
		strol_hlist_init(&htable[b]);
}

#endif

#endif /* _STROLL_HLIST_H */
