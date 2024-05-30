#include "ptest.h"
#include "stroll/array.h"
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

typedef void
        (strollpt_sort_fn)(void * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare,
                           void *                data)
	__stroll_nonull(1, 4);

struct strollpt_iface {
	const char  *      name;
	strollpt_sort_fn * sort;
};

struct strollpt_elem {
	unsigned int id;
	const char   data[0];
};

static struct strollpt_elem *
strollpt_array_create(const unsigned int * elements,
                      unsigned int         nr,
                      size_t               size)
{
	assert(elements);
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_elem * elms;
	struct strollpt_elem * ptr;
	unsigned int           n;

	elms = malloc(nr * size);
	if (!elms)
		return NULL;

	for (n = 0, ptr = elms;
	     n < nr;
	     n++, ptr = (struct strollpt_elem *)((char *)ptr + size))
		ptr->id = elements[n];

	return elms;
}

static int
strollpt_array_sort_validate(const unsigned int * elements,
                             unsigned int         nr,
                             size_t               size,
                             strollpt_sort_fn *   sort)
{
	assert(elements);
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));
	assert(sort);

	struct strollpt_elem * tmp;
	unsigned int           n;
	int                    ret = EXIT_FAILURE;

	tmp = strollpt_array_create(elements, nr, size);
	if (!tmp)
		return EXIT_FAILURE;

	sort(tmp, nr, size, strollpt_compare_min, NULL);

	for (n = 1; n < nr; n++) {
		const struct strollpt_elem * curr;
		const struct strollpt_elem * prev;

		prev = (const struct strollpt_elem *)
		       (&((const char *)tmp)[(n - 1) * size]);
		curr = (const struct strollpt_elem *)
		       (&((const char *)tmp)[n * size]);

		if (prev->id > curr->id) {
			strollpt_err("Bogus sorting scheme.\n");
			goto free;
		}
	}

	ret = EXIT_SUCCESS;

free:
	free(tmp);

	return ret;
}

static int
strollpt_array_sort(const unsigned int * __restrict elements,
                    unsigned int                    nr,
                    size_t                          size,
                    unsigned long long * __restrict nsecs,
                    strollpt_sort_fn *              sort)
{
	struct timespec start, elapse;
	struct strollpt_elem * tmp;

	tmp = strollpt_array_create(elements, nr, size);
	if (!tmp)
		return EXIT_FAILURE;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	sort(tmp, nr, size, strollpt_compare_min, NULL);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	free(tmp);

	return EXIT_SUCCESS;
}

/* Glibc's quick sorting */
static inline void
strollpt_array_qsort(void * __restrict     array,
                     unsigned int          nr,
                     size_t                size,
                     stroll_array_cmp_fn * compare,
                     void *                data)
{
	qsort_r(array, (unsigned int)nr, size, compare, data);
}

#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT)

static void
strollpt_array_merge_sort(void * __restrict     array,
                          unsigned int          nr,
                          size_t                size,
                          stroll_array_cmp_fn * compare,
                          void *                data)
{
	int err;

	err = stroll_array_merge_sort(array, nr, size, compare, data);
	if (err)
		exit(1);
}

#endif /* defined(CONFIG_STROLL_ARRAY_MERGE_SORT) */

static const struct strollpt_iface strollpt_algos[] = {
	{
		.name     = "qsort",
		.sort     = strollpt_array_qsort
	},
#if defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT)
	{
		.name     = "bubble",
		.sort     = stroll_array_bubble_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)
	{
		.name     = "select",
		.sort     = stroll_array_select_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)
	{
		.name     = "insert",
		.sort     = stroll_array_insert_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)
	{
		.name     = "quick",
		.sort     = stroll_array_quick_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT)
	{
		.name     = "3wquick",
		.sort     = stroll_array_3wquick_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT)
	{
		.name     = "merge",
		.sort     = strollpt_array_merge_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_HEAP_SORT)
	{
		.name     = "heap",
		.sort     = stroll_array_heap_sort
	},
#endif
};

static const struct strollpt_iface *
strollpt_setup_algo(const char * __restrict name)
{
	unsigned int a;

	for (a = 0; a < stroll_array_nr(strollpt_algos); a++)
		if (!strcmp(name, strollpt_algos[a].name))
			return &strollpt_algos[a];

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
	const struct strollpt_iface * algo;
	size_t                        dsize;
	unsigned int                  l, loops = 0;
	int                           prio = 0;
	struct strollpt_data          data;
	unsigned int *                elems;
	int                           ret;
	unsigned long long *          nsecs;
	struct strollpt_stats         stats;

	while (true) {
		int                        opt;
		static const struct option lopts[] = {
			{"help",    0, NULL, 'h'},
			{"prio",    1, NULL, 'p'},
			{0,         0, 0,    0}
		};

		opt = getopt_long(argc, argv, "hp:", lopts, NULL);
		if (opt < 0)
			/* No more options: go parsing positional arguments. */
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

	if (strollpt_array_sort_validate(elems, data.nr, dsize, algo->sort))
		goto free_elems;

	if (strollpt_setup_sched_prio(prio))
		goto free_elems;

	nsecs = malloc(loops * sizeof(*nsecs));
	if (!nsecs)
		goto free_elems;
	for (l = 0; l < loops; l++) {
		if (strollpt_array_sort(elems,
		                        data.nr,
		                        dsize,
		                        &nsecs[l],
		                        algo->sort))
			goto free_nsecs;
	}

	strollpt_calc_stats(&stats, nsecs, loops);
	printf("#Samples:       %u\n"
	       "Order ratio:    %hu\n"
	       "Distinct ratio: %hu\n"
	       "Algorithm:      %s\n"
	       "Data size:      %zu\n"
	       "#Loops:         %u\n"
	       "#Inliers:       %u (%.2lf%%)\n"
	       "Mininum:        %llu nSec\n"
	       "Maximum:        %llu nSec\n"
	       "Deviation:      %llu nSec\n"
	       "Median:         %llu nSec\n"
	       "Mean:           %llu nSec\n",
	       data.nr,
	       data.order,
	       data.singles,
	       algo->name,
	       dsize,
	       loops,
	       stats.count, ((double)stats.count * 100.0) / (double)loops,
	       stats.min,
	       stats.max,
	       (unsigned long long)round(stats.stdev),
	       stats.med,
	       (unsigned long long)round(stats.mean));

	ret = EXIT_SUCCESS;

free_nsecs:
	free(nsecs);
free_elems:
	free(elems);

	return ret;
}
