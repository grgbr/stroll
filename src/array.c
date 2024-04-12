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
		stroll_array_bubble_sort32((uint32_t *)array,
		                           nr,
		                           compare,
		                           data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_bubble_sort64((uint64_t *)array,
		                           nr,
		                           compare,
		                           data);
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
		stroll_array_select_sort32((uint32_t *)array,
		                           nr,
		                           compare,
		                           data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_select_sort64((uint64_t *)array,
		                           nr,
		                           compare,
		                           data);
	else
		stroll_array_select_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)

#define STROLL_ARRAY_DEFINE_INSERT_PRESORT(_presort_func, _swap_func, _type) \
	static __stroll_nonull(1, 2, 3) \
	void \
	_presort_func(_type * __restrict    array, \
	              _type * __restrict    unsort, \
	              stroll_array_cmp_fn * compare, \
	              void *                data) \
	{ \
		stroll_array_assert_intern(array); \
		stroll_array_assert_intern(unsort > array); \
		stroll_array_assert_intern(compare); \
		\
		_type * elm = &unsort[-1]; \
		\
		if (compare(unsort, elm, data) < 0) { \
			_type tmp = unsort[0]; \
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

STROLL_ARRAY_DEFINE_INSERT_PRESORT(stroll_array_insert_presort32,
                                   stroll_array_swap32,
                                   uint32_t)

STROLL_ARRAY_DEFINE_INSERT_PRESORT(stroll_array_insert_presort64,
                                   stroll_array_swap64,
                                   uint64_t)

static __stroll_nonull(1, 2, 4)
void
stroll_array_insert_presort_mem(void * __restrict     array,
                                void * __restrict     unsort,
                                size_t                size,
                                stroll_array_cmp_fn * compare,
                                void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(unsort > array);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	char * elm = unsort - size;

	if (compare(unsort, elm, data) < 0) {
		char tmp[size];

		memcpy(tmp, unsort, size);

		do {
			memcpy(elm + size, elm, size);
			elm -= size;
		} while ((elm >= (char *)array) &&
		         (compare(tmp, elm, data) < 0));

		memcpy(elm + size, tmp, size);
	}
}

void
stroll_array_insert_presort(void * __restrict     array,
                            void * __restrict     unsort,
                            size_t                size,
                            stroll_array_cmp_fn * compare,
                            void *                data)
{
	stroll_array_assert_api(array);
	stroll_array_assert_api(unsort > array);
	stroll_array_assert_api(size);
	stroll_array_assert_api(compare);

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_array_insert_presort32((uint32_t *)array,
		                              (uint32_t *)unsort,
		                              compare,
		                              data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_insert_presort64((uint64_t *)array,
		                              (uint64_t *)unsort,
		                              compare,
		                              data);
	else
		stroll_array_insert_presort_mem(array,
		                                unsort,
		                                size,
		                                compare,
		                                data);
}

#define STROLL_ARRAY_DEFINE_INSERT_SORT(_sort_func, _presort_func, _type) \
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
			_presort_func(array, unsort, compare, data); \
			unsort++; \
		} \
	}

STROLL_ARRAY_DEFINE_INSERT_SORT(stroll_array_insert_sort32,
                                stroll_array_insert_presort32,
                                uint32_t)

STROLL_ARRAY_DEFINE_INSERT_SORT(stroll_array_insert_sort64,
                                stroll_array_insert_presort64,
                                uint64_t)

static __stroll_nonull(1, 4)
void
stroll_array_insert_sort_mem(void * __restrict     array,
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
	const char * last = (const char *)array + ((nr - 1) * size);

	while (unsort <= last) {
		stroll_array_insert_presort_mem(array,
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
		stroll_array_insert_sort32((uint32_t *)array,
		                           nr,
		                           compare,
		                           data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_insert_sort64((uint64_t *)array,
		                           nr,
		                           compare,
		                           data);
	else
		stroll_array_insert_sort_mem(array, nr, size, compare, data);

}

#endif /* defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT_UTILS)

#define STROLL_ARRAY_DEFINE_QUICK_HOARE_PART(_part_func, _swap_func, _type) \
	static __stroll_nonull(1, 2, 3) \
	_type * \
	_part_func(_type *               begin, \
	           _type *               end, \
	           stroll_array_cmp_fn * compare, \
	           void *                data) \
	{ \
		stroll_array_assert_intern(begin); \
		stroll_array_assert_intern(end > begin); \
		stroll_array_assert_intern(compare); \
		\
		_type   pivot; \
		_type * mid = &begin[(end - begin) / 2]; \
		\
		if (compare(begin, mid, data) > 0) \
			_swap_func(begin, mid); \
		\
		if (compare(mid, end, data) > 0) { \
			_swap_func(mid, end); \
			\
			if (compare(begin, mid, data) > 0) \
				_swap_func(begin, mid); \
		} \
		\
		pivot = *mid; \
		\
		begin--; \
		end++; \
		while (true) { \
			do { \
				begin++; \
			} while (compare(&pivot, begin, data) > 0); \
			\
			do { \
				end--; \
			} while (compare(end, &pivot, data) > 0); \
			\
			if (begin >= end) \
				return end; \
			\
			_swap_func(begin, end); \
		} \
	}

STROLL_ARRAY_DEFINE_QUICK_HOARE_PART(stroll_array_quick_hoare_part32,
                                     stroll_array_swap32,
                                     uint32_t)

STROLL_ARRAY_DEFINE_QUICK_HOARE_PART(stroll_array_quick_hoare_part64,
                                     stroll_array_swap64,
                                     uint64_t)

/*
 * TODO:
 *  - docs !! see https://stackoverflow.com/questions/6709055/quicksort-stack-size
 *  https://stackoverflow.com/questions/1582356/fastest-way-of-finding-the-middle-value-of-a-triple
 *  https://stackoverflow.com/questions/7559608/median-of-three-values-strategy
 *
 * Pivot is choosen according to the median-of-three strategy, e.g.:
 *     mid = (begin + end) / 2.
 * However, to prevent from addition overflow, compute it as following:
 *     mid = begin + ((end - begin) / 2)
 */
static __stroll_nonull(1, 2, 4)
char *
stroll_array_quick_hoare_part_mem(char *                begin,
                                  char *                end,
                                  size_t                size,
                                  stroll_array_cmp_fn * compare,
                                  void *                data)
{
	stroll_array_assert_intern(begin);
	stroll_array_assert_intern(end > begin);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

	char   pivot[size];
	char * mid = begin + (((size_t)(end - begin) / (2 * size)) * size);

	if (compare(begin, mid, data) > 0)
		stroll_array_swap(begin, mid, size);

	if (compare(mid, end, data) > 0) {
		stroll_array_swap(mid, end, size);

		if (compare(begin, mid, data) > 0)
			stroll_array_swap(begin, mid, size);
	}

	memcpy(pivot, mid, size);

	begin -= size;
	end += size;
	while (true) {
		do {
			begin += size;
		} while (compare(pivot, begin, data) > 0);

		do {
			end -= size;
		} while (compare(end, pivot, data) > 0);

		if (begin >= end)
			return end;

		stroll_array_swap(begin, end, size);
	}
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

#define STROLL_ARRAY_DEFINE_QUICK_THRES(_func, _type) \
	static __stroll_nonull(1, 2) __stroll_const __stroll_nothrow \
	bool \
	_func(const _type * __restrict begin, const _type * __restrict end) \
	{ \
		stroll_array_assert_intern(begin); \
		stroll_array_assert_intern(end >= begin); \
		\
		return (end - begin) <= (STROLL_QSORT_INSERT_THRESHOLD - 1); \
	}

STROLL_ARRAY_DEFINE_QUICK_THRES(stroll_array_quick_switch_insert32, uint32_t)

STROLL_ARRAY_DEFINE_QUICK_THRES(stroll_array_quick_switch_insert64, uint64_t)

static __stroll_nonull(1, 2) __stroll_const __stroll_nothrow
bool
stroll_array_quick_switch_insert_mem(const char * __restrict begin,
                                     const char * __restrict end,
                                     size_t                  size)
{
	stroll_array_assert_intern(begin);
	stroll_array_assert_intern(end >= begin);
	stroll_array_assert_intern(size);

	return ((size_t)(end - begin) <=
	        ((STROLL_QSORT_INSERT_THRESHOLD - 1) * size));
}

#define STROLL_ARRAY_DEFINE_QUICK_SORT(_sort_func, \
                                       _part_func, \
                                       _thres_func, \
                                       _insert_sort, \
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
		_type *                begin = array; \
		_type *                end = &begin[nr - 1]; \
		unsigned int           ptop = 0; \
		struct { \
			_type * begin; \
			_type * end; \
		}                      parts[stroll_array_stack_depth(nr)]; \
		\
		while (true) { \
			_type * pivot; \
			_type * high; \
			\
			while (_thres_func(begin, end)) { \
				if (!ptop--) \
					return _insert_sort(array, \
					                    nr, \
					                    compare, \
					                    data); \
				begin = parts[ptop].begin; \
				end = parts[ptop].end; \
			} \
			\
			pivot = _part_func(begin, end, compare, data); \
			\
			stroll_array_assert_intern(begin <= pivot); \
			stroll_array_assert_intern(pivot < end); \
			stroll_array_assert_intern(ptop < \
			                           stroll_array_nr(parts)); \
			\
			high = &pivot[1]; \
			if ((high - begin) >= (end - pivot)) { \
				parts[ptop].begin = begin; \
				parts[ptop].end = pivot; \
				begin = high; \
			} \
			else { \
				parts[ptop].begin = high; \
				parts[ptop].end = end; \
				end = pivot; \
			} \
			\
			ptop++; \
		} \
	}

STROLL_ARRAY_DEFINE_QUICK_SORT(stroll_array_quick_sort32,
                               stroll_array_quick_hoare_part32,
                               stroll_array_quick_switch_insert32,
                               stroll_array_insert_sort32,
                               uint32_t)

STROLL_ARRAY_DEFINE_QUICK_SORT(stroll_array_quick_sort64,
                               stroll_array_quick_hoare_part64,
                               stroll_array_quick_switch_insert64,
                               stroll_array_insert_sort64,
                               uint64_t)

static __stroll_nonull(1, 4)
void
stroll_array_quick_sort_mem(void * __restrict     array,
                            unsigned int          nr,
                            size_t                size,
                            stroll_array_cmp_fn * compare,
                            void *                data)
{
	stroll_array_assert_intern(array);
	stroll_array_assert_intern(nr > 1);
	stroll_array_assert_intern(size);
	stroll_array_assert_intern(compare);

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

		while (stroll_array_quick_switch_insert_mem(begin, end, size)) {
			if (!ptop--)
				return stroll_array_insert_sort_mem(array,
				                                    nr,
				                                    size,
				                                    compare,
				                                    data);
			begin = parts[ptop].begin;
			end = parts[ptop].end;
		}

		pivot = stroll_array_quick_hoare_part_mem(begin,
		                                          end,
		                                          size,
		                                          compare,
		                                          data);

		stroll_array_assert_intern(begin <= pivot);
		stroll_array_assert_intern(pivot < end);
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
		stroll_array_quick_sort32((uint32_t *)array,
		                          nr,
		                          compare,
		                          data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_array_quick_sort64((uint64_t *)array,
		                          nr,
		                          compare,
		                          data);
	else
		stroll_array_quick_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */
