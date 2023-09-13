/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/lvstr.h"
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_lvstr_assert_intern(_expr) \
	stroll_assert("stroll:lvstr", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_lvstr_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_lvstr_assert_api_cstr(_lvstr, _cstr) \
	stroll_lvstr_assert_api(_lvstr); \
	stroll_lvstr_assert_api(_cstr)

static ssize_t __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
stroll_lvstr_check_cstr(const char * cstr)
{
	stroll_lvstr_assert_intern(cstr);

	size_t len;

	/*
	 * Warning, Glibc based GCC limits string size argument given to
	 * strnlen() to PTRDIFF_MAX. Hence STROLL_LVSTR_LEN_MAX MUST be <
	 * PTRDIFF_MAX to prevent GCC from emitting a Wstringop-overread /
	 * Wstringop-overflow warning:
	 *     'strnlen' specified bound ... exceeds maximum object size ...
	 *
	 * In addition, len MUST be <= (SIZE_MAX >> 1) since stored into struct
	 * stroll_lvstr::len with first bit being used to store
	 * stroll_lvstr::cstr ownership.
	 *
	 * Finally, as we return a ssize_t, it MUST also be <= SSIZE_MAX.
	 *
	 * See how STROLL_LVSTR_LEN_MAX is defined into lvstr.h.
	 */
	len = strnlen(cstr, STROLL_LVSTR_LEN_MAX + 1);
	if (len > STROLL_LVSTR_LEN_MAX)
		return -E2BIG;

	return (ssize_t)len;
}

static bool __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
stroll_lvstr_owns(const struct stroll_lvstr * __restrict lvstr)
{
	stroll_lvstr_assert_intern(lvstr);

	return !!(lvstr->rostr && (lvstr->len & 1UL));
}

static void __stroll_nonull(1) __stroll_nothrow
stroll_lvstr_release(struct stroll_lvstr * __restrict lvstr)
{
	stroll_lvstr_assert_intern(lvstr);

	if (stroll_lvstr_owns(lvstr))
		free(lvstr->rwstr);
}

void
stroll_lvstr_drop(struct stroll_lvstr * __restrict lvstr)
{
	stroll_lvstr_assert_api(lvstr);

	stroll_lvstr_release(lvstr);
	lvstr->rostr = NULL;
}

void
stroll_lvstr_nlend(struct stroll_lvstr * __restrict lvstr,
                   const char *                     cstr,
                   size_t                           len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	stroll_lvstr_release(lvstr);

	lvstr->len = (len << 1) | STROLL_LVSTR_LEASER;
	lvstr->rostr = cstr;
}

int
stroll_lvstr_lend(struct stroll_lvstr * __restrict lvstr, const char * cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	stroll_lvstr_nlend(lvstr, cstr, (size_t)len);

	return 0;
}

void
stroll_lvstr_ncede(struct stroll_lvstr * __restrict lvstr,
                   char *                           cstr,
                   size_t                           len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	stroll_lvstr_release(lvstr);

	lvstr->len = (len << 1) | STROLL_LVSTR_OWNER;
	lvstr->rwstr = cstr;
}

int
stroll_lvstr_cede(struct stroll_lvstr * __restrict lvstr, char * cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	stroll_lvstr_ncede(lvstr, cstr, (size_t)len);

	return 0;
}

static char * __stroll_nonull(1) __stroll_nothrow __warn_result
stroll_lvstr_dup_cstr(const char * cstr, size_t len)
{
	stroll_lvstr_assert_intern(cstr);
	stroll_lvstr_assert_intern(len <= STROLL_LVSTR_LEN_MAX);
	stroll_lvstr_assert_intern(strnlen(cstr, STROLL_LVSTR_LEN_MAX) >= len);

	char * str;

	str = malloc(len + 1);
	if (!str)
		return NULL;

	memcpy(str, cstr, len);
	str[len] = '\0';

	return str;
}

int
stroll_lvstr_ndup(struct stroll_lvstr * __restrict lvstr,
                  const char *                     cstr,
                  size_t                           len)
{
	stroll_lvstr_assert_api(lvstr);
	stroll_lvstr_assert_api(cstr);
	stroll_lvstr_assert_api(len <= STROLL_LVSTR_LEN_MAX);
	stroll_lvstr_assert_api(strnlen(cstr, STROLL_LVSTR_LEN_MAX) >= len);

	char * str;

	str = stroll_lvstr_dup_cstr(cstr, len);
	if (!str)
		return -errno;

	stroll_lvstr_ncede(lvstr, str, len);

	return 0;
}

int
stroll_lvstr_dup(struct stroll_lvstr * __restrict lvstr, const char * cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	return stroll_lvstr_ndup(lvstr, cstr, (size_t)len);
}

int
stroll_lvstr_init_lend(struct stroll_lvstr * __restrict lvstr,
                       const char *                     cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	stroll_lvstr_init_nlend(lvstr, cstr, (size_t)len);

	return 0;
}

int
stroll_lvstr_init_cede(struct stroll_lvstr * __restrict lvstr, char * cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	stroll_lvstr_init_ncede(lvstr, cstr, (size_t)len);

	return 0;
}

int
stroll_lvstr_init_ndup(struct stroll_lvstr * __restrict lvstr,
                       const char *                     cstr,
                       size_t                           len)
{
	stroll_lvstr_assert_api(lvstr);
	stroll_lvstr_assert_api(cstr);
	stroll_lvstr_assert_api(len <= STROLL_LVSTR_LEN_MAX);
	stroll_lvstr_assert_api(strnlen(cstr, STROLL_LVSTR_LEN_MAX) >= len);

	char * str;

	str = stroll_lvstr_dup_cstr(cstr, len);
	if (!str)
		return -errno;

	stroll_lvstr_init_ncede(lvstr, str, (size_t)len);

	return 0;
}

int
stroll_lvstr_init_dup(struct stroll_lvstr * __restrict lvstr,
                      const char *                     cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	return stroll_lvstr_init_ndup(lvstr, cstr, (size_t)len);
}

void
stroll_lvstr_fini(struct stroll_lvstr * __restrict lvstr)
{
	stroll_lvstr_assert_api(lvstr);

	stroll_lvstr_release(lvstr);
}
