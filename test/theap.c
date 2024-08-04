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
#include <stdlib.h>

#define STROLLUT_THEAP_NOASSERT(_test) \
	CUTE_TEST(_test) \
	{ \
		cute_skip("support not compiled-in"); \
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

struct strollut_theap_xpct {
	int                          key;
	unsigned int                 seen;
	struct strollut_theap_node * node;
};

typedef void (strollut_theap_setup_fn)(void * __restrict);

typedef bool (strollut_theap_empty_fn)(const void * __restrict);

typedef struct stroll_lcrs_node *
        (strollut_theap_peek_fn)(const void * __restrict);

typedef void
        (strollut_theap_insert_fn)(void * __restrict,
                                   struct stroll_lcrs_node * __restrict,
                                   stroll_lcrs_cmp_fn *,
                                   void *);

typedef struct stroll_lcrs_node *
        (strollut_theap_extract_fn)(void * __restrict,
                                    stroll_lcrs_cmp_fn *,
                                    void *);

typedef void
        (strollut_theap_remove_fn)(void * __restrict,
                                   struct stroll_lcrs_node * __restrict,
                                   stroll_lcrs_cmp_fn *,
                                   void *);

typedef void
        (strollut_theap_rekey_fn)(void * __restrict,
                                  struct stroll_lcrs_node * __restrict,
                                  stroll_lcrs_cmp_fn *,
                                  void *);

typedef void
        (strollut_theap_merge_fn)(void * __restrict,
                                  void * __restrict,
                                  stroll_lcrs_cmp_fn *,
                                  void *);

typedef void
        (strollut_theap_validate_fn)(void *,
                                     struct strollut_theap_node *,
                                     unsigned int,
                                     stroll_lcrs_cmp_fn *,
                                     void *);

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

static struct stroll_lcrs_node * strollut_prheap_main;
static struct stroll_lcrs_node * strollut_prheap_alt;

static void
strollut_theap_init_prheap(void * __restrict heap)
{
	_stroll_prheap_setup(heap);
}

static void
strollut_theap_fini_prheap(void * __restrict heap __unused)
{
}

static bool
strollut_theap_prheap_empty(const void * __restrict heap)
{
	return _stroll_prheap_isempty(heap);
}

static struct stroll_lcrs_node *
strollut_theap_peek_prheap(const void * __restrict heap)
{
	return _stroll_prheap_peek(heap);
}

static void
strollut_theap_insert_prheap(void * __restrict                    heap,
                             struct stroll_lcrs_node * __restrict node,
                             stroll_lcrs_cmp_fn *                 compare,
                             void *                               data)
{
	_stroll_prheap_insert(heap, node, compare, data);
}

static struct stroll_lcrs_node *
strollut_theap_extract_prheap(void * __restrict    heap,
                              stroll_lcrs_cmp_fn * compare,
                              void *               data)
{
	return _stroll_prheap_extract(heap, compare, data);
}

static void
strollut_theap_remove_prheap(void * __restrict                    heap,
                             struct stroll_lcrs_node * __restrict node,
                             stroll_lcrs_cmp_fn *                 compare,
                             void *                               data)
{
	_stroll_prheap_remove(heap, node, compare, data);
}

static void
strollut_theap_promote_prheap(void * __restrict                    heap,
                              struct stroll_lcrs_node * __restrict node,
                              stroll_lcrs_cmp_fn *                 compare,
                              void *                               data)
{
	_stroll_prheap_promote(heap, node, compare, data);
}

static void
strollut_theap_demote_prheap(void * __restrict                    heap,
                             struct stroll_lcrs_node * __restrict node,
                             stroll_lcrs_cmp_fn *                 compare,
                             void *                               data)
{
	_stroll_prheap_demote(heap, node, compare, data);
}

static void
strollut_theap_merge_prheap(void * __restrict    first,
                            void * __restrict    second,
                            stroll_lcrs_cmp_fn * compare,
                            void *               data)
{
	_stroll_prheap_merge(first, second, compare, data);
}

static void
strollut_theap_check_prheap_props(const struct stroll_lcrs_node * node,
                                  struct strollut_theap_xpct *    xpct,
                                  unsigned int                    nr,
                                  stroll_lcrs_cmp_fn *            compare,
                                  void *                          data)
{
	const struct strollut_theap_node * entry;
	const struct stroll_lcrs_node *    child;
	unsigned int                       n;
	bool                               found = false;

	entry = stroll_lcrs_entry(node,
	                          struct strollut_theap_node,
	                          super);
	for (n = 0; n < nr; n++) {
		if ((xpct[n].key == entry->key) &&
		    (&xpct[n].node->super == node)) {
			xpct[n].seen++;
			found = true;
			break;
		}
	}

	cute_check_bool(found, is, true);

	stroll_lcrs_foreach_child(node, child) {
		cute_check_sint(compare(node, child, data), lower_equal, 0);
		strollut_theap_check_prheap_props(child,
		                                  xpct,
		                                  nr,
		                                  compare,
		                                  data);
	}
}

static void
strollut_theap_validate_prheap(void *                       heap,
                               struct strollut_theap_node * nodes,
                               unsigned int                 nr,
                               stroll_lcrs_cmp_fn *         compare,
                               void *                       data)
{
	const struct stroll_lcrs_node * const * hp = heap;
	struct strollut_theap_xpct              xpct[nr];
	unsigned int                            n;

	if (!nr) {
		cute_check_ptr(*hp, equal, NULL);
		return;
	}

	for (n = 0; n < nr; n++) {
		xpct[n].key = nodes[n].key;
		xpct[n].seen = 0;
		xpct[n].node = &nodes[n];
	}

	strollut_theap_check_prheap_props(*hp, xpct, nr, compare, data);

	for (n = 0; n < nr; n++)
		cute_check_uint(xpct[n].seen, equal, 1);
}

static const struct strollut_theap_ops strollut_theap_prheap_ops = {
	.init     = strollut_theap_init_prheap,
	.fini     = strollut_theap_fini_prheap,
	.empty    = strollut_theap_prheap_empty,
	.peek     = strollut_theap_peek_prheap,
	.insert   = strollut_theap_insert_prheap,
	.extract  = strollut_theap_extract_prheap,
	.remove   = strollut_theap_remove_prheap,
	.promote  = strollut_theap_promote_prheap,
	.demote   = strollut_theap_demote_prheap,
	.merge    = strollut_theap_merge_prheap,
	.validate = strollut_theap_validate_prheap
};

STROLLUT_THEAP_ALGO_SUP(strollut_theap_prheap_setup,
                        &strollut_theap_prheap_ops,
                        &strollut_prheap_main,
                        &strollut_prheap_alt);

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

STROLLUT_THEAP_NOASSERT(strollut_theap_peek_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

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
		strollut_theap_algo->insert(strollut_theap_main,
		                            NULL,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->insert(strollut_theap_main,
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

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);
	strollut_theap_algo->insert(strollut_theap_main,
	                            &node.super,
	                            strollut_theap_compare_min,
	                            NULL);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);
	cute_check_ptr(strollut_theap_algo->peek(strollut_theap_main),
	               equal,
	               &node.super);
}

static void
strollut_theap_check_insert(struct strollut_theap_node * nodes, unsigned int nr)
{
	unsigned int n;

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);

	for (n = 0; n < nr; n++) {
		strollut_theap_algo->insert(strollut_theap_main,
		                            &nodes[n].super,
		                            strollut_theap_compare_min,
		                            NULL);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              n + 1,
		                              strollut_theap_compare_min,
		                              NULL);
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
	cute_expect_assertion(
		strollut_theap_algo->extract(NULL,
		                             strollut_theap_compare_min,
		                             NULL));
	cute_expect_assertion(
		strollut_theap_algo->extract(strollut_theap_main,
		                             strollut_theap_compare_min,
		                             NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_extract_assert)

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
		strollut_theap_algo->insert(strollut_theap_main,
		                            &nodes[n].super,
		                            strollut_theap_compare_min,
		                            NULL);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	for (n = 0; n < nr; n++) {
		const struct stroll_lcrs_node *    lcrs;
		const struct strollut_theap_node * entry;

		lcrs = strollut_theap_algo->extract(strollut_theap_main,
		                                    strollut_theap_compare_min,
		                                    NULL);
		cute_check_ptr(lcrs, unequal, NULL);

		entry = stroll_lcrs_entry(lcrs,
		                          struct strollut_theap_node,
		                          super);
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
	struct stroll_lcrs_node node = STROLL_LCRS_INIT(node);

	cute_expect_assertion(
		strollut_theap_algo->remove(NULL,
		                            &node,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->remove(strollut_theap_main,
		                            NULL,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->remove(strollut_theap_main,
		                            &node,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->remove(strollut_theap_main,
		                            &node,
		                            NULL,
		                            NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_remove_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static void
strollut_theap_check_remove(struct strollut_theap_node * nodes, unsigned int nr)
{
	unsigned int               n;
	struct strollut_theap_xpct tmp[nr];

	for (n = 0; n < nr; n++) {
		tmp[n].key = nodes[n].key;
		tmp[n].node = &nodes[n];
	}

	qsort(tmp, nr, sizeof(tmp[0]), strollut_theap_compare_xpct);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                true);

	for (n = 0; n < nr; n++)
		strollut_theap_algo->insert(strollut_theap_main,
		                            &tmp[n].node->super,
		                            strollut_theap_compare_min,
		                            NULL);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	for (n = 0; n < nr; n++) {
		strollut_theap_algo->validate(strollut_theap_main,
		                              &nodes[n],
		                              nr - n,
		                              strollut_theap_compare_min,
		                              NULL);
		strollut_theap_algo->remove(strollut_theap_main,
		                            &nodes[n].super,
		                            strollut_theap_compare_min,
		                            NULL);
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
	struct stroll_lcrs_node node = STROLL_LCRS_INIT(node);

	cute_expect_assertion(
		strollut_theap_algo->promote(NULL,
		                             &node,
		                             strollut_theap_compare_min,
		                             NULL));
	cute_expect_assertion(
		strollut_theap_algo->promote(strollut_theap_main,
		                             NULL,
		                             strollut_theap_compare_min,
		                             NULL));
	cute_expect_assertion(
		strollut_theap_algo->promote(strollut_theap_main,
		                             &node,
		                             strollut_theap_compare_min,
		                             NULL));
	cute_expect_assertion(
		strollut_theap_algo->promote(strollut_theap_main,
		                             &node,
		                             NULL,
		                             NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_promote_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static void
strollut_theap_check_promote(struct strollut_theap_node * nodes,
                             unsigned int                 nr)
{
	unsigned int               n;
	struct strollut_theap_xpct tmp[nr];
	int                        shift;

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
		strollut_theap_algo->insert(strollut_theap_main,
		                            &tmp[n].node->super,
		                            strollut_theap_compare_min,
		                            NULL);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	for (n = 0; n < nr; n++) {
		strollut_theap_algo->promote(strollut_theap_main,
		                             &nodes[n].super,
		                             strollut_theap_compare_min,
		                             NULL);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              nr,
		                              strollut_theap_compare_min,
		                              NULL);
	}

	for (n = 0; n < nr; n++) {
		nodes[n].key -= shift;
		strollut_theap_algo->promote(strollut_theap_main,
		                             &nodes[n].super,
		                             strollut_theap_compare_min,
		                             NULL);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              nr,
		                              strollut_theap_compare_min,
		                              NULL);
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
	struct stroll_lcrs_node node = STROLL_LCRS_INIT(node);

	cute_expect_assertion(
		strollut_theap_algo->demote(NULL,
		                            &node,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->demote(strollut_theap_main,
		                            NULL,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->demote(strollut_theap_main,
		                            &node,
		                            strollut_theap_compare_min,
		                            NULL));
	cute_expect_assertion(
		strollut_theap_algo->demote(strollut_theap_main,
		                            &node,
		                            NULL,
		                            NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_demote_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static void
strollut_theap_check_demote(struct strollut_theap_node * nodes,
                            unsigned int                 nr)
{
	unsigned int               n;
	struct strollut_theap_xpct tmp[nr];
	int                        shift;

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
		strollut_theap_algo->insert(strollut_theap_main,
		                            &tmp[n].node->super,
		                            strollut_theap_compare_min,
		                            NULL);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	for (n = 0; n < nr; n++) {
		strollut_theap_algo->demote(strollut_theap_main,
		                            &nodes[n].super,
		                            strollut_theap_compare_min,
		                            NULL);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              nr,
		                              strollut_theap_compare_min,
		                              NULL);
	}

	for (n = 0; n < nr; n++) {
		nodes[n].key += shift;
		strollut_theap_algo->demote(strollut_theap_main,
		                            &nodes[n].super,
		                            strollut_theap_compare_min,
		                            NULL);
		strollut_theap_algo->validate(strollut_theap_main,
		                              nodes,
		                              nr,
		                              strollut_theap_compare_min,
		                              NULL);
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
	cute_expect_assertion(
		strollut_theap_algo->merge(NULL,
		                           strollut_theap_alt,
		                           strollut_theap_compare_min,
		                           NULL));
	cute_expect_assertion(
		strollut_theap_algo->merge(strollut_theap_main,
		                           NULL,
		                           strollut_theap_compare_min,
		                           NULL));
	cute_expect_assertion(
		strollut_theap_algo->merge(strollut_theap_main,
		                           strollut_theap_alt,
		                           NULL,
		                           NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_THEAP_NOASSERT(strollut_theap_merge_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_theap_merge_both_empty)
{
	strollut_theap_algo->merge(strollut_theap_main,
	                           strollut_theap_alt,
	                           strollut_theap_compare_min,
	                           NULL);
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
	strollut_theap_algo->insert(strollut_theap_main,
	                            &node.super,
	                            strollut_theap_compare_min,
	                            NULL);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	strollut_theap_algo->merge(strollut_theap_main,
	                           strollut_theap_alt,
	                           strollut_theap_compare_min,
	                           NULL);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	strollut_theap_algo->validate(strollut_theap_main,
	                              &node,
	                              1,
	                              strollut_theap_compare_min,
	                              NULL);
}

CUTE_TEST(strollut_theap_merge_res_empty)
{
	struct strollut_theap_node node = STROLLUT_THEAP_INIT_NODE(1);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_alt),
	                is,
	                true);
	strollut_theap_algo->insert(strollut_theap_alt,
	                            &node.super,
	                            strollut_theap_compare_min,
	                            NULL);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_alt),
	                is,
	                false);

	strollut_theap_algo->merge(strollut_theap_main,
	                           strollut_theap_alt,
	                           strollut_theap_compare_min,
	                           NULL);

	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	strollut_theap_algo->validate(strollut_theap_main,
	                              &node,
	                              1,
	                              strollut_theap_compare_min,
	                              NULL);
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
		strollut_theap_algo->insert(strollut_theap_main,
		                            &tmp[n].super,
		                            strollut_theap_compare_min,
		                            NULL);
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
		                            &tmp[res_nr + n].super,
		                            strollut_theap_compare_min,
		                            NULL);
	}
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_alt),
	                is,
	                false);

	strollut_theap_algo->merge(strollut_theap_main,
	                           strollut_theap_alt,
	                           strollut_theap_compare_min,
	                           NULL);
	cute_check_bool(strollut_theap_algo->empty(strollut_theap_main),
	                is,
	                false);

	strollut_theap_algo->validate(strollut_theap_main,
	                              tmp,
	                              res_nr + src_nr,
	                              strollut_theap_compare_min,
	                              NULL);
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

/******************************************************************************
 * Top-level suite
 ******************************************************************************/

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

CUTE_SUITE_EXTERN(strollut_prheap_suite,
                  strollut_theap_group,
                  strollut_theap_prheap_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
