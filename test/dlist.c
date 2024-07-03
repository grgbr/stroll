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

#if 0
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

/******************************************************************************
 * Sorting tests
 ******************************************************************************/

struct strollut_slist_node {
	struct stroll_slist_node super;
	unsigned int             id;
	const char *             str;
};

#define STROLLUT_SLIST_NODE_INIT(_id) \
	{ \
		.super = STROLL_SLIST_NODE_INIT, \
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

static void (*strollut_slist_sort)(struct stroll_slist * __restrict list,
                                   unsigned int                     nr,
                                   stroll_slist_cmp_fn *            compare);

#define STROLLUT_SLIST_SORT_SETUP(_setup, _sort) \
	static void _setup(void) \
	{ \
		strollut_slist_sort = _sort; \
	}

#define STROLLUT_SLIST_UNSUP(_setup) \
	static void _setup(void) { cute_skip("support not compiled-in"); }

static inline int
strollut_slist_compare_min_num(
	const struct stroll_slist_node * __restrict a,
	const struct stroll_slist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_slist_node * _a =
		stroll_slist_entry(a, const struct strollut_slist_node, super);
	const struct strollut_slist_node * _b =
		stroll_slist_entry(b, const struct strollut_slist_node, super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static void
strollut_slist_sort_check_num(struct strollut_slist_node      nodes[],
	                      const struct strollut_list_xpct expected[],
	                      unsigned int                    nr)
{
	struct stroll_slist list = STROLL_SLIST_INIT(list);
	unsigned int        n;

	for (n = 0; n < nr; n++)
		stroll_slist_nqueue_back(&list, &nodes[n].super);

	strollut_slist_sort(&list, nr, strollut_slist_compare_min_num);

	for (n = 0; n < nr; n++) {
		const struct strollut_list_xpct *  xpct = &expected[n];
		struct strollut_slist_node *       node;

		cute_check_bool(stroll_slist_empty(&list), is, false);

		node = stroll_slist_entry(stroll_slist_dqueue_front(&list),
		                          struct strollut_slist_node,
		                          super);

		cute_check_uint(node->id, equal, xpct->id);
		cute_check_str(node->str, equal, xpct->str);
		cute_check_ptr(node, equal, xpct->ptr);
	}

	cute_check_bool(stroll_slist_empty(&list), is, true);
}

CUTE_TEST(strollut_slist_sort_num1)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0])
	};

	strollut_slist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_inorder_num2)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(1)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(1, &nodes[1])
	};

	strollut_slist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_revorder_num2)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(1),
		STROLLUT_SLIST_NODE_INIT(0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(1, &nodes[0])
	};

	strollut_slist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_duplicate_num2)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(0, &nodes[1])
	};

	strollut_slist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_inorder_num17)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(1),
		STROLLUT_SLIST_NODE_INIT(2),
		STROLLUT_SLIST_NODE_INIT(3),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(5),
		STROLLUT_SLIST_NODE_INIT(6),
		STROLLUT_SLIST_NODE_INIT(7),
		STROLLUT_SLIST_NODE_INIT(8),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(10),
		STROLLUT_SLIST_NODE_INIT(11),
		STROLLUT_SLIST_NODE_INIT(12),
		STROLLUT_SLIST_NODE_INIT(13),
		STROLLUT_SLIST_NODE_INIT(14),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(16)
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

	strollut_slist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_revorder_num17)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(16),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(14),
		STROLLUT_SLIST_NODE_INIT(13),
		STROLLUT_SLIST_NODE_INIT(12),
		STROLLUT_SLIST_NODE_INIT(11),
		STROLLUT_SLIST_NODE_INIT(10),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(8),
		STROLLUT_SLIST_NODE_INIT(7),
		STROLLUT_SLIST_NODE_INIT(6),
		STROLLUT_SLIST_NODE_INIT(5),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(3),
		STROLLUT_SLIST_NODE_INIT(2),
		STROLLUT_SLIST_NODE_INIT(1),
		STROLLUT_SLIST_NODE_INIT(0)
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

	strollut_slist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_unorder_num17)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(11),
		STROLLUT_SLIST_NODE_INIT(16),
		STROLLUT_SLIST_NODE_INIT(1),
		STROLLUT_SLIST_NODE_INIT(7),
		STROLLUT_SLIST_NODE_INIT(14),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(8),
		STROLLUT_SLIST_NODE_INIT(3),
		STROLLUT_SLIST_NODE_INIT(6),
		STROLLUT_SLIST_NODE_INIT(12),
		STROLLUT_SLIST_NODE_INIT(2),
		STROLLUT_SLIST_NODE_INIT(5),
		STROLLUT_SLIST_NODE_INIT(10),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(13)
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

	strollut_slist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_duplicate_num17)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(7),
		STROLLUT_SLIST_NODE_INIT(14),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(8),
		STROLLUT_SLIST_NODE_INIT(3),
		STROLLUT_SLIST_NODE_INIT(6),
		STROLLUT_SLIST_NODE_INIT(12),
		STROLLUT_SLIST_NODE_INIT(2),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(13)
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

	strollut_slist_sort_check_num(nodes, xpct, stroll_array_nr(xpct));
}

static inline int
strollut_slist_compare_min_str(
	const struct stroll_slist_node * __restrict a,
	const struct stroll_slist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_slist_node * _a =
		stroll_slist_entry(a, const struct strollut_slist_node, super);
	const struct strollut_slist_node * _b =
		stroll_slist_entry(b, const struct strollut_slist_node, super);

	return (_a->id > _b->id) - (_a->id < _b->id);
}

static void
strollut_slist_sort_check_str(struct strollut_slist_node      nodes[],
	                      const struct strollut_list_xpct expected[],
	                      unsigned int                    nr)
{
	struct stroll_slist list = STROLL_SLIST_INIT(list);
	unsigned int        n;

	for (n = 0; n < nr; n++)
		stroll_slist_nqueue_back(&list, &nodes[n].super);

	strollut_slist_sort(&list, nr, strollut_slist_compare_min_str);

	for (n = 0; n < nr; n++) {
		const struct strollut_list_xpct *  xpct = &expected[n];
		struct strollut_slist_node *       node;

		cute_check_bool(stroll_slist_empty(&list), is, false);

		node = stroll_slist_entry(stroll_slist_dqueue_front(&list),
		                          struct strollut_slist_node,
		                          super);

		cute_check_uint(node->id, equal, xpct->id);
		cute_check_str(node->str, equal, xpct->str);
		cute_check_ptr(node, equal, xpct->ptr);
	}

	cute_check_bool(stroll_slist_empty(&list), is, true);
}

CUTE_TEST(strollut_slist_sort_str1)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0])
	};

	strollut_slist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_inorder_str2)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(1)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(1, &nodes[1])
	};

	strollut_slist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_revorder_str2)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(1),
		STROLLUT_SLIST_NODE_INIT(0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[1]),
		STROLLUT_LIST_XPCT_INIT(1, &nodes[0])
	};

	strollut_slist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_duplicate_str2)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(0)
	};
	const struct strollut_list_xpct xpct[] = {
		STROLLUT_LIST_XPCT_INIT(0, &nodes[0]),
		STROLLUT_LIST_XPCT_INIT(0, &nodes[1])
	};

	strollut_slist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_inorder_str17)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(1),
		STROLLUT_SLIST_NODE_INIT(2),
		STROLLUT_SLIST_NODE_INIT(3),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(5),
		STROLLUT_SLIST_NODE_INIT(6),
		STROLLUT_SLIST_NODE_INIT(7),
		STROLLUT_SLIST_NODE_INIT(8),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(10),
		STROLLUT_SLIST_NODE_INIT(11),
		STROLLUT_SLIST_NODE_INIT(12),
		STROLLUT_SLIST_NODE_INIT(13),
		STROLLUT_SLIST_NODE_INIT(14),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(16)
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

	strollut_slist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_revorder_str17)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(16),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(14),
		STROLLUT_SLIST_NODE_INIT(13),
		STROLLUT_SLIST_NODE_INIT(12),
		STROLLUT_SLIST_NODE_INIT(11),
		STROLLUT_SLIST_NODE_INIT(10),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(8),
		STROLLUT_SLIST_NODE_INIT(7),
		STROLLUT_SLIST_NODE_INIT(6),
		STROLLUT_SLIST_NODE_INIT(5),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(3),
		STROLLUT_SLIST_NODE_INIT(2),
		STROLLUT_SLIST_NODE_INIT(1),
		STROLLUT_SLIST_NODE_INIT(0)
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

	strollut_slist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_unorder_str17)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(11),
		STROLLUT_SLIST_NODE_INIT(16),
		STROLLUT_SLIST_NODE_INIT(1),
		STROLLUT_SLIST_NODE_INIT(7),
		STROLLUT_SLIST_NODE_INIT(14),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(8),
		STROLLUT_SLIST_NODE_INIT(3),
		STROLLUT_SLIST_NODE_INIT(6),
		STROLLUT_SLIST_NODE_INIT(12),
		STROLLUT_SLIST_NODE_INIT(2),
		STROLLUT_SLIST_NODE_INIT(5),
		STROLLUT_SLIST_NODE_INIT(10),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(13)
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

	strollut_slist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

CUTE_TEST(strollut_slist_sort_duplicate_str17)
{
	struct strollut_slist_node      nodes[] = {
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(7),
		STROLLUT_SLIST_NODE_INIT(14),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(15),
		STROLLUT_SLIST_NODE_INIT(8),
		STROLLUT_SLIST_NODE_INIT(3),
		STROLLUT_SLIST_NODE_INIT(6),
		STROLLUT_SLIST_NODE_INIT(12),
		STROLLUT_SLIST_NODE_INIT(2),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(9),
		STROLLUT_SLIST_NODE_INIT(4),
		STROLLUT_SLIST_NODE_INIT(0),
		STROLLUT_SLIST_NODE_INIT(13)
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

	strollut_slist_sort_check_str(nodes, xpct, stroll_array_nr(xpct));
}

#include "array_data.h"

struct strollut_slist_array_node {
	struct stroll_slist_node                  super;
	union {
		const struct strollut_array_num * num;
		const struct strollut_array_str * str;
	};
};

static struct strollut_slist_array_node
strollut_list_tosort_num[STROLLUT_ARRAY_NUM_NR];

static inline int
strollut_slist_compare_elem_min_num(
	const struct stroll_slist_node * __restrict a,
	const struct stroll_slist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_slist_array_node * _a =
		stroll_slist_entry(a,
		                   const struct strollut_slist_array_node,
		                   super);
	const struct strollut_slist_array_node * _b =
		stroll_slist_entry(b,
		                   const struct strollut_slist_array_node,
		                   super);

	return (_a->num->key > _b->num->key) - (_a->num->key < _b->num->key);
}

static inline int
strollut_slist_compare_elem_max_num(
	const struct stroll_slist_node * __restrict a,
	const struct stroll_slist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_slist_array_node * _a =
		stroll_slist_entry(a,
		                   const struct strollut_slist_array_node,
		                   super);
	const struct strollut_slist_array_node * _b =
		stroll_slist_entry(b,
		                   const struct strollut_slist_array_node,
		                   super);

	return (_b->num->key > _a->num->key) - (_b->num->key < _a->num->key);
}

static void
strollut_slist_sort_check_large_num(const struct strollut_array_num * array,
                                    const struct strollut_array_num * expected,
                                    stroll_slist_cmp_fn *             cmp)
{
	struct stroll_slist list = STROLL_SLIST_INIT(list);
	unsigned int        e;

	for (e = 0; e < stroll_array_nr(strollut_list_tosort_num); e++) {
		strollut_list_tosort_num[e].num = &array[e];
		stroll_slist_nqueue_back(&list,
		                         &strollut_list_tosort_num[e].super);
	}

	strollut_slist_sort(&list,
	                    stroll_array_nr(strollut_list_tosort_num),
	                    cmp);

	for (e = 0; e < stroll_array_nr(strollut_list_tosort_num); e++) {
		const struct strollut_array_num *  xpct = &expected[e];
		struct strollut_slist_array_node * node;

		cute_check_bool(stroll_slist_empty(&list), is, false);

		node = stroll_slist_entry(stroll_slist_dqueue_front(&list),
		                          struct strollut_slist_array_node,
		                          super);

		cute_check_sint(node->num->key, equal, xpct->key);
		cute_check_ptr(node->num, equal, xpct->ptr);
	}

	cute_check_bool(stroll_slist_empty(&list), is, true);
}

CUTE_TEST(strollut_slist_sort_inorder_large_num)
{
	strollut_slist_sort_check_large_num(
			strollut_array_num_input,
			strollut_array_num_inorder,
			strollut_slist_compare_elem_min_num);
}

CUTE_TEST(strollut_slist_sort_postorder_large_num)
{
	strollut_slist_sort_check_large_num(
			strollut_array_num_input,
			strollut_array_num_postorder,
			strollut_slist_compare_elem_max_num);
}

static struct strollut_slist_array_node
strollut_list_tosort_str[STROLLUT_ARRAY_STR_NR];

static inline int
strollut_slist_compare_elem_min_str(
	const struct stroll_slist_node * __restrict a,
	const struct stroll_slist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_slist_array_node * _a =
		stroll_slist_entry(a,
		                   const struct strollut_slist_array_node,
		                   super);
	const struct strollut_slist_array_node * _b =
		stroll_slist_entry(b,
		                   const struct strollut_slist_array_node,
		                   super);

	return strcmp(_a->str->word, _b->str->word);
}

static inline int
strollut_slist_compare_elem_max_str(
	const struct stroll_slist_node * __restrict a,
	const struct stroll_slist_node * __restrict b,
	void *                                      data __unused)
{
	const struct strollut_slist_array_node * _a =
		stroll_slist_entry(a,
		                   const struct strollut_slist_array_node,
		                   super);
	const struct strollut_slist_array_node * _b =
		stroll_slist_entry(b,
		                   const struct strollut_slist_array_node,
		                   super);

	return strcmp(_b->str->word, _a->str->word);
}

static void
strollut_slist_sort_check_large_str(const struct strollut_array_str * array,
                                    const struct strollut_array_str * expected,
                                    stroll_slist_cmp_fn *             cmp)
{
	struct stroll_slist list = STROLL_SLIST_INIT(list);
	unsigned int        e;

	for (e = 0; e < stroll_array_nr(strollut_list_tosort_str); e++) {
		strollut_list_tosort_str[e].str = &array[e];
		stroll_slist_nqueue_back(&list,
		                         &strollut_list_tosort_str[e].super);
	}

	strollut_slist_sort(&list,
	                    stroll_array_nr(strollut_list_tosort_str),
	                    cmp);

	for (e = 0; e < stroll_array_nr(strollut_list_tosort_str); e++) {
		const struct strollut_array_str *  xpct = &expected[e];
		struct strollut_slist_array_node * node;

		cute_check_bool(stroll_slist_empty(&list), is, false);

		node = stroll_slist_entry(stroll_slist_dqueue_front(&list),
		                          struct strollut_slist_array_node,
		                          super);

		cute_check_str(node->str->word, equal, xpct->word);
		cute_check_ptr(node->str, equal, xpct->ptr);
	}

	cute_check_bool(stroll_slist_empty(&list), is, true);
}

CUTE_TEST(strollut_slist_sort_inorder_large_str)
{
	strollut_slist_sort_check_large_str(
			strollut_array_str_input,
			strollut_array_str_inorder,
			strollut_slist_compare_elem_min_str);
}

CUTE_TEST(strollut_slist_sort_postorder_large_str)
{
	strollut_slist_sort_check_large_str(
			strollut_array_str_input,
			strollut_array_str_postorder,
			strollut_slist_compare_elem_max_str);
}

CUTE_GROUP(strollut_slist_sort_group) = {
	CUTE_REF(strollut_slist_sort_num1),
	CUTE_REF(strollut_slist_sort_inorder_num2),
	CUTE_REF(strollut_slist_sort_revorder_num2),
	CUTE_REF(strollut_slist_sort_duplicate_num2),
	CUTE_REF(strollut_slist_sort_inorder_num17),
	CUTE_REF(strollut_slist_sort_revorder_num17),
	CUTE_REF(strollut_slist_sort_unorder_num17),
	CUTE_REF(strollut_slist_sort_duplicate_num17),

	CUTE_REF(strollut_slist_sort_str1),
	CUTE_REF(strollut_slist_sort_inorder_str2),
	CUTE_REF(strollut_slist_sort_revorder_str2),
	CUTE_REF(strollut_slist_sort_duplicate_str2),
	CUTE_REF(strollut_slist_sort_inorder_str17),
	CUTE_REF(strollut_slist_sort_revorder_str17),
	CUTE_REF(strollut_slist_sort_unorder_str17),
	CUTE_REF(strollut_slist_sort_duplicate_str17),

	CUTE_REF(strollut_slist_sort_inorder_large_num),
	CUTE_REF(strollut_slist_sort_postorder_large_num),

	CUTE_REF(strollut_slist_sort_inorder_large_str),
	CUTE_REF(strollut_slist_sort_postorder_large_str),
};

#if defined(CONFIG_STROLL_SLIST_BUBBLE_SORT)

static void
strollut_slist_bubble_sort(struct stroll_slist * __restrict list,
                           unsigned int                     nr __unused,
                           stroll_slist_cmp_fn *            compare)
{
	stroll_slist_bubble_sort(list, compare, NULL);
}

STROLLUT_SLIST_SORT_SETUP(strollut_slist_bubble_setup,
                          strollut_slist_bubble_sort)

#else   /* !defined(CONFIG_STROLL_SLIST_BUBBLE_SORT) */
STROLLUT_SLIST_UNSUP(strollut_slist_bubble_setup)
#endif  /* defined(CONFIG_STROLL_SLIST_BUBBLE_SORT) */

CUTE_SUITE_STATIC(strollut_slist_bubble_suite,
                  strollut_slist_sort_group,
                  strollut_slist_bubble_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_SLIST_SELECT_SORT)

static void
strollut_slist_select_sort(struct stroll_slist * __restrict list,
                           unsigned int                     nr __unused,
                           stroll_slist_cmp_fn *            compare)
{
	stroll_slist_select_sort(list, compare, NULL);
}

STROLLUT_SLIST_SORT_SETUP(strollut_slist_select_setup,
                          strollut_slist_select_sort)

#else   /* !defined(CONFIG_STROLL_SLIST_SELECT_SORT) */
STROLLUT_SLIST_UNSUP(strollut_slist_select_setup)
#endif  /* defined(CONFIG_STROLL_SLIST_SELECT_SORT) */

CUTE_SUITE_STATIC(strollut_slist_select_suite,
                  strollut_slist_sort_group,
                  strollut_slist_select_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_SLIST_INSERT_SORT)

static void
strollut_slist_insert_sort(struct stroll_slist * __restrict list,
                           unsigned int                     nr __unused,
                           stroll_slist_cmp_fn *            compare)
{
	stroll_slist_insert_sort(list, compare, NULL);
}

STROLLUT_SLIST_SORT_SETUP(strollut_slist_insert_setup,
                          strollut_slist_insert_sort)

#else   /* !defined(CONFIG_STROLL_SLIST_INSERT_SORT) */
STROLLUT_SLIST_UNSUP(strollut_slist_insert_setup)
#endif  /* defined(CONFIG_STROLL_SLIST_INSERT_SORT) */

CUTE_SUITE_STATIC(strollut_slist_insert_suite,
                  strollut_slist_sort_group,
                  strollut_slist_insert_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);

#if defined(CONFIG_STROLL_SLIST_MERGE_SORT)

static void
strollut_slist_merge_sort(struct stroll_slist * __restrict list,
                          unsigned int                     nr,
                          stroll_slist_cmp_fn *            compare)
{
	stroll_slist_merge_sort(list, nr, compare, NULL);
}

STROLLUT_SLIST_SORT_SETUP(strollut_slist_merge_setup,
                          strollut_slist_merge_sort)

#else   /* !defined(CONFIG_STROLL_SLIST_MERGE_SORT) */
STROLLUT_SLIST_UNSUP(strollut_slist_merge_setup)
#endif  /* defined(CONFIG_STROLL_SLIST_MERGE_SORT) */

CUTE_SUITE_STATIC(strollut_slist_merge_suite,
                  strollut_slist_sort_group,
                  strollut_slist_merge_setup,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
#endif

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
	CUTE_REF(strollut_dlist_append),
	CUTE_REF(strollut_dlist_append_assert),
	CUTE_REF(strollut_dlist_remove),
	CUTE_REF(strollut_dlist_remove_assert),
	CUTE_REF(strollut_dlist_remove_init),
	CUTE_REF(strollut_dlist_remove_init_assert),
#if 0
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

	CUTE_REF(strollut_slist_bubble_suite),
	CUTE_REF(strollut_slist_select_suite),
	CUTE_REF(strollut_slist_insert_suite),
	CUTE_REF(strollut_slist_merge_suite)
#endif
};

CUTE_SUITE_EXTERN(strollut_dlist_suite,
                  strollut_dlist_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
