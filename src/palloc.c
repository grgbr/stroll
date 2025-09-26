/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/palloc.h"
#include "stroll/pow2.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_palloc_assert_intern(_expr) \
	stroll_assert("stroll:palloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_palloc_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

void
_stroll_palloc_init_from_mem(struct stroll_palloc * __restrict alloc,
                             void * __restrict                 mem,
                             unsigned int                      chunk_nr,
                             size_t                            chunk_size,
                             bool                              owner)
{
	stroll_palloc_assert_api(alloc);
	stroll_palloc_assert_api(mem);
	stroll_palloc_assert_api(stroll_aligned((unsigned long)mem,
	                                        sizeof((alloc)->next_free)));
	stroll_palloc_assert_api(chunk_nr > 1);
	stroll_palloc_assert_api(chunk_size);
	stroll_palloc_assert_api(stroll_aligned(chunk_size,
	                                        sizeof((alloc)->next_free)));

	union stroll_palloc_chunk * chnk;
	union stroll_palloc_chunk * last;

	chnk = mem;
	last = (void *)chnk + ((chunk_nr - 1) * chunk_size);
	do {
		union stroll_palloc_chunk * next = (void *)chnk + chunk_size;

		chnk->next_free = next;
		chnk = next;
	} while (chnk < last);

	stroll_palloc_assert_intern(chnk == last);
	chnk->next_free = NULL;

	alloc->next_free = mem;
	alloc->chunks = mem;
	alloc->chunk_sz = chunk_size;
	alloc->own = owner;
}

int
stroll_palloc_init(struct stroll_palloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   size_t                            chunk_size)
{
	stroll_palloc_assert_api(alloc);
	stroll_palloc_assert_api(chunk_nr > 1);
	stroll_palloc_assert_api(chunk_size);

	void * chunks;
	int    err;

	chunk_size = stroll_align_upper(chunk_size, sizeof(alloc->next_free));

	err = posix_memalign(&chunks,
	                     sizeof(alloc->next_free),
	                     1UL << stroll_pow2_upul((size_t)chunk_nr *
	                                             chunk_size));
	if (err) {
		stroll_palloc_assert_intern(err == ENOMEM);
		return -ENOMEM;
	}

	_stroll_palloc_init_from_mem(alloc, chunks, chunk_nr, chunk_size, true);

	return 0;
}
