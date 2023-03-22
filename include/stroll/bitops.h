/**
 * @file
 * Bit operations
 *
 * @author       Grégor Boirie <gregor.boirie@free.fr>
 * @date         29 Aug 2017
 * @copyright    Copyright (C) 2017-2023 Grégor Boirie.
 * @licensestart GNU Lesser General Public License (LGPL) v3
 *
 * This file is part of libstroll
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, If not, see <http://www.gnu.org/licenses/>.
 * @licenseend
 */
#ifndef _STROLL_BITOPS_H
#define _STROLL_BITOPS_H

#include <stroll/cdefs.h>
#include <stdint.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_bops_assert_api(_expr) \
	stroll_assert("stroll: bops", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_bops_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Find Last (most-significant) bit Set over 32 bits word.
 *
 * @param[in] mask word to search
 *
 * @return Index of bit starting from 1
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * a @p mask is zero, result is undefined. A zero @p mask triggers an assertion
 * otherwise.
 */
static inline unsigned int __stroll_const __nothrow
stroll_bops_fls32(uint32_t mask)
{
	stroll_bops_assert_api(mask);

	return (sizeof(mask) * CHAR_BIT) - __builtin_clz(mask);
}

/**
 * Find First (least-significant) bit Set over 32 bits word.
 *
 * @param[in] mask word to search
 *
 * @return Index of bit starting from 1
  */
static inline unsigned int __const __nothrow
stroll_bops_ffs32(uint32_t mask)
{
	return __builtin_ffs(mask);
}

/**
 * Find the number of set bits over 32 bits word.
 *
 * @param[in] mask word to search
 *
 * @return Number of set bits
 *
 * Returns the Hamming weight of a 32 bits word. The Hamming weight of a number
 * is the total number of bits set in it.
 */
static inline unsigned int __const __nothrow
stroll_bops_hweight32(uint32_t mask)
{
	return __builtin_popcount(mask);
}

#if (__WORDSIZE != 32) && (__WORDSIZE != 64)
#error "Unsupported machine word size !"
#endif /* !((__WORDSIZE != 32) && (__WORDSIZE != 64)) */

/**
 * Find Last (most-significant) bit Set over 64 bits word.
 *
 * @param[in] mask word to search
 *
 * @return Index of bit starting from 1
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * a @p mask is zero, result is undefined. A zero @p mask triggers an assertion
 * otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned int __stroll_const __nothrow
stroll_bops_fls64(uint64_t mask)
{
	stroll_bops_assert_api(mask);

	return (sizeof(mask) * CHAR_BIT) - __builtin_clzl(mask);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops_fls64(uint64_t mask)
{
	stroll_bops_assert_api(mask);

	return (sizeof(mask) * CHAR_BIT) - __builtin_clzll(mask);
}

#endif

/**
 * Find First (least-significant) bit Set over 64 bits word.
 *
 * @param[in] mask word to search
 *
 * @return Index of bit starting from 1
 */

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow
stroll_bops_ffs64(uint64_t mask)
{
	return __builtin_ffsl(mask);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops_ffs64(uint64_t mask)
{
	return __builtin_ffsll(mask);
}

#endif

/**
 * Find the number of set bits over 64 bits word.
 *
 * @param[in] mask word to search
 *
 * @return Number of set bits
 *
 * Returns the Hamming weight of a 64 bits word. The Hamming weight of a number
 * is the total number of bits set in it.
 */

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow
stroll_bops_hweight64(uint64_t mask)
{
	return __builtin_popcountl(mask);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow
stroll_bops_hweight64(uint64_t mask)
{
	return __builtin_popcountll(mask);
}

#endif

#define stroll_bops_bitsof(_expr) \
	(sizeof(_expr) * CHAR_BIT)

#define _stroll_bops_fls(_mask) \
	((stroll_bops_bitsof(_mask) == 32U) ? stroll_bops_fls32(_mask) : \
	                                      stroll_bops_fls64(_mask))

/**
 * Find Last (most-significant) bit over a word
 *
 * @param[in] _mask word to search
 *
 * @return Index of bit starting from 1
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * a @p mask is zero, result is undefined. A zero @p mask triggers an assertion
 * otherwise.
 */
#define stroll_bops_fls(_mask) \
	compile_eval((stroll_bops_bitsof(_mask) == 32U) || \
	             (stroll_bops_bitsof(_mask) == 64U), \
	             _stroll_bops_fls(_mask), \
	             "invalid bitops FLS word size")

#define _stroll_bops_ffs(_mask) \
	((stroll_bops_bitsof(_mask) == 32U) ? stroll_bops_ffs32(_mask) : \
	                                      stroll_bops_ffs64(_mask))

/**
 * Find First (least-significant) bit Set over a word.
 *
 * @param[in] _mask word to search
 *
 * @return Index of bit starting from 1
 */
#define stroll_bops_ffs(_mask) \
	compile_eval((stroll_bops_bitsof(_mask) == 32U) || \
	             (stroll_bops_bitsof(_mask) == 64U), \
	             _stroll_bops_ffs(_mask), \
	             "invalid bitops FFS word size")

#define _stroll_bops_hweight(_mask) \
	((stroll_bops_bitsof(_mask) == 32U) ? stroll_bops_hweight32(_mask) : \
	                                      stroll_bops_hweight64(_mask))

/**
 * Find the number of set bits over a word.
 *
 * @param[in] mask word to search
 *
 * @return Number of set bits
 *
 * Returns the Hamming weight of a word. The Hamming weight of a number is the
 * total number of bits set in it.
 */
#define stroll_bops_hweight(_mask) \
	compile_eval((stroll_bops_bitsof(_mask) == 32U) || \
	             (stroll_bops_bitsof(_mask) == 64U), \
	             _stroll_bops_hweight(_mask), \
	             "invalid bitops Hamming weight word size")

#endif /* _STROLL_BITOPS_H */
