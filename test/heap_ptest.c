#include "ptest.h"
#include "stroll/array.h"
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

typedef int
        (strollpt_heap_prepare_fn)(const unsigned int * __restrict,
                                   unsigned int,
                                   size_t)
	__stroll_nonull(1);

typedef int
        (strollpt_heap_measure_fn)(const unsigned int * __restrict,
                                   unsigned int,
                                   size_t,
                                   unsigned int,
                                   unsigned long long * __restrict)
	__stroll_nonull(1, 5);

struct strollpt_heap_algo {
	const char *               name;
	strollpt_heap_prepare_fn * prepare;
	strollpt_heap_measure_fn * measure;
};

typedef void * (strollpt_heap_create_fn)(void * __restrict,
                                         unsigned int,
                                         size_t)
	__stroll_nonull(1) __warn_result;

typedef void (strollpt_heap_destroy_fn)(void * __restrict) __stroll_nonull(1);

typedef void (strollpt_heap_build_fn)(void * __restrict,
                                      void * __restrict,
                                      unsigned int,
                                      size_t)
	__stroll_nonull(1);

typedef void (strollpt_heap_insert_fn)(void * __restrict, void * __restrict)
	__stroll_nonull(1, 2);

typedef void (strollpt_heap_extract_fn)(void * __restrict, void * __restrict)
	__stroll_nonull(1, 2);

typedef unsigned int (strollpt_heap_count_fn)(void * __restrict)
	__stroll_nonull(1);

typedef bool (strollpt_heap_validate_fn)(const void * __restrict , unsigned int)
	__stroll_nonull(1);

struct strollpt_heap_iface {
	strollpt_heap_create_fn *   create;
	strollpt_heap_destroy_fn *  destroy;
	strollpt_heap_build_fn *    build;
	strollpt_heap_insert_fn *   insert;
	strollpt_heap_extract_fn *  extract;
	strollpt_heap_count_fn *    count;
	strollpt_heap_validate_fn * validate;
};

enum strollpt_heap_op {
	STROLLPT_HEAP_BUILD_OP   = 0,
	STROLLPT_HEAP_INSERT_OP  = 1,
	STROLLPT_HEAP_EXTRACT_OP = 2,
	STROLLPT_HEAP_OP_NR
};

static const char * strollpt_heap_operations[] = {
	[STROLLPT_HEAP_BUILD_OP]   = "heapify",
	[STROLLPT_HEAP_INSERT_OP]  = "insert",
	[STROLLPT_HEAP_EXTRACT_OP] = "extract"
};

#if defined(CONFIG_STROLL_FBHEAP) || defined(CONFIG_STROLL_FWHEAP)

struct strollpt_heap_elem {
	unsigned int id;
	const char   data[0];
};

static void
strollpt_heap_setup_array(struct strollpt_heap_elem * elements,
                          const unsigned int *        from,
                          unsigned int                nr,
                          size_t                      size)
{
	assert(elements);
	assert(from);
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_heap_elem * ptr;
	unsigned int           n;

	for (n = 0, ptr = elements;
	     n < nr;
	     n++, ptr = (struct strollpt_heap_elem *)((char *)ptr + size))
		ptr->id = from[n];
}

static struct strollpt_heap_elem *
strollpt_heap_alloc_array(unsigned int nr, size_t size)
{
	assert(nr);
	assert(size);

	return malloc(nr * size);
}

static struct strollpt_heap_elem *
strollpt_heap_create_array(const unsigned int * elements,
                           unsigned int         nr,
                           size_t               size)
{
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_heap_elem * elms;

	elms = strollpt_heap_alloc_array(nr, size);
	if (!elms)
		return NULL;

	strollpt_heap_setup_array(elms, elements, nr, size);

	return elms;
}

static void
strollpt_heap_destroy_array(struct strollpt_heap_elem * elements)
{
	free(elements);
}

static int
strollpt_heap_prepare_array(const unsigned int * __restrict    elements,
                            unsigned int                       nr,
                            size_t                             size,
                            const struct strollpt_heap_iface * iface)
{
	assert(elements);
	assert(nr);
	assert(size);
	assert(size <= UINT_MAX);
	assert(iface);

	struct strollpt_heap_elem * elms;
	unsigned int *              sort;
	void *                      heap;
	unsigned int                e;
	int                         ret = EXIT_FAILURE;

	if (size % sizeof_member(struct strollpt_heap_elem, id)) {
		strollpt_err("invalid data element size %zu specified: "
		             "multiple of %zu expected.\n",
		             size,
		             sizeof_member(struct strollpt_heap_elem, id));
		return EXIT_FAILURE;
	}

	elms = strollpt_heap_create_array(elements, nr, size);
	if (!elms)
		return EXIT_FAILURE;

	sort = malloc(nr * sizeof(elements[0]));
	if (!sort)
		goto destroy_array;
	memcpy(sort, elements, nr * sizeof(elements[0]));
	qsort_r(sort, nr, sizeof(sort[0]), strollpt_array_compare_min, NULL);

	heap = iface->create(elms, nr, size);
	if (!heap)
		goto free_sort;

	iface->build(heap, elms, nr, size);
	if (!iface->validate(heap, nr)) {
		strollpt_err("Bogus heapify scheme.\n");
		goto free_heap;
	}

	for (e = 0; e < nr; e++) {
		char elm[size];

		memset(elm, 0xa5, size);

		iface->extract(heap, elm);
		if (((struct strollpt_heap_elem *)elm)->id != sort[e]) {
			strollpt_err("Bogus heap extraction scheme.\n");
			goto free_heap;
		}

		if (iface->count(heap) != (nr - e - 1)) {
			strollpt_err("Bogus heap extraction count.\n");
			goto free_heap;
		}
	}

	memset(elms, 0xa5, nr * size);
	for (e = 0; e < nr; e++) {
		char elm[size];

		memset(elm, 0, size);

		((struct strollpt_heap_elem *)elm)->id = elements[e];
		iface->insert(heap, elm);

		if (iface->count(heap) != (e + 1)) {
			strollpt_err("Bogus heap insertion count.\n");
			goto free_heap;
		}
	}
	if (!iface->validate(heap, nr)) {
		strollpt_err("Bogus heap insertion scheme.\n");
		goto free_heap;
	}

	ret = EXIT_SUCCESS;

free_heap:
	iface->destroy(heap);
free_sort:
	free(sort);
destroy_array:
	strollpt_heap_destroy_array(elms);

	return ret;
}

static int
strollpt_heap_measure_array_build(
	const unsigned int * __restrict               elements,
	unsigned int                                  nr,
	size_t                                        size,
	unsigned long long * __restrict               nsecs,
	const struct strollpt_heap_iface * __restrict iface)
{
	struct timespec             start, elapse;
	struct strollpt_heap_elem * elms;
	void *                      heap;
	int                         ret = EXIT_FAILURE;

	elms = strollpt_heap_create_array(elements, nr, size);
	if (!elms)
		return EXIT_FAILURE;
	heap = iface->create(elms, nr, size);
	if (!heap)
		goto destroy_array;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	iface->build(heap, elms, nr, size);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_array:
	strollpt_heap_destroy_array(elms);

	return ret;
}

static int
strollpt_heap_measure_array_insert(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned long long * __restrict    nsecs,
	const struct strollpt_heap_iface * iface)
{
	struct timespec             start, elapse;
	struct strollpt_heap_elem * elms;
	void *                      heap;
	unsigned int                e;
	int                         ret = EXIT_FAILURE;

	elms = strollpt_heap_alloc_array(nr, size);
	if (!elms)
		return EXIT_FAILURE;
	heap = iface->create(elms, nr, size);
	if (!heap)
		goto destroy_array;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		char elm[size];

		((struct strollpt_heap_elem *)elm)->id = elements[e];
		iface->insert(heap, elm);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_array:
	strollpt_heap_destroy_array(elms);

	return ret;
}

static int
strollpt_heap_measure_array_extract(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned long long * __restrict    nsecs,
	const struct strollpt_heap_iface * iface)
{
	struct timespec             start, elapse;
	struct strollpt_heap_elem * elms;
	void *                      heap;
	unsigned int                e;
	int                         ret = EXIT_FAILURE;

	elms = strollpt_heap_create_array(elements, nr, size);
	if (!elms)
		return EXIT_FAILURE;
	heap = iface->create(elms, nr, size);
	if (!heap)
		goto destroy_array;
	iface->build(heap, elms, nr, size);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		char elm[size];

		iface->extract(heap, elm);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_array:
	strollpt_heap_destroy_array(elms);

	return ret;
}

static int
strollpt_heap_measure_array(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned int                       operations,
	unsigned long long                 nsecs[__restrict_arr STROLLPT_HEAP_OP_NR],
	const struct strollpt_heap_iface * iface)
{
	assert(elements);
	assert(nr);
	assert(size);
	assert(operations);
	assert(nsecs);
	assert(iface);

	unsigned int op;
	int          ret = EXIT_SUCCESS;

	memset(nsecs, 0, STROLLPT_HEAP_OP_NR * sizeof(nsecs[0]));

	for (op = 0; !ret && (op < STROLLPT_HEAP_OP_NR); op++) {
		if (!(operations & (1U << op)))
			continue;

		switch (op) {
		case STROLLPT_HEAP_BUILD_OP:
			ret = strollpt_heap_measure_array_build(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_BUILD_OP],
				iface);
			break;

		case STROLLPT_HEAP_INSERT_OP:
			ret = strollpt_heap_measure_array_insert(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_INSERT_OP],
				iface);
			break;

		case STROLLPT_HEAP_EXTRACT_OP:
			ret = strollpt_heap_measure_array_extract(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_EXTRACT_OP],
				iface);
			break;

		default:
			ret = EXIT_SUCCESS;
			break;
		}
	}

	return ret;
}

#endif /* defined(CONFIG_STROLL_FBHEAP) || defined(CONFIG_STROLL_FWHEAP) */

#if defined(CONFIG_STROLL_FBHEAP)

#include "stroll/fbheap.h"

static bool
strollpt_fbheap_validate_rec(unsigned int index,
                             const void * heap,
                             unsigned int nr)
{
	assert(heap);
	assert(nr);

	const struct stroll_fbheap * hp = heap;
	size_t                       sz = hp->size;

	if (index >= nr)
		return true;

	if (index) {
		const char * elms = (const char *)hp->elems;
		unsigned int parent = (index - 1) / 2;

		if (strollpt_array_compare_min(&elms[parent * sz],
		                               &elms[index * sz],
		                               NULL) > 0)
			return false;
	}

	if (!strollpt_fbheap_validate_rec((2 * index) + 1, heap, nr))
		return false;

	return strollpt_fbheap_validate_rec((2 * index) + 2, heap, nr);
}

static bool
strollpt_fbheap_validate(const void * __restrict heap, unsigned int nr)
{
	return strollpt_fbheap_validate_rec(0, heap, nr);
}

static void *
strollpt_fbheap_create(void * __restrict     array,
                       unsigned int          nr,
                       size_t                size)
{
	return stroll_fbheap_create(array,
	                            nr,
	                            size,
	                            strollpt_array_compare_min);
}

static void
strollpt_fbheap_destroy(void * __restrict heap)
{
	stroll_fbheap_destroy(heap);
}

static void
strollpt_fbheap_build(void * __restrict heap,
                      void * __restrict array __unused,
                      unsigned int      nr __unused,
                      size_t            size  __unused)
{
	stroll_fbheap_build(heap, stroll_fbheap_nr(heap), NULL);
}

static void
strollpt_fbheap_insert(void * __restrict heap, void * __restrict elem)
{
	stroll_fbheap_insert(heap, elem, NULL);
}

static void
strollpt_fbheap_extract(void * __restrict heap, void * __restrict elem)
{
	stroll_fbheap_extract(heap, elem, NULL);
}

static unsigned int
strollpt_fbheap_count(void * __restrict heap)
{
	return stroll_fbheap_count(heap);
}

static const struct strollpt_heap_iface strollpt_fbheap_iface = {
	.create   = strollpt_fbheap_create,
	.destroy  = strollpt_fbheap_destroy,
	.build    = strollpt_fbheap_build,
	.insert   = strollpt_fbheap_insert,
	.extract  = strollpt_fbheap_extract,
	.count    = strollpt_fbheap_count,
	.validate = strollpt_fbheap_validate
};

static int
strollpt_heap_prepare_fbheap(const unsigned int * __restrict elements,
                             unsigned int                    nr,
                             size_t                          size)
{
	return strollpt_heap_prepare_array(elements,
	                                   nr,
	                                   size,
	                                   &strollpt_fbheap_iface);
}

static int
strollpt_heap_measure_fbheap(
	const unsigned int * __restrict elements,
	unsigned int                    nr,
	size_t                          size,
	unsigned int                    operations,
	unsigned long long              nsecs[__restrict_arr STROLLPT_HEAP_OP_NR])
{
	return strollpt_heap_measure_array(elements,
	                                   nr,
	                                   size,
	                                   operations,
	                                   nsecs,
	                                   &strollpt_fbheap_iface);
}

#endif /* defined(CONFIG_STROLL_FBHEAP) */

#if defined(CONFIG_STROLL_FWHEAP)

#include "stroll/fwheap.h"

static inline unsigned int
strollpt_fwheap_parent(unsigned int index)
{
	assert(index);

	return index / 2;
}

static inline unsigned int
strollpt_fwheap_left(unsigned int index, const unsigned long * rbits)
{
	return (2 * index) +
	       (unsigned int)_stroll_fbmap_test(rbits, index);
}

static inline unsigned int
strollpt_fwheap_right(unsigned int index, const unsigned long * rbits)
{
	return (2 * index) + 1 - (unsigned int)_stroll_fbmap_test(rbits, index);
}

static inline bool
strollpt_fwheap_isleft(unsigned int index, const unsigned long * rbits)
{
	assert(index);

	return (!!(index & 1)) ==
	       _stroll_fbmap_test(rbits, strollpt_fwheap_parent(index));
}

static inline unsigned int
strollpt_fwheap_dancestor(unsigned int index, const unsigned long * rbits)
{
	/* Move up untill index points to a right child. */
	while (strollpt_fwheap_isleft(index, rbits))
		index = strollpt_fwheap_parent(index);

	/* Then return its parent. */
	return strollpt_fwheap_parent(index);
}

static bool
strollpt_fwheap_validate_rec(unsigned int index,
                             const void * heap,
                             unsigned int nr)
{
	assert(heap);
	assert(nr);

	const struct stroll_fwheap * hp = heap;
	size_t                       sz = hp->size;

	if (index >= nr)
		return true;

	if (index) {
		const char * elms = (const char *)hp->elems;
		unsigned int danc = strollpt_fwheap_dancestor(index, hp->rbits);

		if (strollpt_array_compare_min(&elms[danc * sz],
		                               &elms[index * sz],
		                               NULL) > 0)
			return false;
	}

	if (index) {
		/* Root node has no left child ! */
		if (!strollpt_fwheap_validate_rec(
			strollpt_fwheap_left(index, hp->rbits), heap, nr))
			return false;
	}

	return strollpt_fwheap_validate_rec(
		strollpt_fwheap_right(index, hp->rbits), heap, nr);
}

static bool
strollpt_fwheap_validate(const void * __restrict heap, unsigned int nr)
{
	return strollpt_fwheap_validate_rec(0, heap, nr);
}

static void *
strollpt_fwheap_create(void * __restrict     array,
                       unsigned int          nr,
                       size_t                size)
{
	return stroll_fwheap_create(array,
	                            nr,
	                            size,
	                            strollpt_array_compare_min);
}

static void
strollpt_fwheap_destroy(void * __restrict heap)
{
	stroll_fwheap_destroy(heap);
}

static void
strollpt_fwheap_build(void * __restrict heap,
                      void * __restrict array __unused,
                      unsigned int      nr __unused,
                      size_t            size  __unused)
{
	stroll_fwheap_build(heap, stroll_fwheap_nr(heap), NULL);
}

static void
strollpt_fwheap_insert(void * __restrict heap, void * __restrict elem)
{
	stroll_fwheap_insert(heap, elem, NULL);
}

static void
strollpt_fwheap_extract(void * __restrict heap, void * __restrict elem)
{
	stroll_fwheap_extract(heap, elem, NULL);
}

static unsigned int
strollpt_fwheap_count(void * __restrict heap)
{
	return stroll_fwheap_count(heap);
}

static const struct strollpt_heap_iface strollpt_fwheap_iface = {
	.create   = strollpt_fwheap_create,
	.destroy  = strollpt_fwheap_destroy,
	.build    = strollpt_fwheap_build,
	.insert   = strollpt_fwheap_insert,
	.extract  = strollpt_fwheap_extract,
	.count    = strollpt_fwheap_count,
	.validate = strollpt_fwheap_validate
};

static int
strollpt_heap_prepare_fwheap(const unsigned int * __restrict elements,
                             unsigned int                    nr,
                             size_t                          size)
{
	return strollpt_heap_prepare_array(elements,
	                                   nr,
	                                   size,
	                                   &strollpt_fwheap_iface);
}

static int
strollpt_heap_measure_fwheap(
	const unsigned int * __restrict elements,
	unsigned int                    nr,
	size_t                          size,
	unsigned int                    operations,
	unsigned long long              nsecs[__restrict_arr STROLLPT_HEAP_OP_NR])
{
	return strollpt_heap_measure_array(elements,
	                                   nr,
	                                   size,
	                                   operations,
	                                   nsecs,
	                                   &strollpt_fwheap_iface);
}

#endif /* defined(CONFIG_STROLL_FWHEAP) */

#if defined(CONFIG_STROLL_PRHEAP) || defined(CONFIG_STROLL_BNHEAP)

#include "stroll/priv/lcrs.h"

struct strollpt_heap_node {
	struct stroll_lcrs_node super;
	unsigned int            id;
	const char              data[0];
};

static int
strollpt_heap_compare_min(
	const struct stroll_lcrs_node * __restrict a,
	const struct stroll_lcrs_node * __restrict b,
	void *                                     data __unused)
{
	const struct strollpt_heap_node * _a =
		stroll_lcrs_entry(a, const struct strollpt_heap_node, super);
	const struct strollpt_heap_node * _b =
		stroll_lcrs_entry(b, const struct strollpt_heap_node, super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static struct strollpt_heap_node *
strollpt_heap_alloc_nodes(unsigned int nr, size_t size)
{
	assert(nr);
	assert(size);

	return malloc(nr *
	              (sizeof_member(struct strollpt_heap_node, super) + size));
}

static struct strollpt_heap_node *
strollpt_heap_create_nodes(const unsigned int * elements,
                           unsigned int         nr,
                           size_t               size)
{
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_heap_node * nodes;
	struct strollpt_heap_node * ptr;
	unsigned int                n;

	nodes = strollpt_heap_alloc_nodes(nr, size);
	if (!nodes)
		return NULL;

	for (n = 0,
	     ptr = nodes;
	     n < nr;
	     n++,
	     ptr = (struct strollpt_heap_node *)
	           ((char *)ptr + sizeof(nodes->super) + size))
		ptr->id = elements[n];

	return nodes;
}

static void
strollpt_heap_destroy_nodes(struct strollpt_heap_node * nodes)
{
	free(nodes);
}

static int
strollpt_heap_prepare_nodes(const unsigned int * __restrict    elements,
                            unsigned int                       nr,
                            size_t                             size,
                            const struct strollpt_heap_iface * iface)
{
	assert(elements);
	assert(nr);
	assert(size);
	assert(size <= UINT_MAX);
	assert(iface);

	struct strollpt_heap_node * nodes;
	unsigned int *              sort;
	void *                      heap;
	unsigned int                e;
	int                         ret = EXIT_FAILURE;

	if (size % sizeof_member(struct strollpt_heap_node, id)) {
		strollpt_err("invalid data element size %zu specified: "
		             "multiple of %zu expected.\n",
		             size,
		             sizeof_member(struct strollpt_heap_node, id));
		return EXIT_FAILURE;
	}


	nodes = strollpt_heap_create_nodes(elements, nr, size);
	if (!nodes)
		return EXIT_FAILURE;

	sort = malloc(nr * sizeof(elements[0]));
	if (!sort)
		goto destroy_nodes;
	memcpy(sort, elements, nr * sizeof(elements[0]));
	qsort_r(sort, nr, sizeof(sort[0]), strollpt_array_compare_min, NULL);

	heap = iface->create(nodes, nr, size);
	if (!heap)
		goto free_sort;

	iface->build(heap, nodes, nr, size);
	if (!iface->validate(heap, nr)) {
		strollpt_err("Bogus heapify scheme.\n");
		goto free_heap;
	}

	for (e = 0; e < nr; e++) {
		struct stroll_lcrs_node * lcrs = (struct stroll_lcrs_node *)
		                                 0xdeadbeef;

		iface->extract(heap, &lcrs);
		if (stroll_lcrs_entry(lcrs,
		                      struct strollpt_heap_node,
		                      super)->id != sort[e]) {
			strollpt_err("Bogus heap extraction scheme.\n");
			goto free_heap;
		}

		if (iface->count(heap) != (nr - e - 1)) {
			strollpt_err("Bogus heap extraction count.\n");
			goto free_heap;
		}
	}

	memset(nodes,
	       0xa5,
	       nr * (sizeof(nodes->super) + size));
	for (e = 0; e < nr; e++) {
		struct strollpt_heap_node * node;

		node = (struct strollpt_heap_node *)
		       ((char *)nodes + (e * (sizeof(nodes->super) + size)));

		node->id = elements[e];
		iface->insert(heap, node);

		if (iface->count(heap) != (e + 1)) {
			strollpt_err("Bogus heap insertion count.\n");
			goto free_heap;
		}
	}
	if (!iface->validate(heap, nr)) {
		strollpt_err("Bogus heap insertion scheme.\n");
		goto free_heap;
	}

	ret = EXIT_SUCCESS;

free_heap:
	iface->destroy(heap);
free_sort:
	free(sort);
destroy_nodes:
	strollpt_heap_destroy_nodes(nodes);

	return ret;
}

static int
strollpt_heap_measure_node_build(
	const unsigned int * __restrict               elements,
	unsigned int                                  nr,
	size_t                                        size,
	unsigned long long * __restrict               nsecs,
	const struct strollpt_heap_iface * __restrict iface)
{
	struct timespec             start, elapse;
	struct strollpt_heap_node * nodes;
	void *                      heap;
	int                         ret = EXIT_FAILURE;

	nodes = strollpt_heap_create_nodes(elements, nr, size);
	if (!nodes)
		return EXIT_FAILURE;
	heap = iface->create(nodes, nr, size);
	if (!heap)
		goto destroy_nodes;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	iface->build(heap, nodes, nr, size);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_destroy_nodes(nodes);

	return ret;
}

static int
strollpt_heap_measure_node_insert(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned long long * __restrict    nsecs,
	const struct strollpt_heap_iface * iface)
{
	struct timespec             start, elapse;
	struct strollpt_heap_node * nodes;
	void *                      heap;
	unsigned int                e;
	int                         ret = EXIT_FAILURE;

	nodes = strollpt_heap_alloc_nodes(nr, size);
	if (!nodes)
		return EXIT_FAILURE;
	heap = iface->create(nodes, nr, size);
	if (!heap)
		goto destroy_nodes;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		struct strollpt_heap_node * node;

		node = (struct strollpt_heap_node *)
		       ((char *)nodes + (e * (sizeof(nodes->super) + size)));
		node->id = elements[e];
		iface->insert(heap, &node->super);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_destroy_nodes(nodes);

	return ret;
}

static int
strollpt_heap_measure_node_extract(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned long long * __restrict    nsecs,
	const struct strollpt_heap_iface * iface)
{
	struct timespec             start, elapse;
	struct strollpt_heap_node * nodes;
	void *                      heap;
	unsigned int                e;
	int                         ret = EXIT_FAILURE;

	nodes = strollpt_heap_create_nodes(elements, nr, size);
	if (!nodes)
		return EXIT_FAILURE;
	heap = iface->create(nodes, nr, size);
	if (!heap)
		goto destroy_nodes;
	iface->build(heap, nodes, nr, size);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		struct stroll_lcrs_node * node;

		iface->extract(heap, &node);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_destroy_nodes(nodes);

	return ret;
}

static int
strollpt_heap_measure_nodes(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned int                       operations,
	unsigned long long                 nsecs[__restrict_arr STROLLPT_HEAP_OP_NR],
	const struct strollpt_heap_iface * iface)
{
	assert(elements);
	assert(nr);
	assert(size);
	assert(operations);
	assert(nsecs);
	assert(iface);

	unsigned int op;
	int          ret = EXIT_SUCCESS;

	memset(nsecs, 0, STROLLPT_HEAP_OP_NR * sizeof(nsecs[0]));

	for (op = 0; !ret && (op < STROLLPT_HEAP_OP_NR); op++) {
		if (!(operations & (1U << op)))
			continue;

		switch (op) {
		case STROLLPT_HEAP_BUILD_OP:
			ret = strollpt_heap_measure_node_build(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_BUILD_OP],
				iface);
			break;

		case STROLLPT_HEAP_INSERT_OP:
			ret = strollpt_heap_measure_node_insert(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_INSERT_OP],
				iface);
			break;

		case STROLLPT_HEAP_EXTRACT_OP:
			ret = strollpt_heap_measure_node_extract(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_EXTRACT_OP],
				iface);
			break;

		default:
			ret = EXIT_SUCCESS;
			break;
		}
	}

	return ret;
}

#endif /* defined(CONFIG_STROLL_PRHEAP) || defined(CONFIG_STROLL_BNHEAP) */

#if defined(CONFIG_STROLL_PRHEAP)

#include "stroll/prheap.h"

static bool
strollpt_prheap_validate_rec(const struct stroll_lcrs_node * __restrict node,
                             unsigned int * __restrict                  nr)
{
	const struct stroll_lcrs_node * child;

	*nr += 1;

	stroll_lcrs_foreach_child(node, child) {
		if (strollpt_heap_compare_min(node, child, NULL) > 0)
			return false;
		strollpt_prheap_validate_rec(child, nr);
	}

	return true;
}

static bool
strollpt_prheap_validate(const void * __restrict heap, unsigned int nr)
{
	const struct stroll_prheap * hp = heap;
	unsigned int                 cnt = 0;

	if (!strollpt_prheap_validate_rec(hp->root, &cnt))
		return false;

	if ((cnt != nr) ||
	    (stroll_prheap_count(hp) != nr))
		return false;

	return true;
}

static void *
strollpt_prheap_create(void * __restrict     array __unused,
                       unsigned int          nr,
                       size_t                size __unused)
{
	return stroll_prheap_create(nr, strollpt_heap_compare_min);
}

static void
strollpt_prheap_destroy(void * __restrict heap)
{
	stroll_prheap_destroy(heap);
}

static void
strollpt_prheap_build(void * __restrict heap,
                      void * __restrict array,
                      unsigned int      nr,
                      size_t            size  __unused)
{
	unsigned int n;

	for (n = 0; n < nr; n++) {
		struct strollpt_heap_node * node;

		node = (struct strollpt_heap_node *)
		       ((char *)array + (n * (sizeof(node->super) + size)));
		stroll_prheap_insert(heap, &node->super, NULL);
	}
}

static void
strollpt_prheap_insert(void * __restrict heap, void * __restrict node)
{
	stroll_prheap_insert(heap,
	                     &((struct strollpt_heap_node *)node)->super,
	                     NULL);
}

static void
strollpt_prheap_extract(void * __restrict heap, void * __restrict node)
{
	struct stroll_lcrs_node *    lcrs;
	struct strollpt_heap_node ** ptnode = node;

	lcrs = stroll_prheap_extract(heap, NULL);

	*ptnode = stroll_lcrs_entry(lcrs, struct strollpt_heap_node, super);
}

static unsigned int
strollpt_prheap_count(void * __restrict heap)
{
	return stroll_prheap_count(heap);
}

static const struct strollpt_heap_iface strollpt_prheap_iface = {
	.create   = strollpt_prheap_create,
	.destroy  = strollpt_prheap_destroy,
	.build    = strollpt_prheap_build,
	.insert   = strollpt_prheap_insert,
	.extract  = strollpt_prheap_extract,
	.count    = strollpt_prheap_count,
	.validate = strollpt_prheap_validate
};

static int
strollpt_heap_prepare_prheap(const unsigned int * __restrict elements,
                             unsigned int                    nr,
                             size_t                          size)
{
	return strollpt_heap_prepare_nodes(elements,
	                                   nr,
	                                   size,
	                                   &strollpt_prheap_iface);
}

static int
strollpt_heap_measure_prheap(
	const unsigned int * __restrict elements,
	unsigned int                    nr,
	size_t                          size,
	unsigned int                    operations,
	unsigned long long              nsecs[__restrict_arr STROLLPT_HEAP_OP_NR])
{
	return strollpt_heap_measure_nodes(elements,
	                                   nr,
	                                   size,
	                                   operations,
	                                   nsecs,
	                                   &strollpt_prheap_iface);
}

#endif /* defined(CONFIG_STROLL_PRHEAP) */

static const struct strollpt_heap_algo strollpt_heap_algos[] = {
#if defined(CONFIG_STROLL_FBHEAP)
	{
		.name    = "fbheap",
		.prepare = strollpt_heap_prepare_fbheap,
		.measure = strollpt_heap_measure_fbheap
	},
#endif
#if defined(CONFIG_STROLL_FWHEAP)
	{
		.name    = "fwheap",
		.prepare = strollpt_heap_prepare_fwheap,
		.measure = strollpt_heap_measure_fwheap
	},
#endif
#if defined(CONFIG_STROLL_PRHEAP)
	{
		.name    = "prheap",
		.prepare = strollpt_heap_prepare_prheap,
		.measure = strollpt_heap_measure_prheap
	},
#endif
};

static int
strollpt_heap_parse_algo(const char * __restrict                       arg,
                         const struct strollpt_heap_algo ** __restrict algo)
{
	unsigned int a;

	for (a = 0; a < stroll_array_nr(strollpt_heap_algos); a++) {
		if (!strcmp(arg, strollpt_heap_algos[a].name)) {
			*algo = &strollpt_heap_algos[a];
			return EXIT_SUCCESS;
		}
	}

	strollpt_err("invalid '%s' heap algorithm.\n", arg);

	return EXIT_FAILURE;
}

static int
strollpt_heap_parse_opers(char *                    arg,
                          unsigned int * __restrict operations)
{
	*operations = 0;

	do {
		const char * name;
		unsigned int o;

		assert(arg);
		name = strsep(&arg, ",");
		assert(name);

		for (o = 0;
		     o < stroll_array_nr(strollpt_heap_operations);
		     o++) {
			if (!strcmp(name, strollpt_heap_operations[o])) {
				*operations |= 1U << o;
				break;
			}
		}

		if (o == stroll_array_nr(strollpt_heap_operations)) {
			strollpt_err("invalid '%s' operation.\n", name);
			return EXIT_FAILURE;
		}
	} while (arg);

	return EXIT_SUCCESS;
}

static int
strollpt_heap_show_stats(enum strollpt_heap_op operation,
                         unsigned long long *  nsecs,
                         unsigned int          loops)
{
	struct strollpt_stats stats;

	if (strollpt_calc_stats(&stats,
	                        &nsecs[operation],
	                        STROLLPT_HEAP_OP_NR,
	                        loops))
		return EXIT_FAILURE;

	printf("%s:\n"
	       "    #Inliers:   %u (%.2lf%%)\n"
	       "    Mininum:    %llu nSec\n"
	       "    Maximum:    %llu nSec\n"
	       "    Deviation:  %llu nSec\n"
	       "    Median:     %llu nSec\n"
	       "    Mean:       %llu nSec\n",
	       strollpt_heap_operations[operation],
	       stats.count, ((double)stats.count * 100.0) / (double)loops,
	       stats.min,
	       stats.max,
	       (unsigned long long)round(stats.stdev),
	       stats.med,
	       (unsigned long long)round(stats.mean));

	return EXIT_SUCCESS;
}

static void
strollpt_heap_usage(FILE * __restrict stdio)
{
	fprintf(stdio,
	        "Usage: %s [OPTIONS] FILE ALGORITHM SIZE LOOPS\n"
	        "where OPTIONS:\n"
	        "    -o|--oper OPERATIONS\n"
	        "    -p|--prio PRIORITY\n"
	        "    -h|--help\n",
	        program_invocation_short_name);
}

int main(int argc, char *argv[])
{
	const struct strollpt_heap_algo * algo;
	size_t                            dsize;
	unsigned int                      loops;
	int                               prio = 0;
	unsigned int                      opers = (1U << STROLLPT_HEAP_OP_NR) -
	                                          1;
	struct strollpt_data              data;
	unsigned int *                    elms;
	unsigned long long *              nsecs;
	unsigned int                      i;
	int                               ret = EXIT_FAILURE;

	while (true) {
		int                        opt;
		static const struct option lopts[] = {
			{"oper", 1, NULL, 'o'},
			{"help", 0, NULL, 'h'},
			{"prio", 1, NULL, 'p'},
			{0,      0, 0,    0}
		};

		opt = getopt_long(argc, argv, "o:hp:", lopts, NULL);
		if (opt < 0)
			/* No more options: go parsing positional arguments. */
			break;

		switch (opt) {
		case 'o': /* operation mask */
			if (strollpt_heap_parse_opers(optarg, &opers)) {
				strollpt_heap_usage(stderr);
				return EXIT_FAILURE;
			}

			break;

		case 'p': /* priority */
			if (strollpt_parse_sched_prio(optarg, &prio)) {
				strollpt_heap_usage(stderr);
				return EXIT_FAILURE;
			}

			break;

		case 'h': /* Help message. */
			strollpt_heap_usage(stdout);
			exit(EXIT_SUCCESS);

		case '?': /* Unknown option. */
		default:
			strollpt_heap_usage(stderr);
			return EXIT_FAILURE;
		}
	}

	/*
	 * Check positional arguments are properly specified on command
	 * line.
	 */
	argc -= optind;
	if (argc != 4) {
		strollpt_err("invalid number of arguments.\n");
		strollpt_heap_usage(stderr);
		return EXIT_FAILURE;
	}

	if (strollpt_heap_parse_algo(argv[optind + 1], &algo))
		return EXIT_FAILURE;

	if (strollpt_parse_data_size(argv[optind + 2], &dsize))
		return EXIT_FAILURE;

	if (strollpt_parse_loop_nr(argv[optind + 3], &loops))
		return EXIT_FAILURE;

	elms = strollpt_load_data(&data, argv[optind]);
	if (!elms)
		return EXIT_FAILURE;

	if (algo->prepare(elms, data.nr, dsize))
		goto free_data;

	nsecs = malloc(STROLLPT_HEAP_OP_NR * loops * sizeof(nsecs[0]));
	if (!nsecs)
		goto free_data;

	if (strollpt_setup_sched_prio(prio))
		goto free_nsecs;

	for (i = 0; i < loops; i++) {
		if (algo->measure(elms,
		                  data.nr,
		                  dsize,
		                  opers,
		                  &nsecs[i * STROLLPT_HEAP_OP_NR]))
		    goto free_nsecs;
	}

	printf("#Samples:       %u\n"
	       "Order ratio:    %hu\n"
	       "Distinct ratio: %hu\n"
	       "Algorithm:      %s\n"
	       "Data size:      %zu\n"
	       "#Loops:         %u\n",
	       data.nr,
	       data.order,
	       data.singles,
	       algo->name,
	       dsize,
	       loops);
	for (i = 0; i < STROLLPT_HEAP_OP_NR; i++) {
		if (opers & (1U << i))
			if (strollpt_heap_show_stats(i, nsecs, loops))
				goto free_nsecs;
	}

	ret = EXIT_SUCCESS;

free_nsecs:
	free(nsecs);
free_data:
	free(elms);

	return ret;
}
