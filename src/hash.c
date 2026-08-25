/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2026 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/hash.h"

#if defined(CONFIG_STROLL_HASH_STR)

#include "stroll/bops.h"
#include <string.h>
#include <endian.h>

/*
 * Non-cryptographic string hashing implementation.
 *
 * Cowardly copied from Linux kernel <linux>/fs/namei.c:full_name_hash()
 * function.
 */

#if __WORDSIZE == 64

/*
 * Register pressure in the mixing function is an issue, particularly
 * on 32-bit x86, but almost any function requires one state value and
 * one temporary.  Instead, use a function designed for two state values
 * and no temporaries.
 *
 * This function cannot create a collision in only two iterations, so
 * we have two iterations to achieve avalanche.  In those two iterations,
 * we have six layers of mixing, which is enough to spread one bit's
 * influence out to 2^6 = 64 state bits.
 *
 * Rotate constants are scored by considering either 64 one-bit input
 * deltas or 64*63/2 = 2016 two-bit input deltas, and finding the
 * probability of that delta causing a change to each of the 128 output
 * bits, using a sample of random initial states.
 *
 * The Shannon entropy of the computed probabilities is then summed
 * to produce a score.  Ideally, any input change has a 50% chance of
 * toggling any given output bit.
 *
 * Mixing scores (in bits) for (12,45):
 * Input delta: 1-bit      2-bit
 * 1 round:     713.3    42542.6
 * 2 rounds:   2753.7   140389.8
 * 3 rounds:   5954.1   233458.2
 * 4 rounds:   7862.6   256672.2
 * Perfect:    8192     258048
 *            (64*128) (64*63/2 * 128)
 */
#define STROLL_HASH_MIX(x, y, a) \
	(        x ^= (a), \
	 y ^= x, x = stroll_bops_rotl64(x, 12), \
	 x += y, y = stroll_bops_rotl64(y, 45), \
	 y *= 9)

static inline __const __nothrow
unsigned long
stroll_hash_leultoh(unsigned long value)
{
	return le64toh(value);
}

/* Fold two longs into one 32-bit hash value. */
static inline __const __nothrow
unsigned int
stroll_hash_fold(unsigned long x, unsigned long y)
{
	y ^= x * STROLL_HASH_GOLDEN_RATIO64;
	y *= STROLL_HASH_GOLDEN_RATIO64;

	return (unsigned int)(y >> 32);
}

#else /* __WORDSIZE != 64 */

/*
 * Mixing scores (in bits) for (7,20):
 * Input delta: 1-bit      2-bit
 * 1 round:     330.3     9201.6
 * 2 rounds:   1246.4    25475.4
 * 3 rounds:   1907.1    31295.1
 * 4 rounds:   2042.3    31718.6
 * Perfect:    2048      31744
 *            (32*64)   (32*31/2 * 64)
 */
#define STROLL_HASH_MIX(x, y, a) \
	(        x ^= (a), \
	 y ^= x, x = stroll_bops_rotl32(x, 7), \
	 x += y, y = stroll_bops_rotl32(y, 20), \
	 y *= 9)

static inline __const __nothrow
unsigned long
stroll_hash_leultoh(unsigned long value)
{
	return le32toh(value);
}

static inline __const __nothrow
unsigned int
stroll_hash_fold(unsigned long x, unsigned long y)
{
	return _stroll_hash32(y ^ _stroll_hash32(x))
}

#endif /* __WORDSIZE == 64 */

/*
 * Return the hash of a string of known length.
 */
unsigned int
_stroll_hash_salt_str(unsigned long              salt,
                      const uint8_t * __restrict string,
                      size_t                     length)
{
	stroll_hash_assert_intern(string);
	stroll_hash_assert_intern(length);

	unsigned long a;
	unsigned long x = 0;
	unsigned long y = salt;

	while (length >= sizeof(a)) {
		memcpy(&a, string, sizeof(a));
		STROLL_HASH_MIX(x, y, a);

		string += sizeof(a);
		length -= sizeof(a);
	}

	if (length) {
		a = 0;
		memcpy(&a, string, length);
		x ^= stroll_hash_leultoh(a);
	}

	return stroll_hash_fold(x, y);
}

#endif /* defined(CONFIG_STROLL_HASH_STR) */
