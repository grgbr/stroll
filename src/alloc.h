/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _STROLL_INTERN_ALLOC_H
#define _STROLL_INTERN_ALLOC_H

#include "stroll/alloc.h"

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include "stroll/assert.h"

#define stroll_alloc_assert_intern(_expr) \
	stroll_assert("stroll:alloc", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_alloc_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_alloc_assert_ops_intern(_ops) \
	stroll_alloc_assert_intern(_ops); \
	stroll_alloc_assert_intern((_ops)->alloc); \
	stroll_alloc_assert_intern((_ops)->free); \
	stroll_alloc_assert_intern((_ops)->fini)

#endif /* _STROLL_INTERN_ALLOC_H */
