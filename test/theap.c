/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/config.h"
#if defined(CONFIG_STROLL_HPRHEAP)
#include "stroll/hprheap.h"
#endif
#if defined(CONFIG_STROLL_DRPHEAP)
#include "stroll/drpheap.h"
#endif
#if defined(CONFIG_STROLL_DPRHEAP)
#include "stroll/dprheap.h"
#endif
#if defined(CONFIG_STROLL_PPRHEAP)
#include "stroll/pprheap.h"
#endif
#if defined(CONFIG_STROLL_DBNHEAP)
#include "stroll/dbnheap.h"
#endif
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <stdlib.h>

#define STROLLUT_THEAP_NOTEST(_test) \
	CUTE_TEST(_test) \
	{ \
		cute_skip("support not compiled-in"); \
	}

#define STROLLUT_THEAP_NOSETUP(_setup) \
	static void _setup(void) \
	{ \
		cute_skip("support not compiled-in"); \
	}

union strollut_theap_base_node {
#if defined(CONFIG_STROLL_HPRHEAP)
	struct stroll_hprheap_node hprheap;
#endif
#if defined(CONFIG_STROLL_DRPHEAP)
	struct stroll_drpheap_node drpheap;
#endif
#if defined(CONFIG_STROLL_DPRHEAP)
	struct stroll_dprheap_node dprheap;
#endif
#if defined(CONFIG_STROLL_PPRHEAP)
	struct stroll_pprheap_node pprheap;
#endif
#if defined(CONFIG_STROLL_DBNHEAP)
	struct stroll_dbnheap_node dbnheap;
#endif
};

struct strollut_theap_node {
	union strollut_theap_base_node super;
	int                            key;
};

#define STROLLUT_THEAP_INIT_NODE(_key) \
	{ .key = _key }

struct strollut_theap_xpct {
	int                          key;
	unsigned int                 seen;
	struct strollut_theap_node * node;
};

typedef void (strollut_theap_setup_fn)(void * __restrict);

typedef bool (strollut_theap_empty_fn)(const void * __restrict);

typedef struct strollut_theap_node *
        (strollut_theap_peek_fn)(const void * __restrict);

typedef void
        (strollut_theap_insert_fn)(void * __restrict,
                                   struct strollut_theap_node * __restrict);

typedef struct strollut_theap_node *
        (strollut_theap_extract_fn)(void * __restrict);

typedef void
        (strollut_theap_remove_fn)(void * __restrict,
                                   struct strollut_theap_node * __restrict);

typedef void
        (strollut_theap_rekey_fn)(void * __restrict,
                                  struct strollut_theap_node * __restrict);

typedef void
        (strollut_theap_merge_fn)(void * __restrict, void * __restrict);

typedef void
        (strollut_theap_validate_fn)(void *,
                                     struct strollut_theap_node *,
                                     unsigned int);

struct strollut_theap_ops {
	strollut_theap_setup_fn *    init;
	strollut_theap_setup_fn *    fini;
	strollut_theap_empty_fn *    empty;
	strollut_theap_peek_fn *     peek;
	strollut_theap_insert_fn *   insert;
	strollut_theap_extract_fn *  extract;
	strollut_theap_remove_fn *   remove;
	strollut_theap_rekey_fn *    promote;
	strollut_theap_rekey_fn *    demote;
	strollut_theap_merge_fn *    merge;
	strollut_theap_validate_fn * validate;
};

static const struct strollut_theap_ops * strollut_theap_algo;
static void *                            strollut_theap_main;
static void *                            strollut_theap_alt;

#define STROLLUT_THEAP_ALGO_SUP(_setup, _algo, _main, _alt) \
	static void _setup(void) \
	{ \
		strollut_theap_algo = _algo; \
		(_algo)->init(_main); \
		strollut_theap_main = _main; \
		(_algo)->init(_alt); \
		strollut_theap_alt = _alt; \
	}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_init_assert)
{
	cute_expect_assertion(strollut_theap_algo->init(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_init_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_check_empty_assert)
{
	cute_expect_assertion(strollut_theap_algo->empty(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_check_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_theap_check_empty)
{
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_peek_empty_assert)
{
	cute_expect_assertion(strollut_theap_algo->peek(NULL));
	cute_expect_assertion(strollut_theap_algo->peek(strollut_theap_main));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_peek_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_insert_empty_assert)
{
	struct strollut_theap_node node = STROLLUT_THEAP_INIT_NODE(1);

	cute_expect_assertion(strollut_theap_algo->insert(NULL, &node));
	cute_expect_assertion(strollut_theap_algo->insert(strollut_theap_main,
	                                                  NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_insert_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_theap_insert_empty)
{
	struct strollut_theap_node node = STROLLUT_THEAP_INIT_NODE(1);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);
	strollut_theap_algo->insert(strollut_theap_main, &node);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);
	cute_check_ptr(strollut_theap_algo->peek(strollut_theap_main),
	               equal,
	               &node);
}

static void
strollut_theap_check_insert(struct strollut_theap_node * nodes, unsigned int nr)
{
	unsigned int n;

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);

	for (n = 0; n < nr; n++) {
		strollut_theap_algo->insert(strollut_theap_main, &nodes[n]);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              n + 1);
	}

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);
}

CUTE_TEST(strollut_theap_insert_12)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_21)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_1234)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_1243)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_1324)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_1342)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_1423)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_1432)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_2134)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_2143)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_2314)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_2341)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_2413)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_2431)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_3124)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_3142)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_3214)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_3241)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_3412)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_3421)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_4123)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_4132)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_4213)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_4231)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_4312)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_insert_4321)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_insert(nodes, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_extract_assert)
{
	cute_expect_assertion(strollut_theap_algo->extract(NULL));
	cute_expect_assertion(
		strollut_theap_algo->extract(strollut_theap_main));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_extract_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static int
strollut_theap_compare_xpct(const void * a, const void * b)
{
	const struct strollut_theap_xpct * _a = a;
	const struct strollut_theap_xpct * _b = b;

	return (_a->key > _b->key) - (_a->key < _b->key);
}

static void
strollut_theap_check_extract(struct strollut_theap_node * nodes,
                             unsigned int                 nr)
{
	unsigned int               n;
	struct strollut_theap_xpct xpct[nr];

	for (n = 0; n < nr; n++) {
		xpct[n].key = nodes[n].key;
		xpct[n].node = &nodes[n];
	}

	qsort(xpct, nr, sizeof(xpct[0]), strollut_theap_compare_xpct);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);

	for (n = 0; n < nr; n++)
		strollut_theap_algo->insert(strollut_theap_main, &nodes[n]);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	for (n = 0; n < nr; n++) {
		const struct strollut_theap_node * entry;

		entry = strollut_theap_algo->extract(strollut_theap_main);
		cute_check_ptr(entry, unequal, NULL);

		cute_check_sint(entry->key, equal, xpct[n].key);
		cute_check_ptr(entry, equal, xpct[n].node);
	}

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);
}

CUTE_TEST(strollut_theap_extract_12)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_21)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_1234)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_1243)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_1324)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_1342)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_1423)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_1432)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_2134)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_2143)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_2314)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_2341)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_2413)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_2431)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_3124)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_3142)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_3214)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_3241)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_3412)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_3421)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_4123)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_4132)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_4213)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_4231)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_4312)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_extract_4321)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_extract(nodes, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_remove_assert)
{
	struct strollut_theap_node node;

	if (!strollut_theap_algo->remove) {
		cute_skip("remove() not supported");
		return;
	}

	cute_expect_assertion(
		strollut_theap_algo->remove(NULL, &node));
	cute_expect_assertion(
		strollut_theap_algo->remove(strollut_theap_main, NULL));
	cute_expect_assertion(
		strollut_theap_algo->remove(strollut_theap_main, &node));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_remove_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static void
strollut_theap_check_remove(struct strollut_theap_node * nodes, unsigned int nr)
{
	unsigned int               n;
	struct strollut_theap_xpct tmp[nr];

	if (!strollut_theap_algo->remove) {
		cute_skip("remove() not supported");
		return;
	}

	for (n = 0; n < nr; n++) {
		tmp[n].key = nodes[n].key;
		tmp[n].node = &nodes[n];
	}

	qsort(tmp, nr, sizeof(tmp[0]), strollut_theap_compare_xpct);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);

	for (n = 0; n < nr; n++)
		strollut_theap_algo->insert(strollut_theap_main, tmp[n].node);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	for (n = 0; n < nr; n++) {
		strollut_theap_algo->validate(strollut_theap_main,
		                              &nodes[n],
		                              nr - n);
		strollut_theap_algo->remove(strollut_theap_main, &nodes[n]);
	}

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);
}

CUTE_TEST(strollut_theap_remove_12)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_21)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_1234)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_1243)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_1324)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_1342)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_1423)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_1432)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_2134)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_2143)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_2314)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_2341)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_2413)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_2431)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_3124)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_3142)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_3214)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_3241)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_3412)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_3421)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_4123)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_4132)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_4213)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_4231)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_4312)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_remove_4321)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_remove(nodes, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_promote_assert)
{
	struct strollut_theap_node node;

	if (!strollut_theap_algo->promote) {
		cute_skip("promote() not supported");
		return;
	}

	cute_expect_assertion(strollut_theap_algo->promote(NULL, &node));
	cute_expect_assertion(
		strollut_theap_algo->promote(strollut_theap_main, NULL));
	cute_expect_assertion(
		strollut_theap_algo->promote(strollut_theap_main, &node));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_promote_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static void
strollut_theap_check_promote(struct strollut_theap_node * nodes,
                             unsigned int                 nr)
{
	unsigned int               n;
	struct strollut_theap_xpct tmp[nr];
	int                        shift;

	if (!strollut_theap_algo->promote) {
		cute_skip("promote() not supported");
		return;
	}

	for (n = 0; n < nr; n++) {
		tmp[n].key = nodes[n].key;
		tmp[n].node = &nodes[n];
	}

	qsort(tmp, nr, sizeof(tmp[0]), strollut_theap_compare_xpct);
	shift = stroll_max((tmp[nr - 1].key - tmp[0].key) / 2, 1);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);

	for (n = 0; n < nr; n++)
		strollut_theap_algo->insert(strollut_theap_main, tmp[n].node);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	for (n = 0; n < nr; n++) {
		strollut_theap_algo->promote(strollut_theap_main, &nodes[n]);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              nr);
	}

	for (n = 0; n < nr; n++) {
		nodes[n].key -= shift;
		strollut_theap_algo->promote(strollut_theap_main, &nodes[n]);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              nr);
	}
}

CUTE_TEST(strollut_theap_promote_12)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_21)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_1234)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_1243)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_1324)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_1342)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_1423)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_1432)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_2134)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_2143)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_2314)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_2341)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_2413)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_2431)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_3124)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_3142)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_3214)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_3241)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_3412)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_3421)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_4123)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_4132)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_4213)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_4231)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_4312)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_promote_4321)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_promote(nodes, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_demote_assert)
{
	struct strollut_theap_node node;

	if (!strollut_theap_algo->demote) {
		cute_skip("demote() not supported");
		return;
	}

	cute_expect_assertion(strollut_theap_algo->demote(NULL, &node));
	cute_expect_assertion(
		strollut_theap_algo->demote(strollut_theap_main, NULL));
	cute_expect_assertion(
		strollut_theap_algo->demote(strollut_theap_main, &node));
	cute_expect_assertion(
		strollut_theap_algo->demote(strollut_theap_main, &node));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_demote_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static void
strollut_theap_check_demote(struct strollut_theap_node * nodes,
                            unsigned int                 nr)
{
	unsigned int               n;
	struct strollut_theap_xpct tmp[nr];
	int                        shift;

	if (!strollut_theap_algo->demote) {
		cute_skip("demote() not supported");
		return;
	}

	for (n = 0; n < nr; n++) {
		tmp[n].key = nodes[n].key;
		tmp[n].node = &nodes[n];
	}

	qsort(tmp, nr, sizeof(tmp[0]), strollut_theap_compare_xpct);
	shift = stroll_max((tmp[nr - 1].key - tmp[0].key) / 2, 1);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);

	for (n = 0; n < nr; n++)
		strollut_theap_algo->insert(strollut_theap_main, tmp[n].node);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	for (n = 0; n < nr; n++) {
		strollut_theap_algo->demote(strollut_theap_main, &nodes[n]);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              nr);
	}

	for (n = 0; n < nr; n++) {
		nodes[n].key += shift;
		strollut_theap_algo->demote(strollut_theap_main, &nodes[n]);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              nr);
	}
}

CUTE_TEST(strollut_theap_demote_12)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_21)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_1234)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_1243)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_1324)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_1342)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_1423)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_1432)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_2134)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_2143)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_2314)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_2341)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_2413)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_2431)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_3124)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_3142)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_3214)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_3241)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_3412)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_3421)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_4123)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_4132)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_4213)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_4231)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_4312)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

CUTE_TEST(strollut_theap_demote_4321)
{
	struct strollut_theap_node nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(3),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_demote(nodes, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_merge_assert)
{
	cute_expect_assertion(strollut_theap_algo->merge(NULL,
	                                                 strollut_theap_alt));
	cute_expect_assertion(strollut_theap_algo->merge(strollut_theap_main,
	                                                 NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOTEST(strollut_theap_merge_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_theap_merge_both_empty)
{
	strollut_theap_algo->merge(strollut_theap_main, strollut_theap_alt);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);
}

CUTE_TEST(strollut_theap_merge_src_empty)
{
	struct strollut_theap_node node = STROLLUT_THEAP_INIT_NODE(1);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);
	strollut_theap_algo->insert(strollut_theap_main, &node);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	strollut_theap_algo->merge(strollut_theap_main, strollut_theap_alt);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	strollut_theap_algo->validate(strollut_theap_main, &node, 1);
}

CUTE_TEST(strollut_theap_merge_res_empty)
{
	struct strollut_theap_node node = STROLLUT_THEAP_INIT_NODE(1);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_alt),
	                is,
	                true);
	strollut_theap_algo->insert(strollut_theap_alt, &node);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_alt),
	                is,
	                false);

	strollut_theap_algo->merge(strollut_theap_main, strollut_theap_alt);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	strollut_theap_algo->validate(strollut_theap_main, &node, 1);
}

static void
strollut_theap_check_merge(const struct strollut_theap_node * res_nodes,
                           unsigned int                       res_nr,
                           const struct strollut_theap_node * src_nodes,
                           unsigned int                       src_nr)
{
	unsigned int               n;
	struct strollut_theap_node tmp[res_nr + src_nr];

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);
	for (n = 0; n < res_nr; n++) {
		tmp[n] = res_nodes[n];
		strollut_theap_algo->insert(strollut_theap_main, &tmp[n]);
	}
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_alt),
	                is,
	                true);
	for (n = 0; n < src_nr; n++) {
		tmp[res_nr + n] = src_nodes[n];
		strollut_theap_algo->insert(strollut_theap_alt,
		                            &tmp[res_nr + n]);
	}
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_alt),
	                is,
	                false);

	strollut_theap_algo->merge(strollut_theap_main, strollut_theap_alt);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	strollut_theap_algo->validate(strollut_theap_main,
	                              tmp,
	                              res_nr + src_nr);
}

CUTE_TEST(strollut_theap_merge_01)
{
	const struct strollut_theap_node res_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};
	const struct strollut_theap_node src_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(0),
		STROLLUT_THEAP_INIT_NODE(1)
	};

	strollut_theap_check_merge(res_nodes,
	                           stroll_array_nr(res_nodes),
	                           src_nodes,
	                           stroll_array_nr(src_nodes));
}

CUTE_TEST(strollut_theap_merge_04)
{
	const struct strollut_theap_node res_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};
	const struct strollut_theap_node src_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(0),
		STROLLUT_THEAP_INIT_NODE(4)
	};

	strollut_theap_check_merge(res_nodes,
	                           stroll_array_nr(res_nodes),
	                           src_nodes,
	                           stroll_array_nr(src_nodes));
}

CUTE_TEST(strollut_theap_merge_45)
{
	const struct strollut_theap_node res_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};
	const struct strollut_theap_node src_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(5)
	};

	strollut_theap_check_merge(res_nodes,
	                           stroll_array_nr(res_nodes),
	                           src_nodes,
	                           stroll_array_nr(src_nodes));
}

CUTE_TEST(strollut_theap_merge_0123)
{
	const struct strollut_theap_node res_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(5)
	};
	const struct strollut_theap_node src_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(0),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(3)
	};

	strollut_theap_check_merge(res_nodes,
	                           stroll_array_nr(res_nodes),
	                           src_nodes,
	                           stroll_array_nr(src_nodes));
}

CUTE_TEST(strollut_theap_merge_0126)
{
	const struct strollut_theap_node res_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(5)
	};
	const struct strollut_theap_node src_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(0),
		STROLLUT_THEAP_INIT_NODE(1),
		STROLLUT_THEAP_INIT_NODE(2),
		STROLLUT_THEAP_INIT_NODE(6)
	};

	strollut_theap_check_merge(res_nodes,
	                           stroll_array_nr(res_nodes),
	                           src_nodes,
	                           stroll_array_nr(src_nodes));
}

CUTE_TEST(strollut_theap_merge_6789)
{
	const struct strollut_theap_node res_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(4),
		STROLLUT_THEAP_INIT_NODE(5)
	};
	const struct strollut_theap_node src_nodes[] = {
		STROLLUT_THEAP_INIT_NODE(6),
		STROLLUT_THEAP_INIT_NODE(7),
		STROLLUT_THEAP_INIT_NODE(8),
		STROLLUT_THEAP_INIT_NODE(9)
	};

	strollut_theap_check_merge(res_nodes,
	                           stroll_array_nr(res_nodes),
	                           src_nodes,
	                           stroll_array_nr(src_nodes));
}

CUTE_GROUP(strollut_theap_group) = {
	CUTE_REF(strollut_theap_init_assert),
	CUTE_REF(strollut_theap_check_empty_assert),
	CUTE_REF(strollut_theap_check_empty),
	CUTE_REF(strollut_theap_peek_empty_assert),

	CUTE_REF(strollut_theap_insert_empty_assert),
	CUTE_REF(strollut_theap_insert_empty),
	CUTE_REF(strollut_theap_insert_12),
	CUTE_REF(strollut_theap_insert_21),
	CUTE_REF(strollut_theap_insert_1234),
	CUTE_REF(strollut_theap_insert_1243),
	CUTE_REF(strollut_theap_insert_1324),
	CUTE_REF(strollut_theap_insert_1342),
	CUTE_REF(strollut_theap_insert_1423),
	CUTE_REF(strollut_theap_insert_1432),
	CUTE_REF(strollut_theap_insert_2134),
	CUTE_REF(strollut_theap_insert_2143),
	CUTE_REF(strollut_theap_insert_2314),
	CUTE_REF(strollut_theap_insert_2341),
	CUTE_REF(strollut_theap_insert_2413),
	CUTE_REF(strollut_theap_insert_2431),
	CUTE_REF(strollut_theap_insert_3124),
	CUTE_REF(strollut_theap_insert_3142),
	CUTE_REF(strollut_theap_insert_3214),
	CUTE_REF(strollut_theap_insert_3241),
	CUTE_REF(strollut_theap_insert_3412),
	CUTE_REF(strollut_theap_insert_3421),
	CUTE_REF(strollut_theap_insert_4123),
	CUTE_REF(strollut_theap_insert_4132),
	CUTE_REF(strollut_theap_insert_4213),
	CUTE_REF(strollut_theap_insert_4231),
	CUTE_REF(strollut_theap_insert_4312),
	CUTE_REF(strollut_theap_insert_4321),

	CUTE_REF(strollut_theap_extract_assert),
	CUTE_REF(strollut_theap_extract_12),
	CUTE_REF(strollut_theap_extract_21),
	CUTE_REF(strollut_theap_extract_1234),
	CUTE_REF(strollut_theap_extract_1243),
	CUTE_REF(strollut_theap_extract_1324),
	CUTE_REF(strollut_theap_extract_1342),
	CUTE_REF(strollut_theap_extract_1423),
	CUTE_REF(strollut_theap_extract_1432),
	CUTE_REF(strollut_theap_extract_2134),
	CUTE_REF(strollut_theap_extract_2143),
	CUTE_REF(strollut_theap_extract_2314),
	CUTE_REF(strollut_theap_extract_2341),
	CUTE_REF(strollut_theap_extract_2413),
	CUTE_REF(strollut_theap_extract_2431),
	CUTE_REF(strollut_theap_extract_3124),
	CUTE_REF(strollut_theap_extract_3142),
	CUTE_REF(strollut_theap_extract_3214),
	CUTE_REF(strollut_theap_extract_3241),
	CUTE_REF(strollut_theap_extract_3412),
	CUTE_REF(strollut_theap_extract_3421),
	CUTE_REF(strollut_theap_extract_4123),
	CUTE_REF(strollut_theap_extract_4132),
	CUTE_REF(strollut_theap_extract_4213),
	CUTE_REF(strollut_theap_extract_4231),
	CUTE_REF(strollut_theap_extract_4312),
	CUTE_REF(strollut_theap_extract_4321),

	CUTE_REF(strollut_theap_remove_assert),
	CUTE_REF(strollut_theap_remove_12),
	CUTE_REF(strollut_theap_remove_21),
	CUTE_REF(strollut_theap_remove_1234),
	CUTE_REF(strollut_theap_remove_1243),
	CUTE_REF(strollut_theap_remove_1324),
	CUTE_REF(strollut_theap_remove_1342),
	CUTE_REF(strollut_theap_remove_1423),
	CUTE_REF(strollut_theap_remove_1432),
	CUTE_REF(strollut_theap_remove_2134),
	CUTE_REF(strollut_theap_remove_2143),
	CUTE_REF(strollut_theap_remove_2314),
	CUTE_REF(strollut_theap_remove_2341),
	CUTE_REF(strollut_theap_remove_2413),
	CUTE_REF(strollut_theap_remove_2431),
	CUTE_REF(strollut_theap_remove_3124),
	CUTE_REF(strollut_theap_remove_3142),
	CUTE_REF(strollut_theap_remove_3214),
	CUTE_REF(strollut_theap_remove_3241),
	CUTE_REF(strollut_theap_remove_3412),
	CUTE_REF(strollut_theap_remove_3421),
	CUTE_REF(strollut_theap_remove_4123),
	CUTE_REF(strollut_theap_remove_4132),
	CUTE_REF(strollut_theap_remove_4213),
	CUTE_REF(strollut_theap_remove_4231),
	CUTE_REF(strollut_theap_remove_4312),
	CUTE_REF(strollut_theap_remove_4321),

	CUTE_REF(strollut_theap_promote_assert),
	CUTE_REF(strollut_theap_promote_12),
	CUTE_REF(strollut_theap_promote_21),
	CUTE_REF(strollut_theap_promote_1234),
	CUTE_REF(strollut_theap_promote_1243),
	CUTE_REF(strollut_theap_promote_1324),
	CUTE_REF(strollut_theap_promote_1342),
	CUTE_REF(strollut_theap_promote_1423),
	CUTE_REF(strollut_theap_promote_1432),
	CUTE_REF(strollut_theap_promote_2134),
	CUTE_REF(strollut_theap_promote_2143),
	CUTE_REF(strollut_theap_promote_2314),
	CUTE_REF(strollut_theap_promote_2341),
	CUTE_REF(strollut_theap_promote_2413),
	CUTE_REF(strollut_theap_promote_2431),
	CUTE_REF(strollut_theap_promote_3124),
	CUTE_REF(strollut_theap_promote_3142),
	CUTE_REF(strollut_theap_promote_3214),
	CUTE_REF(strollut_theap_promote_3241),
	CUTE_REF(strollut_theap_promote_3412),
	CUTE_REF(strollut_theap_promote_3421),
	CUTE_REF(strollut_theap_promote_4123),
	CUTE_REF(strollut_theap_promote_4132),
	CUTE_REF(strollut_theap_promote_4213),
	CUTE_REF(strollut_theap_promote_4231),
	CUTE_REF(strollut_theap_promote_4312),
	CUTE_REF(strollut_theap_promote_4321),

	CUTE_REF(strollut_theap_demote_assert),
	CUTE_REF(strollut_theap_demote_12),
	CUTE_REF(strollut_theap_demote_21),
	CUTE_REF(strollut_theap_demote_1234),
	CUTE_REF(strollut_theap_demote_1243),
	CUTE_REF(strollut_theap_demote_1324),
	CUTE_REF(strollut_theap_demote_1342),
	CUTE_REF(strollut_theap_demote_1423),
	CUTE_REF(strollut_theap_demote_1432),
	CUTE_REF(strollut_theap_demote_2134),
	CUTE_REF(strollut_theap_demote_2143),
	CUTE_REF(strollut_theap_demote_2314),
	CUTE_REF(strollut_theap_demote_2341),
	CUTE_REF(strollut_theap_demote_2413),
	CUTE_REF(strollut_theap_demote_2431),
	CUTE_REF(strollut_theap_demote_3124),
	CUTE_REF(strollut_theap_demote_3142),
	CUTE_REF(strollut_theap_demote_3214),
	CUTE_REF(strollut_theap_demote_3241),
	CUTE_REF(strollut_theap_demote_3412),
	CUTE_REF(strollut_theap_demote_3421),
	CUTE_REF(strollut_theap_demote_4123),
	CUTE_REF(strollut_theap_demote_4132),
	CUTE_REF(strollut_theap_demote_4213),
	CUTE_REF(strollut_theap_demote_4231),
	CUTE_REF(strollut_theap_demote_4312),
	CUTE_REF(strollut_theap_demote_4321),

	CUTE_REF(strollut_theap_merge_assert),
	CUTE_REF(strollut_theap_merge_both_empty),
	CUTE_REF(strollut_theap_merge_src_empty),
	CUTE_REF(strollut_theap_merge_res_empty),
	CUTE_REF(strollut_theap_merge_01),
	CUTE_REF(strollut_theap_merge_04),
	CUTE_REF(strollut_theap_merge_45),
	CUTE_REF(strollut_theap_merge_0123),
	CUTE_REF(strollut_theap_merge_0126),
	CUTE_REF(strollut_theap_merge_6789)
};

/******************************************************************************
 * Pairing heap tests
 ******************************************************************************/

#if defined(CONFIG_STROLL_HPRHEAP)

static struct stroll_hprheap_base strollut_hprheap_main;
static struct stroll_hprheap_base strollut_hprheap_alt;

static int
strollut_theap_compare_hprheap(
	const struct stroll_hprheap_node * __restrict a,
	const struct stroll_hprheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollut_theap_node * _a =
		stroll_hprheap_entry(a,
		                     const struct strollut_theap_node,
		                     super.hprheap);
	const struct strollut_theap_node * _b =
		stroll_hprheap_entry(b,
		                     const struct strollut_theap_node,
		                     super.hprheap);

	return (_a->key > _b->key) - (_a->key < _b->key);
}

static void
strollut_theap_init_hprheap(void * __restrict heap)
{
	stroll_hprheap_base_setup(heap);
}

static void
strollut_theap_fini_hprheap(void * __restrict heap __unused)
{
}

static bool
strollut_theap_hprheap_empty(const void * __restrict heap)
{
	return stroll_hprheap_base_isempty(heap);
}

static struct strollut_theap_node *
strollut_theap_peek_hprheap(const void * __restrict heap)
{
	return stroll_hprheap_entry(stroll_hprheap_base_peek(heap),
	                            struct strollut_theap_node,
	                            super.hprheap);
}

static void
strollut_theap_insert_hprheap(void * __restrict                       heap,
                             struct strollut_theap_node * __restrict node)
{
	stroll_hprheap_base_insert(heap,
	                           &node->super.hprheap,
	                           strollut_theap_compare_hprheap,
	                           NULL);
}

static struct strollut_theap_node *
strollut_theap_extract_hprheap(void * __restrict heap)
{
	return stroll_hprheap_entry(
		stroll_hprheap_base_extract(heap,
		                            strollut_theap_compare_hprheap,
		                            NULL),
		struct strollut_theap_node,
		super.hprheap);
}

static void
strollut_theap_merge_hprheap(void * __restrict first, void * __restrict second)
{
	stroll_hprheap_base_merge(first,
	                          second,
	                          strollut_theap_compare_hprheap,
	                          NULL);
}

static void
strollut_theap_check_hprheap_props(const struct stroll_hprheap_node * node,
                                   struct strollut_theap_xpct *       xpct,
                                   unsigned int                       nr)
{
	const struct strollut_theap_node * entry;
	const struct stroll_hprheap_node * child;
	unsigned int                       n;
	bool                               found = false;

	entry = stroll_hprheap_entry(node,
	                             struct strollut_theap_node,
	                             super.hprheap);
	for (n = 0; n < nr; n++) {
		if ((xpct[n].key == entry->key) &&
		    (&xpct[n].node->super.hprheap == node)) {
			xpct[n].seen++;
			found = true;
			break;
		}
	}

	cute_check_bool(found, is, true);

	for (child = node->children[0];
	     child;
	     child = child->children[1]) {
		cute_check_sint(strollut_theap_compare_hprheap(node,
		                                               child,
		                                               NULL),
		                lower_equal,
		                0);
		strollut_theap_check_hprheap_props(child, xpct, nr);
	}
}

static void
strollut_theap_validate_hprheap(void *                       heap,
                                struct strollut_theap_node * nodes,
                                unsigned int                 nr)
{
	const struct stroll_hprheap_base * hp = heap;
	struct strollut_theap_xpct         xpct[nr];
	unsigned int                       n;

	if (!nr) {
		cute_check_ptr(hp->root, equal, NULL);
		return;
	}

	for (n = 0; n < nr; n++) {
		xpct[n].key = nodes[n].key;
		xpct[n].seen = 0;
		xpct[n].node = &nodes[n];
	}

	strollut_theap_check_hprheap_props(hp->root, xpct, nr);

	for (n = 0; n < nr; n++)
		cute_check_uint(xpct[n].seen, equal, 1);
}

static const struct strollut_theap_ops strollut_theap_hprheap_ops = {
	.init     = strollut_theap_init_hprheap,
	.fini     = strollut_theap_fini_hprheap,
	.empty    = strollut_theap_hprheap_empty,
	.peek     = strollut_theap_peek_hprheap,
	.insert   = strollut_theap_insert_hprheap,
	.extract  = strollut_theap_extract_hprheap,
	.merge    = strollut_theap_merge_hprheap,
	.validate = strollut_theap_validate_hprheap
};

STROLLUT_THEAP_ALGO_SUP(strollut_theap_hprheap_setup,
                        &strollut_theap_hprheap_ops,
                        &strollut_hprheap_main,
                        &strollut_hprheap_alt);

#else /* !defined(CONFIG_STROLL_HPRHEAP) */

STROLLUT_THEAP_NOSETUP(strollut_theap_hprheap_setup)

#endif /* defined(CONFIG_STROLL_HPRHEAP) */

CUTE_SUITE_STATIC(strollut_hprheap_suite,
                  strollut_theap_group,
                  strollut_theap_hprheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

/******************************************************************************
 * Rank pairing heap tests
 ******************************************************************************/

#if defined(CONFIG_STROLL_DRPHEAP)

static struct stroll_drpheap_base strollut_drpheap_main;
static struct stroll_drpheap_base strollut_drpheap_alt;

static int
strollut_theap_compare_drpheap(
	const struct stroll_drpheap_node * __restrict a,
	const struct stroll_drpheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollut_theap_node * _a =
		stroll_drpheap_entry(a,
		                     const struct strollut_theap_node,
		                     super.drpheap);
	const struct strollut_theap_node * _b =
		stroll_drpheap_entry(b,
		                     const struct strollut_theap_node,
		                     super.drpheap);

	return (_a->key > _b->key) - (_a->key < _b->key);
}

static void
strollut_theap_init_drpheap(void * __restrict heap)
{
	stroll_drpheap_base_setup(heap);
}

static void
strollut_theap_fini_drpheap(void * __restrict heap __unused)
{
}

static bool
strollut_theap_drpheap_empty(const void * __restrict heap)
{
	return stroll_drpheap_base_isempty(heap);
}

static struct strollut_theap_node *
strollut_theap_peek_drpheap(const void * __restrict heap)
{
	return stroll_drpheap_entry(stroll_drpheap_base_peek(heap),
	                            struct strollut_theap_node,
	                            super.drpheap);
}

static void
strollut_theap_insert_drpheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_drpheap_base_insert(heap,
	                           &node->super.drpheap,
	                           strollut_theap_compare_drpheap,
	                           NULL);
}

static struct strollut_theap_node *
strollut_theap_extract_drpheap(void * __restrict heap)
{
	return stroll_drpheap_entry(
		stroll_drpheap_base_extract(heap,
		                            strollut_theap_compare_drpheap,
		                            NULL),
		struct strollut_theap_node,
		super.drpheap);
}

static void
strollut_theap_remove_drpheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_drpheap_base_remove(heap,
	                           &node->super.drpheap,
	                           strollut_theap_compare_drpheap,
	                           NULL);
}

static void
strollut_theap_promote_drpheap(void * __restrict                       heap,
                               struct strollut_theap_node * __restrict node)
{
	stroll_drpheap_base_promote(heap,
	                            &node->super.drpheap,
	                            strollut_theap_compare_drpheap,
	                            NULL);
}

static void
strollut_theap_demote_drpheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_drpheap_base_demote(heap,
	                           &node->super.drpheap,
	                           strollut_theap_compare_drpheap,
	                           NULL);
}

static void
strollut_theap_merge_drpheap(void * __restrict first, void * __restrict second)
{
	stroll_drpheap_base_merge(first,
	                          second,
	                          strollut_theap_compare_drpheap,
	                          NULL);
}

static void
strollut_theap_check_drpheap_props(const struct stroll_drpheap_link * link,
                                   struct strollut_theap_xpct *       xpct,
                                   unsigned int                       nr)
{
	const struct stroll_drpheap_node * node;
	const struct strollut_theap_node * entry;
	const struct stroll_drpheap_link * child;
	const struct stroll_drpheap_link * parent;
	unsigned int                       n;
	bool                               found = false;

	node = stroll_drpheap_node_from_link(link);
	entry = stroll_drpheap_entry(node,
	                             struct strollut_theap_node,
	                             super.drpheap);
	for (n = 0; n < nr; n++) {
		if ((xpct[n].key == entry->key) &&
		    (&xpct[n].node->super.drpheap == node)) {
			xpct[n].seen++;
			found = true;
			break;
		}
	}

	cute_check_bool(found, is, true);

	if (node->child == stroll_drpheap_tail)
		return;

	for (parent = &node->link, child = node->child;
	     child != stroll_drpheap_tail;
	     parent = child, child = child->edges[0]) {
		const struct stroll_drpheap_node * curr;

		curr = stroll_drpheap_node_from_link(child);

		cute_check_ptr(child->edges[1], equal, parent);
		cute_check_sint(curr->rank, lower_equal, node->rank);
		cute_check_sint(strollut_theap_compare_drpheap(node,
		                                               curr,
		                                               NULL),
		                lower_equal,
		                0);
		strollut_theap_check_drpheap_props(child, xpct, nr);
	}
}

static void
strollut_theap_validate_drpheap(void *                       heap,
                                struct strollut_theap_node * nodes,
                                unsigned int                 nr)
{
	const struct stroll_drpheap_base * hp = heap;
	struct strollut_theap_xpct         xpct[nr];
	unsigned int                       n;
	const struct stroll_drpheap_link * root;

	if (!nr) {
		cute_check_ptr(hp->roots.edges[0], unequal, &hp->roots);
		return;
	}

	for (n = 0; n < nr; n++) {
		xpct[n].key = nodes[n].key;
		xpct[n].seen = 0;
		xpct[n].node = &nodes[n];
	}

	for (root = hp->roots.edges[0];
	     root != &hp->roots;
	     root = root->edges[0]) {
		const struct stroll_drpheap_node * curr =
			stroll_drpheap_node_from_link(root);

		cute_check_sint(
			curr->rank,
			equal,
			stroll_drpheap_node_from_link(curr->child)->rank + 1);
		cute_check_ptr(root->edges[0]->edges[1],
		               equal,
		               stroll_drpheap_encode_root_link(root));

		strollut_theap_check_drpheap_props(root, xpct, nr);
	}

	for (n = 0; n < nr; n++)
		cute_check_uint(xpct[n].seen, equal, 1);
}

static const struct strollut_theap_ops strollut_theap_drpheap_ops = {
	.init     = strollut_theap_init_drpheap,
	.fini     = strollut_theap_fini_drpheap,
	.empty    = strollut_theap_drpheap_empty,
	.peek     = strollut_theap_peek_drpheap,
	.insert   = strollut_theap_insert_drpheap,
	.extract  = strollut_theap_extract_drpheap,
	.remove   = strollut_theap_remove_drpheap,
	.promote  = strollut_theap_promote_drpheap,
	.demote   = strollut_theap_demote_drpheap,
	.merge    = strollut_theap_merge_drpheap,
	.validate = strollut_theap_validate_drpheap
};

STROLLUT_THEAP_ALGO_SUP(strollut_theap_drpheap_setup,
                        &strollut_theap_drpheap_ops,
                        &strollut_drpheap_main,
                        &strollut_drpheap_alt);

#else /* !defined(CONFIG_STROLL_DRPHEAP) */

STROLLUT_THEAP_NOSETUP(strollut_theap_drpheap_setup)

#endif /* defined(CONFIG_STROLL_DRPHEAP) */

CUTE_SUITE_STATIC(strollut_drpheap_suite,
                  strollut_theap_group,
                  strollut_theap_drpheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

/******************************************************************************
 * Doubly linked list based pairing heap tests
 ******************************************************************************/

#if defined(CONFIG_STROLL_DPRHEAP)

static struct stroll_dprheap_base strollut_dprheap_main;
static struct stroll_dprheap_base strollut_dprheap_alt;

static int
strollut_theap_compare_dprheap(
	const struct stroll_dprheap_node * __restrict a,
	const struct stroll_dprheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollut_theap_node * _a =
		stroll_dprheap_entry(a,
		                     const struct strollut_theap_node,
		                     super.dprheap);
	const struct strollut_theap_node * _b =
		stroll_dprheap_entry(b,
		                     const struct strollut_theap_node,
		                     super.dprheap);

	return (_a->key > _b->key) - (_a->key < _b->key);
}

static void
strollut_theap_init_dprheap(void * __restrict heap)
{
	stroll_dprheap_base_setup(heap);
}

static void
strollut_theap_fini_dprheap(void * __restrict heap __unused)
{
}

static bool
strollut_theap_dprheap_empty(const void * __restrict heap)
{
	return stroll_dprheap_base_isempty(heap);
}

static struct strollut_theap_node *
strollut_theap_peek_dprheap(const void * __restrict heap)
{
	return stroll_dprheap_entry(stroll_dprheap_base_peek(heap),
	                            struct strollut_theap_node,
	                            super.dprheap);
}

static void
strollut_theap_insert_dprheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_dprheap_base_insert(heap,
	                           &node->super.dprheap,
	                           strollut_theap_compare_dprheap,
	                           NULL);
}

static struct strollut_theap_node *
strollut_theap_extract_dprheap(void * __restrict heap)
{
	return stroll_dprheap_entry(
		stroll_dprheap_base_extract(heap,
		                            strollut_theap_compare_dprheap,
		                            NULL),
		struct strollut_theap_node,
		super.dprheap);
}

static void
strollut_theap_remove_dprheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_dprheap_base_remove(heap,
	                           &node->super.dprheap,
	                           strollut_theap_compare_dprheap,
	                           NULL);
}

static void
strollut_theap_promote_dprheap(void * __restrict                       heap,
                               struct strollut_theap_node * __restrict node)
{
	stroll_dprheap_base_promote(heap,
	                            &node->super.dprheap,
	                            strollut_theap_compare_dprheap,
	                            NULL);
}

static void
strollut_theap_demote_dprheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_dprheap_base_demote(heap,
	                           &node->super.dprheap,
	                           strollut_theap_compare_dprheap,
	                           NULL);
}

static void
strollut_theap_merge_dprheap(void * __restrict first, void * __restrict second)
{
	stroll_dprheap_base_merge(first,
	                          second,
	                          strollut_theap_compare_dprheap,
	                          NULL);
}

static void
strollut_theap_check_dprheap_props(const struct stroll_dprheap_node * node,
                                   struct strollut_theap_xpct *       xpct,
                                   unsigned int                       nr)
{
	const struct strollut_theap_node * entry;
	const struct stroll_dprheap_node * child;
	unsigned int                       n;
	bool                               found = false;

	entry = stroll_dprheap_entry(node,
	                             struct strollut_theap_node,
	                             super.dprheap);
	for (n = 0; n < nr; n++) {
		if ((xpct[n].key == entry->key) &&
		    (&xpct[n].node->super.dprheap == node)) {
			xpct[n].seen++;
			found = true;
			break;
		}
	}

	cute_check_bool(found, is, true);

	stroll_dlist_foreach_entry(&node->children, child, siblings) {
		cute_check_ptr(child->parent, equal, node);
		cute_check_sint(strollut_theap_compare_dprheap(node,
		                                               child,
		                                               NULL),
		                lower_equal,
		                0);
		strollut_theap_check_dprheap_props(child, xpct, nr);
	}
}

static void
strollut_theap_validate_dprheap(void *                       heap,
                                struct strollut_theap_node * nodes,
                                unsigned int                 nr)
{
	const struct stroll_dprheap_base * hp = heap;
	struct strollut_theap_xpct         xpct[nr];
	unsigned int                       n;

	if (!nr) {
		cute_check_ptr(hp->root, equal, NULL);
		return;
	}

	for (n = 0; n < nr; n++) {
		xpct[n].key = nodes[n].key;
		xpct[n].seen = 0;
		xpct[n].node = &nodes[n];
	}

	cute_check_ptr(hp->root->parent, equal, NULL);
	strollut_theap_check_dprheap_props(hp->root, xpct, nr);

	for (n = 0; n < nr; n++)
		cute_check_uint(xpct[n].seen, equal, 1);
}

static const struct strollut_theap_ops strollut_theap_dprheap_ops = {
	.init     = strollut_theap_init_dprheap,
	.fini     = strollut_theap_fini_dprheap,
	.empty    = strollut_theap_dprheap_empty,
	.peek     = strollut_theap_peek_dprheap,
	.insert   = strollut_theap_insert_dprheap,
	.extract  = strollut_theap_extract_dprheap,
	.remove   = strollut_theap_remove_dprheap,
	.promote  = strollut_theap_promote_dprheap,
	.demote   = strollut_theap_demote_dprheap,
	.merge    = strollut_theap_merge_dprheap,
	.validate = strollut_theap_validate_dprheap
};

STROLLUT_THEAP_ALGO_SUP(strollut_theap_dprheap_setup,
                        &strollut_theap_dprheap_ops,
                        &strollut_dprheap_main,
                        &strollut_dprheap_alt);

#else /* !defined(CONFIG_STROLL_DPRHEAP) */

STROLLUT_THEAP_NOSETUP(strollut_theap_dprheap_setup)

#endif /* defined(CONFIG_STROLL_DPRHEAP) */

CUTE_SUITE_STATIC(strollut_dprheap_suite,
                  strollut_theap_group,
                  strollut_theap_dprheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

/******************************************************************************
 * Half-tree based pairing heap tests
 ******************************************************************************/

#if defined(CONFIG_STROLL_PPRHEAP)

static struct stroll_pprheap_base strollut_pprheap_main;
static struct stroll_pprheap_base strollut_pprheap_alt;

static int
strollut_theap_compare_pprheap(
	const struct stroll_pprheap_node * __restrict a,
	const struct stroll_pprheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollut_theap_node * _a =
		stroll_pprheap_entry(a,
		                     const struct strollut_theap_node,
		                     super.pprheap);
	const struct strollut_theap_node * _b =
		stroll_pprheap_entry(b,
		                     const struct strollut_theap_node,
		                     super.pprheap);

	return (_a->key > _b->key) - (_a->key < _b->key);
}

static void
strollut_theap_init_pprheap(void * __restrict heap)
{
	stroll_pprheap_base_setup(heap);
}

static void
strollut_theap_fini_pprheap(void * __restrict heap __unused)
{
}

static bool
strollut_theap_pprheap_empty(const void * __restrict heap)
{
	return stroll_pprheap_base_isempty(heap);
}

static struct strollut_theap_node *
strollut_theap_peek_pprheap(const void * __restrict heap)
{
	return stroll_pprheap_entry(stroll_pprheap_base_peek(heap),
	                            struct strollut_theap_node,
	                            super.pprheap);
}

static void
strollut_theap_insert_pprheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_pprheap_base_insert(heap,
	                           &node->super.pprheap,
	                           strollut_theap_compare_pprheap,
	                           NULL);
}

static struct strollut_theap_node *
strollut_theap_extract_pprheap(void * __restrict heap)
{
	return stroll_pprheap_entry(
		stroll_pprheap_base_extract(heap,
		                            strollut_theap_compare_pprheap,
		                            NULL),
		struct strollut_theap_node,
		super.pprheap);
}

static void
strollut_theap_remove_pprheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_pprheap_base_remove(heap,
	                           &node->super.pprheap,
	                           strollut_theap_compare_pprheap,
	                           NULL);
}

static void
strollut_theap_promote_pprheap(void * __restrict                       heap,
                               struct strollut_theap_node * __restrict node)
{
	stroll_pprheap_base_promote(heap,
	                            &node->super.pprheap,
	                            strollut_theap_compare_pprheap,
	                            NULL);
}

static void
strollut_theap_demote_pprheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_pprheap_base_demote(heap,
	                           &node->super.pprheap,
	                           strollut_theap_compare_pprheap,
	                           NULL);
}

static void
strollut_theap_merge_pprheap(void * __restrict first, void * __restrict second)
{
	stroll_pprheap_base_merge(first,
	                          second,
	                          strollut_theap_compare_pprheap,
	                          NULL);
}

static void
strollut_theap_check_pprheap_props(const struct stroll_pprheap_node * node,
                                   struct strollut_theap_xpct *       xpct,
                                   unsigned int                       nr)
{
	const struct strollut_theap_node * entry;
	const struct stroll_pprheap_node * child;
	const struct stroll_pprheap_node * parent = node;
	unsigned int                       n;
	bool                               found = false;

	entry = stroll_pprheap_entry(node,
	                             struct strollut_theap_node,
	                             super.pprheap);
	for (n = 0; n < nr; n++) {
		if ((xpct[n].key == entry->key) &&
		    (&xpct[n].node->super.pprheap == node)) {
			xpct[n].seen++;
			found = true;
			break;
		}
	}

	cute_check_bool(found, is, true);

	for (child = node->children[0];
	     child != stroll_pprheap_tail;
	     child = child->children[1]) {
		cute_check_ptr(child->parent, equal, parent);
		cute_check_sint(strollut_theap_compare_pprheap(node,
		                                               child,
		                                               NULL),
		                lower_equal,
		                0);
		strollut_theap_check_pprheap_props(child, xpct, nr);
		parent = child;
	}
}

static void
strollut_theap_validate_pprheap(void *                       heap,
                                struct strollut_theap_node * nodes,
                                unsigned int                 nr)
{
	const struct stroll_pprheap_base * hp = heap;
	struct strollut_theap_xpct         xpct[nr];
	unsigned int                       n;

	if (!nr) {
		cute_check_ptr(hp->root, equal, NULL);
		return;
	}

	for (n = 0; n < nr; n++) {
		xpct[n].key = nodes[n].key;
		xpct[n].seen = 0;
		xpct[n].node = &nodes[n];
	}

	cute_check_ptr(hp->root->parent, equal, NULL);
	strollut_theap_check_pprheap_props(hp->root, xpct, nr);

	for (n = 0; n < nr; n++)
		cute_check_uint(xpct[n].seen, equal, 1);
}

static const struct strollut_theap_ops strollut_theap_pprheap_ops = {
	.init     = strollut_theap_init_pprheap,
	.fini     = strollut_theap_fini_pprheap,
	.empty    = strollut_theap_pprheap_empty,
	.peek     = strollut_theap_peek_pprheap,
	.insert   = strollut_theap_insert_pprheap,
	.extract  = strollut_theap_extract_pprheap,
	.remove   = strollut_theap_remove_pprheap,
	.promote  = strollut_theap_promote_pprheap,
	.demote   = strollut_theap_demote_pprheap,
	.merge    = strollut_theap_merge_pprheap,
	.validate = strollut_theap_validate_pprheap
};

STROLLUT_THEAP_ALGO_SUP(strollut_theap_pprheap_setup,
                        &strollut_theap_pprheap_ops,
                        &strollut_pprheap_main,
                        &strollut_pprheap_alt);

#else /* !defined(CONFIG_STROLL_PPRHEAP) */

STROLLUT_THEAP_NOSETUP(strollut_theap_pprheap_setup)

#endif /* defined(CONFIG_STROLL_PPRHEAP) */

CUTE_SUITE_STATIC(strollut_pprheap_suite,
                  strollut_theap_group,
                  strollut_theap_pprheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

/******************************************************************************
 * Binomial heap tests
 ******************************************************************************/

#if defined(CONFIG_STROLL_DBNHEAP)

static struct stroll_dbnheap_base strollut_dbnheap_main;
static struct stroll_dbnheap_base strollut_dbnheap_alt;

static int
strollut_theap_compare_dbnheap(
	const struct stroll_dbnheap_node * __restrict a,
	const struct stroll_dbnheap_node * __restrict b,
	void *                                        data __unused)
{
	const struct strollut_theap_node * _a =
		stroll_dbnheap_entry(a,
		                    const struct strollut_theap_node,
		                    super.dbnheap);
	const struct strollut_theap_node * _b =
		stroll_dbnheap_entry(b,
		                    const struct strollut_theap_node,
		                    super.dbnheap);

	return (_a->key > _b->key) - (_a->key < _b->key);
}

static void
strollut_theap_init_dbnheap(void * __restrict heap)
{
	stroll_dbnheap_base_setup(heap);
}

static void
strollut_theap_fini_dbnheap(void * __restrict heap __unused)
{
}

static bool
strollut_theap_dbnheap_empty(const void * __restrict heap)
{
	return stroll_dbnheap_base_isempty(heap);
}

static struct strollut_theap_node *
strollut_theap_peek_dbnheap(const void * __restrict heap)
{
	return stroll_dbnheap_entry(
		stroll_dbnheap_base_peek(heap,
		                         strollut_theap_compare_dbnheap,
		                         NULL),
		struct strollut_theap_node,
		super.dbnheap);
}

static void
strollut_theap_insert_dbnheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_dbnheap_base_insert(heap,
	                           &node->super.dbnheap,
	                           strollut_theap_compare_dbnheap,
	                           NULL);
}

static struct strollut_theap_node *
strollut_theap_extract_dbnheap(void * __restrict heap)
{
	return stroll_dbnheap_entry(
		stroll_dbnheap_base_extract(heap,
		                            strollut_theap_compare_dbnheap,
		                            NULL),
		struct strollut_theap_node,
		super.dbnheap);
}

static void
strollut_theap_remove_dbnheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_dbnheap_base_remove(heap,
	                           &node->super.dbnheap,
	                           strollut_theap_compare_dbnheap,
	                           NULL);
}

static void
strollut_theap_promote_dbnheap(void * __restrict                       heap,
                               struct strollut_theap_node * __restrict node)
{
	stroll_dbnheap_base_promote(heap,
	                            &node->super.dbnheap,
	                            strollut_theap_compare_dbnheap,
	                            NULL);
}

static void
strollut_theap_demote_dbnheap(void * __restrict                       heap,
                              struct strollut_theap_node * __restrict node)
{
	stroll_dbnheap_base_demote(heap,
	                           &node->super.dbnheap,
	                           strollut_theap_compare_dbnheap,
	                           NULL);
}

static void
strollut_theap_merge_dbnheap(void * __restrict first, void * __restrict second)
{
	stroll_dbnheap_base_merge(first,
	                          second,
	                          strollut_theap_compare_dbnheap,
	                          NULL);
}

static void
strollut_theap_check_dbnheap_props(const struct stroll_dbnheap_node * node,
                                   struct strollut_theap_xpct *       xpct,
                                   unsigned int                       nr)
{
	const struct strollut_theap_node * entry;
	const struct stroll_dbnheap_node * child;
	unsigned int                       n;
	bool                               found = false;

	entry = stroll_dbnheap_entry(node,
	                             struct strollut_theap_node,
	                             super.dbnheap);
	for (n = 0; n < nr; n++) {
		if ((xpct[n].key == entry->key) &&
		    (&xpct[n].node->super.dbnheap == node)) {
			xpct[n].seen++;
			found = true;
			break;
		}
	}

	cute_check_bool(found, is, true);

	stroll_dlist_foreach_entry(&node->children, child, siblings) {
		cute_check_ptr(child->parent, equal, node);
		cute_check_uint(child->order, lower, node->order);
		cute_check_sint(strollut_theap_compare_dbnheap(node,
		                                               child,
		                                               NULL),
		                lower_equal,
		                0);
		strollut_theap_check_dbnheap_props(child, xpct, nr);
	}
}

static void
strollut_theap_validate_dbnheap(void *                       heap,
                                struct strollut_theap_node * nodes,
                                unsigned int                 nr)
{
	const struct stroll_dbnheap_base * hp = heap;
	struct strollut_theap_xpct         xpct[nr];
	unsigned int                       n;
	const struct stroll_dbnheap_node * root;

	if (!nr) {
		cute_check_bool(stroll_dlist_empty(&hp->roots), is, true);
		return;
	}

	for (n = 0; n < nr; n++) {
		xpct[n].key = nodes[n].key;
		xpct[n].seen = 0;
		xpct[n].node = &nodes[n];
	}

	stroll_dlist_foreach_entry(&hp->roots, root, siblings) {
		cute_check_ptr(root->parent, equal, NULL);
		strollut_theap_check_dbnheap_props(root, xpct, nr);
	}
}

static const struct strollut_theap_ops strollut_theap_dbnheap_ops = {
	.init     = strollut_theap_init_dbnheap,
	.fini     = strollut_theap_fini_dbnheap,
	.empty    = strollut_theap_dbnheap_empty,
	.peek     = strollut_theap_peek_dbnheap,
	.insert   = strollut_theap_insert_dbnheap,
	.extract  = strollut_theap_extract_dbnheap,
	.remove   = strollut_theap_remove_dbnheap,
	.promote  = strollut_theap_promote_dbnheap,
	.demote   = strollut_theap_demote_dbnheap,
	.merge    = strollut_theap_merge_dbnheap,
	.validate = strollut_theap_validate_dbnheap
};

STROLLUT_THEAP_ALGO_SUP(strollut_theap_dbnheap_setup,
                        &strollut_theap_dbnheap_ops,
                        &strollut_dbnheap_main,
                        &strollut_dbnheap_alt);

#else /* !defined(CONFIG_STROLL_DBNHEAP) */

STROLLUT_THEAP_NOSETUP(strollut_theap_dbnheap_setup)

#endif /* defined(CONFIG_STROLL_DBNHEAP) */

CUTE_SUITE_STATIC(strollut_dbnheap_suite,
                  strollut_theap_group,
                  strollut_theap_dbnheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

/******************************************************************************
 * Top-level suite
 ******************************************************************************/

CUTE_GROUP(strollut_theap_main_group) = {
	CUTE_REF(strollut_hprheap_suite),
	CUTE_REF(strollut_drpheap_suite),
	CUTE_REF(strollut_dprheap_suite),
	CUTE_REF(strollut_pprheap_suite),
	CUTE_REF(strollut_dbnheap_suite)
};

CUTE_SUITE_EXTERN(strollut_theap_suite,
                  strollut_theap_main_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
