/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/slist.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>

#define STROLLUT_SLIST_NOASSERT(_test) \
	CUTE_TEST(_test) \
	{ \
		cute_skip("support not compiled-in"); \
	}

CUTE_TEST(strollut_slist_empty)
{
	struct stroll_slist lst = STROLL_SLIST_INIT(lst);

	cute_check_bool(stroll_slist_empty(&lst), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_empty_assert)
{
	struct stroll_slist lst = { 0, };

	cute_expect_assertion(stroll_slist_empty(NULL));
	cute_expect_assertion(stroll_slist_empty(&lst));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_empty_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_init)
{
	struct stroll_slist lst;

	stroll_slist_init(&lst);

	cute_check_bool(stroll_slist_empty(&lst), is, true);
}

CUTE_TEST(strollut_slist_iterate_empty)
{
	struct stroll_slist        lst = STROLL_SLIST_INIT(lst);
	struct stroll_slist_node * node;

	stroll_slist_foreach_node(&lst, node)
		cute_fail("iterable empty slist !");
}

CUTE_TEST(strollut_slist_single)
{
	struct stroll_slist_node   first = { .next = NULL };
	struct stroll_slist        list = {
		.head = { .next = &first },
		.tail = &first
	};
	struct stroll_slist_node * node;
	unsigned int               cnt;

	cute_check_ptr(stroll_slist_head(&list), equal, &list.head);
	cute_check_ptr(stroll_slist_first(&list), equal, &first);
	cute_check_ptr(stroll_slist_last(&list), equal, &first);
	cute_check_ptr(stroll_slist_next(stroll_slist_head(&list)),
	               equal,
	               &first);
	cute_check_ptr(stroll_slist_next(&first), equal, NULL);

	cnt = 0;
	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &first);
		cnt++;
	}
	cute_check_uint(cnt, equal, 1);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_single_assert)
{
	struct stroll_slist list = { .tail = NULL };

	cute_expect_assertion(stroll_slist_head(NULL));
	cute_expect_assertion(stroll_slist_head(&list));
	cute_expect_assertion(stroll_slist_first(NULL));
	cute_expect_assertion(stroll_slist_last(NULL));
	cute_expect_assertion(stroll_slist_next(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_single_assert);

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_second)
{
	struct stroll_slist_node   second = { .next = NULL };
	struct stroll_slist_node   first = { .next = &second };
	struct stroll_slist        list = {
		.head = { .next = &first },
		.tail = &second
	};
	struct stroll_slist_node * node;
	unsigned int               cnt;

	cute_check_ptr(stroll_slist_head(&list), equal, &list.head);
	cute_check_ptr(stroll_slist_first(&list), equal, &first);
	cute_check_ptr(stroll_slist_last(&list), equal, &second);
	cute_check_ptr(stroll_slist_next(stroll_slist_head(&list)),
	               equal,
	               &first);
	cute_check_ptr(stroll_slist_next(&first), equal, &second);
	cute_check_ptr(stroll_slist_next(&second), equal, NULL);

	cnt = 0;
	stroll_slist_foreach_node(&list, node)
		cnt++;
	cute_check_uint(cnt, equal, 2);
}

CUTE_TEST(strollut_slist_append)
{
	struct stroll_slist        list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node   nodes[8];
	struct stroll_slist_node * node;
	unsigned int               c;

	stroll_slist_append(&list, stroll_slist_head(&list), &nodes[0]);
	for (c = 1; c < stroll_array_nr(nodes); c++)
		stroll_slist_append(&list, &nodes[c - 1], &nodes[c]);

	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_slist_last(&list),
	               equal,
	               &nodes[stroll_array_nr(nodes) - 1]);

	c = 0;
	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}

	cute_check_uint(c, equal, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_append_assert)
{
	struct stroll_slist      list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node node;

	cute_expect_assertion(stroll_slist_append(NULL,
	                                          stroll_slist_head(&list),
	                                          &node));
	cute_expect_assertion(stroll_slist_append(&list,
	                                          NULL,
	                                          &node));
	cute_expect_assertion(stroll_slist_append(&list,
	                                          stroll_slist_head(&list),
	                                          NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_append_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_remove)
{
	struct stroll_slist_node   nodes[8] = {
		[0] = { .next = &nodes[1] },
		[1] = { .next = &nodes[2] },
		[2] = { .next = &nodes[3] },
		[3] = { .next = &nodes[4] },
		[4] = { .next = &nodes[5] },
		[5] = { .next = &nodes[6] },
		[6] = { .next = &nodes[7] },
		[7] = { .next = NULL }
	};
	struct stroll_slist        list = {
		.head.next = &nodes[0],
		.tail      = &nodes[7]
	};
	unsigned int               c;

	stroll_slist_remove(&list, stroll_slist_head(&list), &nodes[0]);
	for (c = 1; c < stroll_array_nr(nodes); c++) {
		cute_check_ptr(stroll_slist_first(&list), equal, &nodes[c]);
		cute_check_ptr(stroll_slist_last(&list), equal, &nodes[7]);
		stroll_slist_remove(&list, stroll_slist_head(&list), &nodes[c]);
	}

	cute_check_bool(stroll_slist_empty(&list), is, true);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_remove_assert)
{
	struct stroll_slist      list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node node;

	cute_expect_assertion(stroll_slist_remove(NULL,
	                                          stroll_slist_head(&list),
	                                          &node));
	cute_expect_assertion(stroll_slist_remove(&list,
	                                          NULL,
	                                          &node));
	cute_expect_assertion(stroll_slist_remove(&list,
	                                          stroll_slist_head(&list),
	                                          NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_remove_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_move)
{
	struct stroll_slist_node   nodes[8] = {
		[0] = { .next = &nodes[1] },
		[1] = { .next = &nodes[2] },
		[2] = { .next = &nodes[3] },
		[3] = { .next = &nodes[4] },
		[4] = { .next = &nodes[5] },
		[5] = { .next = &nodes[6] },
		[6] = { .next = &nodes[7] },
		[7] = { .next = NULL }
	};
	struct stroll_slist        list = {
		.head.next = &nodes[0],
		.tail      = &nodes[7]
	};
	unsigned int               c = 0;
	struct stroll_slist_node * node;

	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}

	stroll_slist_move(&list,
	                  stroll_slist_head(&list),
	                  &nodes[3],
	                  &nodes[4]);
	cute_check_ptr(stroll_slist_first(&list),
	               equal,
	               &nodes[4]);
	cute_check_ptr(stroll_slist_next(&nodes[3]), equal, &nodes[5]);

	stroll_slist_move(&list,
	                  &nodes[7],
	                  stroll_slist_head(&list),
	                  &nodes[4]);
	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_slist_next(&nodes[7]), equal, &nodes[4]);
	cute_check_ptr(stroll_slist_next(&nodes[4]), equal, NULL);
	cute_check_ptr(stroll_slist_last(&list), equal, &nodes[4]);

	stroll_slist_move(&list,
	                  &nodes[3],
	                  &nodes[7],
	                  &nodes[4]);
	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_slist_last(&list), equal, &nodes[7]);
	c = 0;
	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_move_assert)
{
	struct stroll_slist      list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node prev = { 0 };
	struct stroll_slist_node node = { 0 };

	cute_expect_assertion(stroll_slist_move(NULL,
	                                        stroll_slist_head(&list),
	                                        &prev,
	                                        &node));
	cute_expect_assertion(stroll_slist_move(&list,
	                                        NULL,
	                                        &prev,
	                                        &node));
	cute_expect_assertion(stroll_slist_move(&list,
	                                        stroll_slist_head(&list),
	                                        &prev,
	                                        NULL));
	cute_expect_assertion(stroll_slist_move(&list,
	                                        stroll_slist_head(&list),
	                                        &prev,
	                                        &node));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_move_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_nqueue_back)
{
	struct stroll_slist        list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node   nodes[8];
	struct stroll_slist_node * node;
	unsigned int               c;

	for (c = 0; c < stroll_array_nr(nodes); c++)
		stroll_slist_nqueue_back(&list, &nodes[c]);

	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_slist_last(&list),
	               equal,
	               &nodes[stroll_array_nr(nodes) - 1]);

	c = 0;
	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}

	cute_check_uint(c, equal, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_nqueue_back_assert)
{
	struct stroll_slist      list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node node;

	cute_expect_assertion(stroll_slist_nqueue_back(NULL, &node));
	cute_expect_assertion(stroll_slist_nqueue_back(&list, NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_nqueue_back_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_nqueue_front)
{
	struct stroll_slist        list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node   nodes[8];
	struct stroll_slist_node * node;
	unsigned int               c = stroll_array_nr(nodes);

	while (c--)
		stroll_slist_nqueue_front(&list, &nodes[c]);

	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_slist_last(&list),
	               equal,
	               &nodes[stroll_array_nr(nodes) - 1]);

	c = 0;
	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}

	cute_check_uint(c, equal, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_nqueue_front_assert)
{
	struct stroll_slist      list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node node;

	cute_expect_assertion(stroll_slist_nqueue_front(NULL, &node));
	cute_expect_assertion(stroll_slist_nqueue_front(&list, NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_nqueue_front_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_dqueue_front)
{
	struct stroll_slist_node   nodes[8] = {
		[0] = { .next = &nodes[1] },
		[1] = { .next = &nodes[2] },
		[2] = { .next = &nodes[3] },
		[3] = { .next = &nodes[4] },
		[4] = { .next = &nodes[5] },
		[5] = { .next = &nodes[6] },
		[6] = { .next = &nodes[7] },
		[7] = { .next = NULL }
	};
	struct stroll_slist        list = {
		.head.next = &nodes[0],
		.tail      = &nodes[7]
	};
	unsigned int               c = 0;

	while (!stroll_slist_empty(&list)) {
		cute_check_ptr(stroll_slist_dqueue_front(&list),
		               equal,
		               &nodes[c]);
		c++;
	}

	cute_check_uint(c, equal, stroll_array_nr(nodes));
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_dqueue_front_assert)
{
	cute_expect_assertion(stroll_slist_dqueue_front(NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_dqueue_front_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_withdraw_first)
{
	struct stroll_slist_node   nodes[8] = {
		[0] = { .next = &nodes[1] },
		[1] = { .next = &nodes[2] },
		[2] = { .next = &nodes[3] },
		[3] = { .next = &nodes[4] },
		[4] = { .next = &nodes[5] },
		[5] = { .next = &nodes[6] },
		[6] = { .next = &nodes[7] },
		[7] = { .next = NULL }
	};
	struct stroll_slist        list = {
		.head.next = &nodes[0],
		.tail      = &nodes[7]
	};
	struct stroll_slist_node * node;
	unsigned int               c;

	stroll_slist_withdraw(&list, stroll_slist_head(&list), &nodes[0]);
	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[1]);
	cute_check_ptr(stroll_slist_last(&list), equal, &nodes[7]);

	c = 0;
	stroll_slist_foreach_node(&list, node)
		cute_check_ptr(node, equal, &nodes[++c]);
	cute_check_uint(c, equal, 7);
}

CUTE_TEST(strollut_slist_withdraw_lead)
{
	struct stroll_slist_node   nodes[8] = {
		[0] = { .next = &nodes[1] },
		[1] = { .next = &nodes[2] },
		[2] = { .next = &nodes[3] },
		[3] = { .next = &nodes[4] },
		[4] = { .next = &nodes[5] },
		[5] = { .next = &nodes[6] },
		[6] = { .next = &nodes[7] },
		[7] = { .next = NULL }
	};
	struct stroll_slist        list = {
		.head.next = &nodes[0],
		.tail      = &nodes[7]
	};
	struct stroll_slist_node * node;
	unsigned int               c;

	stroll_slist_withdraw(&list, stroll_slist_head(&list), &nodes[3]);
	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[4]);
	cute_check_ptr(stroll_slist_last(&list), equal, &nodes[7]);

	c = 0;
	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c + 4]);
		c++;
	}
	cute_check_uint(c, equal, 4);
}

CUTE_TEST(strollut_slist_withdraw_mid)
{
	struct stroll_slist_node   nodes[8] = {
		[0] = { .next = &nodes[1] },
		[1] = { .next = &nodes[2] },
		[2] = { .next = &nodes[3] },
		[3] = { .next = &nodes[4] },
		[4] = { .next = &nodes[5] },
		[5] = { .next = &nodes[6] },
		[6] = { .next = &nodes[7] },
		[7] = { .next = NULL }
	};
	struct stroll_slist        list = {
		.head.next = &nodes[0],
		.tail      = &nodes[7]
	};
	struct stroll_slist_node * node;

	stroll_slist_withdraw(&list, &nodes[2], &nodes[5]);
	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_slist_last(&list), equal, &nodes[7]);

	node = stroll_slist_next(stroll_slist_first(&list));
	cute_check_ptr(node, equal, &nodes[1]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &nodes[2]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &nodes[6]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &nodes[7]);
}

CUTE_TEST(strollut_slist_withdraw_last)
{
	struct stroll_slist_node   nodes[8] = {
		[0] = { .next = &nodes[1] },
		[1] = { .next = &nodes[2] },
		[2] = { .next = &nodes[3] },
		[3] = { .next = &nodes[4] },
		[4] = { .next = &nodes[5] },
		[5] = { .next = &nodes[6] },
		[6] = { .next = &nodes[7] },
		[7] = { .next = NULL }
	};
	struct stroll_slist        list = {
		.head.next = &nodes[0],
		.tail      = &nodes[7]
	};
	struct stroll_slist_node * node;
	unsigned int               c;

	stroll_slist_withdraw(&list, &nodes[6], &nodes[7]);
	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_slist_last(&list), equal, &nodes[6]);

	c = 0;
	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}
	cute_check_uint(c, equal, 7);
}

CUTE_TEST(strollut_slist_withdraw_trail)
{
	struct stroll_slist_node   nodes[8] = {
		[0] = { .next = &nodes[1] },
		[1] = { .next = &nodes[2] },
		[2] = { .next = &nodes[3] },
		[3] = { .next = &nodes[4] },
		[4] = { .next = &nodes[5] },
		[5] = { .next = &nodes[6] },
		[6] = { .next = &nodes[7] },
		[7] = { .next = NULL }
	};
	struct stroll_slist        list = {
		.head.next = &nodes[0],
		.tail      = &nodes[7]
	};
	struct stroll_slist_node * node;
	unsigned int               c;

	stroll_slist_withdraw(&list, &nodes[4], &nodes[7]);
	cute_check_ptr(stroll_slist_first(&list), equal, &nodes[0]);
	cute_check_ptr(stroll_slist_last(&list), equal, &nodes[4]);

	c = 0;
	stroll_slist_foreach_node(&list, node) {
		cute_check_ptr(node, equal, &nodes[c]);
		c++;
	}
	cute_check_uint(c, equal, 5);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_withdraw_assert)
{
	struct stroll_slist_node   first = { 0 };
	struct stroll_slist_node   last = { 0 };
	struct stroll_slist_node * alias = &first;
	struct stroll_slist        list = {
		.head.next = &first,
		.tail      = &last
	};

	cute_expect_assertion(stroll_slist_withdraw(NULL, &first, &last));
	cute_expect_assertion(stroll_slist_withdraw(&list, NULL, &last));
	cute_expect_assertion(stroll_slist_withdraw(&list, &first, NULL));
	cute_expect_assertion(stroll_slist_withdraw(&list, &first, alias));

	stroll_slist_init(&list);
	cute_expect_assertion(stroll_slist_withdraw(&list, &first, &last));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_withdraw_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_embed_assert)
{
	struct stroll_slist_node   first = { 0 };
	struct stroll_slist_node   last = { 0 };
	struct stroll_slist        list = {
		.head.next = &first,
		.tail      = &last
	};

	cute_expect_assertion(stroll_slist_embed(NULL,
	                                         stroll_slist_head(&list),
	                                         &first,
	                                         &last));
	cute_expect_assertion(stroll_slist_embed(&list, NULL, &first, &last));
	cute_expect_assertion(stroll_slist_embed(&list,
	                                         stroll_slist_head(&list),
	                                         &first,
	                                         NULL));
	cute_expect_assertion(stroll_slist_embed(&list,
	                                         stroll_slist_head(&list),
	                                         &first,
	                                         NULL));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_embed_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_embed_empty)
{
	struct stroll_slist        list = STROLL_SLIST_INIT(list);
	struct stroll_slist_node   node = STROLL_SLIST_NODE_INIT;
	struct stroll_slist_node * n;
	unsigned int               c;

	stroll_slist_embed(&list, stroll_slist_head(&list), &node, &node);
	cute_check_ptr(stroll_slist_first(&list), equal, &node);
	cute_check_ptr(stroll_slist_last(&list), equal, &node);

	c = 0;
	stroll_slist_foreach_node(&list, n) {
		cute_check_ptr(n, equal, &node);
		c++;
	}
	cute_check_uint(c, equal, 1);
}

CUTE_TEST(strollut_slist_embed_lead)
{
	struct stroll_slist_node   src_nodes[] = {
		[0] = { .next = &src_nodes[1] },
		[1] = { .next = &src_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist_node   dst_nodes[] = {
		[0] = { .next = &dst_nodes[1] },
		[1] = { .next = &dst_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        dst = {
		.head.next = &dst_nodes[0],
		.tail      = &dst_nodes[2]
	};
	struct stroll_slist_node * node;

	stroll_slist_embed(&dst,
	                   stroll_slist_head(&dst),
	                   &src_nodes[0],
	                   &src_nodes[2]);

	node = stroll_slist_first(&dst);
	cute_check_ptr(node, equal, &src_nodes[0]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[1]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[2]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[0]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[1]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[2]);

	node = stroll_slist_last(&dst);
	cute_check_ptr(node, equal, &dst_nodes[2]);
}

CUTE_TEST(strollut_slist_embed_trail)
{
	struct stroll_slist_node   src_nodes[] = {
		[0] = { .next = &src_nodes[1] },
		[1] = { .next = &src_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist_node   dst_nodes[] = {
		[0] = { .next = &dst_nodes[1] },
		[1] = { .next = &dst_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        dst = {
		.head.next = &dst_nodes[0],
		.tail      = &dst_nodes[2]
	};
	struct stroll_slist_node * node;

	stroll_slist_embed(&dst,
	                   stroll_slist_last(&dst),
	                   &src_nodes[0],
	                   &src_nodes[2]);

	node = stroll_slist_first(&dst);
	cute_check_ptr(node, equal, &dst_nodes[0]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[1]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[2]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[0]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[1]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[2]);

	node = stroll_slist_last(&dst);
	cute_check_ptr(node, equal, &src_nodes[2]);
}

CUTE_TEST(strollut_slist_embed_mid)
{
	struct stroll_slist_node   src_nodes[] = {
		[0] = { .next = &src_nodes[1] },
		[1] = { .next = &src_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist_node   dst_nodes[] = {
		[0] = { .next = &dst_nodes[1] },
		[1] = { .next = &dst_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        dst = {
		.head.next = &dst_nodes[0],
		.tail      = &dst_nodes[2]
	};
	struct stroll_slist_node * node;

	stroll_slist_embed(&dst,
	                   &dst_nodes[1],
	                   &src_nodes[0],
	                   &src_nodes[2]);

	node = stroll_slist_first(&dst);
	cute_check_ptr(node, equal, &dst_nodes[0]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[1]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[0]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[1]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[2]);

	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[2]);

	node = stroll_slist_last(&dst);
	cute_check_ptr(node, equal, &dst_nodes[2]);
}

#if defined(CONFIG_STROLL_ASSERT_API)

CUTE_TEST(strollut_slist_splice_assert)
{
	struct stroll_slist_node   src_nodes[] = {
		[0] = { .next = &src_nodes[1] },
		[1] = { .next = &src_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        src = {
		.head.next = &src_nodes[0],
		.tail      = &src_nodes[2]
	};
	struct stroll_slist        dst = STROLL_SLIST_INIT(dst);
	struct stroll_slist *      lst_alias;
	struct stroll_slist_node * node_alias;

	cute_expect_assertion(stroll_slist_splice(NULL,
	                                          stroll_slist_head(&dst),
	                                          &src,
	                                          stroll_slist_head(&src),
	                                          stroll_slist_last(&src)));
	cute_expect_assertion(stroll_slist_splice(&dst,
	                                          NULL,
	                                          &src,
	                                          stroll_slist_head(&src),
	                                          stroll_slist_last(&src)));
	cute_expect_assertion(stroll_slist_splice(&dst,
	                                          stroll_slist_head(&dst),
	                                          NULL,
	                                          stroll_slist_head(&src),
	                                          stroll_slist_last(&src)));
	cute_expect_assertion(stroll_slist_splice(&dst,
	                                          stroll_slist_head(&dst),
	                                          &src,
	                                          NULL,
	                                          stroll_slist_last(&src)));
	cute_expect_assertion(stroll_slist_splice(&dst,
	                                          stroll_slist_head(&dst),
	                                          &src,
	                                          stroll_slist_head(&src),
	                                          NULL));
	lst_alias = &dst;
	cute_expect_assertion(stroll_slist_splice(&dst,
	                                          stroll_slist_head(&dst),
	                                          lst_alias,
	                                          stroll_slist_head(&src),
	                                          stroll_slist_last(&src)));
	cute_expect_assertion(stroll_slist_splice(&dst,
	                                          stroll_slist_head(&src),
	                                          &src,
	                                          stroll_slist_head(&src),
	                                          stroll_slist_last(&src)));
	cute_expect_assertion(stroll_slist_splice(&dst,
	                                          stroll_slist_last(&src),
	                                          &src,
	                                          stroll_slist_head(&src),
	                                          stroll_slist_last(&src)));
	node_alias = &src_nodes[0];
	cute_expect_assertion(stroll_slist_splice(&dst,
	                                          stroll_slist_last(&dst),
	                                          &src,
	                                          &src_nodes[0],
	                                          node_alias));
}

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

STROLLUT_SLIST_NOASSERT(strollut_slist_splice_assert)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

CUTE_TEST(strollut_slist_splice_empty)
{
	struct stroll_slist_node   src_nodes[] = {
		[0] = { .next = &src_nodes[1] },
		[1] = { .next = &src_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        src = {
		.head.next = &src_nodes[0],
		.tail      = &src_nodes[2]
	};
	struct stroll_slist        dst = STROLL_SLIST_INIT(dst);
	struct stroll_slist_node * node;

	stroll_slist_splice(&dst,
	                    stroll_slist_head(&dst),
	                    &src,
	                    &src_nodes[0],
	                    &src_nodes[2]);

	cute_check_ptr(stroll_slist_first(&src), equal, &src_nodes[0]);
	cute_check_ptr(stroll_slist_last(&src), equal, &src_nodes[0]);

	node = stroll_slist_first(&dst);
	cute_check_ptr(node, equal, &src_nodes[1]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[2]);
	node = stroll_slist_last(&dst);
	cute_check_ptr(node, equal, &src_nodes[2]);
}

CUTE_TEST(strollut_slist_splice_lead)
{
	struct stroll_slist_node   src_nodes[] = {
		[0] = { .next = &src_nodes[1] },
		[1] = { .next = &src_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        src = {
		.head.next = &src_nodes[0],
		.tail      = &src_nodes[2]
	};
	struct stroll_slist_node   dst_nodes[] = {
		[0] = { .next = &dst_nodes[1] },
		[1] = { .next = &dst_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        dst = {
		.head.next = &dst_nodes[0],
		.tail      = &dst_nodes[2]
	};
	struct stroll_slist_node * node;

	stroll_slist_splice(&dst,
	                    stroll_slist_head(&dst),
	                    &src,
	                    stroll_slist_head(&src),
	                    &src_nodes[1]);

	cute_check_ptr(stroll_slist_first(&src), equal, &src_nodes[2]);
	cute_check_ptr(stroll_slist_last(&src), equal, &src_nodes[2]);

	node = stroll_slist_first(&dst);
	cute_check_ptr(node, equal, &src_nodes[0]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[1]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[0]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[1]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[2]);
	node = stroll_slist_last(&dst);
	cute_check_ptr(node, equal, &dst_nodes[2]);
}

CUTE_TEST(strollut_slist_splice_mid)
{
	struct stroll_slist_node   src_nodes[] = {
		[0] = { .next = &src_nodes[1] },
		[1] = { .next = &src_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        src = {
		.head.next = &src_nodes[0],
		.tail      = &src_nodes[2]
	};
	struct stroll_slist_node   dst_nodes[] = {
		[0] = { .next = &dst_nodes[1] },
		[1] = { .next = &dst_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        dst = {
		.head.next = &dst_nodes[0],
		.tail      = &dst_nodes[2]
	};
	struct stroll_slist_node * node;

	stroll_slist_splice(&dst,
	                    &dst_nodes[0],
	                    &src,
	                    &src_nodes[0],
	                    &src_nodes[1]);

	node = stroll_slist_first(&src);
	cute_check_ptr(node, equal, &src_nodes[0]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[2]);
	node = stroll_slist_last(&src);
	cute_check_ptr(node, equal, &src_nodes[2]);

	node = stroll_slist_first(&dst);
	cute_check_ptr(node, equal, &dst_nodes[0]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[1]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[1]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[2]);
	node = stroll_slist_last(&dst);
	cute_check_ptr(node, equal, &dst_nodes[2]);
}

CUTE_TEST(strollut_slist_splice_trail)
{
	struct stroll_slist_node   src_nodes[] = {
		[0] = { .next = &src_nodes[1] },
		[1] = { .next = &src_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        src = {
		.head.next = &src_nodes[0],
		.tail      = &src_nodes[2]
	};
	struct stroll_slist_node   dst_nodes[] = {
		[0] = { .next = &dst_nodes[1] },
		[1] = { .next = &dst_nodes[2] },
		[2] = { .next = NULL }
	};
	struct stroll_slist        dst = {
		.head.next = &dst_nodes[0],
		.tail      = &dst_nodes[2]
	};
	struct stroll_slist_node * node;

	stroll_slist_splice(&dst,
	                    &dst_nodes[2],
	                    &src,
	                    &src_nodes[0],
	                    &src_nodes[2]);

	node = stroll_slist_first(&src);
	cute_check_ptr(node, equal, &src_nodes[0]);
	node = stroll_slist_last(&src);
	cute_check_ptr(node, equal, &src_nodes[0]);

	node = stroll_slist_first(&dst);
	cute_check_ptr(node, equal, &dst_nodes[0]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[1]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &dst_nodes[2]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[1]);
	node = stroll_slist_next(node);
	cute_check_ptr(node, equal, &src_nodes[2]);
	node = stroll_slist_last(&dst);
	cute_check_ptr(node, equal, &src_nodes[2]);
}

CUTE_GROUP(strollut_slist_group) = {
	CUTE_REF(strollut_slist_empty),
	CUTE_REF(strollut_slist_init),
	CUTE_REF(strollut_slist_iterate_empty),
	CUTE_REF(strollut_slist_empty_assert),
	CUTE_REF(strollut_slist_single),
	CUTE_REF(strollut_slist_single_assert),
	CUTE_REF(strollut_slist_second),
	CUTE_REF(strollut_slist_append),
	CUTE_REF(strollut_slist_append_assert),
	CUTE_REF(strollut_slist_remove),
	CUTE_REF(strollut_slist_remove_assert),
	CUTE_REF(strollut_slist_move),
	CUTE_REF(strollut_slist_move_assert),
	CUTE_REF(strollut_slist_nqueue_back),
	CUTE_REF(strollut_slist_nqueue_back_assert),
	CUTE_REF(strollut_slist_nqueue_front),
	CUTE_REF(strollut_slist_nqueue_front_assert),
	CUTE_REF(strollut_slist_dqueue_front),
	CUTE_REF(strollut_slist_dqueue_front_assert),
	CUTE_REF(strollut_slist_withdraw_assert),
	CUTE_REF(strollut_slist_withdraw_first),
	CUTE_REF(strollut_slist_withdraw_lead),
	CUTE_REF(strollut_slist_withdraw_mid),
	CUTE_REF(strollut_slist_withdraw_last),
	CUTE_REF(strollut_slist_withdraw_trail),
	CUTE_REF(strollut_slist_embed_assert),
	CUTE_REF(strollut_slist_embed_empty),
	CUTE_REF(strollut_slist_embed_lead),
	CUTE_REF(strollut_slist_embed_trail),
	CUTE_REF(strollut_slist_embed_mid),
	CUTE_REF(strollut_slist_splice_assert),
	CUTE_REF(strollut_slist_splice_empty),
	CUTE_REF(strollut_slist_splice_lead),
	CUTE_REF(strollut_slist_splice_mid),
	CUTE_REF(strollut_slist_splice_trail),
};


CUTE_SUITE_EXTERN(strollut_slist_suite,
                  strollut_slist_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
