#include "ptest.h"
#include "stroll/array.h"
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

typedef void
        (strollpt_array_sort_fn)(void * __restrict     array,
                                 unsigned int          nr,
                                 size_t                size,
                                 stroll_array_cmp_fn * compare,
                                 void *                data)
	__stroll_nonull(1, 4);

struct strollpt_array_iface {
	const char  *            name;
	strollpt_array_sort_fn * sort;
};

struct strollpt_array_elem {
	unsigned int id;
	const char   data[0];
};

static struct strollpt_array_elem *
strollpt_array_create(const unsigned int * elements,
                      unsigned int         nr,
                      size_t               size)
{
	assert(elements);
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));

	struct strollpt_array_elem * elms;
	struct strollpt_array_elem * ptr;
	unsigned int                 n;

	elms = malloc(nr * size);
	if (!elms)
		return NULL;

	for (n = 0, ptr = elms;
	     n < nr;
	     n++, ptr = (struct strollpt_array_elem *)((char *)ptr + size))
		ptr->id = elements[n];

	return elms;
}

static int
strollpt_array_sort_validate(const unsigned int *     elements,
                             unsigned int             nr,
                             size_t                   size,
                             strollpt_array_sort_fn * sort)
{
	assert(elements);
	assert(nr);
	assert(size >= sizeof(elements[0]));
	assert(!(size % sizeof(elements[0])));
	assert(sort);

	struct strollpt_array_elem * tmp;
	unsigned int                 n;
	int                          ret = EXIT_FAILURE;

	tmp = strollpt_array_create(elements, nr, size);
	if (!tmp)
		return EXIT_FAILURE;

	sort(tmp, nr, size, strollpt_array_compare_min, NULL);

	for (n = 1; n < nr; n++) {
		const struct strollpt_array_elem * curr;
		const struct strollpt_array_elem * prev;

		prev = (const struct strollpt_array_elem *)
		       (&((const char *)tmp)[(n - 1) * size]);
		curr = (const struct strollpt_array_elem *)
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
                    strollpt_array_sort_fn *        sort)
{
	struct timespec              start, elapse;
	struct strollpt_array_elem * tmp;

	tmp = strollpt_array_create(elements, nr, size);
	if (!tmp)
		return EXIT_FAILURE;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	sort(tmp, nr, size, strollpt_array_compare_min, NULL);
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

#if defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT)

static void
strollpt_array_fwheap_sort(void * __restrict     array,
                           unsigned int          nr,
                           size_t                size,
                           stroll_array_cmp_fn * compare,
                           void *                data)
{
	int err;

	err = stroll_array_fwheap_sort(array, nr, size, compare, data);
	if (err)
		exit(1);
}

#endif /* defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT) */

static const struct strollpt_array_iface strollpt_array_algos[] = {
	{
		.name     = "array_qsort",
		.sort     = strollpt_array_qsort
	},
#if defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT)
	{
		.name     = "array_bubble",
		.sort     = stroll_array_bubble_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)
	{
		.name     = "array_select",
		.sort     = stroll_array_select_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)
	{
		.name     = "array_insert",
		.sort     = stroll_array_insert_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)
	{
		.name     = "array_quick",
		.sort     = stroll_array_quick_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_3WQUICK_SORT)
	{
		.name     = "array_3wquick",
		.sort     = stroll_array_3wquick_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT)
	{
		.name     = "array_merge",
		.sort     = strollpt_array_merge_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_FBHEAP_SORT)
	{
		.name     = "array_fbheap",
		.sort     = stroll_array_fbheap_sort
	},
#endif
#if defined(CONFIG_STROLL_ARRAY_FWHEAP_SORT)
	{
		.name     = "array_fwheap",
		.sort     = strollpt_array_fwheap_sort
	},
#endif
};

static const struct strollpt_array_iface *
strollpt_array_setup_algo(const char * __restrict name)
{
	unsigned int a;

	for (a = 0; a < stroll_array_nr(strollpt_array_algos); a++)
		if (!strcmp(name, strollpt_array_algos[a].name))
			return &strollpt_array_algos[a];

	strollpt_err("invalid '%s' sort algorithm.\n", name);

	return NULL;
}

int main(int argc, char *argv[])
{
	struct strollpt                     ptest;
	const struct strollpt_array_iface * algo;
	unsigned long long *                nsecs;
	unsigned int                        l;
	struct strollpt_stats               stats;
	int                                 ret = EXIT_FAILURE;

	if (strollpt_init(&ptest, argc, argv))
		return EXIT_FAILURE;

	algo = strollpt_array_setup_algo(argv[optind + 1]);
	if (!algo)
		goto fini;

	if (strollpt_array_sort_validate(ptest.data_elems,
	                                 ptest.data_desc.nr,
	                                 ptest.data_size,
	                                 algo->sort))
		goto fini;

	nsecs = malloc(ptest.loops_nr * sizeof(*nsecs));
	if (!nsecs)
		goto fini;

	if (strollpt_setup_sched_prio(ptest.sched_prio))
		goto free;

	for (l = 0; l < ptest.loops_nr; l++) {
		if (strollpt_array_sort(ptest.data_elems,
		                        ptest.data_desc.nr,
		                        ptest.data_size,
		                        &nsecs[l],
		                        algo->sort))
			goto free;
	}

	strollpt_calc_stats(&stats, nsecs, ptest.loops_nr);
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
	       ptest.data_desc.nr,
	       ptest.data_desc.order,
	       ptest.data_desc.singles,
	       algo->name,
	       ptest.data_size,
	       ptest.loops_nr,
	       stats.count,
	       ((double)stats.count * 100.0) / (double)ptest.loops_nr,
	       stats.min,
	       stats.max,
	       (unsigned long long)round(stats.stdev),
	       stats.med,
	       (unsigned long long)round(stats.mean));

	ret = EXIT_SUCCESS;

free:
	free(nsecs);
fini:
	strollpt_fini(&ptest);

	return ret;
}
