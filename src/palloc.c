/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/palloc.h"

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
	stroll_palloc_assert_api(chunk_nr);
	stroll_palloc_assert_api(chunk_size);
	stroll_palloc_assert_api(stroll_aligned(chunk_size,
	                                        sizeof((alloc)->next_free)));

	union stroll_alloc_chunk * chnk;
	union stroll_alloc_chunk * last;

	chnk = mem;
	last = (void *)chnk + ((chunk_nr - 1) * chunk_size);
	while (chnk < last) {
		union stroll_alloc_chunk * next = (void *)chnk + chunk_size;

		chnk->next_free = next;
		chnk = next;
	}

	stroll_palloc_assert_intern(chnk == last);
	chnk->next_free = NULL;

	alloc->next_free = mem;
	alloc->chunks = mem;
	alloc->own = owner;
}

int
stroll_palloc_init(struct stroll_palloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   size_t                            chunk_size)
{
	stroll_palloc_assert_api(alloc);
	stroll_palloc_assert_api(chunk_nr);
	stroll_palloc_assert_api(chunk_size);

	void * chunks;

	chunk_size = stroll_align_upper(chunk_size, sizeof(alloc->next_free));

	chunks = malloc(chunk_nr * chunk_size);
	if (!chunks)
		return -ENOMEM;

	_stroll_palloc_init_from_mem(alloc, chunks, chunk_nr, chunk_size, true);

	return 0;
}

#if defined(CONFIG_STROLL_ALLOC)

#include "alloc.h"

struct stroll_palloc_impl {
	struct stroll_alloc  iface;
	struct stroll_palloc palloc;
};

static __stroll_nonull(1) __malloc(free, 1) __stroll_nothrow __warn_result
struct stroll_palloc_impl *
_stroll_palloc_create_alloc(const struct stroll_alloc_ops * __restrict ops)
{
	stroll_alloc_assert_ops_intern(ops);

	struct stroll_palloc_impl * alloc;

	alloc = malloc(sizeof(*alloc));
	if (!alloc)
		return NULL;

	alloc->iface.ops = ops;

	return alloc;
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_palloc_impl_free(struct stroll_alloc * __restrict alloc,
                        void * __restrict                chunk)
{
	stroll_palloc_assert_intern(alloc);

	return stroll_palloc_free(&((struct stroll_palloc_impl *)alloc)->palloc,
	                          chunk);
}

static __stroll_nonull(1)
       __malloc(stroll_palloc_impl_free, 2)
       __assume_align(sizeof(union stroll_alloc_chunk *))
       __stroll_nothrow
       __warn_result
void *
stroll_palloc_impl_alloc(struct stroll_alloc * __restrict alloc)
{
	stroll_palloc_assert_intern(alloc);

	return stroll_palloc_alloc(
		&((struct stroll_palloc_impl *)alloc)->palloc);
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_palloc_impl_fini(struct stroll_alloc * __restrict alloc)
{
	stroll_palloc_assert_intern(alloc);

	free(((struct stroll_palloc_impl *)alloc)->palloc.chunks);
}

static const struct stroll_alloc_ops stroll_palloc_impl_ops = {
	.alloc = stroll_palloc_impl_alloc,
	.free  = stroll_palloc_impl_free,
	.fini  = stroll_palloc_impl_fini
};

struct stroll_alloc *
stroll_palloc_create_alloc(unsigned int chunk_nr, size_t chunk_size)
{
	stroll_palloc_assert_api(chunk_nr);
	stroll_palloc_assert_api(chunk_size);

	struct stroll_palloc_impl * alloc;
	int                         err;

	alloc = _stroll_palloc_create_alloc(&stroll_palloc_impl_ops);
	if (!alloc)
		return NULL;

	err = stroll_palloc_init(&alloc->palloc, chunk_nr, chunk_size);
	if (!err)
		return &alloc->iface;

	free(alloc);

	errno = -err;
	return NULL;
}

static __stroll_nonull(1) __stroll_nothrow
void
stroll_palloc_from_mem_impl_fini(
	struct stroll_alloc * __restrict alloc __unused)
{
	stroll_palloc_assert_intern(alloc);
}

static const struct stroll_alloc_ops stroll_palloc_from_mem_impl_ops = {
	.alloc = stroll_palloc_impl_alloc,
	.free  = stroll_palloc_impl_free,
	.fini  = stroll_palloc_from_mem_impl_fini
};

struct stroll_alloc *
stroll_palloc_create_alloc_from_mem(void * __restrict mem,
                                    unsigned int      chunk_nr,
                                    size_t            chunk_size)
{
	stroll_palloc_assert_api(mem);
	stroll_palloc_assert_api(chunk_nr);
	stroll_palloc_assert_api(chunk_size);

	struct stroll_palloc_impl * alloc;

	alloc = _stroll_palloc_create_alloc(&stroll_palloc_from_mem_impl_ops);
	if (!alloc)
		return NULL;

	stroll_palloc_init_from_mem(&alloc->palloc, mem, chunk_nr, chunk_size);

	return &alloc->iface;
}

#endif /* defined(CONFIG_STROLL_ALLOC) */
