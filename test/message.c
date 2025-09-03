/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Loïc Jourdheuil Sellin
 ******************************************************************************/

#include "stroll/message.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>

static uint8_t data[CONFIG_STROLL_MESSAGE_SIZE_MAX - 1];
static size_t test_off = sizeof(data) / 3;
static size_t test_len = sizeof(data) / 3;

CUTE_TEST(strollut_message_init)
{
	struct stroll_msg msg;

	stroll_msg_init(&msg, data, sizeof(data));

	cute_check_uint(0, equal,
		stroll_msg_get_busy(&msg));
	cute_check_uint(0, equal,
		stroll_msg_get_available_head(&msg));
	cute_check_uint(sizeof(data), equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data, equal,
		stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_init_with_busy)
{
	struct stroll_msg msg;

	stroll_msg_init_with_busy(&msg, data, sizeof(data), test_off, test_len);

	cute_check_uint(test_len, equal,
		stroll_msg_get_busy(&msg));
	cute_check_uint(test_off, equal,
		stroll_msg_get_available_head(&msg));
	cute_check_uint(sizeof(data) - test_off - test_len, equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data + test_off, equal,
		stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_pull_head)
{
	struct stroll_msg msg;

	stroll_msg_init_with_busy(&msg, data, sizeof(data), test_off, test_len);
	
	cute_check_ptr(NULL, equal,
		stroll_msg_pull_head(&msg,
		                     stroll_msg_get_busy(&msg) + 1));
	cute_check_ptr(data + test_off, equal,
		stroll_msg_pull_head(&msg,
		                     stroll_msg_get_busy(&msg)));
	cute_check_uint(0, equal,
		stroll_msg_get_busy(&msg));
	cute_check_uint(test_off + test_len, equal,
		stroll_msg_get_available_head(&msg));
	cute_check_uint(sizeof(data) - test_off - test_len, equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data + test_off + test_len, equal,
		stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_pull_tail)
{
	struct stroll_msg msg;

	stroll_msg_init_with_busy(&msg, data, sizeof(data), test_off, test_len);
	
	cute_check_ptr(NULL, equal,
		stroll_msg_pull_tail(&msg,
		                     stroll_msg_get_busy(&msg) + 1));
	cute_check_ptr(data + test_off, equal,
		stroll_msg_pull_tail(&msg,
		                     stroll_msg_get_busy(&msg)));
	cute_check_uint(0, equal,
		stroll_msg_get_busy(&msg));
	cute_check_uint(test_off, equal,
		stroll_msg_get_available_head(&msg));
	cute_check_uint(sizeof(data) - test_off, equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data + test_off, equal,
		stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_push_head)
{
	struct stroll_msg msg;

	stroll_msg_init_with_busy(&msg, data, sizeof(data), test_off, test_len);
	
	cute_check_ptr(NULL, equal,
		stroll_msg_push_head(&msg,
		                     stroll_msg_get_available_head(&msg) + 1));
	cute_check_ptr(data, equal,
		stroll_msg_push_head(&msg,
		                     stroll_msg_get_available_head(&msg)));
	cute_check_uint(test_off + test_len, equal,
		stroll_msg_get_busy(&msg));
	cute_check_uint(0, equal,
		stroll_msg_get_available_head(&msg));
	cute_check_uint(sizeof(data) - test_off - test_len, equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data, equal,
		stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_push_tail)
{
	struct stroll_msg msg;

	stroll_msg_init(&msg, data, sizeof(data));
	
	cute_check_ptr(NULL, equal,
		stroll_msg_push_tail(&msg,
		                     stroll_msg_get_available_tail(&msg) + 1));
	cute_check_ptr(data, equal,
		stroll_msg_push_tail(&msg,
		                     stroll_msg_get_available_tail(&msg)));
	cute_check_uint(sizeof(data), equal,
		stroll_msg_get_busy(&msg));
	cute_check_uint(0, equal,
		stroll_msg_get_available_head(&msg));
	cute_check_uint(0, equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data, equal,
		stroll_msg_get_data(&msg));
}

#if defined(CONFIG_STROLL_ASSERT_API)
static void
strollut_message_assert_bad_struct(struct stroll_msg *msg)
{
	uint8_t *ret __unused;

	cute_expect_assertion(
		stroll_msg_get_available_head(msg));
	cute_expect_assertion(
		stroll_msg_get_available_tail(msg));
	cute_expect_assertion(
		stroll_msg_get_busy(msg));
	cute_expect_assertion(
		stroll_msg_get_data(msg));
	cute_expect_assertion(
		ret = stroll_msg_push_head(msg, 0));
	cute_expect_assertion(
		ret = stroll_msg_pull_head(msg, 0));
	cute_expect_assertion(
		ret = stroll_msg_push_tail(msg, 0));
	cute_expect_assertion(
		ret = stroll_msg_push_tail(msg, 0));
}

CUTE_TEST(strollut_message_assert)
{
	struct stroll_msg bad_init;
	struct stroll_msg bad_buffer = {
		.buffer   = NULL,
		.capacity = sizeof(data),
		.head_off = 0,
		.busy_len = 0,
	};
	struct stroll_msg bad_capacity = {
		.buffer   = data,
		.capacity = sizeof(data) + 1,
		.head_off = 0,
		.busy_len = 0,
	};
	struct stroll_msg bad_head = {
		.buffer   = data,
		.capacity = sizeof(data),
		.head_off = sizeof(data) + 1,
		.busy_len = 0,
	};
	struct stroll_msg bad_busy = {
		.buffer   = data,
		.capacity = sizeof(data),
		.head_off = 0,
		.busy_len = sizeof(data) + 1,
	};
	struct stroll_msg bad_head_and_busy = {
		.buffer   = data,
		.capacity = sizeof(data),
		.head_off = sizeof(data) / 2 + 1,
		.busy_len = sizeof(data) / 2 + 1,
	};
	
	cute_expect_assertion(
		stroll_msg_init(NULL, data, sizeof(data)));
	cute_expect_assertion(
		stroll_msg_init(&bad_init, NULL, 0));
	cute_expect_assertion(
		stroll_msg_init(&bad_init, data, sizeof(data) + 1));

	cute_expect_assertion(
		stroll_msg_init_with_busy(NULL, data, sizeof(data), 0, 0));
	cute_expect_assertion(
		stroll_msg_init_with_busy(&bad_init, NULL, sizeof(data), 0, 0));
	cute_expect_assertion(
		stroll_msg_init_with_busy(&bad_init,
		                          data,
		                          sizeof(data) + 1,
		                          0,
		                          0));
	cute_expect_assertion(
		stroll_msg_init_with_busy(&bad_init,
		                          data,
		                          sizeof(data),
		                          sizeof(data) + 1,
		                          0));
	cute_expect_assertion(
		stroll_msg_init_with_busy(&bad_init,
		                          data,
		                          sizeof(data),
		                          0,
		                          sizeof(data) + 1));
	cute_expect_assertion(
		stroll_msg_init_with_busy(&bad_init,
		                          data,
		                          sizeof(data),
		                          sizeof(data) / 2 + 1,
		                          sizeof(data) / 2 + 1));

	strollut_message_assert_bad_struct(NULL);
	strollut_message_assert_bad_struct(&bad_buffer);
	strollut_message_assert_bad_struct(&bad_capacity);
	strollut_message_assert_bad_struct(&bad_head);
	strollut_message_assert_bad_struct(&bad_busy);
	strollut_message_assert_bad_struct(&bad_head_and_busy);
}
#else
CUTE_TEST(strollut_message_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_GROUP(strollut_message_group) = {
	CUTE_REF(strollut_message_assert),
	CUTE_REF(strollut_message_init),
	CUTE_REF(strollut_message_init_with_busy),
	CUTE_REF(strollut_message_pull_head),
	CUTE_REF(strollut_message_pull_tail),
	CUTE_REF(strollut_message_push_head),
	CUTE_REF(strollut_message_push_tail),
};

CUTE_SUITE_EXTERN(strollut_message_suite,
                  strollut_message_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
