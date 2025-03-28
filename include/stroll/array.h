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
 * Last argument may point to an optional arbitrary user data used for
 * comparison purposes.
 *
 * The function *MUST* return an integer less than, equal to, or greater than
 * zero if first argument is found, respectively, to be less than, to match, or
 * be greater than the second one.
 *
 * @see stroll_bisect_search()
 */
typedef int stroll_array_cmp_fn(const void * __restrict,
                                const void * __restrict,
                                void *)
	__stroll_nonull(1, 2) __warn_result;

#if defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH)

/**
 * Search by key within a pre-sorted array.
 *
 * @param[in]    key     Key to search for
 * @param[in]    array   Array to seach into
 * @param[in]    nr      @p array number of elements
 * @param[in]    size    Size of a single @p array element
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
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
 * The @p compare routine is expected to handle 3 arguments:
 * - *first* one points to the @p key argument given to stroll_bisect_search() ;
 * - *second* one points to an @p array element ;
 * - an optional *third* argument @p data that may point to arbitrary data for
 *   comparison purposes.
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
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare,
                           void *                data)
	__stroll_nonull(2, 5) __warn_result;

#endif /* defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH) */

#if defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT)

/**
 * Sort an array according to the bubble sort algorithm.
 *
 * @param[inout] array   Array to sort
 * @param[in]    nr      @p array number of elements
 * @param[in]    size    Size of a single @p array element
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
 *
 * Sort @p array containing @p nr elements of size @p size using the @p compare
 * comparison function according to the @rstlnk{Array bubble sort} algorithm.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * @note
 * Refer to @rstlnk{Sorting arrays} for more informations related to algorithm
 * selection.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   `nr <= 1`, result is undefined. An assertion otherwise.
 * - Implemented for reference only: **DO NOT USE IT**. Refer to
 *   @rstlnk{Sorting arrays} for more informations related to algorithm
 *   selection.
 */
extern void
stroll_array_bubble_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)

/**
 * Sort an array according to the selection sort algorithm.
 *
 * @param[inout] array   Array to sort
 * @param[in]    nr      @p array number of elements
 * @param[in]    size    Size of a single @p array element
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
 *
 * Sort @p array containing @p nr elements of size @p size using the @p compare
 * comparison function according to the @rstlnk{Array selection sort} algorithm.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * @note
 * Refer to @rstlnk{Sorting arrays} for more informations related to algorithm
 * selection.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   `nr <= 1`, result is undefined. An assertion otherwise.
 * - Implemented for reference only: **DO NOT USE IT**. Refer to
 *   @rstlnk{Sorting arrays} for more informations related to algorithm selection.
 */
extern void
stroll_array_select_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)

/**
 * Sort an array according to the insertion sort algorithm.
 *
 * @param[inout] array   Array to sort
 * @param[in]    nr      @p array number of elements
 * @param[in]    size    Size of a single @p array element
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
 *
 * Sort @p array containing @p nr elements of size @p size using the @p compare
 * comparison function according to the @rstlnk{Array insertion sort} algorithm.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * @note
 * Refer to @rstlnk{Sorting arrays} for more informations related to algorithm
 * selection.
 *
 * @see stroll_array_insert_inpsort_elem()
 */
extern void
stroll_array_insert_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
	__stroll_nonull(1, 4);

/**
 * Append an element into a pre-sorted array according to the insertion sort
 * algorithm.
 *
 * @param[inout] array   Array to sort
 * @param[inout] elem    Last element of @p array
 * @param[in]    size    Size of a single @p array element
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
 *
 * Sort @p array, given:
 * - @p array, an existing *pre-sorted* array ;
 * - and @p elem, an array element located right at the end of @p array in
 *   unsorted order.
 *
 * @p array should contain elements of size @p size, *pre-sorted* with respect
 * to the @p compare comparison function. Sorting is performed according to the
 * @rstlnk{Array insertion sort} algorithm.
 *
 * @p elem *MUST* point to the first unsorted and last element of @p array.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * Use stroll_array_insert_inpsort_elem() when you know for sure that all
 * elements of @p array up to @p elem (exclusive) are in sorted order.
 * This allows to optimize situations where sorting a *continuous stream of
 * input elements* in-place is required.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p array is empty, i.e., `(char *)elem < ((char *)array + size)`, result is
 *   undefined. An assertion otherwise.
 *
 * @see stroll_array_insert_oopsort_elem()
 * @see stroll_array_insert_sort()
 */
extern void
stroll_array_insert_inpsort_elem(void * __restrict     array,
                                 void * __restrict     elem,
                                 size_t                size,
                                 stroll_array_cmp_fn * compare,
                                 void *                data)
	__stroll_nonull(1, 2, 4);

/**
 * Append an element into a pre-sorted array according to the insertion sort
 * algorithm.
 *
 * @param[inout] array   Array to sort
 * @param[inout] end     Pointer to first unused location of @p array
 * @param[inout] elem    Element to insert into @p array
 * @param[in]    size    Size of a single @p array element
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
 *
 * Sort @p array, given:
 * - @p array, an existing *pre-sorted* array ;
 * - and @p elem, an array element located out of @p array.
 *
 * @p array should contain elements of size @p size, *pre-sorted* with respect
 * to the @p compare comparison function. Sorting is performed according to the
 * @rstlnk{Array insertion sort} algorithm.
 *
 * @p end *MUST* point to the end of @p array, i.e. to the first unused element
 * location of @p array.
 * Once stroll_array_insert_oopsort_elem() completes, @p array contains one
 * more element and @p end points to an existing one.
 *
 * @p elem *MUST* point to an element to insert into @p array and located out of
 * the memory region holding @p array, i.e., with an address `< @p array` and
 * `>= @p end + @p size`.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * Use stroll_array_insert_oopsort_elem() when you know for sure that all
 * elements of @p array up to @p end (exclusive) are in sorted order.
 * This allows to optimize situations where sorting a *continuous stream of
 * input elements* out-of-place is required.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p array is empty, i.e., `(char *)end < ((char *)array + size)`, result is
 *   undefined. An assertion otherwise.
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p elem belongs to @p array, i.e.,
 *   `(elem >= array) && ((char *)elem < ((char *)end + size))`, result is
 *   undefined. An assertion otherwise.
 *
 * @see stroll_array_insert_inpsort_elem()
 * @see stroll_array_insert_sort()
 */
extern void
stroll_array_insert_oopsort_elem(void * __restrict       array,
                                 void * __restrict       end,
                                 const void * __restrict elem,
                                 size_t                  size,
                                 stroll_array_cmp_fn *   compare,
                                 void *                  data)
	__stroll_nonull(1, 2, 3, 5);

#endif /* defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)

/**
 * Sort an array according to the quick sort algorithm.
 *
 * @param[inout] array   Array to sort
 * @param[in]    size    Size of a single @p array element
 * @param[in]    nr      @p array number of elements
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
 *
 * Sort @p array containing @p nr elements of size @p size using the @p compare
 * comparison function according to the @rstlnk{Array quick sort} algorithm.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * @note
 * Refer to @rstlnk{Sorting arrays} for more informations related to algorithm
 * selection.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * `nr <= 1`, result is undefined. An assertion otherwise.
 */
extern void
stroll_array_quick_sort(void * __restrict     array,
                        unsigned int          nr,
                        size_t                size,
                        stroll_array_cmp_fn * compare,
                        void *                data)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */

#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT)

/**
 * Sort an array according to the 3-way partition quick sort algorithm.
 *
 * @param[inout] array   Array to sort
 * @param[in]    size    Size of a single @p array element
 * @param[in]    nr      @p array number of elements
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
 *
 * Sort @p array containing @p nr elements of size @p size using the @p compare
 * comparison function according to the @rstlnk{Array 3-way quick sort}
 * algorithm.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * @note
 * Refer to @rstlnk{Sorting arrays} for more informations related to algorithm
 * selection.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * `nr <= 1`, result is undefined. An assertion otherwise.
 */
extern void
stroll_array_3wquick_sort(void * __restrict     array,
                          unsigned int          nr,
                          size_t                size,
                          stroll_array_cmp_fn * compare,
                          void *                data)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT) */

#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT)

/**
 * Sort an array according to the merge sort algorithm.
 *
 * @param[inout] array   Array to sort
 * @param[in]    size    Size of a single @p array element
 * @param[in]    nr      @p array number of elements
 * @param[in]    compare @p array elements comparison function
 * @param[inout] data    Optional arbitrary user data
 *
 * @return `0` when successful, a negative errno-like return code otherwise.
 * @retval 0       Success
 * @retval -ENOMEM Memory allocation failure
 *
 * Sort @p array containing @p nr elements of size @p size using the @p compare
 * comparison function according to the @rstlnk{Array merge sort} algorithm.
 *
 * The first 2 arguments passed to the @p compare routine both points to
 * distinct @p array elements.
 * @p compare *MUST* return an integer less than, equal to, or greater than zero
 * if first argument is found, respectively, to be less than, to match, or be
 * greater than the second one.
 *
 * The @p compare routine is given @p data as an optional *third* argument
 * as-is. It may point to arbitrary user data for comparison purposes.
 *
 * @note
 * Refer to @rstlnk{Sorting arrays} for more informations related to algorithm
 * selection.
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * `nr <= 1`, result is undefined. An assertion otherwise.
 */
extern int
stroll_array_merge_sort(void * __restrict     array,
                        unsigned int          nr,
                        size_t                size,
                        stroll_array_cmp_fn * compare,
                        void *                data)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_MERGE_SORT) */

#if defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT)

extern void
stroll_array_fbheap_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
	__stroll_nonull(1, 4);

#endif /* defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT) */

#if defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT)

extern int
stroll_array_fwheap_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
	__stroll_nonull(1, 4) __warn_result;

#endif /* defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT) */

#endif /* _STROLL_ARRAY_H */
