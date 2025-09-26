/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Hash table interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      25 Sep 2025
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_HTABLE_H
#define _STROLL_HTABLE_H

#include <stroll/hash.h>
#include <stroll/hlist.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_htable_assert_api(_expr) \
	stroll_assert("stroll:htable", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_htable_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_htable_assert_intern(_expr) \
	stroll_assert("stroll:htable", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_htable_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static inline
void
stroll_htable_base_add32(struct stroll_hlist * __restrict      htable,
                         key,
                         unsigned int                          bits,
                         struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(bits);
	stroll_hlist_assert_api(bits <= 32);

	stroll_hlist_add(&htable[stroll_hash32(key, bits)], hnode);
}

static inline
void
stroll_htable_base_add(struct stroll_hlist * __restrict      htable,
                       unsigned int                          key,
                       unsigned int                          bits,
                       struct stroll_hlist_node * __restrict hnode)
{
	stroll_htable_base_add32(htable, key, bits, hnode);
}

static inline
void
stroll_htable_base_add64(struct stroll_hlist * __restrict      htable,
                         uint64_t                              key,
                         unsigned int                          bits,
                         struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_assert_api(bits);
	stroll_hlist_assert_api(bits <= 32);

	stroll_hlist_add(&htable[stroll_hash64(key, bits)], hnode);
}

#if __WORDSIZE == 64

static inline
void
stroll_htable_base_addul(struct stroll_hlist * __restrict      htable,
                         unsigned long                         key,
                         unsigned int                          bits,
                         struct stroll_hlist_node * __restrict hnode)
{
	stroll_htable_base_add64(htable, key, bits, hnode);
}

#else /* __WORDSIZE != 64 */

static inline
void
stroll_htable_base_addul(struct stroll_hlist * __restrict      htable,
                         long                         key,
                         unsigned int                          bits,
                         struct stroll_hlist_node * __restrict hnode)
{
	stroll_htable_base_add32(htable, key, bits, hnode);
}

#endif /* __WORDSIZE == 64 */

static inline
void
stroll_htable_base_addptr(struct stroll_hlist * __restrict      htable,
                          const void * __restrict               key,
                          unsigned int                          bits,
                          struct stroll_hlist_node * __restrict hnode)
{
	stroll_htable_base_addul(htable, (unsigned long)key, bits, hnode);
}

static inline
void
stroll_htable_base_del(struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_del(hnode);
}

static inline
void
stroll_htable_base_del_init(struct stroll_hlist_node * __restrict hnode)
{
	stroll_hlist_del_init(hnode);
}






static inline
struct stroll_hlist *
stroll_htable_base_bucket32(const struct stroll_hlist * __restrict htable,
                            uint32_t                               key,
                            unsigned int                           bits)
{
	return (struct stroll_hlist *)&htable[stroll_hash_uint32(key, bits)];
}

#define stroll_htable_base_foreach_node32(_htable, _key, _bits, _hnode) \
	stroll_hlist_foreach_node(&(_htable)[stroll_hash32(_key, _bits)], \
	                          _hnode)
	

static inline
struct stroll_hlist_node *
stroll_htable_base_find_uint32(const struct stroll_hlist * __restrict htable,
                               uint32_t                               key,
                               unsigned int                           bits,
                               stroll_htable_match_fn *               match)
{
	unsigned int                     bucket = stroll_hash_uint32(key, bits);
	const struct stroll_hlist_node * hnode = NULL;

	stroll_hlist_foreach_node(&htable[bucket], hnode) {
	}

	return hnode;
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

#endif /* _STROLL_HTABLE_H */
