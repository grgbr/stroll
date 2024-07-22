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
	struct stroll_dlist_node   node = STROLL_DLIST_INIT(node);
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

CUTE_TEST(strollut_dlist_embed_after_assert)
{
	struct stroll_dlist_node   node;
	struct stroll_dlist_node * ptr = &node;
	struct stroll_dlist_node * ptr2 = &node;
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_embed_after(NULL, ptr, &node));
	cute_expect_assertion(stroll_dlist_embed_after(&list, NULL, &node));
	cute_expect_assertion(stroll_dlist_embed_after(&list, &node, NULL));
	cute_expect_assertion(stroll_dlist_embed_after(&node, ptr, ptr2));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_embed_after_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_embed_before_lead)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[2];
	struct stroll_dlist_node node;
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_embed_before(&list, &node, &node);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &node);
	cute_check_ptr(stroll_dlist_prev(&node), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_next(&node), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &node);
}

CUTE_TEST(strollut_dlist_embed_before_mid)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node dst[4];
	struct stroll_dlist_node src[2];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(dst); c++)
		stroll_dlist_nqueue_back(&list, &dst[c]);
	for (c = 0; c < stroll_array_nr(src); c++)
		stroll_dlist_nqueue_back(&list, &src[c]);

	stroll_dlist_embed_before(&dst[1], &src[0], &src[1]);
	cute_check_ptr(stroll_dlist_next(&dst[0]), equal, &src[0]);
	cute_check_ptr(stroll_dlist_prev(&src[0]), equal, &dst[0]);
	cute_check_ptr(stroll_dlist_next(&src[0]), equal, &src[1]);
	cute_check_ptr(stroll_dlist_prev(&src[1]), equal, &src[0]);
	cute_check_ptr(stroll_dlist_next(&src[1]), equal, &dst[1]);
	cute_check_ptr(stroll_dlist_prev(&dst[1]), equal, &src[1]);
}

CUTE_TEST(strollut_dlist_embed_before_trail)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node nodes[2];
	struct stroll_dlist_node node;
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&list, &nodes[c]);

	stroll_dlist_embed_before(&list, &node, &node);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &node);
	cute_check_ptr(stroll_dlist_prev(&node), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_next(&node), equal, &list);
	cute_check_ptr(stroll_dlist_prev(&list), equal, &node);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_embed_before_assert)
{
	struct stroll_dlist_node   node;
	struct stroll_dlist_node * ptr = &node;
	struct stroll_dlist_node * ptr2 = &node;
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);

	cute_expect_assertion(stroll_dlist_embed_before(NULL, ptr, &node));
	cute_expect_assertion(stroll_dlist_embed_before(&list, NULL, &node));
	cute_expect_assertion(stroll_dlist_embed_before(&list, &node, NULL));
	cute_expect_assertion(stroll_dlist_embed_before(&node, ptr, ptr2));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_embed_before_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_splice_before_lead)
{
	struct stroll_dlist_node src = STROLL_DLIST_INIT(src);
	struct stroll_dlist_node nodes[2];
	struct stroll_dlist_node dst = STROLL_DLIST_INIT(dst);
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&src, &nodes[c]);

	stroll_dlist_splice_before(&dst, &nodes[0], &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&dst), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &dst);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &dst);
	cute_check_ptr(stroll_dlist_prev(&dst), equal, &nodes[0]);

	cute_check_ptr(stroll_dlist_next(&src), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &src);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &src);
	cute_check_ptr(stroll_dlist_prev(&src), equal, &nodes[1]);
}

CUTE_TEST(strollut_dlist_splice_before_mid)
{
	struct stroll_dlist_node src = STROLL_DLIST_INIT(src);
	struct stroll_dlist_node src_nodes[4];
	struct stroll_dlist_node dst = STROLL_DLIST_INIT(dst);
	struct stroll_dlist_node dst_nodes[2];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(src_nodes); c++)
		stroll_dlist_nqueue_back(&src, &src_nodes[c]);
	for (c = 0; c < stroll_array_nr(dst_nodes); c++)
		stroll_dlist_nqueue_back(&dst, &dst_nodes[c]);

	stroll_dlist_splice_before(&dst_nodes[1], &src_nodes[1], &src_nodes[2]);
	cute_check_ptr(stroll_dlist_next(&dst_nodes[0]), equal, &src_nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&src_nodes[1]), equal, &dst_nodes[0]);
	cute_check_ptr(stroll_dlist_next(&src_nodes[1]), equal, &src_nodes[2]);
	cute_check_ptr(stroll_dlist_prev(&src_nodes[2]), equal, &src_nodes[1]);
	cute_check_ptr(stroll_dlist_next(&src_nodes[2]), equal, &dst_nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&dst_nodes[1]), equal, &src_nodes[2]);

	cute_check_ptr(stroll_dlist_next(&src_nodes[0]), equal, &src_nodes[3]);
	cute_check_ptr(stroll_dlist_prev(&src_nodes[3]), equal, &src_nodes[0]);
}

CUTE_TEST(strollut_dlist_splice_before_trail)
{
	struct stroll_dlist_node src = STROLL_DLIST_INIT(src);
	struct stroll_dlist_node nodes[2];
	struct stroll_dlist_node dst = STROLL_DLIST_INIT(dst);
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&src, &nodes[c]);

	stroll_dlist_splice_before(&dst, &nodes[1], &nodes[1]);
	cute_check_ptr(stroll_dlist_next(&dst), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &dst);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &dst);
	cute_check_ptr(stroll_dlist_prev(&dst), equal, &nodes[1]);

	cute_check_ptr(stroll_dlist_next(&src), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &src);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &src);
	cute_check_ptr(stroll_dlist_prev(&src), equal, &nodes[0]);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_splice_before_assert)
{
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node * ptr = &list;
	struct stroll_dlist_node   node;

	cute_expect_assertion(stroll_dlist_splice_before(NULL, &node, &node));
	cute_expect_assertion(stroll_dlist_splice_before(&list, NULL, &node));
	cute_expect_assertion(stroll_dlist_splice_before(&list, &node, NULL));
	cute_expect_assertion(stroll_dlist_splice_before(&list, ptr, &node));
	cute_expect_assertion(stroll_dlist_splice_before(&list, &node, ptr));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_splice_before_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_dlist_splice_after_lead)
{
	struct stroll_dlist_node src = STROLL_DLIST_INIT(src);
	struct stroll_dlist_node nodes[2];
	struct stroll_dlist_node dst = STROLL_DLIST_INIT(dst);
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&src, &nodes[c]);

	stroll_dlist_splice_after(&dst, &nodes[0], &nodes[0]);
	cute_check_ptr(stroll_dlist_next(&dst), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &dst);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &dst);
	cute_check_ptr(stroll_dlist_prev(&dst), equal, &nodes[0]);

	cute_check_ptr(stroll_dlist_next(&src), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &src);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &src);
	cute_check_ptr(stroll_dlist_prev(&src), equal, &nodes[1]);
}

CUTE_TEST(strollut_dlist_splice_after_mid)
{
	struct stroll_dlist_node src = STROLL_DLIST_INIT(src);
	struct stroll_dlist_node src_nodes[4];
	struct stroll_dlist_node dst = STROLL_DLIST_INIT(dst);
	struct stroll_dlist_node dst_nodes[2];
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(src_nodes); c++)
		stroll_dlist_nqueue_back(&src, &src_nodes[c]);
	for (c = 0; c < stroll_array_nr(dst_nodes); c++)
		stroll_dlist_nqueue_back(&dst, &dst_nodes[c]);

	stroll_dlist_splice_after(&dst_nodes[0], &src_nodes[1], &src_nodes[2]);
	cute_check_ptr(stroll_dlist_next(&dst_nodes[0]), equal, &src_nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&src_nodes[1]), equal, &dst_nodes[0]);
	cute_check_ptr(stroll_dlist_next(&src_nodes[1]), equal, &src_nodes[2]);
	cute_check_ptr(stroll_dlist_prev(&src_nodes[2]), equal, &src_nodes[1]);
	cute_check_ptr(stroll_dlist_next(&src_nodes[2]), equal, &dst_nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&dst_nodes[1]), equal, &src_nodes[2]);

	cute_check_ptr(stroll_dlist_next(&src_nodes[0]), equal, &src_nodes[3]);
	cute_check_ptr(stroll_dlist_prev(&src_nodes[3]), equal, &src_nodes[0]);
}

CUTE_TEST(strollut_dlist_splice_after_trail)
{
	struct stroll_dlist_node src = STROLL_DLIST_INIT(src);
	struct stroll_dlist_node nodes[2];
	struct stroll_dlist_node dst = STROLL_DLIST_INIT(dst);
	unsigned int             c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_dlist_nqueue_back(&src, &nodes[c]);

	stroll_dlist_splice_after(&dst, &nodes[1], &nodes[1]);
	cute_check_ptr(stroll_dlist_next(&dst), equal, &nodes[1]);
	cute_check_ptr(stroll_dlist_prev(&nodes[1]), equal, &dst);
	cute_check_ptr(stroll_dlist_next(&nodes[1]), equal, &dst);
	cute_check_ptr(stroll_dlist_prev(&dst), equal, &nodes[1]);

	cute_check_ptr(stroll_dlist_next(&src), equal, &nodes[0]);
	cute_check_ptr(stroll_dlist_prev(&nodes[0]), equal, &src);
	cute_check_ptr(stroll_dlist_next(&nodes[0]), equal, &src);
	cute_check_ptr(stroll_dlist_prev(&src), equal, &nodes[0]);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_dlist_splice_after_assert)
{
	struct stroll_dlist_node   list = STROLL_DLIST_INIT(list);
	struct stroll_dlist_node * ptr = &list;
	struct stroll_dlist_node   node;

	cute_expect_assertion(stroll_dlist_splice_after(NULL, &node, &node));
	cute_expect_assertion(stroll_dlist_splice_after(&list, NULL, &node));
	cute_expect_assertion(stroll_dlist_splice_after(&list, &node, NULL));
	cute_expect_assertion(stroll_dlist_splice_after(&list, ptr, &node));
	cute_expect_assertion(stroll_dlist_splice_after(&list, &node, ptr));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_DLIST_NOASSERT(strollut_dlist_splice_after_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/******************************************************************************
 * Sorting tests
 ******************************************************************************/

struct strollut_dlist_node {
	struct stroll_dlist_node super;
	unsigned int             id;
	const char *             str;
};

#define STROLLUT_DLIST_NODE_INIT(_node, _id) \
	{ \
		.super = STROLL_DLIST_INIT(_node.super), \
		.id    = _id, \
		.str   = # _id \
	}

struct strollut_list_xpct {
	unsigned int id;
	const char * str;
	const void * ptr;
};

#define STROLLUT_LIST_XPCT_INIT(_id, _ptr) \
	{ \
		.id  = _id, \
		.str = # _id, \
		.ptr = _ptr \
	}

static void (*strollut_dlist_sort)
            (struct stroll_dlist_node * __restrict list,
             stroll_dlist_cmp_fn *                 compare);

#define STROLLUT_DLIST_SORT_SETUP(_setup, _sort) \
	static void _setup(void) \
	{ \
		strollut_dlist_sort = _sort; \
	}

#define STROLLUT_DLIST_UNSUP(_setup) \
	static void _setup(void) { cute_skip("support not compiled-in"); }

static inline int
strollut_dlist_compare_min_num(
	const struct stroll_dlist_node * __restrict a,
	const struct stroll_dlist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_dlist_node * _a =
		stroll_dlist_entry(a, const struct strollut_dlist_node, super);
	const struct strollut_dlist_node * _b =
		stroll_dlist_entry(b, const struct strollut_dlist_node, super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static void
strollut_dlist_sort_check_num(struct strollut_dlist_node      nodes[],
                              const struct strollut_list_xpct expected[],
                              unsigned int                    nr)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	unsigned int             n;

	for (n = 0; n < nr; n++)
		stroll_dlist_nqueue_back(&list, &nodes[n].super);

	strollut_dlist_sort(&list, strollut_dlist_compare_min_num);

	for (n = 0; n < nr; n++) {
		const struct strollut_list_xpct *  xpct = &expected[n];
		struct strollut_dlist_node *       node;

		cute_check_bool(stroll_dlist_empty(&list), is, false);

		node = stroll_dlist_entry(stroll_dlist_dqueue_front(&list),
		                          struct strollut_dlist_node,
		                          super);

		cute_check_uint(node->id, equal, xpct->id);
		cute_check_str(node->str, equal, xpct->str);
		cute_check_ptr(node, equal, xpct->ptr);
	}

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

CUTE_TEST(strollut_dlist_sort_num1)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0], 0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0])
	};

	strollut_dlist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_inorder_num2)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0], 0),
		STROLLUT_DLIST_NODE_INIT(nodes[1], 1)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(1, &nodes[1])
	};

	strollut_dlist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_revorder_num2)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0], 1),
		STROLLUT_DLIST_NODE_INIT(nodes[1], 0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(1, &nodes[0])
	};

	strollut_dlist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_duplicate_num2)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0], 0),
		STROLLUT_DLIST_NODE_INIT(nodes[1], 0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(0, &nodes[1])
	};

	strollut_dlist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_inorder_num17)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0],  0),
		STROLLUT_DLIST_NODE_INIT(nodes[1],  1),
		STROLLUT_DLIST_NODE_INIT(nodes[2],  2),
		STROLLUT_DLIST_NODE_INIT(nodes[3],  3),
		STROLLUT_DLIST_NODE_INIT(nodes[4],  4),
		STROLLUT_DLIST_NODE_INIT(nodes[5],  5),
		STROLLUT_DLIST_NODE_INIT(nodes[6],  6),
		STROLLUT_DLIST_NODE_INIT(nodes[7],  7),
		STROLLUT_DLIST_NODE_INIT(nodes[8],  8),
		STROLLUT_DLIST_NODE_INIT(nodes[9],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[10], 10),
		STROLLUT_DLIST_NODE_INIT(nodes[11], 11),
		STROLLUT_DLIST_NODE_INIT(nodes[12], 12),
		STROLLUT_DLIST_NODE_INIT(nodes[13], 13),
		STROLLUT_DLIST_NODE_INIT(nodes[14], 14),
		STROLLUT_DLIST_NODE_INIT(nodes[15], 15),
		STROLLUT_DLIST_NODE_INIT(nodes[16], 16)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(1,  &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(2,  &nodes[2]),
		STROLLUT_LIST_XPCT_INIT(3,  &nodes[3]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[4]),
		STROLLUT_LIST_XPCT_INIT(5,  &nodes[5]),
		STROLLUT_LIST_XPCT_INIT(6,  &nodes[6]),
		STROLLUT_LIST_XPCT_INIT(7,  &nodes[7]),
		STROLLUT_LIST_XPCT_INIT(8,  &nodes[8]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[9]),
		STROLLUT_LIST_XPCT_INIT(10, &nodes[10]),
		STROLLUT_LIST_XPCT_INIT(11, &nodes[11]),
		STROLLUT_LIST_XPCT_INIT(12, &nodes[12]),
		STROLLUT_LIST_XPCT_INIT(13, &nodes[13]),
		STROLLUT_LIST_XPCT_INIT(14, &nodes[14]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[15]),
		STROLLUT_LIST_XPCT_INIT(16, &nodes[16])
	};

	strollut_dlist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_revorder_num17)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0],  16),
		STROLLUT_DLIST_NODE_INIT(nodes[1],  15),
		STROLLUT_DLIST_NODE_INIT(nodes[2],  14),
		STROLLUT_DLIST_NODE_INIT(nodes[3],  13),
		STROLLUT_DLIST_NODE_INIT(nodes[4],  12),
		STROLLUT_DLIST_NODE_INIT(nodes[5],  11),
		STROLLUT_DLIST_NODE_INIT(nodes[6],  10),
		STROLLUT_DLIST_NODE_INIT(nodes[7],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[8],  8),
		STROLLUT_DLIST_NODE_INIT(nodes[9],  7),
		STROLLUT_DLIST_NODE_INIT(nodes[10], 6),
		STROLLUT_DLIST_NODE_INIT(nodes[11], 5),
		STROLLUT_DLIST_NODE_INIT(nodes[12], 4),
		STROLLUT_DLIST_NODE_INIT(nodes[13], 3),
		STROLLUT_DLIST_NODE_INIT(nodes[14], 2),
		STROLLUT_DLIST_NODE_INIT(nodes[15], 1),
		STROLLUT_DLIST_NODE_INIT(nodes[16], 0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[16]),
		STROLLUT_LIST_XPCT_INIT(1,  &nodes[15]),
		STROLLUT_LIST_XPCT_INIT(2,  &nodes[14]),
		STROLLUT_LIST_XPCT_INIT(3,  &nodes[13]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[12]),
		STROLLUT_LIST_XPCT_INIT(5,  &nodes[11]),
		STROLLUT_LIST_XPCT_INIT(6,  &nodes[10]),
		STROLLUT_LIST_XPCT_INIT(7,  &nodes[9]),
		STROLLUT_LIST_XPCT_INIT(8,  &nodes[8]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[7]),
		STROLLUT_LIST_XPCT_INIT(10, &nodes[6]),
		STROLLUT_LIST_XPCT_INIT(11, &nodes[5]),
		STROLLUT_LIST_XPCT_INIT(12, &nodes[4]),
		STROLLUT_LIST_XPCT_INIT(13, &nodes[3]),
		STROLLUT_LIST_XPCT_INIT(14, &nodes[2]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(16, &nodes[0])
	};

	strollut_dlist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_unorder_num17)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0],  11),
		STROLLUT_DLIST_NODE_INIT(nodes[1],  16),
		STROLLUT_DLIST_NODE_INIT(nodes[2],  1),
		STROLLUT_DLIST_NODE_INIT(nodes[3],  7),
		STROLLUT_DLIST_NODE_INIT(nodes[4],  14),
		STROLLUT_DLIST_NODE_INIT(nodes[5],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[6],  15),
		STROLLUT_DLIST_NODE_INIT(nodes[7],  8),
		STROLLUT_DLIST_NODE_INIT(nodes[8],  3),
		STROLLUT_DLIST_NODE_INIT(nodes[9],  6),
		STROLLUT_DLIST_NODE_INIT(nodes[10], 12),
		STROLLUT_DLIST_NODE_INIT(nodes[11], 2),
		STROLLUT_DLIST_NODE_INIT(nodes[12], 5),
		STROLLUT_DLIST_NODE_INIT(nodes[13], 10),
		STROLLUT_DLIST_NODE_INIT(nodes[14], 4),
		STROLLUT_DLIST_NODE_INIT(nodes[15], 0),
		STROLLUT_DLIST_NODE_INIT(nodes[16], 13)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[15]),
		STROLLUT_LIST_XPCT_INIT(1,  &nodes[2]),
		STROLLUT_LIST_XPCT_INIT(2,  &nodes[11]),
		STROLLUT_LIST_XPCT_INIT(3,  &nodes[8]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[14]),
		STROLLUT_LIST_XPCT_INIT(5,  &nodes[12]),
		STROLLUT_LIST_XPCT_INIT(6,  &nodes[9]),
		STROLLUT_LIST_XPCT_INIT(7,  &nodes[3]),
		STROLLUT_LIST_XPCT_INIT(8,  &nodes[7]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[5]),
		STROLLUT_LIST_XPCT_INIT(10, &nodes[13]),
		STROLLUT_LIST_XPCT_INIT(11, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(12, &nodes[10]),
		STROLLUT_LIST_XPCT_INIT(13, &nodes[16]),
		STROLLUT_LIST_XPCT_INIT(14, &nodes[4]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[6]),
		STROLLUT_LIST_XPCT_INIT(16, &nodes[1])
	};

	strollut_dlist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_duplicate_num17)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[1],  15),
		STROLLUT_DLIST_NODE_INIT(nodes[2],  0),
		STROLLUT_DLIST_NODE_INIT(nodes[3],  7),
		STROLLUT_DLIST_NODE_INIT(nodes[4],  14),
		STROLLUT_DLIST_NODE_INIT(nodes[5],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[6],  15),
		STROLLUT_DLIST_NODE_INIT(nodes[7],  8),
		STROLLUT_DLIST_NODE_INIT(nodes[8],  3),
		STROLLUT_DLIST_NODE_INIT(nodes[9],  6),
		STROLLUT_DLIST_NODE_INIT(nodes[10], 12),
		STROLLUT_DLIST_NODE_INIT(nodes[11], 2),
		STROLLUT_DLIST_NODE_INIT(nodes[12], 4),
		STROLLUT_DLIST_NODE_INIT(nodes[13], 9),
		STROLLUT_DLIST_NODE_INIT(nodes[14], 4),
		STROLLUT_DLIST_NODE_INIT(nodes[15], 0),
		STROLLUT_DLIST_NODE_INIT(nodes[16], 13)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[2]),
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[15]),
		STROLLUT_LIST_XPCT_INIT(2,  &nodes[11]),
		STROLLUT_LIST_XPCT_INIT(3,  &nodes[8]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[12]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[14]),
		STROLLUT_LIST_XPCT_INIT(6,  &nodes[9]),
		STROLLUT_LIST_XPCT_INIT(7,  &nodes[3]),
		STROLLUT_LIST_XPCT_INIT(8,  &nodes[7]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[5]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[13]),
		STROLLUT_LIST_XPCT_INIT(12, &nodes[10]),
		STROLLUT_LIST_XPCT_INIT(13, &nodes[16]),
		STROLLUT_LIST_XPCT_INIT(14, &nodes[4]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[6])
	};

	strollut_dlist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

static inline int
strollut_dlist_compare_min_str(
	const struct stroll_dlist_node * __restrict a,
	const struct stroll_dlist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_dlist_node * _a =
		stroll_dlist_entry(a, const struct strollut_dlist_node, super);
	const struct strollut_dlist_node * _b =
		stroll_dlist_entry(b, const struct strollut_dlist_node, super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static void
strollut_dlist_sort_check_str(struct strollut_dlist_node      nodes[],
	                      const struct strollut_list_xpct expected[],
	                      unsigned int                    nr)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	unsigned int             n;

	for (n = 0; n < nr; n++)
		stroll_dlist_nqueue_back(&list, &nodes[n].super);

	strollut_dlist_sort(&list, strollut_dlist_compare_min_str);

	for (n = 0; n < nr; n++) {
		const struct strollut_list_xpct *  xpct = &expected[n];
		struct strollut_dlist_node *       node;

		cute_check_bool(stroll_dlist_empty(&list), is, false);

		node = stroll_dlist_entry(stroll_dlist_dqueue_front(&list),
		                          struct strollut_dlist_node,
		                          super);

		cute_check_uint(node->id, equal, xpct->id);
		cute_check_str(node->str, equal, xpct->str);
		cute_check_ptr(node, equal, xpct->ptr);
	}

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

CUTE_TEST(strollut_dlist_sort_str1)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0], 0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0])
	};

	strollut_dlist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_inorder_str2)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0], 0),
		STROLLUT_DLIST_NODE_INIT(nodes[1], 1)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(1, &nodes[1])
	};

	strollut_dlist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_revorder_str2)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0], 1),
		STROLLUT_DLIST_NODE_INIT(nodes[1], 0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(1, &nodes[0])
	};

	strollut_dlist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_duplicate_str2)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0], 0),
		STROLLUT_DLIST_NODE_INIT(nodes[1], 0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(0, &nodes[1])
	};

	strollut_dlist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_inorder_str17)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0],  0),
		STROLLUT_DLIST_NODE_INIT(nodes[1],  1),
		STROLLUT_DLIST_NODE_INIT(nodes[2],  2),
		STROLLUT_DLIST_NODE_INIT(nodes[3],  3),
		STROLLUT_DLIST_NODE_INIT(nodes[4],  4),
		STROLLUT_DLIST_NODE_INIT(nodes[5],  5),
		STROLLUT_DLIST_NODE_INIT(nodes[6],  6),
		STROLLUT_DLIST_NODE_INIT(nodes[7],  7),
		STROLLUT_DLIST_NODE_INIT(nodes[8],  8),
		STROLLUT_DLIST_NODE_INIT(nodes[9],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[10], 10),
		STROLLUT_DLIST_NODE_INIT(nodes[11], 11),
		STROLLUT_DLIST_NODE_INIT(nodes[12], 12),
		STROLLUT_DLIST_NODE_INIT(nodes[13], 13),
		STROLLUT_DLIST_NODE_INIT(nodes[14], 14),
		STROLLUT_DLIST_NODE_INIT(nodes[15], 15),
		STROLLUT_DLIST_NODE_INIT(nodes[16], 16)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(1,  &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(2,  &nodes[2]),
		STROLLUT_LIST_XPCT_INIT(3,  &nodes[3]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[4]),
		STROLLUT_LIST_XPCT_INIT(5,  &nodes[5]),
		STROLLUT_LIST_XPCT_INIT(6,  &nodes[6]),
		STROLLUT_LIST_XPCT_INIT(7,  &nodes[7]),
		STROLLUT_LIST_XPCT_INIT(8,  &nodes[8]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[9]),
		STROLLUT_LIST_XPCT_INIT(10, &nodes[10]),
		STROLLUT_LIST_XPCT_INIT(11, &nodes[11]),
		STROLLUT_LIST_XPCT_INIT(12, &nodes[12]),
		STROLLUT_LIST_XPCT_INIT(13, &nodes[13]),
		STROLLUT_LIST_XPCT_INIT(14, &nodes[14]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[15]),
		STROLLUT_LIST_XPCT_INIT(16, &nodes[16])
	};

	strollut_dlist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_revorder_str17)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0],  16),
		STROLLUT_DLIST_NODE_INIT(nodes[1],  15),
		STROLLUT_DLIST_NODE_INIT(nodes[2],  14),
		STROLLUT_DLIST_NODE_INIT(nodes[3],  13),
		STROLLUT_DLIST_NODE_INIT(nodes[4],  12),
		STROLLUT_DLIST_NODE_INIT(nodes[5],  11),
		STROLLUT_DLIST_NODE_INIT(nodes[6],  10),
		STROLLUT_DLIST_NODE_INIT(nodes[7],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[8],  8),
		STROLLUT_DLIST_NODE_INIT(nodes[9],  7),
		STROLLUT_DLIST_NODE_INIT(nodes[10], 6),
		STROLLUT_DLIST_NODE_INIT(nodes[11], 5),
		STROLLUT_DLIST_NODE_INIT(nodes[12], 4),
		STROLLUT_DLIST_NODE_INIT(nodes[13], 3),
		STROLLUT_DLIST_NODE_INIT(nodes[14], 2),
		STROLLUT_DLIST_NODE_INIT(nodes[15], 1),
		STROLLUT_DLIST_NODE_INIT(nodes[16], 0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[16]),
		STROLLUT_LIST_XPCT_INIT(1,  &nodes[15]),
		STROLLUT_LIST_XPCT_INIT(2,  &nodes[14]),
		STROLLUT_LIST_XPCT_INIT(3,  &nodes[13]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[12]),
		STROLLUT_LIST_XPCT_INIT(5,  &nodes[11]),
		STROLLUT_LIST_XPCT_INIT(6,  &nodes[10]),
		STROLLUT_LIST_XPCT_INIT(7,  &nodes[9]),
		STROLLUT_LIST_XPCT_INIT(8,  &nodes[8]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[7]),
		STROLLUT_LIST_XPCT_INIT(10, &nodes[6]),
		STROLLUT_LIST_XPCT_INIT(11, &nodes[5]),
		STROLLUT_LIST_XPCT_INIT(12, &nodes[4]),
		STROLLUT_LIST_XPCT_INIT(13, &nodes[3]),
		STROLLUT_LIST_XPCT_INIT(14, &nodes[2]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(16, &nodes[0])
	};

	strollut_dlist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_unorder_str17)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0],  11),
		STROLLUT_DLIST_NODE_INIT(nodes[1],  16),
		STROLLUT_DLIST_NODE_INIT(nodes[2],  1),
		STROLLUT_DLIST_NODE_INIT(nodes[3],  7),
		STROLLUT_DLIST_NODE_INIT(nodes[4],  14),
		STROLLUT_DLIST_NODE_INIT(nodes[5],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[6],  15),
		STROLLUT_DLIST_NODE_INIT(nodes[7],  8),
		STROLLUT_DLIST_NODE_INIT(nodes[8],  3),
		STROLLUT_DLIST_NODE_INIT(nodes[9],  6),
		STROLLUT_DLIST_NODE_INIT(nodes[10], 12),
		STROLLUT_DLIST_NODE_INIT(nodes[11], 2),
		STROLLUT_DLIST_NODE_INIT(nodes[12], 5),
		STROLLUT_DLIST_NODE_INIT(nodes[13], 10),
		STROLLUT_DLIST_NODE_INIT(nodes[14], 4),
		STROLLUT_DLIST_NODE_INIT(nodes[15], 0),
		STROLLUT_DLIST_NODE_INIT(nodes[16], 13)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[15]),
		STROLLUT_LIST_XPCT_INIT(1,  &nodes[2]),
		STROLLUT_LIST_XPCT_INIT(2,  &nodes[11]),
		STROLLUT_LIST_XPCT_INIT(3,  &nodes[8]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[14]),
		STROLLUT_LIST_XPCT_INIT(5,  &nodes[12]),
		STROLLUT_LIST_XPCT_INIT(6,  &nodes[9]),
		STROLLUT_LIST_XPCT_INIT(7,  &nodes[3]),
		STROLLUT_LIST_XPCT_INIT(8,  &nodes[7]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[5]),
		STROLLUT_LIST_XPCT_INIT(10, &nodes[13]),
		STROLLUT_LIST_XPCT_INIT(11, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(12, &nodes[10]),
		STROLLUT_LIST_XPCT_INIT(13, &nodes[16]),
		STROLLUT_LIST_XPCT_INIT(14, &nodes[4]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[6]),
		STROLLUT_LIST_XPCT_INIT(16, &nodes[1])
	};

	strollut_dlist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_dlist_sort_duplicate_str17)
{
	struct strollut_dlist_node      nodes[] = {
		STROLLUT_DLIST_NODE_INIT(nodes[0],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[1],  15),
		STROLLUT_DLIST_NODE_INIT(nodes[2],  0),
		STROLLUT_DLIST_NODE_INIT(nodes[3],  7),
		STROLLUT_DLIST_NODE_INIT(nodes[4],  14),
		STROLLUT_DLIST_NODE_INIT(nodes[5],  9),
		STROLLUT_DLIST_NODE_INIT(nodes[6],  15),
		STROLLUT_DLIST_NODE_INIT(nodes[7],  8),
		STROLLUT_DLIST_NODE_INIT(nodes[8],  3),
		STROLLUT_DLIST_NODE_INIT(nodes[9],  6),
		STROLLUT_DLIST_NODE_INIT(nodes[10], 12),
		STROLLUT_DLIST_NODE_INIT(nodes[11], 2),
		STROLLUT_DLIST_NODE_INIT(nodes[12], 4),
		STROLLUT_DLIST_NODE_INIT(nodes[13], 9),
		STROLLUT_DLIST_NODE_INIT(nodes[14], 4),
		STROLLUT_DLIST_NODE_INIT(nodes[15], 0),
		STROLLUT_DLIST_NODE_INIT(nodes[16], 13)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[2]),
		STROLLUT_LIST_XPCT_INIT(0,  &nodes[15]),
		STROLLUT_LIST_XPCT_INIT(2,  &nodes[11]),
		STROLLUT_LIST_XPCT_INIT(3,  &nodes[8]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[12]),
		STROLLUT_LIST_XPCT_INIT(4,  &nodes[14]),
		STROLLUT_LIST_XPCT_INIT(6,  &nodes[9]),
		STROLLUT_LIST_XPCT_INIT(7,  &nodes[3]),
		STROLLUT_LIST_XPCT_INIT(8,  &nodes[7]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[5]),
		STROLLUT_LIST_XPCT_INIT(9,  &nodes[13]),
		STROLLUT_LIST_XPCT_INIT(12, &nodes[10]),
		STROLLUT_LIST_XPCT_INIT(13, &nodes[16]),
		STROLLUT_LIST_XPCT_INIT(14, &nodes[4]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(15, &nodes[6])
	};

	strollut_dlist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

#include "array_data.h"

struct strollut_dlist_array_node {
	struct stroll_dlist_node                  super;
	union {
		const struct strollut_array_num * num;
		const struct strollut_array_str * str;
	};
};

static struct strollut_dlist_array_node
strollut_list_tosort_num[STROLLUT_ARRAY_NUM_NR];

static inline int
strollut_dlist_compare_elem_min_num(
	const struct stroll_dlist_node * __restrict a,
	const struct stroll_dlist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_dlist_array_node * _a =
		stroll_dlist_entry(a,
		                   const struct strollut_dlist_array_node,
		                   super);
	const struct strollut_dlist_array_node * _b =
		stroll_dlist_entry(b,
		                   const struct strollut_dlist_array_node,
		                   super);

	return (_a->num->key > _b->num->key) - (_a->num->key < _b->num->key);
}

static inline int
strollut_dlist_compare_elem_max_num(
	const struct stroll_dlist_node * __restrict a,
	const struct stroll_dlist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_dlist_array_node * _a =
		stroll_dlist_entry(a,
		                   const struct strollut_dlist_array_node,
		                   super);
	const struct strollut_dlist_array_node * _b =
		stroll_dlist_entry(b,
		                   const struct strollut_dlist_array_node,
		                   super);

	return (_b->num->key > _a->num->key) - (_b->num->key < _a->num->key);
}

static void
strollut_dlist_sort_check_large_num(const struct strollut_array_num * array,
                                    const struct strollut_array_num * expected,
                                    stroll_dlist_cmp_fn *             cmp)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	unsigned int             e;

	for (e = 0; e < stroll_array_nr(strollut_list_tosort_num); e++) {
		strollut_list_tosort_num[e].num = &array[e];
		stroll_dlist_nqueue_back(&list,
		                         &strollut_list_tosort_num[e].super);
	}

	strollut_dlist_sort(&list, cmp);

	for (e = 0; e < stroll_array_nr(strollut_list_tosort_num); e++) {
		const struct strollut_array_num *  xpct = &expected[e];
		struct strollut_dlist_array_node * node;

		cute_check_bool(stroll_dlist_empty(&list), is, false);

		node = stroll_dlist_entry(stroll_dlist_dqueue_front(&list),
		                          struct strollut_dlist_array_node,
		                          super);

		cute_check_sint(node->num->key, equal, xpct->key);
		cute_check_ptr(node->num, equal, xpct->ptr);
	}

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

CUTE_TEST(strollut_dlist_sort_inorder_large_num)
{
	strollut_dlist_sort_check_large_num(
			strollut_array_num_input,
			strollut_array_num_inorder,
			strollut_dlist_compare_elem_min_num);
}

CUTE_TEST(strollut_dlist_sort_postorder_large_num)
{
	strollut_dlist_sort_check_large_num(
			strollut_array_num_input,
			strollut_array_num_postorder,
			strollut_dlist_compare_elem_max_num);
}

static struct strollut_dlist_array_node
strollut_list_tosort_str[STROLLUT_ARRAY_STR_NR];

static inline int
strollut_dlist_compare_elem_min_str(
	const struct stroll_dlist_node * __restrict a,
	const struct stroll_dlist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_dlist_array_node * _a =
		stroll_dlist_entry(a,
		                   const struct strollut_dlist_array_node,
		                   super);
	const struct strollut_dlist_array_node * _b =
		stroll_dlist_entry(b,
		                   const struct strollut_dlist_array_node,
		                   super);

	return strcmp(_a->str->word, _b->str->word);
}

static inline int
strollut_dlist_compare_elem_max_str(
	const struct stroll_dlist_node * __restrict a,
	const struct stroll_dlist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_dlist_array_node * _a =
		stroll_dlist_entry(a,
		                   const struct strollut_dlist_array_node,
		                   super);
	const struct strollut_dlist_array_node * _b =
		stroll_dlist_entry(b,
		                   const struct strollut_dlist_array_node,
		                   super);

	return strcmp(_b->str->word, _a->str->word);
}

static void
strollut_dlist_sort_check_large_str(const struct strollut_array_str * array,
                                    const struct strollut_array_str * expected,
                                    stroll_dlist_cmp_fn *             cmp)
{
	struct stroll_dlist_node list = STROLL_DLIST_INIT(list);
	unsigned int             e;

	for (e = 0; e < stroll_array_nr(strollut_list_tosort_str); e++) {
		strollut_list_tosort_str[e].str = &array[e];
		stroll_dlist_nqueue_back(&list,
		                         &strollut_list_tosort_str[e].super);
	}

	strollut_dlist_sort(&list, cmp);

	for (e = 0; e < stroll_array_nr(strollut_list_tosort_str); e++) {
		const struct strollut_array_str *  xpct = &expected[e];
		struct strollut_dlist_array_node * node;

		cute_check_bool(stroll_dlist_empty(&list), is, false);

		node = stroll_dlist_entry(stroll_dlist_dqueue_front(&list),
		                          struct strollut_dlist_array_node,
		                          super);

		cute_check_str(node->str->word, equal, xpct->word);
		cute_check_ptr(node->str, equal, xpct->ptr);
	}

	cute_check_bool(stroll_dlist_empty(&list), is, true);
}

CUTE_TEST(strollut_dlist_sort_inorder_large_str)
{
	strollut_dlist_sort_check_large_str(
			strollut_array_str_input,
			strollut_array_str_inorder,
			strollut_dlist_compare_elem_min_str);
}

CUTE_TEST(strollut_dlist_sort_postorder_large_str)
{
	strollut_dlist_sort_check_large_str(
			strollut_array_str_input,
			strollut_array_str_postorder,
			strollut_dlist_compare_elem_max_str);
}

CUTE_GROUP(strollut_dlist_sort_group) = {
	CUTE_REF(strollut_dlist_sort_num1),
	CUTE_REF(strollut_dlist_sort_inorder_num2),
	CUTE_REF(strollut_dlist_sort_revorder_num2),
	CUTE_REF(strollut_dlist_sort_duplicate_num2),
	CUTE_REF(strollut_dlist_sort_inorder_num17),
	CUTE_REF(strollut_dlist_sort_revorder_num17),
	CUTE_REF(strollut_dlist_sort_unorder_num17),
	CUTE_REF(strollut_dlist_sort_duplicate_num17),

	CUTE_REF(strollut_dlist_sort_str1),
	CUTE_REF(strollut_dlist_sort_inorder_str2),
	CUTE_REF(strollut_dlist_sort_revorder_str2),
	CUTE_REF(strollut_dlist_sort_duplicate_str2),
	CUTE_REF(strollut_dlist_sort_inorder_str17),
	CUTE_REF(strollut_dlist_sort_revorder_str17),
	CUTE_REF(strollut_dlist_sort_unorder_str17),
	CUTE_REF(strollut_dlist_sort_duplicate_str17),

	CUTE_REF(strollut_dlist_sort_inorder_large_num),
	CUTE_REF(strollut_dlist_sort_postorder_large_num),

	CUTE_REF(strollut_dlist_sort_inorder_large_str),
	CUTE_REF(strollut_dlist_sort_postorder_large_str),
};

#if defined(CONFIG_STROLL_DLIST_BUBBLE_SORT)

static void
strollut_dlist_bubble_sort(struct stroll_dlist_node * __restrict list,
                           stroll_dlist_cmp_fn *                 compare)
{
	stroll_dlist_bubble_sort(list, compare, NULL);
}

STROLLUT_DLIST_SORT_SETUP(strollut_dlist_bubble_setup,
                          strollut_dlist_bubble_sort)

#else   /* !defined(CONFIG_STROLL_DLIST_BUBBLE_SORT) */
STROLLUT_DLIST_UNSUP(strollut_dlist_bubble_setup)
#endif  /* defined(CONFIG_STROLL_DLIST_BUBBLE_SORT) */

CUTE_SUITE_STATIC(strollut_dlist_bubble_suite,
                  strollut_dlist_sort_group,
                  strollut_dlist_bubble_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_DLIST_SELECT_SORT)

static void
strollut_dlist_select_sort(struct stroll_dlist_node * __restrict list,
                           stroll_dlist_cmp_fn *                 compare)
{
	stroll_dlist_select_sort(list, compare, NULL);
}

STROLLUT_DLIST_SORT_SETUP(strollut_dlist_select_setup,
                          strollut_dlist_select_sort)

#else   /* !defined(CONFIG_STROLL_DLIST_SELECT_SORT) */
STROLLUT_DLIST_UNSUP(strollut_dlist_select_setup)
#endif  /* defined(CONFIG_STROLL_DLIST_SELECT_SORT) */

CUTE_SUITE_STATIC(strollut_dlist_select_suite,
                  strollut_dlist_sort_group,
                  strollut_dlist_select_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

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
	CUTE_REF(strollut_dlist_embed_after_assert),
	CUTE_REF(strollut_dlist_embed_before_lead),
	CUTE_REF(strollut_dlist_embed_before_mid),
	CUTE_REF(strollut_dlist_embed_before_trail),
	CUTE_REF(strollut_dlist_embed_before_assert),
	CUTE_REF(strollut_dlist_splice_before_lead),
	CUTE_REF(strollut_dlist_splice_before_mid),
	CUTE_REF(strollut_dlist_splice_before_trail),
	CUTE_REF(strollut_dlist_splice_before_assert),
	CUTE_REF(strollut_dlist_splice_after_lead),
	CUTE_REF(strollut_dlist_splice_after_mid),
	CUTE_REF(strollut_dlist_splice_after_trail),
	CUTE_REF(strollut_dlist_splice_after_assert),

	CUTE_REF(strollut_dlist_bubble_suite),
	CUTE_REF(strollut_dlist_select_suite),
#if 0
	CUTE_REF(strollut_dlist_insert_suite),
	CUTE_REF(strollut_dlist_merge_suite)
#endif
};

CUTE_SUITE_EXTERN(strollut_dlist_suite,
                  strollut_dlist_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
