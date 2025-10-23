/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Loïc Jourdheuil Sellin
 ******************************************************************************/

/**
 * @file
 * Low-level buffer management
 *
 * @author    Loïc Jourdheuil Sellin
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      20 Oct 2025
 * @copyright Copyright (C) 2017-2023 Loïc Jourdheuil Sellin.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_BUFFER_H
#define _STROLL_BUFFER_H

#include <stroll/cdefs.h>
#include <stdint.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_buff_assert_api(_expr) \
	stroll_assert("stroll:buffer", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_buff_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

/**
 * Buffer state.
 *
 * Holds internal state of a data buffer that may be used to drive producing to
 * and / or consumming from an arbitrary contiguous data memory block.
 */
struct stroll_buff {
	/**
	 * @internal
	 *
	 * offset starting data.
	 */
	size_t head_off;

	/**
	 * @internal
	 *
	 * length of busy data.
	 */
	size_t busy_len;

	/**
	 * @internal
	 *
	 * data capacity.
	 */
	size_t capacity;
};

#define STROLL_BUFF_CAPACITY_MIN (8U)

#define stroll_buff_assert_head_api(_buff) \
	stroll_buff_assert_api(_buff); \
	stroll_buff_assert_api((_buff)->capacity >= \
	                         STROLL_BUFF_CAPACITY_MIN); \
	stroll_buff_assert_api((_buff)->capacity <= \
	                         CONFIG_STROLL_BUFF_CAPACITY_MAX); \
	stroll_buff_assert_api((_buff)->head_off <= (_buff)->capacity); \
	stroll_buff_assert_api((_buff)->busy_len <= (_buff)->capacity); \
	stroll_buff_assert_api(((_buff)->head_off + (_buff)->busy_len) <= \
	                         (_buff)->capacity);

#define STROLL_BUFF_INIT(_capacity, _off, _len) \
	{ \
		.capacity = compile_eval( \
			(_capacity) <= CONFIG_STROLL_BUFF_CAPACITY_MAX, \
			_capacity, \
			"Capacity MUST be <= CONFIG_STROLL_BUFF_CAPACITY_MAX"), \
		.head_off = compile_eval((_off) <= (_capacity), \
		                         _off, \
		                         "Offset MUST be <= capacity"), \
		.busy_len = compile_eval( \
			(_len) <= (_capacity), \
			compile_eval(((_off) + (_len)) <= (_capacity), \
			             _len, \
			             "Length + offset MUST be <= capacity"), \
			             "Length MUST be <= Capacity"), \
	}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_buff_setup(struct stroll_buff * __restrict buffer,
                  size_t                          capacity,
                  size_t                          off,
                  size_t                          len)
{
	stroll_buff_assert_api(buffer);
	stroll_buff_assert_api(capacity <= CONFIG_STROLL_BUFF_CAPACITY_MAX);
	stroll_buff_assert_api(off <= capacity);
	stroll_buff_assert_api(len <= capacity);
	stroll_buff_assert_api((off + len) <= capacity);

	buffer->head_off = off;
	buffer->busy_len = len;
	buffer->capacity = capacity;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_buff_capacity(const struct stroll_buff * __restrict buffer)
{
	stroll_buff_assert_head_api(buffer);

	return buffer->capacity;
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_buff_busy(const struct stroll_buff * __restrict buffer)
{
	stroll_buff_assert_head_api(buffer);

	return buffer->busy_len;
}

static inline __stroll_nonull(1, 2) __stroll_pure __stroll_nothrow __warn_result
uint8_t *
stroll_buff_data(const struct stroll_buff * __restrict buffer,
                 const uint8_t * __restrict            base)
{
	stroll_buff_assert_head_api(buffer);
	stroll_buff_assert_api(base);
	stroll_buff_assert_api(buffer->head_off < buffer->capacity);

STROLL_IGNORE_WARN("-Wcast-qual")
	return (uint8_t *)&base[buffer->head_off];
STROLL_RESTORE_WARN
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_buff_avail_head(const struct stroll_buff * __restrict buffer)
{
	stroll_buff_assert_head_api(buffer);

	return buffer->head_off;
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_buff_grow_head(struct stroll_buff * __restrict buffer,
                      size_t                          bytes)
{
	stroll_buff_assert_head_api(buffer);
	stroll_buff_assert_api(bytes <= buffer->busy_len);

	buffer->head_off += bytes;
	buffer->busy_len -= bytes;
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_buff_shrink_head(struct stroll_buff * __restrict buffer,
                        size_t                          bytes)
{
	stroll_buff_assert_head_api(buffer);
	stroll_buff_assert_api(bytes <= buffer->head_off);

	buffer->head_off -= bytes;
	buffer->busy_len += bytes;
}

static inline __stroll_nonull(1, 2) __stroll_pure __stroll_nothrow __warn_result
uint8_t *
stroll_buff_head(const struct stroll_buff * __restrict buffer,
                 const uint8_t * __restrict            base,
                 size_t                                bytes)
{
	stroll_buff_assert_head_api(buffer);
	stroll_buff_assert_api(base);
	stroll_buff_assert_api(bytes <= buffer->head_off);

STROLL_IGNORE_WARN("-Wcast-qual")
	return (uint8_t *)&base[buffer->head_off - bytes];
STROLL_RESTORE_WARN
}

static inline __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_buff_avail_tail(const struct stroll_buff * __restrict buffer)
{
	stroll_buff_assert_head_api(buffer);

	return buffer->capacity - (buffer->head_off + buffer->busy_len);
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_buff_grow_tail(struct stroll_buff * __restrict buffer,
                      size_t                          bytes)
{
	stroll_buff_assert_head_api(buffer);
	stroll_buff_assert_api((buffer->head_off + buffer->busy_len + bytes)
	                         <= buffer->capacity);

	buffer->busy_len += bytes;
}

static inline __stroll_nonull(1) __stroll_nothrow
void
stroll_buff_shrink_tail(struct stroll_buff * __restrict buffer,
                        size_t                          bytes)
{
	stroll_buff_assert_head_api(buffer);
	stroll_buff_assert_api(bytes <= buffer->busy_len);

	buffer->busy_len -= bytes;
}

static inline __stroll_nonull(1, 2) __stroll_pure __stroll_nothrow __warn_result
uint8_t *
stroll_buff_tail(const struct stroll_buff * __restrict buffer,
                 const uint8_t * __restrict            base)
{
	stroll_buff_assert_head_api(buffer);
	stroll_buff_assert_api((buffer->head_off + buffer->busy_len) <
	                         buffer->capacity);

STROLL_IGNORE_WARN("-Wcast-qual")
	return (uint8_t *)&base[buffer->head_off + buffer->busy_len];
STROLL_RESTORE_WARN
}

#endif /* _STROLL_BUFFER_H */
