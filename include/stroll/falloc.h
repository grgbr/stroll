/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Fixed sized object allocator interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      22 Dec 2017
 * @copyright Copyright (C) 2017-2025 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_FALLOC_H
#define _STROLL_FALLOC_H

#include <stroll/dlist.h>

/**
 * Fixed sized object allocator.
 *
 * An opaque structure allowing to allocate objects of constant size and
 * suitable for deriving custom allocators.
 *
 * This may be useful to:
 * - speed up allocations ;
 * - mitigate conventional memory heap fragmentation ;
 * - perform allocation in a more deterministic way.
 *
 * Basically, the allocator performs memory allocation on heap thanks to
 * @man{malloc(3)} calls. Objects, called *chunks*, are allocated by *blocks* to
 * minimize the number of calls to @man{malloc(3)} and @man{free(3)}.
 *
 * The total number of allocated objects may dynamically grow or decrease during
 * allocator lifetime, making it suitable for cases where the number of
 * allocated object is not known in advance. If this is not desirable, refer to
 * #stroll_palloc allocator.
 *
 * For performance reasons, *blocks* of memory *chunks* are aligned on power of
 * 2 boundaries. The size and the minimum number of memory *chunks* per block
 * are specified at allocator initialization time thanks to a call to
 * stroll_falloc_init().
 *
 * @see
 * - stroll_falloc_init()
 * - stroll_falloc_fini()
 * - stroll_falloc_alloc()
 * - stroll_falloc_free()
 */
struct stroll_falloc {
	/**
	 * @internal
	 *
	 * List of blocks of memory chunks.
	 */
	struct stroll_dlist_node blocks;
	/**
	 * @internal
	 *
	 * Alignment of a single block of memory chunks.
	 */
	unsigned long            block_al;
	/**
	 * @internal
	 *
	 * Number of memory chunks per block.
	 */
	unsigned int             chunk_nr;
	/**
	 * @internal
	 *
	 * Size of a single memory chunk.
	 */
	size_t                   chunk_sz;
	/**
	 * @internal
	 *
	 * Size of a single block of memory chunks.
	 */
	size_t                   block_sz;
};

/**
 * Release the chunk of memory given in argument.
 *
 * @param[inout] alloc Fixed sized object allocator
 * @param[inout] chunk Chunk of memory to free
 *
 * Free resources allocated by @p alloc allocator for the chunk of memory @p
 * chunk.
 *
 * @p chunk *MUST* point to a chunk of memory returned by a call to
 * stroll_falloc_alloc() using the same @p alloc allocator.
 *
 * @see
 * - stroll_falloc_alloc()
 * - #stroll_falloc
 */
extern void
stroll_falloc_free(struct stroll_falloc * __restrict alloc,
                   void * __restrict                 chunk)
	__stroll_nonull(1) __stroll_nothrow __leaf;

union stroll_falloc_chunk;

/**
 * Allocate a chunk of memory.
 *
 * @param[inout] alloc Fixed sized object allocator
 *
 * @return A pointer to the allocated chunk of memory
 *
 * Request the @p alloc allocator to allocate and return a chunk of memory.
 * @p alloc *MUST* have been previously initialized using stroll_falloc_init().
 *
 * The chunk returned is, at least, as large as the @p chunk_size argument given
 * to stroll_falloc_init() at initialization time.
 * Its address and size are guaranteed to be aligned upon a machine word.
 *
 * @see
 * - stroll_falloc_free()
 * - #stroll_falloc
 */
extern void *
stroll_falloc_alloc(struct stroll_falloc * __restrict alloc)
	__stroll_nonull(1)
	__malloc(stroll_falloc_free, 2)
	__assume_align(sizeof(union stroll_falloc_chunk *))
	__stroll_nothrow
	__leaf
	__warn_result;

/**
 * Initialize a fixed sized object allocator.
 *
 * @param[out] alloc      Fixed sized object allocator
 * @param[in]  chunk_nr   Number of chunks per primary memory block
 * @param[in]  chunk_size Size of a single chunk of memory in bytes.
 *
 * Initialize a fixed sized object allocator so that it may further allocate
 * @p chunk_size bytes long memory chunks thanks to the stroll_falloc_alloc()
 * function.
 *
 * @p chunk_nr specifies the minimum number of chunks that are allocated on heap
 * in a single call to @man{malloc(3)}. The @p chunk_nr is a hint only
 * since memory chunks are allocated by block, which address is a power of 2 and
 * size, a multiple of a machine word.
 *
 * @p chunk_size specify the size of a single *chunk* of memory in bytes and
 * will be rounded up to the size of a machine word.
 *
 * @see
 * - stroll_falloc_fini()
 * - #stroll_falloc
 */
extern void
stroll_falloc_init(struct stroll_falloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   size_t                            chunk_size)
	__stroll_nonull(1) __stroll_nothrow __leaf;

/**
 * Release all resources allocated by a fixed sized object allocator.
 *
 * @param[out] alloc Fixed sized object allocator
 *
 * Release all *blocks* of memory *chunks* allocated by the @p alloc allocator
 * given in argument.
 *
 * @see
 * - stroll_falloc_init()
 * - #stroll_falloc
 */
extern void
stroll_falloc_fini(struct stroll_falloc * __restrict alloc)
	__stroll_nonull(1) __stroll_nothrow __leaf;

#endif /* _STROLL_FALLOC_H */
