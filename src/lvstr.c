/**
 * @file
 * Length-Value String implementation
 *
 * @author       Grégor Boirie <gregor.boirie@free.fr>
 * @date         02 Apr 2023
 * @copyright    Copyright (C) 2017-2023 Grégor Boirie.
 * @licensestart GNU Lesser General Public License (LGPL) v3
 *
 * This file is part of libstroll
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, If not, see <http://www.gnu.org/licenses/>.
 * @licenseend
 */
#include "stroll/lvstr.h"
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#if defined(CONFIG_STROLL_ASSERT_INTERN)

#include <stroll/assert.h>

#define stroll_lvstr_assert_intern(_expr) \
	stroll_assert("stroll: lvstr", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_lvstr_assert_intern(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_INTERN) */

#define stroll_lvstr_assert_api_cstr(_lvstr, _cstr) \
	stroll_lvstr_assert_api(_lvstr); \
	stroll_lvstr_assert_api(_cstr)

#define stroll_lvstr_assert_intern_ncstr(_cstr, _len) \
	stroll_lvstr_assert_intern(_cstr); \
	stroll_lvstr_assert_intern((_len) <= STROLL_LVSTR_LEN_MAX); \
	stroll_lvstr_assert_intern(strnlen(_cstr, (_len) + 1) == (_len))

static ssize_t
stroll_lvstr_check_cstr(const char * cstr)
{
	stroll_lvstr_assert_intern(cstr);

	size_t len;

	len = strnlen(cstr, STROLL_LVSTR_LEN_MAX + 1);
	if (len >= STROLL_LVSTR_LEN_MAX)
		return -E2BIG;

	return (ssize_t)len;
}

static bool
stroll_lvstr_owns(const struct stroll_lvstr * lvstr)
{
	stroll_lvstr_assert_intern(lvstr);

	return !!(lvstr->cstr && (lvstr->len & 1U));
}

static void
stroll_lvstr_release(struct stroll_lvstr * lvstr)
{
	stroll_lvstr_assert_intern(lvstr);

	if (stroll_lvstr_owns(lvstr))
		free(lvstr->cstr);
}

static void
stroll_lvstr_set(struct stroll_lvstr * lvstr,
                 char                * cstr,
                 size_t                len,
                 size_t                owner)
{
	stroll_lvstr_assert_intern(lvstr);
	stroll_lvstr_assert_intern_ncstr(cstr, len);
	stroll_lvstr_assert_intern((owner == STROLL_LVSTR_LEASER) ||
	                           (owner == STROLL_LVSTR_OWNER));

	lvstr->len = (len << 1) | owner;
	lvstr->cstr = cstr;
}

void
stroll_lvstr_nlend(struct stroll_lvstr * lvstr, const char * cstr, size_t len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	stroll_lvstr_release(lvstr);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
	stroll_lvstr_set(lvstr, (char *)cstr, len, STROLL_LVSTR_LEASER);
#pragma GCC diagnostic pop
}

int
stroll_lvstr_lend(struct stroll_lvstr * lvstr, const char * cstr)
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
stroll_lvstr_ncede(struct stroll_lvstr * lvstr, char * cstr, size_t len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	stroll_lvstr_release(lvstr);
	stroll_lvstr_set(lvstr, cstr, len, STROLL_LVSTR_OWNER);
}

int
stroll_lvstr_cede(struct stroll_lvstr * lvstr, char * cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	stroll_lvstr_ncede(lvstr, cstr, (size_t)len);

	return 0;
}

static char *
stroll_lvstr_dup_cstr(const char * cstr, size_t len)
{
	stroll_lvstr_assert_intern_ncstr(cstr, len);

	char * str;

	str = malloc(len + 1);
	if (!str)
		return NULL;

	memcpy(str, cstr, len);
	str[len] = '\0';

	return str;
}

int
stroll_lvstr_ndup(struct stroll_lvstr * lvstr, const char * cstr, size_t len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	char * str;

	str = stroll_lvstr_dup_cstr(cstr, len);
	if (!str)
		return -errno;

	stroll_lvstr_ncede(lvstr, str, len);

	return 0;
}

int
stroll_lvstr_dup(struct stroll_lvstr * lvstr, const char * cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	return stroll_lvstr_ndup(lvstr, cstr, (size_t)len);
}

int
stroll_lvstr_init_lend(struct stroll_lvstr * lvstr, const char * cstr)
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
stroll_lvstr_init_cede(struct stroll_lvstr * lvstr, char * cstr)
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
stroll_lvstr_init_ndup(struct stroll_lvstr * lvstr,
                       const char          * cstr,
                       size_t                len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	char * str;

	str = stroll_lvstr_dup_cstr(cstr, len);
	if (!str)
		return -errno;

	stroll_lvstr_init_ncede(lvstr, str, (size_t)len);

	return 0;
}

int
stroll_lvstr_init_dup(struct stroll_lvstr * lvstr, const char * cstr)
{
	stroll_lvstr_assert_api_cstr(lvstr, cstr);

	ssize_t len;

	len = stroll_lvstr_check_cstr(cstr);
	if (len < 0)
		return (int)len;

	return stroll_lvstr_init_ndup(lvstr, cstr, (size_t)len);
}

void
stroll_lvstr_fini(struct stroll_lvstr * lvstr)
{
	stroll_lvstr_assert_api(lvstr);

	stroll_lvstr_release(lvstr);
}
