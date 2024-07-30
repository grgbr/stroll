/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/prheap.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>

#define STROLLUT_THEAP_NOASSERT(_test) \
	CUTE_TEST(_test) \
	{ \
		cute_skip("support not compiled-in"); \
	}

typedef void (strollut_theap_setup_fn)(void * __restrict);

typedef bool (strollut_theap_empty_fn)(const void * __restrict);

typedef struct stroll_lcrs_node *
        (strollut_theap_peek_fn)(const void * __restrict);

typedef void
        (strollut_theap_insert_fn)(void * __restrict,
                                   struct stroll_lcrs_node * __restrict,
                                   stroll_lcrs_cmp_fn   *,
                                   void *);

typedef struct stroll_lcrs_node *
        (strollut_theap_extract_fn)(void * __restrict,
                                    stroll_lcrs_cmp_fn *,
                                    void *);


struct strollut_theap_ops {
	strollut_theap_setup_fn *   init;
	strollut_theap_setup_fn *   fini;
	strollut_theap_empty_fn *   empty;
	strollut_theap_peek_fn *    peek;
	strollut_theap_insert_fn *  insert;
	strollut_theap_extract_fn * extract;
};

static const struct strollut_theap_ops * strollut_theap_algo;
static void *                            strollut_theap;

#define STROLLUT_THEAP_ALGO_SUP(_setup, _algo, _heap) \
	static void _setup(void) \
	{ \
		strollut_theap_algo = _algo; \
		strollut_theap = _heap; \
	}

struct strollut_theap_node {
	struct stroll_lcrs_node super;
	int                     key;
};

#define STROLLUT_THEAP_INIT_NODE(_key) \
	{ \
		.super = { \
			.sibling  = (struct stroll_lcrs_node *)0xdeadbeef, \
			.youngest = (struct stroll_lcrs_node *)0xdeadbeef, \
		}, \
		.key = _key \
	}

static int
strollut_theap_compare_min(
	const struct stroll_lcrs_node * __restrict a,
	const struct stroll_lcrs_node * __restrict b,
	void *                                     data __unused)
{
	const struct strollut_theap_node * _a =
		stroll_lcrs_entry(a, const struct strollut_theap_node, super);
	const struct strollut_theap_node * _b =
		stroll_lcrs_entry(b, const struct strollut_theap_node, super);

	cute_check_bool(stroll_lcrs_istail(a), is, false);
	cute_check_bool(stroll_lcrs_istail(b), is, false);

	return (_a->key > _b->key) - (_a->key < _b->key);
}

/******************************************************************************
 * Pairing heap tests
 ******************************************************************************/

static struct stroll_prheap strollut_prheap;

static void
strollut_theap_init_prheap(void * __restrict heap)
{
	stroll_prheap_init(heap);
}

static void
strollut_theap_fini_prheap(void * __restrict heap)
{
	stroll_prheap_fini(heap);
}

static bool
strollut_theap_prheap_empty(const void * __restrict heap)
{
	return stroll_prheap_empty(heap);
}

static struct stroll_lcrs_node *
strollut_theap_peek_prheap(const void * __restrict heap)
{
	return stroll_prheap_peek(heap);
}

static void
strollut_theap_insert_prheap(void * __restrict                    heap,
                             struct stroll_lcrs_node * __restrict node,
                             stroll_lcrs_cmp_fn *                 compare,
                             void *                               data)
{
	stroll_prheap_insert(heap, node, compare, data);
}

static struct stroll_lcrs_node *
strollut_theap_extract_prheap(void * __restrict    heap,
                              stroll_lcrs_cmp_fn * compare,
                              void *               data)
{
	return stroll_prheap_extract(heap, compare, data);
}

static const struct strollut_theap_ops strollut_theap_prheap_ops = {
	.init    = strollut_theap_init_prheap,
	.fini    = strollut_theap_fini_prheap,
	.empty   = strollut_theap_prheap_empty,
	.peek    = strollut_theap_peek_prheap,
	.insert  = strollut_theap_insert_prheap,
	.extract = strollut_theap_extract_prheap,
};

STROLLUT_THEAP_ALGO_SUP(strollut_theap_prheap_setup,
                        &strollut_theap_prheap_ops,
                        &strollut_prheap);

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_init_assert)
{
	cute_expect_assertion(strollut_theap_algo->init(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_init_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_check_empty_assert)
{
	cute_expect_assertion(strollut_theap_algo->empty(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_theap_check_empty)
{
	strollut_theap_algo->init(strollut_theap);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_peek_empty_assert)
{
	cute_expect_assertion(strollut_theap_algo->peek(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_peek_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_theap_peek_empty)
{
	strollut_theap_algo->init(strollut_theap);

	cute_check_ptr(strollut_theap_algo->peek(strollut_theap), equal, NULL);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_insert_empty_assert)
{
	struct strollut_theap_node node = STROLLUT_THEAP_INIT_NODE(1);

	cute_expect_assertion(
		strollut_theap_algo->insert(NULL,
		                            &node.super,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->insert(strollut_theap,
		                            NULL,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->insert(strollut_theap,
		                            &node.super,
		                            NULL,
		                            NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_insert_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_theap_insert_empty)
{
	struct strollut_theap_node node = STROLLUT_THEAP_INIT_NODE(1);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap), is, true);
	strollut_theap_algo->insert(strollut_theap,
	                            &node.super,
	                            strollut_theap_compare_min,
	                            NULL);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap), is, false);
	cute_check_ptr(strollut_theap_algo->peek(strollut_theap),
	               equal,
	               &node.super);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_theap_extract_assert)
{
	strollut_theap_algo->init(strollut_theap);

	cute_expect_assertion(
		strollut_theap_algo->extract(NULL,
		                             strollut_theap_compare_min,
		                             NULL));
	cute_expect_assertion(
		strollut_theap_algo->extract(strollut_theap,
		                             strollut_theap_compare_min,
		                             NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_extract_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/******************************************************************************
 * Top-level suite
 ******************************************************************************/

CUTE_GROUP(strollut_theap_group) = {
	CUTE_REF(strollut_theap_init_assert),
	CUTE_REF(strollut_theap_check_empty_assert),
	CUTE_REF(strollut_theap_check_empty),
	CUTE_REF(strollut_theap_peek_empty_assert),
	CUTE_REF(strollut_theap_peek_empty),
	CUTE_REF(strollut_theap_insert_empty_assert),
	CUTE_REF(strollut_theap_insert_empty),
	CUTE_REF(strollut_theap_extract_assert),
};

CUTE_SUITE_EXTERN(strollut_prheap_suite,
                  strollut_theap_group,
                  strollut_theap_prheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
