/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/bops.h"
#include "utest.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bops_fls32_assert)
{
	cute_expect_assertion(stroll_bops_fls32(0U));
}
#else
CUTE_TEST(strollut_bops_fls32_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bops_fls32)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		cute_check_uint(stroll_bops_fls32(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_fls32(word), equal, bit + 1);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bops_fls_assert)
{
	cute_expect_assertion(stroll_bops_fls(0U));
}
#else
CUTE_TEST(strollut_bops_fls_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bops_fls)
{
	unsigned int word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1U << bit;
		cute_check_uint(stroll_bops_fls(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_fls(word), equal, bit + 1);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bops_fls64_assert)
{
	cute_expect_assertion(stroll_bops_fls64(0U));
}
#else
CUTE_TEST(strollut_bops_fls64_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bops_fls64)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		cute_check_uint(stroll_bops_fls64(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_fls64(word), equal, bit + 1);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bops_flsul_assert)
{
	cute_expect_assertion(stroll_bops_fls(0UL));
}
#else
CUTE_TEST(strollut_bops_flsul_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bops_flsul)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		cute_check_uint(stroll_bops_flsul(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_flsul(word), equal, bit + 1);
	}
}

CUTE_TEST(strollut_bops_ffs32)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		cute_check_uint(stroll_bops_ffs32(word), equal, bit + 1);
	}

	word = UINT32_MAX;
	for (bit = 0; bit < 32; bit++) {
		cute_check_uint(stroll_bops_ffs32(word), equal, bit + 1);
		word <<= 1;
	}
}

CUTE_TEST(strollut_bops_ffs)
{
	unsigned int word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1U << bit;
		cute_check_uint(stroll_bops_ffs(word), equal, bit + 1);
	}

	word = UINT32_MAX;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		cute_check_uint(stroll_bops_ffs(word), equal, bit + 1);
		word <<= 1;
	}
}

CUTE_TEST(strollut_bops_ffs64)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		cute_check_uint(stroll_bops_ffs64(word), equal, bit + 1);
	}

	word = UINT64_MAX;
	for (bit = 0; bit < 64; bit++) {
		cute_check_uint(stroll_bops_ffs64(word), equal, bit + 1);
		word <<= 1;
	}
}

CUTE_TEST(strollut_bops_ffsul)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		cute_check_uint(stroll_bops_ffsul(word), equal, bit + 1);
	}

	word = ULONG_MAX;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		cute_check_uint(stroll_bops_ffsul(word), equal, bit + 1);
		word <<= 1;
	}
}

CUTE_TEST(strollut_bops_ffc32)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = ~(UINT32_C(1) << bit);
		cute_check_uint(stroll_bops_ffc32(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		cute_check_uint(stroll_bops_ffc32(word), equal, bit + 1);
		word = (word << 1) | 1;
	}
}

CUTE_TEST(strollut_bops_ffc)
{
	unsigned int word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = ~(1U << bit);
		cute_check_uint(stroll_bops_ffc(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		cute_check_uint(stroll_bops_ffc(word), equal, bit + 1);
		word = (word << 1) | 1;
	}
}

CUTE_TEST(strollut_bops_ffc64)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = ~(UINT64_C(1) << bit);
		cute_check_uint(stroll_bops_ffc64(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		cute_check_uint(stroll_bops_ffc64(word), equal, bit + 1);
		word = (word << 1) | 1;
	}
}

CUTE_TEST(strollut_bops_ffcul)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = ~(1UL << bit);
		cute_check_uint(stroll_bops_ffcul(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		cute_check_uint(stroll_bops_ffcul(word), equal, bit + 1);
		word = (word << 1) | 1;
	}
}

CUTE_TEST(strollut_bops_hweight32)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		cute_check_uint(stroll_bops_hweight32(word), equal, 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_hweight32(word), equal, bit + 1);
	}
}

CUTE_TEST(strollut_bops_hweight)
{
	unsigned int word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1U << bit;
		cute_check_uint(stroll_bops_hweight(word), equal, 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_hweight(word), equal, bit + 1);
	}
}

CUTE_TEST(strollut_bops_hweight64)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		cute_check_uint(stroll_bops_hweight64(word), equal, 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_hweight64(word), equal, bit + 1);
	}
}

CUTE_TEST(strollut_bops_hweightul)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		cute_check_uint(stroll_bops_hweightul(word), equal, 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_hweightul(word), equal, bit + 1);
	}
}

CUTE_GROUP(strollut_bops_group) = {
	CUTE_REF(strollut_bops_fls32_assert),
	CUTE_REF(strollut_bops_fls32),
	CUTE_REF(strollut_bops_fls_assert),
	CUTE_REF(strollut_bops_fls),
	CUTE_REF(strollut_bops_fls64_assert),
	CUTE_REF(strollut_bops_fls64),
	CUTE_REF(strollut_bops_flsul_assert),
	CUTE_REF(strollut_bops_flsul),

	CUTE_REF(strollut_bops_ffs32),
	CUTE_REF(strollut_bops_ffs),
	CUTE_REF(strollut_bops_ffs64),
	CUTE_REF(strollut_bops_ffsul),

	CUTE_REF(strollut_bops_ffc32),
	CUTE_REF(strollut_bops_ffc),
	CUTE_REF(strollut_bops_ffc64),
	CUTE_REF(strollut_bops_ffcul),

	CUTE_REF(strollut_bops_hweight32),
	CUTE_REF(strollut_bops_hweight),
	CUTE_REF(strollut_bops_hweight64),
	CUTE_REF(strollut_bops_hweightul),
};

CUTE_SUITE_EXTERN(strollut_bops_suite,
                  strollut_bops_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
