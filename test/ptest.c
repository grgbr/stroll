/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "ptest.h"
#include "stroll/cdefs.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
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

int
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

int
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

int
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

int
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

int
strollpt_open_data(struct strollpt_data * __restrict data,
                   const char * __restrict           pathname)
{
	assert(data);
	assert(pathname);

	char           buff[9];
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

	memcpy(&data->ratio, &buff[1], sizeof(data->ratio));
	memcpy(&data->nr, &buff[3], sizeof(data->nr));
	memcpy(&len, &buff[7], sizeof(len));

	switch (data->endian) {
	case STROLLPT_NATIVE_ENDIAN:
		break;
	case STROLLPT_LITTLE_ENDIAN:
		data->ratio = le16toh(data->ratio);
		data->nr = le32toh(data->nr);
		len = le16toh(len);
		break;
	case STROLLPT_BIG_ENDIAN:
		data->ratio = be16toh(data->ratio);
		data->nr = be32toh(data->nr);
		len = be16toh(len);
	}

	if (data->ratio > 100) {
		strollpt_err("invalid data ordering ratio '%hu'.\n",
		             data->ratio);
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

void
strollpt_close_data(const struct strollpt_data * __restrict data)
{
	assert(data);

	free(data->cmd);
	fclose(data->file);
}
