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
#include <stdlib.h>

#define DATA_LEN CONFIG_STROLL_BUFF_CAPACITY_MAX

static uint8_t *data;
static size_t test_off = DATA_LEN / 3;
static size_t test_len = DATA_LEN / 3;

CUTE_TEST(strollut_message_init)
{
	struct stroll_msg msg;

	stroll_msg_setup(&msg, data, DATA_LEN, test_off, test_len);

	cute_check_uint(test_len, equal, stroll_msg_get_busy(&msg));
	cute_check_uint(test_off, equal, stroll_msg_get_available_head(&msg));
	cute_check_uint(DATA_LEN - test_off - test_len,
	                equal,
	                stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data + test_off, equal, stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_init_empty)
{
	struct stroll_msg msg;

	stroll_msg_setup_empty(&msg, data, DATA_LEN);

	cute_check_uint(0, equal, stroll_msg_get_busy(&msg));
	cute_check_uint(0, equal, stroll_msg_get_available_head(&msg));
	cute_check_uint(DATA_LEN, equal, stroll_msg_get_available_tail(&msg));
	cute_check_ptr(stroll_msg_get_data(&msg), equal, NULL);
}

CUTE_TEST(strollut_message_init_with_busy)
{
	struct stroll_msg msg;

	stroll_msg_setup_with_busy(&msg, data, DATA_LEN, test_len);

	cute_check_uint(test_len, equal, stroll_msg_get_busy(&msg));
	cute_check_uint(0, equal, stroll_msg_get_available_head(&msg));
	cute_check_uint(DATA_LEN - test_len,
	                equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data, equal, stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_init_with_reserve)
{
	struct stroll_msg msg;

	stroll_msg_setup_with_reserve(&msg, data, DATA_LEN, test_off);

	cute_check_uint(0, equal, stroll_msg_get_busy(&msg));
	cute_check_uint(test_off, equal, stroll_msg_get_available_head(&msg));
	cute_check_uint(DATA_LEN - test_off,
	                equal,
	                stroll_msg_get_available_tail(&msg));
	cute_check_ptr(stroll_msg_get_data(&msg), equal, NULL);
}

CUTE_TEST(strollut_message_pull_head)
{
	struct stroll_msg msg;

	stroll_msg_setup(&msg, data, DATA_LEN, test_off, test_len);
	
	cute_check_ptr(NULL,
	               equal,
	               stroll_msg_pull_head(&msg,
	                                    stroll_msg_get_busy(&msg) + 1));
	cute_check_ptr(data + test_off,
	               equal,
	               stroll_msg_pull_head(&msg, stroll_msg_get_busy(&msg)));
	cute_check_uint(0,
	                equal,
	                stroll_msg_get_busy(&msg));
	cute_check_uint(test_off + test_len,
	                equal,
	                stroll_msg_get_available_head(&msg));
	cute_check_uint(DATA_LEN - test_off - test_len,
	                equal,
	                stroll_msg_get_available_tail(&msg));
	cute_check_ptr(stroll_msg_get_data(&msg),
	               equal,
	               NULL);
}

CUTE_TEST(strollut_message_pull_tail)
{
	struct stroll_msg msg;

	stroll_msg_setup(&msg, data, DATA_LEN, test_off, test_len);
	
	cute_check_ptr(NULL,
	               equal,
	               stroll_msg_pull_tail(&msg,
	                                    stroll_msg_get_busy(&msg) + 1));
	cute_check_ptr(data + test_off,
	               equal,
	               stroll_msg_pull_tail(&msg,
	                                    stroll_msg_get_busy(&msg)));
	cute_check_uint(0, equal, stroll_msg_get_busy(&msg));
	cute_check_uint(test_off, equal, stroll_msg_get_available_head(&msg));
	cute_check_uint(DATA_LEN - test_off,
	                equal,
	                stroll_msg_get_available_tail(&msg));
	cute_check_ptr(NULL, equal, stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_push_head)
{
	struct stroll_msg msg;

	stroll_msg_setup(&msg, data, DATA_LEN, test_off, test_len);
	
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
	cute_check_uint(DATA_LEN - test_off - test_len, equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data, equal,
		stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_push_tail)
{
	struct stroll_msg msg;

	stroll_msg_setup_empty(&msg, data, DATA_LEN);
	
	cute_check_ptr(NULL, equal,
		stroll_msg_push_tail(&msg,
		                     stroll_msg_get_available_tail(&msg) + 1));
	cute_check_ptr(data, equal,
		stroll_msg_push_tail(&msg,
		                     stroll_msg_get_available_tail(&msg)));
	cute_check_uint(DATA_LEN, equal,
		stroll_msg_get_busy(&msg));
	cute_check_uint(0, equal,
		stroll_msg_get_available_head(&msg));
	cute_check_uint(0, equal,
		stroll_msg_get_available_tail(&msg));
	cute_check_ptr(data, equal,
		stroll_msg_get_data(&msg));
}

CUTE_TEST(strollut_message_loop_pull_head)
{
	struct stroll_msg  msg;
	size_t             i;
	uint8_t           *ptr;

	stroll_msg_setup_with_busy(&msg, data, DATA_LEN, DATA_LEN);

	for (i = 0; i <= DATA_LEN; i++) {
		ptr = stroll_msg_pull_head(&msg, 1);
		if (!ptr)
			break;
		cute_check_ptr_range(ptr,
		                     in,
		                     CUTE_PTR_RANGE(&data[0],
						    &data[DATA_LEN - 1]));
	}
	cute_check_uint(i, equal, DATA_LEN);
}

CUTE_TEST(strollut_message_loop_pull_tail)
{
	struct stroll_msg  msg;
	size_t             i;
	uint8_t           *ptr;

	stroll_msg_setup_with_busy(&msg, data, DATA_LEN, DATA_LEN);

	for (i = 0; i <= DATA_LEN; i++) {
		ptr = stroll_msg_pull_tail(&msg, 1);
		if (!ptr)
			break;
		cute_check_ptr_range(ptr,
		                     in,
		                     CUTE_PTR_RANGE(&data[0],
						    &data[DATA_LEN - 1]));
	}
	cute_check_uint(i, equal, DATA_LEN);
}

CUTE_TEST(strollut_message_loop_push_head)
{
	struct stroll_msg  msg;
	size_t             i;
	uint8_t           *ptr;

	stroll_msg_setup_with_reserve(&msg, data, DATA_LEN, DATA_LEN);

	for (i = 0; i <= DATA_LEN; i++) {
		ptr = stroll_msg_push_head(&msg, 1);
		if (!ptr)
			break;
		cute_check_ptr_range(ptr,
		                     in,
		                     CUTE_PTR_RANGE(&data[0],
		                                    &data[DATA_LEN - 1]));
	}
	cute_check_uint(i, equal, DATA_LEN);
}

CUTE_TEST(strollut_message_loop_push_tail)
{
	struct stroll_msg  msg;
	size_t             i;
	uint8_t           *ptr;

	stroll_msg_setup_empty(&msg, data, DATA_LEN);

	for (i = 0; i <= DATA_LEN; i++) {
		ptr = stroll_msg_push_tail(&msg, 1);
		if (!ptr)
			break;
		cute_check_ptr_range(ptr,
		                     in,
		                     CUTE_PTR_RANGE(&data[0],
		                                    &data[DATA_LEN - 1]));
	}
	cute_check_uint(i, equal, DATA_LEN);
}

#if defined(CONFIG_STROLL_ASSERT_API)

static void
strollut_message_assert_bad_struct(struct stroll_msg *msg)
{
	uint8_t * ret __unused;
	size_t    sz __unused;

	cute_expect_assertion(
		sz = stroll_msg_get_available_head(msg));
	cute_expect_assertion(
		sz = stroll_msg_get_available_tail(msg));
	cute_expect_assertion(
		sz = stroll_msg_get_busy(msg));
	cute_expect_assertion(
		ret = stroll_msg_get_data(msg));
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
		.buff.capacity = DATA_LEN,
		.buff.head_off = 0,
		.buff.busy_len = 0,
		.data          = NULL,
	};
	struct stroll_msg bad_capacity = {
		.data          = data,
		.buff.capacity = DATA_LEN + 1,
		.buff.head_off = 0,
		.buff.busy_len = 0,
	};
	struct stroll_msg bad_head = {
		.data          = data,
		.buff.capacity = DATA_LEN,
		.buff.head_off = DATA_LEN + 1,
		.buff.busy_len = 0,
	};
	struct stroll_msg bad_busy = {
		.data          = data,
		.buff.capacity = DATA_LEN,
		.buff.head_off = 0,
		.buff.busy_len = DATA_LEN + 1,
	};
	struct stroll_msg bad_head_and_busy = {
		.data          = data,
		.buff.capacity = DATA_LEN,
		.buff.head_off = DATA_LEN / 2 + 1,
		.buff.busy_len = DATA_LEN / 2 + 1,
	};
	
	cute_expect_assertion(
		stroll_msg_setup(NULL, data, DATA_LEN, 0, 0));
	cute_expect_assertion(
		stroll_msg_setup(&bad_init, NULL, DATA_LEN, 0, 0));
	cute_expect_assertion(
		stroll_msg_setup(&bad_init,
		                data,
		                DATA_LEN + 1,
		                0,
		                0));
	cute_expect_assertion(
		stroll_msg_setup(&bad_init,
		                data,
		                DATA_LEN,
		                DATA_LEN + 1,
		                0));
	cute_expect_assertion(
		stroll_msg_setup(&bad_init,
		                data,
		                DATA_LEN,
		                0,
		                DATA_LEN + 1));
	cute_expect_assertion(
		stroll_msg_setup(&bad_init,
		                data,
		                DATA_LEN,
		                DATA_LEN / 2 + 1,
		                DATA_LEN / 2 + 1));

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
	CUTE_REF(strollut_message_init_empty),
	CUTE_REF(strollut_message_init_with_busy),
	CUTE_REF(strollut_message_init_with_reserve),
	CUTE_REF(strollut_message_pull_head),
	CUTE_REF(strollut_message_pull_tail),
	CUTE_REF(strollut_message_loop_pull_head),
	CUTE_REF(strollut_message_loop_pull_tail),
	CUTE_REF(strollut_message_push_head),
	CUTE_REF(strollut_message_push_tail),
	CUTE_REF(strollut_message_loop_push_head),
	CUTE_REF(strollut_message_loop_push_tail),
};

static void
strollut_message_setup(void)
{
	data = malloc(DATA_LEN);
}

static void
strollut_message_teardown(void)
{
	free(data);
}

CUTE_SUITE_EXTERN(strollut_message_suite,
                  strollut_message_group,
                  strollut_message_setup,
                  strollut_message_teardown,
                  CUTE_NONE_TMOUT);
