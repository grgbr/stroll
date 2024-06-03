/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/array.h"
#include "stroll/pow2.h"
#include <string.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_fheap_assert_api(_expr) \
	stroll_assert("stroll:fheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_fheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_fheap_assert_intern(_expr) \
	stroll_assert("stroll:fheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_fheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

static inline __stroll_const __stroll_nothrow
unsigned int
stroll_fheap_index_depth(unsigned int index)
{
	return stroll_pow2_low(index + 1);
}

static inline __const __nothrow
unsigned int
stroll_fheap_elder_index(unsigned int index, unsigned int offset)
{
	stroll_fheap_assert_intern(offset <= stroll_fheap_index_depth(index));

	return (index - (1U << offset) + 1) >> offset;
}

static inline __stroll_const __stroll_nothrow
unsigned int
stroll_fheap_parent_index(unsigned int index)
{
	stroll_fheap_assert_intern(index);

	return (index - 1) / 2;
}

static inline __const __nothrow
unsigned int
stroll_fheap_left_child_index(unsigned int index)
{
	return (2 * index) + 1;
}

static inline __const __nothrow
unsigned int
stroll_fheap_right_child_index(unsigned int index)
{
	return (2 * index) + 2;
}

#if 0

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

#else

static inline __stroll_nonull(2, 3, 6)
void
stroll_fheap_siftdwn_mem(unsigned int          root,
                         char * __restrict     root_elem,
                         char * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	stroll_fheap_assert_intern(array);
	stroll_fheap_assert_intern(root < nr);
	stroll_fheap_assert_intern(root_elem);
	stroll_fheap_assert_intern(nr >= 1);
	stroll_fheap_assert_intern(size);
	stroll_fheap_assert_intern(compare);

	unsigned int left = stroll_fheap_left_child_index(root);
	unsigned int right = stroll_fheap_right_child_index(root);
	unsigned int leaf = root;

	while (right < nr) {
		if (compare(&array[left * size],
		            &array[right * size],
		            data) >= 0)
			leaf = left;
		else
			leaf = right;

		left = stroll_fheap_left_child_index(leaf);
		right = stroll_fheap_right_child_index(leaf);
	}
	if (left < nr)
		leaf = left;

	while (compare(root_elem, &array[leaf * size], data) > 0)
		leaf = stroll_fheap_parent_index(leaf);

	if (leaf > root) {
		unsigned int depth;

		depth = stroll_fheap_index_depth(leaf) -
		        stroll_fheap_index_depth(root);
		do {
			root = stroll_fheap_elder_index(leaf, --depth);

			memcpy(&array[stroll_fheap_parent_index(root) * size],
			       &array[root * size],
			       size);
		} while (depth);
	}

	memcpy(&array[leaf * size], root_elem, size);
}

#endif

static __stroll_nonull(1, 4)
void
stroll_fheap_build_mem(char * __restrict     array,
                       unsigned int          nr,
                       size_t                size,
                       stroll_array_cmp_fn * compare,
                       void *                data)
{
	stroll_fheap_assert_intern(array);
	stroll_fheap_assert_intern(nr > 1);
	stroll_fheap_assert_intern(size);
	stroll_fheap_assert_intern(compare);

	unsigned int idx = nr / 2;

	do {
		char tmp[size];

		idx--;
		memcpy(tmp, &array[idx * size], size);

		stroll_fheap_siftdwn_mem(idx,
		                         tmp,
		                         array,
		                         nr,
		                         size,
		                         compare,
		                         data);
	} while (idx);
}

#if defined(CONFIG_STROLL_ARRAY_HEAP_SORT)

static __stroll_nonull(1, 4)
void
stroll_fheap_sort_mem(char * __restrict     array,
                      unsigned int          nr,
                      size_t                size,
                      stroll_array_cmp_fn * compare,
                      void *                data)
{
	stroll_fheap_assert_intern(array);
	stroll_fheap_assert_intern(nr > 1);
	stroll_fheap_assert_intern(size);
	stroll_fheap_assert_intern(compare);

	stroll_fheap_build_mem(array, nr, size, compare, data);

	do {
		char tmp[size];

		nr--;
		memcpy(tmp, &array[nr * size], size);
		memcpy(&array[nr * size], array, size);
		stroll_fheap_siftdwn_mem(0,
		                         tmp,
		                         array,
		                         nr,
		                         size,
		                         compare,
		                         data);
	} while (nr > 1);
}

void
stroll_array_heap_sort(void * __restrict     array,
                       unsigned int          nr,
                       size_t                size,
                       stroll_array_cmp_fn * compare,
                       void *                data)
{
	stroll_fheap_assert_api(array);
	stroll_fheap_assert_api(nr);
	stroll_fheap_assert_api(size);
	stroll_fheap_assert_api(compare);

	if (nr == 1)
		return;

	return stroll_fheap_sort_mem(array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_ARRAY_HEAP_SORT) */

#if defined(CONFIG_STROLL_FHEAP)

#include "stroll/heap.h"

static __stroll_nonull(1, 2, 5)
void
stroll_fheap_extract_mem(char * __restrict     elem,
                         char * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	stroll_fheap_assert_intern(elem);
	stroll_fheap_assert_intern(array);
	stroll_fheap_assert_intern(nr);
	stroll_fheap_assert_intern(size);
	stroll_fheap_assert_intern(compare);

	memcpy(elem, array, size);

	if (nr == 1)
		return;

	nr--;
	stroll_fheap_siftdwn_mem(0,
	                         &array[nr * size],
	                         array,
	                         nr,
	                         size,
	                         compare,
	                         data);
}

void
stroll_fheap_extract(void * __restrict     elem,
                     void * __restrict     array,
                     unsigned int          nr,
                     size_t                size,
                     stroll_array_cmp_fn * compare,
                     void *                data)
{
	stroll_fheap_assert_api(elem);
	stroll_fheap_assert_api(array);
	stroll_fheap_assert_api(nr);
	stroll_fheap_assert_api(size);
	stroll_fheap_assert_api(compare);

	stroll_fheap_extract_mem(elem, array, nr, size, compare, data);
}

static __stroll_nonull(1, 2, 5)
void
stroll_fheap_insert_mem(const char * __restrict elem,
                        char * __restrict       array,
                        unsigned int            nr,
                        size_t                  size,
                        stroll_array_cmp_fn *   compare,
                        void *                  data)
{
	stroll_fheap_assert_intern(elem);
	stroll_fheap_assert_intern(array);
	stroll_fheap_assert_intern(size);
	stroll_fheap_assert_intern(compare);

	if (nr > 0) {
		unsigned int node = nr - 1;

		while (node && (compare(elem, &array[node * size], data) > 0)) {
			unsigned int parent = stroll_fheap_parent_index(node);

			memcpy(&array[node * size],
			       &array[parent * size],
			       size);
			node = parent;
		}

		memcpy(&array[node * size], elem, size);
	}
	else
		memcpy(array, elem, size);
}

void
stroll_fheap_insert(void * __restrict     elem,
                    void * __restrict     array,
                    unsigned int          nr,
                    size_t                size,
                    stroll_array_cmp_fn * compare,
                    void *                data)
{
	stroll_fheap_assert_api(elem);
	stroll_fheap_assert_api(array);
	stroll_fheap_assert_api(size);
	stroll_fheap_assert_api(compare);

	stroll_fheap_insert_mem(elem, array, nr, size, compare, data);
}

#endif /* defined(CONFIG_STROLL_FHEAP) */