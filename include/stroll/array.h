/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Array algorithms interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      26 Aug 2024
 * @copyright Copyright (C) 2024 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_ARRAY_H
#define _STROLL_ARRAY_H

#include <stroll/cdefs.h>
#include <sys/types.h>

/**
 * Comparison routine signature.
 *
 * Use this to implement a function that compares 2 data blocks given as
 * arguments according to an arbitrary logic.
 *
 * The function *MUST* return an integer less than, equal to, or greater than
 * zero if first argument is found, respectively, to be less than, to match, or
 * be greater than the second one.
 *
 * @see stroll_bisect_search()
 */
typedef int stroll_array_cmp_fn(const void * __restrict,
                                const void * __restrict)
	__stroll_nonull(1, 2) __warn_result;

#if defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH)

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
 * Search @p array of @p nr elements for an element identified by @p key
 * according to a bisection / dichotomic logic.
 * The size of each @p array element is specified by @p size.
 *
 * The returned @p array element is **unspecified** if multiple elements match
 * the @p key given in argument.
 *
 * The contents of @p array *MUST BE* in ascending sorted order with respect to
 * the @p compare comparison function.
 *
 * The @p compare routine is expected to handle 2 arguments:
 * - *first* one points to the @p key argument given to stroll_bisect_search() ;
 * - *second* one points to an @p array element.
 *
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if @p key is found, respectively, to be less than, to match, or be greater
 * than the @p array element.
 *
 * @see stroll_array_cmp_fn
 */
extern void *
stroll_array_bisect_search(const void *          key,
                           const void *          array,
                           size_t                size,
                           unsigned int          nr,
                           stroll_array_cmp_fn * compare)
	__stroll_nonull(2, 5) __warn_result;

#endif /* defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH) */

#if defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT)

/**
 * Sort an array according to the bubble sort algorithm.
 *
 * @param[in] array   Array to sort
 * @param[in] size    Size of a single @p array element
 * @param[in] nr      @p array number of elements
 * @param[in] compare @p array elements comparison function
 *
 * Sort @p array containing @p nr elements of size @p size using the @p compare
 * comparison function according to the @rstlnk{Bubble sort} algorithm.
 *
 * The @p compare routine is expected to handle 2 arguments, both pointing to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * @rsttable{Sorting algorithm properties}
 * +---------------------------------+---------------------------------------+
 * | |adaptative|                    | yes                                   |
 * +---------------------------------+---------------------------------------+
 * | |online|                        | no                                    |
 * +---------------------------------+---------------------------------------+
 * | |stable|                        | yes                                   |
 * +---------------------------------+---------------------------------------+
 * | |recursive|                     | no                                    |
 * +---------------------------------+---------------------------------------+
 * | allocation                      | none                                  |
 * +---------------------------------+---------------------------------------+
 * | |in-place|                      | yes                                   |
 * +--------------+-------+----------+---------------------------------------+
 * |              |       | worst    | :math:`O(1)`                          |
 * |              |       +----------+---------------------------------------+
 * |              | space | average  | :math:`O(1)`                          |
 * |              |       +----------+---------------------------------------+
 * |              |       | best     | :math:`O(1)`                          |
 * | |complexity| +-------+----------+---------------------------------------+
 * |              |       | worst    | :math:`O(n^2))`                       |
 * |              |       +----------+---------------------------------------+
 * |              | time  | average  | :math:`O(n^2))`                       |
 * |              |       +----------+---------------------------------------+
 * |              |       | best     | :math:`O(n)`                          |
 * +--------------+-------+----------+---------------------------------------+
 * @endrsttable
 *
 * @note
 * - high number of items swaps ;
 * - decent efficiency with presorted data sets ;
 * - poor to extremely low efficiency even over small data sets.
 *
 * @warning
 * Implemented for reference only: **DO NOT USE IT**. Refer to
 * @rstlnk{Sorting arrays} for more informations related to algorithm selection.
 */
extern void
stroll_array_bubble_sort(void * __restrict     array,
                         size_t                size,
                         unsigned int          nr,
                         stroll_array_cmp_fn * compare)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)

/**
 * Sort an array according to the selection sort algorithm.
 *
 * @param[in] array   Array to sort
 * @param[in] size    Size of a single @p array element
 * @param[in] nr      @p array number of elements
 * @param[in] compare @p array elements comparison function
 *
 * FILL ME!!
 *
 * @see stroll_array_cmp_fn
 */
extern void
stroll_array_select_sort(void * __restrict     array,
                         size_t                size,
                         unsigned int          nr,
                         stroll_array_cmp_fn * compare)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)

/**
 * Sort an array according to the insertion sort algorithm.
 *
 * @param[in] array   Array to sort
 * @param[in] size    Size of a single @p array element
 * @param[in] nr      @p array number of elements
 * @param[in] compare @p array elements comparison function
 *
 * FILL ME!!
 *
 * @see stroll_array_cmp_fn
 */
extern void
stroll_array_insert_sort(void * __restrict     array,
                         size_t                size,
                         unsigned int          nr,
                         stroll_array_cmp_fn * compare)
	__stroll_nonull(1, 4);

/**
 * Describe me!!
 *
 * @param[in] array   Array to sort
 * @param[in] unsort  last unsorted element of @p array
 * @param[in] size    Size of a single @p array element
 * @param[in] nr      @p array number of elements
 * @param[in] compare @p array elements comparison function
 *
 * FILL ME!!
 *
 * @see stroll_array_cmp_fn
 */
extern void
stroll_array_insert_presort(void * __restrict     array,
                            void * __restrict     unsort,
                            size_t                size,
                            stroll_array_cmp_fn * compare)
	__stroll_nonull(1, 2, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)

/**
 * Describe me!!
 *
 * @param[in] array   Array to sort
 * @param[in] unsort  last unsorted element of @p array
 * @param[in] size    Size of a single @p array element
 * @param[in] nr      @p array number of elements
 * @param[in] compare @p array elements comparison function
 *
 * FILL ME!!
 *
 * @see stroll_array_cmp_fn
 */
extern void
stroll_array_quick_sort(void * __restrict     array,
                        size_t                size,
                        unsigned int          nr,
                        stroll_array_cmp_fn * compare)
	__stroll_nonull(1, 2, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */

#endif /* _STROLL_ARRAY_H */
