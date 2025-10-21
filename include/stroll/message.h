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

#include <stroll/buffer.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_msg_assert_api(_expr) \
	stroll_assert("stroll:message", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_msg_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Message iterator.
 *
 * Describes a message iterator than can push and pull data from the head and
 * the tail.
 */
struct stroll_msg {
	struct stroll_buff buff;
	uint8_t *          data;
};

#define stroll_msg_assert_msg_api(_msg) \
	stroll_msg_assert_api(_msg); \
	stroll_buff_assert_head_api(&(_msg)->buff); \
	stroll_msg_assert_api((_msg)->data)

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
 * - stroll_msg_setup()
 * - stroll_msg_setup_empty()
 * - stroll_msg_setup_with_busy()
 * - stroll_msg_setup_with_reserve()
 */
#define STROLL_MSG_INIT(_data, _capacity, _off, _len) \
	{ \
		.buff = STROLL_BUFF_INIT(_capacity, _off, _len), \
		.data = compile_eval(_data != NULL, \
		                     _data, \
		                     "Invalid message data"), \
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
 * - stroll_msg_setup()
 * - stroll_msg_setup_empty()
 * - stroll_msg_setup_with_busy()
 * - stroll_msg_setup_with_reserve()
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
 * - stroll_msg_setup()
 * - stroll_msg_setup_empty()
 * - stroll_msg_setup_with_busy()
 * - stroll_msg_setup_with_reserve()
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
 * - stroll_msg_setup()
 * - stroll_msg_setup_empty()
 * - stroll_msg_setup_with_busy()
 * - stroll_msg_setup_with_reserve()
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
 * - stroll_msg_setup_empty()
 * - stroll_msg_setup_with_busy()
 * - stroll_msg_setup_with_reserve()
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_setup(struct stroll_msg * __restrict msg,
                 uint8_t * __restrict           data,
                 size_t                         capacity,
                 size_t                         off,
                 size_t                         len)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);

	stroll_buff_setup(&msg->buff, capacity, off, len);
	msg->data   = data;
}

/**
 * Initialize a stroll_message with empty data.
 *
 * @param[out] msg      stroll_msg to initialize.
 * @param[in]  data     stroll_msg buffer.
 * @param[in]  capacity stroll_msg capacity.
 *
 * @see
 * - stroll_msg_setup()
 * - stroll_msg_setup_with_busy()
 * - stroll_msg_setup_with_reserve()
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_setup_empty(struct stroll_msg * __restrict msg,
                       uint8_t * __restrict           data,
                       size_t                         capacity)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);

	stroll_msg_setup(msg, data, capacity, 0, 0);
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
 * - stroll_msg_setup()
 * - stroll_msg_setup_empty()
 * - stroll_msg_setup_with_reserve()
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_setup_with_busy(struct stroll_msg * __restrict msg,
                          uint8_t * __restrict            data,
                          size_t                          capacity,
                          size_t                          len)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);

	stroll_msg_setup(msg, data, capacity, 0, len);
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
 * - stroll_msg_setup()
 * - stroll_msg_setup_empty()
 * - stroll_msg_setup_with_busy()
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 */
static inline __stroll_nonull(1, 2) __stroll_nothrow
void
stroll_msg_setup_with_reserve(struct stroll_msg * __restrict msg,
                              uint8_t * __restrict           data,
                              size_t                         capacity,
                              size_t                         off)
{
	stroll_msg_assert_api(msg);
	stroll_msg_assert_api(data);

	stroll_msg_setup(msg, data, capacity, off, 0);
}

/**
 * Get the busy length.
 *
 * @param[in] msg stroll_msg.
 *
 * @return The busy length.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_msg_get_busy(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	return stroll_buff_busy(&msg->buff);
}

/**
 * Return pointer to start of data stored into message.
 *
 * @param[in] msg stroll_msg.
 *
 * @return Pointer to start of data or `NULL` if no data stored.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
uint8_t *
stroll_msg_get_data(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	if (!stroll_buff_busy(&msg->buff))
		return NULL;

	return stroll_buff_data(&msg->buff, msg->data);
}

/**
 * Get the available head size.
 *
 * @param[in] msg stroll_msg.
 *
 * @return The available head size.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_msg_get_available_head(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	return stroll_buff_avail_head(&msg->buff);
}

/**
 * Push data ahead of buffer.
 *
 * @param[in,out] msg stroll_msg.
 * @param[in]     len Data length to push.
 *
 * @return pointer to the data.
 * @return NULL if length upper than available head.
 */
extern uint8_t *
stroll_msg_push_head(struct stroll_msg * __restrict msg, size_t len)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

/**
 * Consume data from buffer head.
 *
 * @param[in,out] msg Message
 * @param[in]     len Data length to pull.
 *
 * @return Pointer to the original start of data or `NULL` if not enough stored
 *         data.
 */
extern uint8_t *
stroll_msg_pull_head(struct stroll_msg * __restrict msg, size_t len)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

/**
 * Get the available tail size.
 *
 * @param[in] msg stroll_msg.
 *
 * @return The available tail size.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_msg_get_available_tail(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	return stroll_buff_avail_tail(&msg->buff);
}

/**
 * Return pointer to tail of data stored into message.
 *
 * @param[in] msg stroll_msg.
 *
 * @return Pointer to tail of data or `NULL` if no more available tail space.
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
uint8_t *
stroll_msg_get_tail(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	if (!stroll_buff_avail_tail(&msg->buff))
		return NULL;

	return stroll_buff_tail(&msg->buff, msg->data);
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
extern uint8_t *
stroll_msg_push_tail(struct stroll_msg * __restrict msg, size_t len)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

/**
 * Get the pull data from the tail.
 *
 * @param[in,out] msg stroll_msg.
 * @param[in]     len Data length to push.
 *
 * @return pointer to the data.
 * @return NULL if length upper than busy length.
 */
extern uint8_t *
stroll_msg_pull_tail(struct stroll_msg * __restrict msg, size_t len)
	__stroll_nonull(1) __stroll_nothrow __warn_result;

#endif /* _STROLL_MESSAGE_H */
