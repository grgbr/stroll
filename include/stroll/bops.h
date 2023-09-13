/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Bit operations
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      29 Aug 2017
 * @copyright Copyright (C) 2017-2023 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_BOPS_H
#define _STROLL_BOPS_H

#include <stroll/cdefs.h>
#include <stdint.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_bops_assert_api(_expr) \
	stroll_assert("stroll:bops", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_bops_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if (__WORDSIZE != 32) && (__WORDSIZE != 64)
#error "Unsupported machine word size !"
#endif /* !((__WORDSIZE != 32) && (__WORDSIZE != 64)) */

#define stroll_bops_bitsof(_expr) \
	(sizeof(_expr) * CHAR_BIT)

/**
 * Find First (least-significant) bit Set over 32 bits word.
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1, 0 when no set bits were found.
  */
static inline unsigned int __const __nothrow
stroll_bops32_ffs(uint32_t value)
{
	return (unsigned int)__builtin_ffs((int)value);
}

/**
 * Find First (least-significant) bit Set over 64 bits word.
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1, 0 when no set bits were found.
 */

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow
stroll_bops64_ffs(uint64_t value)
{
	return (unsigned int)__builtin_ffsl((long)value);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops64_ffs(uint64_t value)
{
	return (unsigned int)__builtin_ffsll((long long)value);
}

#endif

/**
 * Find First (least-significant) bit Set over a machine word.
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1, 0 when no set bits were found.
 */

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow
stroll_bops_ffs(unsigned long value)
{
	return stroll_bops64_ffs(value);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops_ffs(unsigned long value)
{
	return stroll_bops32_ffs(value);
}

#endif

/**
 * Find Last (most-significant) bit Set over 32 bits word.
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and a
 * @p value is zero, result is undefined. A zero @p value triggers an assertion
 * otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow
stroll_bops32_fls(uint32_t value)
{
	stroll_bops_assert_api(value);

	return 32 - (unsigned int)__builtin_clz(value);
}

/**
 * Find Last (most-significant) bit Set over 64 bits word.
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and a
 * @p value is zero, result is undefined. A zero @p value triggers an assertion
 * otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned int __stroll_const __stroll_nothrow
stroll_bops64_fls(uint64_t value)
{
	stroll_bops_assert_api(value);

	return 64 - (unsigned int)__builtin_clzl(value);
}

#elif __WORDSIZE == 32

static inline unsigned int __stroll_const __stroll_nothrow
stroll_bops64_fls(uint64_t value)
{
	stroll_bops_assert_api(value);

	return 64 - (unsigned int)__builtin_clzll(value);
}

#endif

/**
 * Find Last (most-significant) bit over a machine word
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and a
 * @p value is zero, result is undefined. A zero @p value triggers an assertion
 * otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned int __stroll_const __stroll_nothrow
stroll_bops_fls(unsigned long value)
{
	return stroll_bops64_fls(value);
}

#elif __WORDSIZE == 32

static inline unsigned int __stroll_const __stroll_nothrow
stroll_bops_fls(unsigned long value)
{
	return stroll_bops32_fls(value);
}

#endif

/**
 * Find First (least-significant) bit Cleared over 32 bits word.
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1, 0 when no cleared bits were found.
 */
static inline unsigned int __const __nothrow
stroll_bops32_ffc(uint32_t value)
{
	return stroll_bops32_ffs(value ^ ~(0U));
}

/**
 * Find First (least-significant) bit Cleared over 64 bits word.
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1, 0 when no cleared bits were found.
 */

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow
stroll_bops64_ffc(uint64_t value)
{
	return stroll_bops64_ffs(value ^ ~(0UL));
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops64_ffc(uint64_t value)
{
	return stroll_bops64_ffs(value ^ ~(0ULL));
}

#endif

/**
 * Find First (least-significant) bit Cleared over a machine word.
 *
 * @param[in] value word to search
 *
 * @return Index of bit starting from 1, 0 when no cleared bits were found.
 */

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow
stroll_bops_ffc(unsigned long value)
{
	return stroll_bops64_ffc(value);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops_ffc(unsigned long value)
{
	return stroll_bops32_ffc(value);
}

#endif

/**
 * Find the number of set bits over 32 bits word.
 *
 * @param[in] value word to search
 *
 * @return Number of set bits
 *
 * Returns the Hamming weight of a 32 bits word. The Hamming weight of a number
 * is the total number of bits set in it.
 */
static inline unsigned int __const __nothrow
stroll_bops32_hweight(uint32_t value)
{
	return (unsigned int)__builtin_popcount(value);
}

/**
 * Find the number of set bits over 64 bits word.
 *
 * @param[in] value word to search
 *
 * @return Number of set bits
 *
 * Returns the Hamming weight of a 64 bits word. The Hamming weight of a number
 * is the total number of bits set in it.
 */

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow
stroll_bops64_hweight(uint64_t value)
{
	return (unsigned int)__builtin_popcountl(value);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops64_hweight(uint64_t value)
{
	return (unsigned int)__builtin_popcountll(value);
}

#endif

/**
 * Find the number of set bits over a machine word.
 *
 * @param[in] value word to search
 *
 * @return Number of set bits
 *
 * Returns the Hamming weight of a word. The Hamming weight of a number is the
 * total number of bits set in it.
 */

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow
stroll_bops_hweight(unsigned long value)
{
	return stroll_bops64_hweight(value);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops_hweight(unsigned long value)
{
	return stroll_bops32_hweight(value);
}

#endif

#endif /* _STROLL_BOPS_H */
