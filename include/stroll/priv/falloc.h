/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _STROLL_PRIV_FALLOC_H
#define _STROLL_PRIV_FALLOC_H

#include <stroll/priv/alloc_chunk.h>
#include <stroll/pow2.h>

#define stroll_falloc_assert_alloc_api(_alloc) \
	stroll_falloc_assert_api(_alloc); \
	stroll_falloc_assert_api((_alloc)->chunk_nr); \
	stroll_falloc_assert_api((_alloc)->chunk_cnt <= (_alloc)->chunk_nr); \
	stroll_falloc_assert_api( \
		stroll_aligned((_alloc)->chunk_sz, \
		               sizeof_member(union stroll_alloc_chunk, \
		                             next_free))); \
	stroll_falloc_assert_api((_alloc)->chunk_per_block > 1); \
	stroll_falloc_assert_api( \
		(_alloc)->block_sz >= \
		(sizeof(struct stroll_falloc_block) + \
		 ((_alloc)->chunk_per_block * (_alloc)->chunk_sz))); \
	stroll_falloc_assert_api( \
		(_alloc)->block_al == \
		(1UL << stroll_pow2_upul((_alloc)->block_sz)))

struct stroll_falloc_block {
	unsigned int               busy_cnt;  /* Count of allocated chunks */
	union stroll_alloc_chunk * next_free; /* Pointer to next free chunk */
	struct stroll_dlist_node   node;
	union stroll_alloc_chunk   chunks[0];
};

#endif /* _STROLL_PRIV_FALLOC_H */
