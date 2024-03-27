/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/bisect.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>

struct strollut_bisect_entry {
	unsigned int id;
	const char * str;
};

static const struct strollut_bisect_entry strollut_bisect_array[] = {
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
strollut_bisect_cmp(const void * __restrict key, const void * __restrict entry)
{
	return (int)((intptr_t)(key) -
	             (intptr_t)
	             ((const struct strollut_bisect_entry *)entry)->id);
}

static const struct strollut_bisect_entry *
strollut_bisect_search(unsigned int                         id,
                       const struct strollut_bisect_entry * array,
                       unsigned int                         nr)
{
	return (const struct strollut_bisect_entry *)
	       stroll_bisect_search((const void *)((uintptr_t)id),
	                            array,
	                            sizeof(array[0]),
	                            nr,
	                            strollut_bisect_cmp);
}

static void
strollut_bisect_check(unsigned int                         id,
                      const struct strollut_bisect_entry * array,
                      unsigned int                         nr,
                      const struct strollut_bisect_entry * found)
{
	const struct strollut_bisect_entry * ent;

	ent = strollut_bisect_search(id, array, nr);
	cute_check_ptr(ent, equal, found);

	if (found) {
		cute_check_uint(ent->id, equal, found->id);
		cute_check_str(ent->str, equal, found->str);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bisect_assert)
{
	int      array[4] = { 0, 10, 100, 1000 };
	intptr_t id = 1;
	int *    ent __unused;

	cute_expect_assertion(
		ent = stroll_bisect_search((const void *)id,
		                           NULL,
		                           sizeof(array[0]),
		                           stroll_array_nr(array),
		                           strollut_bisect_cmp));
	cute_expect_assertion(
		ent = stroll_bisect_search((const void *)id,
		                           array,
		                           sizeof(array[0]),
		                           0,
		                           strollut_bisect_cmp));
	cute_expect_assertion(
		ent = stroll_bisect_search((const void *)id,
		                           array,
		                           sizeof(array[0]),
		                           stroll_array_nr(array),
		                           NULL));
	ent = stroll_bisect_search(NULL,
	                           array,
	                           sizeof(array[0]),
	                           stroll_array_nr(array),
	                           strollut_bisect_cmp);
}
#else
CUTE_TEST(strollut_bisect_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bisect_integral)
{
	int      array[4] = { 0, 10, 100, 1000 };
	intptr_t id = 0;
	int *    ent __unused;

	/* Make sure zero is a valid value as key argument. */
	ent = stroll_bisect_search((const void *)id,
	                           array,
	                           sizeof(array[0]),
	                           stroll_array_nr(array),
	                           strollut_bisect_cmp);
	cute_check_ptr(ent, equal, &array[0]);

	/* Make sure NULL keys don't raise assertions. */
	ent = stroll_bisect_search(NULL,
	                           array,
	                           sizeof(array[0]),
	                           stroll_array_nr(array),
	                           strollut_bisect_cmp);
}

CUTE_TEST(strollut_bisect_one)
{
	const struct strollut_bisect_entry entry = { .id  = 10, .str = "ten" };

	strollut_bisect_check(10, &entry, 1, &entry);
	strollut_bisect_check(1, &entry, 1, NULL);
}

CUTE_TEST(strollut_bisect_found)
{
	unsigned int e;

	for (e = 0; e < stroll_array_nr(strollut_bisect_array); e++)
		strollut_bisect_check(e + INT_MAX,
		                      strollut_bisect_array,
		                      stroll_array_nr(strollut_bisect_array),
		                      NULL);
}

CUTE_TEST(strollut_bisect_notfound)
{
	unsigned int e;

	for (e = 0; e < stroll_array_nr(strollut_bisect_array); e++)
		strollut_bisect_check(strollut_bisect_array[e].id,
		                      strollut_bisect_array,
		                      stroll_array_nr(strollut_bisect_array),
		                      &strollut_bisect_array[e]);
}

CUTE_GROUP(strollut_bisect_group) = {
	CUTE_REF(strollut_bisect_assert),
	CUTE_REF(strollut_bisect_integral),
	CUTE_REF(strollut_bisect_one),
	CUTE_REF(strollut_bisect_found),
	CUTE_REF(strollut_bisect_notfound),
};

CUTE_SUITE_EXTERN(strollut_bisect_suite,
                  strollut_bisect_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
