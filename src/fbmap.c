/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2020-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/fbmap.h"
#include "stroll/bops.h"
#include <errno.h>

#define stroll_fbmap_assert_range(_bmap, _start_bit, _bit_count) \
	stroll_fbmap_assert_map_api(_bmap); \
	stroll_fbmap_assert_api((_bmap)->nr); \
	stroll_fbmap_assert_api((_bmap)->nr <= INT_MAX); \
	stroll_fbmap_assert_api(_bit_count); \
	stroll_fbmap_assert_api((_start_bit) < (_bmap)->nr); \
	stroll_fbmap_assert_api(((_start_bit) + (_bit_count)) <= (_bmap)->nr)

/*
 * Return a bit mask suitable for masking out all bits lower that the bit index
 * given in argument.
 */
static __const __nothrow __warn_result
unsigned long
stroll_fbmap_word_high_mask(unsigned int bit_no)
{
	return ~(0UL) << stroll_fbmap_word_bit_no(bit_no);
}

/*
 * Return a bit mask suitable for masking out all bits higher that the bit index
 * given in argument.
 */
static __const __nothrow __warn_result
unsigned long
stroll_fbmap_word_low_mask(unsigned int bit_no)
{
	return ~(0UL) >> (__WORDSIZE - 1 - stroll_fbmap_word_bit_no(bit_no));
}

bool
stroll_fbmap_test_range(const struct stroll_fbmap * __restrict bmap,
                        unsigned int                           start_bit,
                        unsigned int                           bit_count)
{
	stroll_fbmap_assert_range(bmap, start_bit, bit_count);

	unsigned int  stop_bit = start_bit + bit_count - 1;
	unsigned int  curr = stroll_fbmap_word_no(start_bit);
	unsigned int  last = stroll_fbmap_word_no(stop_bit);
	unsigned long msb = stroll_fbmap_word_high_mask(start_bit);
	unsigned long lsb = stroll_fbmap_word_low_mask(stop_bit);

	if (curr != last) {
		/*
		 * Mask out unwanted least significant bits and check if 0 or
		 * not.
		 */
		if (bmap->bits[curr++] & msb)
			return true;
	}
	else
		/*
		 * Range applies to a single word only: mask out unwanted most
		 * and least significant bits and check if 0 or not.
		 */
		return !!(bmap->bits[curr] & msb & lsb);

	/* Iterate over all remaining words but the last and test if 0. */
	while (curr < last)
		if (bmap->bits[curr++])
			return true;

	/*
	 * Mask out last word's unwanted most significant bits and check if 0 or
	 * not.
	 */
	return !!(bmap->bits[curr] & lsb);
}

bool
stroll_fbmap_test_all(const struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	unsigned int w;

	for (w = 0; w < (stroll_fbmap_word_nr(bmap->nr) - 1); w++) {
		if (bmap->bits[w])
			return true;
	}

	if (bmap->bits[w] & stroll_fbmap_word_low_mask(bmap->nr - 1))
		return true;

	return false;
}

void
stroll_fbmap_toggle_all(struct stroll_fbmap * __restrict bmap)
{
	stroll_fbmap_assert_map_api(bmap);

	unsigned int w;

	for (w = 0; w < stroll_fbmap_word_nr(bmap->nr); w++)
		bmap->bits[w] = ~bmap->bits[w];
}

int
stroll_fbmap_init_set(struct stroll_fbmap * __restrict bmap,
                      unsigned int                     bit_nr)
{
	stroll_fbmap_assert_api(bmap);
	stroll_fbmap_assert_api(bit_nr);
	stroll_fbmap_assert_api(bit_nr <= INT_MAX);

	unsigned int wnr = stroll_fbmap_word_nr(bit_nr);

	bmap->bits = malloc(wnr * sizeof(bmap->bits[0]));
	if (!bmap->bits)
		return -ENOMEM;

	memset(bmap->bits, 0xff, wnr * sizeof(bmap->bits[0]));

	bmap->nr = bit_nr;

	return 0;
}

int
stroll_fbmap_init_clear(struct stroll_fbmap * __restrict bmap,
                        unsigned int                     bit_nr)
{
	stroll_fbmap_assert_api(bmap);
	stroll_fbmap_assert_api(bit_nr);
	stroll_fbmap_assert_api(bit_nr <= INT_MAX);

	bmap->bits = calloc(stroll_fbmap_word_nr(bit_nr),
	                    sizeof(bmap->bits[0]));
	if (!bmap->bits)
		return -ENOMEM;

	bmap->nr = bit_nr;

	return 0;
}

#define stroll_fbmap_assert_iter(_iter) \
	stroll_fbmap_assert_api(_iter); \
	stroll_fbmap_assert_map_api((_iter)->bmap); \
	stroll_fbmap_assert_api((_iter)->nr <= (_iter)->bmap->nr); \
	stroll_fbmap_assert_api((_iter)->curr < stroll_fbmap_word_nr(iter->nr))

int
stroll_fbmap_step_iter(struct stroll_fbmap_iter * __restrict iter)
{
	stroll_fbmap_assert_iter(iter);

	unsigned int bit_no;

	while (!iter->word) {
		if (++iter->curr >= stroll_fbmap_word_nr(iter->nr))
			return -ENOENT;

		iter->word = iter->bmap->bits[iter->curr];
	}

	bit_no = stroll_bops_ffs(iter->word) - 1;
	iter->word &= ~(1UL << bit_no);

	bit_no += (iter->curr * __WORDSIZE);
	if (bit_no >= iter->nr)
		return -ENOENT;

	return (int)bit_no;
}

int
stroll_fbmap_init_range_iter(struct stroll_fbmap_iter * __restrict  iter,
                             const struct stroll_fbmap * __restrict bmap,
                             unsigned int                           start_bit,
                             unsigned int                           bit_count)
{
	stroll_fbmap_assert_api(iter);
	stroll_fbmap_assert_range(bmap, start_bit, bit_count);

	iter->curr = stroll_fbmap_word_no(start_bit);
	iter->word = bmap->bits[iter->curr] &
	             stroll_fbmap_word_high_mask(start_bit);
	iter->nr = start_bit + bit_count;
	iter->bmap = bmap;

	return stroll_fbmap_step_iter(iter);
}
