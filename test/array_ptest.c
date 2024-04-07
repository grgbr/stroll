#include "ptest.h"
#include "stroll/cdefs.h"
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

struct strollpt_iface {
	char  * name;
	int  (* validate)(const unsigned int * __restrict, unsigned int);
	int  (* sort)(const unsigned int * __restrict,
	              unsigned int,
	              unsigned long long * __restrict);
};

/******************************************************************************
 * Glibc's quick sorting
 ******************************************************************************/

static int
strollpt_qsort_validate(const unsigned int * elements, unsigned int nr)
{
	unsigned int   n;
	unsigned int * tmp;
	int            ret = EXIT_FAILURE;

	tmp = malloc(sizeof(*tmp) * nr);
	if (!tmp)
		return EXIT_FAILURE;

	memcpy(tmp, elements, sizeof(*tmp) * nr);

	qsort(tmp, nr, sizeof(unsigned int), strollpt_compare_min);

	for (n = 1; n < nr; n++) {
		if (tmp[n - 1] > tmp[n]) {
			strollpt_err("Bogus sorting scheme.\n");
			goto free;
		}
	}

	ret = EXIT_SUCCESS;

free:
	free(tmp);

	return ret;
}

static int strollpt_qsort_sort(const unsigned int * __restrict elements,
                               unsigned int                    nr,
                               unsigned long long * __restrict nsecs)
{
	struct timespec start, elapse;
	unsigned int *  tmp;

	tmp = malloc(sizeof(*tmp) * nr);
	if (!tmp)
		return EXIT_FAILURE;

	memcpy(tmp, elements, sizeof(*tmp) * nr);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	qsort(tmp, nr, sizeof(unsigned int), strollpt_compare_min);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &elapse);

	elapse = strollpt_tspec_sub(&elapse, &start);
	*nsecs = strollpt_tspec2ns(&elapse);

	free(tmp);

	return EXIT_SUCCESS;
}

static const struct strollpt_iface strollpt_algos[] = {
	{
		.name     = "qsort",
		.validate = strollpt_qsort_validate,
		.sort     = strollpt_qsort_sort
	}
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
	        "Usage: %s [OPTIONS] FILE ALGORITHM LOOPS\n"
	        "where OPTIONS:\n"
	        "    -p|--prio  PRIORITY\n"
	        "    -h|--help\n",
	        program_invocation_short_name);
}

int main(int argc, char *argv[])
{
	const struct strollpt_iface * algo;
	unsigned int                  l, loops = 0;
	int                           prio = 0;
	struct strollpt_data          data;
	unsigned int *                elems;
	int                           ret;
	unsigned long long            nsecs;

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
	if (argc != 3) {
		strollpt_err("invalid number of arguments\n");
		strollpt_usage(stderr);
		return EXIT_FAILURE;
	}

	algo = strollpt_setup_algo(argv[optind + 1]);
	if (!algo)
		return EXIT_FAILURE;

	if (strollpt_parse_loop_nr(argv[optind + 2], &loops))
		return EXIT_FAILURE;

	elems = strollpt_load(&data, argv[optind]);
	if (!elems)
		return EXIT_FAILURE;

	ret = EXIT_FAILURE;

	if (algo->validate(elems, data.nr))
		goto free;

	if (strollpt_setup_sched_prio(prio))
		goto free;

	for (l = 0; l < loops; l++) {
		if (algo->sort(elems, data.nr, &nsecs))
			goto free;
		printf("nsec=%llu\n", nsecs);
	}

	ret = EXIT_SUCCESS;

free:
	free(elems);

	return ret;
}
