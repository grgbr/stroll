/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/page.h"
#include <unistd.h>
#include <stdint.h>

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_page_assert_intern(_expr) \
	stroll_assert("stroll:page", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_page_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

size_t _stroll_page_size = 0;

static __ctor(101) __stroll_nothrow
void
stroll_page_init(void)
{
	long sz;

	sz = sysconf(_SC_PAGESIZE);
	stroll_page_assert_intern(sz > 0);
	stroll_page_assert_intern((size_t)sz <= SIZE_MAX);

	_stroll_page_size = (size_t)sz;
}
