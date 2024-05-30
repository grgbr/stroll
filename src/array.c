/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/array.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_array_assert_api(_expr) \
	stroll_assert("stroll:array", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_array_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_array_assert_intern(_expr) \
	stroll_assert("stroll:array", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_array_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

static inline bool
stroll_array_aligned(const void * __restrict array, size_t size, size_t align)
{
	return (size == align) &&
	       stroll_aligned((unsigned long)array, align);
}

#define STROLL_ARRAY_DEFINE_SWAP(_func, _type) \
	static __stroll_nonull(1, 2) \
	void \
	_func(_type * __restrict first, _type * __restrict second) \
	{ \
		stroll_array_assert_intern(first); \
		stroll_array_assert_intern(second); \
		\
		_type tmp; \
		\
		tmp = *first; \
		*first = *second; \
		*second = tmp; \
	}

STROLL_ARRAY_DEFINE_SWAP(stroll_array_swap32, uint32_t)

STROLL_ARRAY_DEFINE_SWAP(stroll_array_swap64, uint64_t)

static __stroll_nonull(1, 2)
void
stroll_array_swap(void * __restrict first,
                  void * __restrict second,
                  size_t            size)
{
	stroll_array_assert_intern(first);
	stroll_array_assert_intern(second);
	stroll_array_assert_intern(size);

	char tmp[size];

	memcpy(tmp, first, size);
	memcpy(first, second, size);
	memcpy(second, tmp, size);
}

#if defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH)

void *
stroll_array_bisect_search(const void *          key,
                           const void *          array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare,
                           void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	size_t min = 0;
	size_t max = nr;

	do {
		size_t       mid = (min + max) / 2;
		const char * elm = (const char *)array + (mid * size);
		int          cmp;

		cmp = compare(key, elm, data);
		if (cmp < 0)
			max = mid;
		else if (cmp > 0)
			min = mid + 1;
		else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
			return (void *)elm;
#pragma GCC diagnostic pop
		}
	} while (min < max);

	return NULL;
}

#endif /* defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH) */

#if defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT)

#define STROLL_ARRAY_DEFINE_BUBBLE(_sort_func, _swap_func, _type) \
	static __stroll_nonull(1, 3) \
	void \
	_sort_func(_type * __restrict    array, \
	           unsigned int          nr, \
	           stroll_array_cmp_fn * compare, \
	           void *                data) \
	{ \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(nr > 1); \
		stroll_array_assert_intern(compare); \
		\
		const _type * end = &array[nr - 1]; \
		\
		while (end > array) { \
			_type *       elm = array; \
			const _type * last = array; \
			\
			do { \
				_type * neigh = &elm[1]; \
				\
				if (compare(elm, neigh, data) > 0) { \
					_swap_func(elm, neigh); \
					last = neigh; \
				} \
				\
				elm = neigh; \
			} while (elm < end); \
			\
			end = last; \
		} \
	}

STROLL_ARRAY_DEFINE_BUBBLE(stroll_array_bubble_sort32,
                           stroll_array_swap32,
                           uint32_t)

STROLL_ARRAY_DEFINE_BUBBLE(stroll_array_bubble_sort64,
                           stroll_array_swap64,
                           uint64_t)

static __stroll_nonull(1, 4)
void
stroll_array_bubble_sort_mem(void * __restrict     array,
                             unsigned int          nr,
                             size_t                size,
                             stroll_array_cmp_fn * compare,
                             void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(nr > 1);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	const char * end = (const char *)array + ((nr - 1) * size);

	while (end > (const char *)array) {
		char *       elm = array;
		const char * last = array;

		do {
			char * neigh = &elm[size];

			if (compare(elm, neigh, data) > 0) {
				stroll_array_swap(elm, neigh, size);
				last = neigh;
			}

			elm = neigh;
		} while (elm < end);

		end = last;
	}
}

void
stroll_array_bubble_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (nr == 1)
		return;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_array_bubble_sort32(array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_bubble_sort64(array, nr, compare, data);
	else
		stroll_array_bubble_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)

#define STROLL_ARRAY_DEFINE_SELECT(_sort_func, _swap_func, _type) \
	static __stroll_nonull(1, 3) \
	void \
	_sort_func(_type * __restrict    array, \
	           unsigned int          nr, \
	           stroll_array_cmp_fn * compare, \
	           void *                data) \
	{ \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(nr > 1); \
		stroll_array_assert_intern(compare); \
		\
		_type *       unsort = array; \
		const _type * end = &array[nr - 1]; \
		\
		while (unsort < end) { \
			_type * elm = unsort; \
			_type * min = unsort; \
			\
			do { \
				elm++; \
				if (compare(elm, min, data) < 0) \
					min = elm; \
			} while (elm < end); \
			\
			_swap_func(unsort, min); \
			\
			unsort++; \
		} \
	}

STROLL_ARRAY_DEFINE_SELECT(stroll_array_select_sort32,
                           stroll_array_swap32,
                           uint32_t)

STROLL_ARRAY_DEFINE_SELECT(stroll_array_select_sort64,
                           stroll_array_swap64,
                           uint64_t)

static __stroll_nonull(1, 4)
void
stroll_array_select_sort_mem(void * __restrict     array,
                             unsigned int          nr,
                             size_t                size,
                             stroll_array_cmp_fn * compare,
                             void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(nr > 1);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	const char * end = (const char *)array + ((nr - 1) * size);
	char *       unsort = array;

	while (unsort < end) {
		char * elm = unsort;
		char * min = unsort;

		do {
			elm += size;
			if (compare(elm, min, data) < 0)
				min = elm;
		} while (elm < end);

		stroll_array_swap(unsort, min, size);

		unsort += size;
	}
}

void
stroll_array_select_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (nr == 1)
		return;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_array_select_sort32(array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_select_sort64(array, nr, compare, data);
	else
		stroll_array_select_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)

#define STROLL_ARRAY_DEFINE_INSERT_INPSORT_ELEM(_inpsort_elem_func, \
                                                _swap_func, \
                                                _type) \
	static __stroll_nonull(1, 2, 3) \
	void \
	_inpsort_elem_func(_type * __restrict    array, \
	                   _type * __restrict    elem, \
	                   stroll_array_cmp_fn * compare, \
	                   void *                data) \
	{ \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(elem >= (array + 1)); \
		stroll_array_assert_intern(compare); \
		\
		_type * elm = &elem[-1]; \
		\
		if (compare(elem, elm, data) < 0) { \
			_type tmp = elem[0]; \
			\
			do { \
				elm[1] = elm[0]; \
				elm--; \
			} while ((elm >= array) && \
			         (compare(&tmp, elm, data) < 0)); \
			\
			elm[1] = tmp; \
		} \
	}

STROLL_ARRAY_DEFINE_INSERT_INPSORT_ELEM(stroll_array_insert_inpsort_elem32,
                                        stroll_array_swap32,
                                        uint32_t)

STROLL_ARRAY_DEFINE_INSERT_INPSORT_ELEM(stroll_array_insert_inpsort_elem64,
                                        stroll_array_swap64,
                                        uint64_t)

static __stroll_nonull(1, 2, 4)
void
stroll_array_insert_inpsort_elem_mem(char * __restrict     array,
                                     char * __restrict     elem,
                                     size_t                size,
                                     stroll_array_cmp_fn * compare,
                                     void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(elem >= (array + size));
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	char * elm = elem - size;

	if (compare(elem, elm, data) < 0) {
		char tmp[size];

		memcpy(tmp, elem, size);

		do {
			memcpy(elm + size, elm, size);
			elm -= size;
		} while ((elm >= (char *)array) &&
		         (compare(tmp, elm, data) < 0));

		memcpy(elm + size, tmp, size);
	}
}

void
stroll_array_insert_inpsort_elem(void * __restrict     array,
                                 void * __restrict     elem,
                                 size_t                size,
                                 stroll_array_cmp_fn * compare,
                                 void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api((char *)elem >= ((char *)array + size));
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_array_insert_inpsort_elem32(array, elem, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_insert_inpsort_elem64(array, elem, compare, data);
	else
		stroll_array_insert_inpsort_elem_mem(array,
		                                     elem,
		                                     size,
		                                     compare,
		                                     data);
}

#define STROLL_ARRAY_INSERT_OOPSORT_ELEM(_func, _type) \
	static __stroll_nonull(1, 2, 3, 5) \
	void \
	_func(_type * __restrict       array, \
	      _type * __restrict       end, \
	      const _type * __restrict elem, \
	      stroll_array_cmp_fn *    compare, \
	      void *                   data) \
	{ \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(end >= array); \
		stroll_array_assert_intern(elem); \
		stroll_array_assert_intern((elem < array) || (elem >= end)); \
		stroll_array_assert_intern(compare); \
		\
		do { \
			_type * last = &end[-1]; \
			\
			if (compare(elem, last, data) >= 0) \
				break; \
			\
			*end = *last; \
			end = last; \
		} while (end != array); \
		\
		*end = *elem; \
	}

STROLL_ARRAY_INSERT_OOPSORT_ELEM(stroll_array_insert_oopsort_elem32, uint32_t)

STROLL_ARRAY_INSERT_OOPSORT_ELEM(stroll_array_insert_oopsort_elem64, uint64_t)

static __stroll_nonull(1, 2, 3, 5)
void
stroll_array_insert_oopsort_elem_mem(char * __restrict       array,
                                     char * __restrict       end,
                                     const char * __restrict elem,
                                     size_t                  size,
                                     stroll_array_cmp_fn *   compare,
                                     void *                  data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(end >= (array + size));
	stroll_array_assert_intern(!((size_t)(end - array) % size));
	stroll_array_assert_intern(elem);
	stroll_array_assert_intern((elem < array) || (elem >= (end + size)));
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	do {
		char * last = end - size;

		if (compare(elem, last, data) >= 0)
			break;

		memcpy(end, last, size);
		end = last;
	} while (end != array);

	memcpy(end, elem, size);
}

void
stroll_array_insert_oopsort_elem(void * __restrict       array,
                                 void * __restrict       end,
                                 const void * __restrict elem,
                                 size_t                  size,
                                 stroll_array_cmp_fn *   compare,
                                 void *                  data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api((char *)end >= ((char *)array + size));
	stroll_array_assert_api(!((size_t)(end - array) % size));
	stroll_array_assert_api(elem);
	stroll_array_assert_api((elem < array) ||
	                        ((const char *)elem >= ((char *)end + size)));
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_array_insert_oopsort_elem32(array,
		                                   end,
		                                   elem,
		                                   compare,
		                                   data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_insert_oopsort_elem64(array,
		                                   end,
		                                   elem,
		                                   compare,
		                                   data);
	else
		stroll_array_insert_oopsort_elem_mem(array,
		                                     end,
		                                     elem,
		                                     size,
		                                     compare,
		                                     data);
}

#define STROLL_ARRAY_DEFINE_INSERT_SORT(_sort_func, _inpsort_elem_func, _type) \
	static __stroll_nonull(1, 3) \
	void \
	_sort_func(_type * __restrict    array, \
	           unsigned int          nr, \
	           stroll_array_cmp_fn * compare, \
	           void *                data) \
	{ \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(nr > 1); \
		stroll_array_assert_intern(compare); \
		\
		_type *       unsort = &array[1]; \
		const _type * last = &array[nr - 1]; \
		\
		while (unsort <= last) { \
			_inpsort_elem_func(array, unsort, compare, data); \
			unsort++; \
		} \
	}

STROLL_ARRAY_DEFINE_INSERT_SORT(stroll_array_insert_sort32,
                                stroll_array_insert_inpsort_elem32,
                                uint32_t)

STROLL_ARRAY_DEFINE_INSERT_SORT(stroll_array_insert_sort64,
                                stroll_array_insert_inpsort_elem64,
                                uint64_t)

static __stroll_nonull(1, 4)
void
stroll_array_insert_sort_mem(char * __restrict     array,
                             unsigned int          nr,
                             size_t                size,
                             stroll_array_cmp_fn * compare,
                             void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(nr > 1);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	char *       unsort = array + size;
	const char * last = array + ((nr - 1) * size);

	while (unsort <= last) {
		stroll_array_insert_inpsort_elem_mem(array,
		                                     unsort,
		                                     size,
		                                     compare,
		                                     data);
		unsort += size;
	}
}

void
stroll_array_insert_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (nr == 1)
		return;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_array_insert_sort32(array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_insert_sort64(array, nr, compare, data);
	else
		stroll_array_insert_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT_UTILS)

struct stroll_array_quick {
	size_t                size;
	stroll_array_cmp_fn * compare;
	void *                data;
	size_t                thres;
};

#define stroll_array_quick_assert(_parms) \
	stroll_array_assert_intern(_parms); \
	stroll_array_assert_intern((_parms)->compare); \
	stroll_array_assert_intern((_parms)->thres)

#define STROLL_ARRAY_DEFINE_QUICK_MED(_med_func, _swap_func, _type) \
	static __stroll_nonull(1, 2, 3) \
	_type \
	_med_func(const struct stroll_array_quick * __restrict parms, \
	          _type *                                      array, \
	          _type *                                      last) \
	{ \
		_type * mid = &array[(last - array) / 2]; \
		\
		if (parms->compare(array, mid, parms->data) > 0) \
			_swap_func(array, mid); \
		\
		if (parms->compare(mid, last, parms->data) > 0) { \
			_swap_func(mid, last); \
			\
			if (parms->compare(array, mid, parms->data) > 0) \
				_swap_func(array, mid); \
		} \
		\
		return *mid; \
	}

#define STROLL_ARRAY_DEFINE_QUICK_HOARE_PART(_part_func, \
                                             _med_func, \
                                             _swap_func, \
                                             _type) \
	static __stroll_nonull(1, 2, 3) \
	_type * \
	_part_func(const struct stroll_array_quick * __restrict parms, \
	           _type *                                      array, \
	           _type *                                      last) \
	{ \
		stroll_array_assert_intern(parms); \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(last > array); \
		\
		_type pivot = _med_func(parms, array, last); \
		\
		while (true) { \
			do { \
				array++; \
			} while (parms->compare(&pivot, \
			                        array, \
			                        parms->data) > 0); \
			\
			do { \
				last--; \
			} while (parms->compare(last, \
			                        &pivot, \
			                        parms->data) > 0); \
			\
			if (array >= last) \
				return last; \
			\
			_swap_func(array, last); \
		} \
	}

#define STROLL_ARRAY_DEFINE_QUICK_SORT(_sort_func, \
                                       _recsort_func, \
                                       _insert_func, \
                                       _thres, \
                                       _type) \
	static __stroll_nonull(1, 3) \
	void \
	_sort_func(_type * __restrict    array, \
	           unsigned int          nr, \
	           stroll_array_cmp_fn * compare, \
	           void *                data) \
	{ \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(nr > 1); \
		stroll_array_assert_intern(compare); \
		\
		const struct stroll_array_quick parms = { \
			.compare  = compare, \
			.data     = data, \
			.thres    = _thres \
		}; \
		\
		_recsort_func(&parms, array, &array[(nr - 1)]); \
		\
		_insert_func(array, nr, compare, data); \
	}

STROLL_ARRAY_DEFINE_QUICK_MED(stroll_array_quick_med32,
                              stroll_array_swap32,
                              uint32_t)
STROLL_ARRAY_DEFINE_QUICK_HOARE_PART(stroll_array_quick_hoare_part32,
                                     stroll_array_quick_med32,
                                     stroll_array_swap32,
                                     uint32_t)

STROLL_ARRAY_DEFINE_QUICK_MED(stroll_array_quick_med64,
                              stroll_array_swap64,
                              uint64_t)
STROLL_ARRAY_DEFINE_QUICK_HOARE_PART(stroll_array_quick_hoare_part64,
                                     stroll_array_quick_med64,
                                     stroll_array_swap64,
                                     uint64_t)

/*
 * Select pivot for the [array:last] array according to the median-of-three
 * partitioning strategy.
 *
 * Basically, pick the first, middle, and last elements of [array:last]
 * array and swap them according to sorted order so that:
 * - first index points to the smallest element,
 * - middle index points to the median element,
 * - and last index points to the largest element.
 *
 * Finally choose the median element as quicksort pivot and return it to caller.
 *
 * This gives a better estimate of the optimal pivot, i.e. the true median
 * element and prevent from pathological cases where array is mostly
 * (reverse-)sorted.
 *
 * Middle index is computed as following: mid = (begin + end) / 2.
 * However, to prevent from addition overflow, compute it as following:
 *     mid = begin + ((end - begin) / 2)
 */
static __stroll_nonull(1, 2, 3, 4)
void
stroll_array_quick_med_mem(
	const struct stroll_array_quick * __restrict parms,
	char *                                       array,
	char *                                       last,
	char * __restrict                            pivot)
{
	size_t sz = parms->size;
	char * mid = &array[((size_t)(last - array) / (2 * sz)) * sz];

	if (parms->compare(array, mid, parms->data) > 0)
		stroll_array_swap(array, mid, sz);

	if (parms->compare(mid, last, parms->data) > 0) {
		stroll_array_swap(mid, last, sz);

		if (parms->compare(array, mid, parms->data) > 0)
			stroll_array_swap(array, mid, sz);
	}

	memcpy(pivot, mid, sz);
}

/*
 * Partition [array:last] array into 2 consecutive and non empty sub-arrays
 * according to Hoare's partitioning scheme so that no element of the first
 * sub-range is greater than any element of the second sub-range.
 *
 * The process explained below, is excerpted from:
 * https://en.wikipedia.org/wiki/Quicksort#Hoare_partition_scheme
 *
 * 2 pointers starting at both ends of the array being partitioned are moved
 * toward each other until an "inversion" is detected.
 * An "inversion" is detected when a pair of elements meets the following
 * condition:
 * - bottom pointer points to an element which value is greater than the pivot
 *   value, AND,
 * - top pointer points to an element which value is smaller than the pivot
 *   value.
 *
 * At this point, if bottom pointer is still smaller than top pointer, meaning
 * that respective pointed to elements are in the wrong order relative to each
 * other, elements are swapped (exchanged) and the search for the an "inversion"
 * is repeated.
 *
 * When eventually the pointers cross, i.e. the bottom one is higher than the
 * top one, no swap is performed and the partioning process has completed with
 * the point of division between the crossed pointers: any entries that might be
 * strictly between the crossed pointers are equal to the pivot and can be
 * excluded from both sub-ranges formed.
 *
 * With this formulation it is possible that one sub-range turns out to be the
 * whole original range, which would prevent the quicksort from advancing
 * (end-less loop). Hoare therefore stipulates that at the end, the sub-range
 * containing the pivot element (which still is at its original position) can be
 * decreased in size by excluding that pivot, after (if necessary) exchanging it
 * with the sub-range element closest to the separation; thus, termination of
 * quicksort is ensured.
 *
 * Hoare's scheme is more efficient than Lomuto's partition scheme because it
 * does 3 times fewer swaps on average.
 * The implementation here ensures that even in cases where all values are
 * equal, balanced partitions are created although inccuring many swap
 * operations in practice.
 * Hoare's partitioning may also degrade to O(n^2) for already sorted input
 * when the pivot is chosen as the first or the last element. To overcome these
 * pathological cases, pivot is chosen according to the median-of-three
 * strategy.
 */
static __stroll_nonull(1, 2, 3)
char *
stroll_array_quick_hoare_part_mem(
	const struct stroll_array_quick * __restrict parms,
	char *                                       array,
	char *                                       last)
{
	stroll_array_assert_intern(parms);
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(last > array);

	size_t sz = parms->size;
	char   pivot[sz];

	/*
	 * Select a patitioning pivot for [array:last] according to the
	 * median-of-three strategy.
	 * Upon return from stroll_array_quick_med_mem(), pivot value has been
	 * stored into `pivot' variable.
	 */
	stroll_array_quick_med_mem(parms, array, last, pivot);

	while (true) {
		/*
		 * Increase `array' (bottom) pointer as long as the element
		 * pointed to by `array' is strictly smaller than pivot value.
		 * There is no need to check for original [array:last]
		 * out-of-bounds access since it is known for sure that pivot
		 * value is stored into original array.
		 */
		do {
			array += sz;
		} while (parms->compare(pivot, array, parms->data) > 0);

		/*
		 * Decrease `last' (top) pointer as long as the element
		 * pointed to by `last' is strictly greater than pivot value.
		 * There is no need to check for original [array:last]
		 * out-of-bounds access since it is known for sure that pivot
		 * value is stored into original array.
		 */
		do {
			last -= sz;
		} while (parms->compare(last, pivot, parms->data) > 0);

		if (array >= last)
			/*
			 * `array' (bottom) pointer and `last' (top) pointer
			 * have crossed, meaning that the partitioning is over.
			 * Return pointer to the newly found partition pivot,
			 * which should be included by the caller into one of
			 * the sub-arrays.
			 */
			return last;

		/*
		 * Swap `array' and `last' elements to reorder them relative to
		 * each other.
		 */
		stroll_array_swap(array, last, sz);
	}
}

#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT_UTILS) */

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)

#if CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD < 2
#error Invalid Quicksort insertion threshold !
#endif /* CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD < 2 */

#define STROLL_QSORT_INSERT_THRESHOLD \
	STROLL_CONCAT(CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD, U)

#define STROLL_ARRAY_QUICK_RECSORT(_recsort_func, _part_func, _type) \
	static __stroll_nonull(1, 2, 3) \
	void \
	_recsort_func(const struct stroll_array_quick * __restrict parms, \
	              _type *                                      array, \
	              _type *                                      last) \
	{ \
		stroll_array_quick_assert(parms); \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(last >= array); \
		\
		while (&array[parms->thres] <= last) { \
			_type * pivot = _part_func(parms, array, last); \
			\
			if ((pivot - array) < (last - pivot)) { \
				_recsort_func(parms, array, pivot); \
				array = pivot + 1; \
			} \
			else { \
				_recsort_func(parms, pivot + 1, last); \
				last = pivot; \
			} \
		} \
	}

STROLL_ARRAY_QUICK_RECSORT(stroll_array_quick_sort_rec32,
                           stroll_array_quick_hoare_part32,
                           uint32_t)
STROLL_ARRAY_DEFINE_QUICK_SORT(stroll_array_quick_sort32,
                               stroll_array_quick_sort_rec32,
                               stroll_array_insert_sort32,
                               STROLL_QSORT_INSERT_THRESHOLD,
                               uint32_t)

STROLL_ARRAY_QUICK_RECSORT(stroll_array_quick_sort_rec64,
                           stroll_array_quick_hoare_part64,
                           uint64_t)
STROLL_ARRAY_DEFINE_QUICK_SORT(stroll_array_quick_sort64,
                               stroll_array_quick_sort_rec64,
                               stroll_array_insert_sort64,
                               STROLL_QSORT_INSERT_THRESHOLD,
                               uint64_t)

/*
 * A recursive implementation of quicksort with recursive tail call elimination
 * and O(log(n)) stack space requirement optimizations.
 *
 * As stated into https://en.wikipedia.org/wiki/Quicksort:
 * --
 *  Quicksort works by selecting a 'pivot' element from the array and
 *  partitioning the other elements into two sub-arrays, according to whether
 *  they are less than or greater than the pivot.
 *  [...]
 *  The sub-arrays are then sorted recursively. This can be done in-place,
 *  requiring small additional amounts of memory to perform the sorting. --
 *
 * In addition, recursion is stopped early when the number of elements of
 * current partition is no more the STROLL_QSORT_INSERT_THRESHOLD. This leaves
 * the array k-sorted (where k equals STROLL_QSORT_INSERT_THRESHOLD), meaning
 * that each element is at most k positions away from its final sorted
 * position.
 * Caller must run a final sorting pass using a simple algorithm such as
 * insertion sort upon return from stroll_array_quick_sort_recmem().
 *
 * For more infos about usual quicksort optimations, see:
 * https://www.geeksforgeeks.org/tail-call-elimination/
 * https://www.geeksforgeeks.org/quicksort-tail-call-optimization-reducing-worst-case-space-log-n/
 */

static __stroll_nonull(1, 2, 3)
void
stroll_array_quick_sort_recmem(
	const struct stroll_array_quick * __restrict parms,
	char *                                       array,
	char *                                       last)
{
	stroll_array_quick_assert(parms);
	stroll_array_assert_intern(parms->size);
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(last >= array);
	stroll_array_assert_intern(!((size_t)(last - array) % parms->size));

	/*
	 * Transform the quicksort standard double recursive call to this
	 * `while (...) { ... }' based construct to restrict to a single
	 * recursive call. This allows GCC optimizer to perform recursive tail
	 * call elimination.
	 *
	 * Also note that we return to caller when [array:last] contains no more
	 * than STROLL_QSORT_INSERT_THRESHOLD elements so that it may switch to
	 * a simple non-recursive algorithm for final sorting pass.
	 */
	while (&array[parms->thres] <= last) {
		size_t sz = parms->size;
		char * pivot;

		/*
		 * Partition [array:last] into 2 sub-arrays according to the
		 * Hoare's partitioning scheme:
		 * - bottom / lower sub-array [array:pivot],
		 * - top / upper sub-array [pivot+size:last]
		 *
		 * Including pivot within one of the sub-arrays ensures that
		 * sub-arrays are always non empty since [array:last] contains
		 * at least STROLL_QSORT_INSERT_THRESHOLD elements (note the
		 * while condition above).
		 *
		 * This is absolutly necessary to prevent quicksort from
		 * entering an endless loop where one sub-array turns out to be
		 * the  whole original [array:last] range.
		 */
		pivot = stroll_array_quick_hoare_part_mem(parms, array, last);

		/*
		 * Always recurse into the smaller sub-array first to limit
		 * the required auxiliary stack space to O(log(n)) in the worst
		 * case. This may happen when the original [array:last] array
		 * is always sub-divided into :
		 * - one sub-array containing one single element,
		 * - a second sub-array containing all other elements.
		 */
		if ((pivot - array) < (last - pivot)) {
			stroll_array_quick_sort_recmem(parms, array, pivot);
			array = &pivot[sz];
		}
		else {
			stroll_array_quick_sort_recmem(parms, &pivot[sz], last);
			last = pivot;
		}
	}
}

static __stroll_nonull(1, 4)
void
stroll_array_quick_sort_mem(char * __restrict     array,
                            unsigned int          nr,
                            size_t                size,
                            stroll_array_cmp_fn * compare,
                            void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(nr > 1);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	const struct stroll_array_quick parms = {
		.size     = size,
		.compare  = compare,
		.data     = data,
		.thres    = STROLL_QSORT_INSERT_THRESHOLD * size
	};

	/*
	 * Sort according to recursive quicksort algorithm as long as the number
	 * of elements of current partition is larger than
	 * STROLL_QSORT_INSERT_THRESHOLD.
	 */
	stroll_array_quick_sort_recmem(&parms, array, &array[(nr - 1) * size]);

	/*
	 * Now that array is k-sorted (where k equals
	 * STROLL_QSORT_INSERT_THRESHOLD), switch to insertion sort for the
	 * final pass. Here, insertion sort takes O(kn) time to finish the sort.
	 * Compared to the "many small sorts" alternative (where the switch to
	 * insertion sort is performed during the recursion), this version may
	 * execute fewer instructions, but it may make use of data caches in a
	 * suboptimal way.
	 * In practice, switching to insertion sort as a final sorting pass
	 * seems to give better result however.
	 */
	stroll_array_insert_sort_mem(array, nr, size, compare, data);
}

void
stroll_array_quick_sort(void * __restrict     array,
                        unsigned int          nr,
                        size_t                size,
                        stroll_array_cmp_fn * compare,
                        void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (nr == 1)
		return;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_array_quick_sort32(array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_quick_sort64(array, nr, compare, data);
	else
		stroll_array_quick_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */

#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT)

#if CONFIG_STROLL_ARRAY_3WQUICK_SORT_INSERT_THRESHOLD < 2
#error Invalid 3-way quicksort insertion threshold !
#endif /* CONFIG_STROLL_ARRAY_3WQUICK_SORT_INSERT_THRESHOLD < 2 */

#define STROLL_3WQSORT_INSERT_THRESHOLD \
	STROLL_CONCAT(CONFIG_STROLL_ARRAY_3WQUICK_SORT_INSERT_THRESHOLD, U)

#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT_DIJKSTRA)

#define STROLL_ARRAY_3WQUICK_PART(_part_func, _med_func, _swap_func, _type) \
	static __stroll_nonull(1, 2, 3) \
	void \
	_part_func(const struct stroll_array_quick * __restrict parms, \
	           _type ** __restrict                          low, \
	           _type ** __restrict                          high) \
	{ \
		stroll_array_assert_intern(parms); \
		stroll_array_assert_intern(low); \
		stroll_array_assert_intern(*low); \
		stroll_array_assert_intern(high); \
		stroll_array_assert_intern(*high > *low); \
		\
		_type * bot = *low; \
		_type * top = *high; \
		_type   pivot = _med_func(parms, bot, top); \
		_type * elm; \
		\
		while (parms->compare(bot, &pivot, parms->data) < 0) \
			bot++; \
		\
		while (parms->compare(top, &pivot, parms->data) > 0) \
			top--; \
		\
		elm = bot; \
		while (elm <= top) { \
			int cmp = parms->compare(elm, &pivot, parms->data); \
			\
			if (cmp < 0) \
				_swap_func(bot++, elm++); \
			else if (cmp > 0) \
				_swap_func(top--, elm); \
			else \
				elm++; \
		} \
		\
		*low = bot - 1; \
		*high = top + 1; \
	}

#endif /* defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT_DIJKSTRA) */

#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT_BENTLEY_MCILROY)

#define STROLL_ARRAY_3WQUICK_PART(_part_func, _med_func, _swap_func, _type) \
	static __stroll_nonull(1, 2, 3) \
	void \
	_part_func(const struct stroll_array_quick * __restrict parms, \
	           _type ** __restrict                          low, \
	           _type ** __restrict                          high) \
	{ \
		stroll_array_assert_intern(parms); \
		stroll_array_assert_intern(low); \
		stroll_array_assert_intern(*low); \
		stroll_array_assert_intern(high); \
		stroll_array_assert_intern(*high > *low); \
		\
		_type * leq = *low; \
		_type * bot = leq - 1; \
		_type * heq = *high; \
		_type * top = heq + 1; \
		_type   pivot = _med_func(parms, leq, heq); \
		long    lnr; \
		long    hnr; \
		_type * elm; \
		\
		while (true) { \
			do { \
				bot++; \
			} while (parms->compare(bot, \
			                        &pivot, \
			                        parms->data) < 0); \
			\
			do { \
				top--; \
			} while (parms->compare(top, \
			                        &pivot, \
			                        parms->data) > 0); \
			\
			if (bot >= top) \
				break; \
			\
			_swap_func(bot, top); \
			\
			if (!parms->compare(bot, &pivot, parms->data)) \
				_swap_func(bot, leq++); \
			if (!parms->compare(top, &pivot, parms->data)) \
				_swap_func(top, heq--); \
		} \
		\
		lnr = bot - leq; \
		hnr = heq - top; \
		\
		if ((leq - *low) < lnr) \
			leq = *low + lnr; \
		elm = *low; \
		while (leq < bot) \
			_swap_func(elm++, leq++); \
		\
		if (hnr < (*high - heq)) \
			heq = *high + 1 - hnr; \
		else \
			heq++; \
		elm = top + 1; \
		while (heq <= *high) \
			_swap_func(elm++, heq++); \
		\
		*low += lnr - 1; \
		*high -= hnr - 1; \
	}

#endif /* defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT_BENTLEY_MCILROY) */

#define STROLL_ARRAY_3WQUICK_RECSORT(_recsort_func, _part_func, _type) \
	static __stroll_nonull(1, 2, 3) \
	void \
	_recsort_func(const struct stroll_array_quick * __restrict parms, \
	              _type *                                      array, \
	              _type *                                      last) \
	{ \
		stroll_array_quick_assert(parms); \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(last); \
		\
		while (&array[parms->thres] <= last) { \
			_type * low = array; \
			_type * high = last; \
			\
			_part_func(parms, &low, &high); \
			\
			if ((low - array) < (last - high)) { \
				_recsort_func(parms, array, low); \
				array = high; \
			} \
			else { \
				_recsort_func(parms, high, last); \
				last = low; \
			} \
		} \
	}

STROLL_ARRAY_3WQUICK_PART(stroll_array_3wquick_part32,
                          stroll_array_quick_med32,
                          stroll_array_swap32,
                          uint32_t)
STROLL_ARRAY_3WQUICK_RECSORT(stroll_array_3wquick_sort_rec32,
                             stroll_array_3wquick_part32,
                             uint32_t)
STROLL_ARRAY_DEFINE_QUICK_SORT(stroll_array_3wquick_sort32,
                               stroll_array_3wquick_sort_rec32,
                               stroll_array_insert_sort32,
                               STROLL_3WQSORT_INSERT_THRESHOLD,
                               uint32_t)

STROLL_ARRAY_3WQUICK_PART(stroll_array_3wquick_part64,
                          stroll_array_quick_med64,
                          stroll_array_swap64,
                          uint64_t)
STROLL_ARRAY_3WQUICK_RECSORT(stroll_array_3wquick_sort_rec64,
                             stroll_array_3wquick_part64,
                             uint64_t)
STROLL_ARRAY_DEFINE_QUICK_SORT(stroll_array_3wquick_sort64,
                               stroll_array_3wquick_sort_rec64,
                               stroll_array_insert_sort64,
                               STROLL_3WQSORT_INSERT_THRESHOLD,
                               uint64_t)

#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT_DIJKSTRA)

/*
 * Partition [low:high] array into 3 consecutive sub-arrays according to
 * Dijkstra 3-way partitioning scheme so that:
 * - first sub-range contains elements strictly smaller than pivot,
 * - second sub-range contains elements equal to pivot,
 * - third sub-range contains elements strictly greater than pivot.
 *
 * The process is based on the "Dutch National Flag" problem and is explained
 * here:
 * https://algs4.cs.princeton.edu/lectures/demo/23DemoPartitioning.pdf
 *
 * Dijkstra's 3-way partitioning enhances quicksort performances for inputs
 * where there are many duplicate values. However, as it exhibits poor locality
 * of references and performs more swaps than necessary, the standard Hoare's
 * partitioning scheme should be preferred for inputs showing small to moderate
 * number of duplicates.
 *
 * The Bentley-McIlroy 3-way partitioning alternative helps in mitigating
 * Dijkstra's 3-way partitioning scheme performance decrease for inputs with
 * moderate to large number of distinct key values.
 */
static __stroll_nonull(1, 2, 3)
void
stroll_array_3wquick_part_mem(
	const struct stroll_array_quick * __restrict parms,
	char ** __restrict                           low,
	char ** __restrict                           high)
{
	stroll_array_assert_intern(parms);
	stroll_array_assert_intern(low);
	stroll_array_assert_intern(*low);
	stroll_array_assert_intern(high);
	stroll_array_assert_intern(*high > *low);

	char * bot = *low;
	char * top = *high;
	size_t sz = parms->size;
	char   pivot[sz];
	char * elm;

	stroll_array_quick_med_mem(parms, bot, top, pivot);

	/* Locate the middle sub-array boundaries. */
	while (parms->compare(bot, pivot, parms->data) < 0)
		bot += sz;
	while (parms->compare(top, pivot, parms->data) > 0)
		top -= sz;

	elm = bot;
	while (elm <= top) {
		int cmp = parms->compare(elm, pivot, parms->data);

		if (cmp < 0) {
			/* Swap element smaller than pivot to left sub-array. */
			stroll_array_swap(bot, elm, sz);
			bot += sz;
			elm += sz;
		}
		else if (cmp > 0) {
			/*
			 * Swap element greater than pivot to right sub-array.
			 */
			stroll_array_swap(top, elm, sz);
			top -= sz;
		}
		else
			/*
			 * Leave element equal to pivot as-is, i.e., in the
			 * middle sub-array.
			 */
			elm += sz;
	}

	*low = bot - sz;
	*high = top + sz;
}

#endif /* defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT_DIJKSTRA) */

#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT_BENTLEY_MCILROY)

/*
 * Partition [low:high] array into 3 consecutive sub-arrays according to
 * Bentley-McIlroy 3-way partitioning scheme so that:
 * - first (lower) sub-range contains elements strictly smaller than pivot,
 * - second (middle) sub-range contains elements equal to pivot,
 * - third (upper) sub-range contains elements strictly greater than pivot.
 *
 * The algorithm, also known as "fat partitioning", is a 2 phases process.
 *
 * First phase sub-divides the whole [low:high] array into 4 sub-arrays so that:
 * - keys equal to pivot are swapped to both lower / left and upper / right ends
 *   of the original [low:high] array ;
 * - keys smaller than pivot are swapped into the lower portion of the middle
 *   sub-array,
 * - while keys larger than pivot are swapped into the upper portion of the
 *   middle sub-array,
 * such as what is shown below:
 *
 *     *low        leq       bot                  top       heq        *high
 *       |          |         |                    |         |           |
 *       v          v         v                    v         v           v
 *      +-----------------------------------------------------------------+
 *      | == pivot | < pivot |         ??           | > pivot |  == pivot |
 *      +-----------------------------------------------------------------+
 *
 * Phase 2 relocates elements equal to pivot into their final location by
 * swapping them into the middle sub-array so that, at the end of operation, the
 * whole original array looks like:
 *
 *               *low                                        *high
 *                 |                                           |
 *                 v                                           v
 *      +-----------------------------------------------------------------+
 *      |  < pivot  |               == pivot                  |  > pivot  |
 *      +-----------------------------------------------------------------+
 *
 * The process is explained here:
 * https://sedgewick.io/wp-content/uploads/2022/03/2002QuicksortIsOptimal.pdf
 * and detailed step by step here:
 * https://algs4.cs.princeton.edu/lectures/demo/23DemoPartitioning.pdf
 *
 * Bentley-McIlroy 3-way partitioning is an improvement over Dijkstra's scheme
 * since it limits the extra overhead required to process keys equal to pivot
 * with respect to the standard quicksort partitioning logic.
 *
 * It is a suitable alternative to the standard 2-way partitioning logic when
 * inputs contain a small number of distinct key values.
 */
static __stroll_nonull(1, 2, 3)
void
stroll_array_3wquick_part_mem(
	const struct stroll_array_quick * __restrict parms,
	char ** __restrict                           low,
	char ** __restrict                           high)
{
	stroll_array_assert_intern(parms);
	stroll_array_assert_intern(low);
	stroll_array_assert_intern(*low);
	stroll_array_assert_intern(high);
	stroll_array_assert_intern(*high > *low);

	size_t sz = parms->size;
	char * leq = *low;       /* lower sub-array of values equal to pivot */
	char * bot = leq - sz;   /* sub-array of values smaller than pivot */
	char * heq = *high;      /* upper sub-array of values equal to pivot */
	char * top = heq + sz;   /* sub-array of values larger than pivot */
	char   pivot[sz];        /* pivot value */
	size_t lsz;              /* size of final sub-array holding values
	                          * smaller than pivot. */
	size_t hsz;              /* size of final sub-array holding values
	                          * larger than pivot. */
	char * elm;

	/* Select pivot value according to median-of-three strategy. */
	stroll_array_quick_med_mem(parms, leq, heq, pivot);

	while (true) {
		/*
		 * From lower sub-array, iterating upward, find first element
		 * greater than or equal to pivot.
		 */
		do {
			bot += sz;
		} while (parms->compare(bot, pivot, parms->data) < 0);
		/*
		 * From upper sub-array, iterating downward, find first element
		 * greater than or equal to pivot.
		 */
		do {
			top -= sz;
		} while (parms->compare(top, pivot, parms->data) > 0);

		if (bot >= top)
			/*
			 * Pointer to ends of both lower and upper sub-arrays
			 * have crossed, meaning that all elements have been
			 * swapped to their proper temporary location:
			 * proceed to second phase to relocate values equal to
			 * pivot into their final location, i.e., the middle
			 * sub-array.
			 */
			break;

		stroll_array_swap(bot, top, sz);

		/* Swap key equal to pivot to lower / left array end. */
		if (!parms->compare(bot, pivot, parms->data)) {
			stroll_array_swap(bot, leq, sz);
			leq += sz;
		}
		/* Swap key equal to pivot to upper / right array end. */
		if (!parms->compare(top, pivot, parms->data)) {
			stroll_array_swap(top, heq, sz);
			heq -= sz;
		}
	}

	/*
	 * Relocate values equal to pivot into their final location, i.e., the
	 * middle sub-array.
	 */

	lsz = (size_t)(bot - leq);
	hsz = (size_t)(heq - top);

	/*
	 * Swap elements smaller than pivot value to the final lower sub-array,
	 * relocating elements equal to pivot value into the middle sub-array at
	 * the same time.
	 */
	if ((size_t)(leq - *low) < lsz)
		leq = *low + lsz;
	elm = *low;
	while (leq < bot) {
		stroll_array_swap(elm, leq, sz);
		elm += sz;
		leq += sz;
	}

	/*
	 * Swap elements larger than pivot value to the final upper sub-array,
	 * relocating elements equal to pivot value into the middle sub-array at
	 * the same time.
	 */
	if (hsz < (size_t)(*high - heq))
		heq = *high + sz - hsz;
	else
		heq += sz;
	elm = top + sz;
	while (heq <= *high) {
		stroll_array_swap(elm, heq, sz);
		elm += sz;
		heq += sz;
	}

	/*
	 * Return pointer to last element of sub-array holding values smaller
	 * than pivot.
	 */
	*low += lsz - sz;
	/*
	 * Return pointer to first element of sub-array holding values larger
	 * than pivot.
	 */
	*high -= hsz - sz;
}

#endif /* defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT_BENTLEY_MCILROY) */

static __stroll_nonull(1, 2, 3)
void
stroll_array_3wquick_sort_recmem(
	const struct stroll_array_quick * __restrict parms,
	char *                                       array,
	char *                                       last)
{
	stroll_array_quick_assert(parms);
	stroll_array_assert_intern(parms->size);
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(last);
	stroll_array_assert_intern(!((size_t)(last - array) % parms->size));

	while (&array[parms->thres] <= last) {
		char * low = array;
		char * high = last;

		stroll_array_3wquick_part_mem(parms, &low, &high);

		if ((low - array) < (last - high)) {
			stroll_array_3wquick_sort_recmem(parms, array, low);
			array = high;
		}
		else {
			stroll_array_3wquick_sort_recmem(parms, high, last);
			last = low;
		}
	}
}

static __stroll_nonull(1, 4)
void
stroll_array_3wquick_sort_mem(char * __restrict     array,
                              unsigned int          nr,
                              size_t                size,
                              stroll_array_cmp_fn * compare,
                              void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(nr > 1);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	const struct stroll_array_quick parms = {
		.size     = size,
		.compare  = compare,
		.data     = data,
		.thres    = STROLL_3WQSORT_INSERT_THRESHOLD * size
	};

	/*
	 * Sort according to recursive 3-way partition quicksort algorithm as
	 * long as the number of elements of current partition is larger than
	 * STROLL_QSORT_INSERT_THRESHOLD.
	 */
	stroll_array_3wquick_sort_recmem(&parms,
	                                 array,
	                                 &array[(nr - 1) * size]);

	/*
	 * Now that array is k-sorted (where k equals
	 * STROLL_QSORT_INSERT_THRESHOLD), switch to insertion sort for the
	 * final pass. Here, insertion sort takes O(kn) time to finish the sort.
	 * Compared to the "many small sorts" alternative (where the switch to
	 * insertion sort is performed during the recursion), this version may
	 * execute fewer instructions, but it may make use of data caches in a
	 * suboptimal way.
	 * In practice, switching to insertion sort as a final sorting pass
	 * seems to give better result however.
	 */
	stroll_array_insert_sort_mem(array, nr, size, compare, data);
}

void
stroll_array_3wquick_sort(void * __restrict     array,
                          unsigned int          nr,
                          size_t                size,
                          stroll_array_cmp_fn * compare,
                          void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (nr == 1)
		return;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_array_3wquick_sort32(array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_3wquick_sort64(array, nr, compare, data);
	else
		stroll_array_3wquick_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT) */

#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT)

#if CONFIG_STROLL_ARRAY_MERGE_SORT_INSERT_THRESHOLD < 2
#error Invalid Mergesort insertion threshold !
#endif /* CONFIG_STROLL_ARRAY_MERGE_SORT_INSERT_THRESHOLD < 2 */

#define STROLL_MSORT_INSERT_THRESHOLD \
	STROLL_CONCAT(CONFIG_STROLL_ARRAY_MERGE_SORT_INSERT_THRESHOLD, U)

struct stroll_array_merge {
	size_t                size;
	stroll_array_cmp_fn * compare;
	void *                data;
	size_t                thres;
};

#define stroll_array_merge_assert(_parms) \
	stroll_array_assert_intern(_parms); \
	stroll_array_assert_intern((_parms)->compare); \
	stroll_array_assert_intern((_parms)->thres)

#define STROLL_ARRAY_MERGE_PART_RUNS(_func, _type) \
	static inline __stroll_nonull(1, 2, 3, 4, 5, 6) \
	void \
	_func(const struct stroll_array_merge * __restrict parms, \
	      _type * __restrict                           result, \
	      const _type * __restrict                     part0, \
	      const _type * __restrict                     end0, \
	      const _type * __restrict                     part1, \
	      const _type * __restrict                     end1) \
	{ \
		do { \
			if (parms->compare(part0, part1, parms->data) <= 0) { \
				*result = *part0; \
				part0++; \
			} \
			else { \
				*result = *part1; \
				part1++; \
			} \
			\
			result++; \
		} while ((part0 < end0) && (part1 < end1)); \
		\
		if (part0 != end0) { \
			stroll_array_assert_intern(part0 < end0); \
			stroll_array_assert_intern(part1 >= end1); \
			\
			memcpy(result, \
			       part0, \
			       (size_t)(end0 - part0) * parms->size); \
		} \
		else if (part1 != end1) { \
			stroll_array_assert_intern(part1 < end1); \
			stroll_array_assert_intern(part0 >= end0); \
			\
			memcpy(result, \
			       part1, \
			       (size_t)(end1 - part1) * parms->size); \
		} \
	}

#define STROLL_ARRAY_MERGE_PARTS(_func, _runs_func, _type) \
	static __stroll_nonull(1, 2, 3, 4, 5, 6) \
	void \
	_func(const struct stroll_array_merge * __restrict parms, \
	      _type * __restrict                           result, \
	      const _type * __restrict                     part0, \
	      const _type * __restrict                     end0, \
	      const _type * __restrict                     part1, \
	      const _type * __restrict                     end1) \
	{ \
		stroll_array_merge_assert(parms); \
		stroll_array_assert_intern(parms->size); \
		stroll_array_assert_intern(part0); \
		stroll_array_assert_intern(end0 > part0); \
		stroll_array_assert_intern(part1); \
		stroll_array_assert_intern(end1 > part1); \
		\
		if (stroll_array_merge_full_runs(parms, \
		                                 (char *)result, \
		                                 (const char *)part0, \
		                                 (const char *)end0, \
		                                 (const char *)part1, \
		                                 (const char *)end1)) \
			return; \
		\
		_runs_func(parms, result, part0, end0, part1, end1); \
	}

#define STROLL_ARRAY_INSERT_OOPSORT(_func, _insert_oopsort_elem, _type) \
	static __stroll_nonull(1, 2, 3, 4) \
	void \
	_func(const struct stroll_array_merge * __restrict parms, \
	      _type * __restrict                           result, \
	      const _type * __restrict                     array, \
	      const _type * __restrict                     end) \
	{ \
		_type * res; \
		\
		*result = *array; \
		\
		for (res = result + 1, array++; array != end; res++, array++) \
			_insert_oopsort_elem(result, \
			                     res, \
			                     array, \
			                     parms->compare, \
			                     parms->data); \
	}

#define STROLL_ARRAY_MERGE_TOPDWN(_func, _insert_oopsort, _merge_func, _type) \
	static __stroll_nonull(1, 2, 3, 4) \
	void \
	_func(const struct stroll_array_merge * __restrict parms, \
	      _type * __restrict                           result, \
	      _type * __restrict                           array, \
	      const _type * __restrict                     end) \
	{ \
		stroll_array_merge_assert(parms); \
		stroll_array_assert_intern(result); \
		stroll_array_assert_intern(end > array); \
		\
		unsigned int nr = (unsigned int)(end - array); \
		unsigned int nr0; \
		\
		if ((nr * sizeof(*array)) <= parms->thres) { \
			stroll_array_assert_intern(nr >= 1); \
			_insert_oopsort(parms, result, array, end); \
			return; \
		} \
		\
		nr0 = nr / 2; \
		\
		_func(parms, &result[nr0], &array[nr0], end); \
		_func(parms, &array[nr - nr0], array, &array[nr0]); \
		_merge_func(parms, \
		            result, \
		            &array[nr - nr0], \
		            end, \
		            &result[nr0], \
		            &result[nr]); \
	}

#define STROLL_ARRAY_MERGE_SORT(_func, _topdwn_func, _merge_func, _type) \
	static __stroll_nonull(1, 3) \
	int \
	_func(_type * __restrict    array, \
	      unsigned int          nr, \
	      stroll_array_cmp_fn * compare, \
	      void *                data) \
	{ \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(nr > 1); \
		stroll_array_assert_intern(compare); \
		\
		_type *                         aux; \
		unsigned int                    anr = nr / 2; \
		_type *                         mid = &array[anr]; \
		unsigned int                    mnr = nr - anr; \
		const struct stroll_array_merge parms = { \
			.size    = sizeof(*array), \
			.compare = compare, \
			.data    = data, \
			.thres   = STROLL_MSORT_INSERT_THRESHOLD * \
			           sizeof(*array) \
		}; \
		\
		aux = malloc(mnr * sizeof(*array)); \
		if (!aux) \
			return -errno; \
		\
		_topdwn_func(&parms, aux, mid, &mid[mnr]); \
		_topdwn_func(&parms, &array[mnr], array, mid); \
		\
		_merge_func(&parms, \
		            array, \
		            &array[mnr], &mid[mnr], \
		            aux, &aux[mnr]); \
		\
		free(aux); \
		\
		return 0; \
	}

#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT_FULL_RUNS)

static inline __stroll_nonull(1, 2, 3, 4, 5, 6)
bool
stroll_array_merge_full_runs(
	const struct stroll_array_merge * __restrict parms,
	char * __restrict                            result,
	const char * __restrict                      part0,
	const char * __restrict                      end0,
	const char * __restrict                      part1,
	const char * __restrict                      end1)
{
	size_t sz = parms->size;

	if (parms->compare(end0 - sz, part1, parms->data) <= 0) {
		sz = (size_t)(end0 - part0);
		memcpy(result, part0, sz);
		memcpy(&result[sz], part1, (size_t)(end1 - part1));
		return true;
	}
	else if (parms->compare(end1 - sz, part0, parms->data) <= 0) {
		sz = (size_t)(end1 - part1);
		memcpy(result, part1, sz);
		memcpy(&result[sz], part0, (size_t)(end0 - part0));
		return true;
	}
	else
		return false;
}

#else  /* !defined(CONFIG_STROLL_ARRAY_MERGE_SORT_FULL_RUNS) */

static inline __stroll_nonull(1, 2, 3, 4, 5, 6)
bool
stroll_array_merge_full_runs(
	const struct stroll_array_merge * __restrict parms __unused,
	char * __restrict                            result __unused,
	const char * __restrict                      part0 __unused,
	const char * __restrict                      end0 __unused,
	const char * __restrict                      part1 __unused,
	const char * __restrict                      end1 __unused)
{
	return false;
}

#endif /* defined(CONFIG_STROLL_ARRAY_MERGE_SORT_FULL_RUNS) */

STROLL_ARRAY_MERGE_PART_RUNS(stroll_array_merge_part_runs32, uint32_t)
STROLL_ARRAY_MERGE_PARTS(stroll_array_merge_parts32,
                         stroll_array_merge_part_runs32,
                         uint32_t)
STROLL_ARRAY_INSERT_OOPSORT(stroll_array_insert_oopsort32,
                            stroll_array_insert_oopsort_elem32,
                            uint32_t)
STROLL_ARRAY_MERGE_TOPDWN(stroll_array_merge_topdwn32,
                          stroll_array_insert_oopsort32,
                          stroll_array_merge_parts32,
                          uint32_t)
STROLL_ARRAY_MERGE_SORT(stroll_array_merge_sort32,
                        stroll_array_merge_topdwn32,
                        stroll_array_merge_parts32,
                        uint32_t)

STROLL_ARRAY_MERGE_PART_RUNS(stroll_array_merge_part_runs64, uint64_t)
STROLL_ARRAY_MERGE_PARTS(stroll_array_merge_parts64,
                         stroll_array_merge_part_runs64,
                         uint64_t)
STROLL_ARRAY_INSERT_OOPSORT(stroll_array_insert_oopsort64,
                            stroll_array_insert_oopsort_elem64,
                            uint64_t)
STROLL_ARRAY_MERGE_TOPDWN(stroll_array_merge_topdwn64,
                          stroll_array_insert_oopsort64,
                          stroll_array_merge_parts64,
                          uint64_t)
STROLL_ARRAY_MERGE_SORT(stroll_array_merge_sort64,
                        stroll_array_merge_topdwn64,
                        stroll_array_merge_parts64,
                        uint64_t)

#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT_RUNS_BYBLOCK)

static inline __stroll_nonull(1, 2, 3, 4, 5, 6)
void
stroll_array_merge_part_runs_mem(
	const struct stroll_array_merge * __restrict parms,
	char * __restrict                            result,
	const char * __restrict                      part0,
	const char * __restrict                      end0,
	const char * __restrict                      part1,
	const char * __restrict                      end1)
{
	size_t sz = parms->size;

	while (true) {
		const char * run = part0;
		size_t       rsz;

		do {
			if (parms->compare(part0, part1, parms->data) > 0)
				break;
			part0 += sz;
		} while (part0 < end0);

		rsz = (size_t)(part0 - run);
		memcpy(result, run, rsz);
		result += rsz;

		if (part0 == end0) {
			stroll_array_assert_intern(part0 <= end0);
			stroll_array_assert_intern(part1 <= end1);
			memcpy(result, part1, (size_t)(end1 - part1));
			return;
		}

		run = part1;
		do {
			if (parms->compare(part0, part1, parms->data) <= 0)
				break;
			part1 += sz;
		} while (part1 < end1);

		rsz = (size_t)(part1 - run);
		memcpy(result, run, rsz);
		result += rsz;

		if (part1 == end1) {
			stroll_array_assert_intern(part0 <= end0);
			stroll_array_assert_intern(part1 <= end1);
			memcpy(result, part0, (size_t)(end0 - part0));
			return;
		}
	}
}

#else  /* !defined(CONFIG_STROLL_ARRAY_MERGE_SORT_RUNS_BYBLOCK) */

static inline __stroll_nonull(1, 2, 3, 4, 5, 6)
void
stroll_array_merge_part_runs_mem(
	const struct stroll_array_merge * __restrict parms,
	char * __restrict                            result,
	const char * __restrict                      part0,
	const char * __restrict                      end0,
	const char * __restrict                      part1,
	const char * __restrict                      end1)
{
	size_t sz = parms->size;

	do {
		if (parms->compare(part0, part1, parms->data) <= 0) {
			memcpy(result, part0, sz);
			part0 += sz;
		}
		else {
			memcpy(result, part1, sz);
			part1 += sz;
		}

		result += sz;
	} while ((part0 < end0) && (part1 < end1));

	if (part0 != end0) {
		stroll_array_assert_intern(part0 < end0);
		stroll_array_assert_intern(part1 >= end1);

		memcpy(result, part0, (size_t)(end0 - part0));
	}
	else if (part1 != end1) {
		stroll_array_assert_intern(part1 < end1);
		stroll_array_assert_intern(part0 >= end0);

		memcpy(result, part1, (size_t)(end1 - part1));
	}
}

#endif /* defined(CONFIG_STROLL_ARRAY_MERGE_SORT_RUNS_BYBLOCK) */

static __stroll_nonull(1, 2, 3, 4, 5, 6)
void
stroll_array_merge_parts_mem(
	const struct stroll_array_merge * __restrict parms,
	char * __restrict                            result,
	const char * __restrict                      part0,
	const char * __restrict                      end0,
	const char * __restrict                      part1,
	const char * __restrict                      end1)
{
	stroll_array_merge_assert(parms);
	stroll_array_assert_intern(parms->size);
	stroll_array_assert_intern(part0);
	stroll_array_assert_intern(end0 > part0);
	stroll_array_assert_intern(!((size_t)(end0 - part0) % parms->size));
	stroll_array_assert_intern(part1);
	stroll_array_assert_intern(end1 > part1);
	stroll_array_assert_intern(!((size_t)(end1 - part1) % parms->size));

	if (stroll_array_merge_full_runs(parms,
	                                 result,
	                                 part0,
	                                 end0,
	                                 part1,
	                                 end1))
		return;

	stroll_array_merge_part_runs_mem(parms,
	                                 result,
	                                 part0,
	                                 end0,
	                                 part1,
	                                 end1);
}

static __stroll_nonull(1, 2, 3, 4)
void
stroll_array_insert_oopsort_mem(
	const struct stroll_array_merge * __restrict parms,
	char * __restrict                            result,
	const char * __restrict                      array,
	const char * __restrict                      end)
{
	char * res;

	memcpy(result, array, parms->size);

	for (res = result + parms->size, array += parms->size;
	     array != end;
	     res += parms->size, array += parms->size)
		stroll_array_insert_oopsort_elem_mem(result,
		                                     res,
		                                     array,
		                                     parms->size,
		                                     parms->compare,
		                                     parms->data);
}

static __stroll_nonull(1, 2, 3, 4)
void
stroll_array_merge_topdwn_sort(
	const struct stroll_array_merge * __restrict parms,
	char * __restrict                            result,
	char * __restrict                            array,
	const char * __restrict                      end)
{
	stroll_array_merge_assert(parms);
	stroll_array_assert_intern(parms->size);
	stroll_array_assert_intern(result);
	stroll_array_assert_intern(end > array);
	stroll_array_assert_intern(!((size_t)(end - array) % parms->size));

	size_t sz = (size_t)(end - array);
	size_t sz0;

	if (sz <= parms->thres) {
		stroll_array_assert_intern(sz >= parms->size);
		stroll_array_insert_oopsort_mem(parms, result, array, end);
		return;
	}

	sz0 = (sz / (2 * parms->size)) * parms->size;

	stroll_array_merge_topdwn_sort(parms,
	                               &result[sz0],
	                               &array[sz0],
	                               end);
	stroll_array_merge_topdwn_sort(parms,
	                               &array[sz - sz0],
	                               array,
	                               &array[sz0]);
	stroll_array_merge_parts_mem(parms,
	                             result,
	                             &array[sz - sz0], end,
	                             &result[sz0], &result[sz]);
}

static __stroll_nonull(1, 4)
int
stroll_array_merge_sort_mem(char * __restrict     array,
                            unsigned int          nr,
                            size_t                size,
                            stroll_array_cmp_fn * compare,
                            void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(nr > 1);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	char *                          aux;
	size_t                          asz = (nr / 2) * size;
	char *                          mid = &array[asz];
	size_t                          msz = (nr * size) - asz;
	const struct stroll_array_merge parms = {
		.size    = size,
		.compare = compare,
		.data    = data,
		.thres   = STROLL_MSORT_INSERT_THRESHOLD * size
	};

	aux = malloc(msz);
	if (!aux)
		return -errno;

	stroll_array_merge_topdwn_sort(&parms, aux, mid, &mid[msz]);
	stroll_array_merge_topdwn_sort(&parms, &array[msz], array, mid);

	stroll_array_merge_parts_mem(&parms,
	                             array,
	                             &array[msz], &mid[msz],
	                             aux, &aux[msz]);

	free(aux);

	return 0;
}

int
stroll_array_merge_sort(void * __restrict     array,
                        unsigned int          nr,
                        size_t                size,
                        stroll_array_cmp_fn * compare,
                        void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (nr == 1)
		return 0;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		return stroll_array_merge_sort32(array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		return stroll_array_merge_sort64(array, nr, compare, data);
	else
		return stroll_array_merge_sort_mem(array,
		                                   nr,
		                                   size,
		                                   compare,
		                                   data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_MERGE_SORT) */

#if defined(CONFIG_STROLL_ARRAY_HEAP_SORT)

static inline
unsigned int
stroll_heap_left_child_index(unsigned int index)
{
	return (2 * index) + 1;
}

static inline
unsigned int
stroll_heap_right_child_index(unsigned int index)
{
	return (2 * index) + 2;
}

static
void
stroll_array_siftdwn_mem(unsigned int          index,
                         char * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	unsigned int idx = index;
	unsigned int lidx = stroll_heap_left_child_index(index);
	unsigned int ridx = stroll_heap_right_child_index(index);

	if ((lidx < nr) &&
	    compare(&array[lidx * size], &array[idx * size], data) > 0)
		idx = lidx;

	if ((ridx < nr) &&
	    compare(&array[ridx * size], &array[idx * size], data) > 0)
		idx = ridx;

	if (idx == index)
		return;

	stroll_array_swap(&array[idx * size], &array[index * size], size);
	stroll_array_siftdwn_mem(idx, array, nr, size, compare, data);
}

static void
stroll_array_heapify_mem(char * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	unsigned int cnt;

	cnt = nr / 2;
	do {
		cnt--;
		stroll_array_siftdwn_mem(cnt, array, nr, size, compare, data);
	} while (cnt);
}

static void
stroll_array_heap_sort_mem(void * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare,
                           void *                data)
{
	unsigned int cnt;

	stroll_array_heapify_mem(array, nr, size, compare, data);

	cnt = nr;
	do {
		cnt--;
		stroll_array_swap(array, &((char *)array)[cnt * size], size);
		stroll_array_siftdwn_mem(0, array, cnt, size, compare, data);
	} while (cnt);
}

void
stroll_array_heap_sort(void * __restrict     array,
                       unsigned int          nr,
                       size_t                size,
                       stroll_array_cmp_fn * compare,
                       void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (nr == 1)
		return;

	return stroll_array_heap_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_HEAP_SORT) */
