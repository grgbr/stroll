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

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
unsigned int
stroll_fbmap_nr(const struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	return bmap->nr;
}

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

extern bool
stroll_fbmap_test_range(const struct stroll_fbmap * __restrict bmap,
                        unsigned int                           start_bit,
                        unsigned int                           bit_count)
	__stroll_nonull(1) __stroll_pure __stroll_nothrow __leaf __warn_result;

extern bool
stroll_fbmap_test_all(const struct stroll_fbmap * __restrict bmap)
	__stroll_nonull(1) __stroll_pure __stroll_nothrow __leaf __warn_result;

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_set(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
{
	stroll_fbmap_assert_map_api( bmap);
	stroll_fbmap_assert_api(bit_no < bmap->nr);

	 bmap->bits[stroll_fbmap_word_no(bit_no)] |=
		stroll_fbmap_word_bit_mask(bit_no);
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_set_all(struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	memset(bmap->bits,
	       0xff,
	       stroll_fbmap_word_nr(bmap->nr) * sizeof(bmap->bits[0]));
}


static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_clear(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
{
	stroll_fbmap_assert_map_api(bmap);
	stroll_fbmap_assert_api(bit_no < bmap->nr);

	bmap->bits[stroll_fbmap_word_no(bit_no)] &=
		~(stroll_fbmap_word_bit_mask(bit_no));
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_clear_all(struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	memset(bmap->bits,
	       0,
	       stroll_fbmap_word_nr(bmap->nr) * sizeof(bmap->bits[0]));
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_fbmap_toggle(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
{
	stroll_fbmap_assert_map_api(bmap);
	stroll_fbmap_assert_api(bit_no < bmap->nr);

	bmap->bits[stroll_fbmap_word_no(bit_no)] ^=
		stroll_fbmap_word_bit_mask(bit_no);
}

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

#define stroll_fbmap_foreach_set(_iter, _bmap, _bit_no) \
	for ((_bit_no) = stroll_fbmap_init_iter_set(_iter, _bmap); \
	     (_bit_no) >= 0; \
	     (_bit_no) = stroll_fbmap_step_iter_set(_iter))

#endif /* _STROLL_FBMAP_H */
