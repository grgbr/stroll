/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Loïc Jourdheuil Sellin
 ******************************************************************************/

/**
 * @file
 * High-level buffer / message iterator.
 *
 * @author    Loïc Jourdheuil Sellin
 * @author    Grégor Boirie <gregor.boirie@free.fr>
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
 * A high-level buffered message iterator that may be used to produce to
 * and / or consume from an arbitrary fixed sized contiguous data memory block.
 * Data production and consumption may happen at both front (head) and rear
 * (tail) ends.
 *
 * @see
 * - STROLL_MSG_INIT()
 * - STROLL_MSG_INIT_EMPTY()
 * - STROLL_MSG_INIT_WITH_BUSY()
 * - STROLL_MSG_INIT_WITH_RESERVE()
 * - stroll_msg_setup()
 * - stroll_msg_setup_empty()
 * - stroll_msg_setup_with_busy()
 * - stroll_msg_setup_with_reserve()
 * - stroll_msg_get_busy()
 * - stroll_msg_get_data()
 * - stroll_msg_get_avail_head()
 * - stroll_msg_push_head()
 * - stroll_msg_pull_head()
 * - stroll_msg_get_avail_tail()
 * - stroll_msg_get_tail()
 * - stroll_msg_push_tail()
 * - stroll_msg_pull_tail()
 */
struct stroll_msg {
	/**
	 * @internal
	 *
	 * Internal buffer state.
	 */
	struct stroll_buff buff;

	/**
	 * @internal
	 *
	 * Pointer to user defined data area.
	 */
	uint8_t *          data;
};

#define stroll_msg_assert_msg_api(_msg) \
	stroll_msg_assert_api(_msg); \
	stroll_buff_assert_head_api(&(_msg)->buff); \
	stroll_msg_assert_api((_msg)->data)

/**
 * Message constant initializer.
 *
 * @param _data     Pointer to message contiguous memory block
 * @param _capacity @p _data memory area size
 * @param _off      Initial offset to head of user data within @p _data
 * @param _len      Initial size of user data within @p _data
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
 * Initialize a message with an empty data memory block.
 *
 * @param _data     Pointer to message contiguous memory block
 * @param _capacity @p _data memory area size
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
 * Initialize a message with a non-empty data memory block.
 *
 * @param _data     Pointer to message contiguous memory block
 * @param _capacity @p _data memory area size
 * @param _len      Initial size of user data within @p _data
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
 * Initialize a message with an empty data memory block and an initial head
 * offset.
 *
 * @param _data     Pointer to message contiguous memory block
 * @param _capacity @p _data memory area size
 * @param _off      Initial offset to head of user data within @p _data
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
 * Dynamically initialize a message.
 *
 * @param[out] msg      Message to initialize
 * @param[in]  data     Pointer to message contiguous memory block
 * @param[in]  capacity @p data memory area size
 * @param[in]  off      Initial offset to head of user data within @p data
 * @param[in]  len      Initial size of user data within @p data
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
 * Dynamically initialize a message with an empty data memory block.
 *
 * @param[out] msg      Message to initialize
 * @param[in]  data     Pointer to message contiguous memory block
 * @param[in]  capacity @p data memory area size
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
 * Dynamically initialize a message with a non-empty data memory block.
 *
 * @param[out] msg      Message to initialize
 * @param[in]  data     Pointer to message contiguous memory block
 * @param[in]  capacity @p data memory area size
 * @param[in]  len      Initial size of user data within @p data
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
 * Dynamically initialize a message with an empty data memory block and an
 * initial head offset.
 *
 * @param[out] msg      Message to initialize
 * @param[in]  data     Pointer to message contiguous memory block
 * @param[in]  capacity @p data memory area size
 * @param[in]  off      Initial offset to head of user data within @p data
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
 * Return overall storage capacity of a message.
 *
 * @param[in] msg Message to retrieve informations from
 *
 * @return Capacity in bytes
 *
 * @see
 * - stroll_msg_get_busy()
 * - stroll_msg_get_avail_head()
 * - stroll_msg_get_avail_tail()
 * - stroll_msg
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_msg_get_capacity(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	return stroll_buff_capacity(&msg->buff);
}

/**
 * Return size of user data stored into a message.
 *
 * @param[in] msg Message to retrieve informations from
 *
 * @return Size of user data.
 *
 * @see
 * - stroll_msg_get_data()
 * - stroll_msg_get_capacity()
 * - stroll_msg_get_avail_head()
 * - stroll_msg_get_avail_tail()
 * - stroll_msg
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_msg_get_busy(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	return stroll_buff_busy(&msg->buff);
}

/**
 * Return a pointer to start of user data stored into message.
 *
 * @param[in] msg Message to retrieve informations from
 *
 * @return Pointer to start of data or `NULL` if no data stored.
 *
 * @see
 * - stroll_msg_get_busy()
 * - stroll_msg_get_tail()
 * - stroll_msg
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
 * Return offset to start of user data of a message.
 *
 * @param[in] msg Message to retrieve informations from
 *
 * @return Size of (unused) area located ahead of user data.
 *
 * @see
 * - stroll_msg_push_head()
 * - stroll_msg_pull_head()
 * - stroll_msg_get_busy()
 * - stroll_msg_get_capacity()
 * - stroll_msg_get_avail_tail()
 * - stroll_msg
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_msg_get_avail_head(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	return stroll_buff_avail_head(&msg->buff);
}

/**
 * Reserve memory space ahead of message user data.
 *
 * @param[inout] msg Message to push data into.
 * @param[in]    len Size of data to push.
 *
 * As a consequence, the memory area available for storing additional data ahead
 * of user data stored into @p msg decreases by an amount of @p len bytes.
 *
 * @return Pointer to new user data start (head) or `NULL` if memory available
 *         ahead of user data is not large enough.
 *
 * @see
 * - stroll_msg_get_avail_head()
 * - stroll_msg_pull_head()
 * - stroll_msg_push_tail()
 * - stroll_msg
 */
extern uint8_t *
stroll_msg_push_head(struct stroll_msg * __restrict msg, size_t len)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

/**
 * Hand over memory space ahead of message user data.
 *
 * @param[inout] msg Message to pull data from.
 * @param[in]    len Size of data to pull.
 *
 * As a consequence, the memory area available for storing additional data ahead
 * of user data stored into @p msg increases by an amount of @p len bytes.
 *
 * @return Pointer to the original start of data or `NULL` if stored user data
 *         is not large enough.
 *
 * @see
 * - stroll_msg_get_busy()
 * - stroll_msg_get_data()
 * - stroll_msg_push_head()
 * - stroll_msg_pull_tail()
 * - stroll_msg
 */
extern uint8_t *
stroll_msg_pull_head(struct stroll_msg * __restrict msg, size_t len)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

/**
 * Return size of (unused) area located right after user data.
 *
 * @param[in] msg Message to retrieve informations from
 *
 * @return Size of (unused) area located at tail of user data.
 *
 * @see
 * - stroll_msg_get_avail_head()
 * - stroll_msg_get_tail()
 * - stroll_msg_get_busy()
 * - stroll_msg_get_capacity()
 * - stroll_msg
 */
static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_msg_get_avail_tail(const struct stroll_msg * __restrict msg)
{
	stroll_msg_assert_msg_api(msg);

	return stroll_buff_avail_tail(&msg->buff);
}

/**
 * Return a pointer to (unused) area located right after user data.
 *
 * @param[in] msg Message to retrieve informations from
 *
 * @return Pointer to (unused) area located at tail of user data or `NULL` if
 *         there is no more available tail space.
 *
 * @see
 * - stroll_msg_get_avail_tail()
 * - stroll_msg_push_tail()
 * - stroll_msg_pull_tail()
 * - stroll_msg_get_data()
 * - stroll_msg
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
 * Reserve memory space right after user data.
 *
 * @param[inout] msg Message to push data into.
 * @param[in]    len Size of data to push.
 *
 * As a consequence, the memory area available for storing additional data at
 * the tail of user data stored into @p msg decreases by an amount of @p len
 * bytes.
 *
 * @return Pointer to area located at the tail of user data or `NULL` if there
 *         is no more available tail space.
 *
 * @see
 * - stroll_msg_get_avail_tail()
 * - stroll_msg_pull_tail()
 * - stroll_msg_get_tail()
 * - stroll_msg_push_head()
 * - stroll_msg
 */
extern uint8_t *
stroll_msg_push_tail(struct stroll_msg * __restrict msg, size_t len)
	__stroll_nonull(1) __stroll_nothrow __leaf __warn_result;

/**
 * Hand over memory space from the tail of message user data.
 *
 * @param[inout] msg Message to pull data from.
 * @param[in]    len Size of data to pull.
 *
 * As a consequence, the memory area available for storing additional data at
 * the tail of user data stored into @p msg increases by an amount of @p len
 * bytes.
 *
 * @return Pointer to newly (unused) area located at tail of user data or `NULL`
 *         if stored user data is not large enough.
 *
 * @see
 * - stroll_msg_get_busy()
 * - stroll_msg_get_tail()
 * - stroll_msg_push_tail()
 * - stroll_msg_pull_head()
 * - stroll_msg
 */
extern uint8_t *
stroll_msg_pull_tail(struct stroll_msg * __restrict msg, size_t len)
	__stroll_nonull(1) __stroll_nothrow __warn_result;

#endif /* _STROLL_MESSAGE_H */
