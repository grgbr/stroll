/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _STROLL_PTEST_H
#define _STROLL_PTEST_H

#include <stdio.h>
#include <time.h>
#include <errno.h>

#define strollpt_err(_format, ...) \
	fprintf(stderr, \
	        "%s: " _format, \
	        program_invocation_short_name, \
                ## __VA_ARGS__)

static inline int
strollpt_compare_min(const void * __restrict a, const void * __restrict b)
{
	unsigned int _a = *(const unsigned int *)a;
	unsigned int _b = *(const unsigned int *)b;

	if (_a < _b)
		return -1;
	else if (_a > _b)
		return 1;
	else
		return 0;
}

static inline int
strollpt_compare_max(const void * __restrict a, const void * __restrict b)
{
	return 0 - strollpt_compare_min(a, b);
}

struct strollpt_stats {
	unsigned long long min;
	unsigned long long max;
	unsigned long long med;
	unsigned int       count;
	double             mean;
	double             stdev;
};

extern void
strollpt_calc_stats(struct strollpt_stats * __restrict stats,
                    unsigned long long * __restrict    values,
                    unsigned int                       nr);

extern struct timespec
strollpt_tspec_sub(const struct timespec * __restrict a,
                   const struct timespec * __restrict b);

static inline unsigned long long
strollpt_tspec2ns(const struct timespec * __restrict tspec)
{
	return ((unsigned long long)tspec->tv_sec * 1000000000ULL) +
	       (unsigned long long)tspec->tv_nsec;
}

extern int
strollpt_parse_loop_nr(const char * __restrict   arg,
                       unsigned int * __restrict loop_nr);

extern int
strollpt_parse_sched_prio(const char * __restrict arg,
                          int * __restrict        priority);

extern int strollpt_setup_sched_prio(int priority);

enum strollpt_endian {
	STROLLPT_NATIVE_ENDIAN,
	STROLLPT_LITTLE_ENDIAN,
	STROLLPT_BIG_ENDIAN
};

struct strollpt_data {
	FILE *               file;
	enum strollpt_endian endian;
	unsigned short       ratio;
	unsigned int         nr;
	long                 start;
	char *               cmd;
};

extern int
strollpt_init_data_iter(const struct strollpt_data * __restrict data);

extern int
strollpt_step_data_iter(const struct strollpt_data * __restrict data,
                        unsigned int * __restrict               element);

extern int
strollpt_open_data(struct strollpt_data * __restrict data,
                   const char * __restrict           pathname);

extern void
strollpt_close_data(const struct strollpt_data * __restrict data);

#endif /* _STROLL_PTEST_H */
