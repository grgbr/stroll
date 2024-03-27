/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Bisection / dichotomic search interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      26 Aug 2024
 * @copyright Copyright (C) 2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_BISECT_H
#define _STROLL_BISECT_H

#include <stroll/cdefs.h>
#include <sys/types.h>

/**
 * Search by key within a pre-sorted array.
 *
 * @param[in] key     Key to search for
 * @param[in] array   Array to seach into
 * @param[in] size    Size of a single @p array element
 * @param[in] nr      @p array number of elements
 * @param[in] compare @p array elements comparison function
 *
 * @return Matching array element when found, `NULL` otherwise.
 *
 * Search @p array of @p nr elements for an element identified by @p key key.
 * The size of each @p array element is specified by @p size.
 *
 * The returned @p array element is **unspecified** if multiple elements match
 * the @p key key given in argument.
 *
 * The contents of @p array *MUST BE* in ascending sorted order with respect to
 * the @p compare comparison function.
 *
 * The @p compare routine is expected to handle 2 arguments:
 * - *first* one points to the @p key argument given to stroll_bisect_search() ;
 * - *second* one points to an @p array element.
 * It *MUST* return an integer less than, equal to, or greater than zero if
 * @p key is found, respectively, to be less than, to match, or be greater than
 * the @p array element.
 *
 * @see stroll_cmp_fn
 */
extern void *
stroll_bisect_search(const void *    key,
                     const void *    array,
                     size_t          size,
                     unsigned int    nr,
                     stroll_cmp_fn * compare)
	__stroll_nonull(2, 5) __warn_result;

#endif /* _STROLL_BISECT_H */
