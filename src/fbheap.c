/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/fbheap.h"
#include "stroll/pow2.h"
#include <string.h>

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_fbheap_assert_intern(_expr) \
	stroll_assert("stroll:fbheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_fbheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

static inline __stroll_const __stroll_nothrow
unsigned int
stroll_fbheap_index_depth(unsigned int index)
{
	return stroll_pow2_low(index + 1);
}

static inline __const __nothrow
unsigned int
stroll_fbheap_elder_index(unsigned int index, unsigned int offset)
{
	stroll_fbheap_assert_intern(offset <= stroll_fbheap_index_depth(index));

	return (index - (1U << offset) + 1) >> offset;
}

static inline __stroll_const __stroll_nothrow
unsigned int
stroll_fbheap_parent_index(unsigned int index)
{
	stroll_fbheap_assert_intern(index);

	return (index - 1) / 2;
}

static inline __const __nothrow
unsigned int
stroll_fbheap_left_child_index(unsigned int index)
{
	return (2 * index) + 1;
}

static inline __const __nothrow
unsigned int
stroll_fbheap_right_child_index(unsigned int index)
{
	return (2 * index) + 2;
}

#define STROLL_FBHEAP_SIFTUP(_func, _type) \
	static inline __stroll_nonull(4) __stroll_nothrow \
	void \
	_func(unsigned int       root, \
	      unsigned int       node, \
	      _type              elem, \
	      _type * __restrict array) \
	{ \
		stroll_fbheap_assert_intern(node >= root); \
		stroll_fbheap_assert_intern(array); \
		\
		unsigned int depth; \
		\
		depth = stroll_fbheap_index_depth(node) - \
		        stroll_fbheap_index_depth(root); \
		while (depth--) { \
			root = stroll_fbheap_elder_index(node, depth); \
			\
			array[stroll_fbheap_parent_index(root)] = array[root]; \
		} \
		\
		array[node] = elem; \
	}

STROLL_FBHEAP_SIFTUP(stroll_fbheap_siftup32, uint32_t)

STROLL_FBHEAP_SIFTUP(stroll_fbheap_siftup64, uint64_t)

static inline __stroll_nonull(3, 4) __stroll_nothrow
void
stroll_fbheap_siftup_mem(unsigned int            root,
                         unsigned int            node,
                         const char * __restrict elem,
                         char * __restrict       array,
                         size_t                  size)
{
	stroll_fbheap_assert_intern(node >= root);
	stroll_fbheap_assert_intern(elem);
	stroll_fbheap_assert_intern(array);
	stroll_fbheap_assert_intern(size);

	unsigned int depth;

	depth = stroll_fbheap_index_depth(node) -
	        stroll_fbheap_index_depth(root);
	while (depth--) {
		root = stroll_fbheap_elder_index(node, depth);

		memcpy(&array[stroll_fbheap_parent_index(root) * size],
		       &array[root * size],
		       size);
	}

	memcpy(&array[node * size], elem, size);
}

#if defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT)

#include "array.h"

#define STROLL_FBHEAP_FIND_MAX(_func, _type) \
	static inline __stroll_nonull(2, 3, 5) \
	unsigned int \
	_func(unsigned int          index, \
	      _type * __restrict    elem, \
	      _type * __restrict    array, \
	      unsigned int          nr, \
	      stroll_array_cmp_fn * compare, \
	      void *                data) \
	{ \
		stroll_fbheap_assert_intern(elem); \
		stroll_fbheap_assert_intern(array); \
		stroll_fbheap_assert_intern(nr); \
		stroll_fbheap_assert_intern(index < nr); \
		stroll_fbheap_assert_intern(compare); \
		\
		unsigned int left = stroll_fbheap_left_child_index(index); \
		unsigned int right = stroll_fbheap_right_child_index(index); \
		\
		while (right < nr) { \
			if (compare(&array[left], &array[right], data) >= 0) \
				index = left; \
			else \
				index = right; \
			\
			left = stroll_fbheap_left_child_index(index); \
			right = stroll_fbheap_right_child_index(index); \
		} \
		if (left < nr) \
			index = left; \
		\
		while (compare(elem, &array[index], data) > 0) { \
			stroll_fbheap_assert_intern(index); \
			\
			index = stroll_fbheap_parent_index(index); \
		} \
		\
		return index; \
	}

STROLL_FBHEAP_FIND_MAX(stroll_fbheap_find_max32, uint32_t)

STROLL_FBHEAP_FIND_MAX(stroll_fbheap_find_max64, uint64_t)

static inline __stroll_nonull(2, 3, 6)
unsigned int
stroll_fbheap_find_max_mem(unsigned int          index,
                           char * __restrict     elem,
                           char * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare,
                           void *                data)
{
	stroll_fbheap_assert_intern(elem);
	stroll_fbheap_assert_intern(array);
	stroll_fbheap_assert_intern(nr);
	stroll_fbheap_assert_intern(index < nr);
	stroll_fbheap_assert_intern(size);
	stroll_fbheap_assert_intern(compare);

	unsigned int left = stroll_fbheap_left_child_index(index);
	unsigned int right = stroll_fbheap_right_child_index(index);

	while (right < nr) {
		if (compare(&array[left * size],
		            &array[right * size],
		            data) >= 0)
			index = left;
		else
			index = right;

		left = stroll_fbheap_left_child_index(index);
		right = stroll_fbheap_right_child_index(index);
	}
	if (left < nr)
		index = left;

	while (compare(elem, &array[index * size], data) > 0) {
		stroll_fbheap_assert_intern(index);

		index = stroll_fbheap_parent_index(index);
	}

	return index;
}

#define STROLL_FBHEAP_SORT(_func, _find_max, _siftup, _type) \
	static __stroll_nonull(1, 3) \
	void \
	_func(_type * __restrict    array, \
	      unsigned int          nr, \
	      stroll_array_cmp_fn * compare, \
	      void *                data) \
	{ \
		stroll_fbheap_assert_intern(array); \
		stroll_fbheap_assert_intern(nr > 1); \
		stroll_fbheap_assert_intern(compare); \
		\
		unsigned int idx; \
		unsigned int leaf; \
		\
		idx = nr / 2; \
		do { \
			idx--; \
			leaf = _find_max(idx, \
			                 &array[idx], \
			                 array, \
			                 nr, \
			                 compare, \
			                 data); \
			if (leaf != idx) \
				_siftup(idx, leaf, array[idx], array); \
		} while (idx); \
		\
		idx = nr; \
		do { \
			_type tmp = array[--idx]; \
			\
			array[idx] = array[0]; \
			leaf = _find_max(0, &tmp, array, idx, compare, data); \
			_siftup(0, leaf, tmp, array); \
		} while (idx != 1); \
	}

STROLL_FBHEAP_SORT(stroll_fbheap_sort32,
                   stroll_fbheap_find_max32,
                   stroll_fbheap_siftup32,
                   uint32_t)

STROLL_FBHEAP_SORT(stroll_fbheap_sort64,
                   stroll_fbheap_find_max64,
                   stroll_fbheap_siftup64,
                   uint64_t)

static __stroll_nonull(1, 4)
void
stroll_fbheap_sort_mem(char * __restrict     array,
                       unsigned int          nr,
                       size_t                size,
                       stroll_array_cmp_fn * compare,
                       void *                data)
{
	stroll_fbheap_assert_intern(array);
	stroll_fbheap_assert_intern(nr > 1);
	stroll_fbheap_assert_intern(size);
	stroll_fbheap_assert_intern(compare);

	unsigned int idx;
	unsigned int leaf;
	char         tmp[size];

	idx = nr / 2;
	do {
		idx--;
		leaf = stroll_fbheap_find_max_mem(idx,
		                                  &array[idx * size],
		                                  array,
		                                  nr,
		                                  size,
		                                  compare,
		                                  data);
		if (leaf != idx) {
			memcpy(tmp, &array[idx * size], size);
			stroll_fbheap_siftup_mem(idx, leaf, tmp, array, size);
		}
	} while (idx);

	idx = nr;
	do {
		idx--;
		memcpy(tmp, &array[idx * size], size);
		memcpy(&array[idx * size], array, size);
		leaf = stroll_fbheap_find_max_mem(0,
		                                  tmp,
		                                  array,
		                                  idx,
		                                  size,
		                                  compare,
		                                  data);
		stroll_fbheap_siftup_mem(0, leaf, tmp, array, size);
	} while (idx != 1);
}

void
stroll_array_fbheap_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	stroll_fbheap_assert_api(array);
	stroll_fbheap_assert_api(nr);
	stroll_fbheap_assert_api(size);
	stroll_fbheap_assert_api(compare);

	if (nr == 1)
		return;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		return stroll_fbheap_sort32(array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		return stroll_fbheap_sort64(array, nr, compare, data);
	else
		return stroll_fbheap_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT) */

#if defined(CONFIG_STROLL_FBHEAP)

#define STROLL_FBHEAP_INSERT(_func, _type) \
	static __stroll_nonull(1, 2, 4) \
	void \
	_func(const _type * __restrict elem, \
	      _type * __restrict       array, \
	      unsigned int             nr, \
	      stroll_array_cmp_fn *    compare, \
	      void *                   data) \
	{ \
		stroll_fbheap_assert_intern(elem); \
		stroll_fbheap_assert_intern(array); \
		stroll_fbheap_assert_intern((elem < array) || \
		                            (elem >= &array[nr])); \
		stroll_fbheap_assert_intern(compare); \
		\
		while (nr) { \
			unsigned int parent = stroll_fbheap_parent_index(nr); \
			\
			if (compare(elem, &array[parent], data) >= 0) \
				break; \
			\
			array[nr] = array[parent]; \
			nr = parent; \
		} \
		\
		array[nr] = *elem; \
	}

STROLL_FBHEAP_INSERT(stroll_fbheap_insert32, uint32_t)

STROLL_FBHEAP_INSERT(stroll_fbheap_insert64, uint64_t)

static __stroll_nonull(1, 2, 5)
void
stroll_fbheap_insert_mem(const char * __restrict elem,
                         char * __restrict       array,
                         unsigned int            nr,
                         size_t                  size,
                         stroll_array_cmp_fn *   compare,
                         void *                  data)
{
	stroll_fbheap_assert_intern(elem);
	stroll_fbheap_assert_intern(array);
	stroll_fbheap_assert_intern(size);
	stroll_fbheap_assert_intern((elem < array) ||
	                            (elem >= &array[nr * size]));
	stroll_fbheap_assert_intern(compare);

	while (nr) {
		unsigned int parent = stroll_fbheap_parent_index(nr);

		if (compare(elem, &array[parent * size], data) >= 0)
			break;

		memcpy(&array[nr * size], &array[parent * size], size);
		nr = parent;
	}

	memcpy(&array[nr * size], elem, size);
}

void
_stroll_fbheap_insert(const void * __restrict elem,
                      void * __restrict       array,
                      unsigned int            nr,
                      size_t                  size,
                      stroll_array_cmp_fn *   compare,
                      void *                  data)
{
	stroll_fbheap_assert_api(elem);
	stroll_fbheap_assert_api(array);
	stroll_fbheap_assert_api(size);
	stroll_fbheap_assert_api((elem < array) ||
	                         ((const char *)elem >=
	                          &((char *)array)[nr * size]));
	stroll_fbheap_assert_api(compare);

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		return stroll_fbheap_insert32(elem, array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		return stroll_fbheap_insert64(elem, array, nr, compare, data);
	else
		stroll_fbheap_insert_mem(elem, array, nr, size, compare, data);
}

#define STROLL_FBHEAP_FIND_MIN(_func, _type) \
	static inline __stroll_nonull(2, 3, 5) \
	unsigned int \
	_func(unsigned int          index, \
	      _type * __restrict    elem, \
	      _type * __restrict    array, \
	      unsigned int          nr, \
	      stroll_array_cmp_fn * compare, \
	      void *                data) \
	{ \
		stroll_fbheap_assert_intern(elem); \
		stroll_fbheap_assert_intern(array); \
		stroll_fbheap_assert_intern(nr); \
		stroll_fbheap_assert_intern(index < nr); \
		stroll_fbheap_assert_intern(compare); \
		\
		unsigned int left = stroll_fbheap_left_child_index(index); \
		unsigned int right = stroll_fbheap_right_child_index(index); \
		\
		while (right < nr) { \
			if (compare(&array[left], &array[right], data) <= 0) \
				index = left; \
			else \
				index = right; \
			\
			left = stroll_fbheap_left_child_index(index); \
			right = stroll_fbheap_right_child_index(index); \
		} \
		if (left < nr) \
			index = left; \
		\
		while (compare(elem, &array[index], data) < 0) { \
			stroll_fbheap_assert_api(index); \
			\
			index = stroll_fbheap_parent_index(index); \
		} \
		\
		return index; \
	}

STROLL_FBHEAP_FIND_MIN(stroll_fbheap_find_min32, uint32_t)

STROLL_FBHEAP_FIND_MIN(stroll_fbheap_find_min64, uint64_t)

static inline __stroll_nonull(2, 3, 6)
unsigned int
stroll_fbheap_find_min_mem(unsigned int          index,
                           char * __restrict     elem,
                           char * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare,
                           void *                data)
{
	stroll_fbheap_assert_intern(elem);
	stroll_fbheap_assert_intern(array);
	stroll_fbheap_assert_intern(nr);
	stroll_fbheap_assert_intern(index < nr);
	stroll_fbheap_assert_intern(size);
	stroll_fbheap_assert_intern(compare);

	unsigned int left = stroll_fbheap_left_child_index(index);
	unsigned int right = stroll_fbheap_right_child_index(index);

	while (right < nr) {
		if (compare(&array[left * size],
		            &array[right * size],
		            data) <= 0)
			index = left;
		else
			index = right;

		left = stroll_fbheap_left_child_index(index);
		right = stroll_fbheap_right_child_index(index);
	}
	if (left < nr)
		index = left;

	while (compare(elem, &array[index * size], data) < 0) {
		stroll_fbheap_assert_api(index);

		index = stroll_fbheap_parent_index(index);
	}

	return index;
}

#define STROLL_FBHEAP_EXTRACT(_func, _find_min, _siftup, _type) \
	static __stroll_nonull(1, 2, 4) \
	void \
	_func(_type * __restrict    elem, \
	      _type * __restrict    array, \
	      unsigned int          nr, \
	      stroll_array_cmp_fn * compare, \
	      void *                data) \
	{ \
		stroll_fbheap_assert_intern(elem); \
		stroll_fbheap_assert_intern(array); \
		stroll_fbheap_assert_intern(nr); \
		stroll_fbheap_assert_intern((elem < array) || \
		                            (elem >= &array[nr])); \
		stroll_fbheap_assert_intern(compare); \
		\
		*elem = array[0]; \
		\
		if (nr != 1) { \
			unsigned int leaf; \
			\
			nr--; \
			leaf = _find_min(0, \
			                 &array[nr], \
			                 array, \
			                 nr, \
			                 compare, \
			                 data); \
			_siftup(0, leaf, array[nr], array); \
		} \
	}

STROLL_FBHEAP_EXTRACT(stroll_fbheap_extract32,
                      stroll_fbheap_find_min32,
                      stroll_fbheap_siftup32,
                      uint32_t)

STROLL_FBHEAP_EXTRACT(stroll_fbheap_extract64,
                      stroll_fbheap_find_min64,
                      stroll_fbheap_siftup64,
                      uint64_t)

static __stroll_nonull(1, 2, 5)
void
stroll_fbheap_extract_mem(char * __restrict     elem,
                          char * __restrict     array,
                          unsigned int          nr,
                          size_t                size,
                          stroll_array_cmp_fn * compare,
                          void *                data)
{
	stroll_fbheap_assert_intern(elem);
	stroll_fbheap_assert_intern(array);
	stroll_fbheap_assert_intern(nr);
	stroll_fbheap_assert_intern(size);
	stroll_fbheap_assert_intern((elem < array) ||
	                            (elem >= &array[nr * size]));
	stroll_fbheap_assert_intern(compare);

	memcpy(elem, array, size);

	if (nr != 1) {
		unsigned int leaf;

		nr--;
		leaf = stroll_fbheap_find_min_mem(0,
		                                  &array[nr * size],
		                                  array,
		                                  nr,
		                                  size,
		                                  compare,
		                                  data);
		stroll_fbheap_siftup_mem(0,
		                         leaf,
		                         &array[nr * size],
		                         array,
		                         size);
	}
}

void
_stroll_fbheap_extract(void * __restrict     elem,
                       void * __restrict     array,
                       unsigned int          nr,
                       size_t                size,
                       stroll_array_cmp_fn * compare,
                       void *                data)
{
	stroll_fbheap_assert_api(elem);
	stroll_fbheap_assert_api(array);
	stroll_fbheap_assert_api(nr);
	stroll_fbheap_assert_api(size);
	stroll_fbheap_assert_api((elem < array) ||
	                         ((char *)elem >= &((char *)array)[nr * size]));
	stroll_fbheap_assert_api(compare);

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		return stroll_fbheap_extract32(elem, array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		return stroll_fbheap_extract64(elem, array, nr, compare, data);
	else
		stroll_fbheap_extract_mem(elem, array, nr, size, compare, data);
}

#define STROLL_FBHEAP_BUILD(_func, _find_min, _siftup, _type) \
	static __stroll_nonull(1, 3) \
	void \
	_func(_type * __restrict    array, \
	      unsigned int          nr, \
	      stroll_array_cmp_fn * compare, \
	      void *                data) \
	{ \
		stroll_fbheap_assert_intern(array); \
		stroll_fbheap_assert_intern(nr > 1); \
		stroll_fbheap_assert_intern(compare); \
		\
		unsigned int idx = nr / 2; \
		\
		do { \
			unsigned int leaf; \
			\
			idx--; \
			leaf = _find_min(idx, \
			                 &array[idx], \
			                 array, \
			                 nr, \
			                 compare, \
			                 data); \
			if (leaf != idx) \
				_siftup(idx, leaf, array[idx], array); \
		} while (idx); \
	}

STROLL_FBHEAP_BUILD(stroll_fbheap_build32,
                    stroll_fbheap_find_min32,
                    stroll_fbheap_siftup32,
                    uint32_t)

STROLL_FBHEAP_BUILD(stroll_fbheap_build64,
                    stroll_fbheap_find_min64,
                    stroll_fbheap_siftup64,
                    uint64_t)

static __stroll_nonull(1, 4)
void
stroll_fbheap_build_mem(char * __restrict     array,
                        unsigned int          nr,
                        size_t                size,
                        stroll_array_cmp_fn * compare,
                        void *                data)
{
	stroll_fbheap_assert_intern(array);
	stroll_fbheap_assert_intern(nr > 1);
	stroll_fbheap_assert_intern(size);
	stroll_fbheap_assert_intern(compare);

	unsigned int idx = nr / 2;

	do {
		unsigned int leaf;

		idx--;
		leaf = stroll_fbheap_find_min_mem(idx,
		                                  &array[idx * size],
		                                  array,
		                                  nr,
		                                  size,
		                                  compare,
		                                  data);
		if (leaf != idx) {
			char tmp[size];

			memcpy(tmp, &array[idx * size], size);
			stroll_fbheap_siftup_mem(idx, leaf, tmp, array, size);
		}
	} while (idx);
}

void
_stroll_fbheap_build(void * __restrict       array,
                     unsigned int            nr,
                     size_t                  size,
                     stroll_array_cmp_fn *   compare,
                     void *                  data)
{
	stroll_fbheap_assert_api(array);
	stroll_fbheap_assert_api(nr);
	stroll_fbheap_assert_api(size);
	stroll_fbheap_assert_api(compare);

	if (nr == 1)
		return;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		return stroll_fbheap_build32(array, nr, compare, data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		return stroll_fbheap_build64(array, nr, compare, data);
	else
		stroll_fbheap_build_mem(array, nr, size, compare, data);
}

void
stroll_fbheap_insert(struct stroll_fbheap * __restrict heap,
                     const void * __restrict           elem,
                     void * __restrict                 data)
{
	stroll_fbheap_assert_heap_api(heap);
	stroll_fbheap_assert_api(heap->cnt < heap->nr);

	_stroll_fbheap_insert(elem,
	                      heap->elems,
	                      heap->cnt++,
	                      heap->size,
	                      heap->compare,
	                      data);
}

void
stroll_fbheap_extract(struct stroll_fbheap * __restrict heap,
                      void * __restrict                 elem,
                      void * __restrict                 data)
{
	stroll_fbheap_assert_heap_api(heap);
	stroll_fbheap_assert_api(heap->cnt);

	_stroll_fbheap_extract(elem,
	                       heap->elems,
	                       heap->cnt--,
	                       heap->size,
	                       heap->compare,
	                       data);
}

void
stroll_fbheap_build(struct stroll_fbheap * __restrict heap,
                    unsigned int                      count,
                    void * __restrict                 data)
{
	stroll_fbheap_assert_heap_api(heap);
	stroll_fbheap_assert_api(count);
	stroll_fbheap_assert_api(count <= heap->nr);

	_stroll_fbheap_build(heap->elems,
	                     count,
	                     heap->size,
	                     heap->compare,
	                     data);
	heap->cnt = count;
}

void
stroll_fbheap_setup(struct stroll_fbheap * __restrict heap,
                    void * __restrict                 array,
                    unsigned int                      nr,
                    size_t                            size,
                    stroll_array_cmp_fn *             compare)
{
	stroll_fbheap_assert_api(heap);
	stroll_fbheap_assert_api(array);
	stroll_fbheap_assert_api(nr);
	stroll_fbheap_assert_api(size);
	stroll_fbheap_assert_api(compare);

	heap->cnt = 0;
	heap->nr = nr;
	heap->size = size;
	heap->compare = compare;
	heap->elems = array;
}

struct stroll_fbheap *
stroll_fbheap_create(void * __restrict     array,
                     unsigned int          nr,
                     size_t                size,
                     stroll_array_cmp_fn * compare)
{
	stroll_fbheap_assert_api(array);
	stroll_fbheap_assert_api(nr);
	stroll_fbheap_assert_api(size);
	stroll_fbheap_assert_api(compare);

	struct stroll_fbheap * heap;

	heap = malloc(sizeof(*heap));
	if (!heap)
		return NULL;

	stroll_fbheap_setup(heap, array, nr, size, compare);

	return heap;
}

#endif /* defined(CONFIG_STROLL_FBHEAP) */
