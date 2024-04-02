/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/array.h"
#include "stroll/pow2.h"
#include <stdbool.h>
#include <string.h>

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

static __stroll_nonull(1, 2) __stroll_const
void
stroll_array_swap(void * __restrict first,
                  void * __restrict second,
                  size_t            size)
{
	stroll_array_assert_api(first);
	stroll_array_assert_api(second);
	stroll_array_assert_api(size);

	char tmp[size];

	memcpy(tmp, first, size);
	memcpy(first, second, size);
	memcpy(second, tmp, size);
}

#if defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH)

void *
stroll_array_bisect_search(const void *          key,
                           const void *          array,
                           size_t                size,
                           unsigned int          nr,
                           stroll_array_cmp_fn * compare)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(size);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(compare);

	size_t min = 0;
	size_t max = nr;

	do {
		size_t       mid = (min + max) / 2;
		const char * elm = (const char *)array + (mid * size);
		int          cmp;

		cmp = compare(key, elm);
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

void
stroll_array_bubble_sort(void * __restrict     array,
                         size_t                size,
                         unsigned int          nr,
                         stroll_array_cmp_fn * compare)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(size);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(compare);

	const char * end = (const char *)array + ((nr - 1) * size);

	while (end > (const char *)array) {
		char *       elm = array;
		const char * last = array;

		do {
			char * neigh = &elm[size];

			if (compare(elm, neigh) > 0) {
				stroll_array_swap(elm, neigh, size);
				last = neigh;
			}

			elm = neigh;
		} while (elm < end);

		end = last;
	}
}

#endif /* defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT) */

#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)

void
stroll_array_select_sort(void * __restrict     array,
                         size_t                size,
                         unsigned int          nr,
                         stroll_array_cmp_fn * compare)
{
	const char * end = (const char *)array + ((nr - 1) * size);
	char *       unsort = array;

	while (unsort < end) {
		char * elm = unsort;
		char * min = unsort;

		do {
			elm += size;
			if (compare(elm, min) < 0)
				min = elm;
		} while (elm < end);

		stroll_array_swap(unsort, min, size);

		unsort += size;
	}
}

#endif /* defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)

void
stroll_array_insert_presort(void * __restrict     array,
                            void * __restrict     unsort,
                            size_t                size,
                            stroll_array_cmp_fn * compare)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(unsort > array);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	char * elm = unsort - size;

	if (compare(unsort, elm) < 0) {
		char tmp[size];

		memcpy(tmp, unsort, size);

		do {
			memcpy(elm + size, elm, size);
			elm -= size;
		} while ((elm >= (char *)array) && (compare(tmp, elm) < 0));

		memcpy(elm + size, tmp, size);
	}
}

static __stroll_nonull(1, 2, 4)
void
_stroll_array_insert_sort(char * __restrict     array,
                          char * __restrict     last,
                          size_t                size,
                          stroll_array_cmp_fn * compare)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(last >= array);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	char * unsort = array + size;

	while (unsort <= last) {
		stroll_array_insert_presort(array, unsort, size, compare);
		unsort += size;
	}
}

void
stroll_array_insert_sort(void * __restrict     array,
                         size_t                size,
                         unsigned int          nr,
                         stroll_array_cmp_fn * compare)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(size);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(compare);

	_stroll_array_insert_sort(array,
	                          (char *)array + ((nr - 1) * size),
	                          size,
	                          compare);
}

#endif /* defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT_UTILS)

/*
 * TODO:
 *  - docs !! see https://stackoverflow.com/questions/6709055/quicksort-stack-size
 *  https://stackoverflow.com/questions/1582356/fastest-way-of-finding-the-middle-value-of-a-triple
 *  https://stackoverflow.com/questions/7559608/median-of-three-values-strategy
 *
 * Pivot is choosen according to the median-of-three strategy, e.g.:
 *     mid = (begin + end) / 2.
 * However, to prevent from addition overflow compute it as following:
 *     mid = begin + ((end - begin) / 2)
 */
static __stroll_nonull(1, 2, 4)
char *
_stroll_array_quick_hoare_part(char *                begin,
                               char *                end,
                               size_t                size,
                               stroll_array_cmp_fn * compare)
{
	char   pivot[size];
	char * mid = begin + (((size_t)(end - begin) / (2 * size)) * size);

	if (compare(begin, mid) > 0)
		stroll_array_swap(begin, mid, size);

	if (compare(mid, end) > 0) {
		stroll_array_swap(mid, end, size);

		if (compare(begin, mid) > 0)
			stroll_array_swap(begin, mid, size);
	}

	memcpy(pivot, mid, size);

	begin -= size;
	end += size;
	while (true) {
		do {
			begin += size;
		} while (compare(pivot, begin) > 0);

		do {
			end -= size;
		} while (compare(end, pivot) > 0);

		if (begin >= end)
			return end;

		stroll_array_swap(begin, end, size);
	}
}

static __stroll_nonull(1, 2, 4)
char *
stroll_array_quick_hoare_part(char *                begin,
                              char *                end,
                              size_t                size,
                              stroll_array_cmp_fn * compare)
{
	stroll_array_assert_intern(begin);
	stroll_array_assert_intern(end > begin);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	char * pivot;

	pivot = _stroll_array_quick_hoare_part(begin, end, size, compare);

	stroll_array_assert_intern(begin <= pivot);
	stroll_array_assert_intern(pivot < end);

	return pivot;
}

#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT_UTILS) */

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)

#if CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD < 2
#error Invalid Quicksort insertion threshold !
#endif /* CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD < 2 */

#define STROLL_QSORT_INSERT_THRESHOLD \
	STROLL_CONCAT(CONFIG_STROLL_ARRAY_QUICK_SORT_INSERT_THRESHOLD, U)

static inline __stroll_const __stroll_nothrow
unsigned int
stroll_array_stack_depth(unsigned int nr)
{
	stroll_array_assert_intern(nr);

	return stroll_pow2_up(
		stroll_max((nr + STROLL_QSORT_INSERT_THRESHOLD - 1) /
		           STROLL_QSORT_INSERT_THRESHOLD,
		           2U));
}

static __stroll_nonull(1, 2) __stroll_const __stroll_nothrow
bool
stroll_array_quick_switch_insert(const char * __restrict begin,
                                 const char * __restrict end,
                                 size_t                  size)
{
	stroll_array_assert_intern(begin);
	stroll_array_assert_intern(end >= begin);
	stroll_array_assert_intern(size);

	return ((size_t)(end - begin) <=
	        ((STROLL_QSORT_INSERT_THRESHOLD - 1) * size));
}

void
stroll_array_quick_sort(void * __restrict     array,
                        size_t                size,
                        unsigned int          nr,
                        stroll_array_cmp_fn * compare)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(size);
	stroll_array_assert_api(nr);
	stroll_array_assert_api(compare);

	char *                 begin = array;
	char *                 end = &begin[(nr - 1) * size];
	unsigned int           ptop = 0;
	struct {
		char * begin;
		char * end;
	}                      parts[stroll_array_stack_depth(nr)];

	while (true) {
		char * pivot;
		char * high;

		while (stroll_array_quick_switch_insert(begin, end, size)) {
			if (!ptop--)
				return stroll_array_insert_sort(array,
				                                size,
				                                nr,
				                                compare);
			begin = parts[ptop].begin;
			end = parts[ptop].end;
		}

		pivot = stroll_array_quick_hoare_part(begin,
		                                      end,
		                                      size,
		                                      compare);
		stroll_array_assert_intern(ptop < stroll_array_nr(parts));

		high = pivot + size;
		if ((high - begin) >= (end - pivot)) {
			parts[ptop].begin = begin;
			parts[ptop].end = pivot;
			begin = high;
		}
		else {
			parts[ptop].begin = high;
			parts[ptop].end = end;
			end = pivot;
		}

		ptop++;
	}
}

#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */
