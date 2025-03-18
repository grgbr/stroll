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
 * An opaque structure allowing to allocate objects of constant size.
 *
 * This may be useful to:
 * - speed up allocations ;
 * - mitigate conventional memory heap fragmentation ;
 * - perform allocation in a more deterministic way.
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
	 * Size of a single block of memory chunks.
	 */
	size_t                   block_sz;
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
};


void *
stroll_falloc_alloc(struct stroll_falloc * __restrict alloc);

void
stroll_falloc_free(struct stroll_falloc * __restrict alloc, void * chunk);

void
stroll_falloc_init(struct stroll_falloc * __restrict alloc,
                   unsigned int                      chunk_nr,
                   size_t                            chunk_size);

void
stroll_falloc_fini(struct stroll_falloc * __restrict alloc);

#endif /* _STROLL_FALLOC_H */
