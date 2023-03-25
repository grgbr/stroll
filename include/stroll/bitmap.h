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
#ifndef _STROLL_BITMAP_H
#define _STROLL_BITMAP_H

#include <stroll/bitops.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_bmap_assert_api(_expr) \
	stroll_assert("stroll: bmap", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_bmap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERNAL)

#include <stroll/assert.h>

#define stroll_bmap_assert_intern(_expr) \
	stroll_assert("stroll: bmap", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_bmap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static inline uint32_t __stroll_const __nothrow __warn_result
stroll_bmap32_mask(unsigned int start_bit, unsigned int bit_count)
{
	/*
	 * Intel right shift instruction cannot shift more than the number of
	 * available register bits minus one, i.e., here no more than 31 bits.
	 */
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return (~(0U) >> (32 - bit_count)) << start_bit;
}

static inline unsigned int __const __nothrow __warn_result
stroll_bmap32_hweight(uint32_t bmap)
{
	return stroll_bops32_hweight(bmap);
}

static inline uint32_t __const __nothrow __warn_result
stroll_bmap32_and(uint32_t bmap, uint32_t mask)
{
	return bmap & mask;
}

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

static inline uint32_t __const __nothrow __warn_result
stroll_bmap32_or(uint32_t bmap, uint32_t mask)
{
	return bmap | mask;
}

static inline uint32_t __stroll_const __nothrow __warn_result
stroll_bmap32_or_range(uint32_t     bmap,
                       unsigned int start_bit,
                       unsigned int bit_count)
{
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_or(bmap, stroll_bmap32_mask(start_bit, bit_count));
}

static inline uint32_t __const __nothrow __warn_result
stroll_bmap32_xor(uint32_t bmap, uint32_t mask)
{
	return bmap ^ mask;
}

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

static inline bool __const __nothrow __warn_result
stroll_bmap32_test_mask(uint32_t bmap, uint32_t mask)
{
	return !!stroll_bmap32_and(bmap, mask);
}

static inline bool __stroll_const __nothrow __warn_result
stroll_bmap32_test(uint32_t bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bit_no < stroll_bops_bitsof(bmap));

	return stroll_bmap32_test_mask(bmap, 1U << bit_no);
}

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

static inline bool __const __nothrow __warn_result
stroll_bmap32_test_all(uint32_t bmap)
{
	return !!bmap;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap32_or(*bmap, mask);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap32_set_mask(bmap, 1U << bit_no);
}

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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = ~(0U);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap32_and(*bmap, ~mask);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap32_clear_mask(bmap, 1U << bit_no);
}

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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	*bmap = 0;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);

	*bmap = stroll_bmap32_xor(*bmap, mask);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	stroll_bmap32_toggle_mask(bmap, 1U << bit_no);
}

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

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap32_toggle_mask(bmap, ~(0U));
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_init_set(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap32_set_all(bmap);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_init_clear(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	stroll_bmap32_clear_all(bmap);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_fini(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);
}

static inline int __stroll_nonull(1, 2) __nothrow
stroll_bmap32_step_iter(uint32_t * iter, uint32_t * bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	if (*iter) {
		uint32_t diff = stroll_bops32_ffs(*iter);

		*bit_no += diff;
		*iter >>= diff;

		return 0;
	}
	else
		return -1;
}

static inline void __stroll_nonull(1, 3) __nothrow
stroll_bmap32_setup_iter(uint32_t * iter, uint32_t bmap, unsigned int * bit_no)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bit_no);

	/* Will wrap around at next stroll_bmap32_step_iter() call. */
	*bit_no = -1;
	*iter = bmap;
}

#define stroll_bmap32_foreach(_iter, _bmap, _bit_no) \
	for (stroll_bmap32_setup_iter(_iter, _bmap, _bit_no); \
	     !stroll_bmap32_step_iter(_iter, _bit_no); \
	    )

#endif /* _STROLL_BITMAP_H */
