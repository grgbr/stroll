/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Heap data structures interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      02 Jun 2024
 * @copyright Copyright (C) 2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_HEAP_H
#define _STROLL_HEAP_H

#include <stroll/array.h>

#if defined(CONFIG_STROLL_FHEAP)

extern void
stroll_fheap_insert(const void * __restrict elem,
                    void * __restrict       array,
                    unsigned int            nr,
                    size_t                  size,
                    stroll_array_cmp_fn *   compare,
                    void *                  data)
	__stroll_nonull(1, 2, 5);

extern void
stroll_fheap_extract(void * __restrict     elem,
                     void * __restrict     array,
                     unsigned int          nr,
                     size_t                size,
                     stroll_array_cmp_fn * compare,
                     void *                data)
	__stroll_nonull(1, 2, 5);

extern void
stroll_fheap_build(void * __restrict     array,
                   unsigned int          nr,
                   size_t                size,
                   stroll_array_cmp_fn * compare,
                   void *                data)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_FHEAP) */

#endif /* _STROLL_HEAP_H */
