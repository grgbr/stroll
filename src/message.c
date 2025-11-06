/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Loïc Jourdheuil Sellin
 ******************************************************************************/

#include "stroll/message.h"

uint8_t *
stroll_msg_push_head(struct stroll_msg * __restrict msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);

	if (len && (len <= stroll_buff_avail_head(&msg->buff))) {
		stroll_buff_shrink_head(&msg->buff, len);

		return stroll_buff_data(&msg->buff, msg->mem);
	}

	return NULL;
}

uint8_t *
stroll_msg_pull_head(struct stroll_msg * __restrict msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);

	if (len && (len <= stroll_buff_busy(&msg->buff))) {
		uint8_t * data = stroll_buff_data(&msg->buff, msg->mem);

		stroll_buff_grow_head(&msg->buff, len);

		return data;
	}

	return NULL;
}

uint8_t *
stroll_msg_push_tail(struct stroll_msg * __restrict msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);

	if (len && (len <= stroll_buff_avail_tail(&msg->buff))) {
		uint8_t * tail = stroll_buff_tail(&msg->buff, msg->mem);

		stroll_buff_grow_tail(&msg->buff, len);

		return tail;
	}

	return NULL;
}

uint8_t *
stroll_msg_pull_tail(struct stroll_msg * __restrict msg, size_t len)
{
	stroll_msg_assert_msg_api(msg);

	if (len && (len <= stroll_buff_busy(&msg->buff))) {
		stroll_buff_shrink_tail(&msg->buff, len);

		return stroll_buff_tail(&msg->buff, msg->mem);
	}

	return NULL;
}
