#include "ptest.h"
#include "stroll/array.h"
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

struct strollpt_heap_elem {
	unsigned int id;
	const char   data[0];
};

typedef bool (strollpt_heap_validate_fn)(unsigned int,
                                         const void *,
                                         unsigned int);

typedef void * (strollpt_heap_create_fn)(void * __restrict,
                                         unsigned int,
                                         size_t,
                                         stroll_array_cmp_fn *)
	__stroll_nonull(1, 4) __warn_result;

typedef void (strollpt_heap_destroy_fn)(void * __restrict) __stroll_nonull(1);

typedef void (strollpt_heap_build_fn)(void * __restrict) __stroll_nonull(1);

typedef void (strollpt_heap_insert_fn)(void * __restrict,
                                       const void * __restrict)
	__stroll_nonull(1, 2);

typedef void (strollpt_heap_extract_fn)(void * __restrict, void * __restrict)
	__stroll_nonull(1, 2);

typedef unsigned int (strollpt_heap_count_fn)(void * __restrict)
	__stroll_nonull(1);

struct strollpt_heap_iface {
	const char *               name;
	strollpt_heap_create_fn *   create;
	strollpt_heap_destroy_fn *  destroy;
	strollpt_heap_build_fn *    build;
	strollpt_heap_insert_fn *   insert;
	strollpt_heap_extract_fn *  extract;
	strollpt_heap_count_fn *    count;
	strollpt_heap_validate_fn * validate;
};

static void
strollpt_array_init(struct strollpt_heap_elem * elements,
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
strollpt_array_alloc(unsigned int nr, size_t size)
{
	assert(nr);
	assert(size);

	return malloc(nr * size);
}

static struct strollpt_heap_elem *
strollpt_array_create(const unsigned int * elements,
                      unsigned int         nr,
                      size_t               size)
{
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_heap_elem * elms;

	elms = strollpt_array_alloc(nr, size);
	if (!elms)
		return NULL;

	strollpt_array_init(elms, elements, nr, size);

	return elms;
}
static bool
strollpt_heap_validate(const void *                                  heap,
                       unsigned int                                  nr,
                       const struct strollpt_heap_iface * __restrict algo)
{
	return algo->validate(0, heap, nr);
}

static int
strollpt_heap_prepare(void ** __restrict                      heap,
                      struct strollpt_heap_elem ** __restrict array,
                      const unsigned int * __restrict         elements,
                      unsigned int                            nr,
                      size_t                                  size,
                      const struct strollpt_heap_iface *      algo)
{
	assert(elements);
	assert(nr);
	assert(size);
	assert(size >= sizeof(struct strollpt_heap_elem));
	assert(algo);

	struct strollpt_heap_elem * arr;
	unsigned int *              sort;
	void *                      hp;
	unsigned int                e;

	if (size % sizeof(struct strollpt_heap_elem)) {
		strollpt_err("invalid data element size %zu specified: "
		             "multiple of %zu expected.\n",
		             size,
		             sizeof(struct strollpt_heap_elem));
		return EXIT_FAILURE;
	}


	arr = strollpt_array_create(elements, nr, size);
	if (!arr)
		return EXIT_FAILURE;

	sort = malloc(nr * sizeof(elements[0]));
	if (!sort)
		goto free_array;
	for (e = 0; e < nr; e++)
		sort[e] = elements[e];
	qsort_r(sort, nr, sizeof(sort[e]), strollpt_array_compare_min, NULL);

	hp = algo->create(arr, nr, size, strollpt_array_compare_min);
	if (!hp)
		goto free_sort;

	algo->build(hp);
	if (!strollpt_heap_validate(hp, nr, algo)) {
		strollpt_err("Bogus heapify scheme.\n");
		goto free_heap;
	}

	for (e = 0; e < nr; e++) {
		char elm[size];

		memset(elm, 0xa5, size);

		algo->extract(hp, elm);
		if (((struct strollpt_heap_elem *)elm)->id != sort[e]) {
			strollpt_err("Bogus heap extraction scheme.\n");
			goto free_heap;
		}

		if (algo->count(hp) != (nr - e - 1)) {
			strollpt_err("Bogus heap extraction count.\n");
			goto free_heap;
		}
	}

	memset(arr, 0xa5, nr * size);
	for (e = 0; e < nr; e++) {
		char elm[size];

		memset(elm, 0, size);

		((struct strollpt_heap_elem *)elm)->id = elements[e];
		algo->insert(hp, elm);

		if (algo->count(hp) != (e + 1)) {
			strollpt_err("Bogus heap insertion count.\n");
			goto free_heap;
		}
	}
	if (!strollpt_heap_validate(hp, nr, algo)) {
		strollpt_err("Bogus heap insertion scheme.\n");
		goto free_heap;
	}

	free(sort);

	*heap = hp;
	*array = arr;

	return EXIT_SUCCESS;

free_heap:
	algo->destroy(hp);
free_sort:
	free(sort);
free_array:
	free(arr);

	return EXIT_FAILURE;
}

static void
strollpt_heap_build(void * __restrict                  heap,
                    unsigned long long * __restrict    nsecs,
                    const struct strollpt_heap_iface * algo)
{
	struct timespec start, elapse;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	algo->build(heap);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);
}

static void
strollpt_heap_extract(void * __restrict                  heap,
                      unsigned int                       nr,
                      size_t                             size,
                      unsigned long long * __restrict    nsecs,
                      const struct strollpt_heap_iface * algo)
{
	struct timespec start, elapse;
	unsigned int    e;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		char elm[size];

		algo->extract(heap, elm);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);
}

static void
strollpt_heap_insert(void * __restrict                  heap,
                     const unsigned int * __restrict    elements,
                     unsigned int                       nr,
                     size_t                             size,
                     unsigned long long * __restrict    nsecs,
                     const struct strollpt_heap_iface * algo)
{
	struct timespec start, elapse;
	unsigned int    e;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		char elm[size];

		((struct strollpt_heap_elem *)elm)->id = elements[e];
		algo->insert(heap, elm);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);
}

#if defined(CONFIG_STROLL_FBHEAP)

#include "stroll/fbheap.h"

static bool
strollpt_fbheap_validate(unsigned int index, const void * heap, unsigned int nr)
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

	if (!strollpt_fbheap_validate((2 * index) + 1, heap, nr))
		return false;

	return strollpt_fbheap_validate((2 * index) + 2, heap, nr);
}

static void *
strollpt_fbheap_create(void * __restrict     array,
                       unsigned int          nr,
                       size_t                size,
                       stroll_array_cmp_fn * compare)
{
	return stroll_fbheap_create(array, nr, size, compare);
}

static void
strollpt_fbheap_destroy(void * __restrict heap)
{
	stroll_fbheap_destroy(heap);
}

static void
strollpt_fbheap_build(void * __restrict heap)
{
	stroll_fbheap_build(heap, stroll_fbheap_nr(heap), NULL);
}

static void
strollpt_fbheap_insert(void * __restrict heap, const void * __restrict elem)
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
strollpt_fwheap_validate(unsigned int index, const void * heap, unsigned int nr)
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
		if (!strollpt_fwheap_validate(strollpt_fwheap_left(index,
		                                                   hp->rbits),
		                              heap,
		                              nr))
			return false;
	}

	return strollpt_fwheap_validate(strollpt_fwheap_right(index, hp->rbits),
	                                heap,
	                                nr);
}

static void *
strollpt_fwheap_create(void * __restrict     array,
                       unsigned int          nr,
                       size_t                size,
                       stroll_array_cmp_fn * compare)
{
	return stroll_fwheap_create(array, nr, size, compare);
}

static void
strollpt_fwheap_destroy(void * __restrict heap)
{
	stroll_fwheap_destroy(heap);
}

static void
strollpt_fwheap_build(void * __restrict heap)
{
	stroll_fwheap_build(heap, stroll_fwheap_nr(heap), NULL);
}

static void
strollpt_fwheap_insert(void * __restrict heap, const void * __restrict elem)
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

#endif /* defined(CONFIG_STROLL_FWHEAP) */

static const struct strollpt_heap_iface strollpt_heap_algos[] = {
#if defined(CONFIG_STROLL_FBHEAP)
	{
		.name     = "fbheap",
		.create   = strollpt_fbheap_create,
		.destroy  = strollpt_fbheap_destroy,
		.build    = strollpt_fbheap_build,
		.insert   = strollpt_fbheap_insert,
		.extract  = strollpt_fbheap_extract,
		.count    = strollpt_fbheap_count,
		.validate = strollpt_fbheap_validate
	},
#endif
#if defined(CONFIG_STROLL_FWHEAP)
	{
		.name     = "fwheap",
		.create   = strollpt_fwheap_create,
		.destroy  = strollpt_fwheap_destroy,
		.build    = strollpt_fwheap_build,
		.insert   = strollpt_fwheap_insert,
		.extract  = strollpt_fwheap_extract,
		.count    = strollpt_fwheap_count,
		.validate = strollpt_fwheap_validate
	},
#endif
};

static const struct strollpt_heap_iface *
strollpt_heap_setup_algo(const char * __restrict name)
{
	unsigned int a;

	for (a = 0; a < stroll_array_nr(strollpt_heap_algos); a++)
		if (!strcmp(name, strollpt_heap_algos[a].name))
			return &strollpt_heap_algos[a];

	strollpt_err("invalid '%s' sort algorithm.\n", name);

	return NULL;
}

static void
strollpt_heap_show_stats(const char *         title,
                         unsigned long long * nsecs,
                         unsigned int         loops)
{
	struct strollpt_stats stats;

	strollpt_calc_stats(&stats, nsecs, loops);
	printf("%s:\n"
	       "    #Inliers:   %u (%.2lf%%)\n"
	       "    Mininum:    %llu nSec\n"
	       "    Maximum:    %llu nSec\n"
	       "    Deviation:  %llu nSec\n"
	       "    Median:     %llu nSec\n"
	       "    Mean:       %llu nSec\n",
	       title,
	       stats.count, ((double)stats.count * 100.0) / (double)loops,
	       stats.min,
	       stats.max,
	       (unsigned long long)round(stats.stdev),
	       stats.med,
	       (unsigned long long)round(stats.mean));
}

int main(int argc, char *argv[])
{
	struct strollpt                    ptest;
	const struct strollpt_heap_iface * algo;
	void *                             heap;
	struct strollpt_heap_elem *        array;
	unsigned long long *               nsecs;
	unsigned int                       l;
	int                                ret = EXIT_FAILURE;

	if (strollpt_init(&ptest, argc, argv))
		return EXIT_FAILURE;

	algo = strollpt_heap_setup_algo(argv[optind + 1]);
	if (!algo)
		goto fini;

	if (strollpt_heap_prepare(&heap,
	                          &array,
	                          ptest.data_elems,
	                          ptest.data_desc.nr,
	                          ptest.data_size,
	                          algo))
		goto fini;

	nsecs = malloc(3 * ptest.loops_nr * sizeof(nsecs[0]));
	if (!nsecs)
		goto free_heap;

	if (strollpt_setup_sched_prio(ptest.sched_prio))
		goto free_nsecs;

	for (l = 0; l < ptest.loops_nr; l++) {
		strollpt_array_init(array,
		                    ptest.data_elems,
		                    ptest.data_desc.nr,
		                    ptest.data_size);
		strollpt_heap_build(heap, &nsecs[l], algo);
		strollpt_heap_extract(heap,
		                      ptest.data_desc.nr,
		                      ptest.data_size,
		                      &nsecs[ptest.loops_nr + l],
		                      algo);
		strollpt_heap_insert(heap,
		                     ptest.data_elems,
		                     ptest.data_desc.nr,
		                     ptest.data_size,
		                     &nsecs[(2 * ptest.loops_nr) + l],
		                     algo);
	}

	printf("#Samples:       %u\n"
	       "Order ratio:    %hu\n"
	       "Distinct ratio: %hu\n"
	       "Algorithm:      %s\n"
	       "Data size:      %zu\n"
	       "#Loops:         %u\n",
	       ptest.data_desc.nr,
	       ptest.data_desc.order,
	       ptest.data_desc.singles,
	       algo->name,
	       ptest.data_size,
	       ptest.loops_nr);
	strollpt_heap_show_stats("Heapify",
	                         nsecs,
	                         ptest.loops_nr);
	strollpt_heap_show_stats("Extract",
	                         &nsecs[ptest.loops_nr],
	                         ptest.loops_nr);
	strollpt_heap_show_stats("Insert",
	                         &nsecs[2 * ptest.loops_nr],
	                         ptest.loops_nr);

	ret = EXIT_SUCCESS;

free_nsecs:
	free(nsecs);
free_heap:
	algo->destroy(heap);
	free(array);
fini:
	strollpt_fini(&ptest);

	return ret;
}
