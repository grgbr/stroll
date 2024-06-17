/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/fwheap.h"
#include "array.h"

/*
 * An array based Weak Heap implementation.
 *
 * A weak heap is most easily understood as a heap-ordered multi-way tree stored
 * as a binary tree using the "right-child left-sibling" convention (see
 * diagrams below).
 *
 * A weak heap is a binary tree that has the following properties:
 * - the root of the entire tree has no left child ;
 * - except for the root, the nodes that have at most 1 child are at the last 2
 *   levels only ;
 * - leaves at the last level can be scattered, i.e. the last level is not
 *   necessarily filled from left to right ;
 * - assuming a min-heap, each node stores an element that is smaller than or
 *   equal to every element stored in its right subtree (respectively greather
 *   than or equal to for a max-heap).
 *
 * The last property mentioned above is called weak-heap ordering or half-tree
 * ordering. In particular, this property enforces no relation between an
 * element in a node and those stored in its left subtree.
 * The height of a weak heap that has n elements is log2(n) + 1.
 *
 * In an array-based implementation, besides the element array, an array of
 * reverse bits is used to allow left / right swapping of subtrees in constant
 * time.
 * For an array element which index is `i', and given its reverse bit `ri':
 * - left child index is `2i + ri',
 * - right child index is `2i + 1 - ri',
 * - parent index is `i / 2'.
 *
 * A weak heap of size 11 may stored into an array as shown below:
 *
 *         0   1    2    3    4    5    6    7    8    9    10       Indices
 *       +---+----+----+----+----+----+----+----+----+----+----+
 *       | 8 | 12 | 27 | 10 | 47 | 49 | 53 | 46 | 75 | 80 | 26 |     Values
 *       +---+----+----+----+----+----+----+----+----+----+----+
 *
 *
 * ... may look like what is shown below according to a binary tree
 * representation:
 *
 *         0
 *          [8]
 *             \______________________
 *                                    \    1
 *                                     [12]
 *                          __________/    \__________
 *                    3    /                          \    2
 *                     [10]                            [27]
 *                    /    \                       ___/    \___
 *              6    /      \    7           5    /            \    4
 *               [53]        [46]             [49]              [47]
 *                                           /                 /    \
 *                                      10  /             8   /      \   9
 *                                       [26]              [75]      [80]
 *
 *
 *
 * ... which corresponds to the following binary tree representation using
 * "right-child left-sibling" convention:
 *
 *
 *                               0
 *                                [8]
 *                                   \_______________
 *         6                    3                    \   1
 *          [53]-----------------[10]-----------------[12]
 *                                 |                      \____
 *                              7  |       10        5         \    2
 *                               [46]       [26]------[49]------[27]
 *                                                                  \
 *                                                         8         \  4
 *                                                          [75]----[47]
 *                                                                   |
 *                                                                   |  9
 *                                                                  [80]
 *
 *
 * ... and corresponds to the following multi-way tree representation:
 *
 *                               0
 *                                [8]
 *               ________________/ | \_______________
 *         6    /                  |                 \   1
 *          [53]                 [10]                 [12]
 *                              3  |             ____/ |  \____
 *                                 |            /      |       \    2
 *                               [46]       [26]      [49]      [27]
 *                              7          10        5         /    \
 *                                                            /      \  4
 *                                                          [75]    [47]
 *                                                         8         |
 *                                                                   |  9
 *                                                                  [80]
 *
 *
 * One reason why a user might vote for a weak heap over a binary heap is that
 * weak heaps are known to perform less element comparisons in the worst case.
 * This may be usefull when comparisons are expensive with respect to memory
 * swaps.
 *
 * See detailed articles below:
 *     "Weak heaps engineered",
 *         https://www.sciencedirect.com/science/article/pii/S1570866713000592
 *     "The weak-heap data structure: Variants and applications",
 *         https://www.sciencedirect.com/science/article/pii/S1570866712000792
 *
 * See also:
 * - https://en.wikipedia.org/wiki/Weak_heap
 * - https://www.geeksforgeeks.org/weak-heap/
 */

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_fwheap_assert_intern(_expr) \
	stroll_assert("stroll:fwheap", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_fwheap_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

static inline __stroll_const __stroll_nothrow
unsigned int
stroll_fwheap_parent(unsigned int index)
{
	stroll_fwheap_assert_intern(index);

	return index / 2;
}

static inline __stroll_nonull(2) __stroll_pure __stroll_nothrow
unsigned int
stroll_fwheap_left_child(unsigned int                     index,
                         const unsigned long * __restrict rbits)
{
	stroll_fwheap_assert_intern(rbits);

	return (2 * index) + (unsigned int)_stroll_fbmap_test(rbits, index);
}

static inline __stroll_nonull(2) __stroll_pure __stroll_nothrow
unsigned int
stroll_fwheap_right_child(unsigned int                     index,
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
	       _stroll_fbmap_test(rbits, stroll_fwheap_parent(index));
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
stroll_fwheap_dancestor(unsigned int                     index,
                        const unsigned long * __restrict rbits)
{
	/* Root has no ancestor... */
	stroll_fwheap_assert_intern(index);
	stroll_fwheap_assert_intern(rbits);

	/* Move up untill index points to a right child. */
	while (stroll_fwheap_isleft_child(index, rbits))
		index = stroll_fwheap_parent(index);

	/* Then return its parent. */
	return stroll_fwheap_parent(index);
}

/*
 * Compute index of the distinguished ancestor of node specified by index in an
 * optimized manner.
 *
 * Warning: use when reverse bit of all nodes are zero only.
 *
 * When reverse bits are known to be 0 and set bottom-up while scanning nodes,
 * distinguished ancestor can be computed from the array index by considering
 * the position of the least-significant 1-bit.
 */
static inline __stroll_const __stroll_nothrow
unsigned int
stroll_fwheap_fast_dancestor(unsigned int index)
{
	stroll_fwheap_assert_intern(index);

	return index >> (__builtin_ctz(index) + 1);
}

/*
 * Return last child of root node, i.e. last node of root's right subtree left
 * spine since root has no left child.
 *
 * Starting from the right child of root, the last node on the
 * left spine of the right subtree of root is identified by
 * repeatedly visiting left children until reaching a node that
 * has no left child.
 */
static inline __stroll_pure __stroll_nothrow __stroll_nonull(1)
unsigned int
stroll_fwheap_last_child(const unsigned long * __restrict rbits,
                         unsigned int                     nr)
{
	stroll_fwheap_assert_intern(rbits);
	stroll_fwheap_assert_intern(nr);

	unsigned int last = 0;
	unsigned int idx = stroll_fwheap_right_child(last, rbits);

	while (idx < nr) {
		last = idx;
		idx = stroll_fwheap_left_child(idx, rbits);
	}

	return last;
}

#if defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT)

#define STROLL_FWHEAP_JOIN_MAX(_func, _swap, _type) \
	static inline __stroll_nonull(3, 4, 5) \
	void \
	_func(unsigned int               dancestor, \
	      unsigned int               child, \
	      _type * __restrict         array, \
	      unsigned long * __restrict rbits, \
	      stroll_array_cmp_fn *      compare, \
	      void *                     data) \
	{ \
		stroll_fwheap_assert_intern(child > dancestor); \
		stroll_fwheap_assert_intern(array); \
		stroll_fwheap_assert_intern(rbits); \
		stroll_fwheap_assert_intern(compare); \
		\
		if (compare(&array[dancestor], &array[child], data) >= 0) \
			return; \
		\
		_swap(&array[dancestor], &array[child]); \
		\
		_stroll_fbmap_toggle(rbits, child); \
	}

STROLL_FWHEAP_JOIN_MAX(stroll_fwheap_join_max32,
                       stroll_array_swap32,
                       uint32_t)

STROLL_FWHEAP_JOIN_MAX(stroll_fwheap_join_max64,
                       stroll_array_swap64,
                       uint64_t)

/*
 * Join weak (max) sub-heaps rooted at "child" node and its distinguished
 * ancestor "dancestor" into a single one rooted at "dancestor" (ensuring proper
 * nodes ordering).
 * See stroll_fwheap_join_min_mem() for more infos.
 */
static inline __stroll_nonull(3, 4, 6)
void
stroll_fwheap_join_max_mem(unsigned int               dancestor,
                           unsigned int               child,
                           char * __restrict          array,
                           unsigned long * __restrict rbits,
                           size_t                     size,
                           stroll_array_cmp_fn *      compare,
                           void *                     data)
{
	stroll_fwheap_assert_intern(child > dancestor);
	stroll_fwheap_assert_intern(array);
	stroll_fwheap_assert_intern(rbits);
	stroll_fwheap_assert_intern(size);
	stroll_fwheap_assert_intern(compare);

	if (compare(&array[dancestor * size], &array[child * size], data) >= 0)
		/* Child node and its distinguished ancestor are in order... */
		return;

	/*
	 * Swap node and its distinguished ancestor to restore proper heap
	 * ordering.
	 */
	stroll_array_swap(&array[dancestor * size], &array[child * size], size);

	/* Flip former distinguished ancestor's children. */
	_stroll_fbmap_toggle(rbits, child);
}

#define STROLL_FWHEAP_SORT(_func, _join_max, _swap, _type) \
	static __stroll_nonull(1, 2, 4) \
	void \
	_func(_type * __restrict         array, \
	      unsigned long * __restrict rbits, \
	      unsigned int               nr, \
	      stroll_array_cmp_fn *      compare, \
	      void *                     data) \
	{ \
		stroll_fwheap_assert_intern(array); \
		stroll_fwheap_assert_intern(rbits); \
		stroll_fwheap_assert_intern(nr > 1); \
		stroll_fwheap_assert_intern(compare); \
		\
		unsigned int idx = nr; \
		\
		_stroll_fbmap_clear_all(rbits, idx); \
		while (--idx) \
			_join_max(stroll_fwheap_fast_dancestor(idx), \
			          idx, \
			          array, \
			          rbits, \
			          compare, \
			          data); \
		\
		while (--nr) { \
			_swap(array, &array[nr]); \
			\
			idx = stroll_fwheap_last_child(rbits, nr); \
			while (idx) { \
				_join_max(0, \
				          idx, \
				          array, \
				          rbits, \
				          compare, \
				          data); \
				idx = stroll_fwheap_parent(idx); \
			} \
		} \
	}

STROLL_FWHEAP_SORT(stroll_fwheap_sort32,
                   stroll_fwheap_join_max32,
                   stroll_array_swap32,
                   uint32_t)

STROLL_FWHEAP_SORT(stroll_fwheap_sort64,
                   stroll_fwheap_join_max64,
                   stroll_array_swap64,
                   uint64_t)

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

	/*
	 * Perform bottom-up construction of a (max) weak heap from the content
	 * of "array". See stroll_fwheap_build_min_mem() for more infos.
	 */
	_stroll_fbmap_clear_all(rbits, idx);
	while (--idx)
		stroll_fwheap_join_max_mem(stroll_fwheap_fast_dancestor(idx),
		                           idx,
		                           array,
		                           rbits,
		                           size,
		                           compare,
		                           data);

	/*
	 * As long as max-heap is not empty, extract (max) root element and
	 * store it into first sorted array location, i.e., right after heap
	 * array's last element.
	 * At the same time, replace root with last heap element and re-heapify
	 * remaining weak heap.
	 */
	while (--nr) {
		/* Swap (max) heap root with sorted array leading element. */
		stroll_array_swap(array, &array[nr * size], size);

		/*
		 * Now, sift the new root element down to restore weak heap
		 * ordering. See stroll_fwheap_extract_min_mem() for more infos.
		 */
		idx = stroll_fwheap_last_child(rbits, nr);
		while (idx) {
			stroll_fwheap_join_max_mem(0,
			                           idx,
			                           array,
			                           rbits,
			                           size,
			                           compare,
			                           data);
			idx = stroll_fwheap_parent(idx);
		}
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

		if (stroll_array_aligned(array, size, sizeof(uint32_t)))
			stroll_fwheap_sort32(array, rbits, nr, compare, data);
		else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
			stroll_fwheap_sort64(array, rbits, nr, compare, data);
		else
			stroll_fwheap_sort_mem(array,
			                       rbits,
			                       nr,
			                       size,
			                       compare,
			                       data);

		free(rbits);
	}

	return 0;
}

#endif /* defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT) */

#if defined(CONFIG_STROLL_FWHEAP)

#define STROLL_FWHEAP_JOIN_MIN(_func, _swap, _type) \
	static inline __stroll_nonull(3, 4, 5) \
	bool \
	_func(unsigned int               dancestor, \
	      unsigned int               child, \
	      _type * __restrict         array, \
	      unsigned long * __restrict rbits, \
	      stroll_array_cmp_fn *      compare, \
	      void *                     data) \
	{ \
		stroll_fwheap_assert_intern(child > dancestor); \
		stroll_fwheap_assert_intern(array); \
		stroll_fwheap_assert_intern(rbits); \
		stroll_fwheap_assert_intern(compare); \
		\
		if (compare(&array[dancestor], &array[child], data) <= 0) \
			return true; \
		\
		_swap(&array[dancestor], &array[child]); \
		\
		_stroll_fbmap_toggle(rbits, child); \
		\
		return false; \
	}

STROLL_FWHEAP_JOIN_MIN(stroll_fwheap_join_min32, stroll_array_swap32, uint32_t)

STROLL_FWHEAP_JOIN_MIN(stroll_fwheap_join_min64, stroll_array_swap64, uint64_t)

/*
 * Join weak (min) sub-heaps rooted at "child" node and its distinguished
 * ancestor "dancestor" into a single one rooted at "dancestor" (ensuring proper
 * nodes ordering).
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
bool
stroll_fwheap_join_min_mem(unsigned int               dancestor,
                           unsigned int               child,
                           char * __restrict          array,
                           unsigned long * __restrict rbits,
                           size_t                     size,
                           stroll_array_cmp_fn *      compare,
                           void *                     data)
{
	stroll_fwheap_assert_intern(child > dancestor);
	stroll_fwheap_assert_intern(array);
	stroll_fwheap_assert_intern(rbits);
	stroll_fwheap_assert_intern(size);
	stroll_fwheap_assert_intern(compare);

	if (compare(&array[dancestor * size], &array[child * size], data) <= 0)
		return true;

	stroll_array_swap(&array[dancestor * size], &array[child * size], size);

	_stroll_fbmap_toggle(rbits, child);

	return false;
}

#define STROLL_FWHEAP_INSERT_MIN(_func, _join_min, _type) \
	static __stroll_nonull(1, 2, 3, 5) \
	void \
	_func(const _type * __restrict   elem, \
	      _type * __restrict         array, \
	      unsigned long * __restrict rbits, \
	      unsigned int               nr, \
	      stroll_array_cmp_fn *      compare, \
	      void *                     data) \
	{ \
		stroll_fwheap_assert_intern(elem); \
		stroll_fwheap_assert_intern(array); \
		stroll_fwheap_assert_intern(rbits); \
		stroll_fwheap_assert_intern((elem < array) || \
		                            (elem >= &array[nr])); \
		stroll_fwheap_assert_intern(compare); \
		\
		array[nr] = *elem; \
		_stroll_fbmap_clear(rbits, nr); \
		\
		if (!nr) \
			return; \
		\
		if (!(nr & 1)) \
			_stroll_fbmap_clear(rbits, stroll_fwheap_parent(nr)); \
		\
		do { \
			unsigned int didx = stroll_fwheap_dancestor(nr, \
			                                            rbits); \
			\
			if (_join_min(didx, nr, array, rbits, compare, data)) \
				break; \
			\
			nr = didx; \
		} while (nr); \
	}

STROLL_FWHEAP_INSERT_MIN(stroll_fwheap_insert_min32,
                         stroll_fwheap_join_min32,
                         uint32_t)

STROLL_FWHEAP_INSERT_MIN(stroll_fwheap_insert_min64,
                         stroll_fwheap_join_min64,
                         uint64_t)

static __stroll_nonull(1, 2, 3, 6)
void
stroll_fwheap_insert_min_mem(const char * __restrict    elem,
                             char * __restrict          array,
                             unsigned long * __restrict rbits,
                             unsigned int               nr,
                             size_t                     size,
                             stroll_array_cmp_fn *      compare,
                             void *                     data)
{
	stroll_fwheap_assert_intern(elem);
	stroll_fwheap_assert_intern(array);
	stroll_fwheap_assert_intern(rbits);
	stroll_fwheap_assert_intern(size);
	stroll_fwheap_assert_intern((elem < array) ||
	                            ((const char *)elem >=
	                             &((char *)array)[nr * size]));
	stroll_fwheap_assert_intern(compare);

	/*
	 * Add element to next available array entry and initialize its reverse
	 * bit.
	 */
	memcpy(&array[nr * size], elem, size);
	_stroll_fbmap_clear(rbits, nr);

	if (!nr)
		return;

	if (!(nr & 1))
		/*
		 * If this leaf is the only child of its parent, we make it a
		 * left child by updating the reverse bit at the parent to save
		 * an unnecessary element comparison.
		 */
		_stroll_fbmap_clear(rbits, stroll_fwheap_parent(nr));

	/*
	 * Sift inserted node up, i.e. reestablish heap ordering between element
	 * initially stored into array and those located at its (distinguished)
	 * ancestors.
	 */
	do {
		/* Find distinguished ancestor for current node */
		unsigned int didx = stroll_fwheap_dancestor(nr, rbits);

		/*
		 * Join both sub-heaps rooted at current node and its
		 * distinguished ancestor.
		 */
		if (stroll_fwheap_join_min_mem(didx,
		                               nr,
		                               array,
		                               rbits,
		                               size,
		                               compare,
		                               data))
			break;

		/* Iterate up to root. */
		nr = didx;
	} while (nr);
}

void
_stroll_fwheap_insert(const void * __restrict    elem,
                      void * __restrict          array,
                      unsigned long * __restrict rbits,
                      unsigned int               nr,
                      size_t                     size,
                      stroll_array_cmp_fn *      compare,
                      void *                     data)
{
	stroll_fwheap_assert_api(elem);
	stroll_fwheap_assert_api(array);
	stroll_fwheap_assert_api(rbits);
	stroll_fwheap_assert_api(size);
	stroll_fwheap_assert_api((elem < array) ||
	                         ((const char *)elem >=
	                          &((char *)array)[nr * size]));
	stroll_fwheap_assert_api(compare);

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		return stroll_fwheap_insert_min32(elem, \
		                                  array, \
		                                  rbits, \
		                                  nr, \
		                                  compare, \
		                                  data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		return stroll_fwheap_insert_min64(elem, \
		                                  array, \
		                                  rbits, \
		                                  nr, \
		                                  compare, \
		                                  data);
	else
		stroll_fwheap_insert_min_mem(elem,
		                             array,
		                             rbits,
		                             nr,
		                             size,
		                             compare,
		                             data);
}

#define STROLL_FWHEAP_EXTRACT_MIN(_func, _join_min, _type) \
	static __stroll_nonull(1, 2, 3, 5) \
	void \
	_func(_type * __restrict         elem, \
	      _type * __restrict         array, \
	      unsigned long * __restrict rbits, \
	      unsigned int               nr, \
	      stroll_array_cmp_fn *      compare, \
	      void *                     data) \
	{ \
		stroll_fwheap_assert_intern(elem); \
		stroll_fwheap_assert_intern(array); \
		stroll_fwheap_assert_intern(rbits); \
		stroll_fwheap_assert_intern(nr); \
		stroll_fwheap_assert_intern((elem < array) || \
		                            (elem >= &array[nr])); \
		stroll_fwheap_assert_intern(compare); \
		\
		*elem = *array; \
		\
		if (--nr) { \
			unsigned int idx = stroll_fwheap_last_child(rbits, \
			                                            nr); \
			\
			array[0] = array[nr]; \
			\
			while (idx) { \
				_join_min(0, \
				          idx, \
				          array, \
				          rbits, \
				          compare, \
				          data); \
				idx = stroll_fwheap_parent(idx); \
			} \
		} \
	}

STROLL_FWHEAP_EXTRACT_MIN(stroll_fwheap_extract_min32,
                          stroll_fwheap_join_min32,
                          uint32_t)

STROLL_FWHEAP_EXTRACT_MIN(stroll_fwheap_extract_min64,
                          stroll_fwheap_join_min64,
                          uint64_t)

static __stroll_nonull(1, 2, 3, 6)
void
stroll_fwheap_extract_min_mem(char * __restrict          elem,
                              char * __restrict          array,
                              unsigned long * __restrict rbits,
                              unsigned int               nr,
                              size_t                     size,
                              stroll_array_cmp_fn *      compare,
                              void *                     data)
{
	stroll_fwheap_assert_intern(elem);
	stroll_fwheap_assert_intern(array);
	stroll_fwheap_assert_intern(rbits);
	stroll_fwheap_assert_intern(nr);
	stroll_fwheap_assert_intern(size);
	stroll_fwheap_assert_intern((elem < array) ||
	                            ((char *)elem >=
	                             &((char *)array)[nr * size]));
	stroll_fwheap_assert_intern(compare);

	/* Copy root (min) element to location given in argument. */
	memcpy(elem, array, size);

	if (--nr) {
		unsigned int idx;

		/*
		 * Replace root element with that stored at the last occupied
		 * array entry.
		 */
		memcpy(array, &array[nr * size], size);

		/*
		 * Now, sift the new root element down to restore weak heap
		 * ordering between elements located at root and all its
		 * descendants (see stroll_fwheap_last_child() for more infos).
		 *
		 * The path from last left child of root node to the right child
		 * of root is traversed upwards, and join operations are
		 * repeatedly performed between root and the nodes along this
		 * path.
		 *
		 * The correctness of the siftdown operation follows from the
		 * fact that, after each join, the element at root is less than
		 * or equal to every element in the left subtree of the node
		 * considered in the next join.
		 */
		idx = stroll_fwheap_last_child(rbits, nr);
		while (idx) {
			stroll_fwheap_join_min_mem(0,
			                           idx,
			                           array,
			                           rbits,
			                           size,
			                           compare,
			                           data);
			idx = stroll_fwheap_parent(idx);
		}
	}
}

void
_stroll_fwheap_extract(void * __restrict          elem,
                       void * __restrict          array,
                       unsigned long * __restrict rbits,
                       unsigned int               nr,
                       size_t                     size,
                       stroll_array_cmp_fn *      compare,
                       void *                     data)
{
	stroll_fwheap_assert_api(elem);
	stroll_fwheap_assert_api(array);
	stroll_fwheap_assert_api(rbits);
	stroll_fwheap_assert_api(nr);
	stroll_fwheap_assert_api(size);
	stroll_fwheap_assert_api((elem < array) ||
	                         ((char *)elem >= &((char *)array)[nr * size]));
	stroll_fwheap_assert_api(compare);

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		return stroll_fwheap_extract_min32(elem,
		                                   array,
		                                   rbits,
		                                   nr,
		                                   compare,
		                                   data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		return stroll_fwheap_extract_min64(elem,
		                                   array,
		                                   rbits,
		                                   nr,
		                                   compare,
		                                   data);
	else
		stroll_fwheap_extract_min_mem(elem,
	                                      array,
	                                      rbits,
	                                      nr,
	                                      size,
	                                      compare,
	                                      data);
}

#define STROLL_FWHEAP_BUILD_MIN(_func, _join_min, _type) \
	static __stroll_nonull(1, 2, 4) \
	void \
	_func(_type * __restrict         array, \
	      unsigned long * __restrict rbits, \
	      unsigned int               nr, \
	      stroll_array_cmp_fn *      compare, \
	      void *                     data) \
	{ \
		stroll_fwheap_assert_intern(array); \
		stroll_fwheap_assert_intern(rbits); \
		stroll_fwheap_assert_intern(nr > 1); \
		stroll_fwheap_assert_intern(compare); \
		\
		_stroll_fbmap_clear_all(rbits, nr); \
		\
		do { \
			unsigned int danc = \
				stroll_fwheap_fast_dancestor(--nr); \
			\
			_join_min(danc, nr, array, rbits, compare, data); \
		} while (nr != 1); \
	}

STROLL_FWHEAP_BUILD_MIN(stroll_fwheap_build_min32,
                        stroll_fwheap_join_min32,
                        uint32_t)

STROLL_FWHEAP_BUILD_MIN(stroll_fwheap_build_min64,
                        stroll_fwheap_join_min64,
                        uint64_t)

static __stroll_nonull(1, 2, 5)
void
stroll_fwheap_build_min_mem(char * __restrict          array,
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

	/* Zero initialize all reverse bits. */
	_stroll_fbmap_clear_all(rbits, nr);

	/*
	 * Perform bottom-up construction of the weak heap from the content of
	 * "array".
	 * Nodes are visited one by one in reverse order, and the two weak heaps
	 * rooted at a node and its distinguished ancestor are joined.
	 */
	while (--nr)
		stroll_fwheap_join_min_mem(
			stroll_fwheap_fast_dancestor(nr),
			nr,
			array,
			rbits,
			size,
			compare,
			data);
}

void
_stroll_fwheap_build(void * __restrict          array,
                     unsigned long * __restrict rbits,
                     unsigned int               nr,
                     size_t                     size,
                     stroll_array_cmp_fn *      compare,
                     void *                     data)
{
	stroll_fwheap_assert_api(array);
	stroll_fwheap_assert_api(rbits);
	stroll_fwheap_assert_api(nr);
	stroll_fwheap_assert_api(size);
	stroll_fwheap_assert_api(compare);

	if (nr == 1)
		return;

	if (stroll_array_aligned(array, size, sizeof(uint32_t)))
		stroll_fwheap_build_min32(array,
		                          rbits,
		                          nr,
		                          compare,
		                          data);
	else if (stroll_array_aligned(array, size, sizeof(uint64_t)))
		stroll_fwheap_build_min64(array,
		                          rbits,
		                          nr,
		                          compare,
		                          data);
	else
		stroll_fwheap_build_min_mem(array,
		                            rbits,
		                            nr,
		                            size,
		                            compare,
		                            data);
}

unsigned long *
_stroll_fwheap_alloc_rbits(unsigned int nr)
{
	stroll_fwheap_assert_api(nr);

	return malloc(_stroll_fwheap_rbits_size(nr));
}

void
stroll_fwheap_insert(struct stroll_fwheap * __restrict heap,
                     const void * __restrict           elem,
                     void * __restrict                 data)
{
	stroll_fwheap_assert_heap_api(heap);
	stroll_fwheap_assert_api(elem);
	stroll_fwheap_assert_api(heap->cnt < heap->nr);

	_stroll_fwheap_insert(elem,
	                      heap->elems,
	                      heap->rbits,
	                      heap->cnt++,
	                      heap->size,
	                      heap->compare,
	                      data);
}

void
stroll_fwheap_extract(struct stroll_fwheap * __restrict heap,
                      void * __restrict                 elem,
                      void * __restrict                 data)
{
	stroll_fwheap_assert_heap_api(heap);
	stroll_fwheap_assert_api(elem);
	stroll_fwheap_assert_api(heap->cnt);

	_stroll_fwheap_extract(elem,
	                       heap->elems,
	                       heap->rbits,
	                       heap->cnt--,
	                       heap->size,
	                       heap->compare,
	                       data);
}

void
stroll_fwheap_build(struct stroll_fwheap * __restrict heap,
                    unsigned int                      count,
                    void * __restrict                 data)
{
	stroll_fwheap_assert_heap_api(heap);
	stroll_fwheap_assert_api(count);
	stroll_fwheap_assert_api(count <= heap->nr);

	_stroll_fwheap_build(heap->elems,
	                     heap->rbits,
	                     count,
	                     heap->size,
	                     heap->compare,
	                     data);
	heap->cnt = count;
}

int
stroll_fwheap_init(struct stroll_fwheap * __restrict heap,
                   void * __restrict                 array,
                   unsigned int                      nr,
                   size_t                            size,
                   stroll_array_cmp_fn *             compare)
{
	stroll_fwheap_assert_api(heap);
	stroll_fwheap_assert_api(array);
	stroll_fwheap_assert_api(nr);
	stroll_fwheap_assert_api(size);
	stroll_fwheap_assert_api(compare);

	heap->rbits = _stroll_fwheap_alloc_rbits(nr);
	if (!heap->rbits)
		return -errno;

	heap->cnt = 0;
	heap->nr = nr;
	heap->size = size;
	heap->compare = compare;
	heap->elems = array;

	return 0;
}

struct stroll_fwheap *
stroll_fwheap_create(void * __restrict     array,
                     unsigned int          nr,
                     size_t                size,
                     stroll_array_cmp_fn * compare)
{
	stroll_fwheap_assert_api(array);
	stroll_fwheap_assert_api(nr);
	stroll_fwheap_assert_api(size);
	stroll_fwheap_assert_api(compare);

	struct stroll_fwheap * heap;
	int                    err;

	heap = malloc(sizeof(*heap));
	if (!heap)
		return NULL;

	err = stroll_fwheap_init(heap, array, nr, size, compare);
	if (err)
		goto free;

	return heap;

free:
	free(heap);

	errno = -err;

	return NULL;
}

void
stroll_fwheap_destroy(struct stroll_fwheap * heap)
{
	stroll_fwheap_fini(heap);

	free(heap);
}

#endif /* defined(CONFIG_STROLL_FWHEAP) */
