/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/lalloc.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_lalloc_assert_intern(_expr) \
	stroll_assert("stroll:lalloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_lalloc_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

void
stroll_lalloc_fini(struct stroll_lalloc * __restrict alloc)
{
	stroll_lalloc_assert_api(alloc);

	while (alloc->next_free) {
		union stroll_alloc_chunk * chnk = alloc->next_free;

		alloc->next_free = chnk->next_free;

		free(chnk);
	}
}

int
stroll_lalloc_init(struct stroll_lalloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   size_t                            chunk_size)
{
	stroll_lalloc_assert_api(alloc);
	stroll_lalloc_assert_api(chunk_nr);
	stroll_lalloc_assert_api(chunk_size);

	unsigned int c;

	chunk_size = stroll_align_upper(chunk_size, sizeof(alloc->next_free));

	alloc->next_free = NULL;
	for (c = 0; c < chunk_nr; c++) {
		union stroll_alloc_chunk * chnk = malloc(chunk_size);

		if (!chnk)
			goto free;

		chnk->next_free = alloc->next_free;
		alloc->next_free = chnk;
	}

	return 0;

free:
	stroll_lalloc_fini(alloc);

	return -ENOMEM;
}
