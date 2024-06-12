/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

//#include "stroll/fwheap.h"
#include "array.h"
#include "stroll/fbmap.h"
#include <stdlib.h>

/*
 * https://en.wikipedia.org/wiki/Weak_heap
 * https://www.geeksforgeeks.org/weak-heap/
 * https://www.sciencedirect.com/science/article/pii/S1570866713000592
 * https://www.sciencedirect.com/science/article/pii/S1570866712000792
 */

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_fwheap_assert_api(_expr) \
	stroll_assert("stroll:fwheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_fwheap_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_fwheap_assert_intern(_expr) \
	stroll_assert("stroll:fwheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_fwheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

static inline __stroll_const __stroll_nothrow
unsigned int
stroll_fwheap_parent_index(unsigned int index)
{
	stroll_fwheap_assert_intern(index);

	return index / 2;
}

static inline __stroll_nonull(2) __stroll_pure __stroll_nothrow
unsigned int
stroll_fwheap_left_child_index(unsigned int                     index,
                               const unsigned long * __restrict rbits)
{
	stroll_fwheap_assert_intern(rbits);

	return (2 * index) + (unsigned int)_stroll_fbmap_test(rbits, index);
}

static inline __stroll_nonull(2) __stroll_pure __stroll_nothrow
unsigned int
stroll_fwheap_right_child_index(unsigned int                     index,
                                const unsigned long * __restrict rbits)
{
	stroll_fwheap_assert_intern(rbits);

	return (2 * index) + 1 - (unsigned int)_stroll_fbmap_test(rbits, index);
}

static inline __stroll_nonull(2) __stroll_pure __stroll_nothrow
bool
stroll_fwheap_isleft_child(unsigned int                     index,
                           const unsigned long * __restrict rbits)
{
	stroll_fwheap_assert_intern(index);
	stroll_fwheap_assert_intern(rbits);

	return (!!(index & 1)) ==
	       _stroll_fbmap_test(rbits, stroll_fwheap_parent_index(index));
}

/*
 * Return index of the so-called distinguished ancestor of node specified by
 * index.
 *
 * The distinguished ancestor of a node located at "index" is the parent of
 * "index" if "index" points to a right child, and the distinguished ancestor of
 * the parent of "index" if "index" is a left child.
 */
static inline __stroll_nonull(2) __stroll_pure __stroll_nothrow
unsigned int
stroll_fwheap_dancestor_index(unsigned int                     index,
                              const unsigned long * __restrict rbits)
{
	/* Root has no ancestor... */
	stroll_fwheap_assert_intern(index);
	stroll_fwheap_assert_intern(rbits);

	/* Move up untill index points to a right child. */
	while (stroll_fwheap_isleft_child(index, rbits))
		index = stroll_fwheap_parent_index(index);

	/* Then return its parent. */
	return stroll_fwheap_parent_index(index);
}

/*
 * Compute index of the distinguished ancestor of node specified by index in an
 * optimized manner.
 *
 * When reverse bits are known to be 0 and set bottom-up while scanning nodes,
 * distinguished ancestor can be computed from the array index by considering
 * the position of the least-significant 1-bit.
 *
 * Unpredictable result if a 0 "index" is passed as argument.
 */
static inline __stroll_const __stroll_nothrow
unsigned int
stroll_fwheap_fast_dancestor_index(unsigned int index)
{
	stroll_fwheap_assert_intern(index);

	return index >> (__builtin_ctz(index) + 1);
}

/*
 * Join weak sub-heaps rooted at "node" node and its distinguished ancestor
 * "dancestor" into a single one rooted at "dancestor" (ensuring proper nodes
 * ordering).
 *
 * Let Ai and Aj be 2 nodes in a weak heap such that the element at Ai is
 * smaller than or equal to every element in the left subtree of Aj.
 * Conceptually, Aj and its right subtree form a weak heap, while Ai and the
 * left subtree of Aj form another weak heap (note that Ai cannot be a
 * descendant of Aj). If the element at Aj is smaller than that at Ai, the 2
 * elements are swapped and Rj (Aj's reverse bit) is flipped. As a result, the
 * element at Aj will be smaller than or equal to every element in its right
 * subtree, and the element at Ai will be smaller than or equal to every element
 * in the subtree rooted at Aj.
 */
static inline __stroll_nonull(3, 4, 6)
void
stroll_fwheap_join_mem(unsigned int               first,
                       unsigned int               second,
                       char * __restrict          array,
                       unsigned long * __restrict rbits,
                       size_t                     size,
                       stroll_array_cmp_fn *      compare,
                       void *                     data)
{
	if (compare(&array[second * size], &array[first * size], data) > 0) {
		/*
		 * Swap node and its distinguished ancestor to restore proper
		 * heap ordering.
		 */
		stroll_array_swap(&array[first * size],
		                  &array[second * size],
		                  size);

		/* Flip former distinguished ancestor's children. */
		_stroll_fbmap_toggle(rbits, second);
	}
}

/*
 * Re-establish weak-heap ordering between elements located at root and all its
 * descendants (i.e., in root's right subtree since it has no left child).
 *
 * Starting from the right child of root, the last node on the left spine of the
 * right subtree of root is identified by repeatedly visiting left children
 * until reaching a node that has no left child.
 * The path from this node to the right child of root is traversed upwards,
 * and join operations are repeatedly performed between root and the nodes along
 * this path.
 * The correctness of the siftdown operation follows from the fact that, after
 * each join, the element at root is less than or equal to every element in
 * the left subtree of the node considered in the next join.
 */
static inline __stroll_nonull(2, 3, 6)
void
stroll_fwheap_siftdwn_mem(unsigned int               index,
                          char * __restrict          array,
                          unsigned long * __restrict rbits,
                          unsigned int               nr,
                          size_t                     size,
                          stroll_array_cmp_fn *      compare,
                          void *                     data)
{
	unsigned int idx = stroll_fwheap_right_child_index(index, rbits);

	/* Identify last node of root's right subtree left spine. */
	while (true) {
		unsigned int left = stroll_fwheap_left_child_index(idx, rbits);

		if (left >= nr)
			break;

		idx = left;
	}

	/*
	 * Now, traverse back up to the root, restoring weak heap property at
	 * each visited node.
	 */
	while (idx != index) {
		stroll_fwheap_join_mem(index,
		                       idx,
		                       array,
		                       rbits,
		                       size,
		                       compare,
		                       data);

		idx = stroll_fwheap_parent_index(idx);
	}
}

#define BUILD 0
#if BUILD == 0
/*
 * Perform bottom-up construction of a weak heap from the content of "nodes".
 *
 * Nodes are visited one by one in reverse order, and the two weak heaps rooted
 * at a node and its distinguished ancestor are joined.
 */
static __stroll_nonull(1, 2, 5)
void
stroll_fwheap_build_mem(char * __restrict          array,
                        unsigned long * __restrict rbits,
                        unsigned int               nr,
                        size_t                     size,
                        stroll_array_cmp_fn *      compare,
                        void *                     data)
{
	stroll_fwheap_assert_intern(array);
	stroll_fwheap_assert_intern(rbits);
	stroll_fwheap_assert_intern(nr > 1);
	stroll_fwheap_assert_intern(size);
	stroll_fwheap_assert_intern(compare);

	_stroll_fbmap_clear_all(rbits, nr);

	do {
		unsigned int danc = stroll_fwheap_fast_dancestor_index(--nr);

		stroll_fwheap_join_mem(danc,
		                       nr,
		                       array,
		                       rbits,
		                       size,
		                       compare,
		                       data);
	} while (nr != 1);
}
#elif BUILD == 1
static __stroll_nonull(1, 2, 5)
void
stroll_fwheap_build_mem(char * __restrict          array,
                        unsigned long * __restrict rbits,
                        unsigned int               nr,
                        size_t                     size,
                        stroll_array_cmp_fn *      compare,
                        void *                     data)
{
	stroll_fwheap_assert_intern(nr > 1);

	unsigned int idx = nr / 2;

	_stroll_fbmap_clear_all(rbits, nr);

	do {
		stroll_fwheap_siftdwn_mem(--idx,
		                          array,
		                          rbits,
		                          nr,
		                          size,
		                          compare,
		                          data);
	} while (idx);
}
#else
static __stroll_nonull(3, 4, 7)
void
stroll_fwheap_build_recmem(unsigned int               parent,
                           unsigned int               child,
                           char * __restrict          array,
                           unsigned long * __restrict rbits,
                           unsigned int               nr,
                           size_t                     size,
                           stroll_array_cmp_fn *      compare,
                           void *                     data)
{
	if (child < (nr / 2)) {
		stroll_fwheap_build_recmem(child,
		                           (2 * child) + 1,
		                           array,
		                           rbits,
		                           nr,
		                           size,
		                           compare,
		                           data);
		stroll_fwheap_build_recmem(parent,
		                           2 * child,
		                           array,
		                           rbits,
		                           nr,
		                           size,
		                           compare,
		                           data);
	}
	stroll_fwheap_join_mem(parent,
	                       child,
	                       array,
	                       rbits,
	                       size,
	                       compare,
	                       data);
}

static __stroll_nonull(1, 2, 5)
void
stroll_fwheap_build_mem(char * __restrict          array,
                        unsigned long * __restrict rbits,
                        unsigned int               nr,
                        size_t                     size,
                        stroll_array_cmp_fn *      compare,
                        void *                     data)
{
	stroll_fwheap_assert_intern(nr > 1);

	/* FIXME: ensure 0 bit is cleared when nr == 0 !! */
	_stroll_fbmap_clear_all(rbits, nr);

	stroll_fwheap_build_recmem(0,
	                           1,
	                           array,
	                           rbits,
	                           nr,
	                           size,
	                           compare,
	                           data);
}
#endif

#if defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT)

static __stroll_nonull(1, 2, 5)
void
stroll_fwheap_sort_mem(char * __restrict          array,
                       unsigned long * __restrict rbits,
                       unsigned int               nr,
                       size_t                     size,
                       stroll_array_cmp_fn *      compare,
                       void *                     data)
{
	stroll_fwheap_assert_intern(array);
	stroll_fwheap_assert_intern(rbits);
	stroll_fwheap_assert_intern(nr > 1);
	stroll_fwheap_assert_intern(size);
	stroll_fwheap_assert_intern(compare);

	unsigned int idx = nr;

	stroll_fwheap_build_mem(array, rbits, nr, size, compare, data);

	while (true) {
		stroll_array_swap(array, &array[--idx * size], size);
		if (idx == 1)
			break;

		stroll_fwheap_siftdwn_mem(0,
		                          array,
		                          rbits,
		                          idx,
		                          size,
		                          compare,
		                          data);
	}
}

int
stroll_array_fwheap_sort(void * __restrict     array,
                         unsigned int          nr,
                         size_t                size,
                         stroll_array_cmp_fn * compare,
                         void *                data)
{
	stroll_fwheap_assert_api(array);
	stroll_fwheap_assert_api(nr);
	stroll_fwheap_assert_api(size);
	stroll_fwheap_assert_api(compare);

	if (nr > 1) {
		unsigned long * rbits;

		rbits = malloc(stroll_fbmap_word_nr(nr) * sizeof(rbits[0]));
		if (!rbits)
			return -errno;

		stroll_fwheap_sort_mem(array, rbits, nr, size, compare, data);

		free(rbits);
	}

	return 0;
}

#endif /* defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT) */
