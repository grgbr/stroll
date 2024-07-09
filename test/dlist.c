/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/dlist.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <string.h>

#define STROLLUT_DLIST_NOASSERT(_test) \
	CUTE_TEST(_test) \
	{ \
		cute_skip("support not compiled-in"); \
	}

CUTE_TEST(strollut_dlist_empty)
{
	struct stroll_dlist_node lst = STROLL_DLIST_INIT(lst);

	cute_check_bool(stroll_dlist_empty(&lst), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_empty_assert)
{
	cute_expect_assertion(stroll_dlist_empty(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_init)
{
	struct stroll_dlist_node lst;

	stroll_dlist_init(&lst);

	cute_check_bool(stroll_dlist_empty(&lst), is, true);
}

CUTE_TEST(strollut_dlist_iterate_empty)
{
	struct stroll_dlist_node   lst = STROLL_DLIST_INIT(lst);
	struct stroll_dlist_node * node;

	stroll_dlist_foreach_node(&lst, node)
		cute_fail("iterable empty dlist !");
}

CUTE_TEST(strollut_dlist_single)
{
	struct stroll_dlist_node   first;
	struct stroll_dlist_node   list = {
		.next = &first,
		.prev = &first
	};
	struct stroll_dlist_node * node;
	unsigned int               cnt;

	first.next = &list;
	first.prev = &list;

	cute_check_ptr(stroll_dlist_next(&list), equal, &first);
	cute_check_ptr(stroll_dlist_next(&first), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &first);
	cute_check_ptr(stroll_dlist_prev(&first), equal, &list);

	cnt = 0;
	stroll_dlist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &first);
		cnt++;
	}
	cute_check_uint(cnt, equal, 1);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_single_assert)
{
	cute_expect_assertion(stroll_dlist_next(NULL));
	cute_expect_assertion(stroll_dlist_prev(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_single_assert);

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_second)
{
	struct stroll_dlist_node   second;
	struct stroll_dlist_node   first;
	struct stroll_dlist_node   list = {
		.next = &first,
		.prev = &second
	};
	struct stroll_dlist_node * node;
	unsigned int               cnt;

	first.next = &second;
	first.prev = &list;
	second.next = &list;
	second.prev = &first;

	cute_check_ptr(stroll_dlist_next(&list), equal, &first);
	cute_check_ptr(stroll_dlist_next(&first), equal, &second);
	cute_check_ptr(stroll_dlist_next(&second), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &second);
	cute_check_ptr(stroll_dlist_prev(&second), equal, &first);
	cute_check_ptr(stroll_dlist_prev(&first), equal, &list);

	cnt = 0;
	stroll_dlist_foreach_node(&list, node)
		cnt++;
	cute_check_uint(cnt, equal, 2);
}

CUTE_TEST(strollut_dlist_insert)
{
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node   nodes[8];
	struct stroll_dlist_node * node;
	unsigned int               c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_insert(&list, &nodes[c]);

	c = 0;
	stroll_dlist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}

	cute_check_uint(c, equal, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_insert_assert)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node node;

	cute_expect_assertion(stroll_dlist_insert(NULL, &node));
	cute_expect_assertion(stroll_dlist_insert(&list, NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_insert_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_append)
{
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node   nodes[8];
	struct stroll_dlist_node * node;
	unsigned int               c;

	stroll_dlist_append(&list, &nodes[0]);
	for (c = 1; c < stroll_array_nr(nodes); c++)
		stroll_dlist_append(&nodes[c - 1], &nodes[c]);

	c = 0;
	stroll_dlist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}

	cute_check_uint(c, equal, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_append_assert)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node node;

	cute_expect_assertion(stroll_dlist_append(NULL, &node));
	cute_expect_assertion(stroll_dlist_append(&list, NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_append_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_nqueue_front)
{
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node   nodes[8];
	struct stroll_dlist_node * node;
	unsigned int               c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_front(&list, &nodes[c]);

	c = 0;
	stroll_dlist_foreach_node(&list, node) {
		cute_check_ptr(node,
		               equal,
		               &nodes[stroll_array_nr(nodes) - 1 - c]);
		c++;
	}

	cute_check_uint(c, equal, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_nqueue_front_assert)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node node;

	cute_expect_assertion(stroll_dlist_nqueue_front(NULL, &node));
	cute_expect_assertion(stroll_dlist_nqueue_front(&list, NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_nqueue_front_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_nqueue_back)
{
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node   nodes[8];
	struct stroll_dlist_node * node;
	unsigned int               c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	c = 0;
	stroll_dlist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}

	cute_check_uint(c, equal, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_nqueue_back_assert)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node node;

	cute_expect_assertion(stroll_dlist_nqueue_back(NULL, &node));
	cute_expect_assertion(stroll_dlist_nqueue_back(&list, NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_nqueue_back_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_remove)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[8];
	unsigned int             c;

	stroll_dlist_append(&list, &nodes[0]);
	for (c = 1; c < stroll_array_nr(nodes); c++)
		stroll_dlist_append(&nodes[c - 1], &nodes[c]);

	for (c = 0; c < stroll_array_nr(nodes); c++) {
		cute_check_ptr(stroll_dlist_next(&list), equal, &nodes[c]);
		cute_check_ptr(stroll_dlist_prev(&list), equal, &nodes[7]);
		stroll_dlist_remove(&nodes[c]);
	}

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_remove_assert)
{
	cute_expect_assertion(stroll_dlist_remove(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_remove_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_remove_init)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[8];
	unsigned int             c;

	stroll_dlist_append(&list, &nodes[0]);
	for (c = 1; c < stroll_array_nr(nodes); c++)
		stroll_dlist_append(&nodes[c - 1], &nodes[c]);

	for (c = 0; c < stroll_array_nr(nodes); c++) {
		cute_check_ptr(stroll_dlist_next(&list), equal, &nodes[c]);
		cute_check_ptr(stroll_dlist_prev(&list), equal, &nodes[7]);
		stroll_dlist_remove_init(&nodes[c]);
		cute_check_bool(stroll_dlist_empty(&nodes[c]), is, true);
	}

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_remove_init_assert)
{
	cute_expect_assertion(stroll_dlist_remove_init(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_remove_init_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_dqueue_front)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[8];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	for (c = 0; c < stroll_array_nr(nodes); c++)
		cute_check_ptr(stroll_dlist_dqueue_front(&list),
		               equal,
		               &nodes[c]);

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_dqueue_front_assert)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_dqueue_front(NULL));
	cute_expect_assertion(stroll_dlist_dqueue_front(&list));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_dqueue_front_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_dqueue_front_init)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[8];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	for (c = 0; c < stroll_array_nr(nodes); c++) {
		cute_check_ptr(stroll_dlist_dqueue_front_init(&list),
		               equal,
		               &nodes[c]);
		cute_check_bool(stroll_dlist_empty(&nodes[c]), is, true);
	}

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_dqueue_front_init_assert)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_dqueue_front_init(NULL));
	cute_expect_assertion(stroll_dlist_dqueue_front_init(&list));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_dqueue_front_init_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_dqueue_back)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[8];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_front(&list, &nodes[c]);

	for (c = 0; c < stroll_array_nr(nodes); c++)
		cute_check_ptr(stroll_dlist_dqueue_back(&list),
		               equal,
		               &nodes[c]);

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_dqueue_back_assert)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_dqueue_back(NULL));
	cute_expect_assertion(stroll_dlist_dqueue_back(&list));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_dqueue_back_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_dqueue_back_init)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[8];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_front(&list, &nodes[c]);

	for (c = 0; c < stroll_array_nr(nodes); c++) {
		cute_check_ptr(stroll_dlist_dqueue_back_init(&list),
		               equal,
		               &nodes[c]);
		cute_check_bool(stroll_dlist_empty(&nodes[c]), is, true);
	}

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_dqueue_back_init_assert)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_dqueue_back_init(NULL));
	cute_expect_assertion(stroll_dlist_dqueue_back_init(&list));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_dqueue_back_init_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_replace)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[3];
	struct stroll_dlist_node node;
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_replace(&nodes[0], &node);
	cute_check_ptr(stroll_dlist_next(&list), equal, &node);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &node);
	stroll_dlist_replace(&node, &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &nodes[0]);

	stroll_dlist_replace(&nodes[1], &node);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &node);
	cute_check_ptr(stroll_dlist_prev(&nodes[2]), equal, &node);
	stroll_dlist_replace(&node, &nodes[1]);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[2]), equal, &nodes[1]);

	stroll_dlist_replace(&nodes[2], &node);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &node);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &node);
	stroll_dlist_replace(&node, &nodes[2]);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &nodes[2]);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_replace_assert)
{
	struct stroll_dlist_node   node;
	struct stroll_dlist_node * ptr = &node;
	struct stroll_dlist_node   list = {
		.next = &node,
		.prev = &node
	};

	cute_expect_assertion(stroll_dlist_replace(NULL, &node));
	cute_expect_assertion(stroll_dlist_replace(&list, NULL));
	cute_expect_assertion(stroll_dlist_replace(&list, &node));
	cute_expect_assertion(stroll_dlist_replace(ptr, &node));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_replace_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_move_before)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[4];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_move_before(&list, &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&nodes[3]), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &nodes[3]);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&list), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &list);

	stroll_dlist_move_before(&nodes[1], &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &list);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_prev(&nodes[2]), equal, &nodes[1]);

	stroll_dlist_move_before(&nodes[1], &nodes[2]);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_prev(&nodes[2]), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&nodes[2]), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &nodes[3]);
	cute_check_ptr(stroll_dlist_prev(&nodes[3]), equal, &nodes[1]);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_move_before_assert)
{
	struct stroll_dlist_node   node;
	struct stroll_dlist_node * ptr = &node;
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_move_before(NULL, &node));
	cute_expect_assertion(stroll_dlist_move_before(&list, NULL));
	cute_expect_assertion(stroll_dlist_move_before(ptr, &node));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_move_before_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_move_after)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[4];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_move_after(&list, &nodes[3]);
	cute_check_ptr(stroll_dlist_next(&nodes[2]), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_next(&list), equal, &nodes[3]);
	cute_check_ptr(stroll_dlist_prev(&nodes[3]), equal, &list);
	cute_check_ptr(stroll_dlist_next(&nodes[3]), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &nodes[3]);

	stroll_dlist_move_after(&nodes[2], &nodes[3]);
	cute_check_ptr(stroll_dlist_next(&nodes[2]), equal, &nodes[3]);
	cute_check_ptr(stroll_dlist_prev(&nodes[3]), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_next(&nodes[3]), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &nodes[3]);
	cute_check_ptr(stroll_dlist_next(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &list);

	stroll_dlist_move_after(&nodes[2], &nodes[1]);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_prev(&nodes[2]), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&nodes[2]), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &nodes[3]);
	cute_check_ptr(stroll_dlist_prev(&nodes[3]), equal, &nodes[1]);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_move_after_assert)
{
	struct stroll_dlist_node   node;
	struct stroll_dlist_node * ptr = &node;
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_move_after(NULL, &node));
	cute_expect_assertion(stroll_dlist_move_after(&list, NULL));
	cute_expect_assertion(stroll_dlist_move_after(ptr, &node));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_move_after_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_withdraw_lead)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[4];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_withdraw(&nodes[0], &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&list), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &list);
}

CUTE_TEST(strollut_dlist_withdraw_mid)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[4];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_withdraw(&nodes[1], &nodes[1]);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &nodes[2]);
	cute_check_ptr(stroll_dlist_prev(&nodes[2]), equal, &nodes[0]);
}

CUTE_TEST(strollut_dlist_withdraw_trail)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[4];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_withdraw(&nodes[3], &nodes[3]);
	cute_check_ptr(stroll_dlist_next(&nodes[2]), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &nodes[2]);
}

CUTE_TEST(strollut_dlist_withdraw_range)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[4];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_withdraw(&nodes[1], &nodes[2]);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &nodes[3]);
	cute_check_ptr(stroll_dlist_prev(&nodes[3]), equal, &nodes[0]);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_withdraw_assert)
{
	struct stroll_dlist_node node;

	cute_expect_assertion(stroll_dlist_withdraw(NULL, &node));
	cute_expect_assertion(stroll_dlist_withdraw(&node, NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_withdraw_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_embed_after_lead)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[2];
	struct stroll_dlist_node node;
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_embed_after(&list, &node, &node);
	cute_check_ptr(stroll_dlist_next(&list), equal, &node);
	cute_check_ptr(stroll_dlist_prev(&node), equal, &list);
	cute_check_ptr(stroll_dlist_next(&node), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &node);
}

CUTE_TEST(strollut_dlist_embed_after_mid)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node dst[4];
	struct stroll_dlist_node src[2];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(dst); c++)
		stroll_dlist_nqueue_back(&list, &dst[c]);
	for (c = 0; c < stroll_array_nr(src); c++)
		stroll_dlist_nqueue_back(&list, &src[c]);

	stroll_dlist_embed_after(&dst[0], &src[0], &src[1]);
	cute_check_ptr(stroll_dlist_next(&dst[0]), equal, &src[0]);
	cute_check_ptr(stroll_dlist_prev(&src[0]), equal, &dst[0]);
	cute_check_ptr(stroll_dlist_next(&src[0]), equal, &src[1]);
	cute_check_ptr(stroll_dlist_prev(&src[1]), equal, &src[0]);
	cute_check_ptr(stroll_dlist_next(&src[1]), equal, &dst[1]);
	cute_check_ptr(stroll_dlist_prev(&dst[1]), equal, &src[1]);
}

CUTE_TEST(strollut_dlist_embed_after_trail)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[2];
	struct stroll_dlist_node node;
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_embed_after(&nodes[1], &node, &node);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &node);
	cute_check_ptr(stroll_dlist_prev(&node), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_next(&node), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &node);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_embed_assert)
{
	struct stroll_dlist_node node;
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_embed_after(NULL, &node, &node));
	cute_expect_assertion(stroll_dlist_embed_after(&list, NULL, &node));
	cute_expect_assertion(stroll_dlist_embed_after(&list, &node, NULL));
	cute_expect_assertion(stroll_dlist_embed_after(&node, &node, &node));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_embed_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/******************************************************************************
 * Top-level suite
 ******************************************************************************/

CUTE_GROUP(strollut_dlist_group) = {
	CUTE_REF(strollut_dlist_empty),
	CUTE_REF(strollut_dlist_empty_assert),
	CUTE_REF(strollut_dlist_init),
	CUTE_REF(strollut_dlist_iterate_empty),
	CUTE_REF(strollut_dlist_single),
	CUTE_REF(strollut_dlist_single_assert),
	CUTE_REF(strollut_dlist_second),
	CUTE_REF(strollut_dlist_insert),
	CUTE_REF(strollut_dlist_insert_assert),
	CUTE_REF(strollut_dlist_append),
	CUTE_REF(strollut_dlist_append_assert),
	CUTE_REF(strollut_dlist_nqueue_front),
	CUTE_REF(strollut_dlist_nqueue_front_assert),
	CUTE_REF(strollut_dlist_nqueue_back),
	CUTE_REF(strollut_dlist_nqueue_back_assert),
	CUTE_REF(strollut_dlist_remove),
	CUTE_REF(strollut_dlist_remove_assert),
	CUTE_REF(strollut_dlist_remove_init),
	CUTE_REF(strollut_dlist_remove_init_assert),
	CUTE_REF(strollut_dlist_dqueue_front),
	CUTE_REF(strollut_dlist_dqueue_front_assert),
	CUTE_REF(strollut_dlist_dqueue_front_init),
	CUTE_REF(strollut_dlist_dqueue_front_init_assert),
	CUTE_REF(strollut_dlist_dqueue_back),
	CUTE_REF(strollut_dlist_dqueue_back_assert),
	CUTE_REF(strollut_dlist_dqueue_back_init),
	CUTE_REF(strollut_dlist_dqueue_back_init_assert),
	CUTE_REF(strollut_dlist_replace),
	CUTE_REF(strollut_dlist_replace_assert),
	CUTE_REF(strollut_dlist_move_before),
	CUTE_REF(strollut_dlist_move_before_assert),
	CUTE_REF(strollut_dlist_move_after),
	CUTE_REF(strollut_dlist_move_after_assert),
	CUTE_REF(strollut_dlist_withdraw_lead),
	CUTE_REF(strollut_dlist_withdraw_mid),
	CUTE_REF(strollut_dlist_withdraw_trail),
	CUTE_REF(strollut_dlist_withdraw_range),
	CUTE_REF(strollut_dlist_withdraw_assert),
	CUTE_REF(strollut_dlist_embed_after_lead),
	CUTE_REF(strollut_dlist_embed_after_mid),
	CUTE_REF(strollut_dlist_embed_after_trail),
#if 0
	CUTE_REF(strollut_dlist_embed_assert),
	CUTE_REF(strollut_dlist_embed_lead),
	CUTE_REF(strollut_dlist_embed_trail),
	CUTE_REF(strollut_dlist_embed_mid),
	CUTE_REF(strollut_dlist_splice_assert),
	CUTE_REF(strollut_dlist_splice_empty),
	CUTE_REF(strollut_dlist_splice_lead),
	CUTE_REF(strollut_dlist_splice_mid),
	CUTE_REF(strollut_dlist_splice_trail),

	CUTE_REF(strollut_dlist_bubble_suite),
	CUTE_REF(strollut_dlist_select_suite),
	CUTE_REF(strollut_dlist_insert_suite),
	CUTE_REF(strollut_dlist_merge_suite)
#endif
};

CUTE_SUITE_EXTERN(strollut_dlist_suite,
                  strollut_dlist_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
