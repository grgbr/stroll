/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2020-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/fbmap.h"
#include "stroll/bops.h"
#include <errno.h>

#define stroll_fbmap_assert_range(_bits, _start_bit, _bit_count) \
	stroll_fbmap_assert_api(_bits); \
	stroll_fbmap_assert_api(_bit_count); \
	stroll_fbmap_assert_api(((_start_bit) + (_bit_count)) <= \
	                        (unsigned int)INT_MAX)

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

unsigned int
_stroll_fbmap_hweight(const unsigned long * __restrict bits, unsigned int nr)
{
	stroll_fbmap_assert_api(bits);

	unsigned int w, hw = 0;

	for (w = 0; w < (stroll_fbmap_word_nr(nr) - 1); w++)
		hw += stroll_bops_hweightul(bits[w]);

	return hw +
	       stroll_bops_hweightul(bits[w] &
	                             stroll_fbmap_word_low_mask(nr - 1));
}

bool
_stroll_fbmap_test_range(const unsigned long * __restrict bits,
                         unsigned int                     start_bit,
                         unsigned int                     bit_count)
{
	stroll_fbmap_assert_range(bits, start_bit, bit_count);

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
		if (bits[curr++] & msb)
			return true;
	}
	else
		/*
		 * Range applies to a single word only: mask out unwanted most
		 * and least significant bits and check if 0 or not.
		 */
		return !!(bits[curr] & msb & lsb);

	/* Iterate over all remaining words but the last and test if 0. */
	while (curr < last)
		if (bits[curr++])
			return true;

	/*
	 * Mask out last word's unwanted most significant bits and check if 0 or
	 * not.
	 */
	return !!(bits[curr] & lsb);
}

bool
_stroll_fbmap_test_all(const unsigned long * __restrict bits, unsigned int nr)
{
	stroll_fbmap_assert_bits_api(bits, nr);

	unsigned int w;

	for (w = 0; w < (stroll_fbmap_word_nr(nr) - 1); w++) {
		if (bits[w])
			return true;
	}

	if (bits[w] & stroll_fbmap_word_low_mask(nr - 1))
		return true;

	return false;
}

void
_stroll_fbmap_toggle_all(unsigned long * __restrict bits, unsigned int nr)
{
	stroll_fbmap_assert_bits_api(bits, nr);

	unsigned int w;

	for (w = 0; w < stroll_fbmap_word_nr(nr); w++)
		bits[w] = ~bits[w];
}

unsigned long *
_stroll_fbmap_create_bits_clear(unsigned int bit_nr)
{
	stroll_fbmap_assert_api(bit_nr);
	stroll_fbmap_assert_api(bit_nr <= (unsigned int)INT_MAX);

	unsigned long * bits;
	unsigned int    sz = stroll_fbmap_word_nr(bit_nr) * sizeof(bits[0]);

	bits = malloc(sz);
	if (!bits)
		return NULL;

	memset(bits, 0, sz);

	return bits;
}

int
stroll_fbmap_init_clear(struct stroll_fbmap * __restrict bmap,
                        unsigned int                     bit_nr)
{
	stroll_fbmap_assert_api(bmap);
	stroll_fbmap_assert_api(bit_nr);
	stroll_fbmap_assert_api(bit_nr <= (unsigned int)INT_MAX);

	bmap->bits = _stroll_fbmap_create_bits_clear(bit_nr);
	if (!bmap->bits)
		return -errno;

	bmap->nr = bit_nr;

	return 0;
}

unsigned long *
_stroll_fbmap_create_bits_set(unsigned int bit_nr)
{
	stroll_fbmap_assert_api(bit_nr);
	stroll_fbmap_assert_api(bit_nr <= (unsigned int)INT_MAX);

	unsigned long * bits;
	unsigned int    sz = stroll_fbmap_word_nr(bit_nr) * sizeof(bits[0]);

	bits = malloc(sz);
	if (!bits)
		return NULL;

	memset(bits, 0xff, sz);

	return bits;
}

int
stroll_fbmap_init_set(struct stroll_fbmap * __restrict bmap,
                      unsigned int                     bit_nr)
{
	stroll_fbmap_assert_api(bmap);
	stroll_fbmap_assert_api(bit_nr);
	stroll_fbmap_assert_api(bit_nr <= (unsigned int)INT_MAX);

	bmap->bits = _stroll_fbmap_create_bits_set(bit_nr);
	if (!bmap->bits)
		return -errno;

	bmap->nr = bit_nr;

	return 0;
}

unsigned long *
_stroll_fbmap_create_bits_dup(unsigned long * __restrict src,
                              unsigned int               bit_nr)
{
	stroll_fbmap_assert_api(src);
	stroll_fbmap_assert_api(bit_nr);
	stroll_fbmap_assert_api(bit_nr <= (unsigned int)INT_MAX);

	unsigned long * bits;
	unsigned int    sz = stroll_fbmap_word_nr(bit_nr) * sizeof(bits[0]);

	bits = malloc(sz);
	if (!bits)
		return NULL;

	memcpy(bits, src, sz);

	return bits;
}

int
stroll_fbmap_init_dup(struct stroll_fbmap * __restrict       bmap,
                      const struct stroll_fbmap * __restrict src)
{
	stroll_fbmap_assert_api(bmap);
	stroll_fbmap_assert_map_api(src);

	bmap->bits = _stroll_fbmap_create_bits_dup(src->bits, src->nr);
	if (!bmap->bits)
		return -errno;

	bmap->nr = src->nr;

	return 0;
}

#define stroll_fbmap_assert_iter(_iter) \
	stroll_fbmap_assert_api(_iter); \
	stroll_fbmap_assert_bits_api((_iter)->bits, (_iter)->nr); \
	stroll_fbmap_assert_api((_iter)->curr < stroll_fbmap_word_nr(iter->nr))

int
stroll_fbmap_step_iter_set(struct stroll_fbmap_iter * __restrict iter)
{
	stroll_fbmap_assert_iter(iter);

	unsigned int bit_no;

	while (!iter->word) {
		if (++iter->curr >= stroll_fbmap_word_nr(iter->nr))
			return -ENOENT;

		iter->word = iter->bits[iter->curr];
	}

	bit_no = stroll_bops_ffsul(iter->word) - 1;
	iter->word &= ~(1UL << bit_no);

	bit_no += (iter->curr * __WORDSIZE);
	if (bit_no >= iter->nr)
		return -ENOENT;

	return (int)bit_no;
}

int
_stroll_fbmap_init_range_iter_set(
	struct stroll_fbmap_iter * __restrict iter,
	const unsigned long * __restrict      bits,
	unsigned int                          start_bit,
	unsigned int                          bit_count)
{
	stroll_fbmap_assert_api(iter);
	stroll_fbmap_assert_range(bits, start_bit, bit_count);

	iter->curr = stroll_fbmap_word_no(start_bit);
	iter->word = bits[iter->curr] & stroll_fbmap_word_high_mask(start_bit);
	iter->nr = start_bit + bit_count;
	iter->bits = bits;

	return stroll_fbmap_step_iter_set(iter);
}

int
stroll_fbmap_step_iter_clear(struct stroll_fbmap_iter * __restrict iter)
{
	stroll_fbmap_assert_iter(iter);

	unsigned int bit_no;

	while (iter->word == ~(0UL)) {
		if (++iter->curr >= stroll_fbmap_word_nr(iter->nr))
			return -ENOENT;

		iter->word = iter->bits[iter->curr];
	}

	bit_no = stroll_bops_ffcul(iter->word) - 1;
	iter->word |= 1UL << bit_no;

	bit_no += (iter->curr * __WORDSIZE);
	if (bit_no >= iter->nr)
		return -ENOENT;

	return (int)bit_no;
}

int
_stroll_fbmap_init_range_iter_clear(
	struct stroll_fbmap_iter * __restrict iter,
	const unsigned long * __restrict      bits,
	unsigned int                          start_bit,
	unsigned int                          bit_count)
{
	stroll_fbmap_assert_api(iter);
	stroll_fbmap_assert_range(bits, start_bit, bit_count);

	iter->curr = stroll_fbmap_word_no(start_bit);
	iter->word = bits[iter->curr] | ~stroll_fbmap_word_high_mask(start_bit);
	iter->nr = start_bit + bit_count;
	iter->bits = bits;

	return stroll_fbmap_step_iter_clear(iter);
}
