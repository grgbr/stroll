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

typedef void strollut_heap_insert_fn(const void *,
                                     void * __restrict,
                                     unsigned int,
                                     size_t,
                                     stroll_array_cmp_fn *,
                                     void *)
	__stroll_nonull(1, 2, 5);

typedef void strollut_heap_extract_fn(void * __restrict,
                                      void * __restrict,
                                      unsigned int,
                                      size_t,
                                      stroll_array_cmp_fn *,
                                      void *)
	__stroll_nonull(1, 2, 5);

typedef void strollut_heap_build_fn(void * __restrict,
                                    unsigned int,
                                    size_t,
                                    stroll_array_cmp_fn *,
                                    void *)
	__stroll_nonull(1, 4);

struct strollut_heap_elem {
	unsigned int id;
	const char   str[28];
};

typedef void strollut_heap_check_fn(const struct strollut_heap_elem *,
                                    unsigned int,
                                    stroll_array_cmp_fn *);

struct strollut_heap_iface {
	strollut_heap_build_fn *   build;
	strollut_heap_insert_fn *  insert;
	strollut_heap_extract_fn * extract;
	strollut_heap_check_fn *   check;
};

static struct strollut_heap_iface strollut_heap_algo;

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
strollut_heap_compare_str_min(const void * first,
                              const void * second,
                              void *       data __unused)
{
	const struct strollut_heap_elem * fst =
		(const struct strollut_heap_elem *)first;
	const struct strollut_heap_elem * snd =
		(const struct strollut_heap_elem *)second;

	return strcmp(fst->str, snd->str);
}

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

#define STROLLUT_HEAP_ELEM_NR (stroll_array_nr(strollut_heap_array8))

static void
strollut_heap_check_extract(const struct strollut_heap_elem * array,
                            unsigned int                      nr,
                            stroll_array_cmp_fn *             compare)
{
	struct strollut_heap_elem heap[nr];
	struct strollut_heap_elem sorted[nr];
	unsigned int              e;

	memcpy(sorted, array, sizeof(sorted));

	memcpy(heap, array, sizeof(heap));
	strollut_heap_algo.build(heap, nr, sizeof(heap[0]), compare, NULL);

	for (e = 0; e < nr; e++) {
		struct strollut_heap_elem elem;

		strollut_heap_algo.extract(&elem,
		                           heap,
		                           nr - e,
		                           sizeof(array[0]),
		                           compare,
		                           NULL);

		cute_check_uint(elem.id, equal, sorted[e].id);
		cute_check_str(elem.str, equal, sorted[e].str);

		strollut_heap_algo.check(heap, nr - e, compare);
	}
}

CUTE_TEST(strollut_heap_extract1_num)
{
	strollut_heap_check_extract(strollut_heap_array1,
	                            stroll_array_nr(strollut_heap_array1),
	                            strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_extract1_str)
{
	strollut_heap_check_extract(strollut_heap_array1,
	                            stroll_array_nr(strollut_heap_array1),
	                            strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_extract2_num)
{
	strollut_heap_check_extract(strollut_heap_array2,
	                            stroll_array_nr(strollut_heap_array2),
	                            strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_extract2_str)
{
	strollut_heap_check_extract(strollut_heap_array2,
	                            stroll_array_nr(strollut_heap_array2),
	                            strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_extract3_num)
{
	strollut_heap_check_extract(strollut_heap_array3,
	                            stroll_array_nr(strollut_heap_array3),
	                            strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_extract3_str)
{
	strollut_heap_check_extract(strollut_heap_array3,
	                            stroll_array_nr(strollut_heap_array3),
	                            strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_extract4_num)
{
	strollut_heap_check_extract(strollut_heap_array4,
	                            stroll_array_nr(strollut_heap_array4),
	                            strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_extract4_str)
{
	strollut_heap_check_extract(strollut_heap_array4,
	                            stroll_array_nr(strollut_heap_array4),
	                            strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_extract5_num)
{
	strollut_heap_check_extract(strollut_heap_array5,
	                            stroll_array_nr(strollut_heap_array5),
	                            strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_extract5_str)
{
	strollut_heap_check_extract(strollut_heap_array5,
	                            stroll_array_nr(strollut_heap_array5),
	                            strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_extract6_num)
{
	strollut_heap_check_extract(strollut_heap_array6,
	                            stroll_array_nr(strollut_heap_array6),
	                            strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_extract6_str)
{
	strollut_heap_check_extract(strollut_heap_array6,
	                            stroll_array_nr(strollut_heap_array6),
	                            strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_extract7_num)
{
	strollut_heap_check_extract(strollut_heap_array7,
	                            stroll_array_nr(strollut_heap_array7),
	                            strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_extract7_str)
{
	strollut_heap_check_extract(strollut_heap_array7,
	                            stroll_array_nr(strollut_heap_array7),
	                            strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_extract8_num)
{
	strollut_heap_check_extract(strollut_heap_array8,
	                            stroll_array_nr(strollut_heap_array8),
	                            strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_extract8_str)
{
	strollut_heap_check_extract(strollut_heap_array8,
	                            stroll_array_nr(strollut_heap_array8),
	                            strollut_heap_compare_str_min);
}

static void
strollut_heap_check_build_inorder(const struct strollut_heap_elem * array,
                                  unsigned int                      nr,
                                  stroll_array_cmp_fn *             compare)
{
	struct strollut_heap_elem heap[nr];

	memcpy(heap, array, sizeof(heap));
	strollut_heap_algo.build(heap, nr, sizeof(heap[0]), compare, NULL);

	strollut_heap_algo.check(heap, nr, compare);
}

CUTE_TEST(strollut_heap_build_inorder1_num)
{
	strollut_heap_check_build_inorder(strollut_heap_array1,
	                                  stroll_array_nr(strollut_heap_array1),
	                                  strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_inorder1_str)
{
	strollut_heap_check_build_inorder(strollut_heap_array1,
	                                  stroll_array_nr(strollut_heap_array1),
	                                  strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_inorder2_num)
{
	strollut_heap_check_build_inorder(strollut_heap_array2,
	                                  stroll_array_nr(strollut_heap_array2),
	                                  strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_inorder2_str)
{
	strollut_heap_check_build_inorder(strollut_heap_array2,
	                                  stroll_array_nr(strollut_heap_array2),
	                                  strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_inorder3_num)
{
	strollut_heap_check_build_inorder(strollut_heap_array3,
	                                  stroll_array_nr(strollut_heap_array3),
	                                  strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_inorder3_str)
{
	strollut_heap_check_build_inorder(strollut_heap_array3,
	                                  stroll_array_nr(strollut_heap_array3),
	                                  strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_inorder4_num)
{
	strollut_heap_check_build_inorder(strollut_heap_array4,
	                                  stroll_array_nr(strollut_heap_array4),
	                                  strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_inorder4_str)
{
	strollut_heap_check_build_inorder(strollut_heap_array4,
	                                  stroll_array_nr(strollut_heap_array4),
	                                  strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_inorder5_num)
{
	strollut_heap_check_build_inorder(strollut_heap_array5,
	                                  stroll_array_nr(strollut_heap_array5),
	                                  strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_inorder5_str)
{
	strollut_heap_check_build_inorder(strollut_heap_array5,
	                                  stroll_array_nr(strollut_heap_array5),
	                                  strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_inorder6_num)
{
	strollut_heap_check_build_inorder(strollut_heap_array6,
	                                  stroll_array_nr(strollut_heap_array6),
	                                  strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_inorder6_str)
{
	strollut_heap_check_build_inorder(strollut_heap_array6,
	                                  stroll_array_nr(strollut_heap_array6),
	                                  strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_inorder7_num)
{
	strollut_heap_check_build_inorder(strollut_heap_array7,
	                                  stroll_array_nr(strollut_heap_array7),
	                                  strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_inorder7_str)
{
	strollut_heap_check_build_inorder(strollut_heap_array7,
	                                  stroll_array_nr(strollut_heap_array7),
	                                  strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_inorder8_num)
{
	strollut_heap_check_build_inorder(strollut_heap_array8,
	                                  stroll_array_nr(strollut_heap_array8),
	                                  strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_inorder8_str)
{
	strollut_heap_check_build_inorder(strollut_heap_array8,
	                                  stroll_array_nr(strollut_heap_array8),
	                                  strollut_heap_compare_str_min);
}

static void
strollut_heap_check_build_revorder(const struct strollut_heap_elem * array,
                                   unsigned int                      nr,
                                   stroll_array_cmp_fn *             compare)
{
	struct strollut_heap_elem heap[nr];
	unsigned int              e;

	for (e = 0; e < nr; e++)
		memcpy(&heap[e], &array[nr - 1 - e], sizeof(heap[0]));

	strollut_heap_algo.build(heap, nr, sizeof(heap[0]), compare, NULL);

	strollut_heap_algo.check(heap, nr, compare);
}

CUTE_TEST(strollut_heap_build_revorder1_num)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array1,
		stroll_array_nr(strollut_heap_array1),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_revorder1_str)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array1,
		stroll_array_nr(strollut_heap_array1),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_revorder2_num)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array2,
		stroll_array_nr(strollut_heap_array2),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_revorder2_str)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array2,
		stroll_array_nr(strollut_heap_array2),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_revorder3_num)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array3,
		stroll_array_nr(strollut_heap_array3),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_revorder3_str)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array3,
		stroll_array_nr(strollut_heap_array3),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_revorder4_num)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array4,
		stroll_array_nr(strollut_heap_array4),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_revorder4_str)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array4,
		stroll_array_nr(strollut_heap_array4),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_revorder5_num)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array5,
		stroll_array_nr(strollut_heap_array5),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_revorder5_str)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array5,
		stroll_array_nr(strollut_heap_array5),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_revorder6_num)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array6,
		stroll_array_nr(strollut_heap_array6),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_revorder6_str)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array6,
		stroll_array_nr(strollut_heap_array6),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_revorder7_num)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array7,
		stroll_array_nr(strollut_heap_array7),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_revorder7_str)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array7,
		stroll_array_nr(strollut_heap_array7),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_build_revorder8_num)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array8,
		stroll_array_nr(strollut_heap_array8),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_build_revorder8_str)
{
	strollut_heap_check_build_revorder(
		strollut_heap_array8,
		stroll_array_nr(strollut_heap_array8),
		strollut_heap_compare_str_min);
}

static void
strollut_heap_check_insert_inorder(const struct strollut_heap_elem * array,
                                   unsigned int                      nr,
                                   stroll_array_cmp_fn *             compare)
{
	struct strollut_heap_elem heap[nr];
	unsigned int              e;

	for (e = 0; e < nr; e++) {

		strollut_heap_algo.insert(&array[e],
		                          heap,
		                          e,
		                          sizeof(array[0]),
		                          compare,
		                          NULL);
		strollut_heap_algo.check(heap, e + 1, compare);
	}
}

CUTE_TEST(strollut_heap_insert_inorder1_num)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array1,
		stroll_array_nr(strollut_heap_array1),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_inorder1_str)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array1,
		stroll_array_nr(strollut_heap_array1),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_inorder2_num)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array2,
		stroll_array_nr(strollut_heap_array2),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_inorder2_str)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array2,
		stroll_array_nr(strollut_heap_array2),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_inorder3_num)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array3,
		stroll_array_nr(strollut_heap_array3),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_inorder3_str)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array3,
		stroll_array_nr(strollut_heap_array3),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_inorder4_num)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array4,
		stroll_array_nr(strollut_heap_array4),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_inorder4_str)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array4,
		stroll_array_nr(strollut_heap_array4),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_inorder5_num)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array5,
		stroll_array_nr(strollut_heap_array5),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_inorder5_str)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array5,
		stroll_array_nr(strollut_heap_array5),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_inorder6_num)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array6,
		stroll_array_nr(strollut_heap_array6),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_inorder6_str)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array6,
		stroll_array_nr(strollut_heap_array6),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_inorder7_num)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array7,
		stroll_array_nr(strollut_heap_array7),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_inorder7_str)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array7,
		stroll_array_nr(strollut_heap_array7),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_inorder8_num)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array8,
		stroll_array_nr(strollut_heap_array8),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_inorder8_str)
{
	strollut_heap_check_insert_inorder(
		strollut_heap_array8,
		stroll_array_nr(strollut_heap_array8),
		strollut_heap_compare_str_min);
}

static void
strollut_heap_check_insert_revorder(const struct strollut_heap_elem * array,
                                    unsigned int                      nr,
                                    stroll_array_cmp_fn *             compare)
{
	struct strollut_heap_elem heap[nr];
	unsigned int              e = nr;

	for (e = 0; e < nr; e++) {
		strollut_heap_algo.insert(&array[nr - 1 - e],
		                          heap,
		                          e,
		                          sizeof(array[0]),
		                          compare,
		                          NULL);
		strollut_heap_algo.check(heap, e + 1, compare);
	}
}

CUTE_TEST(strollut_heap_insert_revorder1_num)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array1,
		stroll_array_nr(strollut_heap_array1),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_revorder1_str)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array1,
		stroll_array_nr(strollut_heap_array1),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_revorder2_num)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array2,
		stroll_array_nr(strollut_heap_array2),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_revorder2_str)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array2,
		stroll_array_nr(strollut_heap_array2),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_revorder3_num)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array3,
		stroll_array_nr(strollut_heap_array3),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_revorder3_str)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array3,
		stroll_array_nr(strollut_heap_array3),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_revorder4_num)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array4,
		stroll_array_nr(strollut_heap_array4),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_revorder4_str)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array4,
		stroll_array_nr(strollut_heap_array4),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_revorder5_num)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array5,
		stroll_array_nr(strollut_heap_array5),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_revorder5_str)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array5,
		stroll_array_nr(strollut_heap_array5),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_revorder6_num)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array6,
		stroll_array_nr(strollut_heap_array6),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_revorder6_str)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array6,
		stroll_array_nr(strollut_heap_array6),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_revorder7_num)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array7,
		stroll_array_nr(strollut_heap_array7),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_revorder7_str)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array7,
		stroll_array_nr(strollut_heap_array7),
		strollut_heap_compare_str_min);
}

CUTE_TEST(strollut_heap_insert_revorder8_num)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array8,
		stroll_array_nr(strollut_heap_array8),
		strollut_heap_compare_num_min);
}

CUTE_TEST(strollut_heap_insert_revorder8_str)
{
	strollut_heap_check_insert_revorder(
		strollut_heap_array8,
		stroll_array_nr(strollut_heap_array8),
		strollut_heap_compare_str_min);
}

CUTE_GROUP(strollut_heap_group) = {
	CUTE_REF(strollut_heap_build_inorder1_num),
	CUTE_REF(strollut_heap_build_inorder2_num),
	CUTE_REF(strollut_heap_build_inorder3_num),
	CUTE_REF(strollut_heap_build_inorder4_num),
	CUTE_REF(strollut_heap_build_inorder5_num),
	CUTE_REF(strollut_heap_build_inorder6_num),
	CUTE_REF(strollut_heap_build_inorder7_num),
	CUTE_REF(strollut_heap_build_inorder8_num),

	CUTE_REF(strollut_heap_build_inorder1_str),
	CUTE_REF(strollut_heap_build_inorder2_str),
	CUTE_REF(strollut_heap_build_inorder3_str),
	CUTE_REF(strollut_heap_build_inorder4_str),
	CUTE_REF(strollut_heap_build_inorder5_str),
	CUTE_REF(strollut_heap_build_inorder6_str),
	CUTE_REF(strollut_heap_build_inorder7_str),
	CUTE_REF(strollut_heap_build_inorder8_str),

	CUTE_REF(strollut_heap_build_revorder1_num),
	CUTE_REF(strollut_heap_build_revorder2_num),
	CUTE_REF(strollut_heap_build_revorder3_num),
	CUTE_REF(strollut_heap_build_revorder4_num),
	CUTE_REF(strollut_heap_build_revorder5_num),
	CUTE_REF(strollut_heap_build_revorder6_num),
	CUTE_REF(strollut_heap_build_revorder7_num),
	CUTE_REF(strollut_heap_build_revorder8_num),

	CUTE_REF(strollut_heap_build_revorder1_str),
	CUTE_REF(strollut_heap_build_revorder2_str),
	CUTE_REF(strollut_heap_build_revorder3_str),
	CUTE_REF(strollut_heap_build_revorder4_str),
	CUTE_REF(strollut_heap_build_revorder5_str),
	CUTE_REF(strollut_heap_build_revorder6_str),
	CUTE_REF(strollut_heap_build_revorder7_str),
	CUTE_REF(strollut_heap_build_revorder8_str),

	CUTE_REF(strollut_heap_extract1_num),
	CUTE_REF(strollut_heap_extract2_num),
	CUTE_REF(strollut_heap_extract3_num),
	CUTE_REF(strollut_heap_extract4_num),
	CUTE_REF(strollut_heap_extract5_num),
	CUTE_REF(strollut_heap_extract6_num),
	CUTE_REF(strollut_heap_extract7_num),
	CUTE_REF(strollut_heap_extract8_num),

	CUTE_REF(strollut_heap_extract1_str),
	CUTE_REF(strollut_heap_extract2_str),
	CUTE_REF(strollut_heap_extract3_str),
	CUTE_REF(strollut_heap_extract4_str),
	CUTE_REF(strollut_heap_extract5_str),
	CUTE_REF(strollut_heap_extract6_str),
	CUTE_REF(strollut_heap_extract7_str),
	CUTE_REF(strollut_heap_extract8_str),

	CUTE_REF(strollut_heap_insert_inorder1_num),
	CUTE_REF(strollut_heap_insert_inorder2_num),
	CUTE_REF(strollut_heap_insert_inorder3_num),
	CUTE_REF(strollut_heap_insert_inorder4_num),
	CUTE_REF(strollut_heap_insert_inorder5_num),
	CUTE_REF(strollut_heap_insert_inorder6_num),
	CUTE_REF(strollut_heap_insert_inorder7_num),
	CUTE_REF(strollut_heap_insert_inorder8_num),

	CUTE_REF(strollut_heap_insert_inorder1_str),
	CUTE_REF(strollut_heap_insert_inorder2_str),
	CUTE_REF(strollut_heap_insert_inorder3_str),
	CUTE_REF(strollut_heap_insert_inorder4_str),
	CUTE_REF(strollut_heap_insert_inorder5_str),
	CUTE_REF(strollut_heap_insert_inorder6_str),
	CUTE_REF(strollut_heap_insert_inorder7_str),
	CUTE_REF(strollut_heap_insert_inorder8_str),

	CUTE_REF(strollut_heap_insert_revorder1_num),
	CUTE_REF(strollut_heap_insert_revorder2_num),
	CUTE_REF(strollut_heap_insert_revorder3_num),
	CUTE_REF(strollut_heap_insert_revorder4_num),
	CUTE_REF(strollut_heap_insert_revorder5_num),
	CUTE_REF(strollut_heap_insert_revorder6_num),
	CUTE_REF(strollut_heap_insert_revorder7_num),
	CUTE_REF(strollut_heap_insert_revorder8_num),

	CUTE_REF(strollut_heap_insert_revorder1_str),
	CUTE_REF(strollut_heap_insert_revorder2_str),
	CUTE_REF(strollut_heap_insert_revorder3_str),
	CUTE_REF(strollut_heap_insert_revorder4_str),
	CUTE_REF(strollut_heap_insert_revorder5_str),
	CUTE_REF(strollut_heap_insert_revorder6_str),
	CUTE_REF(strollut_heap_insert_revorder7_str),
	CUTE_REF(strollut_heap_insert_revorder8_str),
};

#if defined(CONFIG_STROLL_FBHEAP)

#include "stroll/fbheap.h"

static void
strollut_fbheap_check_recurs(unsigned int                      e,
                             const struct strollut_heap_elem * array,
                             unsigned int                      nr,
                             stroll_array_cmp_fn *             cmp)
{
	const char * const check[] = {
		[0] = "0zero",
		[1] = "1one",
		[2] = "2two",
		[3] = "3three",
		[4] = "4four",
		[5] = "5five",
		[6] = "6six",
		[7] = "7seven"
	};

	if (e >= nr)
		return;

	if (e)
		cute_check_sint(cmp(&array[(e - 1) / 2], &array[e], NULL),
		                lower_equal,
		                0);

	cute_check_uint(array[e].id, lower, stroll_array_nr(check));
	cute_check_str(array[e].str, equal, check[array[e].id]);

	strollut_fbheap_check_recurs((2 * e) + 1, array, nr, cmp);
	strollut_fbheap_check_recurs((2 * e) + 2, array, nr, cmp);
}

static void
strollut_fbheap_check(const struct strollut_heap_elem * array,
                      unsigned int                      nr,
                      stroll_array_cmp_fn *             cmp)
{
	strollut_fbheap_check_recurs(0, array, nr, cmp);
}

static void
strollut_fbheap_setup(void)
{
	strollut_heap_algo.build = _stroll_fbheap_build;
	strollut_heap_algo.insert = _stroll_fbheap_insert;
	strollut_heap_algo.extract = _stroll_fbheap_extract;
	strollut_heap_algo.check = strollut_fbheap_check;
}

#else  /* !defined(CONFIG_STROLL_FBHEAP) */

static void
strollut_fbheap_setup(void)
{
	cute_skip("support not compiled-in");
}

#endif /* defined(CONFIG_STROLL_FBHEAP) */

CUTE_SUITE_STATIC(strollut_fbheap_suite,
                  strollut_heap_group,
                  strollut_fbheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_FWHEAP)

#include "stroll/fwheap.h"

static unsigned long
strollut_fwheap_rbits[_STROLL_FWHEAP_RBITS_SIZE(STROLLUT_HEAP_ELEM_NR)];

static void
strollut_fwheap_build(void * __restrict     array,
                      unsigned int          nr,
                      size_t                size,
                      stroll_array_cmp_fn * compare,
                      void *                data)
{
	_stroll_fwheap_build(array,
	                     strollut_fwheap_rbits,
	                     nr,
	                     size,
	                     compare,
	                     data);
}

static void
strollut_fwheap_insert(const void * __restrict elem,
                       void * __restrict       array,
                       unsigned int            nr,
                       size_t                  size,
                       stroll_array_cmp_fn *   compare,
                       void *                  data)
{
	_stroll_fwheap_insert(elem,
	                      array,
	                      strollut_fwheap_rbits,
	                      nr,
	                      size,
	                      compare,
	                      data);
}

static void
strollut_fwheap_extract(void * __restrict     elem,
                        void * __restrict     array,
                        unsigned int          nr,
                        size_t                size,
                        stroll_array_cmp_fn * compare,
                        void *                data)
{
	_stroll_fwheap_extract(elem,
	                       array,
	                       strollut_fwheap_rbits,
	                       nr,
	                       size,
	                       compare,
	                       data);
}

static inline unsigned int
strollut_fwheap_parent(unsigned int index)
{
	assert(index);

	return index / 2;
}

static inline unsigned int
strollut_fwheap_left(unsigned int index)
{
	return (2 * index) +
	       (unsigned int)_stroll_fbmap_test(strollut_fwheap_rbits, index);
}

static inline unsigned int
strollut_fwheap_right(unsigned int index)
{
	return (2 * index) +
	       1 - (unsigned int)_stroll_fbmap_test(strollut_fwheap_rbits,
	                                            index);
}

static inline bool
strollut_fwheap_isleft(unsigned int index)
{
	assert(index);

	return (!!(index & 1)) ==
	       _stroll_fbmap_test(strollut_fwheap_rbits,
	                          strollut_fwheap_parent(index));
}

static inline unsigned int
strollut_fwheap_dancestor(unsigned int index)
{
	/* Move up untill index points to a right child. */
	while (strollut_fwheap_isleft(index))
		index = strollut_fwheap_parent(index);

	/* Then return its parent. */
	return strollut_fwheap_parent(index);
}

static void
strollut_fwheap_check_recurs(unsigned int                      e,
                             const struct strollut_heap_elem * array,
                             unsigned int                      nr,
                             stroll_array_cmp_fn *             cmp)
{
	const char * const check[] = {
		[0] = "0zero",
		[1] = "1one",
		[2] = "2two",
		[3] = "3three",
		[4] = "4four",
		[5] = "5five",
		[6] = "6six",
		[7] = "7seven"
	};

	if (e >= nr)
		return;

	if (e) {
		unsigned int didx = strollut_fwheap_dancestor(e);

		cute_check_sint(cmp(&array[didx], &array[e], NULL),
		                lower_equal,
		                0);
	}

	cute_check_uint(array[e].id, lower, stroll_array_nr(check));
	cute_check_str(array[e].str, equal, check[array[e].id]);

	if (e)
		/* Root node has no left child ! */
		strollut_fwheap_check_recurs(strollut_fwheap_left(e),
		                             array,
		                             nr,
		                             cmp);
	strollut_fwheap_check_recurs(strollut_fwheap_right(e), array, nr, cmp);
}

static void
strollut_fwheap_check(const struct strollut_heap_elem * array,
                      unsigned int                      nr,
                      stroll_array_cmp_fn *             cmp)
{
	strollut_fwheap_check_recurs(0, array, nr, cmp);
}

static void
strollut_fwheap_setup(void)
{
	strollut_heap_algo.build = strollut_fwheap_build;
	strollut_heap_algo.insert = strollut_fwheap_insert;
	strollut_heap_algo.extract = strollut_fwheap_extract;
	strollut_heap_algo.check = strollut_fwheap_check;
}

#else  /* !defined(CONFIG_STROLL_FWHEAP) */

static void
strollut_fwheap_setup(void)
{
	cute_skip("support not compiled-in");
}

#endif /* defined(CONFIG_STROLL_FWHEAP) */

CUTE_SUITE_STATIC(strollut_fwheap_suite,
                  strollut_heap_group,
                  strollut_fwheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

CUTE_GROUP(strollut_heap_main_group) = {
	CUTE_REF(strollut_fbheap_suite),
	CUTE_REF(strollut_fwheap_suite),
};

CUTE_SUITE_EXTERN(strollut_heap_suite,
                  strollut_heap_main_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
