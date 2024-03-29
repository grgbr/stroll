/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Power of 2 operations
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      25 Aug 2017
 * @copyright Copyright (C) 2017-2023 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_POW2_H
#define _STROLL_POW2_H

#include <stroll/bops.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_pow2_assert_api(_expr) \
	stroll_assert("stroll:pow2", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_pow2_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if (__WORDSIZE != 32) && (__WORDSIZE != 64)
#error "Unsupported machine word size !"
#endif /* !((__WORDSIZE != 32) && (__WORDSIZE != 64)) */

/**
 * Find the closest lower power of 2 of a 32 bits word.
 *
 * @param[in] value word
 *
 * @return closest lower power of 2
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p value is zero, result is undefined. A zero @p value triggers an
 * assertion otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow
stroll_pow2_low32(uint32_t value)
{
	stroll_pow2_assert_api(value);

	return stroll_bops_fls32(value) - 1;
}

/**
 * Find the closest lower power of 2 of a 64 bits word.
 *
 * @param[in] value word
 *
 * @return closest lower power of 2
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p value is zero, result is undefined. A zero @p value triggers an
 * assertion otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow
stroll_pow2_low64(uint64_t value)
{
	stroll_pow2_assert_api(value);

	return stroll_bops_fls64(value) - 1;
}

/**
 * Find the closest lower power of 2 of an integer.
 *
 * @param[in] value integer
 *
 * @return closest lower power of 2
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p value is zero, result is undefined. A zero @p value triggers an
 * assertion otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow
stroll_pow2_low(unsigned int value)
{
	return stroll_pow2_low32(value);
}

/**
 * Find the closest lower power of 2 of a machine word.
 *
 * @param[in] value word
 *
 * @return closest lower power of 2
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p value is zero, result is undefined. A zero @p value triggers an
 * assertion otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned int __stroll_const __stroll_nothrow
stroll_pow2_lowul(unsigned long value)
{
	return stroll_pow2_low64(value);
}

#elif __WORDSIZE == 32

static inline unsigned int __stroll_const __stroll_nothrow
stroll_pow2_lowul(unsigned long value)
{
	return stroll_pow2_low32(value);
}

#endif

/**
 * Find the closest upper power of 2 of a 32 bits word.
 *
 * @param[in] value word
 *
 * @return closest upper power of 2
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p value is zero, result is undefined. A zero @p value triggers an
 * assertion otherwise.
 */
extern unsigned int
stroll_pow2_up32(uint32_t value) __stroll_const __stroll_nothrow __leaf;

/**
 * Find the closest upper power of 2 of a 64 bits word.
 *
 * @param[in] value word
 *
 * @return closest upper power of 2
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p value is zero, result is undefined. A zero @p value triggers an
 * assertion otherwise.
 */
extern unsigned int
stroll_pow2_up64(uint64_t value) __stroll_const __stroll_nothrow __leaf;

/**
 * Find the closest upper power of 2 of an integer.
 *
 * @param[in] value integer
 *
 * @return closest upper power of 2
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p value is zero, result is undefined. A zero @p value triggers an
 * assertion otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow
stroll_pow2_up(unsigned int value)
{
	return stroll_pow2_up32(value);
}

/**
 * Find the closest upper power of 2 of a machine word.
 *
 * @param[in] value word
 *
 * @return closest upper power of 2
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p value is zero, result is undefined. A zero @p value triggers an
 * assertion otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned int __stroll_const __stroll_nothrow
stroll_pow2_upul(unsigned long value)
{
	return stroll_pow2_up64(value);
}

#elif __WORDSIZE == 32

static inline unsigned int __stroll_const __stroll_nothrow
stroll_pow2_upul(unsigned long value)
{
	return stroll_pow2_up32(value);
}

#endif

#endif /* _STROLL_POW2_H */
