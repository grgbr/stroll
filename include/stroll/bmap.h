/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Bitmap interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      29 Aug 2017
 * @copyright Copyright (C) 2017-2023 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_BMAP_H
#define _STROLL_BMAP_H

#include <stroll/bops.h>
#include <stdbool.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_bmap_assert_api(_expr) \
	stroll_assert("stroll:bmap", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_bmap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Compute a 32-bits word mask.
 *
 * @param[in] start_bit Index of first mask bit set starting from 0
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Computed mask
 *
 * Compute a 32-bits mask where all bits are set starting from bit which index
 * is @p start_bit to `start_bit + bit_count - 1`. Bit indices start from 0.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint32_t __stroll_const __stroll_nothrow __warn_result
stroll_bmap_mask32(unsigned int start_bit, unsigned int bit_count)
{
	/*
	 * Intel right shift instruction cannot shift more than the number of
	 * available register bits minus one, i.e., here no more than 31 bits.
	 */
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return (UINT32_MAX >> (32 - bit_count)) << start_bit;
}

/**
 * Compute an integer word mask.
 *
 * @param[in] start_bit Index of first mask bit set starting from 0
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Computed mask
 *
 * Compute an integer mask where all bits are set starting from bit which index
 * is @p start_bit to `start_bit + bit_count - 1`. Bit indices start from 0.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow __warn_result
stroll_bmap_mask(unsigned int start_bit, unsigned int bit_count)
{
	return stroll_bmap_mask32(start_bit, bit_count);
}

/**
 * Compute a 64-bits word mask.
 *
 * @param[in] start_bit Index of first mask bit set starting from 0
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Computed mask
 *
 * Compute a 64-bits mask where all bits are set starting from bit which index
 * is @p start_bit to `start_bit + bit_count - 1`. Bit indices start from 0.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint64_t __stroll_const __stroll_nothrow __warn_result
stroll_bmap_mask64(unsigned int start_bit, unsigned int bit_count)
{
	/*
	 * Intel right shift instruction cannot shift more than the number of
	 * available register bits minus one, i.e., here no more than 31 bits.
	 */
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return (UINT64_MAX >> (64 - bit_count)) << start_bit;
}

/**
 * Compute a machine word mask.
 *
 * @param[in] start_bit Index of first mask bit set starting from 0
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Computed mask
 *
 * Compute a mask over a machine word where all bits are set starting from bit
 * which index is @p start_bit to `start_bit + bit_count - 1`. Bit indices
 * starts from 0.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be less than the maximum number of
 *   bits contained within a machine word. If not, an undefined result is
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned long __stroll_const __stroll_nothrow __warn_result
stroll_bmap_maskul(unsigned int start_bit, unsigned int bit_count)
{
	return stroll_bmap_mask64(start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline unsigned long __stroll_const __stroll_nothrow __warn_result
stroll_bmap_maskul(unsigned int start_bit, unsigned int bit_count)
{
	return stroll_bmap_mask32(start_bit, bit_count);
}

#endif

/**
 * Compute the number of bits set over a 32-bits bitmap.
 *
 * @param[in] bmap Bitmap to search
 *
 * @return Number of bits set
 *
 * Returns the Hamming weight of a 32-bits bitmap. The Hamming weight of a
 * number is the total number of bits set in it.
 */
static inline unsigned int __const __nothrow __warn_result
stroll_bmap_hweight32(uint32_t bmap)
{
	return stroll_bops_hweight32(bmap);
}

/**
 * Compute the number of bits set over an integer.
 *
 * @param[in] bmap Bitmap to search
 *
 * @return Number of bits set
 *
 * Returns the Hamming weight of an integer bitmap. The Hamming weight of a
 * number is the total number of bits set in it.
 */

static inline unsigned int __const __nothrow __warn_result
stroll_bmap_hweight(unsigned int bmap)
{
	return stroll_bops_hweight(bmap);
}

/**
 * Compute the number of bits set over a 64-bits bitmap.
 *
 * @param[in] bmap Bitmap to search
 *
 * @return Number of bits set
 *
 * Returns the Hamming weight of a 64-bits bitmap. The Hamming weight of a
 * number is the total number of bits set in it.
 */
static inline unsigned int __const __nothrow __warn_result
stroll_bmap_hweight64(uint64_t bmap)
{
	return stroll_bops_hweight64(bmap);
}

/**
 * Compute the number of bits set over a machine word bitmap.
 *
 * @param[in] bmap Bitmap to search
 *
 * @return Number of bits set
 *
 * Returns the Hamming weight of a machine word bitmap. The Hamming weight of a
 * number is the total number of bits set in it.
 */

static inline unsigned int __const __nothrow __warn_result
stroll_bmap_hweightul(unsigned long bmap)
{
	return stroll_bops_hweightul(bmap);
}

/**
 * Perform bitwise mask based AND operation on 32-bits word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap & mask`
 */
static inline uint32_t __const __nothrow __warn_result
stroll_bmap_and32(uint32_t bmap, uint32_t mask)
{
	return bmap & mask;
}

/**
 * Perform bitwise mask based AND operation on integer bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap & mask`
 */
static inline unsigned int __const __nothrow __warn_result
stroll_bmap_and(unsigned int bmap, unsigned int mask)
{
	return stroll_bmap_and32(bmap, mask);
}

/**
 * Perform bitwise mask based AND operation on 64-bits word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap & mask`
 */
static inline uint64_t __const __nothrow __warn_result
stroll_bmap_and64(uint64_t bmap, uint64_t mask)
{
	return bmap & mask;
}

/**
 * Perform bitwise mask based AND operation on machine word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap & mask`
 */

#if __WORDSIZE == 64

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_andul(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap_and64(bmap, mask);
}

#elif __WORDSIZE == 32

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_andul(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap_and32(bmap, mask);
}

#endif

/**
 * Perform bitwise range based AND operation on 32-bits word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise AND operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise AND operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask32().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint32_t __stroll_const __stroll_nothrow __warn_result
stroll_bmap_and_range32(uint32_t     bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap_and32(bmap,
	                         stroll_bmap_mask32(start_bit, bit_count));
}

/**
 * Perform bitwise range based AND operation on integer bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise AND operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise AND operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow __warn_result
stroll_bmap_and_range(unsigned int bmap,
                      unsigned int start_bit,
                      unsigned int bit_count)
{
	return stroll_bmap_and_range32(bmap, start_bit, bit_count);
}

/**
 * Perform bitwise range based AND operation on 64-bits word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise AND operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise AND operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask64().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint64_t __stroll_const __stroll_nothrow __warn_result
stroll_bmap_and_range64(uint64_t     bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return stroll_bmap_and64(bmap,
	                         stroll_bmap_mask64(start_bit, bit_count));
}

/**
 * Perform bitwise range based AND operation on machine word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise AND operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise AND operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_maskul().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be less than the maximum number of
 *   bits contained within a machine word. If not, an undefined result is
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
#if __WORDSIZE == 64

static inline unsigned long __stroll_const __stroll_nothrow __warn_result
stroll_bmap_and_rangeul(unsigned long bmap,
                        unsigned int  start_bit,
                        unsigned int  bit_count)
{
	return stroll_bmap_and_range64(bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline unsigned long __stroll_const __stroll_nothrow __warn_result
stroll_bmap_and_rangeul(unsigned long bmap,
                        unsigned int  start_bit,
                        unsigned int  bit_count)
{
	return stroll_bmap_and_range32(bmap, start_bit, bit_count);
}

#endif

/**
 * Perform bitwise mask based OR operation on 32-bits word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap | mask`
 */
static inline uint32_t __const __nothrow __warn_result
stroll_bmap_or32(uint32_t bmap, uint32_t mask)
{
	return bmap | mask;
}

/**
 * Perform bitwise mask based OR operation on 64-bits word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap | mask`
 */
static inline uint64_t __const __nothrow __warn_result
stroll_bmap_or64(uint64_t bmap, uint64_t mask)
{
	return bmap | mask;
}

/**
 * Perform bitwise mask based OR operation on integer bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap | mask`
 */
static inline unsigned int __const __nothrow __warn_result
stroll_bmap_or(unsigned int bmap, unsigned int mask)
{
	return stroll_bmap_or32(bmap, mask);
}

/**
 * Perform bitwise mask based OR operation on machine word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap | mask`
 */

#if __WORDSIZE == 64

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_orul(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap_or64(bmap, mask);
}

#elif __WORDSIZE == 32

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_orul(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap_or32(bmap, mask);
}

#endif

/**
 * Perform bitwise range based OR operation on 32-bits word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise OR operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise OR operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask32().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint32_t __stroll_const __stroll_nothrow __warn_result
stroll_bmap_or_range32(uint32_t     bmap,
                       unsigned int start_bit,
                       unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap_or32(bmap, stroll_bmap_mask32(start_bit, bit_count));
}

/**
 * Perform bitwise range based OR operation on integer bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise OR operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise OR operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow __warn_result
stroll_bmap_or_range(unsigned int bmap,
                     unsigned int start_bit,
                     unsigned int bit_count)
{
	return stroll_bmap_or_range32(bmap, start_bit, bit_count);
}

/**
 * Perform bitwise range based OR operation on 64-bits word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise OR operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise OR operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask64().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint64_t __stroll_const __stroll_nothrow __warn_result
stroll_bmap_or_range64(uint64_t     bmap,
                       unsigned int start_bit,
                       unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return stroll_bmap_or64(bmap, stroll_bmap_mask64(start_bit, bit_count));
}

/**
 * Perform bitwise range based OR operation on machine word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise OR operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise OR operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_maskul().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be less than the maximum number of
 *   bits contained within a machine word. If not, an undefined result is
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is disabled. An
 *   assertion is triggered otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned long __stroll_const __stroll_nothrow __warn_result
stroll_bmap_or_rangeul(unsigned long bmap,
                       unsigned int  start_bit,
                       unsigned int  bit_count)
{
	return stroll_bmap_or_range64(bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline unsigned long __stroll_const __stroll_nothrow __warn_result
stroll_bmap_or_rangeul(unsigned long bmap,
                       unsigned int  start_bit,
                       unsigned int  bit_count)
{
	return stroll_bmap_or_range32(bmap, start_bit, bit_count);
}

#endif

/**
 * Perform bitwise mask based XOR operation on 32-bits word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap ^ mask`
 */
static inline uint32_t __const __nothrow __warn_result
stroll_bmap_xor32(uint32_t bmap, uint32_t mask)
{
	return bmap ^ mask;
}

/**
 * Perform bitwise mask based XOR operation on integer bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap ^ mask`
 */
static inline unsigned int __const __nothrow __warn_result
stroll_bmap_xor(unsigned int bmap, unsigned int mask)
{
	return stroll_bmap_xor32(bmap, mask);
}

/**
 * Perform bitwise mask based XOR operation on 64-bits word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap ^ mask`
 */
static inline uint64_t __const __nothrow __warn_result
stroll_bmap_xor64(uint64_t bmap, uint64_t mask)
{
	return bmap ^ mask;
}

/**
 * Perform bitwise mask based XOR operation on machine word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap ^ mask`
 */

#if __WORDSIZE == 64

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_xorul(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap_xor64(bmap, mask);
}

#elif __WORDSIZE == 32

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_xorul(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap_xor32(bmap, mask);
}

#endif

/**
 * Perform bitwise range based XOR operation on 32-bits word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise XOR operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise XOR operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask32().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint32_t __stroll_const __stroll_nothrow __warn_result
stroll_bmap_xor_range32(uint32_t     bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap_xor32(bmap,
	                         stroll_bmap_mask32(start_bit, bit_count));
}

/**
 * Perform bitwise range based XOR operation on integer bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise XOR operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise XOR operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline unsigned int __stroll_const __stroll_nothrow __warn_result
stroll_bmap_xor_range(unsigned int bmap,
                      unsigned int start_bit,
                      unsigned int bit_count)
{
	return stroll_bmap_xor_range32(bmap, start_bit, bit_count);
}

/**
 * Perform bitwise range based XOR operation on 64-bits word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise XOR operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise XOR operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_mask64().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint64_t __stroll_const __stroll_nothrow __warn_result
stroll_bmap_xor_range64(uint64_t     bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return stroll_bmap_xor64(bmap,
	                         stroll_bmap_mask64(start_bit, bit_count));
}

/**
 * Perform bitwise range based XOR operation on machine word bitmap.
 *
 * @param[in] bmap      Bitmap to perform operation on
 * @param[in] start_bit Index of first mask bit set
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Result of the bitwise XOR operation
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument then use it to perform a bitwise XOR operation over given @p bmap
 * bitmap.
 *
 * Mask is computed thanks to stroll_bmap_maskul().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be less than the maximum number of
 *   bits contained within a machine word. If not, an undefined result is
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is disabled. An
 *   assertion is triggered otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned long __stroll_const __stroll_nothrow __warn_result
stroll_bmap_xor_rangeul(unsigned long bmap,
                        unsigned int  start_bit,
                        unsigned int  bit_count)
{
	return stroll_bmap_xor_range64(bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline unsigned long __stroll_const __stroll_nothrow __warn_result
stroll_bmap_xor_rangeul(unsigned long bmap,
                        unsigned int  start_bit,
                        unsigned int  bit_count)
{
	return stroll_bmap_xor_range32(bmap, start_bit, bit_count);
}

#endif

/**
 * Test wether bits in a 32-bits word bitmap are set or not.
 *
 * @param[in] bmap Bitmap to test
 * @param[in] mask The set of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Test wether a set of bits specified by @p mask are nonzero into @p bmap.
 */
static inline bool __const __nothrow __warn_result
stroll_bmap_test_mask32(uint32_t bmap, uint32_t mask)
{
	return !!stroll_bmap_and32(bmap, mask);
}

/**
 * Test wether bits in a integer bitmap are set or not.
 *
 * @param[in] bmap Bitmap to test
 * @param[in] mask The set of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Test wether a set of bits specified by @p mask are nonzero into @p bmap.
 */
static inline bool __const __nothrow __warn_result
stroll_bmap_test_mask(unsigned int bmap, unsigned int mask)
{
	return stroll_bmap_test_mask32(bmap, mask);
}

/**
 * Test wether bits in a 64-bits word bitmap are set or not.
 *
 * @param[in] bmap Bitmap to test
 * @param[in] mask The set of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Test wether a set of bits specified by @p mask are nonzero into @p bmap.
 */
static inline bool __const __nothrow __warn_result
stroll_bmap_test_mask64(uint64_t bmap, uint64_t mask)
{
	return !!stroll_bmap_and64(bmap, mask);
}

/**
 * Test wether bits in a machine word bitmap are set or not.
 *
 * @param[in] bmap Bitmap to test
 * @param[in] mask The set of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Test wether a set of bits specified by @p mask are nonzero into @p bmap.
 */
#if __WORDSIZE == 64

static inline bool __const __nothrow __warn_result
stroll_bmap_test_maskul(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap_test_mask64(bmap, mask);
}

#elif __WORDSIZE == 32

static inline bool __const __nothrow __warn_result
stroll_bmap_test_maskul(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap_test_mask32(bmap, mask);
}

#endif

/**
 * Test wether a bit in a 32-bits word bitmap is set or not.
 *
 * @param[in] bmap   Bitmap to test
 * @param[in] bit_no Index of bit to test starting from 0
 *
 * @return Test result
 * @retval true  tested bit is set
 * @retval false tested bit is cleared
 *
 * Test wether the bit specified by @p bit_no is set into @p bmap bitmap.
 * Bit indices start from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_test32(uint32_t bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bit_no < stroll_bops_bitsof(bmap));

	return stroll_bmap_test_mask32(bmap, UINT32_C(1) << bit_no);
}

/**
 * Test wether a bit in a integer bitmap is set or not.
 *
 * @param[in] bmap   Bitmap to test
 * @param[in] bit_no Index of bit to test starting from 0
 *
 * @return Test result
 * @retval true  tested bit is set
 * @retval false tested bit is cleared
 *
 * Test wether the bit specified by @p bit_no is set into @p bmap bitmap.
 * Bit indices start from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_test(unsigned int bmap, unsigned int bit_no)
{
	return stroll_bmap_test32(bmap, bit_no);
}

/**
 * Test wether a bit in a 64-bits word bitmap is set or not.
 *
 * @param[in] bmap   Bitmap to test
 * @param[in] bit_no Index of bit to test starting from 0
 *
 * @return Test result
 * @retval true  tested bit is set
 * @retval false tested bit is cleared
 *
 * Test wether the bit specified by @p bit_no is set into @p bmap bitmap.
 * Bit indices start from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 64`, result is undefined. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_test64(uint64_t bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bit_no < stroll_bops_bitsof(bmap));

	return stroll_bmap_test_mask64(bmap, UINT64_C(1) << bit_no);
}

/**
 * Test wether a bit in a machine word bitmap is set or not.
 *
 * @param[in] bmap   Bitmap to test
 * @param[in] bit_no Index of bit to test starting from 0
 *
 * @return Test result
 * @retval true  tested bit is set
 * @retval false tested bit is cleared
 *
 * Test wether the bit specified by @p bit_no is set into @p bmap bitmap.
 * Bit indices start from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the maximum number of bits contained
 * within a machine word, result is undefined. An assertion is triggered
 * otherwise.
 */

#if __WORDSIZE == 64

static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_testul(unsigned long bmap, unsigned int bit_no)
{
	return stroll_bmap_test64(bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_testul(unsigned long bmap, unsigned int bit_no)
{
	return stroll_bmap_test32(bmap, bit_no);
}

#endif

/**
 * Test wether a range of bits in a 32-bits word bitmap are set or not.
 *
 * @param[in] bmap      Bitmap to test
 * @param[in] start_bit Index of first bit to test
 * @param[in] bit_count Number of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, check wether, within the given range, there are bits set into
 * the @p bmap bitmap or not.
 *
 * Mask is computed thanks to stroll_bmap_mask32().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_test_range32(uint32_t     bmap,
                         unsigned int start_bit,
                         unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap_test_mask32(bmap, stroll_bmap_mask32(start_bit,
	                                                        bit_count));
}

/**
 * Test wether a range of bits in a integer bitmap are set or not.
 *
 * @param[in] bmap      Bitmap to test
 * @param[in] start_bit Index of first bit to test
 * @param[in] bit_count Number of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, check wether, within the given range, there are bits set into
 * the @p bmap bitmap or not.
 *
 * Mask is computed thanks to stroll_bmap_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_test_range(unsigned int bmap,
                       unsigned int start_bit,
                       unsigned int bit_count)
{
	return stroll_bmap_test_range32(bmap, start_bit, bit_count);
}

/**
 * Test wether a range of bits in a 64-bits word bitmap are set or not.
 *
 * @param[in] bmap      Bitmap to test
 * @param[in] start_bit Index of first bit to test
 * @param[in] bit_count Number of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, check wether, within the given range, there are bits set into
 * the @p bmap bitmap or not.
 *
 * Mask is computed thanks to stroll_bmap_mask64().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_test_range64(uint64_t     bmap,
                         unsigned int start_bit,
                         unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return stroll_bmap_test_mask64(bmap, stroll_bmap_mask64(start_bit,
	                                                        bit_count));
}

/**
 * Test wether a range of bits in a machine word bitmap are set or not.
 *
 * @param[in] bmap      Bitmap to test
 * @param[in] start_bit Index of first bit to test
 * @param[in] bit_count Number of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, check wether, within the given range, there are bits set into
 * the @p bmap bitmap or not.
 *
 * Mask is computed thanks to stroll_bmap_maskul().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be less than the maximum number of
 *   bits contained within a machine word. If not, an undefined result is
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is disabled. An
 *   assertion is triggered otherwise.
 */

#if __WORDSIZE == 64

static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_test_rangeul(unsigned long bmap,
                         unsigned int  start_bit,
                         unsigned int  bit_count)
{
	return stroll_bmap_test_range64(bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline bool __stroll_const __stroll_nothrow __warn_result
stroll_bmap_test_rangeul(unsigned long bmap,
                         unsigned int  start_bit,
                         unsigned int  bit_count)
{
	return stroll_bmap_test_range32(bmap, start_bit, bit_count);
}

#endif

/**
 * Test wether a 32-bits word bitmap contains bits set or not.
 *
 * @param[in] bmap Bitmap to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 */
static inline bool __const __nothrow __warn_result
stroll_bmap_test_all32(uint32_t bmap)
{
	return !!bmap;
}

/**
 * Test wether a integer bitmap contains bits set or not.
 *
 * @param[in] bmap Bitmap to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 */
static inline bool __const __nothrow __warn_result
stroll_bmap_test_all(unsigned int bmap)
{
	return stroll_bmap_test_all32(bmap);
}

/**
 * Test wether a 64-bits word bitmap contains bits set or not.
 *
 * @param[in] bmap Bitmap to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 */
static inline bool __const __nothrow __warn_result
stroll_bmap_test_all64(uint64_t bmap)
{
	return !!bmap;
}

/**
 * Test wether a machine word bitmap contains bits set or not.
 *
 * @param[in] bmap Bitmap to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 */

#if __WORDSIZE == 64

static inline bool __const __nothrow __warn_result
stroll_bmap_test_allul(unsigned long bmap)
{
	return stroll_bmap_test_all64(bmap);
}

#elif __WORDSIZE == 32

static inline bool __const __nothrow __warn_result
stroll_bmap_test_allul(unsigned long bmap)
{
	return stroll_bmap_test_all32(bmap);
}

#endif

/**
 * Set bits of 32-bits word bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to set
 * @param[in]    mask Mask of bits to set
 *
 * Set all bits of @p bmap bitmap which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_mask32(uint32_t * __restrict bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap_or32(*bmap, mask);
}

/**
 * Set bits of integer bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to set
 * @param[in]    mask Mask of bits to set
 *
 * Set all bits of @p bmap bitmap which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_mask(unsigned int * __restrict bmap, unsigned int mask)
{
	return stroll_bmap_set_mask32(bmap, mask);
}

/**
 * Set bits of 64-bits word bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to set
 * @param[in]    mask Mask of bits to set
 *
 * Set all bits of @p bmap bitmap which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_mask64(uint64_t * __restrict bmap, uint64_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap_or64(*bmap, mask);
}

/**
 * Set bits of machine word bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to set
 * @param[in]    mask Mask of bits to set
 *
 * Set all bits of @p bmap bitmap which are set into @p mask.
 */
#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_maskul(unsigned long * __restrict bmap, unsigned long mask)
{
	stroll_bmap_set_mask64((uint64_t *)bmap, mask);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_maskul(unsigned long * __restrict bmap, unsigned long mask)
{
	stroll_bmap_set_mask32((uint32_t *)bmap, mask);
}

#endif

/**
 * Set a bit in a 32-bits word bitmap.
 *
 * @param[inout] bmap   Bitmap to set
 * @param[in]    bit_no Index of bit to set
 *
 * Set bit specified by @p bit_no into @p bmap bitmap. Bit indices start from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set32(uint32_t * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap_set_mask32(bmap, UINT32_C(1) << bit_no);
}

/**
 * Set a bit in a integer bitmap.
 *
 * @param[inout] bmap   Bitmap to set
 * @param[in]    bit_no Index of bit to set
 *
 * Set bit specified by @p bit_no into @p bmap bitmap. Bit indices start from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set(unsigned int * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_set32((uint32_t *)bmap, bit_no);
}

/**
 * Set a bit in a 64-bits word bitmap.
 *
 * @param[inout] bmap   Bitmap to set
 * @param[in]    bit_no Index of bit to set
 *
 * Set bit specified by @p bit_no into @p bmap bitmap. Bit indices start from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 64`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set64(uint64_t * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 64);

	stroll_bmap_set_mask64(bmap, UINT64_C(1) << bit_no);
}

/**
 * Set a bit in a machine word bitmap.
 *
 * @param[inout] bmap   Bitmap to set
 * @param[in]    bit_no Index of bit to set
 *
 * Set bit specified by @p bit_no into @p bmap bitmap. Bit indices start from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the maximum number of bits contained
 * within a machine word, result is undefined. An assertion is triggered
 * otherwise.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setul(unsigned long * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_set64((uint64_t *)bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setul(unsigned long * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_set32((uint32_t *)bmap, bit_no);
}

#endif

/**
 * Set bits in a 32-bits word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap to set
 * @param[in]    start_bit Index of first bit to set
 * @param[in]    bit_count Number of bits to set
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, set all bits in @p bmap bitmap which are set into this
 * computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask32().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_range32(uint32_t *  __restrict bmap,
                        unsigned int           start_bit,
                        unsigned int           bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	stroll_bmap_set_mask32(bmap, stroll_bmap_mask32(start_bit, bit_count));
}

/**
 * Set bits in a integer bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap to set
 * @param[in]    start_bit Index of first bit to set
 * @param[in]    bit_count Number of bits to set
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, set all bits in @p bmap bitmap which are set into this
 * computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_range(unsigned int * __restrict bmap,
                      unsigned int              start_bit,
                      unsigned int              bit_count)
{
	stroll_bmap_set_range32((uint32_t *)bmap, start_bit, bit_count);
}

/**
 * Set bits in a 64-bits word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap to set
 * @param[in]    start_bit Index of first bit to set
 * @param[in]    bit_count Number of bits to set
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, set all bits in @p bmap bitmap which are set into this
 * computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask64().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_range64(uint64_t * __restrict bmap,
                        unsigned int          start_bit,
                        unsigned int          bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	stroll_bmap_set_mask64(bmap, stroll_bmap_mask64(start_bit, bit_count));
}

/**
 * Set bits in a machine word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap to set
 * @param[in]    start_bit Index of first bit to set
 * @param[in]    bit_count Number of bits to set
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, set all bits in @p bmap bitmap which are set into this
 * computed mask.
 *
 * Mask is computed thanks to stroll_bmap_maskul().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be less than the maximum number of
 *   bits contained within a machine word. If not, an undefined result is
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is disabled. An
 *   assertion is triggered otherwise.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_rangeul(unsigned long * __restrict bmap,
                        unsigned int               start_bit,
                        unsigned int               bit_count)
{
	stroll_bmap_set_range64((uint64_t *)bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_rangeul(unsigned long * __restrict bmap,
                        unsigned int               start_bit,
                        unsigned int               bit_count)
{
	stroll_bmap_set_range32((uint32_t *)bmap, start_bit, bit_count);
}

#endif

/**
 * Set all bits of a 32-bits word bitmap.
 *
 * @param[out] bmap Bitmap to set
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_all32(uint32_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = UINT32_MAX;
}

/**
 * Set all bits of a integer bitmap.
 *
 * @param[out] bmap Bitmap to set
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_all(unsigned int * __restrict bmap)
{
	stroll_bmap_set_all32((uint32_t *)bmap);
}

/**
 * Set all bits of a 64-bits word bitmap.
 *
 * @param[out] bmap Bitmap to set
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_all64(uint64_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = UINT64_MAX;
}

/**
 * Set all bits of a machine word bitmap.
 *
 * @param[out] bmap Bitmap to set
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_allul(unsigned long * __restrict bmap)
{
	stroll_bmap_set_all64((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_set_allul(unsigned long * __restrict bmap)
{
	stroll_bmap_set_all32((uint32_t *)bmap);
}

#endif

/**
 * Clear bits of 32-bits word bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to clear
 * @param[in]    mask Mask of bits to clear
 *
 * Clear all bits of @p bmap bitmap which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_mask32(uint32_t * __restrict bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap_and32(*bmap, ~mask);
}

/**
 * Clear bits of integer bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to clear
 * @param[in]    mask Mask of bits to clear
 *
 * Clear all bits of @p bmap bitmap which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_mask(unsigned int * __restrict bmap, unsigned int mask)
{
	return stroll_bmap_clear_mask32((uint32_t *)bmap, mask);
}

/**
 * Clear bits of 64-bits word bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to clear
 * @param[in]    mask Mask of bits to clear
 *
 * Clear all bits of @p bmap bitmap which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_mask64(uint64_t * __restrict bmap, uint64_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap_and64(*bmap, ~mask);
}

/**
 * Clear bits of machine word bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to clear
 * @param[in]    mask Mask of bits to clear
 *
 * Clear all bits of @p bmap bitmap which are set into @p mask.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_maskul(unsigned long * __restrict bmap, unsigned long mask)
{
	return stroll_bmap_clear_mask64((uint64_t *)bmap, mask);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_maskul(unsigned long * __restrict bmap, unsigned long mask)
{
	return stroll_bmap_clear_mask32((uint32_t *)bmap, mask);
}

#endif

/**
 * Clear a bit in a 32-bits word bitmap.
 *
 * @param[inout] bmap   Bitmap to clear
 * @param[in]    bit_no Index of bit to clear
 *
 * Clear bit specified by @p bit_no into @p bmap bitmap. Bit indices start from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear32(uint32_t * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap_clear_mask32(bmap, UINT32_C(1) << bit_no);
}

/**
 * Clear a bit in a integer bitmap.
 *
 * @param[inout] bmap   Bitmap to clear
 * @param[in]    bit_no Index of bit to clear
 *
 * Clear bit specified by @p bit_no into @p bmap bitmap. Bit indices start from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear(unsigned int * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_clear32((uint32_t *)bmap, bit_no);
}

/**
 * Clear a bit in a 64-bits word bitmap.
 *
 * @param[inout] bmap   Bitmap to clear
 * @param[in]    bit_no Index of bit to clear
 *
 * Clear bit specified by @p bit_no into @p bmap bitmap. Bit indices start from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 64`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear64(uint64_t * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 64);

	stroll_bmap_clear_mask64(bmap, UINT64_C(1) << bit_no);
}

/**
 * Clear a bit in a machine word bitmap.
 *
 * @param[inout] bmap   Bitmap to clear
 * @param[in]    bit_no Index of bit to clear
 *
 * Clear bit specified by @p bit_no into @p bmap bitmap. Bit indices start from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the maximum number of bits contained
 * within a machine word, result is undefined. An assertion is triggered
 * otherwise.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clearul(unsigned long * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_clear64((uint64_t *)bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clearul(unsigned long * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_clear32((uint32_t *)bmap, bit_no);
}

#endif

/**
 * Clear bits in a 32-bits word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap to clear
 * @param[in]    start_bit Index of first bit to clear
 * @param[in]    bit_count Number of bits to clear
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, clear all bits in @p bmap bitmap which are set into this
 * computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask32().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_range32(uint32_t * __restrict bmap,
                          unsigned int          start_bit,
                          unsigned int          bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	stroll_bmap_clear_mask32(bmap,
	                         stroll_bmap_mask32(start_bit, bit_count));
}

/**
 * Clear bits in a integer bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap to clear
 * @param[in]    start_bit Index of first bit to clear
 * @param[in]    bit_count Number of bits to clear
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, clear all bits in @p bmap bitmap which are set into this
 * computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_range(unsigned int * __restrict bmap,
                        unsigned int              start_bit,
                        unsigned int              bit_count)
{
	stroll_bmap_clear_range32((uint32_t *)bmap, start_bit, bit_count);
}

/**
 * Clear bits in a 64-bits word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap to clear
 * @param[in]    start_bit Index of first bit to clear
 * @param[in]    bit_count Number of bits to clear
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, clear all bits in @p bmap bitmap which are set into this
 * computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask64().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_range64(uint64_t * __restrict bmap,
                          unsigned int          start_bit,
                          unsigned int          bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	stroll_bmap_clear_mask64(bmap,
	                         stroll_bmap_mask64(start_bit, bit_count));
}

/**
 * Clear bits in a machine word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap to clear
 * @param[in]    start_bit Index of first bit to clear
 * @param[in]    bit_count Number of bits to clear
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, clear all bits in @p bmap bitmap which are set into this
 * computed mask.
 *
 * Mask is computed thanks to stroll_bmap_maskul().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be less than the maximum number of
 *   bits contained within a machine word. If not, an undefined result is
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is disabled. An
 *   assertion is triggered otherwise.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_rangeul(unsigned long * __restrict bmap,
                          unsigned int               start_bit,
                          unsigned int               bit_count)
{
	stroll_bmap_clear_range64((uint64_t *)bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_rangeul(unsigned long * __restrict bmap,
                          unsigned int               start_bit,
                          unsigned int               bit_count)
{
	stroll_bmap_clear_range32((uint32_t *)bmap, start_bit, bit_count);
}

#endif

/**
 * Clear all bits of a 32-bits word bitmap.
 *
 * @param[out] bmap Bitmap to clear
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_all32(uint32_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = 0;
}

/**
 * Clear all bits of a integer bitmap.
 *
 * @param[out] bmap Bitmap to clear
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_all(unsigned int * __restrict bmap)
{
	stroll_bmap_clear_all32((uint32_t *)bmap);
}

/**
 * Clear all bits of a 64-bits word bitmap.
 *
 * @param[out] bmap Bitmap to clear
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_all64(uint64_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = 0;
}

/**
 * Clear all bits of a machine word bitmap.
 *
 * @param[out] bmap Bitmap to clear
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_allul(unsigned long * __restrict bmap)
{
	stroll_bmap_clear_all64((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_clear_allul(unsigned long * __restrict bmap)
{
	stroll_bmap_clear_all32((uint32_t *)bmap);
}

#endif

/**
 * Toggle state of bits of a 32-bits word bitmap according to given mask.
 *
 * @param[inout] bmap Bitmap which bits to toggle
 * @param[in]    mask Mask of bits to toggle
 *
 * Invert value of @p bmap bitmap bits which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_mask32(uint32_t * __restrict bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap_xor32(*bmap, mask);
}

/**
 * Toggle state of bits of a integer bitmap according to given mask.
 *
 * @param[inout] bmap Bitmap which bits to toggle
 * @param[in]    mask Mask of bits to toggle
 *
 * Invert value of @p bmap bitmap bits which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_mask(unsigned int * __restrict bmap, unsigned int mask)
{
	stroll_bmap_toggle_mask32((uint32_t *)bmap, mask);
}

/**
 * Toggle state of bits of a 64-bits word bitmap according to given mask.
 *
 * @param[inout] bmap Bitmap which bits to toggle
 * @param[in]    mask Mask of bits to toggle
 *
 * Invert value of @p bmap bitmap bits which are set into @p mask.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_mask64(uint64_t * __restrict bmap, uint64_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap_xor64(*bmap, mask);
}

/**
 * Toggle state of bits of a machine word bitmap according to given mask.
 *
 * @param[inout] bmap Bitmap which bits to toggle
 * @param[in]    mask Mask of bits to toggle
 *
 * Invert value of @p bmap bitmap bits which are set into @p mask.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_maskul(unsigned long * __restrict bmap, unsigned long mask)
{
	stroll_bmap_toggle_mask64((uint64_t *)bmap, mask);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_maskul(unsigned long * __restrict bmap, unsigned long mask)
{
	stroll_bmap_toggle_mask32((uint32_t *)bmap, mask);
}

#endif

/**
 * Toggle state of a single 32-bits word bitmap bit.
 *
 * @param[inout] bmap   Bitmap which bit to toggle
 * @param[in]    bit_no Index of bit to toggle
 *
 * Invert value of bit specified by @p bit_no into @p bmap bitmap.
 * Bit indices start from 0.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle32(uint32_t * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap_toggle_mask32(bmap, UINT32_C(1) << bit_no);
}

/**
 * Toggle state of a single integer bitmap bit.
 *
 * @param[inout] bmap   Bitmap which bit to toggle
 * @param[in]    bit_no Index of bit to toggle
 *
 * Invert value of bit specified by @p bit_no into @p bmap bitmap.
 * Bit indices start from 0.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle(unsigned int * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_toggle32((uint32_t *)bmap, bit_no);
}

/**
 * Toggle state of a single 64-bits word bitmap bit.
 *
 * @param[inout] bmap   Bitmap which bit to toggle
 * @param[in]    bit_no Index of bit to toggle
 *
 * Invert value of bit specified by @p bit_no into @p bmap bitmap.
 * Bit indices start from 0.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle64(uint64_t * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 64);

	stroll_bmap_toggle_mask64(bmap, UINT64_C(1) << bit_no);
}

/**
 * Toggle state of a single machine word bitmap bit.
 *
 * @param[inout] bmap   Bitmap which bit to toggle
 * @param[in]    bit_no Index of bit to toggle
 *
 * Invert value of bit specified by @p bit_no into @p bmap bitmap.
 * Bit indices start from 0.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggleul(unsigned long * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_toggle64((uint64_t *)bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggleul(unsigned long * __restrict bmap, unsigned int bit_no)
{
	stroll_bmap_toggle32((uint32_t *)bmap, bit_no);
}

#endif

/**
 * Toggle state of bits of a 32-bits word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap which bits to toggle
 * @param[in]    start_bit Index of first bit to toggle
 * @param[in]    bit_count Number of bits to toggle
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, invert value of all bits in @p bmap bitmap which are set into
 * this computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask32().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_range32(uint32_t * __restrict bmap,
                           unsigned int          start_bit,
                           unsigned int          bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	stroll_bmap_toggle_mask32(bmap,
	                          stroll_bmap_mask32(start_bit, bit_count));
}

/**
 * Toggle state of bits of a integer bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap which bits to toggle
 * @param[in]    start_bit Index of first bit to toggle
 * @param[in]    bit_count Number of bits to toggle
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, invert value of all bits in @p bmap bitmap which are set into
 * this computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_range(unsigned int * __restrict bmap,
                         unsigned int              start_bit,
                         unsigned int              bit_count)
{
	stroll_bmap_toggle_range32((uint32_t *)bmap, start_bit, bit_count);
}

/**
 * Toggle state of bits of a 64-bits word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap which bits to toggle
 * @param[in]    start_bit Index of first bit to toggle
 * @param[in]    bit_count Number of bits to toggle
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, invert value of all bits in @p bmap bitmap which are set into
 * this computed mask.
 *
 * Mask is computed thanks to stroll_bmap_mask64().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_range64(uint64_t * __restrict bmap,
                           unsigned int          start_bit,
                           unsigned int          bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	stroll_bmap_toggle_mask64(bmap,
	                          stroll_bmap_mask64(start_bit, bit_count));
}

/**
 * Toggle state of bits of a machine word bitmap according to specified range.
 *
 * @param[inout] bmap      Bitmap which bits to toggle
 * @param[in]    start_bit Index of first bit to toggle
 * @param[in]    bit_count Number of bits to toggle
 *
 * Compute a mask according to @p start_bit and @p bit_count range given in
 * argument. Then, invert value of all bits in @p bmap bitmap which are set into
 * this computed mask.
 *
 * Mask is computed thanks to stroll_bmap_maskul().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be less than the maximum number of
 *   bits contained within a machine word. If not, an undefined result is
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is disabled. An
 *   assertion is triggered otherwise.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_rangeul(unsigned long * __restrict bmap,
                           unsigned int               start_bit,
                           unsigned int               bit_count)
{
	stroll_bmap_toggle_range64((uint64_t *)bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_rangeul(unsigned long * __restrict bmap,
                           unsigned int               start_bit,
                           unsigned int               bit_count)
{
	stroll_bmap_toggle_range32((uint32_t *)bmap, start_bit, bit_count);
}

#endif

/**
 * Toggle all bits of a 32-bits word bitmap.
 *
 * @param[out] bmap Bitmap which bits to toggle
 *
 * Invert the value of all bits of @p bmap bitmap.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_all32(uint32_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = ~(*bmap);
}

/**
 * Toggle all bits of a integer bitmap.
 *
 * @param[out] bmap Bitmap which bits to toggle
 *
 * Invert the value of all bits of @p bmap bitmap.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_all(unsigned int * __restrict bmap)
{
	stroll_bmap_toggle_all32((uint32_t *)bmap);
}

/**
 * Toggle all bits of a 64-bits word bitmap.
 *
 * @param[out] bmap Bitmap which bits to toggle
 *
 * Invert the value of all bits of @p bmap bitmap.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_all64(uint64_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = ~(*bmap);
}

/**
 * Toggle all bits of a machine word bitmap.
 *
 * @param[out] bmap Bitmap which bits to toggle
 *
 * Invert the value of all bits of @p bmap bitmap.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_allul(unsigned long * __restrict bmap)
{
	stroll_bmap_toggle_all64((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_toggle_allul(unsigned long * __restrict bmap)
{
	stroll_bmap_toggle_all32((uint32_t *)bmap);
}

#endif

/**
 * Static initialize a 32-bits bitmap with all bits set.
 */
#define STROLL_BMAP_INIT_SET32 (UINT32_MAX)

/**
 * Setup a 32-bits bitmap with all bits set.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_set32(uint32_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap_set_all32(bmap);
}

/**
 * Static initialize an integer bitmap with all bits set.
 */
#define STROLL_BMAP_INIT_SET STROLL_BMAP_INIT_SET32

/**
 * Setup an integer bitmap with all bits set.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_set(unsigned int * __restrict bmap)
{
	stroll_bmap_setup_set32((uint32_t *)bmap);
}

/**
 * Static initialize a 64-bits bitmap with all bits set.
 */
#define STROLL_BMAP_INIT_SET64 (UINT64_MAX)

/**
 * Setup a 64-bits bitmap with all bits set.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_set64(uint64_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap_set_all64(bmap);
}

/**
 * Static initialize a machine word bitmap with all bits set.
 */
#define STROLL_BMAP_INIT_SETUL (ULONG_MAX)

/**
 * Setup a machine word bitmap with all bits set.
 *
 * @param[out] bmap Bitmap to setup.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_setul(unsigned long * __restrict bmap)
{
	stroll_bmap_setup_set64((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_setul(unsigned long * __restrict bmap)
{
	stroll_bmap_setup_set32((uint32_t *)bmap);
}

#endif

/**
 * Static initialize a 32-bits bitmap with all bits cleared.
 */
#define STROLL_BMAP_INIT_CLEAR32 (UINT32_C(0))

/**
 * Setup a 32-bits bitmap with all bits cleared.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_clear32(uint32_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap_clear_all32(bmap);
}

/**
 * Static initialize an integer bitmap with all bits cleared.
 */
#define STROLL_BMAP_INIT_CLEAR STROLL_BMAP_INIT_CLEAR32

/**
 * Setup an integer bitmap with all bits cleared.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_clear(unsigned int * __restrict bmap)
{
	stroll_bmap_setup_clear32((uint32_t *)bmap);
}

/**
 * Static initialize a 64-bits bitmap with all bits cleared.
 */
#define STROLL_BMAP_INIT_CLEAR64 (UINT64_C(0))

/**
 * Setup a 64-bits bitmap with all bits cleared.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_clear64(uint64_t * __restrict bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap_clear_all64(bmap);
}

/**
 * Static initialize a machine word bitmap with all bits cleared.
 */
#define STROLL_BMAP_INIT_CLEARUL (0UL)

/**
 * Setup a machine word bitmap with all bits cleared.
 *
 * @param[out] bmap Bitmap to setup.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_clearul(unsigned long * __restrict bmap)
{
	stroll_bmap_setup_clear64((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __stroll_nothrow
stroll_bmap_setup_clearul(unsigned long * __restrict bmap)
{
	stroll_bmap_setup_clear32((uint32_t *)bmap);
}

#endif

static inline int __stroll_nonull(1, 2) __stroll_nothrow
_stroll_bmap_step_iter32(uint32_t * __restrict     iter,
                         unsigned int * __restrict bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	if (*iter) {
		unsigned int diff = stroll_bops_ffs32(*iter);

		*bit_no += diff;
		*iter >>= diff;

		return 0;
	}
	else
		return -1;
}

static inline void __stroll_nonull(1, 3) __stroll_nothrow
_stroll_bmap_setup_set_iter32(uint32_t * __restrict     iter,
                              uint32_t                  bmap,
                              unsigned int * __restrict bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Wrap around at next _stroll_bmap_step_iter32() call. */
	*bit_no = (unsigned int)-1;
	*iter = bmap;
}

/**
 * Iterate over all bits set in a 32-bits word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter iterator, iterate over bits set in @p
 * _bmap bitmap and update @p _bit_no to reflect current bit set.
 */
#define stroll_bmap_foreach_set32(_iter, _bmap, _bit_no) \
	for (_stroll_bmap_setup_set_iter32(_iter, _bmap, _bit_no); \
	     !_stroll_bmap_step_iter32(_iter, _bit_no); \
	    )

/**
 * Iterate over all bits set in an integer bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter iterator, iterate over bits set in @p
 * _bmap bitmap and update @p _bit_no to reflect current bit set.
 */
#define stroll_bmap_foreach_set(_iter, _bmap, _bit_no) \
	stroll_bmap_foreach_set32(_iter, _bmap, _bit_no)

static inline int __stroll_nonull(1, 2) __stroll_nothrow
_stroll_bmap_step_iter64(uint64_t * __restrict     iter,
                         unsigned int * __restrict bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	if (*iter) {
		unsigned int diff = stroll_bops_ffs64(*iter);

		*bit_no += diff;
		*iter >>= diff;

		return 0;
	}
	else
		return -1;
}

static inline void __stroll_nonull(1, 3) __stroll_nothrow
_stroll_bmap_setup_set_iter64(uint64_t * __restrict     iter,
                              uint64_t                  bmap,
                              unsigned int * __restrict bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Wrap around at next _stroll_bmap_step_iter64() call. */
	*bit_no = (unsigned int)-1;
	*iter = bmap;
}

/**
 * Iterate over all bits set in a 64-bits word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter iterator, iterate over bits set in @p
 * _bmap bitmap and update @p _bit_no to reflect current bit set.
 */
#define stroll_bmap_foreach_set64(_iter, _bmap, _bit_no) \
	for (_stroll_bmap_setup_set_iter64(_iter, _bmap, _bit_no); \
	     !_stroll_bmap_step_iter64(_iter, _bit_no); \
	    )

/**
 * Iterate over all bits set in a machine word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter iterator, iterate over bits set in @p
 * _bmap bitmap and update @p _bit_no to reflect current bit set.
 */
#if __WORDSIZE == 64

#define stroll_bmap_foreach_setul(_iter, _bmap, _bit_no) \
	stroll_bmap_foreach_set64((uint64_t *)(_iter), _bmap, _bit_no)

#elif __WORDSIZE == 32

#define stroll_bmap_foreach_setul(_iter, _bmap, _bit_no) \
	stroll_bmap_foreach_set32((uint32_t *)(_iter), _bmap, _bit_no)

#endif

static inline void __stroll_nonull(1, 3) __stroll_nothrow
_stroll_bmap_setup_clear_iter32(uint32_t * __restrict     iter,
                                uint32_t                  bmap,
                                unsigned int * __restrict bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Wrap around at next _stroll_bmap_step_iter32() call. */
	*bit_no = (unsigned int)-1;
	*iter = bmap ^ UINT32_MAX;
}

/**
 * Iterate over all bits cleared in a 32-bits word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter iterator, iterate over bits cleared in @p
 * _bmap bitmap and update @p _bit_no to reflect current bit cleared
 */
#define stroll_bmap_foreach_clear32(_iter, _bmap, _bit_no) \
	for (_stroll_bmap_setup_clear_iter32(_iter, _bmap, _bit_no); \
	     !_stroll_bmap_step_iter32(_iter, _bit_no); \
	    )

/**
 * Iterate over all bits cleared in an integer bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter iterator, iterate over bits cleared in @p
 * _bmap bitmap and update @p _bit_no to reflect current bit cleared
 */
#define stroll_bmap_foreach_clear(_iter, _bmap, _bit_no) \
	stroll_bmap_foreach_clear32(_iter, _bmap, _bit_no)

static inline void __stroll_nonull(1, 3) __stroll_nothrow
_stroll_bmap_setup_clear_iter64(uint64_t * __restrict     iter,
                                uint64_t                  bmap,
                                unsigned int * __restrict bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Wrap around at next _stroll_bmap_step_iter64() call. */
	*bit_no = (unsigned int)-1;
	*iter = bmap ^ UINT64_MAX;
}

/**
 * Iterate over all bits cleared in a 64-bits word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter iterator, iterate over bits cleared in @p
 * _bmap bitmap and update @p _bit_no to reflect current bit cleared
 */
#define stroll_bmap_foreach_clear64(_iter, _bmap, _bit_no) \
	for (_stroll_bmap_setup_clear_iter64(_iter, _bmap, _bit_no); \
	     !_stroll_bmap_step_iter64(_iter, _bit_no); \
	    )

/**
 * Iterate over all bits cleared in a machine word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter iterator, iterate over bits cleared in @p
 * _bmap bitmap and update @p _bit_no to reflect current bit cleared
 */
#if __WORDSIZE == 64

#define stroll_bmap_foreach_clearul(_iter, _bmap, _bit_no) \
	stroll_bmap_foreach_clear64((uint64_t *)(_iter), _bmap, _bit_no)

#elif __WORDSIZE == 32

#define stroll_bmap_foreach_clearul(_iter, _bmap, _bit_no) \
	stroll_bmap_foreach_clear32((uint32_t *)(_iter), _bmap, _bit_no)

#endif

#endif /* _STROLL_BMAP_H */
