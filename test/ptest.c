/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "ptest.h"
#include "stroll/cdefs.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>
#include <sched.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>

static int
strollpt_compare_sample(const void * __restrict a, const void * __restrict b)
{
	assert(a);
	assert(b);

	unsigned long long _a = *(const unsigned long long *)a;
	unsigned long long _b = *(const unsigned long long *)b;

	if (_a < _b)
		return -1;
	else if (_a > _b)
		return 1;
	else
		return 0;
}

static double
strollpt_update_mean(double mean, double value, unsigned int iter)
{
	return ((mean * (double)iter) + value) / ((double)iter + 1.0);
}

static double
strollpt_calc_mean(unsigned long long * __restrict    values,
                   unsigned int                       nr)
{
	assert(values);
	assert(nr > 0);

	unsigned int v;
	double       mean = (double)values[0];

	for (v = 1; v < nr; v++)
		mean = strollpt_update_mean(mean, (double)values[v], v);

	return mean;
}

static double
strollpt_calc_stdev(double                          mean,
                    unsigned long long * __restrict values,
                    unsigned int                    nr)
{
	assert(values);
	assert(nr > 0);

	unsigned int v;
	double       diff = (double)values[0] - mean;
	double       var = diff * diff;

	for (v = 1; v < nr; v++) {
		diff = (double)values[v] - mean;
		var = strollpt_update_mean(var, (diff * diff), v);
	}

	assert(var >= 0.0);

	return sqrt(var);
}

/*
 * Exclude outliers using interquartile range (IQR) outlier detection logic.
 *
 * Timing measurements distribution shows positive skewness, i.e. right-skewed,
 * with long right tail. This is likely due to underlying system activity
 * introducing delays within measurements.
 *
 * Let's assume that meaningful measurements are normally distributed and exclude
 * "exceptional values" / outliers using standard +/- 1.5*IQR whiskers.
 *
 * See:
 * https://en.wikipedia.org/wiki/Interquartile_range
 * https://en.wikipedia.org/wiki/Box_plot
 */
static void
strollpt_probe_outliers(const unsigned long long * __restrict values,
                        unsigned int                          nr,
                        unsigned int * __restrict             low,
                        unsigned int * __restrict             high)
{
	assert(values);
	assert(nr > 0);
	assert(low);
	assert(high);

	unsigned int       q1 = nr / 4;
	unsigned int       q3 = (3 * nr) / 4;
	unsigned long long iqr = ((values[q3] - values[q1]) * 2) / 3;
	unsigned long long thres;
	unsigned int       v;

	/* At first, find index of lowest inlier. */
	thres = stroll_max(values[q1], iqr) - iqr;
	for (v = 0; v < nr; v++) {
		if (values[v] >= thres)
			break;
	}
	assert(v <= q1);
	*low = v;

	/* Then, find index of highest inlier. */
	thres = values[q3] + iqr;
	for (v = q3; v < nr; v++) {
		if (values[v] > thres)
			break;
	}
	assert(v > q3);
	assert(v <= nr);
	*high = v - 1;
}

void
strollpt_calc_stats(struct strollpt_stats * __restrict stats,
                    unsigned long long * __restrict    values,
                    unsigned int                       nr)
{
	assert(stats);
	assert(values);
	assert(nr > 0);

	unsigned int low;   /* index of lowest inlier */
	unsigned int high;  /* index of highest inlier */

	qsort(values, nr, sizeof(*values), strollpt_compare_sample);

	strollpt_probe_outliers(values, nr, &low, &high);

	stats->min = values[low];
	stats->max = values[high];
	stats->med = values[(high + 1 + low) / 2];
	stats->count = high + 1 - low;
	stats->mean = strollpt_calc_mean(&values[low], stats->count);
	stats->stdev = strollpt_calc_stdev(stats->mean,
	                                   &values[low],
	                                   stats->count);
}

struct timespec
strollpt_tspec_sub(const struct timespec * __restrict a,
                   const struct timespec * __restrict b)
{
	assert(a);
	assert(b);

	struct timespec res = {
		.tv_sec  = a->tv_sec - b->tv_sec,
		.tv_nsec = a->tv_nsec - b->tv_nsec
	};

	if (res.tv_nsec < 0) {
		res.tv_sec--;
		res.tv_nsec += 1000000000;
	}

	return res;
}

static int
strollpt_parse_algo_name(const char * __restrict  arg,
                         const char ** __restrict algo)
{
	size_t len;

#define STROLLPT_ALGO_NAME_MAX (128U)
	len = strnlen(arg, STROLLPT_ALGO_NAME_MAX);
	if (!len || (len == STROLLPT_ALGO_NAME_MAX)) {
		strollpt_err("invalid algorithm name.\n");
		return EXIT_FAILURE;
	}

	*algo = arg;

	return EXIT_SUCCESS;
}

static int
strollpt_parse_data_size(const char * __restrict arg,
                         size_t * __restrict     data_size)
{
	assert(arg);
	assert(data_size);

	char *        str;
	unsigned long sz;

	sz = strtoul(arg, &str, 0);
	if (*str) {
		strollpt_err("invalid data element size '%s' specified: "
		             "positive integer expected.\n",
		             arg);
		return EXIT_FAILURE;
	}
	else if (!sz) {
		strollpt_err("invalid data element size '%s' specified: "
		             "non-zero integer expected.\n",
		             arg);
		return EXIT_FAILURE;
	}
	else if (sz > UINT_MAX) {
		strollpt_err("invalid data element size '%s' specified: "
		             "integer <= UINT_MAX expected.\n",
		             arg);
		return EXIT_FAILURE;
	}

	*data_size = (size_t)sz;

	return EXIT_SUCCESS;
}

static int
strollpt_parse_loop_nr(const char * __restrict   arg,
                       unsigned int * __restrict loop_nr)
{
	assert(arg);
	assert(loop_nr);

	char *        str;
	unsigned long nr;
	int           err = 0;

	nr = strtoul(arg, &str, 0);
	if (*str)
		err = EINVAL;
	else if (!nr || (nr > UINT_MAX))
		err = ERANGE;

	if (err) {
		strollpt_err("invalid number of loops '%s' specified: "
		             "%s (%d).\n",
		             arg,
		             strerror(err),
		             err);
		return EXIT_FAILURE;
	}

	*loop_nr = (unsigned int)nr;

	return EXIT_SUCCESS;
}

static int
strollpt_parse_sched_prio(const char * __restrict arg,
                          int * __restrict        priority)
{
	assert(arg);
	assert(priority);

	char * str;
	int    prio;
	int    err = 0;

	prio = (int)strtoul(arg, &str, 0);
	if (*str)
		err = EINVAL;
	else if ((prio < sched_get_priority_min(SCHED_FIFO)) ||
	         (prio > sched_get_priority_max(SCHED_FIFO)))
		err = ERANGE;

	if (err) {
		strollpt_err("invalid scheduling priority '%s' specified: "
		             "%s (%d).\n",
		             arg,
		             strerror(err),
		             err);
		return EXIT_FAILURE;
	}

	*priority = prio;

	return EXIT_SUCCESS;
}

int
strollpt_setup_sched_prio(int priority)
{
	if (priority) {
		assert(priority >= sched_get_priority_min(SCHED_FIFO));
		assert(priority <= sched_get_priority_max(SCHED_FIFO));

		const struct sched_param parm = { .sched_priority = priority };

		if (sched_setscheduler(getpid(), SCHED_FIFO, &parm)) {
			strollpt_err("failed set scheduling policy: "
			             "%s (%d).\n",
			             strerror(errno),
			             errno);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

static int
strollpt_step_data_iter(const struct strollpt_data * __restrict data,
                        unsigned int * __restrict               element)
{
	assert(data);
	assert(element);

	unsigned int elem;

	if (fread(&elem, sizeof(elem), 1, data->file) == 1) {
		switch (data->endian) {
		case STROLLPT_NATIVE_ENDIAN:
			*element = elem;
			break;
		case STROLLPT_LITTLE_ENDIAN:
			*element = le32toh(elem);
			break;
		case STROLLPT_BIG_ENDIAN:
			*element = be32toh(elem);
		}

		return 0;
	}

	if (feof(data->file))
		return -EPERM;

	strollpt_err("failed to fetch data element.\n");

	return -EIO;
}

static int
strollpt_init_data_iter(const struct strollpt_data * __restrict data)
{
	assert(data);

	if (fseek(data->file,  data->start, SEEK_SET)) {
		int err = errno;

		strollpt_err("failed to initialize data iterator: %s (%d).\n",
		             strerror(err),
		             err);
		return -err;
	}

	return 0;
}

static int
strollpt_open_data(struct strollpt_data * __restrict data,
                   const char * __restrict           pathname)
{
	assert(data);
	assert(pathname);

	char           buff[11];
	unsigned short len;
	long           nr;

	data->file = fopen(pathname, "r");
	if (!data->file) {
		strollpt_err("failed to open '%s' file: %s (%d).\n",
		             pathname,
		             strerror(errno),
		             errno);
		return EXIT_FAILURE;
	}

	if (fread(buff, sizeof(buff), 1, data->file) != 1) {
		strollpt_err("failed to fetch data header: %s (%d).\n",
		             strerror(errno),
		             errno);
		goto close;
	}

	switch (buff[0]) {
	case 'n':
		data->endian = STROLLPT_NATIVE_ENDIAN;
		break;
	case 'l':
		data->endian = STROLLPT_LITTLE_ENDIAN;
		break;
	case 'b':
		data->endian = STROLLPT_BIG_ENDIAN;
		break;
	default:
		strollpt_err("unexpected data endianness.\n");
		goto close;
	}

	memcpy(&data->order, &buff[1], sizeof(data->order));
	memcpy(&data->singles, &buff[3], sizeof(data->singles));
	memcpy(&data->nr, &buff[5], sizeof(data->nr));
	memcpy(&len, &buff[9], sizeof(len));

	switch (data->endian) {
	case STROLLPT_NATIVE_ENDIAN:
		break;
	case STROLLPT_LITTLE_ENDIAN:
		data->order = le16toh(data->order);
		data->singles = le16toh(data->singles);
		data->nr = le32toh(data->nr);
		len = le16toh(len);
		break;
	case STROLLPT_BIG_ENDIAN:
		data->order = be16toh(data->order);
		data->singles = be16toh(data->singles);
		data->nr = be32toh(data->nr);
		len = be16toh(len);
	}

	if (data->order > 100) {
		strollpt_err("invalid data ordering ratio '%hu'.\n",
		             data->order);
		goto close;
	}

	if (data->singles > 100) {
		strollpt_err("invalid data distinct value ratio '%hu'.\n",
		             data->order);
		goto close;
	}

#define STROLLPT_SAMPLES_MIN (8U)
	if (data->nr < STROLLPT_SAMPLES_MIN) {
		strollpt_err("invalid number of data samples '%u'.\n",
		             data->nr);
		goto close;
	}

	if (!len || (len >= 4096)) {
		strollpt_err("invalid data command line length '%hu'.\n",
		             len);
		goto close;
	}

	data->cmd = malloc(len + 1);
	if (!data->cmd)
		goto close;

	if (fread(data->cmd, len, 1, data->file) != 1) {
		strollpt_err("failed to fetch data command line: %s (%d).\n",
		             strerror(errno),
		             errno);
		goto free;
	}

	data->cmd[len] = '\0';

	if (fseek(data->file, 0, SEEK_END)) {
		strollpt_err("failed to probe end of data: %s (%d).\n",
		             strerror(errno),
		             errno);
		goto free;
	}

	nr = ftell(data->file);
	if (nr < 0) {
		strollpt_err("failed to probe number of data elements: "
		             "%s (%d).\n",
		             strerror(errno),
		             errno);
		goto free;
	}

	nr -= (long)sizeof(buff) + (long)len;
	nr /= (long)sizeof(unsigned int);
	if (nr != (long)data->nr) {
		strollpt_err("invalid number of data elements: "
		             "probed %ld, expected %u.\n",
		             nr,
		             data->nr);
		goto free;
	}

	data->start = sizeof(buff) + len;

	return EXIT_SUCCESS;

free:
	free(data->cmd);
close:
	fclose(data->file);

	return EXIT_FAILURE;
}

static void
strollpt_close_data(const struct strollpt_data * __restrict data)
{
	assert(data);

	free(data->cmd);
	fclose(data->file);
}

static unsigned int *
strollpt_load_data(struct strollpt_data * __restrict data,
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

int
strollpt_init(struct strollpt * ptest, int argc, char * const argv[])
{
	assert(ptest);
	assert(argc);
	assert(argv);

	memset(ptest, 0, sizeof(*ptest));

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
			if (strollpt_parse_sched_prio(optarg,
			                              &ptest->sched_prio)) {
				strollpt_usage(stderr);
				return EXIT_FAILURE;
			}

			break;

		case 'h': /* Help message. */
			strollpt_usage(stdout);
			exit(EXIT_SUCCESS);

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
		strollpt_err("invalid number of arguments.\n");
		strollpt_usage(stderr);
		return EXIT_FAILURE;
	}

	if (strollpt_parse_algo_name(argv[optind + 1], &ptest->algo_name))
		return EXIT_FAILURE;

	if (strollpt_parse_data_size(argv[optind + 2], &ptest->data_size))
		return EXIT_FAILURE;

	if (strollpt_parse_loop_nr(argv[optind + 3], &ptest->loops_nr))
		return EXIT_FAILURE;

	ptest->data_elems = strollpt_load_data(&ptest->data_desc, argv[optind]);
	if (!ptest->data_elems)
		return EXIT_FAILURE;

	return  EXIT_SUCCESS;
}

void
strollpt_fini(const struct strollpt * ptest)
{
	free(ptest->data_elems);
}
