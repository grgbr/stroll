/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _STROLL_PTEST_H
#define _STROLL_PTEST_H

#include "stroll/cdefs.h"
#include <stdio.h>
#include <time.h>
#include <errno.h>

#define strollpt_err(_format, ...) \
	fprintf(stderr, \
	        "%s: " _format, \
	        program_invocation_short_name, \
                ## __VA_ARGS__)

static inline int
strollpt_array_compare_min(const void * __restrict a,
                           const void * __restrict b,
                           void *                  data __unused)
{
	unsigned int _a = *(const unsigned int *)a;
	unsigned int _b = *(const unsigned int *)b;

	return (_a > _b) - (_a < _b);
}

static inline int
strollpt_array_compare_max(const void * __restrict a,
                           const void * __restrict b,
                           void *                  data __unused)
{
	return 0 - strollpt_array_compare_min(a, b, NULL);
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

extern int strollpt_setup_sched_prio(int priority);

enum strollpt_endian {
	STROLLPT_NATIVE_ENDIAN,
	STROLLPT_LITTLE_ENDIAN,
	STROLLPT_BIG_ENDIAN
};

struct strollpt_data {
	FILE *               file;
	enum strollpt_endian endian;
	unsigned short       order;
	unsigned short       singles;
	unsigned int         nr;
	long                 start;
	char *               cmd;
};

struct strollpt {
	const char *         algo_name;
	size_t               data_size;
	unsigned int         loops_nr;
	int                  sched_prio;
	struct strollpt_data data_desc;
	unsigned int *       data_elems;
};

extern int
strollpt_init(struct strollpt * ptest, int argc, char * const argv[]);

extern void
strollpt_fini(const struct strollpt * ptest);

#endif /* _STROLL_PTEST_H */
