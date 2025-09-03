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

#include <stroll/cdefs.h>
#include <stdint.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_msg_assert_api(_expr) \
	stroll_assert("stroll:message", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_msg_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

compile_assert(CONFIG_STROLL_MESSAGE_SIZE_MAX <= SIZE_MAX);

/**
 * Message iterator.
 *
 * Describes a message iterator than can push and pull data from the head and
 * the tail.
 */
struct stroll_msg {
	/**
	 * @internal
	 *
	 * offset starting data.
	 */
	size_t   head_off;

	/**
	 * @internal
	 *
	 * length of busy data.
	 */
	size_t   busy_len;

	/**
	 * @internal
	 *
	 * data capacity.
	 */
	size_t   capacity;

	/**
	 * @internal
	 *
	 * pointer to data buffer.
	 */
	uint8_t *buffer;
};

#define stroll_msg_assert_msg_api(_msg) \
	stroll_msg_assert_api(_msg); \
	stroll_msg_assert_api(_msg->buffer); \
	stroll_msg_assert_api(_msg->capacity < CONFIG_STROLL_MESSAGE_SIZE_MAX); \
	stroll_msg_assert_api(_msg->head_off <= _msg->capacity); \
	stroll_msg_assert_api(_msg->busy_len <= _msg->capacity); \
	stroll_msg_assert_api(_msg->busy_len <= _msg->capacity - _msg->head_off);

/**
 * stroll_msg constant initializer.
 *
 * @param _data     stroll_msg buffer.
 * @param _capacity stroll_msg capacity.
 * @param _off      stroll_msg start offset.
 * @param _len      stroll_msg busy length.
 *
 * @see
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 * - stroll_msg_init()
 * - stroll_msg_init_empty()
 * - stroll_msg_init_with_busy()
 * - stroll_msg_init_with_reserve()
 */
#define STROLL_MSG_INIT(_data, _capacity, _off, _len) \
	{ \
		.buffer   = compile_eval(_data != NULL, \
		                         _data, \
		                         "Data MUST not be NULL"), \
		.capacity = compile_eval(_capacity < CONFIG_STROLL_MESSAGE_SIZE_MAX, \
		                         _capacity, \
		                         "Capacity MUST be lower than CONFIG_STROLL_MESSAGE_SIZE_MAX"), \
		.head_off = compile_eval(_off <= _capacity, \
		                         _off, \
		                         "Offset MUST be lower than Capacity"), \
		.busy_len = compile_eval(_len <= _capacity, \
		                         compile_eval(_len <= _capacity - _off, \
		                                      _len, \
		                                      "Length + offset MUST be lower than Capacity"), \
		                         "Length MUST be lower than Capacity"), \
	}

/**
 * stroll_msg constant initializer with empty data.
 *
 * @param _data     stroll_msg buffer.
 * @param _capacity stroll_msg capacity.
 *
 * @see
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 * - stroll_msg_init()
 * - stroll_msg_init_empty()
 * - stroll_msg_init_with_busy()
 * - stroll_msg_init_with_reserve()
 */
#define STROLL_MSG_INIT_EMPTY(_data, _capacity) \
	STROLL_MSG_INIT(_data, _capacity, 0, 0)

/**
 * stroll_msg constant initializer with set data.
 *
 * @param _data     stroll_msg buffer.
 * @param _capacity stroll_msg capacity.
 * @param _len      stroll_msg busy length.
 *
 * @see
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 * - stroll_msg_init()
 * - stroll_msg_init_empty()
 * - stroll_msg_init_with_busy()
 * - stroll_msg_init_with_reserve()
 */
#define STROLL_MSG_INIT_WITH_BUSY(_data, _capacity, _len) \
	STROLL_MSG_INIT(_data, _capacity, 0, _len)

/**
 * stroll_msg constant initializer with header reserve.
 *
 * @param _data     stroll_msg buffer.
 * @param _capacity stroll_msg capacity.
 * @param _off      stroll_msg start offset.
 *
 * @see
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - stroll_msg_init()
 * - stroll_msg_init_empty()
 * - stroll_msg_init_with_busy()
 * - stroll_msg_init_with_reserve()
 */
#define STROLL_MSG_INIT_WITH_RESERVE(_data, _capacity, _off) \
	STROLL_MSG_INIT(_data, _capacity, _off, 0)

/**
 * Initialize a stroll_message.
 *
 * @param[out] msg      stroll_msg to initialize.
 * @param[in]  data     stroll_msg buffer.
 * @param[in]  capacity stroll_msg capacity.
 * @param[in]  off      stroll_msg start offset.
 * @param[in]  len      stroll_msh busy length.
 *
 * @see
 * - stroll_msg_init_empty()
 * - stroll_msg_init_with_busy()
 * - stroll_msg_init_with_reserve()
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_init(struct stroll_msg *msg,
                uint8_t           *data,
                size_t             capacity,
                size_t             off,
                size_t             len)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);
	stroll_msg_assert_api(capacity < CONFIG_STROLL_MESSAGE_SIZE_MAX);
	stroll_msg_assert_api(off <= capacity);
	stroll_msg_assert_api(len <= capacity);
	stroll_msg_assert_api(off + len <= capacity);

	msg->head_off = off;
	msg->busy_len = len;
	msg->capacity = capacity;
	msg->buffer   = data;
}

/**
 * Initialize a stroll_message with empty data.
 *
 * @param[out] msg      stroll_msg to initialize.
 * @param[in]  data     stroll_msg buffer.
 * @param[in]  capacity stroll_msg capacity.
 *
 * @see
 * - stroll_msg_init()
 * - stroll_msg_init_with_busy()
 * - stroll_msg_init_with_reserve()
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_init_empty(struct stroll_msg *msg,
                      uint8_t           *data,
                      size_t             capacity)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);

	stroll_msg_init(msg, data, capacity, 0, 0);
}

/**
 * Initialize a stroll_message with set data.
 *
 * @param[out] msg      stroll_msg to initialize.
 * @param[in]  data     stroll_msg buffer.
 * @param[in]  capacity stroll_msg capacity.
 * @param[in]  len      stroll_msh busy length.
 *
 * @see
 * - stroll_msg_init()
 * - stroll_msg_init_empty()
 * - stroll_msg_init_with_reserve()
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_init_with_busy(struct stroll_msg *msg,
                          uint8_t           *data,
                          size_t             capacity,
                          size_t             len)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);

	stroll_msg_init(msg, data, capacity, 0, len);
}

/**
 * Initialize a stroll_message header reserve.
 *
 * @param[out] msg      stroll_msg to initialize.
 * @param[in]  data     stroll_msg buffer.
 * @param[in]  capacity stroll_msg capacity.
 * @param[in]  off      stroll_msg start offset.
 *
 * @see
 * - stroll_msg_init()
 * - stroll_msg_init_empty()
 * - stroll_msg_init_with_busy()
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_init_with_reserve(struct stroll_msg *msg,
                             uint8_t           *data,
                             size_t             capacity,
                             size_t             off)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);

	stroll_msg_init(msg, data, capacity, off, 0);
}

/**
 * Get the busy length.
 *
 * @param[in] msg stroll_msg.
 *
 * @return The busy length.
 */
static inline __stroll_nonull(1) __stroll_nothrow
size_t
stroll_msg_get_busy(struct stroll_msg *msg)
{
	stroll_msg_assert_msg_api(msg);

	return msg->busy_len;
}

/**
 * Get the current data ptr.
 *
 * @param[in] msg stroll_msg.
 *
 * @return The current data ptr.
 * @return NULL current header off is out of buffer.
 */
static inline __stroll_nonull(1) __stroll_nothrow
uint8_t *
stroll_msg_get_data(struct stroll_msg *msg)
{
	stroll_msg_assert_msg_api(msg);

	if (msg->head_off == msg->capacity)
		return NULL;

	return &msg->buffer[msg->head_off];
}

/**
 * Get the available head size.
 *
 * @param[in] msg stroll_msg.
 *
 * @return The available head size.
 */
static inline __stroll_nonull(1) __stroll_nothrow
size_t
stroll_msg_get_available_head(struct stroll_msg *msg)
{
	stroll_msg_assert_msg_api(msg);

	return msg->head_off;
}

/**
 * Get the available tail size.
 *
 * @param[in] msg stroll_msg.
 *
 * @return The available tail size.
 */
static inline __stroll_nonull(1) __stroll_nothrow
size_t
stroll_msg_get_available_tail(struct stroll_msg *msg)
{
	stroll_msg_assert_msg_api(msg);

	return msg->capacity - msg->head_off - msg->busy_len;
}

/**
 * Get the push data to the head.
 *
 * @param[in,out] msg stroll_msg.
 * @param[in]     len Data length to push.
 *
 * @return pointer to the data.
 * @return NULL if length upper than available head.
 */
static inline __stroll_nonull(1) __stroll_nothrow __warn_result
uint8_t *
stroll_msg_push_head(struct stroll_msg *msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);

	if (len > stroll_msg_get_available_head(msg))
		return NULL;

	msg->head_off -= len;
	msg->busy_len += len;
	return &msg->buffer[msg->head_off];
}

/**
 * Get the pull data from the head.
 *
 * @param[in,out] msg stroll_msg.
 * @param[in]     len Data length to push.
 *
 * @return pointer to the data.
 * @return NULL if length upper than busy length.
 */
static inline __stroll_nonull(1) __stroll_nothrow __warn_result
uint8_t *
stroll_msg_pull_head(struct stroll_msg *msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);

	uint8_t * head = &msg->buffer[msg->head_off];

	if (len > stroll_msg_get_busy(msg))
		return NULL;

	msg->head_off += len;
	msg->busy_len -= len;
	return head;
}

/**
 * Get the push data to the tail.
 *
 * @param[in,out] msg stroll_msg.
 * @param[in]     len Data length to push.
 *
 * @return pointer to the data.
 * @return NULL if length upper than available tail.
 */
static inline __stroll_nonull(1) __stroll_nothrow __warn_result
uint8_t *
stroll_msg_push_tail(struct stroll_msg *msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);

	uint8_t * tail = &msg->buffer[msg->head_off + msg->busy_len];

	if (len > stroll_msg_get_available_tail(msg))
		return NULL;

	msg->busy_len += len;
	return tail;
}

/**
 * Get the pull data from the tail.
 *
 * @param[in,out] msg stroll_msg.
 * @param[in]     len Data length to push.
 *
 * @return pointer to the data.
 * @return NULL if length upper than busy length.
 */
static inline __stroll_nonull(1) __stroll_nothrow __warn_result
uint8_t *
stroll_msg_pull_tail(struct stroll_msg *msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);

	if (len > stroll_msg_get_busy(msg))
		return NULL;

	msg->busy_len -= len;
	return &msg->buffer[msg->head_off + msg->busy_len];
}

#endif /* _STROLL_MESSAGE_H */
