/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Weak heap interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      02 Jun 2024
 * @copyright Copyright (C) 2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_FWHEAP_H
#define _STROLL_FWHEAP_H

#include <stroll/array.h>
#include <stroll/fbmap.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_fwheap_assert_api(_expr) \
	stroll_assert("stroll:fwheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_fwheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#define _STROLL_FWHEAP_RBITS_SIZE(_nr) \
	(STROLL_FBMAP_WORD_NR(_nr) * sizeof(unsigned long))

static inline __stroll_const __stroll_nothrow
unsigned int
_stroll_fwheap_rbits_size(unsigned int nr)
{
	stroll_fwheap_assert_api(nr);

	return stroll_fbmap_word_nr(nr) * sizeof(unsigned long);
}

static inline __stroll_nonull(1) __stroll_const __stroll_nothrow
const void *
_stroll_fwheap_find(const void * __restrict array)
{
	stroll_fwheap_assert_api(array);

	return array;
}

extern void
_stroll_fwheap_insert(const void * __restrict    elem,
                      void * __restrict          array,
                      unsigned long * __restrict rbits,
                      unsigned int               nr,
                      size_t                     size,
                      stroll_array_cmp_fn *      compare,
                      void *                     data)
	__stroll_nonull(1, 2, 3, 6);

extern void
_stroll_fwheap_extract(void * __restrict          elem,
                       void * __restrict          array,
                       unsigned long * __restrict rbits,
                       unsigned int               nr,
                       size_t                     size,
                       stroll_array_cmp_fn *      compare,
                       void *                     data)
	__stroll_nonull(1, 2, 3, 6);

extern void
_stroll_fwheap_build(void * __restrict          array,
                     unsigned long * __restrict rbits,
                     unsigned int               nr,
                     size_t                     size,
                     stroll_array_cmp_fn *      compare,
                     void *                     data)
	__stroll_nonull(1, 2, 5);

extern unsigned long *
_stroll_fwheap_alloc_rbits(unsigned int nr)
	__stroll_nothrow __leaf __warn_result;

extern void
_stroll_fwheap_free_rbits(unsigned long * rbits)
	__stroll_nothrow __leaf;

#endif /* _STROLL_FWHEAP_H */
