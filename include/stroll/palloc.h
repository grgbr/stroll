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
 * @date      26 Sep 2025
 * @copyright Copyright (C) 2017-2025 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_PALLOC_H
#define _STROLL_PALLOC_H

#include <stroll/cdefs.h>
#include <stroll/priv/alloc_chunk.h>
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
 * Basically, #stroll_palloc is a primitive and lightweight allocator that
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
 * - all memory objects may be allocated thanks to a single call to
 *   @man{malloc(3)} ;
 * - and known at the time of allocator instantiation.
 *
 * @note
 * All managed memory being allocated in a single call to @man{malloc(3)},
 * #stroll_palloc is highly subject to number of objects, object size and / or
 * process memory restrictions.
 * See @man{malloc(3)}, @man{mallopt(3)} and @man{setrlimit(2)} for further
 * details.
 * When stroll_palloc_init() fails to perform allocation because of these
 * restrictions, #stroll_lalloc may provide a suitable alternative.
 *
 * @see
 * - stroll_palloc_init()
 * - stroll_palloc_init_from_mem()
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
	union stroll_alloc_chunk * next_free;
	/**
	 * @internal
	 *
	 * Memory area holding chunks.
	 */
	void *                     chunks;
	/**
	 * @internal
	 *
	 * Indicate wether this allocator owns the memory area pointed to by
	 * #stroll_palloc::chunks.
	 */
	bool                       own;
};

#define stroll_palloc_assert_alloc_api(_alloc) \
	stroll_palloc_assert_api(_alloc); \
	stroll_palloc_assert_api((_alloc)->chunks)

/**
 * Release the allocated chunk of memory given in argument.
 *
 * @param[inout] alloc Pre-allocated fixed sized object allocator
 * @param[inout] chunk Chunk of memory to free
 *
 * Free resources allocated by @p alloc allocator for the chunk of memory @p
 * chunk.
 *
 * @p chunk *MUST* point to a chunk of memory returned by a call to
 * stroll_palloc_alloc() using the same @p alloc allocator.
 *
 * @see
 * - stroll_palloc_alloc()
 * - #stroll_palloc
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_palloc_free(struct stroll_palloc * __restrict alloc, void * chunk)
{
	stroll_palloc_assert_alloc_api(alloc);
	stroll_palloc_assert_api(chunk >= alloc->chunks);

	if (chunk) {
		union stroll_alloc_chunk * chnk = chunk;

		chnk->next_free = alloc->next_free;
		alloc->next_free = chnk;
	}
}

/**
 * Allocate a chunk of memory.
 *
 * @param[inout] alloc Pre-allocated fixed sized object allocator
 *
 * @return A pointer to the allocated chunk of memory
 *
 * Request the @p alloc allocator to allocate and return a chunk of memory.
 * @p alloc *MUST* have been previously initialized using stroll_palloc_init().
 *
 * The chunk returned is, at least, as large as the @p chunk_size argument given
 * to stroll_palloc_init() at initialization time.
 * Its address and size are guaranteed to be aligned upon a machine word.
 *
 * @see
 * - stroll_palloc_free()
 * - #stroll_palloc
 */
static inline __stroll_nonull(1)
              __assume_align(sizeof(union stroll_alloc_chunk *))
              __stroll_nothrow
              __warn_result
void *
stroll_palloc_alloc(struct stroll_palloc * __restrict alloc)
{
	stroll_palloc_assert_alloc_api(alloc);

	union stroll_alloc_chunk * chnk = alloc->next_free;

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

/**
 * Initialize a pre-allocated fixed sized object allocator with external memory
 * area.
 *
 * @param[out] alloc      Pre-allocated fixed sized object allocator
 * @param[in]  mem        Pointer to memory area allocated by caller
 * @param[in]  chunk_nr   Number of chunks per primary memory block
 * @param[in]  chunk_size Size of a single chunk of memory in bytes.
 *
 * Initialize a pre-allocated fixed sized object allocator so that it may
 * further allocate @p chunk_size bytes long memory chunks thanks to the
 * stroll_palloc_alloc() function.
 *
 * Unlinke the stroll_palloc_init() function, stroll_palloc_init_from_mem() do
 * not allocate (neither own) its internal memory area used to store chunks.
 * The caller is responsible for allocating a suitable memory area and passing
 * it as the @p mem argument instead. See below for requirements related to
 * the memory area size.
 *
 * @p chunk_nr specifies the minimum number of chunks that are allocated on heap
 * in a single call to @man{malloc(3)}.
 *
 * @p chunk_size specify the size of a single *chunk* of memory in bytes and
 * will be rounded up to the size of a machine word.
 *
 * @warning
 * The size of the memory area pointed to by @p mem **MUST** be at least as
 * large as @p chunk_nr rounded up to the size of a machine word multiplied by
 * @p chunk_size.
 *
 * @see
 * - stroll_palloc_init()
 * - stroll_palloc_fini()
 * - #stroll_palloc
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_palloc_init_from_mem(struct stroll_palloc * __restrict alloc,
                            void * __restrict                 mem,
                            unsigned int                      chunk_nr,
                            size_t                            chunk_size)
{
	_stroll_palloc_init_from_mem(alloc, mem, chunk_nr, chunk_size, false);
}

/**
 * Initialize a pre-allocated fixed sized object allocator.
 *
 * @param[out] alloc      Pre-allocated fixed sized object allocator
 * @param[in]  chunk_nr   Number of chunks per primary memory block
 * @param[in]  chunk_size Size of a single chunk of memory in bytes.
 *
 * Initialize a pre-allocated fixed sized object allocator so that it may
 * further allocate @p chunk_size bytes long memory chunks thanks to the
 * stroll_palloc_alloc() function.
 *
 * @p chunk_nr specifies the minimum number of chunks that are allocated on heap
 * in a single call to @man{malloc(3)}.
 *
 * @p chunk_size specify the size of a single *chunk* of memory in bytes and
 * will be rounded up to the size of a machine word.
 *
 * @see
 * - stroll_palloc_init_from_mem()
 * - stroll_palloc_fini()
 * - #stroll_palloc
 */
extern int
stroll_palloc_init(struct stroll_palloc * __restrict alloc,
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
 * - stroll_palloc_init()
 * - stroll_palloc_init_from_mem()
 * - #stroll_palloc
 */
static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_palloc_fini(struct stroll_palloc * __restrict alloc)
{
	stroll_palloc_assert_alloc_api(alloc);

	if (alloc->own)
		free(alloc->chunks);
}

#if defined(CONFIG_STROLL_ALLOC)

#include <stroll/alloc.h>

extern struct stroll_alloc *
stroll_palloc_create_alloc(unsigned int chunk_nr, size_t chunk_size)
	__malloc(stroll_alloc_destroy, 1) __stroll_nothrow __warn_result;

extern struct stroll_alloc *
stroll_palloc_create_alloc_from_mem(void * __restrict mem,
                                    unsigned int      chunk_nr,
                                    size_t            chunk_size)
	__stroll_nonull(1)
	__malloc(stroll_alloc_destroy, 1)
	__stroll_nothrow
	__warn_result;

#endif /* defined(CONFIG_STROLL_ALLOC) */

#endif /* _STROLL_PALLOC_H */
