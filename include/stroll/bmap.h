/**
 * @file
 * Bitmap interface
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
#ifndef _STROLL_BMAP_H
#define _STROLL_BMAP_H

#include <stroll/bops.h>
#include <stdbool.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_bmap_assert_api(_expr) \
	stroll_assert("stroll: bmap", _expr)

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
 * is @p start_bit to `start_bit + bit_count - 1`. Bit indices starts from 0.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint32_t __stroll_const __nothrow __warn_result
stroll_bmap32_mask(unsigned int start_bit, unsigned int bit_count)
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
 * Compute a 64-bits word mask.
 *
 * @param[in] start_bit Index of first mask bit set starting from 0
 * @param[in] bit_count Number of bits set in mask
 *
 * @return Computed mask
 *
 * Compute a 64-bits mask where all bits are set starting from bit which index
 * is @p start_bit to `start_bit + bit_count - 1`. Bit indices starts from 0.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint64_t __stroll_const __nothrow __warn_result
stroll_bmap64_mask(unsigned int start_bit, unsigned int bit_count)
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
 *   returned when the #CONFIG_STROLL_ASSERT_API build option is disabled. An
 *   assertion is triggered otherwise.
 */

#if __WORDSIZE == 64

static inline unsigned long __stroll_const __nothrow __warn_result
stroll_bmap_mask(unsigned int start_bit, unsigned int bit_count)
{
	return stroll_bmap64_mask(start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline unsigned long __stroll_const __nothrow __warn_result
stroll_bmap_mask(unsigned int start_bit, unsigned int bit_count)
{
	return stroll_bmap32_mask(start_bit, bit_count);
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
stroll_bmap32_hweight(uint32_t bmap)
{
	return stroll_bops32_hweight(bmap);
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
stroll_bmap64_hweight(uint64_t bmap)
{
	return stroll_bops64_hweight(bmap);
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

#if __WORDSIZE == 64

static inline unsigned int __const __nothrow __warn_result
stroll_bmap_hweight(unsigned long bmap)
{
	return stroll_bops64_hweight(bmap);
}

#elif __WORDSIZE == 32

static inline unsigned int __const __nothrow __warn_result
stroll_bmap_hweight(unsigned long bmap)
{
	return stroll_bops32_hweight(bmap);
}

#endif

/**
 * Perform bitwise mask based AND operation on 32-bits word bitmap.
 *
 * @param[in] bmap Bitmap to perform operation on
 * @param[in] mask Mask to perform operation with
 *
 * @return `bmap & mask`
 */
static inline uint32_t __const __nothrow __warn_result
stroll_bmap32_and(uint32_t bmap, uint32_t mask)
{
	return bmap & mask;
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
stroll_bmap64_and(uint64_t bmap, uint64_t mask)
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
stroll_bmap_and(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap64_and(bmap, mask);
}

#elif __WORDSIZE == 32

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_and(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap32_and(bmap, mask);
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
 * Mask is computed thanks to stroll_bmap32_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint32_t __stroll_const __nothrow __warn_result
stroll_bmap32_and_range(uint32_t     bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_and(bmap,
	                         stroll_bmap32_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap64_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint64_t __stroll_const __nothrow __warn_result
stroll_bmap64_and_range(uint64_t     bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return stroll_bmap64_and(bmap,
	                         stroll_bmap64_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap_mask().
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

static inline unsigned long __stroll_const __nothrow __warn_result
stroll_bmap_and_range(unsigned long bmap,
                      unsigned int  start_bit,
                      unsigned int  bit_count)
{
	return stroll_bmap64_and_range(bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline unsigned long __stroll_const __nothrow __warn_result
stroll_bmap_and_range(unsigned long bmap,
                      unsigned int  start_bit,
                      unsigned int  bit_count)
{
	return stroll_bmap32_and_range(bmap, start_bit, bit_count);
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
stroll_bmap32_or(uint32_t bmap, uint32_t mask)
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
stroll_bmap64_or(uint64_t bmap, uint64_t mask)
{
	return bmap | mask;
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
stroll_bmap_or(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap64_or(bmap, mask);
}

#elif __WORDSIZE == 32

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_or(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap32_or(bmap, mask);
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
 * Mask is computed thanks to stroll_bmap32_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint32_t __stroll_const __nothrow __warn_result
stroll_bmap32_or_range(uint32_t     bmap,
                       unsigned int start_bit,
                       unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_or(bmap, stroll_bmap32_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap64_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint64_t __stroll_const __nothrow __warn_result
stroll_bmap64_or_range(uint64_t     bmap,
                       unsigned int start_bit,
                       unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return stroll_bmap64_or(bmap, stroll_bmap64_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap_mask().
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

static inline unsigned long __stroll_const __nothrow __warn_result
stroll_bmap_or_range(unsigned long bmap,
                     unsigned int  start_bit,
                     unsigned int  bit_count)
{
	return stroll_bmap64_or_range(bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline unsigned long __stroll_const __nothrow __warn_result
stroll_bmap_or_range(unsigned long bmap,
                     unsigned int  start_bit,
                     unsigned int  bit_count)
{
	return stroll_bmap32_or_range(bmap, start_bit, bit_count);
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
stroll_bmap32_xor(uint32_t bmap, uint32_t mask)
{
	return bmap ^ mask;
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
stroll_bmap64_xor(uint64_t bmap, uint64_t mask)
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
stroll_bmap_xor(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap64_xor(bmap, mask);
}

#elif __WORDSIZE == 32

static inline unsigned long __const __nothrow __warn_result
stroll_bmap_xor(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap32_xor(bmap, mask);
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
 * Mask is computed thanks to stroll_bmap32_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint32_t __stroll_const __nothrow __warn_result
stroll_bmap32_xor_range(uint32_t     bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_xor(bmap,
	                         stroll_bmap32_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap64_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline uint64_t __stroll_const __nothrow __warn_result
stroll_bmap64_xor_range(uint64_t     bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return stroll_bmap64_xor(bmap,
	                         stroll_bmap64_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap_mask().
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

static inline unsigned long __stroll_const __nothrow __warn_result
stroll_bmap_xor_range(unsigned long bmap,
                      unsigned int  start_bit,
                      unsigned int  bit_count)
{
	return stroll_bmap64_xor_range(bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline unsigned long __stroll_const __nothrow __warn_result
stroll_bmap_xor_range(unsigned long bmap,
                      unsigned int  start_bit,
                      unsigned int  bit_count)
{
	return stroll_bmap32_xor_range(bmap, start_bit, bit_count);
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
stroll_bmap32_test_mask(uint32_t bmap, uint32_t mask)
{
	return !!stroll_bmap32_and(bmap, mask);
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
stroll_bmap64_test_mask(uint64_t bmap, uint64_t mask)
{
	return !!stroll_bmap64_and(bmap, mask);
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
stroll_bmap_test_mask(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap64_test_mask(bmap, mask);
}

#elif __WORDSIZE == 32

static inline bool __const __nothrow __warn_result
stroll_bmap_test_mask(unsigned long bmap, unsigned long mask)
{
	return stroll_bmap32_test_mask(bmap, mask);
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
 * Bit indices starts from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __nothrow __warn_result
stroll_bmap32_test(uint32_t bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bit_no < stroll_bops_bitsof(bmap));

	return stroll_bmap32_test_mask(bmap, UINT32_C(1) << bit_no);
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
 * Bit indices starts from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 64`, result is undefined. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __nothrow __warn_result
stroll_bmap64_test(uint64_t bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bit_no < stroll_bops_bitsof(bmap));

	return stroll_bmap64_test_mask(bmap, UINT64_C(1) << bit_no);
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
 * Bit indices starts from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the maximum number of bits contained
 * within a machine word, result is undefined. An assertion is triggered
 * otherwise.
 */

#if __WORDSIZE == 64

static inline bool __stroll_const __nothrow __warn_result
stroll_bmap_test(unsigned long bmap, unsigned int bit_no)
{
	return stroll_bmap64_test(bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline bool __stroll_const __nothrow __warn_result
stroll_bmap_test(unsigned long bmap, unsigned int bit_no)
{
	return stroll_bmap32_test(bmap, bit_no);
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
 * Mask is computed thanks to stroll_bmap32_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __nothrow __warn_result
stroll_bmap32_test_range(uint32_t     bmap,
                         unsigned int start_bit,
                         unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_test_mask(bmap, stroll_bmap32_mask(start_bit,
	                                                        bit_count));
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
 * Mask is computed thanks to stroll_bmap64_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline bool __stroll_const __nothrow __warn_result
stroll_bmap64_test_range(uint64_t     bmap,
                         unsigned int start_bit,
                         unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	return stroll_bmap64_test_mask(bmap, stroll_bmap64_mask(start_bit,
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
 * Mask is computed thanks to stroll_bmap_mask().
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

static inline bool __stroll_const __nothrow __warn_result
stroll_bmap_test_range(unsigned long bmap,
                       unsigned int  start_bit,
                       unsigned int  bit_count)
{
	return stroll_bmap64_test_range(bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline bool __stroll_const __nothrow __warn_result
stroll_bmap_test_range(unsigned long bmap,
                       unsigned int  start_bit,
                       unsigned int  bit_count)
{
	return stroll_bmap32_test_range(bmap, start_bit, bit_count);
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
stroll_bmap32_test_all(uint32_t bmap)
{
	return !!bmap;
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
stroll_bmap64_test_all(uint64_t bmap)
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
stroll_bmap_test_all(unsigned long bmap)
{
	return stroll_bmap64_test_all(bmap);
}

#elif __WORDSIZE == 32

static inline bool __const __nothrow __warn_result
stroll_bmap_test_all(unsigned long bmap)
{
	return stroll_bmap32_test_all(bmap);
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
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap32_or(*bmap, mask);
}

/**
 * Set bits of 64-bits word bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to set
 * @param[in]    mask Mask of bits to set
 *
 * Set all bits of @p bmap bitmap which are set into @p mask.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_set_mask(uint64_t * bmap, uint64_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap64_or(*bmap, mask);
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_set_mask(unsigned long * bmap, unsigned long mask)
{
	stroll_bmap64_set_mask((uint64_t *)bmap, mask);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_set_mask(unsigned long * bmap, unsigned long mask)
{
	stroll_bmap32_set_mask((uint32_t *)bmap, mask);
}

#endif

/**
 * Set a bit in a 32-bits word bitmap.
 *
 * @param[inout] bmap   Bitmap to set
 * @param[in]    bit_no Index of bit to set
 *
 * Set bit specified by @p bit_no into @p bmap bitmap. Bit indices starts from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap32_set_mask(bmap, UINT32_C(1) << bit_no);
}

/**
 * Set a bit in a 64-bits word bitmap.
 *
 * @param[inout] bmap   Bitmap to set
 * @param[in]    bit_no Index of bit to set
 *
 * Set bit specified by @p bit_no into @p bmap bitmap. Bit indices starts from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 64`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_set(uint64_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 64);

	stroll_bmap64_set_mask(bmap, UINT64_C(1) << bit_no);
}

/**
 * Set a bit in a machine word bitmap.
 *
 * @param[inout] bmap   Bitmap to set
 * @param[in]    bit_no Index of bit to set
 *
 * Set bit specified by @p bit_no into @p bmap bitmap. Bit indices starts from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the maximum number of bits contained
 * within a machine word, result is undefined. An assertion is triggered
 * otherwise.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_set(unsigned long * bmap, unsigned int bit_no)
{
	stroll_bmap64_set((uint64_t *)bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_set(unsigned long * bmap, unsigned int bit_no)
{
	stroll_bmap32_set((uint32_t *)bmap, bit_no);
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
 * Mask is computed thanks to stroll_bmap32_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set_range(uint32_t *   bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	stroll_bmap32_set_mask(bmap, stroll_bmap32_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap64_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_set_range(uint64_t *   bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	stroll_bmap64_set_mask(bmap, stroll_bmap64_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap_mask().
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_set_range(unsigned long * bmap,
                      unsigned int    start_bit,
                      unsigned int    bit_count)
{
	stroll_bmap64_set_range((uint64_t *)bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_set_range(unsigned long * bmap,
                      unsigned int    start_bit,
                      unsigned int    bit_count)
{
	stroll_bmap32_set_range((uint32_t *)bmap, start_bit, bit_count);
}

#endif

/**
 * Set all bits of a 32-bits word bitmap.
 *
 * @param[out] bmap Bitmap to set
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = UINT32_MAX;
}

/**
 * Set all bits of a 64-bits word bitmap.
 *
 * @param[out] bmap Bitmap to set
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_set_all(uint64_t * bmap)
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_set_all(unsigned long * bmap)
{
	stroll_bmap64_set_all((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_set_all(unsigned long * bmap)
{
	stroll_bmap32_set_all((uint32_t *)bmap);
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
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap32_and(*bmap, ~mask);
}

/**
 * Clear bits of 64-bits word bitmap according to mask given in argument.
 *
 * @param[inout] bmap Bitmap to clear
 * @param[in]    mask Mask of bits to clear
 *
 * Clear all bits of @p bmap bitmap which are set into @p mask.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_clear_mask(uint64_t * bmap, uint64_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap64_and(*bmap, ~mask);
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_clear_mask(unsigned long * bmap, unsigned long mask)
{
	return stroll_bmap64_clear_mask((uint64_t *)bmap, mask);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_clear_mask(unsigned long * bmap, unsigned long mask)
{
	return stroll_bmap32_clear_mask((uint32_t *)bmap, mask);
}

#endif

/**
 * Clear a bit in a 32-bits word bitmap.
 *
 * @param[inout] bmap   Bitmap to clear
 * @param[in]    bit_no Index of bit to clear
 *
 * Clear bit specified by @p bit_no into @p bmap bitmap. Bit indices starts from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 32`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap32_clear_mask(bmap, UINT32_C(1) << bit_no);
}

/**
 * Clear a bit in a 64-bits word bitmap.
 *
 * @param[inout] bmap   Bitmap to clear
 * @param[in]    bit_no Index of bit to clear
 *
 * Clear bit specified by @p bit_no into @p bmap bitmap. Bit indices starts from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is `>= 64`, result is undefined. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_clear(uint64_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 64);

	stroll_bmap64_clear_mask(bmap, UINT64_C(1) << bit_no);
}

/**
 * Clear a bit in a machine word bitmap.
 *
 * @param[inout] bmap   Bitmap to clear
 * @param[in]    bit_no Index of bit to clear
 *
 * Clear bit specified by @p bit_no into @p bmap bitmap. Bit indices starts from
 * 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the maximum number of bits contained
 * within a machine word, result is undefined. An assertion is triggered
 * otherwise.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_clear(unsigned long * bmap, unsigned int bit_no)
{
	stroll_bmap64_clear((uint64_t *)bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_clear(unsigned long * bmap, unsigned int bit_no)
{
	stroll_bmap32_clear((uint32_t *)bmap, bit_no);
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
 * Mask is computed thanks to stroll_bmap32_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear_range(uint32_t *   bmap,
                          unsigned int start_bit,
                          unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	stroll_bmap32_clear_mask(bmap,
	                         stroll_bmap32_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap64_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_clear_range(uint64_t *   bmap,
                          unsigned int start_bit,
                          unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	stroll_bmap64_clear_mask(bmap,
	                         stroll_bmap64_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap_mask().
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_clear_range(unsigned long * bmap,
                        unsigned int    start_bit,
                        unsigned int    bit_count)
{
	stroll_bmap64_clear_range((uint64_t *)bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_clear_range(unsigned long * bmap,
                        unsigned int    start_bit,
                        unsigned int    bit_count)
{
	stroll_bmap32_clear_range((uint32_t *)bmap, start_bit, bit_count);
}

#endif

/**
 * Clear all bits of a 32-bits word bitmap.
 *
 * @param[out] bmap Bitmap to clear
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = 0;
}

/**
 * Clear all bits of a 64-bits word bitmap.
 *
 * @param[out] bmap Bitmap to clear
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_clear_all(uint64_t * bmap)
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_clear_all(unsigned long * bmap)
{
	stroll_bmap64_clear_all((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_clear_all(unsigned long * bmap)
{
	stroll_bmap32_clear_all((uint32_t *)bmap);
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
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap32_xor(*bmap, mask);
}

/**
 * Toggle state of bits of a 64-bits word bitmap according to given mask.
 *
 * @param[inout] bmap Bitmap which bits to toggle
 * @param[in]    mask Mask of bits to toggle
 *
 * Invert value of @p bmap bitmap bits which are set into @p mask.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_toggle_mask(uint64_t * bmap, uint64_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap64_xor(*bmap, mask);
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_toggle_mask(unsigned long * bmap, unsigned long mask)
{
	stroll_bmap64_toggle_mask((uint64_t *)bmap, mask);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_toggle_mask(unsigned long * bmap, unsigned long mask)
{
	stroll_bmap32_toggle_mask((uint32_t *)bmap, mask);
}

#endif

/**
 * Toggle state of a single 32-bits word bitmap bit.
 *
 * @param[inout] bmap   Bitmap which bit to toggle
 * @param[in]    bit_no Index of bit to toggle
 *
 * Invert value of bit specified by @p bit_no into @p bmap bitmap.
 * Bit indices starts from 0.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap32_toggle_mask(bmap, UINT32_C(1) << bit_no);
}

/**
 * Toggle state of a single 64-bits word bitmap bit.
 *
 * @param[inout] bmap   Bitmap which bit to toggle
 * @param[in]    bit_no Index of bit to toggle
 *
 * Invert value of bit specified by @p bit_no into @p bmap bitmap.
 * Bit indices starts from 0.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_toggle(uint64_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 64);

	stroll_bmap64_toggle_mask(bmap, UINT64_C(1) << bit_no);
}

/**
 * Toggle state of a single machine word bitmap bit.
 *
 * @param[inout] bmap   Bitmap which bit to toggle
 * @param[in]    bit_no Index of bit to toggle
 *
 * Invert value of bit specified by @p bit_no into @p bmap bitmap.
 * Bit indices starts from 0.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_toggle(unsigned long * bmap, unsigned int bit_no)
{
	stroll_bmap64_toggle((uint64_t *)bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_toggle(unsigned long * bmap, unsigned int bit_no)
{
	stroll_bmap32_toggle((uint32_t *)bmap, bit_no);
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
 * Mask is computed thanks to stroll_bmap32_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 32`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle_range(uint32_t *   bmap,
                           unsigned int start_bit,
                           unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	stroll_bmap32_toggle_mask(bmap,
	                          stroll_bmap32_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap64_mask().
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be `<= 64`. If not, an undefined
 *   result is returned when the #CONFIG_STROLL_ASSERT_API build option is
 *   disabled. An assertion is triggered otherwise.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_toggle_range(uint64_t *   bmap,
                           unsigned int start_bit,
                           unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 64);

	stroll_bmap64_toggle_mask(bmap,
	                          stroll_bmap64_mask(start_bit, bit_count));
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
 * Mask is computed thanks to stroll_bmap_mask().
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_toggle_range(unsigned long * bmap,
                         unsigned int    start_bit,
                         unsigned int    bit_count)
{
	stroll_bmap64_toggle_range((uint64_t *)bmap, start_bit, bit_count);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_toggle_range(unsigned long * bmap,
                         unsigned int    start_bit,
                         unsigned int    bit_count)
{
	stroll_bmap32_toggle_range((uint32_t *)bmap, start_bit, bit_count);
}

#endif

/**
 * Toggle all bits of a 32-bits word bitmap.
 *
 * @param[out] bmap Bitmap which bits to toggle
 *
 * Invert the value of all bits of @p bmap bitmap.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = ~(*bmap);
}

/**
 * Toggle all bits of a 64-bits word bitmap.
 *
 * @param[out] bmap Bitmap which bits to toggle
 *
 * Invert the value of all bits of @p bmap bitmap.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_toggle_all(uint64_t * bmap)
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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_toggle_all(unsigned long * bmap)
{
	stroll_bmap64_toggle_all((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_toggle_all(unsigned long * bmap)
{
	stroll_bmap32_toggle_all((uint32_t *)bmap);
}

#endif

/**
 * Static initialize a 32-bits bitmap with all bits set.
 */
#define STROLL_BMAP32_INIT_SET (UINT32_MAX)

/**
 * Setup a 32-bits bitmap with all bits set.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_setup_set(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap32_set_all(bmap);
}

/**
 * Static initialize a 64-bits bitmap with all bits set.
 */
#define STROLL_BMAP64_INIT_SET (UINT64_MAX)

/**
 * Setup a 64-bits bitmap with all bits set.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_setup_set(uint64_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap64_set_all(bmap);
}

/**
 * Static initialize a machine word bitmap with all bits set.
 */
#define STROLL_BMAP_INIT_SET   (ULONG_MAX)

/**
 * Setup a machine word bitmap with all bits set.
 *
 * @param[out] bmap Bitmap to setup.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_setup_set(unsigned long * bmap)
{
	stroll_bmap64_setup_set((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_setup_set(unsigned long * bmap)
{
	stroll_bmap32_setup_set((uint32_t *)bmap);
}

#endif

/**
 * Static initialize a 32-bits bitmap with all bits cleared.
 */
#define STROLL_BMAP32_INIT_CLEAR (UINT32_C(0))

/**
 * Setup a 32-bits bitmap with all bits cleared.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_setup_clear(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap32_clear_all(bmap);
}

/**
 * Static initialize a 64-bits bitmap with all bits cleared.
 */
#define STROLL_BMAP64_INIT_CLEAR (UINT64_C(0))

/**
 * Setup a 64-bits bitmap with all bits cleared.
 *
 * @param[out] bmap Bitmap to setup.
 */
static inline void __stroll_nonull(1) __nothrow
stroll_bmap64_setup_clear(uint64_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap64_clear_all(bmap);
}

/**
 * Static initialize a machine word bitmap with all bits cleared.
 */
#define STROLL_BMAP_INIT_CLEAR (0UL)

/**
 * Setup a machine word bitmap with all bits cleared.
 *
 * @param[out] bmap Bitmap to setup.
 */

#if __WORDSIZE == 64

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_setup_clear(unsigned long * bmap)
{
	stroll_bmap64_setup_clear((uint64_t *)bmap);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1) __nothrow
stroll_bmap_setup_clear(unsigned long * bmap)
{
	stroll_bmap32_setup_clear((uint32_t *)bmap);
}

#endif

static inline int __stroll_nonull(1, 2) __nothrow
_stroll_bmap32_step_iter(uint32_t * iter, unsigned int * bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	if (*iter) {
		unsigned int diff = stroll_bops32_ffs(*iter);

		*bit_no += diff;
		*iter >>= diff;

		return 0;
	}
	else
		return -1;
}

static inline int __stroll_nonull(1, 2) __nothrow
_stroll_bmap64_step_iter(uint64_t * iter, unsigned int * bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	if (*iter) {
		unsigned int diff = stroll_bops64_ffs(*iter);

		*bit_no += diff;
		*iter >>= diff;

		return 0;
	}
	else
		return -1;
}

#if __WORDSIZE == 64

static inline int __stroll_nonull(1, 2) __nothrow
_stroll_bmap_step_iter(unsigned long * iter, unsigned int * bit_no)
{
	return _stroll_bmap64_step_iter((uint64_t *)iter, bit_no);
}

#elif __WORDSIZE == 32

static inline int __stroll_nonull(1, 2) __nothrow
_stroll_bmap_step_iter(unsigned long * iter, unsigned int * bit_no)
{
	return _stroll_bmap32_step_iter((uint32_t *)iter, bit_no);
}

#endif

static inline void __stroll_nonull(1, 3) __nothrow
_stroll_bmap32_setup_set_iter(uint32_t     * iter,
                              uint32_t       bmap,
                              unsigned int * bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Wrap around at next _stroll_bmap32_step_iter() call. */
	*bit_no = (unsigned int)-1;
	*iter = bmap;
}

/**
 * Iterater over all bits set in a 32-bits word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter, iterate over bits set in @p bmap
 * bitmap and update @p bit_no to reflect current bit set.
 */
#define stroll_bmap32_foreach_set(_iter, _bmap, _bit_no) \
	for (_stroll_bmap32_setup_set_iter(_iter, _bmap, _bit_no); \
	     !_stroll_bmap32_step_iter(_iter, _bit_no); \
	    )

static inline void __stroll_nonull(1, 3) __nothrow
_stroll_bmap64_setup_set_iter(uint64_t     * iter,
                              uint64_t       bmap,
                              unsigned int * bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Wrap around at next _stroll_bmap64_step_iter() call. */
	*bit_no = (unsigned int)-1;
	*iter = bmap;
}

/**
 * Iterater over all bits set in a 64-bits word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter, iterate over bits set in @p bmap
 * bitmap and update @p bit_no to reflect current bit set.
 */
#define stroll_bmap64_foreach_set(_iter, _bmap, _bit_no) \
	for (_stroll_bmap64_setup_set_iter(_iter, _bmap, _bit_no); \
	     !_stroll_bmap64_step_iter(_iter, _bit_no); \
	    )

#if __WORDSIZE == 64

static inline void __stroll_nonull(1, 3) __nothrow
_stroll_bmap_setup_set_iter(unsigned long * iter,
                            unsigned long   bmap,
                            unsigned int  * bit_no)
{
	_stroll_bmap64_setup_set_iter((uint64_t *)iter, bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1, 3) __nothrow
_stroll_bmap_setup_set_iter(unsigned long * iter,
                            unsigned long   bmap,
                            unsigned int  * bit_no)
{
	_stroll_bmap32_setup_set_iter((uint32_t *)iter, bmap, bit_no);
}

#endif

/**
 * Iterater over all bits set in a machine word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter, iterate over bits set in @p bmap
 * bitmap and update @p bit_no to reflect current bit set.
 */
#define stroll_bmap_foreach_set(_iter, _bmap, _bit_no) \
	for (_stroll_bmap_setup_set_iter(_iter, _bmap, _bit_no); \
	     !_stroll_bmap_step_iter(_iter, _bit_no); \
	    )

static inline void __stroll_nonull(1, 3) __nothrow
_stroll_bmap32_setup_clear_iter(uint32_t     * iter,
                                uint32_t       bmap,
                                unsigned int * bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Wrap around at next _stroll_bmap32_step_iter() call. */
	*bit_no = (unsigned int)-1;
	*iter = bmap ^ UINT32_MAX;
}

/**
 * Iterater over all bits cleared in a 32-bits word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter, iterate over bits cleared in @p bmap
 * bitmap and update @p bit_no to reflect current bit cleared
 */
#define stroll_bmap32_foreach_clear(_iter, _bmap, _bit_no) \
	for (_stroll_bmap32_setup_clear_iter(_iter, _bmap, _bit_no); \
	     !_stroll_bmap32_step_iter(_iter, _bit_no); \
	    )

static inline void __stroll_nonull(1, 3) __nothrow
_stroll_bmap64_setup_clear_iter(uint64_t     * iter,
                                uint64_t       bmap,
                                unsigned int * bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Wrap around at next _stroll_bmap64_step_iter() call. */
	*bit_no = (unsigned int)-1;
	*iter = bmap ^ UINT64_MAX;
}

/**
 * Iterater over all bits cleared in a 64-bits word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter, iterate over bits cleared in @p bmap
 * bitmap and update @p bit_no to reflect current bit cleared
 */
#define stroll_bmap64_foreach_clear(_iter, _bmap, _bit_no) \
	for (_stroll_bmap64_setup_clear_iter(_iter, _bmap, _bit_no); \
	     !_stroll_bmap64_step_iter(_iter, _bit_no); \
	    )

#if __WORDSIZE == 64

static inline void __stroll_nonull(1, 3) __nothrow
_stroll_bmap_setup_clear_iter(unsigned long * iter,
                              unsigned long   bmap,
                              unsigned int  * bit_no)
{
	_stroll_bmap64_setup_clear_iter((uint64_t *)iter, bmap, bit_no);
}

#elif __WORDSIZE == 32

static inline void __stroll_nonull(1, 3) __nothrow
_stroll_bmap_setup_clear_iter(unsigned long * iter,
                              unsigned long   bmap,
                              unsigned int  * bit_no)
{
	_stroll_bmap32_setup_clear_iter((uint32_t *)iter, bmap, bit_no);
}

#endif

/**
 * Iterater over all bits cleared in a machine word bitmap.
 *
 * @param[in]  _iter   Temporary iteration bitmap
 * @param[in]  _bmap   Bitmap to iterate over
 * @param[out] _bit_no Current bit number
 *
 * Given an uninitialed bitmap @p iter, iterate over bits cleared in @p bmap
 * bitmap and update @p bit_no to reflect current bit cleared
 */
#define stroll_bmap_foreach_clear(_iter, _bmap, _bit_no) \
	for (_stroll_bmap_setup_clear_iter(_iter, _bmap, _bit_no); \
	     !_stroll_bmap_step_iter(_iter, _bit_no); \
	    )

#endif /* _STROLL_BMAP_H */
