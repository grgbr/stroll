/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @internal
 * 
 * @file
 * Object allocators internals
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      23 Oct 2025
 * @copyright Copyright (C) 2017-2025 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_PRIV_ALLOC_H
#define _STROLL_PRIV_ALLOC_H

/**
 * @internal
 *
 * Object allocator memory chunk.
 */
union stroll_alloc_chunk {
	/**
	 * @internal
	 *
	 * Link to next chunk within the free chunk list.
	 */
	union stroll_alloc_chunk * next_free;
	/**
	 * @internal
	 *
	 * Chunk data area.
	 */
	char                       data[0];
};

#endif /* _STROLL_PRIV_ALLOC_H */
