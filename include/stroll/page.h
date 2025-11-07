/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * system memory page definition
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      07 Nov 2025
 * @copyright Copyright (C) 2017-2025 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_PAGE_H
#define _STROLL_PAGE_H

#include <stroll/cdefs.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include "stroll/assert.h"

#define stroll_page_assert_api(_expr) \
	stroll_assert("stroll:page", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_page_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

extern size_t _stroll_page_size;

/**
 * Return system memory page size.
 *
 * @return Page size
 */
static inline __stroll_pure __stroll_nothrow __warn_result
size_t
stroll_page_size(void)
{
	stroll_page_assert_api(_stroll_page_size > 0);

	return _stroll_page_size;
}

#endif /* _STROLL_PAGE_H */
