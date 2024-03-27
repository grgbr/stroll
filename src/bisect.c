/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include <stroll/bisect.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_bisect_assert_api(_expr) \
	stroll_assert("stroll:bisect", _expr)

#else  /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_bisect_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

void *
stroll_bisect_search(const void *    key,
                     const void *    array,
                     size_t          size,
                     unsigned int    nr,
                     stroll_cmp_fn * compare)
{
	stroll_bisect_assert_api(array);
	stroll_bisect_assert_api(size);
	stroll_bisect_assert_api(nr);
	stroll_bisect_assert_api(compare);

	size_t min = 0;
	size_t max = nr;

	do {
		size_t       mid = (min + max) / 2;
		const void * elm = (const char *)array + (mid * size);
		int          cmp;

		cmp = compare(key, elm);
		if (cmp < 0)
			max = mid;
		else if (cmp > 0)
			min = mid + 1;
		else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
			return (void *)elm;
#pragma GCC diagnostic pop
		}
	} while (min < max);

	return NULL;
}
