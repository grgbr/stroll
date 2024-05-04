/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/array.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <string.h>

#define STROLLUT_ARRAY_UNSUP(_setup) \
	static void _setup(void) { cute_skip("support not compiled-in"); }

/******************************************************************************
 * Bisect search tests
 ******************************************************************************/

struct strollut_array_bisect_entry {
	unsigned int id;
	const char * str;
};

static const struct strollut_array_bisect_entry
strollut_array_bisect_array[] = {
	{ 0,    "zero" },
	{ 1,    "one" },
	{ 4,    "four" },
	{ 6,    "six" },
	{ 30,   "thirty" },
	{ 50,   "fifty" },
	{ 56,   "fifty six" },
	{ 70,   "seventy" },
	{ 100,  "one hundred" },
};

static int
strollut_array_bisect_cmp(const void * __restrict key,
                          const void * __restrict entry,
                          void *                  data __unused)
{
	intptr_t k = (intptr_t)key;
	intptr_t e = (intptr_t)
	             ((const struct strollut_array_bisect_entry *)entry)->id;

	/* Prevent from integer overflow. */
	return (k > e) - (k < e);
}

static const struct strollut_array_bisect_entry *
strollut_array_bisect_search(unsigned int                               id,
                             const struct strollut_array_bisect_entry * array,
                             unsigned int                               nr)
{
	return (const struct strollut_array_bisect_entry *)
	       stroll_array_bisect_search((const void *)((uintptr_t)id),
	                                  array,
	                                  nr,
	                                  sizeof(array[0]),
	                                  strollut_array_bisect_cmp,
	                                  NULL);
}

static void
strollut_array_bisect_check(unsigned int                               id,
                            const struct strollut_array_bisect_entry * array,
                            unsigned int                               nr,
                            const struct strollut_array_bisect_entry * found)
{
	const struct strollut_array_bisect_entry * ent;

	ent = strollut_array_bisect_search(id, array, nr);
	cute_check_ptr(ent, equal, found);

	if (found) {
		cute_check_uint(ent->id, equal, found->id);
		cute_check_str(ent->str, equal, found->str);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_array_bisect_assert)
{
	int      array[4] = { 0, 10, 100, 1000 };
	intptr_t id = 1;
	int *    ent __unused;

	cute_expect_assertion(
		ent = stroll_array_bisect_search((const void *)id,
		                                 NULL,
		                                 stroll_array_nr(array),
		                                 sizeof(array[0]),
		                                 strollut_array_bisect_cmp,
		                                 NULL));
	cute_expect_assertion(
		ent = stroll_array_bisect_search((const void *)id,
		                                 array,
		                                 0,
		                                 sizeof(array[0]),
		                                 strollut_array_bisect_cmp,
		                                 NULL));
	cute_expect_assertion(
		ent = stroll_array_bisect_search((const void *)id,
		                                 array,
		                                 stroll_array_nr(array),
		                                 sizeof(array[0]),
		                                 NULL,
		                                 NULL));
	ent = stroll_array_bisect_search(NULL,
	                                 array,
	                                 stroll_array_nr(array),
	                                 sizeof(array[0]),
	                                 strollut_array_bisect_cmp,
	                                 NULL);
}
#else
CUTE_TEST(strollut_array_bisect_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_array_bisect_integral)
{
	int      array[4] = { 0, 10, 100, 1000 };
	intptr_t id = 0;
	int *    ent __unused;

	/* Make sure zero is a valid value as key argument. */
	ent = stroll_array_bisect_search((const void *)id,
	                                 array,
	                                 stroll_array_nr(array),
	                                 sizeof(array[0]),
	                                 strollut_array_bisect_cmp,
	                                 NULL);
	cute_check_ptr(ent, equal, &array[0]);

	/* Make sure NULL keys don't raise assertions. */
	ent = stroll_array_bisect_search(NULL,
	                                 array,
	                                 stroll_array_nr(array),
	                                 sizeof(array[0]),
	                                 strollut_array_bisect_cmp,
	                                 NULL);
}

CUTE_TEST(strollut_array_bisect_one)
{
	const struct strollut_array_bisect_entry entry = {
		.id  = 10,
		.str = "ten"
	};

	strollut_array_bisect_check(10, &entry, 1, &entry);
	strollut_array_bisect_check(1, &entry, 1, NULL);
}

CUTE_TEST(strollut_array_bisect_found)
{
	unsigned int e;

	for (e = 0; e < stroll_array_nr(strollut_array_bisect_array); e++)
		strollut_array_bisect_check(
			e + INT_MAX,
			strollut_array_bisect_array,
			stroll_array_nr(strollut_array_bisect_array),
			NULL);
}

CUTE_TEST(strollut_array_bisect_notfound)
{
	unsigned int e;

	for (e = 0; e < stroll_array_nr(strollut_array_bisect_array); e++)
		strollut_array_bisect_check(
			strollut_array_bisect_array[e].id,
			strollut_array_bisect_array,
			stroll_array_nr(strollut_array_bisect_array),
			&strollut_array_bisect_array[e]);
}

CUTE_GROUP(strollut_array_bisect_group) = {
	CUTE_REF(strollut_array_bisect_assert),
	CUTE_REF(strollut_array_bisect_integral),
	CUTE_REF(strollut_array_bisect_one),
	CUTE_REF(strollut_array_bisect_found),
	CUTE_REF(strollut_array_bisect_notfound),
};

#if defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH)
static void
strollut_array_bisect_setup(void)
{

}
#else  /* !defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH) */
STROLLUT_ARRAY_UNSUP(strollut_array_bisect_setup)
#endif /* defined(CONFIG_STROLL_ARRAY_BISECT_SEARCH) */

CUTE_SUITE_EXTERN(strollut_array_bisect_suite,
                  strollut_array_bisect_group,
                  strollut_array_bisect_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

/******************************************************************************
 * Sorting tests
 ******************************************************************************/

#include "array_data.h"

#define STROLLUT_ARRAY_SORT_ALGO_SUP(_setup, _sort, _stable) \
	static void _setup(void) \
	{ \
		strollut_array_sort = _sort; \
		strollut_array_stable_sort = _stable; \
	}

static void (*strollut_array_sort)(void *                array,
                                   unsigned int          nr,
                                   size_t                size,
                                   stroll_array_cmp_fn * compare,
                                   void *                data);

static bool strollut_array_stable_sort;

static int
strollut_array_compare_min(const void * first,
                           const void * second,
                           void *       data __unused)
{
	int f = *(const int *)first;
	int s = *(const int *)second;

	/* Prevent from integer overflow. */
	return (f > s) - (f < s);
}

static void
strollut_array_check_sort(void *       array,
                          const void * expect,
                          size_t       size,
                          unsigned int nr)
{
	const char * val = array;
	const char * exp = expect;
	const char * end = &val[nr * size];

	strollut_array_sort(array, nr, size, strollut_array_compare_min, NULL);

	while (val < end) {
		cute_check_sint(*(const int *)val, equal, *(const int *)exp);
		val += size;
		exp += size;
	}
}

CUTE_TEST(strollut_array_sort_single32)
{
	int array[] = {
		0
	};
	const int expect[] = {
		0
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_single64)
{
	union { int value; char data[8]; } array[] = {
		[0] = { .value = 0 }
	};
	const union { int value; char data[8]; } expect[] = {
		[0] = { .value = 0 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_single64b)
{
	union { int value; char data[64]; } array[] = {
		[0] = { .value = 0 }
	};
	const union { int value; char data[64]; } expect[] = {
		[0] = { .value = 0 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_inorder_double32)
{
	int array[] = {
		0, 1
	};
	const int expect[] = {
		0, 1
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_inorder_double64)
{
	union { int value; char data[8]; } array[] = {
		[0] = { .value = 0 },
		[1] = { .value = 1 }
	};
	const union { int value; char data[8]; } expect[] = {
		[0] = { .value = 0 },
		[1] = { .value = 1 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_inorder_double64b)
{
	union { int value; char data[64]; } array[] = {
		[0] = { .value = 0 },
		[1] = { .value = 1 }
	};
	const union { int value; char data[64]; } expect[] = {
		[0] = { .value = 0 },
		[1] = { .value = 1 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_postorder_double32)
{
	int array[] = {
		1, 0
	};
	const int expect[] = {
		0, 1
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_postorder_double64)
{
	union { int value; char data[8]; } array[] = {
		[0] = { .value = 1 },
		[1] = { .value = 0 }
	};
	const union { int value; char data[8]; } expect[] = {
		[0] = { .value = 0 },
		[1] = { .value = 1 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_postorder_double64b)
{
	union { int value; char data[64]; } array[] = {
		[0] = { .value = 1 },
		[1] = { .value = 0 }
	};
	const union { int value; char data[64]; } expect[] = {
		[0] = { .value = 0 },
		[1] = { .value = 1 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_duplicates32)
{
	int array[] = {
		1, 1
	};
	const int expect[] = {
		1, 1
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_duplicates64)
{
	union { int value; char data[8]; } array[] = {
		[0] = { .value = 1 },
		[1] = { .value = 1 }
	};
	const union { int value; char data[8]; } expect[] = {
		[0] = { .value = 1 },
		[1] = { .value = 1 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_duplicates64b)
{
	union { int value; char data[64]; } array[] = {
		[0] = { .value = 1 },
		[1] = { .value = 1 }
	};
	const union { int value; char data[64]; } expect[] = {
		[0] = { .value = 1 },
		[1] = { .value = 1 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_presorted32)
{
	int array[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
	};
	const int expect[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_presorted64)
{
	union { int value; char data[8]; } array[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 3 },
		[4]  = { .value = 4 },
		[5]  = { .value = 5 },
		[6]  = { .value = 6 },
		[7]  = { .value = 7 },
		[8]  = { .value = 8 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 13 }
	};
	const union { int value; char data[8]; } expect[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 3 },
		[4]  = { .value = 4 },
		[5]  = { .value = 5 },
		[6]  = { .value = 6 },
		[7]  = { .value = 7 },
		[8]  = { .value = 8 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 13 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_presorted64b)
{
	union { int value; char data[64]; } array[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 3 },
		[4]  = { .value = 4 },
		[5]  = { .value = 5 },
		[6]  = { .value = 6 },
		[7]  = { .value = 7 },
		[8]  = { .value = 8 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 13 }
	};
	const union { int value; char data[64]; } expect[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 3 },
		[4]  = { .value = 4 },
		[5]  = { .value = 5 },
		[6]  = { .value = 6 },
		[7]  = { .value = 7 },
		[8]  = { .value = 8 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 13 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_reverse_sorted32)
{
	int array[] = {
		13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
	};
	const int expect[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_reverse_sorted64)
{
	union { int value; char data[8]; } array[] = {
		[0]  = { .value = 13 },
		[1]  = { .value = 12 },
		[2]  = { .value = 11 },
		[3]  = { .value = 10 },
		[4]  = { .value = 9 },
		[5]  = { .value = 8 },
		[6]  = { .value = 7 },
		[7]  = { .value = 6 },
		[8]  = { .value = 5 },
		[9]  = { .value = 4 },
		[10] = { .value = 3 },
		[11] = { .value = 2 },
		[12] = { .value = 1 },
		[13] = { .value = 0 }
	};
	const union { int value; char data[8]; } expect[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 3 },
		[4]  = { .value = 4 },
		[5]  = { .value = 5 },
		[6]  = { .value = 6 },
		[7]  = { .value = 7 },
		[8]  = { .value = 8 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 13 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_reverse_sorted64b)
{
	union { int value; char data[64]; } array[] = {
		[0]  = { .value = 13 },
		[1]  = { .value = 12 },
		[2]  = { .value = 11 },
		[3]  = { .value = 10 },
		[4]  = { .value = 9 },
		[5]  = { .value = 8 },
		[6]  = { .value = 7 },
		[7]  = { .value = 6 },
		[8]  = { .value = 5 },
		[9]  = { .value = 4 },
		[10] = { .value = 3 },
		[11] = { .value = 2 },
		[12] = { .value = 1 },
		[13] = { .value = 0 }
	};
	const union { int value; char data[64]; } expect[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 3 },
		[4]  = { .value = 4 },
		[5]  = { .value = 5 },
		[6]  = { .value = 6 },
		[7]  = { .value = 7 },
		[8]  = { .value = 8 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 13 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_unsorted32)
{
	int array[] = {
		2, 12, 13, 0, 1, 3, 10, 9, 8, 11, 4, 6, 5, 7
	};
	const int expect[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_unsorted64)
{
	union { int value; char data[8]; } array[] = {
		[0]  = { .value = 2 },
		[1]  = { .value = 12 },
		[2]  = { .value = 13 },
		[3]  = { .value = 0 },
		[4]  = { .value = 1 },
		[5]  = { .value = 3 },
		[6]  = { .value = 10 },
		[7]  = { .value = 9 },
		[8]  = { .value = 8 },
		[9]  = { .value = 11 },
		[10] = { .value = 4 },
		[11] = { .value = 6 },
		[12] = { .value = 5 },
		[13] = { .value = 7 }
	};
	const union { int value; char data[8]; } expect[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 3 },
		[4]  = { .value = 4 },
		[5]  = { .value = 5 },
		[6]  = { .value = 6 },
		[7]  = { .value = 7 },
		[8]  = { .value = 8 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 13 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_unsorted64b)
{
	union { int value; char data[64]; } array[] = {
		[0]  = { .value = 2 },
		[1]  = { .value = 12 },
		[2]  = { .value = 13 },
		[3]  = { .value = 0 },
		[4]  = { .value = 1 },
		[5]  = { .value = 3 },
		[6]  = { .value = 10 },
		[7]  = { .value = 9 },
		[8]  = { .value = 8 },
		[9]  = { .value = 11 },
		[10] = { .value = 4 },
		[11] = { .value = 6 },
		[12] = { .value = 5 },
		[13] = { .value = 7 }
	};
	const union { int value; char data[64]; } expect[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 3 },
		[4]  = { .value = 4 },
		[5]  = { .value = 5 },
		[6]  = { .value = 6 },
		[7]  = { .value = 7 },
		[8]  = { .value = 8 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 13 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_unsorted_duplicates32)
{
	int array[] = {
		2, 12, 12, 0, 1, 3, 10, 9, 3, 11, 4, 6, 5, 2
	};
	const int expect[] = {
		0, 1, 2, 2, 3, 3, 4, 5, 6, 9, 10, 11, 12, 12
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_unsorted_duplicates64)
{
	union { int value; char data[8]; } array[] = {
		[0]  = { .value = 2 },
		[1]  = { .value = 12 },
		[2]  = { .value = 12 },
		[3]  = { .value = 0 },
		[4]  = { .value = 1 },
		[5]  = { .value = 3 },
		[6]  = { .value = 10 },
		[7]  = { .value = 9 },
		[8]  = { .value = 3 },
		[9]  = { .value = 11 },
		[10] = { .value = 4 },
		[11] = { .value = 6 },
		[12] = { .value = 5 },
		[13] = { .value = 2 }
	};
	const union { int value; char data[8]; } expect[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 2 },
		[4]  = { .value = 3 },
		[5]  = { .value = 3 },
		[6]  = { .value = 4 },
		[7]  = { .value = 5 },
		[8]  = { .value = 6 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 12 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

CUTE_TEST(strollut_array_sort_unsorted_duplicates64b)
{
	union { int value; char data[64]; } array[] = {
		[0]  = { .value = 2 },
		[1]  = { .value = 12 },
		[2]  = { .value = 12 },
		[3]  = { .value = 0 },
		[4]  = { .value = 1 },
		[5]  = { .value = 3 },
		[6]  = { .value = 10 },
		[7]  = { .value = 9 },
		[8]  = { .value = 3 },
		[9]  = { .value = 11 },
		[10] = { .value = 4 },
		[11] = { .value = 6 },
		[12] = { .value = 5 },
		[13] = { .value = 2 }
	};
	const union { int value; char data[64]; } expect[] = {
		[0]  = { .value = 0 },
		[1]  = { .value = 1 },
		[2]  = { .value = 2 },
		[3]  = { .value = 2 },
		[4]  = { .value = 3 },
		[5]  = { .value = 3 },
		[6]  = { .value = 4 },
		[7]  = { .value = 5 },
		[8]  = { .value = 6 },
		[9]  = { .value = 9 },
		[10] = { .value = 10 },
		[11] = { .value = 11 },
		[12] = { .value = 12 },
		[13] = { .value = 12 }
	};

	strollut_array_check_sort(array,
	                          expect,
	                          sizeof(array[0]),
	                          stroll_array_nr(expect));
}

static int
strollut_array_compare_elem_inorder(const void * first,
                                    const void * second,
                                    void *       data __unused)
{
	const struct strollut_array_elem * fst =
		(const struct strollut_array_elem *)first;
	const struct strollut_array_elem * snd =
		(const struct strollut_array_elem *)second;

	return fst->key - snd->key;
}

static int
strollut_array_compare_elem_postorder(const void * first,
                                      const void * second,
                                      void *       data __unused)
{
	const struct strollut_array_elem * fst =
		(const struct strollut_array_elem *)first;
	const struct strollut_array_elem * snd =
		(const struct strollut_array_elem *)second;

	return snd->key - fst->key;
}

static struct strollut_array_elem
strollut_array_tosort[STROLLUT_ARRAY_NR];

static void
strollut_array_check_stable(const struct strollut_array_elem * array,
                            const struct strollut_array_elem * expect,
                            stroll_array_cmp_fn *              cmp)
{
	unsigned int e;

	memcpy(strollut_array_tosort, array, sizeof(strollut_array_tosort));

	strollut_array_sort((void *)strollut_array_tosort,
	                    stroll_array_nr(strollut_array_tosort),
	                    sizeof(strollut_array_tosort[0]),
	                    cmp,
	                    NULL);

	for (e = 0; e < stroll_array_nr(strollut_array_tosort); e++) {
		cute_check_sint(strollut_array_tosort[e].key,
		                equal,
		                expect[e].key);
		cute_check_ptr(strollut_array_tosort[e].ptr,
		               equal,
		               expect[e].ptr);
	}
}

static void
strollut_array_check_unstable(const struct strollut_array_elem * array,
                              const struct strollut_array_elem * expect,
                              stroll_array_cmp_fn *              cmp)
{
	unsigned int e;

	memcpy(strollut_array_tosort, array, sizeof(strollut_array_tosort));

	strollut_array_sort((void *)strollut_array_tosort,
	                    stroll_array_nr(strollut_array_tosort),
	                    sizeof(strollut_array_tosort[0]),
	                    cmp,
	                    NULL);

	for (e = 0; e < stroll_array_nr(strollut_array_tosort); e++) {
		cute_check_sint(strollut_array_tosort[e].key,
		                equal,
		                expect[e].key);
	}
}

CUTE_TEST(strollut_array_sort_inorder)
{
	if (strollut_array_stable_sort)
		strollut_array_check_stable(
			strollut_array_input,
			strollut_array_inorder,
			strollut_array_compare_elem_inorder);
	else
		strollut_array_check_unstable(
			strollut_array_input,
			strollut_array_inorder,
			strollut_array_compare_elem_inorder);
}

CUTE_TEST(strollut_array_sort_postorder)
{
	if (strollut_array_stable_sort)
		strollut_array_check_stable(
			strollut_array_input,
			strollut_array_postorder,
			strollut_array_compare_elem_postorder);
	else
		strollut_array_check_unstable(
			strollut_array_input,
			strollut_array_postorder,
			strollut_array_compare_elem_postorder);
}

CUTE_GROUP(strollut_array_sort_group) = {
	CUTE_REF(strollut_array_sort_single32),
	CUTE_REF(strollut_array_sort_single64),
	CUTE_REF(strollut_array_sort_single64b),

	CUTE_REF(strollut_array_sort_inorder_double32),
	CUTE_REF(strollut_array_sort_inorder_double64),
	CUTE_REF(strollut_array_sort_inorder_double64b),

	CUTE_REF(strollut_array_sort_postorder_double32),
	CUTE_REF(strollut_array_sort_postorder_double64),
	CUTE_REF(strollut_array_sort_postorder_double64b),

	CUTE_REF(strollut_array_sort_duplicates32),
	CUTE_REF(strollut_array_sort_duplicates64),
	CUTE_REF(strollut_array_sort_duplicates64b),

	CUTE_REF(strollut_array_sort_presorted32),
	CUTE_REF(strollut_array_sort_presorted64),
	CUTE_REF(strollut_array_sort_presorted64b),

	CUTE_REF(strollut_array_sort_reverse_sorted32),
	CUTE_REF(strollut_array_sort_reverse_sorted64),
	CUTE_REF(strollut_array_sort_reverse_sorted64b),

	CUTE_REF(strollut_array_sort_unsorted32),
	CUTE_REF(strollut_array_sort_unsorted64),
	CUTE_REF(strollut_array_sort_unsorted64b),

	CUTE_REF(strollut_array_sort_unsorted_duplicates32),
	CUTE_REF(strollut_array_sort_unsorted_duplicates64),
	CUTE_REF(strollut_array_sort_unsorted_duplicates64b),

	CUTE_REF(strollut_array_sort_inorder),
	CUTE_REF(strollut_array_sort_postorder)
};

#if defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT)
STROLLUT_ARRAY_SORT_ALGO_SUP(strollut_array_bubble_setup,
                             stroll_array_bubble_sort,
                             true)
#else   /* !defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT) */
STROLLUT_ARRAY_UNSUP(strollut_array_bubble_setup)
#endif  /* defined(CONFIG_STROLL_ARRAY_BUBBLE_SORT) */

CUTE_SUITE_STATIC(strollut_array_bubble_suite,
                  strollut_array_sort_group,
                  strollut_array_bubble_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_ARRAY_SELECT_SORT)
STROLLUT_ARRAY_SORT_ALGO_SUP(strollut_array_select_setup,
                             stroll_array_select_sort,
                             false)
#else   /* !defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */
STROLLUT_ARRAY_UNSUP(strollut_array_select_setup)
#endif  /* defined(CONFIG_STROLL_ARRAY_SELECT_SORT) */

CUTE_SUITE_STATIC(strollut_array_select_suite,
                  strollut_array_sort_group,
                  strollut_array_select_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_ARRAY_INSERT_SORT)
STROLLUT_ARRAY_SORT_ALGO_SUP(strollut_array_insert_setup,
                             stroll_array_insert_sort,
                             true)
#else   /* !defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */
STROLLUT_ARRAY_UNSUP(strollut_array_insert_setup)
#endif  /* defined(CONFIG_STROLL_ARRAY_INSERT_SORT) */

CUTE_SUITE_STATIC(strollut_array_insert_suite,
                  strollut_array_sort_group,
                  strollut_array_insert_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_ARRAY_QUICK_SORT)
STROLLUT_ARRAY_SORT_ALGO_SUP(strollut_array_quick_setup,
                             stroll_array_quick_sort,
                             false)
#else   /* !defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */
STROLLUT_ARRAY_UNSUP(strollut_array_quick_setup)
#endif  /* defined(CONFIG_STROLL_ARRAY_QUICK_SORT) */

CUTE_SUITE_STATIC(strollut_array_quick_suite,
                  strollut_array_sort_group,
                  strollut_array_quick_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_ARRAY_MERGE_SORT)

static void
strollut_array_merge_sort(void * __restrict     array,
                          unsigned int          nr,
                          size_t                size,
                          stroll_array_cmp_fn * compare,
                          void *                data)
{
	int err;

	err = stroll_array_merge_sort(array, nr, size, compare, data);
	cute_check_sint(err, equal, 0);
}

STROLLUT_ARRAY_SORT_ALGO_SUP(strollut_array_merge_setup,
                             strollut_array_merge_sort,
                             true)
#else   /* !defined(CONFIG_STROLL_ARRAY_MERGE_SORT) */
STROLLUT_ARRAY_UNSUP(strollut_array_merge_setup)
#endif  /* defined(CONFIG_STROLL_ARRAY_MERGE_SORT) */

CUTE_SUITE_STATIC(strollut_array_merge_suite,
                  strollut_array_sort_group,
                  strollut_array_merge_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

CUTE_GROUP(strollut_array_group) = {
	CUTE_REF(strollut_array_bisect_suite),
	CUTE_REF(strollut_array_bubble_suite),
	CUTE_REF(strollut_array_select_suite),
	CUTE_REF(strollut_array_insert_suite),
	CUTE_REF(strollut_array_quick_suite),
	CUTE_REF(strollut_array_merge_suite)
};

CUTE_SUITE_EXTERN(strollut_array_suite,
                  strollut_array_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
