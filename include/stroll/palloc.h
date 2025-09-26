/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Pre-allocated fixed sized object allocator interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      25 Dec 2025
 * @copyright Copyright (C) 2017-2025 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_PALLOC_H
#define _STROLL_PALLOC_H

#include <stroll/cdefs.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_palloc_assert_api(_expr) \
	stroll_assert("stroll:palloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_palloc_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * @internal
 *
 * Primary stroll_palloc memory chunk.
 */
union stroll_palloc_chunk {
	/**
	 * @internal
	 *
	 * Link to next chunk within the free chunk list.
	 */
	union stroll_palloc_chunk * next_free;
	/**
	 * @internal
	 *
	 * Chunk data area.
	 */
	char                        data[0];
};


/**
 * Pre-allocated fixed sized object allocator.
 *
 * An opaque structure allowing to allocate objects of constant size and
 * suitable for deriving custom allocators.
 *
 * This may be useful to:
 * - speed up allocations ;
 * - mitigate conventional memory heap fragmentation ;
 * - perform allocation in a more deterministic way.
 *
 * Basically, stroll_palloc is a primitive and lightweight allocator that
 * manages memory on heap thanks to calls to @man{malloc(3)} / @man{free(3)}.
 *
 * Memory objects, called *chunks*, are pre-allocated at allocator
 * initialization time thanks to a single @man{malloc(3)} call.
 * Similarly, all pre-allocated chunks are released at allocator termination
 * time thanks to a single call to @man{free(3)}.
 *
 * This makes this allocator suitable for workload where:
 * - the number of memory objects and their size are constant across the
 *   allocator lifetime ;
 * - and known at the time of allocator instantiation.
 *
 * @note
 * All managed memory being allocated in a single call to @man{malloc(3)},
 * stroll_palloc is highly subject to object size restrictions and process
 * memory limits.
 * See @man{malloc(3)}, @man{mallopt(3)} and @man{setrlimit(2)} for further
 * details.
 *
 * @see
 * - stroll_palloc_init()
 * - stroll_palloc_fini()
 * - stroll_palloc_alloc()
 * - stroll_palloc_free()
 */
struct stroll_palloc {
	/**
	 * @internal
	 *
	 * Pointer to head of free memory chunk list.
	 * This points to the memory chunk returned by next call to
	 * stroll_palloc_alloc().
	 */
	union stroll_palloc_chunk * next_free; /* Pointer to next free chunk */
	/**
	 * @internal
	 *
	 * Memory area holding chunks.
	 */
	void *                      chunks;
	/**
	 * @internal
	 *
	 * Size of a single memory chunk.
	 */
	size_t                      chunk_sz;
	/**
	 * @internal
	 *
	 * Indicate wether this allocator owns the memory area pointed to by
	 * stroll_palloc::chunks.
	 */
	bool                        own;
};

#define stroll_palloc_assert_alloc_api(_alloc) \
	stroll_palloc_assert_api(_alloc); \
	stroll_palloc_assert_api( \
		stroll_aligned((_alloc)->chunk_sz, \
		               sizeof((_alloc)->next_free))); \
	stroll_palloc_assert_api((_alloc)->chunks)

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_palloc_free(struct stroll_palloc * __restrict alloc, void * chunk)
{
	stroll_palloc_assert_alloc_api(alloc);
	stroll_palloc_assert_api(chunk >= alloc->chunks);

	if (chunk) {
		union stroll_palloc_chunk * chnk = chunk;

		chnk->next_free = alloc->next_free;
		alloc->next_free = chnk;
	}
}

static inline __stroll_nonull(1)
              __assume_align(sizeof(union stroll_palloc_chunk *))
              __stroll_nothrow
              __warn_result
void *
stroll_palloc_alloc(struct stroll_palloc * __restrict alloc)
{
	stroll_palloc_assert_alloc_api(alloc);

	union stroll_palloc_chunk * chnk = alloc->next_free;

	if (chnk) {
		alloc->next_free = chnk->next_free;
		return chnk;
	}

	errno = ENOBUFS;

	return NULL;
}

extern void
_stroll_palloc_init_from_mem(struct stroll_palloc * __restrict alloc,
                             void * __restrict                 mem,
                             unsigned int                      chunk_nr,
                             size_t                            chunk_size,
                             bool                              owner)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf;

static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_palloc_init_from_mem(struct stroll_palloc * __restrict alloc,
                            void * __restrict                 mem,
                            unsigned int                      chunk_nr,
                            size_t                            chunk_size)
{
	_stroll_palloc_init_from_mem(alloc, mem, chunk_nr, chunk_size, false);
}

extern int
stroll_palloc_init(struct stroll_palloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   size_t                            chunk_size)
	__stroll_nonull(1) __stroll_nothrow;

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_palloc_fini(struct stroll_palloc * __restrict alloc)
{
	stroll_palloc_assert_alloc_api(alloc);

	if (alloc->own)
		free(alloc->chunks);
}

#endif /* _STROLL_PALLOC_H */
