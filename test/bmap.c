/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "utest.h"
#include "stroll/bmap.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <stdlib.h>

/******************************************************************************
 * Common utils
 ******************************************************************************/

static void * strollut_bmap_xpct;

static void
strollut_bmap_teardown(void)
{
	cute_check_ptr(strollut_bmap_xpct, unequal, NULL);
	free(strollut_bmap_xpct);
	strollut_bmap_xpct = NULL;
}

/******************************************************************************
 * Unsigned 32-bits support
 ******************************************************************************/

static const uint32_t strollut_bmap_words32[] = {
	0x00000000,
	0xffff0000,
	0x0000ffff,
	0xff00ff00,
	0x00ff00ff,
	0xf0a0f050,
	0x0f050f0a,
	0x00ff00aa,
	0xff005500,
	0xaaaa0000,
	0x00005555,
	0xffffffff
};

static const uint32_t strollut_bmap_masks32[] = {
	0x00000000,
	0xffffffff,
	0x0000ffff,
	0xffff0000,
	0xff00ff00,
	0x00ff00ff,
	0xaa005500,
	0x005500aa
};

static const struct stroll_bmap_range32 {
	uint32_t     mask;
	unsigned int start;
	unsigned int count;
} strollut_bmap_ranges32[] = {
	{ 0xffffffff,  0, 32 },
	{ 0x0000ffff,  0, 16 },
	{ 0xffff0000, 16, 16 },
	{ 0x00ffff00,  8, 16 },
	{ 0x000000f0,  4,  4 },
	{ 0x0000f000, 12,  4 },
	{ 0x00f00000, 20,  4 },
	{ 0xf0000000, 28,  4 }
};

static void
strollut_bmap_setup_mask_oper32(uint32_t (* oper)(uint32_t, uint32_t))
{
	unsigned int   b, m;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words32);
	unsigned int   mnr = stroll_array_nr(strollut_bmap_masks32);
	uint32_t     * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			uint32_t * exp = &expected[(b * mnr) + m];

			*exp = oper(strollut_bmap_words32[b],
			            strollut_bmap_masks32[m]);
		}
	}

	strollut_bmap_xpct = expected;
}

static void
strollut_bmap_run_mask_oper32(uint32_t (* oper)(uint32_t, uint32_t))
{
	unsigned int     b, m;
	unsigned int     bnr = stroll_array_nr(strollut_bmap_words32);
	unsigned int     mnr = stroll_array_nr(strollut_bmap_masks32);
	const uint32_t * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			cute_check_hex(oper(strollut_bmap_words32[b],
			                    strollut_bmap_masks32[m]),
			               equal,
			               expected[(b * mnr) + m]);
		}
	}
}

static void
strollut_bmap_setup_range_oper32(uint32_t (* oper)(uint32_t, uint32_t))
{
	unsigned int   b, r;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words32);
	unsigned int   rnr = stroll_array_nr(strollut_bmap_ranges32);
	uint32_t     * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			uint32_t * exp = &expected[(b * rnr) + r];

			*exp = oper(strollut_bmap_words32[b],
			            strollut_bmap_ranges32[r].mask);
		}
	}

	strollut_bmap_xpct = expected;
}

static void
strollut_bmap_run_range_oper32(uint32_t (* oper)(uint32_t,
                                                 unsigned int,
                                                 unsigned int))
{
	unsigned int     b, r;
	unsigned int     bnr = stroll_array_nr(strollut_bmap_words32);
	unsigned int     rnr = stroll_array_nr(strollut_bmap_ranges32);
	const uint32_t * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			cute_check_hex(
				oper(strollut_bmap_words32[b],
				     strollut_bmap_ranges32[r].start,
				     strollut_bmap_ranges32[r].count),
				equal,
				expected[(b * rnr) + r]);
		}
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_init_assert32)
{
	cute_expect_assertion(stroll_bmap_setup_set32(NULL));
	cute_expect_assertion(stroll_bmap_setup_clear32(NULL));
}
#else
CUTE_TEST(strollut_bmap_init_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_init32)
{
	uint32_t bmp = 0x5A5A5A5A;

	stroll_bmap_setup_set32(&bmp);
	cute_check_hex(bmp, equal, UINT32_MAX);

	stroll_bmap_setup_clear32(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_mask_assert32)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_mask32(0, 0));
	cute_expect_assertion(stroll_bmap_mask32(0, 33));
	cute_expect_assertion(stroll_bmap_mask32(3, 30));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_mask_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_mask32)
{
	unsigned int r;

	for (r = 0; r < stroll_array_nr(strollut_bmap_ranges32); r++) {
		const struct stroll_bmap_range32 * rng;

		rng = &strollut_bmap_ranges32[r];

		cute_check_hex(stroll_bmap_mask32(rng->start, rng->count),
		               equal,
		               rng->mask);
	}
}

CUTE_TEST(strollut_bmap_hweight32)
{
	unsigned int m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words32); m++) {
		uint32_t     bmp = strollut_bmap_words32[m];
		unsigned int b;
		unsigned int cnt;

		for (b = 0, cnt = 0; b < 32; b++)
			if (bmp & (1U << b))
				cnt++;

		cute_check_hex(stroll_bmap_hweight32(bmp), equal, cnt);
	}
}

static uint32_t
strollut_bmap_and_oper32(uint32_t bmap, uint32_t mask)
{
	return bmap & mask;
}

static void
strollut_bmap_setup_and32(void)
{
	strollut_bmap_setup_mask_oper32(strollut_bmap_and_oper32);
}

CUTE_TEST_STATIC(strollut_bmap_and32,
                 strollut_bmap_setup_and32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper32(stroll_bmap_and32);
}

static void
strollut_bmap_setup_and_range32(void)
{
	strollut_bmap_setup_range_oper32(strollut_bmap_and_oper32);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_and_range_assert32)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_and_range32(0, 0, 0));
	cute_expect_assertion(stroll_bmap_and_range32(0, 32, 1));
	cute_expect_assertion(stroll_bmap_and_range32(0, 30, 3));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_and_range_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_and_range32,
                 strollut_bmap_setup_and_range32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper32(stroll_bmap_and_range32);
}

static uint32_t
strollut_bmap_or_oper32(uint32_t bmap, uint32_t mask)
{
	return bmap | mask;
}

static void
strollut_bmap_setup_or32(void)
{
	strollut_bmap_setup_mask_oper32(strollut_bmap_or_oper32);
}

CUTE_TEST_STATIC(strollut_bmap_or32,
                 strollut_bmap_setup_or32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper32(stroll_bmap_or32);
}

static void
strollut_bmap_setup_or_range32(void)
{
	strollut_bmap_setup_range_oper32(strollut_bmap_or_oper32);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_or_range_assert32)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_or_range32(0, 0, 0));
	cute_expect_assertion(stroll_bmap_or_range32(0, 32, 1));
	cute_expect_assertion(stroll_bmap_or_range32(0, 30, 3));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_or_range_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_or_range32,
                 strollut_bmap_setup_or_range32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper32(stroll_bmap_or_range32);
}

static uint32_t
strollut_bmap_xor_oper32(uint32_t bmap, uint32_t mask)
{
	return bmap ^ mask;
}

static void
strollut_bmap_setup_xor32(void)
{
	strollut_bmap_setup_mask_oper32(strollut_bmap_xor_oper32);
}

CUTE_TEST_STATIC(strollut_bmap_xor32,
                 strollut_bmap_setup_xor32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper32(stroll_bmap_xor32);
}

static void
strollut_bmap_setup_xor_range32(void)
{
	strollut_bmap_setup_range_oper32(strollut_bmap_xor_oper32);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_xor_range_assert32)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_xor_range32(0, 0, 0));
	cute_expect_assertion(stroll_bmap_xor_range32(0, 32, 1));
	cute_expect_assertion(stroll_bmap_xor_range32(0, 30, 3));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_xor_range_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_xor_range32,
                 strollut_bmap_setup_xor_range32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper32(stroll_bmap_xor_range32);
}

CUTE_TEST(strollut_bmap_test_bit32)
{
	unsigned int b, m;
	unsigned int mnr = stroll_array_nr(strollut_bmap_words32);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 32; b++) {
			uint32_t bmp = strollut_bmap_words32[m];

			if (bmp & (1U << b))
				cute_check_bool(stroll_bmap_test32(bmp, b),
				                is,
				                true);
			else
				cute_check_bool(stroll_bmap_test32(bmp, b),
				                is,
				                false);
		}
	}
}

CUTE_TEST(strollut_bmap_test_all32)
{
	unsigned int b, m;
	unsigned int mnr = stroll_array_nr(strollut_bmap_words32);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 32; b++) {
			uint32_t bmp = strollut_bmap_words32[m];

			if (!!bmp)
				cute_check_bool(stroll_bmap_test_all32(bmp),
				                is,
				                true);
			else
				cute_check_bool(stroll_bmap_test_all32(bmp),
				                is,
				                false);
		}
	}
}

static void
strollut_bmap_setup_test_mask32(void)
{
	unsigned int   b, m;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words32);
	unsigned int   mnr = stroll_array_nr(strollut_bmap_masks32);
	bool         * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool * exp = &expected[(b * mnr) + m];

			*exp = !!(strollut_bmap_words32[b] &
			          strollut_bmap_masks32[m]);
		}
	}

	strollut_bmap_xpct = expected;
}

CUTE_TEST_STATIC(strollut_bmap_test_mask32,
                 strollut_bmap_setup_test_mask32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	unsigned int   b, m;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words32);
	unsigned int   mnr = stroll_array_nr(strollut_bmap_masks32);
	const bool   * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool res;

			res = stroll_bmap_test_mask32(
				strollut_bmap_words32[b],
				strollut_bmap_masks32[m]);
			cute_check_bool(res, is, expected[(b * mnr) + m]);
		}
	}
}

static void
strollut_bmap_setup_test_range32(void)
{
	unsigned int   b, r;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words32);
	unsigned int   rnr = stroll_array_nr(strollut_bmap_ranges32);
	bool         * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool * exp = &expected[(b * rnr) + r];

			*exp = !!(strollut_bmap_words32[b] &
			          strollut_bmap_ranges32[r].mask);
		}
	}

	strollut_bmap_xpct = expected;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_test_range_assert32)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_test_range32(0, 0, 0));
	cute_expect_assertion(stroll_bmap_test_range32(0, 32, 1));
	cute_expect_assertion(stroll_bmap_test_range32(0, 30, 3));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_test_range_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_test_range32,
                 strollut_bmap_setup_test_range32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	unsigned int   b, r;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words32);
	unsigned int   rnr = stroll_array_nr(strollut_bmap_ranges32);
	const bool   * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool res;

			res = stroll_bmap_test_range32(
				strollut_bmap_words32[b],
				strollut_bmap_ranges32[r].start,
				strollut_bmap_ranges32[r].count);
			cute_check_bool(res, is, expected[(b * rnr) + r]);
		}
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_bit_assert32)
{
	cute_expect_assertion(stroll_bmap_set32(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_set_bit_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_set_bit32)
{
	uint32_t     bmp = 0;
	unsigned int b;

	for (b = 0; b < 32; b++) {
		bmp = 0;
		stroll_bmap_set32(&bmp, b);
		cute_check_bool(!!(bmp & (1U << b)), is, true);
		cute_check_bool(!!(bmp & ~(1U << b)), is, false);
	}
}

static uint32_t
strollut_bmap_set_mask_oper32(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap_set_mask32(&bmp, mask);

	return bmp;
}


#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_mask_assert32)
{
	cute_expect_assertion(stroll_bmap_set_mask32(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_set_mask_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_set_mask32,
                 strollut_bmap_setup_or32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper32(strollut_bmap_set_mask_oper32);
}

static uint32_t
strollut_bmap_set_range_oper32(uint32_t     bmap,
                               unsigned int start_bit,
                               unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap_set_range32(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_range_assert32)
{
	uint32_t bmp = 0;

	cute_expect_assertion(stroll_bmap_set_range32(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_set_range32(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap_set_range32(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_set_range32(&bmp, 30, 3));
}
#else
CUTE_TEST(strollut_bmap_set_range_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_set_range32,
                 strollut_bmap_setup_or_range32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper32(strollut_bmap_set_range_oper32);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_all_assert32)
{
	cute_expect_assertion(stroll_bmap_set_all32(NULL));
}
#else
CUTE_TEST(strollut_bmap_set_all_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_set_all32)
{
	uint32_t bmp = 0;

	stroll_bmap_set_all32(&bmp);
	cute_check_hex(bmp, equal, ~(0U));
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_bit_assert32)
{
	cute_expect_assertion(stroll_bmap_clear32(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_clear_bit_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_clear_bit32)
{
	uint32_t     bmp = ~(0U);
	unsigned int b;

	for (b = 0; b < 32; b++) {
		bmp = ~(0U);
		stroll_bmap_clear32(&bmp, b);
		cute_check_hex(bmp, equal, ~(0U) & ~(1U << b));
	}
}

static uint32_t
strollut_bmap_notand_oper32(uint32_t bmap, uint32_t mask)
{
	return bmap & ~mask;
}

static void
strollut_bmap_setup_clear_mask32(void)
{
	strollut_bmap_setup_mask_oper32(strollut_bmap_notand_oper32);
}

static uint32_t
strollut_bmap_clear_mask_oper32(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap_clear_mask32(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_mask_assert32)
{
	cute_expect_assertion(stroll_bmap_clear_mask32(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_clear_mask_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_clear_mask32,
                 strollut_bmap_setup_clear_mask32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper32(strollut_bmap_clear_mask_oper32);
}

static void
strollut_bmap_setup_clear_range32(void)
{
	strollut_bmap_setup_range_oper32(strollut_bmap_notand_oper32);
}

static uint32_t
strollut_bmap_clear_range_oper32(uint32_t     bmap,
                                 unsigned int start_bit,
                                 unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap_clear_range32(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_range_assert32)
{
	uint32_t bmp = ~(0U);

	cute_expect_assertion(stroll_bmap_clear_range32(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_clear_range32(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap_clear_range32(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_clear_range32(&bmp, 30, 3));
}
#else
CUTE_TEST(strollut_bmap_clear_range_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_clear_range32,
                 strollut_bmap_setup_clear_range32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper32(strollut_bmap_clear_range_oper32);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_all_assert32)
{
	cute_expect_assertion(stroll_bmap_clear_all32(NULL));
}
#else
CUTE_TEST(strollut_bmap_clear_all_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_clear_all32)
{
	uint32_t bmp = ~(0U);

	stroll_bmap_clear_all32(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_toggle_bit_assert32)
{
	cute_expect_assertion(stroll_bmap_toggle32(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_toggle_bit_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_toggle_bit32)
{
	uint32_t     bmp = 0;
	unsigned int b;

	for (b = 0; b < 32; b++) {
		bmp = 0;
		stroll_bmap_toggle32(&bmp, b);
		cute_check_hex(bmp, equal, 1U << b);

		bmp = ~(0U);
		stroll_bmap_toggle32(&bmp, b);
		cute_check_hex(bmp, equal, ~(1U << b));
	}
}

static void
strollut_bmap_setup_toggle_mask32(void)
{
	strollut_bmap_setup_mask_oper32(strollut_bmap_xor_oper32);
}

static uint32_t
strollut_bmap_toggle_mask_oper32(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap_toggle_mask32(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_toggle_mask_assert32)
{
	cute_expect_assertion(stroll_bmap_toggle_mask32(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_toggle_mask_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_toggle_mask32,
                 strollut_bmap_setup_toggle_mask32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper32(strollut_bmap_toggle_mask_oper32);
}

static void
strollut_bmap_setup_toggle_range32(void)
{
	strollut_bmap_setup_range_oper32(strollut_bmap_xor_oper32);
}

static uint32_t
strollut_bmap_toggle_range_oper32(uint32_t     bmap,
                                  unsigned int start_bit,
                                  unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap_toggle_range32(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_toggle_range_assert32)
{
	uint32_t bmp = 0;

	cute_expect_assertion(stroll_bmap_toggle_range32(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_toggle_range32(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap_toggle_range32(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_toggle_range32(&bmp, 30, 3));
}
#else
CUTE_TEST(strollut_bmap_toggle_range_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_toggle_range32,
                 strollut_bmap_setup_toggle_range32,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper32(strollut_bmap_toggle_range_oper32);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_toggle_all_assert32)
{
	cute_expect_assertion(stroll_bmap_toggle_all32(NULL));
}
#else
CUTE_TEST(strollut_bmap_toggle_all_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_toggle_all32)
{
	unsigned int b;
	unsigned int bnr = stroll_array_nr(strollut_bmap_words32);

	for (b = 0; b < bnr; b++) {
		uint32_t bmp = strollut_bmap_words32[b];
		uint32_t exp = bmp ^ UINT32_MAX;

		stroll_bmap_toggle_all32(&bmp);
		cute_check_hex(bmp, equal, exp);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_iter_assert32)
{
	uint32_t     iter;
	uint32_t     bmp = 0;
	unsigned int b;

	cute_expect_assertion(_stroll_bmap_setup_set_iter32(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_set_iter32(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter32(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter32(&iter, NULL));
}
#else
CUTE_TEST(strollut_bmap_set_iter_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_set_iter32)
{
	uint32_t     iter;
	uint32_t     bmp = 0;
	unsigned int b;
	unsigned int m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words32); m++) {
		unsigned int e = 0;

		bmp = strollut_bmap_words32[m];
		b = 0;
		stroll_bmap_foreach_set32(&iter, bmp, &b) {
			while ((e < 32) && !(bmp & (UINT32_C(1) << e)))
				e++;
			cute_check_uint(b, equal, e);
			e++;
		}

		if (!bmp)
			cute_check_uint(b, equal, (unsigned int)-1);
		else
			cute_check_uint_range(b, in, CUTE_UINT_RANGE(0, 31));
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_iter_assert32)
{
	uint32_t     iter;
	uint32_t     bmp = 0;
	unsigned int b;

	cute_expect_assertion(_stroll_bmap_setup_clear_iter32(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_clear_iter32(&iter,
	                                                      bmp,
							      NULL));
	cute_expect_assertion(_stroll_bmap_step_iter32(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter32(&iter, NULL));
}
#else
CUTE_TEST(strollut_bmap_clear_iter_assert32)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_clear_iter32)
{
	uint32_t     iter;
	uint32_t     bmp = 0;
	unsigned int b;
	unsigned int m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words32); m++) {
		unsigned int e = 0;

		bmp = strollut_bmap_words32[m];
		b = 0;
		stroll_bmap_foreach_clear32(&iter, bmp, &b) {
			while ((e < 32) && (bmp & (UINT32_C(1) << e)))
				e++;
			cute_check_uint(b, equal, e);
			e++;
		}

		if (bmp == UINT32_MAX)
			cute_check_uint(b, equal, (unsigned int)-1);
		else
			cute_check_uint_range(b, in, CUTE_UINT_RANGE(0, 31));
	}
}

CUTE_GROUP(strollut_bmap_group32) = {
	CUTE_REF(strollut_bmap_init_assert32),
	CUTE_REF(strollut_bmap_init32),
	CUTE_REF(strollut_bmap_mask_assert32),
	CUTE_REF(strollut_bmap_mask32),
	CUTE_REF(strollut_bmap_hweight32),

	CUTE_REF(strollut_bmap_and32),
	CUTE_REF(strollut_bmap_and_range_assert32),
	CUTE_REF(strollut_bmap_and_range32),

	CUTE_REF(strollut_bmap_or32),
	CUTE_REF(strollut_bmap_or_range_assert32),
	CUTE_REF(strollut_bmap_or_range32),

	CUTE_REF(strollut_bmap_xor32),
	CUTE_REF(strollut_bmap_xor_range_assert32),
	CUTE_REF(strollut_bmap_xor_range32),

	CUTE_REF(strollut_bmap_test_bit32),
	CUTE_REF(strollut_bmap_test_all32),
	CUTE_REF(strollut_bmap_test_mask32),
	CUTE_REF(strollut_bmap_test_range_assert32),
	CUTE_REF(strollut_bmap_test_range32),

	CUTE_REF(strollut_bmap_set_bit_assert32),
	CUTE_REF(strollut_bmap_set_bit32),
	CUTE_REF(strollut_bmap_set_mask_assert32),
	CUTE_REF(strollut_bmap_set_mask32),
	CUTE_REF(strollut_bmap_set_range_assert32),
	CUTE_REF(strollut_bmap_set_range32),
	CUTE_REF(strollut_bmap_set_all_assert32),
	CUTE_REF(strollut_bmap_set_all32),

	CUTE_REF(strollut_bmap_clear_bit_assert32),
	CUTE_REF(strollut_bmap_clear_bit32),
	CUTE_REF(strollut_bmap_clear_mask_assert32),
	CUTE_REF(strollut_bmap_clear_mask32),
	CUTE_REF(strollut_bmap_clear_range_assert32),
	CUTE_REF(strollut_bmap_clear_range32),
	CUTE_REF(strollut_bmap_clear_all_assert32),
	CUTE_REF(strollut_bmap_clear_all32),

	CUTE_REF(strollut_bmap_toggle_bit_assert32),
	CUTE_REF(strollut_bmap_toggle_bit32),
	CUTE_REF(strollut_bmap_toggle_mask_assert32),
	CUTE_REF(strollut_bmap_toggle_mask32),
	CUTE_REF(strollut_bmap_toggle_range_assert32),
	CUTE_REF(strollut_bmap_toggle_range32),
	CUTE_REF(strollut_bmap_toggle_all_assert32),
	CUTE_REF(strollut_bmap_toggle_all32),

	CUTE_REF(strollut_bmap_set_iter_assert32),
	CUTE_REF(strollut_bmap_set_iter32),
	CUTE_REF(strollut_bmap_clear_iter_assert32),
	CUTE_REF(strollut_bmap_clear_iter32)
};

CUTE_SUITE(strollut_bmap_suite32, strollut_bmap_group32);

/******************************************************************************
 * Unsigned 64-bits support
 ******************************************************************************/

static const uint64_t strollut_bmap_words64[] = {
	        0x00000000,
	        0xffff0000,
	        0x0000ffff,
	        0xff00ff00,
	        0x00ff00ff,
	        0xf0a0f050,
	        0x0f050f0a,
	        0x00ff00aa,
	        0xff005500,
	        0xaaaa0000,
	        0x00005555,
	        0xffffffff,

	0xffffffff00000000,
	0x00000000ffffffff,
	0xfff000ff0000fff0,
	0x00ff00aa00ff0055,
	0xaaaa000055550000,
	0x00aa005500aa0055,
	0xf0a0f050f0a0f050,
	0xffffffffffffffff
};

static const uint64_t strollut_bmap_masks64[] = {
	        0x00000000,
	        0xffffffff,
	        0x0000ffff,
	        0xffff0000,
	        0xff00ff00,
	        0x00ff00ff,
	        0xaa005500,
	        0x005500aa,

	0xffffffffffffffff,
	0x0000ffff0000ffff,
	0xffff0000ffff0000,
	0xff00ff00ff00ff00,
	0x00ff00ff00ff00ff,
	0xaa005500aa005500,
	0x005500aa005500aa
};

static const struct strollut_bmap_range64 {
	uint64_t     mask;
	unsigned int start;
	unsigned int count;
} strollut_bmap_ranges64[] = {
	{ 0x00000000ffffffff,  0, 32 },
	{ 0x000000000000ffff,  0, 16 },
	{ 0x00000000ffff0000, 16, 16 },
	{ 0x0000000000ffff00,  8, 16 },
	{ 0x00000000000000f0,  4,  4 },
	{ 0x000000000000f000, 12,  4 },
	{ 0x0000000000f00000, 20,  4 },
	{ 0x00000000f0000000, 28,  4 },

	{ 0xffffffffffffffff,  0, 64 },
	{ 0x00000000ffffffff,  0, 32 },
	{ 0xffffffff00000000, 32, 32 },
	{ 0x000000ffffff0000, 16, 24 },
	{ 0xf000000000000000, 60,  4 },
	{ 0x00f0000000000000, 52,  4 },
	{ 0x0000f00000000000, 44,  4 },
	{ 0x000000f000000000, 36,  4 }
};

static void
strollut_bmap_setup_mask_oper64(uint64_t (* oper)(uint64_t, uint64_t))
{
	unsigned int   b, m;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words64);
	unsigned int   mnr = stroll_array_nr(strollut_bmap_masks64);
	uint64_t     * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			uint64_t * exp = &expected[(b * mnr) + m];

			*exp = oper(strollut_bmap_words64[b],
			            strollut_bmap_masks64[m]);
		}
	}

	strollut_bmap_xpct = expected;
}

static void
strollut_bmap_run_mask_oper64(uint64_t  (* oper)(uint64_t, uint64_t))
{
	unsigned int     b, m;
	unsigned int     bnr = stroll_array_nr(strollut_bmap_words64);
	unsigned int     mnr = stroll_array_nr(strollut_bmap_masks64);
	const uint64_t * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			cute_check_hex(oper(strollut_bmap_words64[b],
			                    strollut_bmap_masks64[m]),
			               equal,
			               expected[(b * mnr) + m]);
		}
	}
}

static void
strollut_bmap_setup_range_oper64(uint64_t (* oper)(uint64_t, uint64_t))
{
	unsigned int   b, r;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words64);
	unsigned int   rnr = stroll_array_nr(strollut_bmap_ranges64);
	uint64_t     * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			uint64_t * exp = &expected[(b * rnr) + r];

			*exp = oper(strollut_bmap_words64[b],
			            strollut_bmap_ranges64[r].mask);
		}
	}

	strollut_bmap_xpct = expected;
}

static void
strollut_bmap_run_range_oper64(uint64_t (* oper)(uint64_t,
                                                 unsigned int,
                                                 unsigned int))
{
	unsigned int     b, r;
	unsigned int     bnr = stroll_array_nr(strollut_bmap_words64);
	unsigned int     rnr = stroll_array_nr(strollut_bmap_ranges64);
	const uint64_t * expected = strollut_bmap_xpct;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			cute_check_hex(oper(strollut_bmap_words64[b],
			                    strollut_bmap_ranges64[r].start,
			                    strollut_bmap_ranges64[r].count),
			               equal,
			               expected[(b * rnr) + r]);
		}
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_init_assert64)
{
	cute_expect_assertion(stroll_bmap_setup_set64(NULL));
	cute_expect_assertion(stroll_bmap_setup_clear64(NULL));
}
#else
CUTE_TEST(strollut_bmap_init_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_init64)
{
	uint64_t bmp = 0x5A5A5A5A5A5A5A5A;

	stroll_bmap_setup_set64(&bmp);
	cute_check_hex(bmp, equal, UINT64_MAX);

	stroll_bmap_setup_clear64(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_mask_assert64)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_mask64(0, 0));
	cute_expect_assertion(stroll_bmap_mask64(0, 65));
	cute_expect_assertion(stroll_bmap_mask64(3, 63));
#pragma GCC diagnostic pop

}
#else
CUTE_TEST(strollut_bmap_mask_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_mask64)
{
	unsigned int r;

	for (r = 0; r < stroll_array_nr(strollut_bmap_ranges64); r++) {
		const struct strollut_bmap_range64 * rng;

		rng = &strollut_bmap_ranges64[r];

		cute_check_hex(stroll_bmap_mask64(rng->start, rng->count),
		               equal,
		               rng->mask);
	}
}

CUTE_TEST(strollut_bmap_hweight64)
{
	unsigned int m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words64); m++) {
		uint64_t     bmp = strollut_bmap_words64[m];
		unsigned int b;
		unsigned int cnt;

		for (b = 0, cnt = 0; b < 64; b++)
			if (bmp & (UINT64_C(1) << b))
				cnt++;

		cute_check_hex(stroll_bmap_hweight64(bmp), equal, cnt);
	}
}

static uint64_t
strollut_bmap_and_oper64(uint64_t bmap, uint64_t mask)
{
	return bmap & mask;
}

static void
strollut_bmap_setup_and64(void)
{
	strollut_bmap_setup_mask_oper64(strollut_bmap_and_oper64);
}

CUTE_TEST_STATIC(strollut_bmap_and64,
                 strollut_bmap_setup_and64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper64(stroll_bmap_and64);
}

static void
strollut_bmap_setup_and_range64(void)
{
	strollut_bmap_setup_range_oper64(strollut_bmap_and_oper64);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_and_range_assert64)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_and_range64(0, 0, 0));
	cute_expect_assertion(stroll_bmap_and_range64(0, 64, 1));
	cute_expect_assertion(stroll_bmap_and_range64(0, 60, 5));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_and_range_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_and_range64,
                 strollut_bmap_setup_and_range64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper64(stroll_bmap_and_range64);
}

static uint64_t
strollut_bmap_or_oper64(uint64_t bmap, uint64_t mask)
{
	return bmap | mask;
}

static void
strollut_bmap_setup_or64(void)
{
	strollut_bmap_setup_mask_oper64(strollut_bmap_or_oper64);
}

CUTE_TEST_STATIC(strollut_bmap_or64,
                 strollut_bmap_setup_or64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper64(stroll_bmap_or64);
}

static void
strollut_bmap_setup_or_range64(void)
{
	strollut_bmap_setup_range_oper64(strollut_bmap_or_oper64);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_or_range_assert64)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_or_range64(0, 0, 0));
	cute_expect_assertion(stroll_bmap_or_range64(0, 64, 1));
	cute_expect_assertion(stroll_bmap_or_range64(0, 60, 5));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_or_range_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_or_range64,
                 strollut_bmap_setup_or_range64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper64(stroll_bmap_or_range64);
}

static uint64_t
strollut_bmap_xor_oper64(uint64_t bmap, uint64_t mask)
{
	return bmap ^ mask;
}

static void
strollut_bmap_setup_xor64(void)
{
	strollut_bmap_setup_mask_oper64(strollut_bmap_xor_oper64);
}

CUTE_TEST_STATIC(strollut_bmap_xor64,
                 strollut_bmap_setup_xor64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper64(stroll_bmap_xor64);
}

static void
strollut_bmap_setup_xor_range64(void)
{
	strollut_bmap_setup_range_oper64(strollut_bmap_xor_oper64);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_xor_range_assert64)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_xor_range64(0, 0, 0));
	cute_expect_assertion(stroll_bmap_xor_range64(0, 64, 1));
	cute_expect_assertion(stroll_bmap_xor_range64(0, 60, 5));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_xor_range_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_xor_range64,
                 strollut_bmap_setup_xor_range64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper64(stroll_bmap_xor_range64);
}

CUTE_TEST(strollut_bmap_test_bit64)
{
	unsigned int b, m;
	unsigned int mnr = stroll_array_nr(strollut_bmap_words64);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 64; b++) {
			uint64_t bmp = strollut_bmap_words64[m];

			if (bmp & (UINT64_C(1) << b))
				cute_check_bool(stroll_bmap_test64(bmp, b),
				                is,
				                true);
			else
				cute_check_bool(stroll_bmap_test64(bmp, b),
				                is,
				                false);
		}
	}
}

CUTE_TEST(strollut_bmap_test_all64)
{
	unsigned int b, m;
	unsigned int mnr = stroll_array_nr(strollut_bmap_words64);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 64; b++) {
			uint64_t bmp = strollut_bmap_words64[m];

			if (!!bmp)
				cute_check_bool(stroll_bmap_test_all64(bmp),
				                is,
				                true);
			else
				cute_check_bool(stroll_bmap_test_all64(bmp),
				                is,
				                false);
		}
	}
}

static void
strollut_bmap_setup_test_mask64(void)
{
	unsigned int   b, m;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words64);
	unsigned int   mnr = stroll_array_nr(strollut_bmap_masks64);
	bool         * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool * exp = &expected[(b * mnr) + m];

			*exp = !!(strollut_bmap_words64[b] &
			          strollut_bmap_masks64[m]);
		}
	}

	strollut_bmap_xpct = expected;
}

CUTE_TEST_STATIC(strollut_bmap_test_mask64,
                 strollut_bmap_setup_test_mask64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	unsigned int   b, m;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words64);
	unsigned int   mnr = stroll_array_nr(strollut_bmap_masks64);
	const bool   * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool res;

			res = stroll_bmap_test_mask64(
				strollut_bmap_words64[b],
				strollut_bmap_masks64[m]);
			cute_check_bool(res, is, expected[(b * mnr) + m]);
		}
	}
}

static void
strollut_bmap_setup_test_range64(void)
{
	unsigned int   b, r;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words64);
	unsigned int   rnr = stroll_array_nr(strollut_bmap_ranges64);
	bool         * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool * exp = &expected[(b * rnr) + r];

			*exp = !!(strollut_bmap_words64[b] &
			          strollut_bmap_ranges64[r].mask);
		}
	}

	strollut_bmap_xpct = expected;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_test_range_assert64)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_test_range64(0, 0, 0));
	cute_expect_assertion(stroll_bmap_test_range64(0, 64, 1));
	cute_expect_assertion(stroll_bmap_test_range64(0, 60, 5));
#pragma GCC diagnostic pop

}
#else
CUTE_TEST(strollut_bmap_test_range_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_test_range64,
                 strollut_bmap_setup_test_range64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	unsigned int   b, r;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words64);
	unsigned int   rnr = stroll_array_nr(strollut_bmap_ranges64);
	const bool   * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool res;

			res = stroll_bmap_test_range64(
				strollut_bmap_words64[b],
				strollut_bmap_ranges64[r].start,
				strollut_bmap_ranges64[r].count);
			cute_check_bool(res, is, expected[(b * rnr) + r]);
		}
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_bit_assert64)
{
	cute_expect_assertion(stroll_bmap_set64(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_set_bit_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_set_bit64)
{
	uint64_t     bmp = 0;
	unsigned int b;

	for (b = 0; b < 64; b++) {
		bmp = 0;
		stroll_bmap_set64(&bmp, b);
		cute_check_bool(!!(bmp & (UINT64_C(1) << b)), is, true);
		cute_check_bool(!!(bmp & ~(UINT64_C(1) << b)), is, false);
	}
}

static uint64_t
strollut_bmap_set_mask_oper64(uint64_t bmap, uint64_t mask)
{
	uint64_t bmp = bmap;

	stroll_bmap_set_mask64(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_mask_assert64)
{
	cute_expect_assertion(stroll_bmap_set_mask64(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_set_mask_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_set_mask64,
                 strollut_bmap_setup_or64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper64(strollut_bmap_set_mask_oper64);
}

static uint64_t
strollut_bmap_set_range_oper64(uint64_t     bmap,
                               unsigned int start_bit,
                               unsigned int bit_count)
{
	uint64_t bmp = bmap;

	stroll_bmap_set_range64(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_range_assert64)
{
	uint64_t bmp = 0;

	cute_expect_assertion(stroll_bmap_set_range64(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_set_range64(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap_set_range64(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_set_range64(&bmp, 60, 5));
}
#else
CUTE_TEST(strollut_bmap_set_range_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_set_range64,
                 strollut_bmap_setup_or_range64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper64(strollut_bmap_set_range_oper64);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_all_assert64)
{
	cute_expect_assertion(stroll_bmap_set_all64(NULL));
}
#else
CUTE_TEST(strollut_bmap_set_all_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_set_all64)
{
	uint64_t bmp = 0;

	stroll_bmap_set_all64(&bmp);
	cute_check_hex(bmp, equal, UINT64_MAX);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_bit_assert64)
{
	cute_expect_assertion(stroll_bmap_clear64(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_clear_bit_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_clear_bit64)
{
	uint64_t     bmp = UINT64_MAX;
	unsigned int b;

	for (b = 0; b < 64; b++) {
		bmp = UINT64_MAX;
		stroll_bmap_clear64(&bmp, b);
		cute_check_hex(bmp, equal, UINT64_MAX & ~(UINT64_C(1) << b));
	}
}

static uint64_t
strollut_bmap_notand_oper64(uint64_t bmap, uint64_t mask)
{
	return bmap & ~mask;
}

static void
strollut_bmap_setup_clear_mask64(void)
{
	strollut_bmap_setup_mask_oper64(strollut_bmap_notand_oper64);
}

static uint64_t
strollut_bmap_clear_mask_oper64(uint64_t bmap, uint64_t mask)
{
	uint64_t bmp = bmap;

	stroll_bmap_clear_mask64(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_mask_assert64)
{
	cute_expect_assertion(stroll_bmap_clear_mask64(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_clear_mask_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_clear_mask64,
                 strollut_bmap_setup_clear_mask64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper64(strollut_bmap_clear_mask_oper64);
}

static void
strollut_bmap_setup_clear_range64(void)
{
	strollut_bmap_setup_range_oper64(strollut_bmap_notand_oper64);
}

static uint64_t
strollut_bmap_clear_range_oper64(uint64_t     bmap,
                                 unsigned int start_bit,
                                 unsigned int bit_count)
{
	uint64_t bmp = bmap;

	stroll_bmap_clear_range64(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_range_assert64)
{
	uint64_t bmp = UINT64_MAX;

	cute_expect_assertion(stroll_bmap_clear_range64(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_clear_range64(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap_clear_range64(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_clear_range64(&bmp, 60, 5));
}
#else
CUTE_TEST(strollut_bmap_clear_range_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_clear_range64,
                 strollut_bmap_setup_clear_range64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper64(strollut_bmap_clear_range_oper64);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_all_assert64)
{
	cute_expect_assertion(stroll_bmap_clear_all64(NULL));
}
#else
CUTE_TEST(strollut_bmap_clear_all_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_clear_all64)
{
	uint64_t bmp = UINT64_MAX;

	stroll_bmap_clear_all64(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_toggle_bit_assert64)
{
	cute_expect_assertion(stroll_bmap_toggle64(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_toggle_bit_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_toggle_bit64)
{
	uint64_t     bmp = 0;
	unsigned int b;

	for (b = 0; b < 64; b++) {
		bmp = 0;
		stroll_bmap_toggle64(&bmp, b);
		cute_check_hex(bmp, equal, UINT64_C(1) << b);

		bmp = UINT64_MAX;
		stroll_bmap_toggle64(&bmp, b);
		cute_check_hex(bmp, equal, ~(UINT64_C(1) << b));
	}
}

static void
strollut_bmap_setup_toggle_mask64(void)
{
	strollut_bmap_setup_mask_oper64(strollut_bmap_xor_oper64);
}

static uint64_t
strollut_bmap_toggle_mask_oper64(uint64_t bmap, uint64_t mask)
{
	uint64_t bmp = bmap;

	stroll_bmap_toggle_mask64(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_toggle_mask_assert64)
{
	cute_expect_assertion(stroll_bmap_toggle_mask64(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_toggle_mask_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_toggle_mask64,
                 strollut_bmap_setup_toggle_mask64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_mask_oper64(strollut_bmap_toggle_mask_oper64);
}

static void
strollut_bmap_setup_toggle_range64(void)
{
	strollut_bmap_setup_range_oper64(strollut_bmap_xor_oper64);
}

static uint64_t
strollut_bmap_toggle_range_oper64(uint64_t     bmap,
                                  unsigned int start_bit,
                                  unsigned int bit_count)
{
	uint64_t bmp = bmap;

	stroll_bmap_toggle_range64(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_toggle_range_assert64)
{
	uint64_t bmp = 0;

	cute_expect_assertion(stroll_bmap_toggle_range64(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_toggle_range64(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap_toggle_range64(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_toggle_range64(&bmp, 60, 5));
}
#else
CUTE_TEST(strollut_bmap_toggle_range_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_toggle_range64,
                 strollut_bmap_setup_toggle_range64,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_run_range_oper64(strollut_bmap_toggle_range_oper64);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_toggle_all_assert64)
{
	cute_expect_assertion(stroll_bmap_toggle_all64(NULL));
}
#else
CUTE_TEST(strollut_bmap_toggle_all_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_toggle_all64)
{
	unsigned int b;
	unsigned int bnr = stroll_array_nr(strollut_bmap_words64);

	for (b = 0; b < bnr; b++) {
		uint64_t bmp = strollut_bmap_words64[b];
		uint64_t exp = bmp ^ UINT64_MAX;

		stroll_bmap_toggle_all64(&bmp);
		cute_check_hex(bmp, equal, exp);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_set_iter_assert64)
{
	uint64_t     iter;
	uint64_t     bmp = 0;
	unsigned int b;

	cute_expect_assertion(_stroll_bmap_setup_set_iter64(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_set_iter64(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter64(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter64(&iter, NULL));
}
#else
CUTE_TEST(strollut_bmap_set_iter_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_set_iter64)
{
	uint64_t     iter;
	uint64_t     bmp = 0;
	unsigned int b;
	unsigned int m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words64); m++) {
		unsigned int e = 0;

		bmp = strollut_bmap_words64[m];
		b = 0;
		stroll_bmap_foreach_set64(&iter, bmp, &b) {
			while ((e < 64) && !(bmp & (UINT64_C(1) << e)))
				e++;
			cute_check_uint(b, equal, e);
			e++;
		}

		if (!bmp)
			cute_check_uint(b, equal, (unsigned int)-1);
		else
			cute_check_uint_range(b, in, CUTE_UINT_RANGE(0, 63));
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_clear_iter_assert64)
{
	uint64_t     iter;
	uint64_t     bmp = 0;
	unsigned int b;

	cute_expect_assertion(_stroll_bmap_setup_clear_iter64(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_clear_iter64(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter64(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter64(&iter, NULL));
}
#else
CUTE_TEST(strollut_bmap_clear_iter_assert64)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_clear_iter64)
{
	uint64_t     iter;
	uint64_t     bmp = 0;
	unsigned int b;
	unsigned int m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words64); m++) {
		unsigned int e = 0;

		bmp = strollut_bmap_words64[m];
		b = 0;
		stroll_bmap_foreach_clear64(&iter, bmp, &b) {
			while ((e < 64) && (bmp & (UINT64_C(1) << e)))
				e++;
			cute_check_uint(b, equal, e);
			e++;
		}

		if (bmp == UINT64_MAX)
			cute_check_uint(b, equal, (unsigned int)-1);
		else
			cute_check_uint_range(b, in, CUTE_UINT_RANGE(0, 63));
	}
}

CUTE_GROUP(strollut_bmap_group64) = {
	CUTE_REF(strollut_bmap_init_assert64),
	CUTE_REF(strollut_bmap_init64),
	CUTE_REF(strollut_bmap_mask_assert64),
	CUTE_REF(strollut_bmap_mask64),
	CUTE_REF(strollut_bmap_hweight64),

	CUTE_REF(strollut_bmap_and64),
	CUTE_REF(strollut_bmap_and_range_assert64),
	CUTE_REF(strollut_bmap_and_range64),

	CUTE_REF(strollut_bmap_or64),
	CUTE_REF(strollut_bmap_or_range_assert64),
	CUTE_REF(strollut_bmap_or_range64),

	CUTE_REF(strollut_bmap_xor64),
	CUTE_REF(strollut_bmap_xor_range_assert64),
	CUTE_REF(strollut_bmap_xor_range64),

	CUTE_REF(strollut_bmap_test_bit64),
	CUTE_REF(strollut_bmap_test_all64),
	CUTE_REF(strollut_bmap_test_mask64),
	CUTE_REF(strollut_bmap_test_range_assert64),
	CUTE_REF(strollut_bmap_test_range64),

	CUTE_REF(strollut_bmap_set_bit_assert64),
	CUTE_REF(strollut_bmap_set_bit64),
	CUTE_REF(strollut_bmap_set_mask_assert64),
	CUTE_REF(strollut_bmap_set_mask64),
	CUTE_REF(strollut_bmap_set_range_assert64),
	CUTE_REF(strollut_bmap_set_range64),
	CUTE_REF(strollut_bmap_set_all_assert64),
	CUTE_REF(strollut_bmap_set_all64),

	CUTE_REF(strollut_bmap_clear_bit_assert64),
	CUTE_REF(strollut_bmap_clear_bit64),
	CUTE_REF(strollut_bmap_clear_mask_assert64),
	CUTE_REF(strollut_bmap_clear_mask64),
	CUTE_REF(strollut_bmap_clear_range_assert64),
	CUTE_REF(strollut_bmap_clear_range64),
	CUTE_REF(strollut_bmap_clear_all_assert64),
	CUTE_REF(strollut_bmap_clear_all64),

	CUTE_REF(strollut_bmap_toggle_bit_assert64),
	CUTE_REF(strollut_bmap_toggle_bit64),
	CUTE_REF(strollut_bmap_toggle_mask_assert64),
	CUTE_REF(strollut_bmap_toggle_mask64),
	CUTE_REF(strollut_bmap_toggle_range_assert64),
	CUTE_REF(strollut_bmap_toggle_range64),
	CUTE_REF(strollut_bmap_toggle_all_assert64),
	CUTE_REF(strollut_bmap_toggle_all64),

	CUTE_REF(strollut_bmap_set_iter_assert64),
	CUTE_REF(strollut_bmap_set_iter64),
	CUTE_REF(strollut_bmap_clear_iter_assert64),
	CUTE_REF(strollut_bmap_clear_iter64)
};

CUTE_SUITE(strollut_bmap_suite64, strollut_bmap_group64);

/******************************************************************************
 * Machine word support
 ******************************************************************************/

static const unsigned long strollut_bmap_words[] = {
	        0x00000000,
	        0xffff0000,
	        0x0000ffff,
	        0xff00ff00,
	        0x00ff00ff,
	        0xf0a0f050,
	        0x0f050f0a,
	        0x00ff00aa,
	        0xff005500,
	        0xaaaa0000,
	        0x00005555,
	        0xffffffff,
#if (__WORDSIZE == 64)
	0xffffffff00000000,
	0x00000000ffffffff,
	0xfff000ff0000fff0,
	0x00ff00aa00ff0055,
	0xaaaa000055550000,
	0x00aa005500aa0055,
	0xf0a0f050f0a0f050,
	0xffffffffffffffff
#endif
};

static const unsigned long strollut_bmap_word_masks[] = {
	        0x00000000,
	        0xffffffff,
	        0x0000ffff,
	        0xffff0000,
	        0xff00ff00,
	        0x00ff00ff,
	        0xaa005500,
	        0x005500aa,
#if (__WORDSIZE == 64)
	0xffffffffffffffff,
	0x0000ffff0000ffff,
	0xffff0000ffff0000,
	0xff00ff00ff00ff00,
	0x00ff00ff00ff00ff,
	0xaa005500aa005500,
	0x005500aa005500aa
#endif
};

static const struct strollut_bmap_word_range {
	unsigned long mask;
	unsigned int  start;
	unsigned int  count;
} strollut_bmap_word_ranges[] = {
	{         0xffffffff,  0, 32 },
	{         0x0000ffff,  0, 16 },
	{         0xffff0000, 16, 16 },
	{         0x00ffff00,  8, 16 },
	{         0x000000f0,  4,  4 },
	{         0x0000f000, 12,  4 },
	{         0x00f00000, 20,  4 },
	{         0xf0000000, 28,  4 },
#if (__WORDSIZE == 64)
	{ 0xffffffffffffffff,  0, 64 },
	{ 0x00000000ffffffff,  0, 32 },
	{ 0xffffffff00000000, 32, 32 },
	{ 0x000000ffffff0000, 16, 24 },
	{ 0xf000000000000000, 60,  4 },
	{ 0x00f0000000000000, 52,  4 },
	{ 0x0000f00000000000, 44,  4 },
	{ 0x000000f000000000, 36,  4 }
#endif
};

static void
strollut_bmap_word_setup_mask_oper(unsigned long (* oper)(unsigned long,
                                                          unsigned long))
{
	unsigned int    b, m;
	unsigned int    bnr = stroll_array_nr(strollut_bmap_words);
	unsigned int    mnr = stroll_array_nr(strollut_bmap_word_masks);
	unsigned long * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			unsigned long * exp = &expected[(b * mnr) + m];

			*exp = oper(strollut_bmap_words[b],
			            strollut_bmap_word_masks[m]);
		}
	}

	cute_check_ptr(strollut_bmap_xpct, equal, NULL);
	strollut_bmap_xpct = expected;
}

static void
strollut_bmap_word_setup_range_oper(unsigned long (* oper)(unsigned long,
                                                           unsigned long))
{
	unsigned int    b, r;
	unsigned int    bnr = stroll_array_nr(strollut_bmap_words);
	unsigned int    rnr = stroll_array_nr(strollut_bmap_word_ranges);
	unsigned long * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			unsigned long * exp = &expected[(b * rnr) + r];

			*exp = oper(strollut_bmap_words[b],
			            strollut_bmap_word_ranges[r].mask);
		}
	}

	cute_check_ptr(strollut_bmap_xpct, equal, NULL);
	strollut_bmap_xpct = expected;
}

static void
strollut_bmap_word_run_mask_oper(unsigned long (* oper)(unsigned long,
                                                        unsigned long))
{
	unsigned int          b, m;
	unsigned int          bnr = stroll_array_nr(strollut_bmap_words);
	unsigned int          mnr = stroll_array_nr(strollut_bmap_word_masks);
	const unsigned long * expected = strollut_bmap_xpct;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++)
			cute_check_hex(oper(strollut_bmap_words[b],
			                    strollut_bmap_word_masks[m]),
			               equal,
			               expected[(b * mnr) + m]);
	}
}

static void
strollut_bmap_word_run_range_oper(unsigned long (* oper)(unsigned long,
                                                         unsigned int,
                                                         unsigned int))
{
	unsigned int          b, r;
	unsigned int          bnr = stroll_array_nr(strollut_bmap_words);
	unsigned int          rnr = stroll_array_nr(strollut_bmap_word_ranges);
	const unsigned long * expected = strollut_bmap_xpct;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			cute_check_hex(oper(strollut_bmap_words[b],
				            strollut_bmap_word_ranges[r].start,
				            strollut_bmap_word_ranges[r].count),
				       equal,
				       expected[(b * rnr) + r]);
		}
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_setup_assert)
{
	cute_expect_assertion(stroll_bmap_setup_setul(NULL));
	cute_expect_assertion(stroll_bmap_setup_clearul(NULL));
}
#else
CUTE_TEST(strollut_bmap_word_setup_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_setup)
{
	unsigned long   bmp = 0x5A5A5A5A;
	unsigned long * null __unused = NULL;

	stroll_bmap_setup_setul(&bmp);
	cute_check_hex(bmp, equal, ULONG_MAX);

	stroll_bmap_setup_clearul(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_mask_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_maskul(0, 0));
	cute_expect_assertion(stroll_bmap_maskul(0, 65));
	cute_expect_assertion(stroll_bmap_maskul(3, 63));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_word_mask_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_mask)
{
	unsigned int  r;

	for (r = 0; r < stroll_array_nr(strollut_bmap_word_ranges); r++) {
		const struct strollut_bmap_word_range * rng;

		rng = &strollut_bmap_word_ranges[r];

		cute_check_hex(stroll_bmap_maskul(rng->start, rng->count),
		               equal,
		               rng->mask);
	}
}

CUTE_TEST(strollut_bmap_word_hweight)
{
	unsigned int m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words); m++) {
		unsigned long bmp = strollut_bmap_words[m];
		unsigned int  b;
		unsigned int  cnt;

		for (b = 0, cnt = 0; b < stroll_bops_bitsof(bmp); b++)
			if (bmp & (1UL << b))
				cnt++;

		cute_check_uint(stroll_bmap_hweightul(bmp), equal, cnt);
	}
}

static unsigned long
strollut_bmap_word_and_oper(unsigned long bmap, unsigned long mask)
{
	return bmap & mask;
}

static void
strollut_bmap_word_setup_and(void)
{
	strollut_bmap_word_setup_mask_oper(strollut_bmap_word_and_oper);
}

CUTE_TEST_STATIC(strollut_bmap_word_and,
                 strollut_bmap_word_setup_and,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_mask_oper(stroll_bmap_andul);
}

static void
strollut_bmap_word_setup_and_range(void)
{
	strollut_bmap_word_setup_range_oper(strollut_bmap_word_and_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_and_range_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_and_rangeul(0, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_and_rangeul(0, 64, 1));
	cute_expect_assertion(stroll_bmap_and_rangeul(0, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_and_rangeul(0, 32, 1));
	cute_expect_assertion(stroll_bmap_and_rangeul(0, 30, 3));
#endif
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_word_and_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_and_range,
                 strollut_bmap_word_setup_and_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_range_oper(stroll_bmap_and_rangeul);
}

static unsigned long
strollut_bmap_word_or_oper(unsigned long bmap, unsigned long mask)
{
	return bmap | mask;
}

static void
strollut_bmap_word_setup_or(void)
{
	return strollut_bmap_word_setup_mask_oper(strollut_bmap_word_or_oper);
}

CUTE_TEST_STATIC(strollut_bmap_word_or,
                 strollut_bmap_word_setup_or,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_mask_oper(stroll_bmap_orul);
}

static void
strollut_bmap_word_setup_or_range(void)
{
	return strollut_bmap_word_setup_range_oper(strollut_bmap_word_or_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_or_range_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_or_rangeul(0, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_or_rangeul(0, 64, 1));
	cute_expect_assertion(stroll_bmap_or_rangeul(0, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_or_rangeul(0, 32, 1));
	cute_expect_assertion(stroll_bmap_or_rangeul(0, 30, 3));
#endif
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_word_or_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_or_range,
                 strollut_bmap_word_setup_or_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_range_oper(stroll_bmap_or_rangeul);
}

static unsigned long
strollut_bmap_word_xor_oper(unsigned long bmap, unsigned long mask)
{
	return bmap ^ mask;
}

static void
strollut_bmap_word_setup_xor(void)
{
	strollut_bmap_word_setup_mask_oper(strollut_bmap_word_xor_oper);
}

CUTE_TEST_STATIC(strollut_bmap_word_xor,
                 strollut_bmap_word_setup_xor,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_mask_oper(stroll_bmap_xorul);
}

static void
strollut_bmap_word_setup_xor_range(void)
{
	strollut_bmap_word_setup_range_oper(strollut_bmap_word_xor_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_xor_range_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_xor_rangeul(0, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_xor_rangeul(0, 64, 1));
	cute_expect_assertion(stroll_bmap_xor_rangeul(0, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_xor_rangeul(0, 62, 1));
	cute_expect_assertion(stroll_bmap_xor_rangeul(0, 30, 3));
#endif
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_word_xor_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_xor_range,
                 strollut_bmap_word_setup_xor_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_range_oper(stroll_bmap_xor_rangeul);
}

CUTE_TEST(strollut_bmap_word_test_bit)
{
	unsigned int b, m;
	unsigned int mnr = stroll_array_nr(strollut_bmap_words);
	unsigned long bmp;

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < stroll_bops_bitsof(bmp); b++) {
			bmp = strollut_bmap_words[m];
			cute_check_bool(stroll_bmap_testul(bmp, b),
			                is,
			                !!(bmp & (1UL << b)));
		}
	}
}

CUTE_TEST(strollut_bmap_word_test_all)
{
	unsigned int b, m;
	unsigned int mnr = stroll_array_nr(strollut_bmap_words);
	unsigned long bmp;

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < stroll_bops_bitsof(bmp); b++) {
			bmp = strollut_bmap_words[m];
			cute_check_bool(stroll_bmap_test_allul(bmp), is, !!bmp);
		}
	}
}

static void
strollut_bmap_word_setup_test_mask(void)
{
	unsigned int   b, m;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words);
	unsigned int   mnr = stroll_array_nr(strollut_bmap_word_masks);
	bool         * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool * exp = &expected[(b * mnr) + m];

			*exp = !!(strollut_bmap_words[b] &
			          strollut_bmap_word_masks[m]);
		}
	}

	cute_check_ptr(strollut_bmap_xpct, equal, NULL);
	strollut_bmap_xpct = expected;
}

CUTE_TEST_STATIC(strollut_bmap_word_test_mask,
                 strollut_bmap_word_setup_test_mask,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	unsigned int   b, m;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words);
	unsigned int   mnr = stroll_array_nr(strollut_bmap_word_masks);
	const bool *   expected = strollut_bmap_xpct;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool res = stroll_bmap_test_maskul(
				strollut_bmap_words[b],
				strollut_bmap_word_masks[m]);

			cute_check_bool(res, is, expected[(b * mnr) + m]);
		}
	}
}

static void
strollut_bmap_word_setup_test_range(void)
{
	unsigned int   b, r;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words);
	unsigned int   rnr = stroll_array_nr(strollut_bmap_word_ranges);
	bool         * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool * exp = &expected[(b * rnr) + r];

			*exp = !!(strollut_bmap_words[b] &
			          strollut_bmap_word_ranges[r].mask);
		}
	}

	cute_check_ptr(strollut_bmap_xpct, equal, NULL);
	strollut_bmap_xpct = expected;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_test_range_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_test_rangeul(0, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_test_rangeul(0, 64, 1));
	cute_expect_assertion(stroll_bmap_test_rangeul(0, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_test_rangeul(0, 32, 1));
	cute_expect_assertion(stroll_bmap_test_rangeul(0, 30, 3));
#endif
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap_word_test_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_test_range,
                 strollut_bmap_word_setup_test_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	unsigned int   b, r;
	unsigned int   bnr = stroll_array_nr(strollut_bmap_words);
	unsigned int   rnr = stroll_array_nr(strollut_bmap_word_ranges);
	const bool *   expected = strollut_bmap_xpct;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool res = stroll_bmap_test_rangeul(
				strollut_bmap_words[b],
				strollut_bmap_word_ranges[r].start,
				strollut_bmap_word_ranges[r].count);

			cute_check_bool(res, is, expected[(b * rnr) + r]);
		}
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_set_bit_assert)
{
	cute_expect_assertion(stroll_bmap_setul(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_word_set_bit_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_set_bit)
{
	unsigned long bmp = 0;
	unsigned int  b;

	for (b = 0; b < stroll_bops_bitsof(bmp); b++) {
		bmp = 0;
		stroll_bmap_setul(&bmp, b);
		cute_check_hex(bmp, equal, 1UL << b);
	}
}

static unsigned long
strollut_bmap_word_set_mask_oper(unsigned long bmap, unsigned long mask)
{
	unsigned long bmp = bmap;

	stroll_bmap_set_maskul(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_set_mask_assert)
{
	cute_expect_assertion(stroll_bmap_set_maskul(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_word_set_mask_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_set_mask,
                 strollut_bmap_word_setup_or,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_mask_oper(strollut_bmap_word_set_mask_oper);
}

static unsigned long
strollut_bmap_word_set_range_oper(unsigned long bmap,
                                 unsigned int  start_bit,
                                 unsigned int  bit_count)
{
	unsigned long bmp = bmap;

	stroll_bmap_set_rangeul(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_set_range_assert)
{
	unsigned long bmp = 0;

	cute_expect_assertion(stroll_bmap_set_rangeul(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_set_rangeul(&bmp, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_set_rangeul(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_set_rangeul(&bmp, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_set_rangeul(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_set_rangeul(&bmp, 30, 3));
#endif
}
#else
CUTE_TEST(strollut_bmap_word_set_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_set_range,
                 strollut_bmap_word_setup_or_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_range_oper(strollut_bmap_word_set_range_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_set_all_assert)
{
	cute_expect_assertion(stroll_bmap_set_allul(NULL));
}
#else
CUTE_TEST(strollut_bmap_word_set_all_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_set_all)
{
	unsigned long bmp = 0;

	stroll_bmap_set_allul(&bmp);
	cute_check_hex(bmp, equal, ULONG_MAX);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_clear_bit_assert)
{
	cute_expect_assertion(stroll_bmap_clearul(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_word_clear_bit_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_clear_bit)
{
	unsigned long bmp = ~(0UL);
	unsigned int  b;

	for (b = 0; b < stroll_bops_bitsof(bmp); b++) {
		bmp = ~(0UL);
		stroll_bmap_clearul(&bmp, b);
		cute_check_hex(bmp, equal, ~(0UL) & ~(1UL << b));
	}
}

static unsigned long
strollut_bmap_word_notand_oper(unsigned long bmap, unsigned long mask)
{
	return bmap & ~mask;
}

static void
strollut_bmap_word_setup_clear_mask(void)
{
	strollut_bmap_word_setup_mask_oper(strollut_bmap_word_notand_oper);
}

static unsigned long
strollut_bmap_word_clear_mask_oper(unsigned long bmap, unsigned long mask)
{
	unsigned long bmp = bmap;

	stroll_bmap_clear_maskul(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_clear_mask_assert)
{
	cute_expect_assertion(stroll_bmap_clear_maskul(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_word_clear_mask_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_clear_mask,
                 strollut_bmap_word_setup_clear_mask,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_mask_oper(strollut_bmap_word_clear_mask_oper);
}

static void
strollut_bmap_word_setup_clear_range(void)
{
	strollut_bmap_word_setup_range_oper(strollut_bmap_word_notand_oper);
}

static unsigned long
strollut_bmap_word_clear_range_oper(unsigned long bmap,
                                    unsigned int  start_bit,
                                    unsigned int  bit_count)
{
	unsigned long bmp = bmap;

	stroll_bmap_clear_rangeul(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_clear_range_assert)
{
	unsigned long bmp = ~(0UL);

	cute_expect_assertion(stroll_bmap_clear_rangeul(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_clear_rangeul(&bmp, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_clear_rangeul(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_clear_rangeul(&bmp, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_clear_rangeul(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_clear_rangeul(&bmp, 30, 3));
#endif
}
#else
CUTE_TEST(strollut_bmap_word_clear_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_clear_range,
                 strollut_bmap_word_setup_clear_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_range_oper(strollut_bmap_word_clear_range_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_clear_all_assert)
{
	cute_expect_assertion(stroll_bmap_clear_allul(NULL));
}
#else
CUTE_TEST(strollut_bmap_word_clear_all_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_clear_all)
{
	unsigned long bmp = ~(0UL);

	stroll_bmap_clear_allul(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_toggle_bit_assert)
{
	cute_expect_assertion(stroll_bmap_toggleul(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_word_toggle_bit_assert)
{
	cute_skip("assertion unsupported");
}
#endif

static void
strollut_bmap_word_setup_toggle_mask(void)
{
	strollut_bmap_word_setup_mask_oper(strollut_bmap_word_xor_oper);
}

CUTE_TEST_STATIC(strollut_bmap_word_toggle_bit,
                 strollut_bmap_word_setup_toggle_mask,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	unsigned long bmp = 0;
	unsigned int  b;

	for (b = 0; b < stroll_bops_bitsof(bmp); b++) {
		bmp = 0;
		stroll_bmap_toggleul(&bmp, b);
		cute_check_hex(bmp, equal, 1UL << b);

		bmp = ~(0UL);
		stroll_bmap_toggleul(&bmp, b);
		cute_check_hex(bmp, equal, ~(1UL << b));
	}
}

static unsigned long
strollut_bmap_word_toggle_mask_oper(unsigned long bmap, unsigned long mask)
{
	unsigned long bmp = bmap;

	stroll_bmap_toggle_maskul(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_toggle_mask_assert)
{
	cute_expect_assertion(stroll_bmap_toggle_maskul(NULL, 0xf));
}
#else
CUTE_TEST(strollut_bmap_word_toggle_mask_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_toggle_mask,
                 strollut_bmap_word_setup_toggle_mask,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_mask_oper(strollut_bmap_word_toggle_mask_oper);
}

static void
strollut_bmap_word_setup_toggle_range(void)
{
	strollut_bmap_word_setup_range_oper(strollut_bmap_word_xor_oper);
}

static unsigned long
strollut_bmap_word_toggle_range_oper(unsigned long bmap,
                                     unsigned int  start_bit,
                                     unsigned int  bit_count)
{
	unsigned long bmp = bmap;

	stroll_bmap_toggle_rangeul(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_toggle_range_assert)
{
	unsigned long bmp = 0;

	cute_expect_assertion(stroll_bmap_toggle_rangeul(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_toggle_rangeul(&bmp, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_toggle_rangeul(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_toggle_rangeul(&bmp, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_toggle_rangeul(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_toggle_rangeul(&bmp, 30, 3));
#endif
}
#else
CUTE_TEST(strollut_bmap_word_toggle_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap_word_toggle_range,
                 strollut_bmap_word_setup_toggle_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap_word_run_range_oper(strollut_bmap_word_toggle_range_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_toggle_all_assert)
{
	cute_expect_assertion(stroll_bmap_toggle_allul(NULL));
}
#else
CUTE_TEST(strollut_bmap_word_toggle_all_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_toggle_all)
{
	unsigned int b;
	unsigned int bnr = stroll_array_nr(strollut_bmap_words);

	for (b = 0; b < bnr; b++) {
		unsigned long bmp = strollut_bmap_words[b];
		unsigned long exp = bmp ^ ~(0UL);

		stroll_bmap_toggle_allul(&bmp);
		cute_check_hex(bmp, equal, exp);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_set_iter_assert)
{
	unsigned int  b;

#if __WORDSIZE == 64
	uint64_t iter;
	uint64_t bmp = 0;

	cute_expect_assertion(_stroll_bmap_setup_set_iter64(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_set_iter64(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter64(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter64(&iter, NULL));
#elif __WORDSIZE == 32
	uint32_t iter;
	uint32_t bmp = 0;

	cute_expect_assertion(_stroll_bmap_setup_set_iter32(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_set_iter32(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter32(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter32(&iter, NULL));
#endif
}
#else
CUTE_TEST(strollut_bmap_word_set_iter_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_set_iter)
{
	unsigned long iter;
	unsigned long bmp = 0;
	unsigned int  b;
	unsigned int  m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words); m++) {
		unsigned int  e = 0;

		bmp = strollut_bmap_words[m];
		b = 0;
		stroll_bmap_foreach_setul(&iter, bmp, &b) {
			while ((e < stroll_bops_bitsof(bmp)) &&
			       !(bmp & (1UL << e)))
				e++;
			cute_check_uint(b, equal, e);
			e++;
		}

		if (!bmp)
			cute_check_uint(b, equal, (unsigned int)-1);
		else
			cute_check_uint_range(
				b,
				in,
				CUTE_UINT_RANGE(0,
				                stroll_bops_bitsof(bmp) - 1));
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_clear_iter_assert)
{
	unsigned int  b;

#if __WORDSIZE == 64
	uint64_t iter;
	uint64_t bmp = 0;

	cute_expect_assertion(_stroll_bmap_setup_clear_iter64(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_clear_iter64(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter64(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter64(&iter, NULL));
#elif __WORDSIZE == 32
	uint32_t iter;
	uint32_t bmp = 0;

	cute_expect_assertion(_stroll_bmap_setup_clear_iter32(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_clear_iter32(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter32(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter32(&iter, NULL));
#endif
}
#else
CUTE_TEST(strollut_bmap_word_clear_iter_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap_word_clear_iter)
{
	unsigned long iter;
	unsigned long bmp = 0;
	unsigned int  b;
	unsigned int  m;

	for (m = 0; m < stroll_array_nr(strollut_bmap_words); m++) {
		unsigned int  e = 0;

		bmp = strollut_bmap_words[m];
		b = 0;
		stroll_bmap_foreach_clearul(&iter, bmp, &b) {
			while ((e < stroll_bops_bitsof(bmp)) &&
			       (bmp & (1UL << e)))
				e++;
			cute_check_uint(b, equal, e);
			e++;
		}

		if (bmp == ~(0UL))
			cute_check_uint(b, equal, (unsigned int)-1);
		else
			cute_check_uint_range(
				b,
				in,
				CUTE_UINT_RANGE(0,
				                stroll_bops_bitsof(bmp) - 1));
	}
}

CUTE_GROUP(strollut_bmap_word_group) = {
	CUTE_REF(strollut_bmap_word_setup_assert),
	CUTE_REF(strollut_bmap_word_setup),
	CUTE_REF(strollut_bmap_word_mask_assert),
	CUTE_REF(strollut_bmap_word_mask),
	CUTE_REF(strollut_bmap_word_hweight),

	CUTE_REF(strollut_bmap_word_and),
	CUTE_REF(strollut_bmap_word_and_range_assert),
	CUTE_REF(strollut_bmap_word_and_range),

	CUTE_REF(strollut_bmap_word_or),
	CUTE_REF(strollut_bmap_word_or_range_assert),
	CUTE_REF(strollut_bmap_word_or_range),

	CUTE_REF(strollut_bmap_word_xor),
	CUTE_REF(strollut_bmap_word_xor_range_assert),
	CUTE_REF(strollut_bmap_word_xor_range),

	CUTE_REF(strollut_bmap_word_test_bit),
	CUTE_REF(strollut_bmap_word_test_all),
	CUTE_REF(strollut_bmap_word_test_mask),
	CUTE_REF(strollut_bmap_word_test_range_assert),
	CUTE_REF(strollut_bmap_word_test_range),

	CUTE_REF(strollut_bmap_word_set_bit_assert),
	CUTE_REF(strollut_bmap_word_set_bit),
	CUTE_REF(strollut_bmap_word_set_mask_assert),
	CUTE_REF(strollut_bmap_word_set_mask),
	CUTE_REF(strollut_bmap_word_set_range_assert),
	CUTE_REF(strollut_bmap_word_set_range),
	CUTE_REF(strollut_bmap_word_set_all_assert),
	CUTE_REF(strollut_bmap_word_set_all),

	CUTE_REF(strollut_bmap_word_clear_bit_assert),
	CUTE_REF(strollut_bmap_word_clear_bit),
	CUTE_REF(strollut_bmap_word_clear_mask_assert),
	CUTE_REF(strollut_bmap_word_clear_mask),
	CUTE_REF(strollut_bmap_word_clear_range_assert),
	CUTE_REF(strollut_bmap_word_clear_range),
	CUTE_REF(strollut_bmap_word_clear_all_assert),
	CUTE_REF(strollut_bmap_word_clear_all),

	CUTE_REF(strollut_bmap_word_toggle_bit_assert),
	CUTE_REF(strollut_bmap_word_toggle_bit),
	CUTE_REF(strollut_bmap_word_toggle_mask_assert),
	CUTE_REF(strollut_bmap_word_toggle_mask),
	CUTE_REF(strollut_bmap_word_toggle_range_assert),
	CUTE_REF(strollut_bmap_word_toggle_range),
	CUTE_REF(strollut_bmap_word_toggle_all_assert),
	CUTE_REF(strollut_bmap_word_toggle_all),

	CUTE_REF(strollut_bmap_word_set_iter_assert),
	CUTE_REF(strollut_bmap_word_set_iter),
	CUTE_REF(strollut_bmap_word_clear_iter_assert),
	CUTE_REF(strollut_bmap_word_clear_iter)
};

CUTE_SUITE(strollut_bmap_word_suite, strollut_bmap_word_group);

/******************************************************************************
 * Top-level bitmap support
 ******************************************************************************/

CUTE_GROUP(strollut_bmap_group) = {
	CUTE_REF(strollut_bmap_suite32),
	CUTE_REF(strollut_bmap_suite64),
	CUTE_REF(strollut_bmap_word_suite),
};

CUTE_SUITE_EXTERN(strollut_bmap_suite,
                  strollut_bmap_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
