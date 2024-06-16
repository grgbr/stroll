/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/fwheap.h"
#include "array.h"

/*
 * https://en.wikipedia.org/wiki/Weak_heap
 * https://www.geeksforgeeks.org/weak-heap/
 * https://www.sciencedirect.com/science/article/pii/S1570866713000592
 * https://www.sciencedirect.com/science/article/pii/S1570866712000792
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
 * Identify last node of index's right subtree left spine.
 */
static inline __stroll_pure __stroll_nothrow __stroll_nonull(1)
unsigned int
stroll_fwheap_last_child(const unsigned long * __restrict rbits,
                         unsigned int                     nr)
{
	stroll_fwheap_assert_intern(rbits);
	stroll_fwheap_assert_intern(nr);

	unsigned int last = 0;
	unsigned int idx = stroll_fwheap_right_child_index(last, rbits);

	while (idx < nr) {
		last = idx;
		idx = stroll_fwheap_left_child_index(idx, rbits);
	}

	return last;
}

#if defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT)

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
		return;

	/*
	 * Swap node and its distinguished ancestor to restore proper heap
	 * ordering.
	 */
	stroll_array_swap(&array[dancestor * size], &array[child * size], size);

	/* Flip former distinguished ancestor's children. */
	_stroll_fbmap_toggle(rbits, child);
}

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
	 * Perform bottom-up construction of a weak heap from the content of
	 * "nodes".
	 *
	 * Nodes are visited one by one in reverse order, and the two weak heaps
	 * rooted at a node and its distinguished ancestor are joined.
	 */
	_stroll_fbmap_clear_all(rbits, idx);
	while (--idx)
		stroll_fwheap_join_max_mem(
			stroll_fwheap_fast_dancestor_index(idx),
			idx,
			array,
			rbits,
			size,
			compare,
			data);

	/*
	 * As long as max-heap is not empty, extract max element and store it
	 * into first sorted array location, i.e., right after heap array's last
	 * element.
	 * At the same time, copy last heap element into top position then
	 * re-heapify remaining weak heap.
	 */
	while (--nr) {
		/* Extract max heap element and store it into sorted array. */
		stroll_array_swap(array, &array[nr * size], size);

		/*
		 * Re-establish weak-heap ordering between elements located at
		 * root and all its descendants (i.e., in root's right subtree
		 * since it has no left child) according to a "sift-down"
		 * procedure.
		 *
		 * Starting from the right child of root, the last node on the
		 * left spine of the right subtree of root is identified by
		 * repeatedly visiting left children until reaching a node that
		 * has no left child.
		 * The path from this node to the right child of root is
		 * traversed upwards, and join operations are repeatedly
		 * performed between root and the nodes along this path.
		 * The correctness of the siftdown operation follows from the
		 * fact that, after each join, the element at root is less than
		 * or equal to every element in the left subtree of the node
		 * considered in the next join.
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
			idx = stroll_fwheap_parent_index(idx);
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

		stroll_fwheap_sort_mem(array, rbits, nr, size, compare, data);

		free(rbits);
	}

	return 0;
}

#endif /* defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT) */

#if defined(CONFIG_STROLL_FWHEAP)

static __stroll_nonull(3, 4, 6)
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

	/*
	 * Swap node and its distinguished ancestor to restore proper heap
	 * ordering.
	 */
	stroll_array_swap(&array[dancestor * size], &array[child * size], size);

	/* Flip former distinguished ancestor's children. */
	_stroll_fbmap_toggle(rbits, child);

	/* Tell caller that we swapped child and its distinguished ancestor. */
	return false;
}

static
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

	memcpy(&array[nr * size], elem, size);
	_stroll_fbmap_clear(rbits, nr);

	if (!nr)
		return;

	if (!(nr & 1))
		/*
		 * If this leaf is the only child of its parent, we make
		 * it a left child by updating the reverse bit at the
		 * parent to save an unnecessary element comparison.
		 */
		_stroll_fbmap_clear(rbits, stroll_fwheap_parent_index(nr));

	/*
	 * Sift inserted node up, i.e. reestablish heap ordering between element
	 * initially located at idx and those located at its ancestors.
	 */
	do {
		/* Find distinguished ancestor for current node */
		unsigned int didx = stroll_fwheap_dancestor_index(nr, rbits);

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

	stroll_fwheap_insert_min_mem(elem,
	                             array,
	                             rbits,
	                             nr,
	                             size,
	                             compare,
	                             data);
}

static
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

	memcpy(elem, array, size);

	if (--nr) {
		unsigned int idx = stroll_fwheap_last_child(rbits, nr);

		memcpy(array, &array[nr * size], size);

		while (idx) {
			stroll_fwheap_join_min_mem(0,
			                           idx,
			                           array,
			                           rbits,
			                           size,
			                           compare,
			                           data);
			idx = stroll_fwheap_parent_index(idx);
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

	stroll_fwheap_extract_min_mem(elem,
	                              array,
	                              rbits,
	                              nr,
	                              size,
	                              compare,
	                              data);
}

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

	_stroll_fbmap_clear_all(rbits, nr);

	while (--nr)
		stroll_fwheap_join_min_mem(
			stroll_fwheap_fast_dancestor_index(nr),
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

	stroll_fwheap_build_min_mem(array, rbits, nr, size, compare, data);
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
