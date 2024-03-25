/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2020-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Fixed bitmap interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      29 May 2020
 * @copyright Copyright (C) 2020-2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_FBMAP_H
#define _STROLL_FBMAP_H

#include <stroll/cdefs.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_fbmap_assert_api(_expr) \
	stroll_assert("stroll:fbmap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_fbmap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static inline __const __nothrow __warn_result
unsigned int
stroll_fbmap_word_no(unsigned int bit_no)
{
	return bit_no >> STROLL_WORD_SHIFT;
}

static inline __const __nothrow __warn_result
unsigned int
stroll_fbmap_word_nr(unsigned int bit_nr)
{
	return stroll_fbmap_word_no(bit_nr + __WORDSIZE - 1);
}

static inline __const __nothrow __warn_result
unsigned int
stroll_fbmap_word_bit_no(unsigned int bit_no)
{
	return bit_no & ((1UL << STROLL_WORD_SHIFT) - 1);
}

static inline __const __nothrow __warn_result
unsigned long
stroll_fbmap_word_bit_mask(unsigned int bit_no)
{
	return 1UL << stroll_fbmap_word_bit_no(bit_no);
}

/**
 * Fixed sized bitmap.
 *
 * An array of bits which size is set at initialization time where bit values
 * may be addressed by bit number / index.
 *
 * @warning
 * Cannot hold more than `INT_MAX` bits.
 */
struct stroll_fbmap {
	/**
	 * @internal
	 *
	 * Maximum number of bits this stroll_fbmap may hold.
	 * MUST BE <= `INT_MAX`.
	 */
	unsigned int    nr;
	/**
	 * @internal
	 *
	 * Memory area holding bit values.
	 */
	unsigned long * bits;
};

#define stroll_fbmap_assert_map_api(_bmap) \
	stroll_fbmap_assert_api(_bmap); \
	stroll_fbmap_assert_api((_bmap)->nr); \
	stroll_fbmap_assert_api((_bmap)->nr <= (unsigned int)INT_MAX); \
	stroll_fbmap_assert_api((_bmap)->bits)

/**
 * Return the maximum number of bits a fixed sized bitmap may hold.
 *
 * @param[in] bmap Bitmap
 *
 * @see
 * - stroll_fbmap_init_clear()
 * - stroll_fbmap_init_dup()
 * - stroll_fbmap_init_set()
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
unsigned int
stroll_fbmap_nr(const struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	return bmap->nr;
}

/**
 * Find the number of set bits over a fixed sized bitmap.
 *
 * @param[in] bmap Fixed sized bitmap
 *
 * @return Number of set bits
 *
 * Returns the Hamming weight of a stroll_fbmap fixed sized bitmap. The Hamming
 * weight of a bitmap is the total number of bits set in it.
 */
extern unsigned int
stroll_fbmap_hweight(const struct stroll_fbmap * __restrict bmap)
	__stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result;

/**
 * Test wether a bit in a fixed sized bitmap is set or not.
 *
 * @param[in] bmap   Bitmap to test
 * @param[in] bit_no Index of bit to test starting from 0
 *
 * @return Test result
 * @retval true  tested bit is set
 * @retval false tested bit is cleared
 *
 * Test wether the bit specified by @p bit_no is set into @p bmap fixed sized
 * bitmap. Bit indices start from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the number of bits specified at
 * initialization time, result is undefined. An assertion is triggered
 * otherwise.
 *
 * @see
 * - stroll_fbmap_test_all()
 * - stroll_fbmap_test_range()
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
bool
stroll_fbmap_test(const struct stroll_fbmap * __restrict bmap,
                  unsigned int                           bit_no)
{
	stroll_fbmap_assert_map_api(bmap);
	stroll_fbmap_assert_api(bit_no < bmap->nr);

	return !!(bmap->bits[stroll_fbmap_word_no(bit_no)] &
	          stroll_fbmap_word_bit_mask(bit_no));
}

/**
 * Test wether a range of bits in a fixed sized bitmap are set or not.
 *
 * @param[in] bmap      Bitmap to test
 * @param[in] start_bit Index of first bit to test starting from zero
 * @param[in] bit_count Number of bits to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * Check wether, within the range defined as
 * [@p start_bit, @p start_bit + @p bit_count[, there are bits set into
 * the @p bmap bitmap or not.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_count is zero, result is undefined. A zero @p bit_count triggers an
 *   assertion otherwise.
 * - The sum `start_bit + bit_count` **MUST** be lower than or equal to the
 *   number of bits specified at initialization time.
 *   If not, an undefined result is returned when the #CONFIG_STROLL_ASSERT_API
 *   build option is disabled. An assertion is triggered otherwise.
 *
 * @see
 * - stroll_fbmap_test()
 * - stroll_fbmap_test_all()
 */
extern bool
stroll_fbmap_test_range(const struct stroll_fbmap * __restrict bmap,
                        unsigned int                           start_bit,
                        unsigned int                           bit_count)
	__stroll_nonull(1) __stroll_pure __stroll_nothrow __leaf __warn_result;

/**
 * Test wether a fixed sized bitmap contains bits set or not.
 *
 * @param[in] bmap Bitmap to test
 *
 * @return Test result
 * @retval true  One or multiple tested bits are set into @p bmap
 * @retval false No tested bits are set into @p bmap
 *
 * @see
 * - stroll_fbmap_test()
 * - stroll_fbmap_test_range()
 */
extern bool
stroll_fbmap_test_all(const struct stroll_fbmap * __restrict bmap)
	__stroll_nonull(1) __stroll_pure __stroll_nothrow __leaf __warn_result;

/**
 * Set a bit in a fixed sized bitmap.
 *
 * @param[inout] bmap   Bitmap to set
 * @param[in]    bit_no Index of bit to set
 *
 * Set bit specified by @p bit_no into @p bmap fixed sized bitmap. Bit indices
 * start from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the number of bits specified at
 * initialization time, result is undefined. An assertion is triggered
 * otherwise.
 *
 * @see
 * - stroll_fbmap_clear()
 * - stroll_fbmap_toggle()
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_set(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
{
	stroll_fbmap_assert_map_api( bmap);
	stroll_fbmap_assert_api(bit_no < bmap->nr);

	 bmap->bits[stroll_fbmap_word_no(bit_no)] |=
		stroll_fbmap_word_bit_mask(bit_no);
}

/**
 * Set all bits of a fixed sized bitmap.
 *
 * @param[out] bmap Bitmap to set
 *
 * @see
 * - stroll_fbmap_clear_all()
 * - stroll_fbmap_toggle_all()
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_set_all(struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	memset(bmap->bits,
	       0xff,
	       stroll_fbmap_word_nr(bmap->nr) * sizeof(bmap->bits[0]));
}

/**
 * Clear a bit in a fixed sized bitmap.
 *
 * @param[inout] bmap   Bitmap to clear
 * @param[in]    bit_no Index of bit to clear
 *
 * Clear bit specified by @p bit_no into @p bmap fixed sized bitmap. Bit
 * indices start from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the number of bits specified at
 * initialization time, result is undefined. An assertion is triggered
 * otherwise.
 *
 * @see
 * - stroll_fbmap_set()
 * - stroll_fbmap_toggle()
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_clear(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
{
	stroll_fbmap_assert_map_api(bmap);
	stroll_fbmap_assert_api(bit_no < bmap->nr);

	bmap->bits[stroll_fbmap_word_no(bit_no)] &=
		~(stroll_fbmap_word_bit_mask(bit_no));
}

/**
 * Clear all bits of a fixed sized bitmap.
 *
 * @param[out] bmap Bitmap to clear
 *
 * @see
 * - stroll_fbmap_set_all()
 * - stroll_fbmap_toggle_all()
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_clear_all(struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	memset(bmap->bits,
	       0,
	       stroll_fbmap_word_nr(bmap->nr) * sizeof(bmap->bits[0]));
}

/**
 * Toggle a bit in a fixed sized bitmap.
 *
 * @param[inout] bmap   Bitmap to toggle
 * @param[in]    bit_no Index of bit to toggle
 *
 * Toggle bit specified by @p bit_no into @p bmap fixed sized bitmap. Bit
 * indices start from 0.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and @p
 * bit_no is greater than or equal to the number of bits specified at
 * initialization time, result is undefined. An assertion is triggered
 * otherwise.
 *
 * @see
 * - stroll_fbmap_clear()
 * - stroll_fbmap_set()
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_toggle(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
{
	stroll_fbmap_assert_map_api(bmap);
	stroll_fbmap_assert_api(bit_no < bmap->nr);

	bmap->bits[stroll_fbmap_word_no(bit_no)] ^=
		stroll_fbmap_word_bit_mask(bit_no);
}

/**
 * Toggle all bits of a fixed sized bitmap.
 *
 * @param[out] bmap Bitmap to toggle
 *
 * @see
 * - stroll_fbmap_set_all()
 * - stroll_fbmap_toggle_all()
 */
extern void
stroll_fbmap_toggle_all(struct stroll_fbmap * __restrict bmap)
	__stroll_nonull(1) __stroll_nothrow __leaf;

/**
 * Initialize fixed sized bitmap with all bits set.
 *
 * @param[out] bmap   Fixed sized bitmap
 * @param[in]  bit_nr Maximum number of bits @p bmap may hold
 *
 * @return an errno-like error code
 * @retval 0       success
 * @retval -ENOMEM memory allocation failed
 *
 * Initialize the @p bmap fixed sized bitmap with all bits set.
 *
 * @note
 * Once client code is done with @p bmap, it *MUST* call stroll_fbmap_fini() to
 * release allocated resources.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * @p bit_nr > `INT_MAX` result is undefined. An assertion is triggered
 * otherwise.
 *
 * @see
 * - stroll_fbmap_init_clear()
 * - stroll_fbmap_init_dup()
 * - stroll_fbmap_fini()
 */
extern int
stroll_fbmap_init_set(struct stroll_fbmap * __restrict bmap,
                      unsigned int                     bit_nr)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

/**
 * Zero-initialize fixed sized bitmap.
 *
 * @param[out] bmap   Fixed sized bitmap
 * @param[in]  bit_nr Maximum number of bits @p bmap may hold
 *
 * @return an errno-like error code
 * @retval 0       success
 * @retval -ENOMEM memory allocation failed
 *
 * Initialize the @p bmap fixed sized bitmap with all bits cleared.
 *
 * @note
 * Once client code is done with @p bmap, it *MUST* call stroll_fbmap_fini() to
 * release allocated resources.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p bit_nr > `INT_MAX` result is undefined. An assertion is triggered
 *   otherwise.
 *
 * @see
 * - stroll_fbmap_init_set()
 * - stroll_fbmap_init_dup()
 * - stroll_fbmap_fini()
 */
extern int
stroll_fbmap_init_clear(struct stroll_fbmap * __restrict bmap,
                        unsigned int                     bit_nr)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

/**
 * Initialize fixed sized bitmap using the content of another stroll_fbmap.
 *
 * @param[out] bmap  Fixed sized bitmap
 * @param[in]  src   Fixed sized bitmap to initialize @p bmap with
 *
 * @return an errno-like error code
 * @retval 0       success
 * @retval -ENOMEM memory allocation failed
 *
 * Initialize the @p bmap fixed sized bitmap with all bits set to values of @p
 * src bitmap.
 *
 * @note
 * Once client code is done with @p bmap, it *MUST* call stroll_fbmap_fini() to
 * release allocated resources.
 *
 * @see
 * - stroll_fbmap_init_clear()
 * - stroll_fbmap_init_set()
 * - stroll_fbmap_fini()
 */
extern int
stroll_fbmap_init_dup(struct stroll_fbmap * __restrict       bmap,
                      const struct stroll_fbmap * __restrict src)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __warn_result;

/**
 * Finalize a fixed sized bitmap.
 *
 * @param[inout] bmap Fixed sized bitmap
 *
 * Release resources allocated for @p bmap. Once called, you *MUST NOT* re-use
 * @p bmap unless re-initialized first.
 *
 * @see
 * - stroll_fbmap_init_set()
 * - stroll_fbmap_init_clear()
 * - stroll_fbmap_init_dup()
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_fini(struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	free(bmap->bits);
}

/**
 * Fixed sized bitmap iterator.
 *
 * An opaque structure that holds internal state of a stroll_fbmap fixed sized
 * bitmap iteration.
 *
 * @see
 * - stroll_fbmap_foreach_range_set()
 * - stroll_fbmap_foreach_set()
 * - stroll_fbmap_foreach_range_clear()
 * - stroll_fbmap_foreach_clear()
 */
struct stroll_fbmap_iter {
	unsigned long               word;
	unsigned int                curr;
	unsigned int                nr;
	const struct stroll_fbmap * bmap;
};

extern int
stroll_fbmap_step_iter_set(struct stroll_fbmap_iter * __restrict iter)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

extern int
stroll_fbmap_init_range_iter_set(
	struct stroll_fbmap_iter * __restrict  iter,
	const struct stroll_fbmap * __restrict bmap,
	unsigned int                           start_bit,
	unsigned int                           bit_count)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __warn_result;

/**
 * Iterate over bits set in a fixed sized bitmap given the specified bit range.
 *
 * @param[inout] _iter      Temporary iteration bitmap
 * @param[in]    _bmap      Bitmap to iterate over
 * @param[in]    _start_bit Range first bit
 * @param[in]    _bit_count Number of bits to iterate over
 * @param[out]   _bit_no    Current bit number
 *
 * Given an uninitialed @p iter stroll_fbmap_iter iterator, iterate over bits
 * set in @p _bmap stroll_fbmap bitmap and update @p _bit_no to reflect current
 * bit set.
 *
 * Iteration happens over the range defined as
 * [@p start_bit, @p start_bit + @p bit_count[.
 *
 * @see
 * - stroll_fbmap_foreach_range_clear()
 * - stroll_fbmap_foreach_set()
 */
#define stroll_fbmap_foreach_range_set(_iter, \
                                       _bmap, \
                                       _start_bit, \
                                       _bit_count, \
                                       _bit_no) \
	for ((_bit_no) = stroll_fbmap_init_range_iter_set(_iter, \
	                                                  _bmap, \
	                                                  _start_bit, \
	                                                  _bit_count); \
	     (_bit_no) >= 0; \
	     (_bit_no) = stroll_fbmap_step_iter_set(_iter))

static inline __stroll_nonull(1, 2) __stroll_nothrow __warn_result
int
stroll_fbmap_init_iter_set(struct stroll_fbmap_iter * __restrict  iter,
                           const struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_api(bmap);

	return stroll_fbmap_init_range_iter_set(iter, bmap, 0, bmap->nr);
}

/**
 * Iterate over all bits set in a fixed sized bitmap.
 *
 * @param[inout] _iter   Temporary iteration bitmap
 * @param[in]    _bmap   Bitmap to iterate over
 * @param[out]   _bit_no Current bit number
 *
 * Given an uninitialed @p iter stroll_fbmap_iter iterator, iterate over bits
 * set in @p _bmap stroll_fbmap bitmap and update @p _bit_no to reflect current
 * bit set.
 *
 * @see
 * - stroll_fbmap_foreach_range_set()
 * - stroll_fbmap_foreach_clear()
 */
#define stroll_fbmap_foreach_set(_iter, _bmap, _bit_no) \
	for ((_bit_no) = stroll_fbmap_init_iter_set(_iter, _bmap); \
	     (_bit_no) >= 0; \
	     (_bit_no) = stroll_fbmap_step_iter_set(_iter))

extern int
stroll_fbmap_step_iter_clear(struct stroll_fbmap_iter * __restrict iter)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

extern int
stroll_fbmap_init_range_iter_clear(
	struct stroll_fbmap_iter * __restrict  iter,
	const struct stroll_fbmap * __restrict bmap,
	unsigned int                           start_bit,
	unsigned int                           bit_count)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __warn_result;

/**
 * Iterate over bits cleared in a fixed sized bitmap given the specified bit
 * range.
 *
 * @param[inout] _iter      Temporary iteration bitmap
 * @param[in]    _bmap      Bitmap to iterate over
 * @param[in]    _start_bit Range first bit
 * @param[in]    _bit_count Number of bits to iterate over
 * @param[out]   _bit_no    Current bit number
 *
 * Given an uninitialed @p iter stroll_fbmap_iter iterator, iterate over bits
 * set to zero in @p _bmap stroll_fbmap bitmap and update @p _bit_no to reflect
 * current cleared bit.
 *
 * Iteration happens over the range defined as
 * [@p start_bit, @p start_bit + @p bit_count[.
 *
 * @see
 * - stroll_fbmap_foreach_range_set()
 * - stroll_fbmap_foreach_clear()
 */
#define stroll_fbmap_foreach_range_clear(_iter, \
                                         _bmap, \
                                         _start_bit, \
                                         _bit_count, \
                                         _bit_no) \
	for ((_bit_no) = stroll_fbmap_init_range_iter_clear(_iter, \
	                                                    _bmap, \
	                                                    _start_bit, \
	                                                    _bit_count); \
	     (_bit_no) >= 0; \
	     (_bit_no) = stroll_fbmap_step_iter_clear(_iter))

static inline __stroll_nonull(1, 2) __stroll_nothrow __warn_result
int
stroll_fbmap_init_iter_clear(struct stroll_fbmap_iter * __restrict  iter,
                             const struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_api(bmap);

	return stroll_fbmap_init_range_iter_clear(iter, bmap, 0, bmap->nr);
}

/**
 * Iterate over all bits cleared in a fixed sized bitmap.
 *
 * @param[inout] _iter   Temporary iteration bitmap
 * @param[in]    _bmap   Bitmap to iterate over
 * @param[out]   _bit_no Current bit number
 *
 * Given an uninitialed @p iter stroll_fbmap_iter iterator, iterate over bits
 * set to zero in @p _bmap stroll_fbmap bitmap and update @p _bit_no to reflect
 * current cleared bit.
 *
 * @see
 * - stroll_fbmap_foreach_range_clear()
 * - stroll_fbmap_foreach_set()
 */
#define stroll_fbmap_foreach_clear(_iter, _bmap, _bit_no) \
	for ((_bit_no) = stroll_fbmap_init_iter_clear(_iter, _bmap); \
	     (_bit_no) >= 0; \
	     (_bit_no) = stroll_fbmap_step_iter_clear(_iter))

#endif /* _STROLL_FBMAP_H */
