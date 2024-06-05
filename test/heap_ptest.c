#include "ptest.h"
#include "stroll/heap.h"
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

typedef void
        (strollpt_heap_build_fn)(void * __restrict     array,
                                 unsigned int          nr,
                                 size_t                size,
                                 stroll_array_cmp_fn * compare,
                                 void *                data)
	__stroll_nonull(1, 4);

typedef void
        (strollpt_heap_insert_fn)(const void * __restrict elem,
                                  void * __restrict       array,
                                  unsigned int            nr,
                                  size_t                  size,
                                  stroll_array_cmp_fn *   compare,
                                  void *                  data)
	__stroll_nonull(1, 2, 5);

typedef void
        (strollpt_heap_extract_fn)(void * __restrict     elem,
                                   void * __restrict     array,
                                   unsigned int          nr,
                                   size_t                size,
                                   stroll_array_cmp_fn * compare,
                                   void *                data)
	__stroll_nonull(1, 2, 5);

struct strollpt_heap_iface {
	const char  *              name;
	strollpt_heap_build_fn *   build;
	strollpt_heap_insert_fn *  insert;
	strollpt_heap_extract_fn * extract;
};

struct strollpt_elem {
	unsigned int id;
	const char   data[0];
};

static void
strollpt_array_init(struct strollpt_elem * elements,
                    const unsigned int *   from,
                    unsigned int           nr,
                    size_t                 size)
{
	assert(elements);
	assert(from);
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_elem * ptr;
	unsigned int           n;

	for (n = 0, ptr = elements;
	     n < nr;
	     n++, ptr = (struct strollpt_elem *)((char *)ptr + size))
		ptr->id = from[n];
}

static struct strollpt_elem *
strollpt_array_alloc(unsigned int nr, size_t size)
{
	assert(nr);
	assert(size);

	return malloc(nr * size);
}

static struct strollpt_elem *
strollpt_array_create(const unsigned int * elements,
                      unsigned int         nr,
                      size_t               size)
{
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_elem * elms;

	elms = strollpt_array_alloc(nr, size);
	if (!elms)
		return NULL;

	strollpt_array_init(elms, elements, nr, size);

	return elms;
}

static bool
strollpt_heap_validate(unsigned int                 index,
                       const struct strollpt_elem * elements,
                       unsigned int                 nr,
                       size_t                       size)
{
	assert(elements);
	assert(nr);

	const char * elms = (const char *)elements;

	if (index >= nr)
		return true;

	if (index) {
		unsigned int parent = (index - 1) / 2;

		if (strollpt_compare_min(&elms[parent * size],
		                         &elms[index * size],
		                         NULL) > 0)
			return false;
	}

	if (!strollpt_heap_validate((2 * index) + 1, elements, nr, size))
		return false;

	if (!strollpt_heap_validate((2 * index) + 2, elements, nr, size))
		return false;

	return true;
}

static int
strollpt_heap_prepare(struct strollpt_elem ** __restrict array,
                      struct strollpt_elem ** __restrict heap,
                      const unsigned int * __restrict    elements,
                      unsigned int                       nr,
                      size_t                             size,
                      const struct strollpt_heap_iface * algo)
{
	assert(array);
	assert(heap);
	assert(elements);
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));
	assert(algo);

	struct strollpt_elem * arr;
	struct strollpt_elem * hp;
	unsigned int *         sort;
	unsigned int           e;

	arr = strollpt_array_create(elements, nr, size);
	if (!arr)
		return EXIT_FAILURE;

	hp = strollpt_array_alloc(nr, size);
	if (!hp)
		goto free_array;
	memset(hp, 0xa5, nr * size);

	sort = malloc(nr * sizeof(elements[0]));
	if (!sort)
		goto free_heap;
	for (e = 0; e < nr; e++)
		sort[e] = elements[e];
	qsort_r(sort, nr, sizeof(sort[e]), strollpt_compare_min, NULL);

	algo->build(arr, nr, size, strollpt_compare_min, NULL);
	if (!strollpt_heap_validate(0, arr, nr, size)) {
		strollpt_err("Bogus heapify scheme.\n");
		goto free_sort;
	}

	for (e = 0; e < nr; e++) {
		char elm[size];

		((struct strollpt_elem *)elm)->id = elements[e];
		algo->insert(elm, hp, e, size, strollpt_compare_min, NULL);
	}
	if (!strollpt_heap_validate(0, hp, nr, size)) {
		strollpt_err("Bogus heap insertion scheme.\n");
		goto free_sort;
	}

	for (e = 0; e < nr; e++) {
		char elm[size];

		memset(elm, 0xa5, size);
		algo->extract(elm,
		              arr,
		              nr - e,
		              size,
		              strollpt_compare_min,
		              NULL);
		if (((struct strollpt_elem *)elm)->id != sort[e]) {
			strollpt_err("Bogus heap extraction scheme.\n");
			goto free_sort;
		}
	}

	free(sort);

	*array = arr;
	*heap = hp;

	return EXIT_SUCCESS;

free_sort:
	free(sort);
free_heap:
	free(hp);
free_array:
	free(arr);

	return EXIT_FAILURE;
}

static void
strollpt_heap_build(struct strollpt_elem * __restrict array,
                    const unsigned int * __restrict   elements,
                    unsigned int                      nr,
                    size_t                            size,
                    unsigned long long * __restrict   nsecs,
                    strollpt_heap_build_fn *          build)
{
	struct timespec start, elapse;

	strollpt_array_init(array, elements, nr, size);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	build(array, nr, size, strollpt_compare_min, NULL);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);
}

static void
strollpt_heap_insert(struct strollpt_elem * __restrict array,
                     const unsigned int * __restrict   elements,
                     unsigned int                      nr,
                     size_t                            size,
                     unsigned long long * __restrict   nsecs,
                     strollpt_heap_insert_fn *         insert)
{
	struct timespec start, elapse;
	unsigned int    e;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		char elm[size];

		((struct strollpt_elem *)elm)->id = elements[e];
		insert(elm, array, e, size, strollpt_compare_min, NULL);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);
}

static void
strollpt_heap_extract(struct strollpt_elem * __restrict       array,
                      const struct strollpt_elem * __restrict heap,
                      unsigned int                            nr,
                      size_t                                  size,
                      unsigned long long * __restrict         nsecs,
                      strollpt_heap_extract_fn *              extract)
{
	struct timespec start, elapse;
	unsigned int    e;

	memcpy(array, heap, nr * size);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (e = 0; e < nr; e++) {
		char elm[size];

		extract(elm, array, nr - e, size, strollpt_compare_min, NULL);
	}
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);
}

static const struct strollpt_heap_iface strollpt_heap_algos[] = {
#if defined(CONFIG_STROLL_FHEAP)
	{
		.name     = "fheap",
		.build    = stroll_fheap_build,
		.insert   = stroll_fheap_insert,
		.extract  = stroll_fheap_extract
	},
#endif
};

static const struct strollpt_heap_iface *
strollpt_setup_algo(const char * __restrict name)
{
	unsigned int a;

	for (a = 0; a < stroll_array_nr(strollpt_heap_algos); a++)
		if (!strcmp(name, strollpt_heap_algos[a].name))
			return &strollpt_heap_algos[a];

	strollpt_err("invalid '%s' sort algorithm.\n", name);

	return NULL;
}

static unsigned int *
strollpt_load(struct strollpt_data * __restrict data,
              const char * __restrict           pathname)
{
	unsigned int * keys;
	unsigned int * k;
	int            ret;

	if (strollpt_open_data(data, pathname))
		return NULL;

	keys = malloc(sizeof(*k) * data->nr);
	if (!keys)
		goto close;

	ret = strollpt_init_data_iter(data);
	if (ret)
		goto free;

	k = keys;
	while (true) {
		ret = strollpt_step_data_iter(data, k);
		if (ret)
			break;

		k++;
	}

	if (ret == -EPERM) {
		strollpt_close_data(data);
		return keys;
	}

free:
	free(keys);
close:
	strollpt_close_data(data);

	strollpt_err("failed to load data elements.\n");

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

static void
strollpt_usage(FILE * __restrict stdio)
{
	fprintf(stdio,
	        "Usage: %s [OPTIONS] FILE ALGORITHM SIZE LOOPS\n"
	        "where OPTIONS:\n"
	        "    -p|--prio  PRIORITY\n"
	        "    -h|--help\n",
	        program_invocation_short_name);
}

int main(int argc, char *argv[])
{
	const struct strollpt_heap_iface * algo;
	size_t                             dsize;
	unsigned int                       l, loops = 0;
	int                                prio = 0;
	struct strollpt_data               data;
	unsigned int *                     elems;
	int                                ret;
	struct strollpt_elem *             array;
	struct strollpt_elem *             heap;
	unsigned long long *               nsecs;

	while (true) {
		int                        opt;
		static const struct option lopts[] = {
			{"help",    0, NULL, 'h'},
			{"prio",    1, NULL, 'p'},
			{0,         0, 0,    0}
		};

		opt = getopt_long(argc, argv, "hp:", lopts, NULL);
		if (opt < 0)
			/* No more options: go parsing positional arguments. */
			break;

		switch (opt) {
		case 'p': /* priority */
			if (strollpt_parse_sched_prio(optarg, &prio)) {
				strollpt_usage(stderr);
				return EXIT_FAILURE;
			}

			break;

		case 'h': /* Help message. */
			strollpt_usage(stdout);
			return EXIT_SUCCESS;

		case '?': /* Unknown option. */
		default:
			strollpt_usage(stderr);
			return EXIT_FAILURE;
		}
	}

	/*
	 * Check positional arguments are properly specified on command
	 * line.
	 */
	argc -= optind;
	if (argc != 4) {
		strollpt_err("invalid number of arguments\n");
		strollpt_usage(stderr);
		return EXIT_FAILURE;
	}

	algo = strollpt_setup_algo(argv[optind + 1]);
	if (!algo)
		return EXIT_FAILURE;

	if (strollpt_parse_data_size(argv[optind + 2], &dsize))
		return EXIT_FAILURE;

	if (strollpt_parse_loop_nr(argv[optind + 3], &loops))
		return EXIT_FAILURE;

	elems = strollpt_load(&data, argv[optind]);
	if (!elems)
		return EXIT_FAILURE;

	ret = EXIT_FAILURE;

	if (strollpt_heap_prepare(&array, &heap, elems, data.nr, dsize, algo))
		goto free_elems;

	if (strollpt_setup_sched_prio(prio))
		goto free_heaps;

	nsecs = malloc(3 * loops * sizeof(nsecs[0]));
	if (!nsecs)
		goto free_heaps;
	for (l = 0; l < loops; l++) {
		strollpt_heap_build(array,
		                    elems,
		                    data.nr,
		                    dsize,
		                    &nsecs[l],
		                    algo->build);
		strollpt_heap_insert(array,
		                     elems,
		                     data.nr,
		                     dsize,
		                     &nsecs[loops + l],
		                     algo->insert);
		strollpt_heap_extract(array,
		                      heap,
		                      data.nr,
		                      dsize,
		                      &nsecs[(2 * loops) + l],
		                      algo->extract);
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
	strollpt_heap_show_stats("Heapify", nsecs, loops);
	strollpt_heap_show_stats("Insert", &nsecs[loops], loops);
	strollpt_heap_show_stats("Extract", &nsecs[2 * loops], loops);

	ret = EXIT_SUCCESS;

	free(nsecs);

free_heaps:
	free(array);
	free(heap);
free_elems:
	free(elems);

	return ret;
}
