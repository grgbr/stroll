/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/heap.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <stdlib.h>
#include <string.h>

#define STROLLUT_HEAP_UNSUP(_test) \
static void _test(void) { cute_skip("support not compiled-in"); }

typedef int (strollut_qsort_cmp_fn)(const void *, const void *);

struct strollut_heap_elem {
	unsigned int id;
	const char * str;
};

static int
strollut_heap_compare_num_min(const void * first,
                              const void * second,
                              void *       data __unused)
{
	const struct strollut_heap_elem * fst =
		(const struct strollut_heap_elem *)first;
	const struct strollut_heap_elem * snd =
		(const struct strollut_heap_elem *)second;

	return (fst->id > snd->id) - (fst->id < snd->id);
}

static int
strollut_qsort_compare_num_min(const void * first, const void * second)
{
	return strollut_heap_compare_num_min(first, second, NULL);
}

#if defined(CONFIG_STROLL_FHEAP)

static const struct strollut_heap_elem strollut_heap_array1[] = {
	{ 0, "0zero" },
};

static const struct strollut_heap_elem strollut_heap_array2[] = {
	{ 0, "0zero" },
	{ 1, "1one" }
};

static const struct strollut_heap_elem strollut_heap_array3[] = {
	{ 0, "0zero" },
	{ 1, "1one" },
	{ 2, "2two" }
};

static const struct strollut_heap_elem strollut_heap_array4[] = {
	{ 0, "0zero" },
	{ 1, "1one" },
	{ 2, "2two" },
	{ 3, "3three" }
};

static const struct strollut_heap_elem strollut_heap_array5[] = {
	{ 0, "0zero" },
	{ 1, "1one" },
	{ 2, "2two" },
	{ 3, "3three" },
	{ 4, "4four" }
};

static const struct strollut_heap_elem strollut_heap_array6[] = {
	{ 0, "0zero" },
	{ 1, "1one" },
	{ 2, "2two" },
	{ 3, "3three" },
	{ 4, "4four" },
	{ 5, "5five" }
};

static const struct strollut_heap_elem strollut_heap_array7[] = {
	{ 0, "0zero" },
	{ 1, "1one" },
	{ 2, "2two" },
	{ 3, "3three" },
	{ 4, "4four" },
	{ 5, "5five" },
	{ 6, "6six" }
};

static const struct strollut_heap_elem strollut_heap_array8[] = {
	{ 0, "0zero" },
	{ 1, "1one" },
	{ 2, "2two" },
	{ 3, "3three" },
	{ 4, "4four" },
	{ 5, "5five" },
	{ 6, "6six" },
	{ 7, "7seven" }
};

static void
strollut_heap_check_recurs(unsigned int                      e,
                           const struct strollut_heap_elem * array,
                           unsigned int                      nr,
                           stroll_array_cmp_fn *             cmp)
{
	cute_check_uint(e, lower_equal, nr);

	if ((e == 0) || (e == nr))
		return;

	cute_check_sint(cmp(&array[(e - 1) / 2], &array[e], NULL),
	                greater_equal,
	                0);

	strollut_heap_check_recurs((2 * e) + 1, array, nr, cmp);
	strollut_heap_check_recurs((2 * e) + 2, array, nr, cmp);
}

static void
strollut_heap_check_extract(const struct strollut_heap_elem * array,
                            unsigned int                      nr,
                            stroll_array_cmp_fn *             heap_cmp,
                            strollut_qsort_cmp_fn *           qsort_cmp)
{
	struct strollut_heap_elem heap[nr];
	struct strollut_heap_elem sorted[nr];
	unsigned int              e;

	memcpy(heap, array, sizeof(heap));

	memcpy(sorted, array, sizeof(sorted));
	qsort(sorted, nr, sizeof(sorted[0]), qsort_cmp);

	for (e = 0; e < nr; e++) {
		struct strollut_heap_elem elem;

		stroll_fheap_extract(&elem,
		                     heap,
		                     nr - e,
		                     sizeof(array[0]),
		                     heap_cmp,
		                     NULL);

		cute_check_uint(elem.id, equal, sorted[e].id);
		cute_check_str(elem.str, equal, sorted[e].str);

		strollut_heap_check_recurs(0,
		                           heap,
		                           nr - e,
		                           heap_cmp);
	}
}

CUTE_TEST(strollut_fheap_extract1)
{
	strollut_heap_check_extract(strollut_heap_array1,
	                            stroll_array_nr(strollut_heap_array1),
	                            strollut_heap_compare_num_min,
	                            strollut_qsort_compare_num_min);
}

CUTE_TEST(strollut_fheap_extract2)
{
	strollut_heap_check_extract(strollut_heap_array2,
	                            stroll_array_nr(strollut_heap_array2),
	                            strollut_heap_compare_num_min,
	                            strollut_qsort_compare_num_min);
}

CUTE_TEST(strollut_fheap_extract3)
{
	strollut_heap_check_extract(strollut_heap_array3,
	                            stroll_array_nr(strollut_heap_array3),
	                            strollut_heap_compare_num_min,
	                            strollut_qsort_compare_num_min);
}

CUTE_TEST(strollut_fheap_extract4)
{
	strollut_heap_check_extract(strollut_heap_array4,
	                            stroll_array_nr(strollut_heap_array4),
	                            strollut_heap_compare_num_min,
	                            strollut_qsort_compare_num_min);
}

CUTE_TEST(strollut_fheap_extract5)
{
	strollut_heap_check_extract(strollut_heap_array5,
	                            stroll_array_nr(strollut_heap_array5),
	                            strollut_heap_compare_num_min,
	                            strollut_qsort_compare_num_min);
}

CUTE_TEST(strollut_fheap_extract6)
{
	strollut_heap_check_extract(strollut_heap_array6,
	                            stroll_array_nr(strollut_heap_array6),
	                            strollut_heap_compare_num_min,
	                            strollut_qsort_compare_num_min);
}

CUTE_TEST(strollut_fheap_extract7)
{
	strollut_heap_check_extract(strollut_heap_array7,
	                            stroll_array_nr(strollut_heap_array7),
	                            strollut_heap_compare_num_min,
	                            strollut_qsort_compare_num_min);
}

CUTE_TEST(strollut_fheap_extract8)
{
	strollut_heap_check_extract(strollut_heap_array8,
	                            stroll_array_nr(strollut_heap_array8),
	                            strollut_heap_compare_num_min,
	                            strollut_qsort_compare_num_min);
}

static void
strollut_heap_check_build(const struct strollut_heap_elem * array,
                          unsigned int                      nr,
                          stroll_array_cmp_fn *             heap_cmp)
{
	struct strollut_heap_elem heap[nr];

	memcpy(heap, array, sizeof(heap));
	stroll_fheap_build(heap, nr, sizeof(heap[0]), heap_cmp, NULL);

	strollut_heap_check_recurs(0, heap, nr, heap_cmp);
}

CUTE_TEST(strollut_fheap_build1)
{
	strollut_heap_check_build(strollut_heap_array1,
	                          stroll_array_nr(strollut_heap_array1),
	                          strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_fheap_build2)
{
	strollut_heap_check_build(strollut_heap_array2,
	                          stroll_array_nr(strollut_heap_array2),
	                          strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_fheap_build3)
{
	strollut_heap_check_build(strollut_heap_array3,
	                          stroll_array_nr(strollut_heap_array3),
	                          strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_fheap_build4)
{
	strollut_heap_check_build(strollut_heap_array4,
	                          stroll_array_nr(strollut_heap_array4),
	                          strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_fheap_build5)
{
	strollut_heap_check_build(strollut_heap_array5,
	                          stroll_array_nr(strollut_heap_array5),
	                          strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_fheap_build6)
{
	strollut_heap_check_build(strollut_heap_array6,
	                          stroll_array_nr(strollut_heap_array6),
	                          strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_fheap_build7)
{
	strollut_heap_check_build(strollut_heap_array7,
	                          stroll_array_nr(strollut_heap_array7),
	                          strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_fheap_build8)
{
	strollut_heap_check_build(strollut_heap_array8,
	                          stroll_array_nr(strollut_heap_array8),
	                          strollut_heap_compare_num_min);
}

#else  /* !defined(CONFIG_STROLL_FHEAP) */

STROLLUT_HEAP_UNSUP(strollut_fheap_extract1)
STROLLUT_HEAP_UNSUP(strollut_fheap_extract2)
STROLLUT_HEAP_UNSUP(strollut_fheap_extract3)
STROLLUT_HEAP_UNSUP(strollut_fheap_extract4)
STROLLUT_HEAP_UNSUP(strollut_fheap_extract5)
STROLLUT_HEAP_UNSUP(strollut_fheap_extract6)
STROLLUT_HEAP_UNSUP(strollut_fheap_extract7)
STROLLUT_HEAP_UNSUP(strollut_fheap_extract8)

STROLLUT_HEAP_UNSUP(strollut_fheap_build1)
STROLLUT_HEAP_UNSUP(strollut_fheap_build2)
STROLLUT_HEAP_UNSUP(strollut_fheap_build3)
STROLLUT_HEAP_UNSUP(strollut_fheap_build4)
STROLLUT_HEAP_UNSUP(strollut_fheap_build5)
STROLLUT_HEAP_UNSUP(strollut_fheap_build6)
STROLLUT_HEAP_UNSUP(strollut_fheap_build7)
STROLLUT_HEAP_UNSUP(strollut_fheap_build8)

#endif /* defined(CONFIG_STROLL_FHEAP) */

CUTE_GROUP(strollut_fheap_group) = {
	CUTE_REF(strollut_fheap_extract1),
	CUTE_REF(strollut_fheap_extract2),
	CUTE_REF(strollut_fheap_extract3),
	CUTE_REF(strollut_fheap_extract4),
	CUTE_REF(strollut_fheap_extract5),
	CUTE_REF(strollut_fheap_extract6),
	CUTE_REF(strollut_fheap_extract7),
	CUTE_REF(strollut_fheap_extract8),

	CUTE_REF(strollut_fheap_build1),
	CUTE_REF(strollut_fheap_build2),
	CUTE_REF(strollut_fheap_build3),
	CUTE_REF(strollut_fheap_build4),
	CUTE_REF(strollut_fheap_build5),
	CUTE_REF(strollut_fheap_build6),
	CUTE_REF(strollut_fheap_build7),
	CUTE_REF(strollut_fheap_build8),
};

CUTE_SUITE_STATIC(strollut_fheap_suite,
                  strollut_fheap_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

CUTE_GROUP(strollut_heap_group) = {
	CUTE_REF(strollut_fheap_suite)
};

CUTE_SUITE_EXTERN(strollut_heap_suite,
                  strollut_heap_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
