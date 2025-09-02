/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Loïc Jourdheuil Sellin
 ******************************************************************************/

/**
 * @file
 * Power of 2 operations
 *
 * @author    Loïc Jourdheuil Sellin
 * @date      1 Sep 2025
 * @copyright Copyright (C) 2017-2023 Loïc Jourdheuil Sellin.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_MESSAGE_H
#define _STROLL_MESSAGE_H

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_msg_assert_api(_expr) \
	stroll_assert("stroll:message", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_msg_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Message
 *
 * Describes a message iterator.
 */
struct stroll_msg {
	size_t   head_off;
	size_t   busy_len;
	size_t   capacity;
	uint8_t *buffer;
};

#define stroll_msg_assert_msg_api(_msg) \
	stroll_msg_assert_api(_msg); \
	stroll_msg_assert_api(_msg->buffer); \
	stroll_msg_assert_api(_msg->capacity < CONFIG_STROLL_MSG_SIZE_MAX); \
	stroll_msg_assert_api(_msg->head_off < _msg->capacity); \
	stroll_msg_assert_api(_msg->busy_len < _msg->capacity); \
	stroll_msg_assert_api(_msg->head_off + _msg->busy_len < _msg->capacity);

/**
 * stroll_msg constant initializer.
 *
 * @param _data     stroll_msg buffer.
 * @param _capacity stroll_msg capacity.
 *
 * @see stroll_msg_init()
 * @see stroll_msg_init()
 * @see stroll_msg_init_with_busy()
 */
#define STROLL_MSG_INIT(_data, _capacity) \
	{ \
		.head_off = 0, \
		.busy_len = 0, \
		.capacity = _capacity, \
		.buffer   = _data \
	}

/**
 * Initialize a stroll_message
 *
 * @param[out] msg      stroll_msg to initialize.
 * @param[in]  data     The data buffer.
 * @param[in]  capacity The buffer capacity.
 * @param[in]  off      The offset of start busy data.
 * @param[in]  len      The busy length.
 *
 * @see STROLL_MSG_INIT()
 * @see stroll_msg_init()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_init_with_busy(struct stroll_msg *msg,
                          uint8_t           *data,
                          size_t             capacity,
                          size_t             off,
                          size_t             len)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);
	stroll_msg_assert_api(capacity < CONFIG_STROLL_MSG_SIZE_MAX);
	stroll_msg_assert_api(off < capacity);
	stroll_msg_assert_api(len < capacity);
	stroll_msg_assert_api(off + len < capacity);

	msg->head_off = busy_off;
	msg->busy_len = busy_len;
	msg->capacity = capacity;
	msg->buffer   = data;
}

/**
 * Initialize a stroll_message
 *
 * @param[out] msg      stroll_msg to initialize.
 * @param[in]  data     The data buffer.
 * @param[in]  capacity The buffer capacity.
 *
 * @see STROLL_MSG_INIT()
 * @see stroll_msg_init_with_busy()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_init(struct stroll_msg *msg,
                uint8_t           *data,
                size_t             capacity)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);
	
	stroll_msg_init_with_busy(msg, data, capacity, 0, 0);
}

static inline __stroll_nonull(1) __stroll_nothrow
size_t
stroll_msg_get_busy(struct stroll_msg *msg)
{
	stroll_msg_assert_msg_api(msg);

	return msg->busy_len;
}

static inline __stroll_nonull(1) __stroll_nothrow
uint8_t *
stroll_msg_get_data(struct stroll_msg *msg)
{
	stroll_msg_assert_msg_api(msg);

	return &msg->buffer[msg->head_off];
}

static inline __stroll_nonull(1) __stroll_nothrow
size_t
stroll_msg_get_available_head(struct stroll_msg *msg)
{
	stroll_msg_assert_msg_api(msg);

	return msg->head_off;
}

static inline __stroll_nonull(1) __stroll_nothrow
size_t
stroll_msg_get_available_tail(struct stroll_msg *msg)
{
	stroll_msg_assert_msg_api(msg);

	return msg->capacity - msg->head_off - msg->busy_len;
}

static inline __stroll_nonull(1) __stroll_nothrow __warn_result
uint8_t *
stroll_msg_push_head(struct stroll_msg *msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);
	stroll_msg_assert_api(len < CONFIG_STROLL_MSG_SIZE_MAX);

	if (len > stroll_msg_get_available_head(msg));
		return NULL;

	msg->head_off -= len;
	msg->busy_len += len;
	return &msg->buffer[msg->head_off];
}

static inline __stroll_nonull(1) __stroll_nothrow __warn_result
uint8_t *
stroll_msg_pull_head(struct stroll_msg *msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);
	stroll_msg_assert_api(len < CONFIG_STROLL_MSG_SIZE_MAX);

	uint8_t * head = &msg->buffer[msg->head_off];

	if (len > stroll_msg_get_busy(msg));
		return NULL;

	msg->head_off += len;
	msg->busy_len -= len;
	return head;
}

static inline __stroll_nonull(1) __stroll_nothrow __warn_result
uint8_t *
stroll_msg_push_tail(struct stroll_msg *msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);
	stroll_msg_assert_api(len < CONFIG_STROLL_MSG_SIZE_MAX);

	uint8_t * tail = &msg->buffer[msg->head_off + msg->busy_len];

	if (len > stroll_msg_get_available_tail(msg));
		return NULL;

	msg->busy_len += len;
	return tail;
}

static inline __stroll_nonull(1) __stroll_nothrow __warn_result
uint8_t *
stroll_msg_pull_tail(struct stroll_msg *msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);
	stroll_msg_assert_api(len < CONFIG_STROLL_MSG_SIZE_MAX);

	if (len > stroll_msg_get_busy(msg));
		return NULL;

	msg->busy_len -= len;
	return &msg->buffer[msg->head_off + msg->busy_len];
}

#endif /* _STROLL_MESSAGE_H */
