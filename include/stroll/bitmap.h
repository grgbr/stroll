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

static inline bool __stroll_nonull(1) __stroll_pure __nothrow __warn_result
stroll_bmap32_test(const uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < stroll_bops_bitsof(*bmap);

	return !!(*bmap & (1U << bit_no));
}

static inline uint32_t __stroll_const __nothrow __warn_result
stroll_bmap32_mask(unsigned int start_bit, unsigned int bit_count)
{
	stroll_bmap_assert_intern(bit_count);
	stroll_bmap_assert_intern((start_bit + bit_count) <= 32);

	~(0U) >> (32 - (start_bit + bit_count));
}

static inline bool __stroll_nonull(1) __stroll_pure __nothrow __warn_result
stroll_bmap32_test_mask(const uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(mask);

	return !!(*bmap & mask);
}

static inline bool __stroll_nonull(1) __stroll_pure __nothrow __warn_result
stroll_bmap32_test_range(const uint32_t * bmap,
                         unsigned int     start_bit,
                         unsigned int     bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_test_mask(bmap,
	                               stroll_bmap32_mask(start_bit,
	                                                  bit_count));
}

static inline bool __stroll_nonull(1) __stroll_pure __nothrow __warn_result
stroll_bmap32_test_all(const uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	return !!*bmap;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	return *bmap |= 1U << bit_no;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(mask);

	*bmap |= mask;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set_range(uint32_t *   bmap,
                        unsigned int start_bit,
                        unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_set_mask(bmap,
	                              stroll_bmap32_mask(start_bit,
	                                                 bit_count));
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_set_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	return *bmap = ~(0U);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	return *bmap &= ~(1U << bit_no);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(mask);

	*bmap &= ~mask;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear_range(uint32_t *   bmap,
                          unsigned int start_bit,
                          unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_clear_mask(bmap,
	                                stroll_bmap32_mask(start_bit,
	                                                   bit_count));
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_clear_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	return *bmap = 0;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle(uint32_t * bmap, unsigned int bit_no)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_no < 32);

	return *bmap ^= 1U << bit_no;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle_mask(uint32_t * bmap, uint32_t mask)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(mask);

	return *bmap ^= mask;
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle_range(uint32_t *   bmap,
                           unsigned int start_bit,
                           unsigned int bit_count)
{
	stroll_bmap_assert_api(bmap);
	stroll_bmap_assert_api(bit_count);
	stroll_bmap_assert_api((start_bit + bit_count) <= 32);

	return stroll_bmap32_toggle_mask(bmap,
	                                 stroll_bmap32_mask(start_bit,
	                                                    bit_count));
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_toggle_all(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);

	return *bmap ^= ~(*bmap);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_init_set(uint32_t * bmap)
{
	stroll_bmap32_set_all(bmap);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_init_clear(uint32_t * bmap)
{
	stroll_bmap32_clear_all(bmap);
}

static inline void __stroll_nonull(1) __nothrow
stroll_bmap32_fini(uint32_t * bmap)
{
	stroll_bmap_assert_api(bmap);
}

struct stroll_bmap32_iter {
	unsigned int tmp;
	unsigned int diff;
};

static inline unsigned int __stroll_nonull(1) __nothrow
stroll_bmap32_step_iter(struct stroll_bmap32_iter * iter, unsigned int bit_no)
{
	stroll_bmap_assert_api(iter);

	iter->tmp >>= iter->diff;
	iter->diff = stroll_bops_ffs32(iter->tmp);

	return bit_no + iter->diff;
}

static inline bool __stroll_nonull(1) __stroll_pure __nothrow
stroll_bmap32_iter_end(const struct stroll_bmap32_iter * iter)
{
	stroll_bmap_assert_api(iter);

	return !iter->tmp;
}

static inline unsigned int __stroll_nonull(1, 2) __nothrow
stroll_bmap32_init_iter(struct stroll_bmap32_iter * iter, uint32_t * bmap)
{
	stroll_bmap_assert_api(iter);
	stroll_bmap_assert_api(bmap);

	iter->tmp = *bmap;
	iter->diff = stroll_bops_ffs32(iter->tmp);

	return iter->diff - 1;
}

#define stroll_bmap32_foreach(_iter, _bmap, _bit_no) \
	for ((_bit_no) = stroll_bmap32_init_iter(_iter, _bmap); \
	     !stroll_bmap32_iter_end(_iter); \
	     (_bit_no) = stroll_bmap32_step_iter(_iter, _bit_no))

#endif /* _STROLL_BITMAP_H */
