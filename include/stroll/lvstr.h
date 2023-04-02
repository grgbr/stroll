/**
 * @file
 * Length-Value String interface
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
#ifndef _STROLL_LVSTR_H
#define _STROLL_LVSTR_H

#include <stroll/cdefs.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_lvstr_assert_api(_expr) \
	stroll_assert("stroll: lvstr", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_lvstr_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_lvstr_assert_api_ncstr(_lvstr, _cstr, _len) \
	stroll_lvstr_assert_api(_lvstr); \
	stroll_lvstr_assert_api(_cstr); \
	stroll_lvstr_assert_api((_len) <= STROLL_LVSTR_LEN_MAX); \
	stroll_lvstr_assert_api(strnlen(_cstr, (_len) + 1) == (_len))

#define STROLL_LVSTR_LEASER  (0U)
#define STROLL_LVSTR_OWNER   (1U)

#define STROLL_LVSTR_LEN_MAX (SIZE_MAX >> 1)

struct stroll_lvstr {
	size_t   len;
	char   * cstr;
};

#define STROLL_LVSTR_INIT \
	{ .cstr = NULL }

#define STROLL_LVSTR_INIT_NLEND(_cstr, _len) \
	{ .len = (_len) << 1 | STROLL_LVSTR_LEASER, .cstr = _cstr }

#define STROLL_LVSTR_INIT_NCEDE(_cstr, _len) \
	{ .len = (_len) << 1 | STROLL_LVSTR_OWNER, .cstr = _cstr }

static inline size_t
stroll_lvstr_len(const struct stroll_lvstr * lvstr)
{
	stroll_lvstr_assert_api(lvstr);

	return lvstr->len >> 1;
}

static inline const char *
stroll_lvstr_cstr(const struct stroll_lvstr * lvstr)
{
	stroll_lvstr_assert_api(lvstr);

	return lvstr->cstr;
}

extern void
stroll_lvstr_nlend(struct stroll_lvstr * lvstr, const char * cstr, size_t len);

extern int
stroll_lvstr_lend(struct stroll_lvstr * lvstr, const char * cstr);

extern void
stroll_lvstr_ncede(struct stroll_lvstr * lvstr, char * cstr, size_t len);

extern int
stroll_lvstr_cede(struct stroll_lvstr * lvstr, char * cstr);

extern int
stroll_lvstr_ndup(struct stroll_lvstr * lvstr, const char * cstr, size_t len);

extern int
stroll_lvstr_dup(struct stroll_lvstr * lvstr, const char * cstr);

static inline void
stroll_lvstr_init_nlend(struct stroll_lvstr * lvstr,
                        const char          * cstr,
                        size_t                len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	lvstr->len = len << 1 | STROLL_LVSTR_LEASER;
	lvstr->cstr = (char *)cstr;
}

extern int
stroll_lvstr_init_lend(struct stroll_lvstr * lvstr, const char * cstr);

static inline void
stroll_lvstr_init_ncede(struct stroll_lvstr * lvstr, char * cstr, size_t len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	lvstr->len = (len << 1) | STROLL_LVSTR_OWNER;
	lvstr->cstr = cstr;
}

extern int
stroll_lvstr_init_cede(struct stroll_lvstr * lvstr, char * cstr);

extern int
stroll_lvstr_init_ndup(struct stroll_lvstr * lvstr,
                       const char          * cstr,
                       size_t                len);

extern int
stroll_lvstr_init_dup(struct stroll_lvstr * lvstr, const char * cstr);

static inline void
stroll_lvstr_init(struct stroll_lvstr * lvstr)
{
	lvstr->cstr = NULL;
}

extern void
stroll_lvstr_fini(struct stroll_lvstr * lvstr);

#endif /* _STROLL_LVSTR_H */
