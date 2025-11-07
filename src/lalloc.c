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

#if defined(CONFIG_STROLL_ALLOC)

#include "alloc.h"

struct stroll_lalloc_impl {
	struct stroll_alloc  iface;
	struct stroll_lalloc lalloc;
};

static __stroll_nonull(1) __stroll_nothrow
void
stroll_lalloc_impl_free(struct stroll_alloc * __restrict alloc,
                        void * __restrict                chunk)
{
	stroll_lalloc_assert_intern(alloc);

	return stroll_lalloc_free(&((struct stroll_lalloc_impl *)alloc)->lalloc,
	                          chunk);
}

static __stroll_nonull(1)
       __malloc(stroll_lalloc_impl_free, 2)
       __assume_align(sizeof(union stroll_alloc_chunk *))
       __stroll_nothrow
       __warn_result
void *
stroll_lalloc_impl_alloc(struct stroll_alloc * __restrict alloc)
{
	stroll_lalloc_assert_intern(alloc);

	return stroll_lalloc_alloc(
		&((struct stroll_lalloc_impl *)alloc)->lalloc);
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_lalloc_impl_fini(struct stroll_alloc * __restrict alloc)
{
	stroll_lalloc_assert_intern(alloc);

	stroll_lalloc_fini(&((struct stroll_lalloc_impl *)alloc)->lalloc);
}

static const struct stroll_alloc_ops stroll_lalloc_impl_ops = {
	.alloc = stroll_lalloc_impl_alloc,
	.free  = stroll_lalloc_impl_free,
	.fini  = stroll_lalloc_impl_fini
};

struct stroll_alloc *
stroll_lalloc_create_alloc(unsigned int chunk_nr, size_t chunk_size)
{
	stroll_lalloc_assert_api(chunk_nr);
	stroll_lalloc_assert_api(chunk_size);

	struct stroll_lalloc_impl * alloc;
	int                         err;

	alloc = malloc(sizeof(*alloc));
	if (!alloc)
		return NULL;

	err = stroll_lalloc_init(&alloc->lalloc, chunk_nr, chunk_size);
	if (!err) {
		alloc->iface.ops = &stroll_lalloc_impl_ops;
		return &alloc->iface;
	}

	free(alloc);

	errno = -err;
	return NULL;
}

#endif /* defined(CONFIG_STROLL_ALLOC) */
