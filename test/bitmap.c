#include "stroll/bitmap.h"

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

/******************************************************************************
 * Unsigned 32-bits support
 ******************************************************************************/

static const uint32_t stroll_bmap32_utest_bmaps[] = {
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

static const uint32_t stroll_bmap32_utest_masks[] = {
	0x00000000,
	0xffffffff,
	0x0000ffff,
	0xffff0000,
	0xff00ff00,
	0x00ff00ff,
	0xaa005500,
	0x005500aa
};

static const struct stroll_bmap_utest_range {
	uint32_t     mask;
	unsigned int start;
	unsigned int count;
} stroll_bmap32_utest_ranges[] = {
	{ 0xffffffff,  0, 32 },
	{ 0x0000ffff,  0, 16 },
	{ 0xffff0000, 16, 16 },
	{ 0x00ffff00,  8, 16 },
	{ 0x000000f0,  4,  4 },
	{ 0x0000f000, 12,  4 },
	{ 0x00f00000, 20,  4 },
	{ 0xf0000000, 28,  4 }
};

static int
stroll_bmap32_utest_setup_mask_oper(void     ** state,
                                    uint32_t (* oper)(uint32_t, uint32_t))
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(stroll_bmap32_utest_bmaps);
	unsigned int   mnr = array_nr(stroll_bmap32_utest_masks);
	uint32_t     * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			uint32_t * exp = &expected[(b * mnr) + m];

			*exp = oper(stroll_bmap32_utest_bmaps[b],
			            stroll_bmap32_utest_masks[m]);
		}
	}

	*state = expected;

	return 0;
}

static void
stroll_bmap32_utest_run_mask_oper(void      ** state,
                                  uint32_t  (* oper)(uint32_t, uint32_t))
{
	unsigned int     b, m;
	unsigned int     bnr = array_nr(stroll_bmap32_utest_bmaps);
	unsigned int     mnr = array_nr(stroll_bmap32_utest_masks);
	const uint32_t * expected = *state;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			assert_int_equal(oper(stroll_bmap32_utest_bmaps[b],
			                      stroll_bmap32_utest_masks[m]),
			                 expected[(b * mnr) + m]);
		}
	}
}

static int
stroll_bmap32_utest_setup_range_oper(void     ** state,
                                     uint32_t (* oper)(uint32_t, uint32_t))
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(stroll_bmap32_utest_bmaps);
	unsigned int   rnr = array_nr(stroll_bmap32_utest_ranges);
	uint32_t     * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			uint32_t * exp = &expected[(b * rnr) + r];

			*exp = oper(stroll_bmap32_utest_bmaps[b],
			            stroll_bmap32_utest_ranges[r].mask);
		}
	}

	*state = expected;

	return 0;
}

static void
stroll_bmap32_utest_run_range_oper(void     ** state,
                                   uint32_t (* oper)(uint32_t,
                                                     unsigned int,
                                                     unsigned int))
{
	unsigned int     b, r;
	unsigned int     bnr = array_nr(stroll_bmap32_utest_bmaps);
	unsigned int     rnr = array_nr(stroll_bmap32_utest_ranges);
	const uint32_t * expected = *state;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			assert_int_equal(
				oper(stroll_bmap32_utest_bmaps[b],
				     stroll_bmap32_utest_ranges[r].start,
				     stroll_bmap32_utest_ranges[r].count),
				expected[(b * rnr) + r]);
		}
	}
}

static int
stroll_bmap32_utest_teardown(void ** state)
{
	free(*state);

	return 0;
}

static void
stroll_bmap32_utest_init(void ** state __unused)
{
	uint32_t bmp = 0x5A5A5A5A;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_init_set(NULL));
#endif
	stroll_bmap32_init_set(&bmp);
	assert_int_equal(bmp, UINT32_MAX);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_init_clear(NULL));
#endif
	stroll_bmap32_init_clear(&bmp);
	assert_int_equal(bmp, 0);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_fini(NULL));
#endif
	stroll_bmap32_fini(&bmp);
}

static void
stroll_bmap32_utest_mask(void ** state __unused)
{
	unsigned int r;

#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	expect_assert_failure(stroll_bmap32_mask(0, 0));
	expect_assert_failure(stroll_bmap32_mask(0, 33));
	expect_assert_failure(stroll_bmap32_mask(3, 30));
#pragma GCC diagnostic pop
#endif

	for (r = 0; r < array_nr(stroll_bmap32_utest_ranges); r++) {
		const struct stroll_bmap_utest_range * rng;

		rng = &stroll_bmap32_utest_ranges[r];

		assert_int_equal(stroll_bmap32_mask(rng->start, rng->count),
		                 rng->mask);
	}
}

static uint32_t
stroll_bmap32_utest_and_oper(uint32_t bmap, uint32_t mask)
{
	return bmap & mask;
}

static int
stroll_bmap32_utest_setup_and(void ** state)
{
	return stroll_bmap32_utest_setup_mask_oper(
		state,
		stroll_bmap32_utest_and_oper);
}

static void
stroll_bmap32_utest_and(void ** state)
{
	stroll_bmap32_utest_run_mask_oper(state, stroll_bmap32_and);
}

static int
stroll_bmap32_utest_setup_and_range(void ** state)
{
	return stroll_bmap32_utest_setup_range_oper(
		state,
		stroll_bmap32_utest_and_oper);
}

static void
stroll_bmap32_utest_and_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	expect_assert_failure(stroll_bmap32_and_range(0, 0, 0));
	expect_assert_failure(stroll_bmap32_and_range(0, 32, 1));
	expect_assert_failure(stroll_bmap32_and_range(0, 30, 3));
#pragma GCC diagnostic pop
#endif
	stroll_bmap32_utest_run_range_oper(state, stroll_bmap32_and_range);
}

static uint32_t
stroll_bmap32_utest_or_oper(uint32_t bmap, uint32_t mask)
{
	return bmap | mask;
}

static int
stroll_bmap32_utest_setup_or(void ** state)
{
	return stroll_bmap32_utest_setup_mask_oper(
		state,
		stroll_bmap32_utest_or_oper);
}

static void
stroll_bmap32_utest_or(void ** state)
{
	stroll_bmap32_utest_run_mask_oper(state, stroll_bmap32_or);
}

static int
stroll_bmap32_utest_setup_or_range(void ** state)
{
	return stroll_bmap32_utest_setup_range_oper(
		state,
		stroll_bmap32_utest_or_oper);
}

static void
stroll_bmap32_utest_or_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	expect_assert_failure(stroll_bmap32_or_range(0, 0, 0));
	expect_assert_failure(stroll_bmap32_or_range(0, 32, 1));
	expect_assert_failure(stroll_bmap32_or_range(0, 30, 3));
#pragma GCC diagnostic pop
#endif
	stroll_bmap32_utest_run_range_oper(state, stroll_bmap32_or_range);
}

static uint32_t
stroll_bmap32_utest_xor_oper(uint32_t bmap, uint32_t mask)
{
	return bmap ^ mask;
}

static int
stroll_bmap32_utest_setup_xor(void ** state)
{
	return stroll_bmap32_utest_setup_mask_oper(
		state,
		stroll_bmap32_utest_xor_oper);
}

static void
stroll_bmap32_utest_xor(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	expect_assert_failure(stroll_bmap32_xor_range(0, 0, 0));
	expect_assert_failure(stroll_bmap32_xor_range(0, 32, 1));
	expect_assert_failure(stroll_bmap32_xor_range(0, 30, 3));
#pragma GCC diagnostic pop
#endif
	stroll_bmap32_utest_run_mask_oper(state, stroll_bmap32_xor);
}

static int
stroll_bmap32_utest_setup_xor_range(void ** state)
{
	return stroll_bmap32_utest_setup_range_oper(
		state,
		stroll_bmap32_utest_xor_oper);
}

static void
stroll_bmap32_utest_xor_range(void ** state)
{
	stroll_bmap32_utest_run_range_oper(state, stroll_bmap32_xor_range);
}

static void
stroll_bmap32_utest_test_bit(void ** state __unused)
{
	unsigned int b, m;
	unsigned int mnr = array_nr(stroll_bmap32_utest_bmaps);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 32; b++) {
			uint32_t bmp = stroll_bmap32_utest_bmaps[m];

			if (bmp & (1U << b))
				assert_true(stroll_bmap32_test(bmp, b));
			else
				assert_false(stroll_bmap32_test(bmp, b));
		}
	}
}

static void
stroll_bmap32_utest_test_all(void ** state __unused)
{
	unsigned int b, m;
	unsigned int mnr = array_nr(stroll_bmap32_utest_bmaps);

	for (m = 0; m < mnr; m++) {
		for (b = 0; b < 32; b++) {
			uint32_t bmp = stroll_bmap32_utest_bmaps[m];

			if (!!bmp)
				assert_true(stroll_bmap32_test_all(bmp));
			else
				assert_false(stroll_bmap32_test_all(bmp));
		}
	}
}


static int
stroll_bmap32_utest_setup_test_mask(void ** state)
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(stroll_bmap32_utest_bmaps);
	unsigned int   mnr = array_nr(stroll_bmap32_utest_masks);
	bool         * expected;

	expected = malloc(bnr * mnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool * exp = &expected[(b * mnr) + m];

			*exp = !!(stroll_bmap32_utest_bmaps[b] &
			          stroll_bmap32_utest_masks[m]);
		}
	}

	*state = expected;

	return 0;
}

static void
stroll_bmap32_utest_test_mask(void ** state)
{
	unsigned int   b, m;
	unsigned int   bnr = array_nr(stroll_bmap32_utest_bmaps);
	unsigned int   mnr = array_nr(stroll_bmap32_utest_masks);
	const bool   * expected = *state;

	for (b = 0; b < bnr; b++) {
		for (m = 0; m < mnr; m++) {
			bool res;

			res = stroll_bmap32_test_mask(
				stroll_bmap32_utest_bmaps[b],
				stroll_bmap32_utest_masks[m]);
			assert_true(res == expected[(b * mnr) + m]);
		}
	}
}

static int
stroll_bmap32_utest_setup_test_range(void ** state)
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(stroll_bmap32_utest_bmaps);
	unsigned int   rnr = array_nr(stroll_bmap32_utest_ranges);
	bool         * expected;

	expected = malloc(bnr * rnr * sizeof(*expected));
	if (!expected)
		return -1;

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool * exp = &expected[(b * rnr) + r];

			*exp = !!(stroll_bmap32_utest_bmaps[b] &
			          stroll_bmap32_utest_ranges[r].mask);
		}
	}

	*state = expected;

	return 0;
}

static void
stroll_bmap32_utest_test_range(void ** state)
{
	unsigned int   b, r;
	unsigned int   bnr = array_nr(stroll_bmap32_utest_bmaps);
	unsigned int   rnr = array_nr(stroll_bmap32_utest_ranges);
	const bool   * expected = *state;

#if defined(CONFIG_STROLL_ASSERT_API)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	expect_assert_failure(stroll_bmap32_test_range(0, 0, 0));
	expect_assert_failure(stroll_bmap32_test_range(0, 32, 1));
	expect_assert_failure(stroll_bmap32_test_range(0, 30, 3));
#pragma GCC diagnostic pop
#endif

	for (b = 0; b < bnr; b++) {
		for (r = 0; r < rnr; r++) {
			bool res;

			res = stroll_bmap32_test_range(
				stroll_bmap32_utest_bmaps[b],
				stroll_bmap32_utest_ranges[r].start,
				stroll_bmap32_utest_ranges[r].count);
			assert_true(res == expected[(b * rnr) + r]);
		}
	}
}

static void
stroll_bmap32_utest_set_bit(void ** state __unused)
{
	uint32_t     bmp = 0;
	unsigned int b;


#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_set(NULL, 1));
#endif

	for (b = 0; b < 32; b++) {
		bmp = 0;
		stroll_bmap32_set(&bmp, b);
		assert_true(bmp & (1U << b));
		assert_false(!!(bmp & ~(1U << b)));
	}
}

static uint32_t
stroll_bmap32_utest_set_mask_oper(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap32_set_mask(&bmp, mask);

	return bmp;
}

static void
stroll_bmap32_utest_set_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_set_mask(NULL, 0xf));
#endif
	stroll_bmap32_utest_run_mask_oper(state,
	                                  stroll_bmap32_utest_set_mask_oper);
}

static uint32_t
stroll_bmap32_utest_set_range_oper(uint32_t     bmap,
                                   unsigned int start_bit,
                                   unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap32_set_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
stroll_bmap32_utest_set_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint32_t bmp = 0;

	expect_assert_failure(stroll_bmap32_set_range(NULL, 1, 1));
	expect_assert_failure(stroll_bmap32_set_range(&bmp, 0, 0));
	expect_assert_failure(stroll_bmap32_set_range(&bmp, 32, 1));
	expect_assert_failure(stroll_bmap32_set_range(&bmp, 30, 3));
#endif
	stroll_bmap32_utest_run_range_oper(state,
	                                   stroll_bmap32_utest_set_range_oper);
}

static void
stroll_bmap32_utest_set_all(void ** state __unused)
{
	uint32_t bmp = 0;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_set_all(NULL));
#endif

	stroll_bmap32_set_all(&bmp);
	assert_int_equal(bmp, ~(0U));
}

static void
stroll_bmap32_utest_clear_bit(void ** state __unused)
{
	uint32_t     bmp = ~(0U);
	unsigned int b;


#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_clear(NULL, 1));
#endif

	for (b = 0; b < 32; b++) {
		bmp = ~(0U);
		stroll_bmap32_clear(&bmp, b);
		assert_int_equal(bmp, ~(0U) & ~(1U << b));
	}
}

static uint32_t
stroll_bmap32_utest_notand_oper(uint32_t bmap, uint32_t mask)
{
	return bmap & ~mask;
}

static int
stroll_bmap32_utest_setup_clear_mask(void ** state)
{
	return stroll_bmap32_utest_setup_mask_oper(
		state,
		stroll_bmap32_utest_notand_oper);
}

static uint32_t
stroll_bmap32_utest_clear_mask_oper(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap32_clear_mask(&bmp, mask);

	return bmp;
}

static void
stroll_bmap32_utest_clear_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_clear_mask(NULL, 0xf));
#endif
	stroll_bmap32_utest_run_mask_oper(state,
	                                  stroll_bmap32_utest_clear_mask_oper);
}

static int
stroll_bmap32_utest_setup_clear_range(void ** state)
{
	return stroll_bmap32_utest_setup_range_oper(
		state,
		stroll_bmap32_utest_notand_oper);
}

static uint32_t
stroll_bmap32_utest_clear_range_oper(uint32_t     bmap,
                                     unsigned int start_bit,
                                     unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap32_clear_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
stroll_bmap32_utest_clear_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint32_t bmp = ~(0U);

	expect_assert_failure(stroll_bmap32_clear_range(NULL, 1, 1));
	expect_assert_failure(stroll_bmap32_clear_range(&bmp, 0, 0));
	expect_assert_failure(stroll_bmap32_clear_range(&bmp, 32, 1));
	expect_assert_failure(stroll_bmap32_clear_range(&bmp, 30, 3));
#endif
	stroll_bmap32_utest_run_range_oper(
		state,
		stroll_bmap32_utest_clear_range_oper);
}

static void
stroll_bmap32_utest_clear_all(void ** state __unused)
{
	uint32_t bmp = ~(0U);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_clear_all(NULL));
#endif

	stroll_bmap32_clear_all(&bmp);
	assert_int_equal(bmp, 0);
}

static void
stroll_bmap32_utest_toggle_bit(void ** state __unused)
{
	uint32_t     bmp = 0;
	unsigned int b;


#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_toggle(NULL, 1));
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
stroll_bmap32_utest_setup_toggle_mask(void ** state)
{
	return stroll_bmap32_utest_setup_mask_oper(
		state,
		stroll_bmap32_utest_xor_oper);
}

static uint32_t
stroll_bmap32_utest_toggle_mask_oper(uint32_t bmap, uint32_t mask)
{
	uint32_t bmp = bmap;

	stroll_bmap32_toggle_mask(&bmp, mask);

	return bmp;
}

static void
stroll_bmap32_utest_toggle_mask(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_toggle_mask(NULL, 0xf));
#endif
	stroll_bmap32_utest_run_mask_oper(state,
	                                  stroll_bmap32_utest_toggle_mask_oper);
}

static int
stroll_bmap32_utest_setup_toggle_range(void ** state)
{
	return stroll_bmap32_utest_setup_range_oper(
		state,
		stroll_bmap32_utest_xor_oper);
}

static uint32_t
stroll_bmap32_utest_toggle_range_oper(uint32_t    bmap,
                                     unsigned int start_bit,
                                     unsigned int bit_count)
{
	uint32_t bmp = bmap;

	stroll_bmap32_toggle_range(&bmp, start_bit, bit_count);

	return bmp;
}

static void
stroll_bmap32_utest_toggle_range(void ** state)
{
#if defined(CONFIG_STROLL_ASSERT_API)
	uint32_t bmp = 0;

	expect_assert_failure(stroll_bmap32_toggle_range(NULL, 1, 1));
	expect_assert_failure(stroll_bmap32_toggle_range(&bmp, 0, 0));
	expect_assert_failure(stroll_bmap32_toggle_range(&bmp, 32, 1));
	expect_assert_failure(stroll_bmap32_toggle_range(&bmp, 30, 3));
#endif
	stroll_bmap32_utest_run_range_oper(
		state,
		stroll_bmap32_utest_toggle_range_oper);
}

static void
stroll_bmap32_utest_toggle_all(void ** state __unused)
{
	unsigned int   b;
	unsigned int   bnr = array_nr(stroll_bmap32_utest_bmaps);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_toggle_all(NULL));
#endif
	for (b = 0; b < bnr; b++) {
		uint32_t bmp = stroll_bmap32_utest_bmaps[b];
		uint32_t exp = bmp ^ ~(0U);

		stroll_bmap32_toggle_all(&bmp);
		assert_int_equal(bmp, exp);
	}
}

static void
stroll_bmap32_utest_set_iter(void ** state __unused)
{
	uint32_t     iter;
	uint32_t     bmp = 0;
	unsigned int b;
	unsigned int m;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_setup_set_iter(NULL, bmp, &b));
	expect_assert_failure(stroll_bmap32_setup_set_iter(&iter, bmp, NULL));
	expect_assert_failure(stroll_bmap32_step_iter(NULL, &b));
	expect_assert_failure(stroll_bmap32_step_iter(&iter, NULL));
#endif

	for (m = 0; m < array_nr(stroll_bmap32_utest_bmaps); m++) {
		uint32_t     bmp = stroll_bmap32_utest_bmaps[m];
		unsigned int e = 0;

		b = 0;
		stroll_bmap32_foreach_set(&iter, bmp, &b) {
			while ((e < 32) && !(bmp & (1U << e)))
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
stroll_bmap32_utest_clear_iter(void ** state __unused)
{
	uint32_t     iter;
	uint32_t     bmp = 0;
	unsigned int b;
	unsigned int m;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap32_setup_clear_iter(NULL, bmp, &b));
	expect_assert_failure(stroll_bmap32_setup_clear_iter(&iter, bmp, NULL));
	expect_assert_failure(stroll_bmap32_step_iter(NULL, &b));
	expect_assert_failure(stroll_bmap32_step_iter(&iter, NULL));
#endif

	for (m = 0; m < array_nr(stroll_bmap32_utest_bmaps); m++) {
		uint32_t     bmp = stroll_bmap32_utest_bmaps[m];
		unsigned int e = 0;

		b = 0;
		stroll_bmap32_foreach_clear(&iter, bmp, &b) {
			while ((e < 32) && (bmp & (1U << e)))
				e++;
			assert_int_equal(b, e);
			e++;
		}

		if (bmp == ~(0U))
			assert_int_equal(b, (unsigned int)-1);
		else
			assert_in_range(b, 0, 31);
	}
}

static const struct CMUnitTest stroll_bmap32_utests[] = {
	cmocka_unit_test(stroll_bmap32_utest_init),
	cmocka_unit_test(stroll_bmap32_utest_mask),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_and,
	                                stroll_bmap32_utest_setup_and,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_and_range,
	                                stroll_bmap32_utest_setup_and_range,
	                                stroll_bmap32_utest_teardown),

	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_or,
	                                stroll_bmap32_utest_setup_or,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_or_range,
	                                stroll_bmap32_utest_setup_or_range,
	                                stroll_bmap32_utest_teardown),

	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_xor,
	                                stroll_bmap32_utest_setup_xor,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_xor_range,
	                                stroll_bmap32_utest_setup_xor_range,
	                                stroll_bmap32_utest_teardown),

	cmocka_unit_test(stroll_bmap32_utest_test_bit),
	cmocka_unit_test(stroll_bmap32_utest_test_all),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_test_mask,
	                                stroll_bmap32_utest_setup_test_mask,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_test_range,
	                                stroll_bmap32_utest_setup_test_range,
	                                stroll_bmap32_utest_teardown),

	cmocka_unit_test(stroll_bmap32_utest_set_bit),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_set_mask,
	                                stroll_bmap32_utest_setup_or,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_set_range,
	                                stroll_bmap32_utest_setup_or_range,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test(stroll_bmap32_utest_set_all),

	cmocka_unit_test(stroll_bmap32_utest_clear_bit),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_clear_mask,
	                                stroll_bmap32_utest_setup_clear_mask,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_clear_range,
	                                stroll_bmap32_utest_setup_clear_range,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test(stroll_bmap32_utest_clear_all),

	cmocka_unit_test(stroll_bmap32_utest_toggle_bit),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_toggle_mask,
	                                stroll_bmap32_utest_setup_toggle_mask,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test_setup_teardown(stroll_bmap32_utest_toggle_range,
	                                stroll_bmap32_utest_setup_toggle_range,
	                                stroll_bmap32_utest_teardown),
	cmocka_unit_test(stroll_bmap32_utest_toggle_all),
	cmocka_unit_test(stroll_bmap32_utest_set_iter),
	cmocka_unit_test(stroll_bmap32_utest_clear_iter)
};

/******************************************************************************
 *  Unsigned int support
 ******************************************************************************/

static void
stroll_bmapuint_utest_init(void ** state __unused)
{
	unsigned int   bmp = 0x5A5A5A5A;
	unsigned int * null = NULL;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap_init_set(null));
#endif
	stroll_bmap_init_set(&bmp);
	assert_int_equal(bmp, UINT_MAX);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap_init_clear(null));
#endif
	stroll_bmap_init_clear(&bmp);
	assert_int_equal(bmp, 0);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap_fini(null));
#endif
	stroll_bmap_fini(&bmp);
}

static const struct CMUnitTest stroll_bmapuint_utests[] = {
	cmocka_unit_test(stroll_bmapuint_utest_init),
};

/******************************************************************************
 * Unsigned 64-bits support
 ******************************************************************************/

static void
stroll_bmap64_utest_init(void ** state __unused)
{
	uint64_t bmp = 0x5A5A5A5A5A5A5A5A;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap64_init_set(NULL));
#endif
	stroll_bmap64_init_set(&bmp);
	assert_int_equal(bmp, UINT64_MAX);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap64_init_clear(NULL));
#endif
	stroll_bmap64_init_clear(&bmp);
	assert_int_equal(bmp, 0);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap64_fini(NULL));
#endif
	stroll_bmap64_fini(&bmp);
}

static const struct CMUnitTest stroll_bmap64_utests[] = {
	cmocka_unit_test(stroll_bmap64_utest_init),
};

/******************************************************************************
 *  Unsigned long support
 ******************************************************************************/

static void
stroll_bmapulong_utest_init(void ** state __unused)
{
	unsigned long   bmp = 0x5A5A5A5A;
	unsigned long * null = NULL;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap_init_set(null));
#endif
	stroll_bmap_init_set(&bmp);
	assert_int_equal(bmp, ULONG_MAX);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap_init_clear(null));
#endif
	stroll_bmap_init_clear(&bmp);
	assert_int_equal(bmp, 0);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bmap_fini(null));
#endif
	stroll_bmap_fini(&bmp);
}

static const struct CMUnitTest stroll_bmapulong_utests[] = {
	cmocka_unit_test(stroll_bmapulong_utest_init),
};

int
main(void)
{
	int ret;

	ret = cmocka_run_group_tests_name("32-bits bitmap (bmap32)",
	                                  stroll_bmap32_utests,
	                                  NULL,
	                                  NULL);

	ret |= cmocka_run_group_tests_name("unsigned int bitmap (bmapuint)",
	                                   stroll_bmapuint_utests,
	                                   NULL,
	                                   NULL);

	ret |= cmocka_run_group_tests_name("64-bits bitmap (bmap64)",
	                                   stroll_bmap64_utests,
	                                   NULL,
	                                   NULL);

	ret |= cmocka_run_group_tests_name("unsigned long bitmap (bmapulong)",
	                                   stroll_bmapulong_utests,
	                                   NULL,
	                                   NULL);

	return !ret ? EXIT_SUCCESS : EXIT_FAILURE;
}
