#include "stroll/bmap.h"
#include "utest.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <stdlib.h>

#if 0
#include "utest.h"
#include "stroll/bmap.h"
#include <stdlib.h>

static int
strollut_bmap32_setup_test_mask(void ** state)
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(strollut_bmap32_words);
	unsigned int   mnr = array_nr(strollut_bmap32_masks);
	bool         * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool * exp = &expected[(b * mnr) + m];

			*exp = !!(strollut_bmap32_words[b] &
			          strollut_bmap32_masks[m]);
		}
	}

	*state = expected;

	return 0;
}

static void
strollut_bmap32_test_mask(void ** state)
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(strollut_bmap32_words);
	unsigned int   mnr = array_nr(strollut_bmap32_masks);
	const bool   * expected = *state;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool res;

			res = stroll_bmap32_test_mask(
				strollut_bmap32_words[b],
				strollut_bmap32_masks[m]);
			assert_true(res == expected[(b * mnr) + m]);
		}
	}
}

static int
strollut_bmap32_setup_test_range(void ** state)
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(strollut_bmap32_words);
	unsigned int   rnr = array_nr(strollut_bmap32_ranges);
	bool         * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool * exp = &expected[(b * rnr) + r];

			*exp = !!(strollut_bmap32_words[b] &
			          strollut_bmap32_ranges[r].mask);
		}
	}

	*state = expected;

	return 0;
}

static void
strollut_bmap32_test_range(void ** state)
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(strollut_bmap32_words);
	unsigned int   rnr = array_nr(strollut_bmap32_ranges);
	const bool   * expected = *state;

#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap32_test_range(0, 0, 0));
	cute_expect_assertion(stroll_bmap32_test_range(0, 32, 1));
	cute_expect_assertion(stroll_bmap32_test_range(0, 30, 3));
#pragma GCC diagnostic pop
#endif

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool res;

			res = stroll_bmap32_test_range(
				strollut_bmap32_words[b],
				strollut_bmap32_ranges[r].start,
				strollut_bmap32_ranges[r].count);
			assert_true(res == expected[(b * rnr) + r]);
		}
	}
}

static void
strollut_bmap32_set_bit(void ** state __unused)
{
	uint32_t     bmp = 0;
	unsigned int b;


#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_set(NULL, 1));
#endif

	for (b = 0; b < 32; b++) {
		bmp = 0;
		stroll_bmap32_set(&bmp, b);
		assert_true(bmp & (1U << b));
		assert_false(!!(bmp & ~(1U << b)));
	}
}

static uint32_t
strollut_bmap32_set_mask_oper(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap32_set_mask(&bmp, mask);

	return bmp;
}

static void
strollut_bmap32_set_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_set_mask(NULL, 0xf));
#endif
	strollut_bmap32_run_mask_oper(state,
	                                  strollut_bmap32_set_mask_oper);
}

static uint32_t
strollut_bmap32_set_range_oper(uint32_t     bmap,
                                   unsigned int start_bit,
                                   unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap32_set_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
strollut_bmap32_set_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint32_t bmp = 0;

	cute_expect_assertion(stroll_bmap32_set_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap32_set_range(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap32_set_range(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap32_set_range(&bmp, 30, 3));
#endif
	strollut_bmap32_run_range_oper(state,
	                                   strollut_bmap32_set_range_oper);
}

static void
strollut_bmap32_set_all(void ** state __unused)
{
	uint32_t bmp = 0;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_set_all(NULL));
#endif

	stroll_bmap32_set_all(&bmp);
	assert_int_equal(bmp, ~(0U));
}

static void
strollut_bmap32_clear_bit(void ** state __unused)
{
	uint32_t     bmp = ~(0U);
	unsigned int b;


#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_clear(NULL, 1));
#endif

	for (b = 0; b < 32; b++) {
		bmp = ~(0U);
		stroll_bmap32_clear(&bmp, b);
		assert_int_equal(bmp, ~(0U) & ~(1U << b));
	}
}

static uint32_t
strollut_bmap32_notand_oper(uint32_t bmap, uint32_t mask)
{
	return bmap & ~mask;
}

static int
strollut_bmap32_setup_clear_mask(void ** state)
{
	return strollut_bmap32_setup_mask_oper(
		state,
		strollut_bmap32_notand_oper);
}

static uint32_t
strollut_bmap32_clear_mask_oper(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap32_clear_mask(&bmp, mask);

	return bmp;
}

static void
strollut_bmap32_clear_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_clear_mask(NULL, 0xf));
#endif
	strollut_bmap32_run_mask_oper(state,
	                                  strollut_bmap32_clear_mask_oper);
}

static int
strollut_bmap32_setup_clear_range(void ** state)
{
	return strollut_bmap32_setup_range_oper(
		state,
		strollut_bmap32_notand_oper);
}

static uint32_t
strollut_bmap32_clear_range_oper(uint32_t     bmap,
                                     unsigned int start_bit,
                                     unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap32_clear_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
strollut_bmap32_clear_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint32_t bmp = ~(0U);

	cute_expect_assertion(stroll_bmap32_clear_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap32_clear_range(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap32_clear_range(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap32_clear_range(&bmp, 30, 3));
#endif
	strollut_bmap32_run_range_oper(
		state,
		strollut_bmap32_clear_range_oper);
}

static void
strollut_bmap32_clear_all(void ** state __unused)
{
	uint32_t bmp = ~(0U);

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_clear_all(NULL));
#endif

	stroll_bmap32_clear_all(&bmp);
	assert_int_equal(bmp, 0);
}

static void
strollut_bmap32_toggle_bit(void ** state __unused)
{
	uint32_t     bmp = 0;
	unsigned int b;


#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_toggle(NULL, 1));
#endif

	for (b = 0; b < 32; b++) {
		bmp = 0;
		stroll_bmap32_toggle(&bmp, b);
		assert_int_equal(bmp, 1U << b);

		bmp = ~(0U);
		stroll_bmap32_toggle(&bmp, b);
		assert_int_equal(bmp, ~(1U << b));
	}
}

static int
strollut_bmap32_setup_toggle_mask(void ** state)
{
	return strollut_bmap32_setup_mask_oper(
		state,
		strollut_bmap32_xor_oper);
}

static uint32_t
strollut_bmap32_toggle_mask_oper(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap32_toggle_mask(&bmp, mask);

	return bmp;
}

static void
strollut_bmap32_toggle_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_toggle_mask(NULL, 0xf));
#endif
	strollut_bmap32_run_mask_oper(state,
	                                  strollut_bmap32_toggle_mask_oper);
}

static int
strollut_bmap32_setup_toggle_range(void ** state)
{
	return strollut_bmap32_setup_range_oper(
		state,
		strollut_bmap32_xor_oper);
}

static uint32_t
strollut_bmap32_toggle_range_oper(uint32_t     bmap,
                                      unsigned int start_bit,
                                      unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap32_toggle_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
strollut_bmap32_toggle_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint32_t bmp = 0;

	cute_expect_assertion(stroll_bmap32_toggle_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap32_toggle_range(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap32_toggle_range(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap32_toggle_range(&bmp, 30, 3));
#endif
	strollut_bmap32_run_range_oper(
		state,
		strollut_bmap32_toggle_range_oper);
}

static void
strollut_bmap32_toggle_all(void ** state __unused)
{
	unsigned int   b;
	unsigned int   bnr = array_nr(strollut_bmap32_words);

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap32_toggle_all(NULL));
#endif
	for (b = 0; b < bnr; b++) {
		uint32_t bmp = strollut_bmap32_words[b];
		uint32_t exp = bmp ^ UINT32_MAX;

		stroll_bmap32_toggle_all(&bmp);
		assert_int_equal(bmp, exp);
	}
}

static void
strollut_bmap32_set_iter(void ** state __unused)
{
	uint32_t     iter;
	uint32_t     bmp = 0;
	unsigned int b;
	unsigned int m;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(_stroll_bmap32_setup_set_iter(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap32_setup_set_iter(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap32_step_iter(NULL, &b));
	cute_expect_assertion(_stroll_bmap32_step_iter(&iter, NULL));
#endif

	for (m = 0; m < array_nr(strollut_bmap32_words); m++) {
		unsigned int e = 0;

		bmp = strollut_bmap32_words[m];
		b = 0;
		stroll_bmap32_foreach_set(&iter, bmp, &b) {
			while ((e < 32) && !(bmp & (UINT32_C(1) << e)))
				e++;
			assert_int_equal(b, e);
			e++;
		}

		if (!bmp)
			assert_int_equal(b, (unsigned int)-1);
		else
			assert_in_range(b, 0, 31);
	}
}

static void
strollut_bmap32_clear_iter(void ** state __unused)
{
	uint32_t     iter;
	uint32_t     bmp = 0;
	unsigned int b;
	unsigned int m;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(_stroll_bmap32_setup_clear_iter(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap32_setup_clear_iter(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap32_step_iter(NULL, &b));
	cute_expect_assertion(_stroll_bmap32_step_iter(&iter, NULL));
#endif

	for (m = 0; m < array_nr(strollut_bmap32_words); m++) {
		unsigned int e = 0;

		bmp = strollut_bmap32_words[m];
		b = 0;
		stroll_bmap32_foreach_clear(&iter, bmp, &b) {
			while ((e < 32) && (bmp & (UINT32_C(1) << e)))
				e++;
			assert_int_equal(b, e);
			e++;
		}

		if (bmp == UINT32_MAX)
			assert_int_equal(b, (unsigned int)-1);
		else
			assert_in_range(b, 0, 31);
	}
}

static const struct CMUnitTest stroll_bmap32_utests[] = {
	cmocka_unit_test_setup_teardown(strollut_bmap32_test_mask,
	                                strollut_bmap32_setup_test_mask,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(strollut_bmap32_test_range,
	                                strollut_bmap32_setup_test_range,
	                                strollut_bmap_teardown),

	cmocka_unit_test(strollut_bmap32_set_bit),
	cmocka_unit_test_setup_teardown(strollut_bmap32_set_mask,
	                                strollut_bmap32_setup_or,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(strollut_bmap32_set_range,
	                                strollut_bmap32_setup_or_range,
	                                strollut_bmap_teardown),
	cmocka_unit_test(strollut_bmap32_set_all),

	cmocka_unit_test(strollut_bmap32_clear_bit),
	cmocka_unit_test_setup_teardown(strollut_bmap32_clear_mask,
	                                strollut_bmap32_setup_clear_mask,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(strollut_bmap32_clear_range,
	                                strollut_bmap32_setup_clear_range,
	                                strollut_bmap_teardown),
	cmocka_unit_test(strollut_bmap32_clear_all),

	cmocka_unit_test(strollut_bmap32_toggle_bit),
	cmocka_unit_test_setup_teardown(strollut_bmap32_toggle_mask,
	                                strollut_bmap32_setup_toggle_mask,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(strollut_bmap32_toggle_range,
	                                strollut_bmap32_setup_toggle_range,
	                                strollut_bmap_teardown),
	cmocka_unit_test(strollut_bmap32_toggle_all),

	cmocka_unit_test(strollut_bmap32_set_iter),
	cmocka_unit_test(strollut_bmap32_clear_iter)
};

/******************************************************************************
 * Unsigned 64-bits support
 ******************************************************************************/

static const uint64_t stroll_bmap64_utest_bmaps[] = {
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

static const uint64_t stroll_bmap64_utest_masks[] = {
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

static const struct stroll_bmap64_utest_range {
	uint64_t     mask;
	unsigned int start;
	unsigned int count;
} stroll_bmap64_utest_ranges[] = {
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

static int
stroll_bmap64_utest_setup_mask_oper(void     ** state,
                                    uint64_t (* oper)(uint64_t, uint64_t))
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(stroll_bmap64_utest_bmaps);
	unsigned int   mnr = array_nr(stroll_bmap64_utest_masks);
	uint64_t     * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			uint64_t * exp = &expected[(b * mnr) + m];

			*exp = oper(stroll_bmap64_utest_bmaps[b],
			            stroll_bmap64_utest_masks[m]);
		}
	}

	*state = expected;

	return 0;
}

static void
stroll_bmap64_utest_run_mask_oper(void      ** state,
                                  uint64_t  (* oper)(uint64_t, uint64_t))
{
	unsigned int     b, m;
	unsigned int     bnr = array_nr(stroll_bmap64_utest_bmaps);
	unsigned int     mnr = array_nr(stroll_bmap64_utest_masks);
	const uint64_t * expected = *state;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			assert_int_equal(oper(stroll_bmap64_utest_bmaps[b],
			                      stroll_bmap64_utest_masks[m]),
			                 expected[(b * mnr) + m]);
		}
	}
}

static int
stroll_bmap64_utest_setup_range_oper(void     ** state,
                                     uint64_t (* oper)(uint64_t, uint64_t))
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(stroll_bmap64_utest_bmaps);
	unsigned int   rnr = array_nr(stroll_bmap64_utest_ranges);
	uint64_t     * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			uint64_t * exp = &expected[(b * rnr) + r];

			*exp = oper(stroll_bmap64_utest_bmaps[b],
			            stroll_bmap64_utest_ranges[r].mask);
		}
	}

	*state = expected;

	return 0;
}

static void
stroll_bmap64_utest_run_range_oper(void     ** state,
                                   uint64_t (* oper)(uint64_t,
                                                     unsigned int,
                                                     unsigned int))
{
	unsigned int     b, r;
	unsigned int     bnr = array_nr(stroll_bmap64_utest_bmaps);
	unsigned int     rnr = array_nr(stroll_bmap64_utest_ranges);
	const uint64_t * expected = *state;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			assert_int_equal(
				oper(stroll_bmap64_utest_bmaps[b],
				     stroll_bmap64_utest_ranges[r].start,
				     stroll_bmap64_utest_ranges[r].count),
				expected[(b * rnr) + r]);
		}
	}
}

static void
stroll_bmap64_utest_init(void ** state __unused)
{
	uint64_t bmp = 0x5A5A5A5A5A5A5A5A;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_setup_set(NULL));
#endif
	stroll_bmap64_setup_set(&bmp);
	assert_int_equal(bmp, UINT64_MAX);

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_setup_clear(NULL));
#endif
	stroll_bmap64_setup_clear(&bmp);
	assert_int_equal(bmp, 0);
}

static void
stroll_bmap64_utest_mask(void ** state __unused)
{
	unsigned int r;

#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap64_mask(0, 0));
	cute_expect_assertion(stroll_bmap64_mask(0, 65));
	cute_expect_assertion(stroll_bmap64_mask(3, 63));
#pragma GCC diagnostic pop
#endif

	for (r = 0; r < array_nr(stroll_bmap64_utest_ranges); r++) {
		const struct stroll_bmap64_utest_range * rng;

		rng = &stroll_bmap64_utest_ranges[r];

		assert_int_equal(stroll_bmap64_mask(rng->start, rng->count),
		                 rng->mask);
	}
}

static void
stroll_bmap64_utest_hweight(void ** state __unused)
{
	unsigned int m;

	for (m = 0; m < array_nr(stroll_bmap64_utest_bmaps); m++) {
		uint64_t     bmp = stroll_bmap64_utest_bmaps[m];
		unsigned int b;
		unsigned int cnt;

		for (b = 0, cnt = 0; b < 64; b++)
			if (bmp & (UINT64_C(1) << b))
				cnt++;

		assert_int_equal(cnt, stroll_bmap64_hweight(bmp));
	}
}

static uint64_t
stroll_bmap64_utest_and_oper(uint64_t bmap, uint64_t mask)
{
	return bmap & mask;
}

static int
stroll_bmap64_utest_setup_and(void ** state)
{
	return stroll_bmap64_utest_setup_mask_oper(
		state,
		stroll_bmap64_utest_and_oper);
}

static void
stroll_bmap64_utest_and(void ** state)
{
	stroll_bmap64_utest_run_mask_oper(state, stroll_bmap64_and);
}

static int
stroll_bmap64_utest_setup_and_range(void ** state)
{
	return stroll_bmap64_utest_setup_range_oper(
		state,
		stroll_bmap64_utest_and_oper);
}

static void
stroll_bmap64_utest_and_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap64_and_range(0, 0, 0));
	cute_expect_assertion(stroll_bmap64_and_range(0, 64, 1));
	cute_expect_assertion(stroll_bmap64_and_range(0, 60, 5));
#pragma GCC diagnostic pop
#endif
	stroll_bmap64_utest_run_range_oper(state, stroll_bmap64_and_range);
}

static uint64_t
stroll_bmap64_utest_or_oper(uint64_t bmap, uint64_t mask)
{
	return bmap | mask;
}

static int
stroll_bmap64_utest_setup_or(void ** state)
{
	return stroll_bmap64_utest_setup_mask_oper(
		state,
		stroll_bmap64_utest_or_oper);
}

static void
stroll_bmap64_utest_or(void ** state)
{
	stroll_bmap64_utest_run_mask_oper(state, stroll_bmap64_or);
}

static int
stroll_bmap64_utest_setup_or_range(void ** state)
{
	return stroll_bmap64_utest_setup_range_oper(
		state,
		stroll_bmap64_utest_or_oper);
}

static void
stroll_bmap64_utest_or_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap64_or_range(0, 0, 0));
	cute_expect_assertion(stroll_bmap64_or_range(0, 64, 1));
	cute_expect_assertion(stroll_bmap64_or_range(0, 60, 5));
#pragma GCC diagnostic pop
#endif
	stroll_bmap64_utest_run_range_oper(state, stroll_bmap64_or_range);
}

static uint64_t
stroll_bmap64_utest_xor_oper(uint64_t bmap, uint64_t mask)
{
	return bmap ^ mask;
}

static int
stroll_bmap64_utest_setup_xor(void ** state)
{
	return stroll_bmap64_utest_setup_mask_oper(
		state,
		stroll_bmap64_utest_xor_oper);
}

static void
stroll_bmap64_utest_xor(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap64_xor_range(0, 0, 0));
	cute_expect_assertion(stroll_bmap64_xor_range(0, 64, 1));
	cute_expect_assertion(stroll_bmap64_xor_range(0, 60, 5));
#pragma GCC diagnostic pop
#endif
	stroll_bmap64_utest_run_mask_oper(state, stroll_bmap64_xor);
}

static int
stroll_bmap64_utest_setup_xor_range(void ** state)
{
	return stroll_bmap64_utest_setup_range_oper(
		state,
		stroll_bmap64_utest_xor_oper);
}

static void
stroll_bmap64_utest_xor_range(void ** state)
{
	stroll_bmap64_utest_run_range_oper(state, stroll_bmap64_xor_range);
}

static void
stroll_bmap64_utest_test_bit(void ** state __unused)
{
	unsigned int b, m;
	unsigned int mnr = array_nr(stroll_bmap64_utest_bmaps);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 64; b++) {
			uint64_t bmp = stroll_bmap64_utest_bmaps[m];

			if (bmp & (UINT64_C(1) << b))
				assert_true(stroll_bmap64_test(bmp, b));
			else
				assert_false(stroll_bmap64_test(bmp, b));
		}
	}
}

static void
stroll_bmap64_utest_test_all(void ** state __unused)
{
	unsigned int b, m;
	unsigned int mnr = array_nr(stroll_bmap64_utest_bmaps);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 64; b++) {
			uint64_t bmp = stroll_bmap64_utest_bmaps[m];

			if (!!bmp)
				assert_true(stroll_bmap64_test_all(bmp));
			else
				assert_false(stroll_bmap64_test_all(bmp));
		}
	}
}


static int
stroll_bmap64_utest_setup_test_mask(void ** state)
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(stroll_bmap64_utest_bmaps);
	unsigned int   mnr = array_nr(stroll_bmap64_utest_masks);
	bool         * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool * exp = &expected[(b * mnr) + m];

			*exp = !!(stroll_bmap64_utest_bmaps[b] &
			          stroll_bmap64_utest_masks[m]);
		}
	}

	*state = expected;

	return 0;
}

static void
stroll_bmap64_utest_test_mask(void ** state)
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(stroll_bmap64_utest_bmaps);
	unsigned int   mnr = array_nr(stroll_bmap64_utest_masks);
	const bool   * expected = *state;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool res;

			res = stroll_bmap64_test_mask(
				stroll_bmap64_utest_bmaps[b],
				stroll_bmap64_utest_masks[m]);
			assert_true(res == expected[(b * mnr) + m]);
		}
	}
}

static int
stroll_bmap64_utest_setup_test_range(void ** state)
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(stroll_bmap64_utest_bmaps);
	unsigned int   rnr = array_nr(stroll_bmap64_utest_ranges);
	bool         * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool * exp = &expected[(b * rnr) + r];

			*exp = !!(stroll_bmap64_utest_bmaps[b] &
			          stroll_bmap64_utest_ranges[r].mask);
		}
	}

	*state = expected;

	return 0;
}

static void
stroll_bmap64_utest_test_range(void ** state)
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(stroll_bmap64_utest_bmaps);
	unsigned int   rnr = array_nr(stroll_bmap64_utest_ranges);
	const bool   * expected = *state;

#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap64_test_range(0, 0, 0));
	cute_expect_assertion(stroll_bmap64_test_range(0, 64, 1));
	cute_expect_assertion(stroll_bmap64_test_range(0, 60, 5));
#pragma GCC diagnostic pop
#endif

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool res;

			res = stroll_bmap64_test_range(
				stroll_bmap64_utest_bmaps[b],
				stroll_bmap64_utest_ranges[r].start,
				stroll_bmap64_utest_ranges[r].count);
			assert_true(res == expected[(b * rnr) + r]);
		}
	}
}

static void
stroll_bmap64_utest_set_bit(void ** state __unused)
{
	uint64_t     bmp = 0;
	unsigned int b;


#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_set(NULL, 1));
#endif

	for (b = 0; b < 64; b++) {
		bmp = 0;
		stroll_bmap64_set(&bmp, b);
		assert_true(bmp & (UINT64_C(1) << b));
		assert_false(!!(bmp & ~(UINT64_C(1) << b)));
	}
}

static uint64_t
stroll_bmap64_utest_set_mask_oper(uint64_t bmap, uint64_t mask)
{
	uint64_t bmp = bmap;

	stroll_bmap64_set_mask(&bmp, mask);

	return bmp;
}

static void
stroll_bmap64_utest_set_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_set_mask(NULL, 0xf));
#endif
	stroll_bmap64_utest_run_mask_oper(state,
	                                  stroll_bmap64_utest_set_mask_oper);
}

static uint64_t
stroll_bmap64_utest_set_range_oper(uint64_t     bmap,
                                   unsigned int start_bit,
                                   unsigned int bit_count)
{
	uint64_t bmp = bmap;

	stroll_bmap64_set_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
stroll_bmap64_utest_set_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint64_t bmp = 0;

	cute_expect_assertion(stroll_bmap64_set_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap64_set_range(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap64_set_range(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap64_set_range(&bmp, 60, 5));
#endif
	stroll_bmap64_utest_run_range_oper(state,
	                                   stroll_bmap64_utest_set_range_oper);
}

static void
stroll_bmap64_utest_set_all(void ** state __unused)
{
	uint64_t bmp = 0;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_set_all(NULL));
#endif

	stroll_bmap64_set_all(&bmp);
	assert_int_equal(bmp, UINT64_MAX);
}

static void
stroll_bmap64_utest_clear_bit(void ** state __unused)
{
	uint64_t     bmp = UINT64_MAX;
	unsigned int b;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_clear(NULL, 1));
#endif

	for (b = 0; b < 64; b++) {
		bmp = UINT64_MAX;
		stroll_bmap64_clear(&bmp, b);
		assert_int_equal(bmp, UINT64_MAX & ~(UINT64_C(1) << b));
	}
}

static uint64_t
stroll_bmap64_utest_notand_oper(uint64_t bmap, uint64_t mask)
{
	return bmap & ~mask;
}

static int
stroll_bmap64_utest_setup_clear_mask(void ** state)
{
	return stroll_bmap64_utest_setup_mask_oper(
		state,
		stroll_bmap64_utest_notand_oper);
}

static uint64_t
stroll_bmap64_utest_clear_mask_oper(uint64_t bmap, uint64_t mask)
{
	uint64_t bmp = bmap;

	stroll_bmap64_clear_mask(&bmp, mask);

	return bmp;
}

static void
stroll_bmap64_utest_clear_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_clear_mask(NULL, 0xf));
#endif
	stroll_bmap64_utest_run_mask_oper(state,
	                                  stroll_bmap64_utest_clear_mask_oper);
}

static int
stroll_bmap64_utest_setup_clear_range(void ** state)
{
	return stroll_bmap64_utest_setup_range_oper(
		state,
		stroll_bmap64_utest_notand_oper);
}

static uint64_t
stroll_bmap64_utest_clear_range_oper(uint64_t     bmap,
                                     unsigned int start_bit,
                                     unsigned int bit_count)
{
	uint64_t bmp = bmap;

	stroll_bmap64_clear_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
stroll_bmap64_utest_clear_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint64_t bmp = UINT64_MAX;

	cute_expect_assertion(stroll_bmap64_clear_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap64_clear_range(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap64_clear_range(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap64_clear_range(&bmp, 60, 5));
#endif
	stroll_bmap64_utest_run_range_oper(
		state,
		stroll_bmap64_utest_clear_range_oper);
}

static void
stroll_bmap64_utest_clear_all(void ** state __unused)
{
	uint64_t bmp = UINT64_MAX;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_clear_all(NULL));
#endif

	stroll_bmap64_clear_all(&bmp);
	assert_int_equal(bmp, 0);
}

static void
stroll_bmap64_utest_toggle_bit(void ** state __unused)
{
	uint64_t     bmp = 0;
	unsigned int b;


#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_toggle(NULL, 1));
#endif

	for (b = 0; b < 64; b++) {
		bmp = 0;
		stroll_bmap64_toggle(&bmp, b);
		assert_int_equal(bmp, UINT64_C(1) << b);

		bmp = UINT64_MAX;
		stroll_bmap64_toggle(&bmp, b);
		assert_int_equal(bmp, ~(UINT64_C(1) << b));
	}
}

static int
stroll_bmap64_utest_setup_toggle_mask(void ** state)
{
	return stroll_bmap64_utest_setup_mask_oper(
		state,
		stroll_bmap64_utest_xor_oper);
}

static uint64_t
stroll_bmap64_utest_toggle_mask_oper(uint64_t bmap, uint64_t mask)
{
	uint64_t bmp = bmap;

	stroll_bmap64_toggle_mask(&bmp, mask);

	return bmp;
}

static void
stroll_bmap64_utest_toggle_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_toggle_mask(NULL, 0xf));
#endif
	stroll_bmap64_utest_run_mask_oper(state,
	                                  stroll_bmap64_utest_toggle_mask_oper);
}

static int
stroll_bmap64_utest_setup_toggle_range(void ** state)
{
	return stroll_bmap64_utest_setup_range_oper(
		state,
		stroll_bmap64_utest_xor_oper);
}

static uint64_t
stroll_bmap64_utest_toggle_range_oper(uint64_t     bmap,
                                      unsigned int start_bit,
                                      unsigned int bit_count)
{
	uint64_t bmp = bmap;

	stroll_bmap64_toggle_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
stroll_bmap64_utest_toggle_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint64_t bmp = 0;

	cute_expect_assertion(stroll_bmap64_toggle_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap64_toggle_range(&bmp, 0, 0));
	cute_expect_assertion(stroll_bmap64_toggle_range(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap64_toggle_range(&bmp, 60, 5));
#endif
	stroll_bmap64_utest_run_range_oper(
		state,
		stroll_bmap64_utest_toggle_range_oper);
}

static void
stroll_bmap64_utest_toggle_all(void ** state __unused)
{
	unsigned int b;
	unsigned int bnr = array_nr(stroll_bmap64_utest_bmaps);

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(stroll_bmap64_toggle_all(NULL));
#endif
	for (b = 0; b < bnr; b++) {
		uint64_t bmp = stroll_bmap64_utest_bmaps[b];
		uint64_t exp = bmp ^ UINT64_MAX;

		stroll_bmap64_toggle_all(&bmp);
		assert_int_equal(bmp, exp);
	}
}

static void
stroll_bmap64_utest_set_iter(void ** state __unused)
{
	uint64_t     iter;
	uint64_t     bmp = 0;
	unsigned int b;
	unsigned int m;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(_stroll_bmap64_setup_set_iter(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap64_setup_set_iter(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap64_step_iter(NULL, &b));
	cute_expect_assertion(_stroll_bmap64_step_iter(&iter, NULL));
#endif

	for (m = 0; m < array_nr(stroll_bmap64_utest_bmaps); m++) {
		unsigned int e = 0;

		bmp = stroll_bmap64_utest_bmaps[m];
		b = 0;
		stroll_bmap64_foreach_set(&iter, bmp, &b) {
			while ((e < 64) && !(bmp & (UINT64_C(1) << e)))
				e++;
			assert_int_equal(b, e);
			e++;
		}

		if (!bmp)
			assert_int_equal(b, (unsigned int)-1);
		else
			assert_in_range(b, 0, 63);
	}
}

static void
stroll_bmap64_utest_clear_iter(void ** state __unused)
{
	uint64_t     iter;
	uint64_t     bmp = 0;
	unsigned int b;
	unsigned int m;

#if defined(CONFIG_STROLL_ASSERT_API)
	cute_expect_assertion(_stroll_bmap64_setup_clear_iter(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap64_setup_clear_iter(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap64_step_iter(NULL, &b));
	cute_expect_assertion(_stroll_bmap64_step_iter(&iter, NULL));
#endif

	for (m = 0; m < array_nr(stroll_bmap64_utest_bmaps); m++) {
		unsigned int e = 0;

		bmp = stroll_bmap64_utest_bmaps[m];
		b = 0;
		stroll_bmap64_foreach_clear(&iter, bmp, &b) {
			while ((e < 64) && (bmp & (UINT64_C(1) << e)))
				e++;
			assert_int_equal(b, e);
			e++;
		}

		if (bmp == UINT64_MAX)
			assert_int_equal(b, (unsigned int)-1);
		else
			assert_in_range(b, 0, 63);
	}
}

static const struct CMUnitTest stroll_bmap64_utests[] = {
	cmocka_unit_test(stroll_bmap64_utest_init),
	cmocka_unit_test(stroll_bmap64_utest_mask),
	cmocka_unit_test(stroll_bmap64_utest_hweight),

	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_and,
	                                stroll_bmap64_utest_setup_and,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_and_range,
	                                stroll_bmap64_utest_setup_and_range,
	                                strollut_bmap_teardown),

	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_or,
	                                stroll_bmap64_utest_setup_or,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_or_range,
	                                stroll_bmap64_utest_setup_or_range,
	                                strollut_bmap_teardown),

	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_xor,
	                                stroll_bmap64_utest_setup_xor,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_xor_range,
	                                stroll_bmap64_utest_setup_xor_range,
	                                strollut_bmap_teardown),

	cmocka_unit_test(stroll_bmap64_utest_test_bit),
	cmocka_unit_test(stroll_bmap64_utest_test_all),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_test_mask,
	                                stroll_bmap64_utest_setup_test_mask,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_test_range,
	                                stroll_bmap64_utest_setup_test_range,
	                                strollut_bmap_teardown),

	cmocka_unit_test(stroll_bmap64_utest_set_bit),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_set_mask,
	                                stroll_bmap64_utest_setup_or,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_set_range,
	                                stroll_bmap64_utest_setup_or_range,
	                                strollut_bmap_teardown),
	cmocka_unit_test(stroll_bmap64_utest_set_all),

	cmocka_unit_test(stroll_bmap64_utest_clear_bit),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_clear_mask,
	                                stroll_bmap64_utest_setup_clear_mask,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_clear_range,
	                                stroll_bmap64_utest_setup_clear_range,
	                                strollut_bmap_teardown),
	cmocka_unit_test(stroll_bmap64_utest_clear_all),

	cmocka_unit_test(stroll_bmap64_utest_toggle_bit),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_toggle_mask,
	                                stroll_bmap64_utest_setup_toggle_mask,
	                                strollut_bmap_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap64_utest_toggle_range,
	                                stroll_bmap64_utest_setup_toggle_range,
	                                strollut_bmap_teardown),
	cmocka_unit_test(stroll_bmap64_utest_toggle_all),

	cmocka_unit_test(stroll_bmap64_utest_set_iter),
	cmocka_unit_test(stroll_bmap64_utest_clear_iter)
};

int
main(void)
{
	int ret;

	ret = cmocka_run_group_tests_name("32-bits bitmap (bmap32)",
	                                  stroll_bmap32_utests,
	                                  NULL,
	                                  NULL);

	ret |= cmocka_run_group_tests_name("64-bits bitmap (bmap64)",
	                                   stroll_bmap64_utests,
	                                   NULL,
	                                   NULL);

	return !ret ? EXIT_SUCCESS : EXIT_FAILURE;
}
#endif

/******************************************************************************
 * Common utils
 ******************************************************************************/

static void * strollut_bmap_xpct;

static void
strollut_bmap_teardown(void)
{
	free(strollut_bmap_xpct);
	cute_check_ptr(strollut_bmap_xpct, unequal, NULL);
	strollut_bmap_xpct = NULL;
}

/******************************************************************************
 * Unsigned 32-bits support
 ******************************************************************************/

static const uint32_t strollut_bmap32_words[] = {
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

static const uint32_t strollut_bmap32_masks[] = {
	0x00000000,
	0xffffffff,
	0x0000ffff,
	0xffff0000,
	0xff00ff00,
	0x00ff00ff,
	0xaa005500,
	0x005500aa
};

static const struct strollut_bmap32_range {
	uint32_t     mask;
	unsigned int start;
	unsigned int count;
} strollut_bmap32_ranges[] = {
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
strollut_bmap32_setup_mask_oper(uint32_t (* oper)(uint32_t, uint32_t))
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(strollut_bmap32_words);
	unsigned int   mnr = array_nr(strollut_bmap32_masks);
	uint32_t     * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			uint32_t * exp = &expected[(b * mnr) + m];

			*exp = oper(strollut_bmap32_words[b],
			            strollut_bmap32_masks[m]);
		}
	}

	strollut_bmap_xpct = expected;
}

static void
strollut_bmap32_run_mask_oper(uint32_t (* oper)(uint32_t, uint32_t))
{
	unsigned int     b, m;
	unsigned int     bnr = array_nr(strollut_bmap32_words);
	unsigned int     mnr = array_nr(strollut_bmap32_masks);
	const uint32_t * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			cute_check_hex(oper(strollut_bmap32_words[b],
			                    strollut_bmap32_masks[m]),
			               equal,
			               expected[(b * mnr) + m]);
		}
	}
}

static void
strollut_bmap32_setup_range_oper(uint32_t (* oper)(uint32_t, uint32_t))
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(strollut_bmap32_words);
	unsigned int   rnr = array_nr(strollut_bmap32_ranges);
	uint32_t     * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			uint32_t * exp = &expected[(b * rnr) + r];

			*exp = oper(strollut_bmap32_words[b],
			            strollut_bmap32_ranges[r].mask);
		}
	}

	strollut_bmap_xpct = expected;
}

static void
strollut_bmap32_run_range_oper(uint32_t (* oper)(uint32_t,
                                                 unsigned int,
                                                 unsigned int))
{
	unsigned int     b, r;
	unsigned int     bnr = array_nr(strollut_bmap32_words);
	unsigned int     rnr = array_nr(strollut_bmap32_ranges);
	const uint32_t * expected = strollut_bmap_xpct;

	cute_check_ptr(expected, unequal, NULL);

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			cute_check_hex(
				oper(strollut_bmap32_words[b],
				     strollut_bmap32_ranges[r].start,
				     strollut_bmap32_ranges[r].count),
				equal,
				expected[(b * rnr) + r]);
		}
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap32_init_assert)
{
	cute_expect_assertion(stroll_bmap32_setup_set(NULL));
	cute_expect_assertion(stroll_bmap32_setup_clear(NULL));
}
#else
CUTE_TEST(strollut_bmap32_init_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap32_init)
{
	uint32_t bmp = 0x5A5A5A5A;

	stroll_bmap32_setup_set(&bmp);
	cute_check_hex(bmp, equal, UINT32_MAX);

	stroll_bmap32_setup_clear(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap32_mask_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap32_mask(0, 0));
	cute_expect_assertion(stroll_bmap32_mask(0, 33));
	cute_expect_assertion(stroll_bmap32_mask(3, 30));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap32_mask_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bmap32_mask)
{
	unsigned int r;

	for (r = 0; r < array_nr(strollut_bmap32_ranges); r++) {
		const struct strollut_bmap32_range * rng;

		rng = &strollut_bmap32_ranges[r];

		cute_check_hex(stroll_bmap32_mask(rng->start, rng->count),
		               equal,
		               rng->mask);
	}
}

CUTE_TEST(strollut_bmap32_hweight)
{
	unsigned int m;

	for (m = 0; m < array_nr(strollut_bmap32_words); m++) {
		uint32_t     bmp = strollut_bmap32_words[m];
		unsigned int b;
		unsigned int cnt;

		for (b = 0, cnt = 0; b < 32; b++)
			if (bmp & (1U << b))
				cnt++;

		cute_check_hex(cnt, equal, stroll_bmap32_hweight(bmp));
	}
}

static uint32_t
strollut_bmap32_and_oper(uint32_t bmap, uint32_t mask)
{
	return bmap & mask;
}

static void
strollut_bmap32_setup_and(void)
{
	strollut_bmap32_setup_mask_oper(strollut_bmap32_and_oper);
}

CUTE_TEST_STATIC(strollut_bmap32_and,
                 strollut_bmap32_setup_and,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap32_run_mask_oper(stroll_bmap32_and);
}

static void
strollut_bmap32_setup_and_range(void)
{
	strollut_bmap32_setup_range_oper(strollut_bmap32_and_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap32_and_range_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap32_and_range(0, 0, 0));
	cute_expect_assertion(stroll_bmap32_and_range(0, 32, 1));
	cute_expect_assertion(stroll_bmap32_and_range(0, 30, 3));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap32_and_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap32_and_range,
                 strollut_bmap32_setup_and_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap32_run_range_oper(stroll_bmap32_and_range);
}

static uint32_t
strollut_bmap32_or_oper(uint32_t bmap, uint32_t mask)
{
	return bmap | mask;
}

static void
strollut_bmap32_setup_or(void)
{
	strollut_bmap32_setup_mask_oper(strollut_bmap32_or_oper);
}

CUTE_TEST_STATIC(strollut_bmap32_or,
                 strollut_bmap32_setup_or,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap32_run_mask_oper(stroll_bmap32_or);
}

static void
strollut_bmap32_setup_or_range(void)
{
	strollut_bmap32_setup_range_oper(strollut_bmap32_or_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap32_or_range_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap32_or_range(0, 0, 0));
	cute_expect_assertion(stroll_bmap32_or_range(0, 32, 1));
	cute_expect_assertion(stroll_bmap32_or_range(0, 30, 3));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap32_or_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap32_or_range,
                 strollut_bmap32_setup_or_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap32_run_range_oper(stroll_bmap32_or_range);
}

static uint32_t
strollut_bmap32_xor_oper(uint32_t bmap, uint32_t mask)
{
	return bmap ^ mask;
}

static void
strollut_bmap32_setup_xor(void)
{
	strollut_bmap32_setup_mask_oper(strollut_bmap32_xor_oper);
}

CUTE_TEST_STATIC(strollut_bmap32_xor,
                 strollut_bmap32_setup_xor,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap32_run_mask_oper(stroll_bmap32_xor);
}

static void
strollut_bmap32_setup_xor_range(void)
{
	strollut_bmap32_setup_range_oper(strollut_bmap32_xor_oper);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap32_xor_range_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap32_xor_range(0, 0, 0));
	cute_expect_assertion(stroll_bmap32_xor_range(0, 32, 1));
	cute_expect_assertion(stroll_bmap32_xor_range(0, 30, 3));
#pragma GCC diagnostic pop
}
#else
CUTE_TEST(strollut_bmap32_xor_range_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST_STATIC(strollut_bmap32_xor_range,
                 strollut_bmap32_setup_xor_range,
                 strollut_bmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_bmap32_run_range_oper(stroll_bmap32_xor_range);
}

CUTE_TEST(strollut_bmap32_test_bit)
{
	unsigned int b, m;
	unsigned int mnr = array_nr(strollut_bmap32_words);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 32; b++) {
			uint32_t bmp = strollut_bmap32_words[m];

			if (bmp & (1U << b))
				cute_check_bool(stroll_bmap32_test(bmp, b),
				                is,
				                true);
			else
				cute_check_bool(stroll_bmap32_test(bmp, b),
				                is,
				                false);
		}
	}
}

CUTE_TEST(strollut_bmap32_test_all)
{
	unsigned int b, m;
	unsigned int mnr = array_nr(strollut_bmap32_words);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 32; b++) {
			uint32_t bmp = strollut_bmap32_words[m];

			if (!!bmp)
				cute_check_bool(stroll_bmap32_test_all(bmp),
				                is,
				                true);
			else
				cute_check_bool(stroll_bmap32_test_all(bmp),
				                is,
				                false);
		}
	}
}

CUTE_GROUP(strollut_bmap32_group) = {
	CUTE_REF(strollut_bmap32_init_assert),
	CUTE_REF(strollut_bmap32_init),
	CUTE_REF(strollut_bmap32_mask_assert),
	CUTE_REF(strollut_bmap32_mask),
	CUTE_REF(strollut_bmap32_hweight),

	CUTE_REF(strollut_bmap32_and),
	CUTE_REF(strollut_bmap32_and_range_assert),
	CUTE_REF(strollut_bmap32_and_range),

	CUTE_REF(strollut_bmap32_or),
	CUTE_REF(strollut_bmap32_or_range_assert),
	CUTE_REF(strollut_bmap32_or_range),

	CUTE_REF(strollut_bmap32_xor),
	CUTE_REF(strollut_bmap32_xor_range_assert),
	CUTE_REF(strollut_bmap32_xor_range),

	CUTE_REF(strollut_bmap32_test_bit),
	CUTE_REF(strollut_bmap32_test_all),
};

CUTE_SUITE(strollut_bmap32_suite, strollut_bmap32_group);

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
	unsigned int    bnr = array_nr(strollut_bmap_words);
	unsigned int    mnr = array_nr(strollut_bmap_word_masks);
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
	unsigned int    bnr = array_nr(strollut_bmap_words);
	unsigned int    rnr = array_nr(strollut_bmap_word_ranges);
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
	unsigned int          bnr = array_nr(strollut_bmap_words);
	unsigned int          mnr = array_nr(strollut_bmap_word_masks);
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
	unsigned int          bnr = array_nr(strollut_bmap_words);
	unsigned int          rnr = array_nr(strollut_bmap_word_ranges);
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
	cute_expect_assertion(stroll_bmap_setup_set(NULL));
	cute_expect_assertion(stroll_bmap_setup_clear(NULL));
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

	stroll_bmap_setup_set(&bmp);
	cute_check_hex(bmp, equal, ULONG_MAX);

	stroll_bmap_setup_clear(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_mask_assert)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	cute_expect_assertion(stroll_bmap_mask(0, 0));
	cute_expect_assertion(stroll_bmap_mask(0, 65));
	cute_expect_assertion(stroll_bmap_mask(3, 63));
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

	for (r = 0; r < array_nr(strollut_bmap_word_ranges); r++) {
		const struct strollut_bmap_word_range * rng;

		rng = &strollut_bmap_word_ranges[r];

		cute_check_hex(stroll_bmap_mask(rng->start, rng->count),
		               equal,
		               rng->mask);
	}
}

CUTE_TEST(strollut_bmap_word_hweight)
{
	unsigned int m;

	for (m = 0; m < array_nr(strollut_bmap_words); m++) {
		unsigned long bmp = strollut_bmap_words[m];
		unsigned int  b;
		unsigned int  cnt;

		for (b = 0, cnt = 0; b < stroll_bops_bitsof(bmp); b++)
			if (bmp & (1UL << b))
				cnt++;

		cute_check_uint(stroll_bmap_hweight(bmp), equal, cnt);
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
	strollut_bmap_word_run_mask_oper(stroll_bmap_and);
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
	cute_expect_assertion(stroll_bmap_and_range(0, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_and_range(0, 64, 1));
	cute_expect_assertion(stroll_bmap_and_range(0, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_and_range(0, 32, 1));
	cute_expect_assertion(stroll_bmap_and_range(0, 30, 3));
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
	strollut_bmap_word_run_range_oper(stroll_bmap_and_range);
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
	strollut_bmap_word_run_mask_oper(stroll_bmap_or);
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
	cute_expect_assertion(stroll_bmap_or_range(0, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_or_range(0, 64, 1));
	cute_expect_assertion(stroll_bmap_or_range(0, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_or_range(0, 32, 1));
	cute_expect_assertion(stroll_bmap_or_range(0, 30, 3));
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
	strollut_bmap_word_run_range_oper(stroll_bmap_or_range);
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
	strollut_bmap_word_run_mask_oper(stroll_bmap_xor);
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
	cute_expect_assertion(stroll_bmap_xor_range(0, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_xor_range(0, 64, 1));
	cute_expect_assertion(stroll_bmap_xor_range(0, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_xor_range(0, 62, 1));
	cute_expect_assertion(stroll_bmap_xor_range(0, 30, 3));
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
	strollut_bmap_word_run_range_oper(stroll_bmap_xor_range);
}

CUTE_TEST(strollut_bmap_word_test_bit)
{
	unsigned int b, m;
	unsigned int mnr = array_nr(strollut_bmap_words);
	unsigned long bmp;

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < stroll_bops_bitsof(bmp); b++) {
			bmp = strollut_bmap_words[m];
			cute_check_bool(stroll_bmap_test(bmp, b),
			                is,
			                !!(bmp & (1UL << b)));
		}
	}
}

CUTE_TEST(strollut_bmap_word_test_all)
{
	unsigned int b, m;
	unsigned int mnr = array_nr(strollut_bmap_words);
	unsigned long bmp;

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < stroll_bops_bitsof(bmp); b++) {
			bmp = strollut_bmap_words[m];
			cute_check_bool(stroll_bmap_test_all(bmp), is, !!bmp);
		}
	}
}

static void
strollut_bmap_word_setup_test_mask(void)
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(strollut_bmap_words);
	unsigned int   mnr = array_nr(strollut_bmap_word_masks);
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
	unsigned int   bnr = array_nr(strollut_bmap_words);
	unsigned int   mnr = array_nr(strollut_bmap_word_masks);
	const bool *   expected = strollut_bmap_xpct;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool res = stroll_bmap_test_mask(
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
	unsigned int   bnr = array_nr(strollut_bmap_words);
	unsigned int   rnr = array_nr(strollut_bmap_word_ranges);
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
	cute_expect_assertion(stroll_bmap_test_range(0, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_test_range(0, 64, 1));
	cute_expect_assertion(stroll_bmap_test_range(0, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_test_range(0, 32, 1));
	cute_expect_assertion(stroll_bmap_test_range(0, 30, 3));
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
	unsigned int   bnr = array_nr(strollut_bmap_words);
	unsigned int   rnr = array_nr(strollut_bmap_word_ranges);
	const bool *   expected = strollut_bmap_xpct;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool res = stroll_bmap_test_range(
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
	cute_expect_assertion(stroll_bmap_set(NULL, 1));
}
#else
CUTE_TEST(strollut_bmap_word_set_bit)
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
		stroll_bmap_set(&bmp, b);
		cute_check_hex(bmp, equal, 1UL << b);
	}
}

static unsigned long
strollut_bmap_word_set_mask_oper(unsigned long bmap, unsigned long mask)
{
	unsigned long bmp = bmap;

	stroll_bmap_set_mask(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_set_mask_assert)
{
	cute_expect_assertion(stroll_bmap_set_mask(NULL, 0xf));
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

	stroll_bmap_set_range(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_set_range_assert)
{
	unsigned long bmp = 0;

	cute_expect_assertion(stroll_bmap_set_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_set_range(&bmp, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_set_range(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_set_range(&bmp, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_set_range(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_set_range(&bmp, 30, 3));
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
	cute_expect_assertion(stroll_bmap_set_all(NULL));
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

	stroll_bmap_set_all(&bmp);
	cute_check_hex(bmp, equal, ULONG_MAX);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_clear_bit_assert)
{
	cute_expect_assertion(stroll_bmap_clear(NULL, 1));
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
		stroll_bmap_clear(&bmp, b);
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

	stroll_bmap_clear_mask(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_clear_mask_assert)
{
	cute_expect_assertion(stroll_bmap_clear_mask(NULL, 0xf));
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

	stroll_bmap_clear_range(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_clear_range_assert)
{
	unsigned long bmp = ~(0UL);

	cute_expect_assertion(stroll_bmap_clear_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_clear_range(&bmp, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_clear_range(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_clear_range(&bmp, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_clear_range(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_clear_range(&bmp, 30, 3));
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
	cute_expect_assertion(stroll_bmap_clear_all(NULL));
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

	stroll_bmap_clear_all(&bmp);
	cute_check_hex(bmp, equal, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_toggle_bit_assert)
{
	cute_expect_assertion(stroll_bmap_toggle(NULL, 1));
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
		stroll_bmap_toggle(&bmp, b);
		cute_check_hex(bmp, equal, 1UL << b);

		bmp = ~(0UL);
		stroll_bmap_toggle(&bmp, b);
		cute_check_hex(bmp, equal, ~(1UL << b));
	}
}

static unsigned long
strollut_bmap_word_toggle_mask_oper(unsigned long bmap, unsigned long mask)
{
	unsigned long bmp = bmap;

	stroll_bmap_toggle_mask(&bmp, mask);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_toggle_mask_assert)
{
	cute_expect_assertion(stroll_bmap_toggle_mask(NULL, 0xf));
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

	stroll_bmap_toggle_range(&bmp, start_bit, bit_count);

	return bmp;
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_toggle_range_assert)
{
	unsigned long bmp = 0;

	cute_expect_assertion(stroll_bmap_toggle_range(NULL, 1, 1));
	cute_expect_assertion(stroll_bmap_toggle_range(&bmp, 0, 0));
#if (__WORDSIZE == 64)
	cute_expect_assertion(stroll_bmap_toggle_range(&bmp, 64, 1));
	cute_expect_assertion(stroll_bmap_toggle_range(&bmp, 60, 5));
#else
	cute_expect_assertion(stroll_bmap_toggle_range(&bmp, 32, 1));
	cute_expect_assertion(stroll_bmap_toggle_range(&bmp, 30, 3));
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
	cute_expect_assertion(stroll_bmap_toggle_all(NULL));
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
	unsigned int bnr = array_nr(strollut_bmap_words);

	for (b = 0; b < bnr; b++) {
		unsigned long bmp = strollut_bmap_words[b];
		unsigned long exp = bmp ^ ~(0UL);

		stroll_bmap_toggle_all(&bmp);
		cute_check_hex(bmp, equal, exp);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bmap_word_set_iter_assert)
{
	unsigned long iter;
	unsigned long bmp = 0;
	unsigned int  b;

	cute_expect_assertion(_stroll_bmap_setup_set_iter(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_set_iter(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter(&iter, NULL));
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

	for (m = 0; m < array_nr(strollut_bmap_words); m++) {
		unsigned int  e = 0;

		bmp = strollut_bmap_words[m];
		b = 0;
		stroll_bmap_foreach_set(&iter, bmp, &b) {
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
	unsigned long iter;
	unsigned long bmp = 0;
	unsigned int  b;

	cute_expect_assertion(_stroll_bmap_setup_clear_iter(NULL, bmp, &b));
	cute_expect_assertion(_stroll_bmap_setup_clear_iter(&iter, bmp, NULL));
	cute_expect_assertion(_stroll_bmap_step_iter(NULL, &b));
	cute_expect_assertion(_stroll_bmap_step_iter(&iter, NULL));
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

	for (m = 0; m < array_nr(strollut_bmap_words); m++) {
		unsigned int  e = 0;

		bmp = strollut_bmap_words[m];
		b = 0;
		stroll_bmap_foreach_clear(&iter, bmp, &b) {
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
	CUTE_REF(strollut_bmap32_suite),
	CUTE_REF(strollut_bmap_word_suite),
};

CUTE_SUITE_EXTERN(strollut_bmap_suite,
                  strollut_bmap_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
