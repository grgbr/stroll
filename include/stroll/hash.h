/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Content hashing interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      25 Sep 2025
 * @copyright Copyright (C) 2017-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_HASH_H
#define _STROLL_HASH_H

#include <stroll/cdefs.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_hash_assert_api(_expr) \
	stroll_assert("stroll:hash", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_hash_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_hash_assert_intern(_expr) \
	stroll_assert("stroll:hash", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_hash_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

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
_stroll_hash32(uint32_t key)
{
	return key * STROLL_HASH_GOLDEN_RATIO32;
}

static inline
unsigned int
stroll_hash32(uint32_t key, unsigned int bits)
{
	stroll_hash_assert_api(bits);
	stroll_hash_assert_api(bits <= 32);

	/* High bits are more random, so use them. */
	return _stroll_hash32(key) >> (32U - bits);
}

#if __WORDSIZE == 64

static inline
unsigned int
stroll_hash64(uint64_t key, unsigned int bits)
{
	stroll_hash_assert_api(bits);
	stroll_hash_assert_api(bits <= 32);

	/*
	 * 64x64-bit multiply is efficient on all 64-bit processors.
	 * High bits are more random, so use them.
	 */
	return (unsigned int)((key *
	                       STROLL_HASH_GOLDEN_RATIO64) >> (64U - bits));
}

#else /* __WORDSIZE != 64 */

static inline
unsigned int
stroll_hash64(uint64_t key, unsigned int bits)
{
	stroll_hash_assert_api(bits);
	stroll_hash_assert_api(bits <= 32);

	/* Hash 64 bits using only 32x32-bit multiply. */
	uint32_t tmp = (uint32_t)key ^ _stroll_hash32(key >> 32);

	return stroll_hash32(tmp, bits);
}

#endif /* __WORDSIZE == 64 */

static inline
unsigned int
stroll_hash(unsigned int key, unsigned int bits)
{
	return stroll_hash32(key, bits);
}

#if __WORDSIZE == 64

static inline
unsigned int
stroll_hashul(unsigned long key, unsigned int bits)
{
	return stroll_hash64(key, bits);
}

#else /* __WORDSIZE != 64 */

static inline
unsigned int
stroll_hashul(unsigned long key, unsigned int bits)
{
	return stroll_hash32(key, bits);
}

#endif /* __WORDSIZE == 64 */

static inline
unsigned int
stroll_hash_ptr(const void * __restrict ptr, unsigned int bits)
{
	return stroll_hashul((unsigned long)ptr, bits);
}

#endif /*  _STROLL_HASH_H */
