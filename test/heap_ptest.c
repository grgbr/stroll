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

struct strollpt_heap_store {
	unsigned int nr;
	size_t       size;
	char *       nodes;
};

typedef int
        (strollpt_heap_create_nodes_fn)(struct strollpt_heap_store * __restrict,
                                        const unsigned int * __restrict,
                                        unsigned int,
                                        size_t);

typedef void (strollpt_heap_build_fn)(void * __restrict,
                                      void * __restrict,
                                      unsigned int,
                                      size_t)
	__stroll_nonull(1);

typedef void (strollpt_heap_insert_fn)(void * __restrict, void * __restrict)
	__stroll_nonull(1, 2);

typedef void (strollpt_heap_extract_fn)(void * __restrict, void * __restrict)
	__stroll_nonull(1, 2);

typedef void (strollpt_heap_remove_fn)(void * __restrict, void * __restrict)
	__stroll_nonull(1, 2);

typedef void (strollpt_heap_promote_fn)(void * __restrict, void * __restrict)
	__stroll_nonull(1, 2);

typedef void (strollpt_heap_demote_fn)(void * __restrict, void * __restrict)
	__stroll_nonull(1, 2);

typedef unsigned int (strollpt_heap_count_fn)(void * __restrict)
	__stroll_nonull(1);

typedef bool (strollpt_heap_validate_fn)(const void * __restrict , unsigned int)
	__stroll_nonull(1);

struct strollpt_heap_iface {
	strollpt_heap_create_fn *       create;
	strollpt_heap_destroy_fn *      destroy;
	strollpt_heap_create_nodes_fn * create_nodes;
	strollpt_heap_build_fn *        build;
	strollpt_heap_insert_fn *       insert;
	strollpt_heap_extract_fn *      extract;
	strollpt_heap_remove_fn *       remove;
	strollpt_heap_promote_fn *      promote;
	strollpt_heap_demote_fn *       demote;
	strollpt_heap_count_fn *        count;
	strollpt_heap_validate_fn *     validate;
};

enum strollpt_heap_op {
	STROLLPT_HEAP_BUILD_OP   = 0,
	STROLLPT_HEAP_INSERT_OP  = 1,
	STROLLPT_HEAP_EXTRACT_OP = 2,
	STROLLPT_HEAP_REMOVE_OP  = 3,
	STROLLPT_HEAP_PROMOTE_OP = 4,
	STROLLPT_HEAP_DEMOTE_OP  = 5,
	STROLLPT_HEAP_OP_NR
};

static const char * strollpt_heap_operations[] = {
	[STROLLPT_HEAP_BUILD_OP]   = "heapify",
	[STROLLPT_HEAP_INSERT_OP]  = "insert",
	[STROLLPT_HEAP_EXTRACT_OP] = "extract",
	[STROLLPT_HEAP_REMOVE_OP]  = "remove",
	[STROLLPT_HEAP_PROMOTE_OP] = "promote",
	[STROLLPT_HEAP_DEMOTE_OP]  = "demote"
};

#if defined(CONFIG_STROLL_FBHEAP) || defined(CONFIG_STROLL_FWHEAP)

struct strollpt_heap_elem {
	int        id;
	const char data[0];
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
		ptr->id = (int)from[n];
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
		if (iface->count(heap) != (nr - e - 1)) {
			strollpt_err("Bogus heap extraction count.\n");
			goto free_heap;
		}

		if (((struct strollpt_heap_elem *)elm)->id != (int)sort[e]) {
			strollpt_err("Bogus heap extraction key.\n");
			goto free_heap;
		}

		if (!iface->validate(heap, nr - e - 1)) {
			strollpt_err("Bogus heap extraction scheme.\n");
			goto free_heap;
		}
	}

	memset(elms, 0xa5, nr * size);
	for (e = 0; e < nr; e++) {
		char elm[size];

		memset(elm, 0, size);

		((struct strollpt_heap_elem *)elm)->id = (int)elements[e];
		iface->insert(heap, elm);

		if (iface->count(heap) != (e + 1)) {
			strollpt_err("Bogus heap insertion count.\n");
			goto free_heap;
		}

		if (!iface->validate(heap, e + 1)) {
			strollpt_err("Bogus heap insertion scheme.\n");
			goto free_heap;
		}
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

		((struct strollpt_heap_elem *)elm)->id = (int)elements[e];
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
	if (!nr)
		return true;
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
	if (!nr)
		return true;
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

#if defined(CONFIG_STROLL_HPRHEAP) || \
    defined(CONFIG_STROLL_DRPHEAP) || \
    defined(CONFIG_STROLL_DPRHEAP) || \
    defined(CONFIG_STROLL_PPRHEAP) || \
    defined(CONFIG_STROLL_DBNHEAP)

#define strollpt_heap_foreach_node(_store, _index, _curr) \
	for (_index = 0, _curr = strollpt_heap_store_get(_store, _index); \
	     strollpt_heap_store_valid(_store, _index); \
	     _index++, _curr = strollpt_heap_store_get(_store, _index))

static inline void *
strollpt_heap_store_nodes(const struct strollpt_heap_store * __restrict store)
{
	return store->nodes;
}

static inline bool
strollpt_heap_store_valid(struct strollpt_heap_store * __restrict store,
                          unsigned int                            index)
{
	return index < store->nr;
}

static inline void *
strollpt_heap_store_get(const struct strollpt_heap_store * __restrict store,
                        unsigned int                                  index)
{
	return &store->nodes[index * store->size];
}

static void
strollpt_heap_store_corrupt(const struct strollpt_heap_store * __restrict store)
{
	memset(store->nodes, 0xa5, store->nr * store->size);
}

static int
strollpt_heap_store_init(struct strollpt_heap_store * __restrict store,
                         const unsigned int * __restrict         elements,
                         unsigned int                            nr,
                         size_t                                  size)
{
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	unsigned int n;

	if ((size < sizeof(elements[0])) || (size % sizeof(elements[0]))) {
		strollpt_err("invalid data element size %zu specified: "
		             "multiple of %zu expected.\n",
		             size,
		             sizeof(elements[0]));
		return EXIT_FAILURE;
	}

	store->nodes = malloc(nr * size);
	if (!store)
		return EXIT_FAILURE;

	store->nr = nr;
	store->size = size;

	for (n = 0; n < nr; n++) {
		int * node = strollpt_heap_store_get(store, n);

		*node = (int)elements[n];
	}

	return EXIT_SUCCESS;
}

static void
strollpt_heap_store_fini(struct strollpt_heap_store * __restrict store)
{
	free(store->nodes);
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

	struct strollpt_heap_store store;
	int *                      ptr;
	unsigned int *             sort;
	void *                     heap;
	unsigned int               e;
	int                        ret = EXIT_FAILURE;

	if (iface->create_nodes(&store, elements, nr, size))
		return EXIT_FAILURE;

	sort = malloc(nr * sizeof(elements[0]));
	if (!sort)
		goto destroy_nodes;
	memcpy(sort, elements, nr * sizeof(elements[0]));
	qsort_r(sort, nr, sizeof(sort[0]), strollpt_array_compare_min, NULL);

	heap = iface->create(strollpt_heap_store_nodes(&store), nr, size);
	if (!heap)
		goto free_sort;

	iface->build(heap, strollpt_heap_store_nodes(&store), nr, size);
	if (!iface->validate(heap, nr)) {
		strollpt_err("Bogus heapify scheme.\n");
		goto free_heap;
	}

	for (e = 0; e < nr; e++) {
		iface->extract(heap, &ptr);

		if (iface->count(heap) != (nr - e - 1)) {
			strollpt_err("Bogus heap extraction count.\n");
			goto free_heap;
		}

		if (*ptr != (int)sort[e]) {
			strollpt_err("Bogus heap extraction key.\n");
			goto free_heap;
		}

		if (!iface->validate(heap, nr - e - 1)) {
			strollpt_err("Bogus heap extraction scheme.\n");
			goto free_heap;
		}
	}

	strollpt_heap_store_corrupt(&store);
	strollpt_heap_foreach_node(&store, e, ptr) {
		*ptr = (int)elements[e];
		iface->insert(heap, ptr);

		if (iface->count(heap) != (e + 1)) {
			strollpt_err("Bogus heap insertion count.\n");
			goto free_heap;
		}

		if (!iface->validate(heap, e + 1)) {
			strollpt_err("Bogus heap insertion scheme.\n");
			goto free_heap;
		}
	}

	if (iface->promote) {
		strollpt_heap_foreach_node(&store, e, ptr) {
			*ptr -= (int)nr / 4;
			iface->promote(heap, ptr);

			if (iface->count(heap) != nr) {
				strollpt_err("Bogus heap promote count.\n");
				goto free_heap;
			}

			if (!iface->validate(heap, nr)) {
				strollpt_err("Bogus heap promote scheme.\n");
				goto free_heap;
			}
		}
	}

	if (iface->demote) {
		strollpt_heap_foreach_node(&store, e, ptr) {
			*ptr += (int)nr / 2;
			iface->demote(heap, ptr);

			if (iface->count(heap) != nr) {
				strollpt_err("Bogus heap demote count.\n");
				goto free_heap;
			}

			if (!iface->validate(heap, nr)) {
				strollpt_err("Bogus heap demote scheme.\n");
				goto free_heap;
			}
		}
	}

	if (iface->remove) {
		strollpt_heap_foreach_node(&store, e, ptr) {
			iface->remove(heap, ptr);

			if (iface->count(heap) != (nr - e - 1)) {
				strollpt_err("Bogus heap removal count.\n");
				goto free_heap;
			}

			if (!iface->validate(heap, nr - e - 1)) {
				strollpt_err("Bogus heap removal scheme.\n");
				goto free_heap;
			}
		}
	}

	ret = EXIT_SUCCESS;

free_heap:
	iface->destroy(heap);
free_sort:
	free(sort);
destroy_nodes:
	strollpt_heap_store_fini(&store);

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
	struct strollpt_heap_store store;
	void *                     heap;
	struct timespec            start, elapse;
	int                        ret = EXIT_FAILURE;

	if (iface->create_nodes(&store, elements, nr, size))
		return EXIT_FAILURE;

	heap = iface->create(strollpt_heap_store_nodes(&store), nr, size);
	if (!heap)
		goto destroy_nodes;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	iface->build(heap, strollpt_heap_store_nodes(&store), nr, size);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_store_fini(&store);

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
	struct timespec            start, elapse;
	struct strollpt_heap_store store;
	int *                      ptr;
	void *                     heap;
	unsigned int               e;
	int                        ret = EXIT_FAILURE;

	if (iface->create_nodes(&store, elements, nr, size))
		return EXIT_FAILURE;
	heap = iface->create(strollpt_heap_store_nodes(&store), nr, size);
	if (!heap)
		goto destroy_nodes;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	strollpt_heap_foreach_node(&store, e, ptr)
		iface->insert(heap, ptr);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_store_fini(&store);

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
	struct timespec            start, elapse;
	struct strollpt_heap_store store;
	void *                     heap;
	unsigned int               e;
	int                        ret = EXIT_FAILURE;

	if (iface->create_nodes(&store, elements, nr, size))
		return EXIT_FAILURE;
	heap = iface->create(strollpt_heap_store_nodes(&store), nr, size);
	if (!heap)
		goto destroy_nodes;
	iface->build(heap, strollpt_heap_store_nodes(&store), nr, size);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		void * node;

		iface->extract(heap, &node);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_store_fini(&store);

	return ret;
}

static int
strollpt_heap_measure_node_remove(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned long long * __restrict    nsecs,
	const struct strollpt_heap_iface * iface)
{
	struct timespec            start, elapse;
	struct strollpt_heap_store store;
	int *                      ptr;
	void *                     heap;
	unsigned int               e;
	int                        ret = EXIT_FAILURE;

	if (iface->create_nodes(&store, elements, nr, size))
		return EXIT_FAILURE;
	heap = iface->create(strollpt_heap_store_nodes(&store), nr, size);
	if (!heap)
		goto destroy_nodes;
	iface->build(heap, strollpt_heap_store_nodes(&store), nr, size);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	strollpt_heap_foreach_node(&store, e, ptr)
		iface->remove(heap, ptr);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_store_fini(&store);

	return ret;
}

static int
strollpt_heap_measure_node_promote(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned long long * __restrict    nsecs,
	const struct strollpt_heap_iface * iface)
{
	struct timespec            start, elapse;
	struct strollpt_heap_store store;
	int *                      ptr;
	void *                     heap;
	unsigned int               e;
	int                        ret = EXIT_FAILURE;

	if (iface->create_nodes(&store, elements, nr, size))
		return EXIT_FAILURE;
	heap = iface->create(strollpt_heap_store_nodes(&store), nr, size);
	if (!heap)
		goto destroy_nodes;
	iface->build(heap, strollpt_heap_store_nodes(&store), nr, size);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	strollpt_heap_foreach_node(&store, e, ptr) {
		*ptr -= (int)nr / 4;
		iface->promote(heap, ptr);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_store_fini(&store);

	return ret;
}

static int
strollpt_heap_measure_node_demote(
	const unsigned int * __restrict    elements,
	unsigned int                       nr,
	size_t                             size,
	unsigned long long * __restrict    nsecs,
	const struct strollpt_heap_iface * iface)
{
	struct timespec            start, elapse;
	struct strollpt_heap_store store;
	int *                      ptr;
	void *                     heap;
	unsigned int               e;
	int                        ret = EXIT_FAILURE;

	if (iface->create_nodes(&store, elements, nr, size))
		return EXIT_FAILURE;
	heap = iface->create(strollpt_heap_store_nodes(&store), nr, size);
	if (!heap)
		goto destroy_nodes;
	iface->build(heap, strollpt_heap_store_nodes(&store), nr, size);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	strollpt_heap_foreach_node(&store, e, ptr) {
		*ptr += (int)nr / 4;
		iface->demote(heap, ptr);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	ret = EXIT_SUCCESS;

	iface->destroy(heap);

destroy_nodes:
	strollpt_heap_store_fini(&store);

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

		case STROLLPT_HEAP_REMOVE_OP:
			ret = strollpt_heap_measure_node_remove(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_REMOVE_OP],
				iface);
			break;

		case STROLLPT_HEAP_PROMOTE_OP:
			ret = strollpt_heap_measure_node_promote(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_PROMOTE_OP],
				iface);
			break;

		case STROLLPT_HEAP_DEMOTE_OP:
			ret = strollpt_heap_measure_node_demote(
				elements,
				nr,
				size,
				&nsecs[STROLLPT_HEAP_DEMOTE_OP],
				iface);
			break;

		default:
			ret = EXIT_SUCCESS;
			break;
		}
	}

	return ret;
}

#endif /* defined(CONFIG_STROLL_HPRHEAP) || \
          defined(CONFIG_STROLL_DRPHEAP) || \
          defined(CONFIG_STROLL_DPRHEAP) || \
          defined(CONFIG_STROLL_PPRHEAP) || \
          defined(CONFIG_STROLL_DBNHEAP) */

#if defined(CONFIG_STROLL_HPRHEAP)

#include "stroll/hprheap.h"

struct strollpt_hprheap_node {
	int                        id;
	struct stroll_hprheap_node super;
	const char                 data[0];
};

#define strollpt_hprheap_node_size(_size) \
	(sizeof(struct strollpt_hprheap_node) + (_size) - sizeof(int))

static int
strollpt_hprheap_compare_min(
	const struct stroll_hprheap_node * __restrict a,
	const struct stroll_hprheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollpt_hprheap_node * _a =
		stroll_hprheap_entry(a,
		                     const struct strollpt_hprheap_node,
		                     super);
	const struct strollpt_hprheap_node * _b =
		stroll_hprheap_entry(b,
		                     const struct strollpt_hprheap_node,
		                     super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static int
strollpt_hprheap_create_nodes(struct strollpt_heap_store * __restrict store,
                              const unsigned int * __restrict         elements,
                              unsigned int                            nr,
                              size_t                                  size)
{
	return strollpt_heap_store_init(store,
	                                elements,
	                                nr,
	                                strollpt_hprheap_node_size(size));
}

static bool
strollpt_hprheap_validate_rec(const struct stroll_hprheap_node * __restrict node,
                              unsigned int * __restrict                     nr)
{
	const struct stroll_hprheap_node * child;

	*nr += 1;

	for (child = node->children[0]; child; child = child->children[1]) {
		if (strollpt_hprheap_compare_min(node, child, NULL) > 0)
			return false;
		if (!strollpt_hprheap_validate_rec(child, nr))
			return false;
	}

	return true;
}

static bool
strollpt_hprheap_validate(const void * __restrict heap, unsigned int nr)
{
	const struct stroll_hprheap * hp = heap;
	unsigned int                  cnt = 0;

	if (!nr)
		return hp->base.root == NULL;

	if (!strollpt_hprheap_validate_rec(hp->base.root, &cnt))
		return false;

	if ((cnt != nr) || (stroll_hprheap_count(hp) != nr))
		return false;

	return true;
}

static void *
strollpt_hprheap_create(void * __restrict array __unused,
                        unsigned int      nr,
                        size_t            size __unused)
{
	return stroll_hprheap_create(nr, strollpt_hprheap_compare_min);
}

static void
strollpt_hprheap_destroy(void * __restrict heap)
{
	stroll_hprheap_destroy(heap);
}

static void
strollpt_hprheap_build(void * __restrict heap,
                       void * __restrict array,
                       unsigned int      nr,
                       size_t            size)
{
	unsigned int n;

	for (n = 0; n < nr; n++) {
		struct strollpt_hprheap_node * ptr;

		ptr = (struct strollpt_hprheap_node *)
		      ((char *)array + (n * strollpt_hprheap_node_size(size)));
		stroll_hprheap_insert(heap, &ptr->super, NULL);
	}
}

static void
strollpt_hprheap_insert(void * __restrict heap, void * __restrict node)
{
	stroll_hprheap_insert(heap,
	                      &((struct strollpt_hprheap_node *)node)->super,
	                      NULL);
}

static void
strollpt_hprheap_extract(void * __restrict heap, void * __restrict node)
{
	struct strollpt_hprheap_node ** ptnode = node;

	*ptnode = stroll_hprheap_entry(stroll_hprheap_extract(heap, NULL),
	                               struct strollpt_hprheap_node,
	                               super);
}

static unsigned int
strollpt_hprheap_count(void * __restrict heap)
{
	return stroll_hprheap_count(heap);
}

static const struct strollpt_heap_iface strollpt_hprheap_iface = {
	.create       = strollpt_hprheap_create,
	.destroy      = strollpt_hprheap_destroy,
	.create_nodes = strollpt_hprheap_create_nodes,
	.build        = strollpt_hprheap_build,
	.insert       = strollpt_hprheap_insert,
	.extract      = strollpt_hprheap_extract,
	.count        = strollpt_hprheap_count,
	.validate     = strollpt_hprheap_validate
};

static int
strollpt_heap_prepare_hprheap(const unsigned int * __restrict elements,
                              unsigned int                    nr,
                              size_t                          size)
{
	return strollpt_heap_prepare_nodes(elements,
	                                   nr,
	                                   size,
	                                   &strollpt_hprheap_iface);
}

static int
strollpt_heap_measure_hprheap(
	const unsigned int * __restrict elements,
	unsigned int                    nr,
	size_t                          size,
	unsigned int                    operations,
	unsigned long long              nsecs[__restrict_arr STROLLPT_HEAP_OP_NR])
{
	operations &= ~((1U << STROLLPT_HEAP_REMOVE_OP) |
	                (1U << STROLLPT_HEAP_PROMOTE_OP) |
	                (1U << STROLLPT_HEAP_DEMOTE_OP));
	return strollpt_heap_measure_nodes(elements,
	                                   nr,
	                                   size,
	                                   operations,
	                                   nsecs,
	                                   &strollpt_hprheap_iface);
}

#endif /* defined(CONFIG_STROLL_HPRHEAP) */

#if defined(CONFIG_STROLL_DRPHEAP)

#include "stroll/drpheap.h"

struct strollpt_drpheap_node {
	int                        id;
	struct stroll_drpheap_node super;
	const char                 data[0];
};

#define strollpt_drpheap_node_size(_size) \
	(sizeof(struct strollpt_drpheap_node) + (_size) - sizeof(int))

static int
strollpt_drpheap_compare_min(
	const struct stroll_drpheap_node * __restrict a,
	const struct stroll_drpheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollpt_drpheap_node * _a =
		stroll_drpheap_entry(a,
		                     const struct strollpt_drpheap_node,
		                     super);
	const struct strollpt_drpheap_node * _b =
		stroll_drpheap_entry(b,
		                     const struct strollpt_drpheap_node,
		                     super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static int
strollpt_drpheap_create_nodes(struct strollpt_heap_store * __restrict store,
                              const unsigned int * __restrict         elements,
                              unsigned int                            nr,
                              size_t                                  size)
{
	return strollpt_heap_store_init(store,
	                                elements,
	                                nr,
	                                strollpt_drpheap_node_size(size));
}

static bool
strollpt_drpheap_validate_rec(
	const struct stroll_drpheap_link * __restrict link,
	unsigned int * __restrict                     nr)
{
	const struct stroll_drpheap_node * node;
	const struct stroll_drpheap_link * parent;

	*nr += 1;

	node = stroll_drpheap_node_from_link(link);
	if (node->child == stroll_drpheap_tail)
		return true;

	for (parent = link, link = node->child;
	     link != stroll_drpheap_tail;
	     parent = link, link = link->edges[0]) {
		const struct stroll_drpheap_node * child;

		child = stroll_drpheap_node_from_link(link);
		if (link->edges[1] != parent)
			return false;
		if (node->rank < child->rank)
			return false;
		if (strollpt_drpheap_compare_min(node, child, NULL) > 0)
			return false;
		if (!strollpt_drpheap_validate_rec(&child->link, nr))
			return false;
	}

	return true;
}

static bool
strollpt_drpheap_validate(const void * __restrict heap, unsigned int nr)
{
	const struct stroll_drpheap *      hp = heap;
	const struct stroll_drpheap_link * rlink;
	unsigned int                       cnt = 0;

	if (!nr)
		return hp->base.roots.edges[0] == &hp->base.roots;

	for (rlink = hp->base.roots.edges[0];
	     rlink != &hp->base.roots;
	     rlink = rlink->edges[0]) {
		const struct stroll_drpheap_node * rnode;

		if (rlink->edges[0]->edges[1] !=
		    stroll_drpheap_encode_root_link(rlink))
			return false;

		rnode = stroll_drpheap_node_from_link(rlink);
		if (rnode->rank !=
		    (stroll_drpheap_node_from_link(rnode->child)->rank + 1))
			return false;

		if (!strollpt_drpheap_validate_rec(rlink, &cnt))
			return false;
	}

	if ((cnt != nr) || (stroll_drpheap_count(hp) != nr))
		return false;

	return true;
}

static void *
strollpt_drpheap_create(void * __restrict array __unused,
                        unsigned int      nr,
                        size_t            size __unused)
{
	return stroll_drpheap_create(nr, strollpt_drpheap_compare_min);
}

static void
strollpt_drpheap_destroy(void * __restrict heap)
{
	stroll_drpheap_destroy(heap);
}

static void
strollpt_drpheap_build(void * __restrict heap,
                       void * __restrict array,
                       unsigned int      nr,
                       size_t            size)
{
	unsigned int n;

	for (n = 0; n < nr; n++) {
		struct strollpt_drpheap_node * ptr;

		ptr = (struct strollpt_drpheap_node *)
		      ((char *)array + (n * strollpt_drpheap_node_size(size)));
		stroll_drpheap_insert(heap, &ptr->super, NULL);
	}
}

static void
strollpt_drpheap_insert(void * __restrict heap, void * __restrict node)
{
	stroll_drpheap_insert(heap,
	                      &((struct strollpt_drpheap_node *)node)->super,
	                      NULL);
}

static void
strollpt_drpheap_extract(void * __restrict heap, void * __restrict node)
{
	struct strollpt_drpheap_node ** ptnode = node;

	*ptnode = stroll_drpheap_entry(stroll_drpheap_extract(heap, NULL),
	                               struct strollpt_drpheap_node,
	                               super);
}

static void
strollpt_drpheap_remove(void * __restrict heap, void * __restrict node)
{
	struct strollpt_drpheap_node * ptnode = node;

	stroll_drpheap_remove(heap, &ptnode->super, NULL);
}

static void
strollpt_drpheap_promote(void * __restrict heap, void * __restrict node)
{
	struct strollpt_drpheap_node * ptnode = node;

	stroll_drpheap_promote(heap, &ptnode->super, NULL);
}

static void
strollpt_drpheap_demote(void * __restrict heap, void * __restrict node)
{
	struct strollpt_drpheap_node * ptnode = node;

	stroll_drpheap_demote(heap, &ptnode->super, NULL);
}

static unsigned int
strollpt_drpheap_count(void * __restrict heap)
{
	return stroll_drpheap_count(heap);
}

static const struct strollpt_heap_iface strollpt_drpheap_iface = {
	.create       = strollpt_drpheap_create,
	.destroy      = strollpt_drpheap_destroy,
	.create_nodes = strollpt_drpheap_create_nodes,
	.build        = strollpt_drpheap_build,
	.insert       = strollpt_drpheap_insert,
	.extract      = strollpt_drpheap_extract,
	.remove       = strollpt_drpheap_remove,
	.promote      = strollpt_drpheap_promote,
	.demote       = strollpt_drpheap_demote,
	.count        = strollpt_drpheap_count,
	.validate     = strollpt_drpheap_validate
};

static int
strollpt_heap_prepare_drpheap(const unsigned int * __restrict elements,
                              unsigned int                    nr,
                              size_t                          size)
{
	return strollpt_heap_prepare_nodes(elements,
	                                   nr,
	                                   size,
	                                   &strollpt_drpheap_iface);
}

static int
strollpt_heap_measure_drpheap(
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
	                                   &strollpt_drpheap_iface);
}

#endif /* defined(CONFIG_STROLL_DRPHEAP) */

#if defined(CONFIG_STROLL_DPRHEAP)

#include "stroll/dprheap.h"

struct strollpt_dprheap_node {
	int                        id;
	struct stroll_dprheap_node super;
	const char                 data[0];
};

#define strollpt_dprheap_node_size(_size) \
	(sizeof(struct strollpt_dprheap_node) + (_size) - sizeof(int))

static int
strollpt_dprheap_compare_min(
	const struct stroll_dprheap_node * __restrict a,
	const struct stroll_dprheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollpt_dprheap_node * _a =
		stroll_dprheap_entry(a,
		                     const struct strollpt_dprheap_node,
		                     super);
	const struct strollpt_dprheap_node * _b =
		stroll_dprheap_entry(b,
		                     const struct strollpt_dprheap_node,
		                     super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static int
strollpt_dprheap_create_nodes(struct strollpt_heap_store * __restrict store,
                              const unsigned int * __restrict         elements,
                              unsigned int                            nr,
                              size_t                                  size)
{
	return strollpt_heap_store_init(store,
	                                elements,
	                                nr,
	                                strollpt_dprheap_node_size(size));
}

static bool
strollpt_dprheap_validate_rec(const struct stroll_dprheap_node * __restrict node,
                              unsigned int * __restrict                     nr)
{
	const struct stroll_dprheap_node * child;

	*nr += 1;

	stroll_dlist_foreach_entry(&node->children, child, siblings) {
		if (child->parent != node)
			return false;
		if (strollpt_dprheap_compare_min(node, child, NULL) > 0)
			return false;
		if (!strollpt_dprheap_validate_rec(child, nr))
			return false;
	}

	return true;
}

static bool
strollpt_dprheap_validate(const void * __restrict heap, unsigned int nr)
{
	const struct stroll_dprheap * hp = heap;
	unsigned int                  cnt = 0;

	if (!nr)
		return hp->base.root == NULL;

	if (hp->base.root->parent != NULL)
		return false;
	if (!strollpt_dprheap_validate_rec(hp->base.root, &cnt))
		return false;

	if ((cnt != nr) || (stroll_dprheap_count(hp) != nr))
		return false;

	return true;
}

static void *
strollpt_dprheap_create(void * __restrict array __unused,
                        unsigned int      nr,
                        size_t            size __unused)
{
	return stroll_dprheap_create(nr, strollpt_dprheap_compare_min);
}

static void
strollpt_dprheap_destroy(void * __restrict heap)
{
	stroll_dprheap_destroy(heap);
}

static void
strollpt_dprheap_build(void * __restrict heap,
                       void * __restrict array,
                       unsigned int      nr,
                       size_t            size)
{
	unsigned int n;

	for (n = 0; n < nr; n++) {
		struct strollpt_dprheap_node * ptr;

		ptr = (struct strollpt_dprheap_node *)
		      ((char *)array + (n * strollpt_dprheap_node_size(size)));
		stroll_dprheap_insert(heap, &ptr->super, NULL);
	}
}

static void
strollpt_dprheap_insert(void * __restrict heap, void * __restrict node)
{
	stroll_dprheap_insert(heap,
	                      &((struct strollpt_dprheap_node *)node)->super,
	                      NULL);
}

static void
strollpt_dprheap_extract(void * __restrict heap, void * __restrict node)
{
	struct strollpt_dprheap_node ** ptnode = node;

	*ptnode = stroll_dprheap_entry(stroll_dprheap_extract(heap, NULL),
	                               struct strollpt_dprheap_node,
	                               super);
}

static void
strollpt_dprheap_remove(void * __restrict heap, void * __restrict node)
{
	struct strollpt_dprheap_node * ptnode = node;

	stroll_dprheap_remove(heap, &ptnode->super, NULL);
}

static void
strollpt_dprheap_promote(void * __restrict heap, void * __restrict node)
{
	struct strollpt_dprheap_node * ptnode = node;

	stroll_dprheap_promote(heap, &ptnode->super, NULL);
}

static void
strollpt_dprheap_demote(void * __restrict heap, void * __restrict node)
{
	struct strollpt_dprheap_node * ptnode = node;

	stroll_dprheap_demote(heap, &ptnode->super, NULL);
}

static unsigned int
strollpt_dprheap_count(void * __restrict heap)
{
	return stroll_dprheap_count(heap);
}

static const struct strollpt_heap_iface strollpt_dprheap_iface = {
	.create       = strollpt_dprheap_create,
	.destroy      = strollpt_dprheap_destroy,
	.create_nodes = strollpt_dprheap_create_nodes,
	.build        = strollpt_dprheap_build,
	.insert       = strollpt_dprheap_insert,
	.extract      = strollpt_dprheap_extract,
	.remove       = strollpt_dprheap_remove,
	.promote      = strollpt_dprheap_promote,
	.demote       = strollpt_dprheap_demote,
	.count        = strollpt_dprheap_count,
	.validate     = strollpt_dprheap_validate
};

static int
strollpt_heap_prepare_dprheap(const unsigned int * __restrict elements,
                              unsigned int                    nr,
                              size_t                          size)
{
	return strollpt_heap_prepare_nodes(elements,
	                                   nr,
	                                   size,
	                                   &strollpt_dprheap_iface);
}

static int
strollpt_heap_measure_dprheap(
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
	                                   &strollpt_dprheap_iface);
}

#endif /* defined(CONFIG_STROLL_DPRHEAP) */



#if defined(CONFIG_STROLL_PPRHEAP)

#include "stroll/pprheap.h"

struct strollpt_pprheap_node {
	int                        id;
	struct stroll_pprheap_node super;
	const char                 data[0];
};

#define strollpt_pprheap_node_size(_size) \
	(sizeof(struct strollpt_pprheap_node) + (_size) - sizeof(int))

static int
strollpt_pprheap_compare_min(
	const struct stroll_pprheap_node * __restrict a,
	const struct stroll_pprheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollpt_pprheap_node * _a =
		stroll_pprheap_entry(a,
		                     const struct strollpt_pprheap_node,
		                     super);
	const struct strollpt_pprheap_node * _b =
		stroll_pprheap_entry(b,
		                     const struct strollpt_pprheap_node,
		                     super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static int
strollpt_pprheap_create_nodes(struct strollpt_heap_store * __restrict store,
                              const unsigned int * __restrict         elements,
                              unsigned int                            nr,
                              size_t                                  size)
{
	return strollpt_heap_store_init(store,
	                                elements,
	                                nr,
	                                strollpt_pprheap_node_size(size));
}

static bool
strollpt_pprheap_validate_rec(const struct stroll_pprheap_node * __restrict node,
                              unsigned int * __restrict                     nr)
{
	const struct stroll_pprheap_node * child;
	const struct stroll_pprheap_node * parent = node;

	*nr += 1;

	for (child = node->children[0];
	     child != stroll_pprheap_tail;
	     child = child->children[1]) {
		if (child->parent != parent)
			return false;
		if (strollpt_pprheap_compare_min(node, child, NULL) > 0)
			return false;
		if (!strollpt_pprheap_validate_rec(child, nr))
			return false;
		parent = child;
	}

	return true;
}

static bool
strollpt_pprheap_validate(const void * __restrict heap, unsigned int nr)
{
	const struct stroll_pprheap * hp = heap;
	unsigned int                  cnt = 0;

	if (!nr)
		return hp->base.root == NULL;

	if (hp->base.root->parent != NULL)
		return false;
	if (!strollpt_pprheap_validate_rec(hp->base.root, &cnt))
		return false;

	if ((cnt != nr) || (stroll_pprheap_count(hp) != nr))
		return false;

	return true;
}

static void *
strollpt_pprheap_create(void * __restrict array __unused,
                        unsigned int      nr,
                        size_t            size __unused)
{
	return stroll_pprheap_create(nr, strollpt_pprheap_compare_min);
}

static void
strollpt_pprheap_destroy(void * __restrict heap)
{
	stroll_pprheap_destroy(heap);
}

static void
strollpt_pprheap_build(void * __restrict heap,
                       void * __restrict array,
                       unsigned int      nr,
                       size_t            size)
{
	unsigned int n;

	for (n = 0; n < nr; n++) {
		struct strollpt_pprheap_node * ptr;

		ptr = (struct strollpt_pprheap_node *)
		      ((char *)array + (n * strollpt_pprheap_node_size(size)));
		stroll_pprheap_insert(heap, &ptr->super, NULL);
	}
}

static void
strollpt_pprheap_insert(void * __restrict heap, void * __restrict node)
{
	stroll_pprheap_insert(heap,
	                      &((struct strollpt_pprheap_node *)node)->super,
	                      NULL);
}

static void
strollpt_pprheap_extract(void * __restrict heap, void * __restrict node)
{
	struct strollpt_pprheap_node ** ptnode = node;

	*ptnode = stroll_pprheap_entry(stroll_pprheap_extract(heap, NULL),
	                               struct strollpt_pprheap_node,
	                               super);
}

static void
strollpt_pprheap_remove(void * __restrict heap, void * __restrict node)
{
	struct strollpt_pprheap_node * ptnode = node;

	stroll_pprheap_remove(heap, &ptnode->super, NULL);
}

static void
strollpt_pprheap_promote(void * __restrict heap, void * __restrict node)
{
	struct strollpt_pprheap_node * ptnode = node;

	stroll_pprheap_promote(heap, &ptnode->super, NULL);
}

static void
strollpt_pprheap_demote(void * __restrict heap, void * __restrict node)
{
	struct strollpt_pprheap_node * ptnode = node;

	stroll_pprheap_demote(heap, &ptnode->super, NULL);
}

static unsigned int
strollpt_pprheap_count(void * __restrict heap)
{
	return stroll_pprheap_count(heap);
}

static const struct strollpt_heap_iface strollpt_pprheap_iface = {
	.create       = strollpt_pprheap_create,
	.destroy      = strollpt_pprheap_destroy,
	.create_nodes = strollpt_pprheap_create_nodes,
	.build        = strollpt_pprheap_build,
	.insert       = strollpt_pprheap_insert,
	.extract      = strollpt_pprheap_extract,
	.remove       = strollpt_pprheap_remove,
	.promote      = strollpt_pprheap_promote,
	.demote       = strollpt_pprheap_demote,
	.count        = strollpt_pprheap_count,
	.validate     = strollpt_pprheap_validate
};

static int
strollpt_heap_prepare_pprheap(const unsigned int * __restrict elements,
                              unsigned int                    nr,
                              size_t                          size)
{
	return strollpt_heap_prepare_nodes(elements,
	                                   nr,
	                                   size,
	                                   &strollpt_pprheap_iface);
}

static int
strollpt_heap_measure_pprheap(
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
	                                   &strollpt_pprheap_iface);
}

#endif /* defined(CONFIG_STROLL_PPRHEAP) */

#if defined(CONFIG_STROLL_DBNHEAP)

#include "stroll/dbnheap.h"

struct strollpt_dbnheap_node {
	int                        id;
	struct stroll_dbnheap_node super;
	const char                 data[0];
};

#define strollpt_dbnheap_node_size(_size) \
	(sizeof(struct strollpt_dbnheap_node) + (_size) - sizeof(int))

static int
strollpt_dbnheap_compare_min(
	const struct stroll_dbnheap_node * __restrict a,
	const struct stroll_dbnheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollpt_dbnheap_node * _a =
		stroll_dbnheap_entry(a,
		                     const struct strollpt_dbnheap_node,
		                     super);
	const struct strollpt_dbnheap_node * _b =
		stroll_dbnheap_entry(b,
		                     const struct strollpt_dbnheap_node,
		                     super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static int
strollpt_dbnheap_create_nodes(struct strollpt_heap_store * __restrict store,
                              const unsigned int * __restrict         elements,
                              unsigned int                            nr,
                              size_t                                  size)
{
	return strollpt_heap_store_init(store,
	                                elements,
	                                nr,
	                                strollpt_dbnheap_node_size(size));
}

static bool
strollpt_dbnheap_validate_rec(const struct stroll_dbnheap_node * __restrict node,
                              unsigned int * __restrict                     nr)
{
	const struct stroll_dbnheap_node * child;

	*nr += 1;

	stroll_dlist_foreach_entry(&node->children, child, siblings) {
		if (child->parent != node)
			return false;
		if (child->order >= node->order)
			return false;
		if (strollpt_dbnheap_compare_min(node, child, NULL) > 0)
			return false;
		if (!strollpt_dbnheap_validate_rec(child, nr))
			return false;
	}

	return true;
}

static bool
strollpt_dbnheap_validate(const void * __restrict heap, unsigned int nr)
{
	const struct stroll_dbnheap *      hp = heap;
	const struct stroll_dbnheap_node * root;
	unsigned int                       cnt = 0;

	if (!nr)
		return stroll_dlist_empty(&hp->base.roots);

	stroll_dlist_foreach_entry(&hp->base.roots, root, siblings) {
		if (root->parent != NULL)
			return false;
		if (!strollpt_dbnheap_validate_rec(root, &cnt))
			return false;
	}

	if ((cnt != nr) || (stroll_dbnheap_count(hp) != nr))
		return false;

	return true;
}

static void *
strollpt_dbnheap_create(void * __restrict array __unused,
                       unsigned int       nr,
                       size_t             size __unused)
{
	return stroll_dbnheap_create(nr, strollpt_dbnheap_compare_min);
}

static void
strollpt_dbnheap_destroy(void * __restrict heap)
{
	stroll_dbnheap_destroy(heap);
}

static void
strollpt_dbnheap_build(void * __restrict heap,
                      void * __restrict  array,
                      unsigned int       nr,
                      size_t             size)
{
	unsigned int n;

	for (n = 0; n < nr; n++) {
		struct strollpt_dbnheap_node * ptr;

		ptr = (struct strollpt_dbnheap_node *)
		      ((char *)array + (n * strollpt_dbnheap_node_size(size)));
		stroll_dbnheap_insert(heap, &ptr->super, NULL);
	}
}

static void
strollpt_dbnheap_insert(void * __restrict heap, void * __restrict node)
{
	stroll_dbnheap_insert(heap,
	                      &((struct strollpt_dbnheap_node *)node)->super,
	                      NULL);
}

static void
strollpt_dbnheap_extract(void * __restrict heap, void * __restrict node)
{
	struct strollpt_dbnheap_node ** ptnode = node;

	*ptnode = stroll_dbnheap_entry(stroll_dbnheap_extract(heap, NULL),
	                               struct strollpt_dbnheap_node,
	                               super);
}

static void
strollpt_dbnheap_remove(void * __restrict heap, void * __restrict node)
{
	struct strollpt_dbnheap_node * ptnode = node;

	stroll_dbnheap_remove(heap, &ptnode->super, NULL);
}

static void
strollpt_dbnheap_promote(void * __restrict heap, void * __restrict node)
{
	struct strollpt_dbnheap_node * ptnode = node;

	stroll_dbnheap_promote(heap, &ptnode->super, NULL);
}

static void
strollpt_dbnheap_demote(void * __restrict heap, void * __restrict node)
{
	struct strollpt_dbnheap_node * ptnode = node;

	stroll_dbnheap_demote(heap, &ptnode->super, NULL);
}

static unsigned int
strollpt_dbnheap_count(void * __restrict heap)
{
	return stroll_dbnheap_count(heap);
}

static const struct strollpt_heap_iface strollpt_dbnheap_iface = {
	.create       = strollpt_dbnheap_create,
	.destroy      = strollpt_dbnheap_destroy,
	.create_nodes = strollpt_dbnheap_create_nodes,
	.build        = strollpt_dbnheap_build,
	.insert       = strollpt_dbnheap_insert,
	.extract      = strollpt_dbnheap_extract,
	.remove       = strollpt_dbnheap_remove,
	.promote      = strollpt_dbnheap_promote,
	.demote       = strollpt_dbnheap_demote,
	.count        = strollpt_dbnheap_count,
	.validate     = strollpt_dbnheap_validate
};

static int
strollpt_heap_prepare_dbnheap(const unsigned int * __restrict elements,
                              unsigned int                    nr,
                              size_t                          size)
{
	return strollpt_heap_prepare_nodes(elements,
	                                   nr,
	                                   size,
	                                   &strollpt_dbnheap_iface);
}

static int
strollpt_heap_measure_dbnheap(
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
	                                   &strollpt_dbnheap_iface);
}

#endif /* defined(CONFIG_STROLL_DBNHEAP) */

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
#if defined(CONFIG_STROLL_HPRHEAP)
	{
		.name    = "hprheap",
		.prepare = strollpt_heap_prepare_hprheap,
		.measure = strollpt_heap_measure_hprheap
	},
#endif
#if defined(CONFIG_STROLL_DRPHEAP)
	{
		.name    = "drpheap",
		.prepare = strollpt_heap_prepare_drpheap,
		.measure = strollpt_heap_measure_drpheap
	},
#endif
#if defined(CONFIG_STROLL_DPRHEAP)
	{
		.name    = "dprheap",
		.prepare = strollpt_heap_prepare_dprheap,
		.measure = strollpt_heap_measure_dprheap
	},
#endif
#if defined(CONFIG_STROLL_PPRHEAP)
	{
		.name    = "pprheap",
		.prepare = strollpt_heap_prepare_pprheap,
		.measure = strollpt_heap_measure_pprheap
	},
#endif
#if defined(CONFIG_STROLL_DBNHEAP)
	{
		.name    = "dbnheap",
		.prepare = strollpt_heap_prepare_dbnheap,
		.measure = strollpt_heap_measure_dbnheap
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
