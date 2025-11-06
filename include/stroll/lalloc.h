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
 * @date      23 Oct 2025
 * @copyright Copyright (C) 2017-2025 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_LALLOC_H
#define _STROLL_LALLOC_H

#include <stroll/cdefs.h>
#include <stroll/priv/alloc_chunk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_lalloc_assert_api(_expr) \
	stroll_assert("stroll:lalloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_lalloc_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Pre-allocated large fixed sized object allocator.
 *
 * An opaque structure allowing to allocate large objects of constant size and
 * suitable for deriving custom allocators.
 *
 * This may be useful to:
 * - speed up allocations ;
 * - mitigate conventional memory heap fragmentation ;
 * - perform allocation in a more deterministic way.
 *
 * It may provide an alternative to #stroll_palloc allocator when initial
 * objects allocation fails because of number of objects, object size and / or
 * process memory restrictions.
 *
 * Basically, #stroll_lalloc is a primitive and lightweight allocator that
 * manages memory on heap thanks to calls to @man{malloc(3)} / @man{free(3)}.
 *
 * Memory objects, called *chunks*, are pre-allocated at allocator
 * initialization time thanks to one @man{malloc(3)} call per object.
 * Similarly, all pre-allocated chunks are released at allocator termination
 * time thanks to one call to @man{free(3)} per object.
 *
 * This makes this allocator suitable for workload where:
 * - the number of memory objects and their size are constant across the
 *   allocator lifetime ;
 * - and known at the time of allocator instantiation.
 *
 * @note
 * When all memory objects may be allocated thanks to a single call to
 * @man{malloc(3)}, #stroll_palloc may provide a suitable alternative.
 * See @man{malloc(3)}, @man{mallopt(3)} and @man{setrlimit(2)} for further
 * details.
 *
 * @see
 * - stroll_lalloc_init()
 * - stroll_lalloc_fini()
 * - stroll_lalloc_alloc()
 * - stroll_lalloc_free()
 */
struct stroll_lalloc {
	/**
	 * @internal
	 *
	 * Pointer to head of free memory chunk list.
	 * This points to the memory chunk returned by next call to
	 * stroll_lalloc_alloc().
	 */
	union stroll_alloc_chunk * next_free;
};

/**
 * Release the allocated chunk of memory given in argument.
 *
 * @param[inout] alloc Pre-allocated large fixed sized object allocator
 * @param[inout] chunk Chunk of memory to free
 *
 * Free resources allocated by @p alloc allocator for the chunk of memory @p
 * chunk.
 *
 * @p chunk *MUST* point to a chunk of memory returned by a call to
 * stroll_lalloc_alloc() using the same @p alloc allocator.
 *
 * @see
 * - stroll_lalloc_alloc()
 * - #stroll_lalloc
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_lalloc_free(struct stroll_lalloc * __restrict alloc, void * chunk)
{
	stroll_lalloc_assert_api(alloc);

	if (chunk) {
		union stroll_alloc_chunk * chnk = chunk;

		chnk->next_free = alloc->next_free;
		alloc->next_free = chnk;
	}
}

/**
 * Allocate a chunk of memory.
 *
 * @param[inout] alloc Pre-allocated large fixed sized object allocator
 *
 * @return A pointer to the allocated chunk of memory
 *
 * Request the @p alloc allocator to allocate and return a chunk of memory.
 * @p alloc *MUST* have been previously initialized using stroll_lalloc_init().
 *
 * The chunk returned is, at least, as large as the @p chunk_size argument given
 * to stroll_lalloc_init() at initialization time.
 * Its address and size are guaranteed to be aligned upon a machine word.
 *
 * @see
 * - stroll_lalloc_free()
 * - #stroll_lalloc
 */
static inline __stroll_nonull(1)
              __assume_align(sizeof(union stroll_alloc_chunk *))
              __stroll_nothrow
              __warn_result
void *
stroll_lalloc_alloc(struct stroll_lalloc * __restrict alloc)
{
	stroll_lalloc_assert_api(alloc);

	union stroll_alloc_chunk * chnk = alloc->next_free;

	if (chnk) {
		alloc->next_free = chnk->next_free;
		return chnk;
	}

	errno = ENOBUFS;

	return NULL;
}

/**
 * Initialize a pre-allocated fixed sized object allocator.
 *
 * @param[out] alloc      Pre-allocated large fixed sized object allocator
 * @param[in]  chunk_nr   Number of chunks per primary memory block
 * @param[in]  chunk_size Size of a single chunk of memory in bytes.
 *
 * Initialize a pre-allocated fixed sized object allocator so that it may
 * further allocate @p chunk_size bytes long memory chunks thanks to the
 * stroll_lalloc_alloc() function.
 *
 * @p chunk_nr specifies the minimum number of chunks that are allocated on heap
 * in a single call to @man{malloc(3)}.
 *
 * @p chunk_size specify the size of a single *chunk* of memory in bytes and
 * will be rounded up to the size of a machine word.
 *
 * @see
 * - stroll_lalloc_fini()
 * - #stroll_lalloc
 */
extern int
stroll_lalloc_init(struct stroll_lalloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   size_t                            chunk_size)
	__stroll_nonull(1) __stroll_nothrow __warn_result;

/**
 * Release all resources allocated by a pre-allocated fixed sized object
 * allocator.
 *
 * @param[inout] alloc Pre-allocated fixed sized object allocator
 *
 * Release all memory *chunks* allocated by the @p alloc allocator given in
 * argument.
 *
 * @see
 * - stroll_lalloc_init()
 * - #stroll_lalloc
 */
extern void
stroll_lalloc_fini(struct stroll_lalloc * __restrict alloc)
	__stroll_nonull(1) __stroll_nothrow __leaf;

#endif /* _STROLL_LALLOC_H */
