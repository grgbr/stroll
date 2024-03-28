/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/pow2.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>

struct strollut_pow2_test {
	union {
		uint32_t      uint32;
		uint64_t      uint64;
		unsigned long ulong;
	};
	unsigned int          pow;
};

static void
strollut_pow2_check_lower32(const struct strollut_pow2_test * tests,
                            unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_lower32(tests[t].uint32),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_low32_assert)
{
	cute_expect_assertion(stroll_pow2_lower32(0));
}
#else
CUTE_TEST(strollut_pow2_lowp32_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_low32_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = UINT32_C(1) << 0,  .pow  = 0 },
		{ .uint32 = UINT32_C(1) << 1,  .pow  = 1 },
		{ .uint32 = UINT32_C(1) << 2,  .pow  = 2 },
		{ .uint32 = UINT32_C(1) << 15, .pow = 15 },
		{ .uint32 = UINT32_C(1) << 16, .pow = 16 },
		{ .uint32 = UINT32_C(1) << 17, .pow = 17 },
		{ .uint32 = UINT32_C(1) << 31, .pow = 31 }
	};

	strollut_pow2_check_lower32(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low32_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = (UINT32_C(1) << 1) + 1,  .pow  = 1 },
		{ .uint32 = (UINT32_C(1) << 2) + 1,  .pow  = 2 },
		{ .uint32 = (UINT32_C(1) << 15) + 1, .pow = 15 },
		{ .uint32 = (UINT32_C(1) << 16) + 1, .pow = 16 },
		{ .uint32 = (UINT32_C(1) << 17) + 1, .pow = 17 },
		{ .uint32 = (UINT32_C(1) << 30) + 1, .pow = 30 },
		{ .uint32 = (UINT32_C(1) << 31) + 1, .pow = 31 }
	};

	strollut_pow2_check_lower32(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low32_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = (UINT32_C(1) << 2) - 1,  .pow  = 1 },
		{ .uint32 = (UINT32_C(1) << 3) - 1,  .pow  = 2 },
		{ .uint32 = (UINT32_C(1) << 16) - 1, .pow = 15 },
		{ .uint32 = (UINT32_C(1) << 17) - 1, .pow = 16 },
		{ .uint32 = (UINT32_C(1) << 18) - 1, .pow = 17 },
		{ .uint32 = (UINT32_C(1) << 31) - 1, .pow = 30 },
		{ .uint32 = UINT32_MAX,              .pow = 31 }
	};

	strollut_pow2_check_lower32(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_upper32(const struct strollut_pow2_test * tests,
                            unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_upper32(tests[t].uint32),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_up32_assert)
{
	cute_expect_assertion(stroll_pow2_upper32(0));
}
#else
CUTE_TEST(strollut_pow2_up32_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_up32_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = UINT32_C(1) << 0,  .pow  = 0 },
		{ .uint32 = UINT32_C(1) << 1,  .pow  = 1 },
		{ .uint32 = UINT32_C(1) << 2,  .pow  = 2 },
		{ .uint32 = UINT32_C(1) << 15, .pow = 15 },
		{ .uint32 = UINT32_C(1) << 16, .pow = 16 },
		{ .uint32 = UINT32_C(1) << 17, .pow = 17 },
		{ .uint32 = UINT32_C(1) << 31, .pow = 31 }
	};

	strollut_pow2_check_upper32(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up32_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = (UINT32_C(1) << 1) + 1,  .pow  = 2 },
		{ .uint32 = (UINT32_C(1) << 2) + 1,  .pow  = 3 },
		{ .uint32 = (UINT32_C(1) << 15) + 1, .pow = 16 },
		{ .uint32 = (UINT32_C(1) << 16) + 1, .pow = 17 },
		{ .uint32 = (UINT32_C(1) << 17) + 1, .pow = 18 },
		{ .uint32 = (UINT32_C(1) << 30) + 1, .pow = 31 },
		{ .uint32 = (UINT32_C(1) << 31) + 1, .pow = 32 }
	};

	strollut_pow2_check_upper32(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up32_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = (UINT32_C(1) << 2) - 1,  .pow  = 2 },
		{ .uint32 = (UINT32_C(1) << 3) - 1,  .pow  = 3 },
		{ .uint32 = (UINT32_C(1) << 16) - 1, .pow = 16 },
		{ .uint32 = (UINT32_C(1) << 17) - 1, .pow = 17 },
		{ .uint32 = (UINT32_C(1) << 18) - 1, .pow = 18 },
		{ .uint32 = (UINT32_C(1) << 31) - 1, .pow = 31 },
		{ .uint32 = UINT32_MAX,              .pow = 32 }
	};

	strollut_pow2_check_upper32(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_lower64(const struct strollut_pow2_test * tests,
                            unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_lower64(tests[t].uint64),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_low64_assert)
{
	cute_expect_assertion(stroll_pow2_lower64(0));
}
#else
CUTE_TEST(strollut_pow2_lowp64_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_low64_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = UINT64_C(1) << 0,  .pow  = 0 },
		{ .uint64 = UINT64_C(1) << 1,  .pow  = 1 },
		{ .uint64 = UINT64_C(1) << 2,  .pow  = 2 },
		{ .uint64 = UINT64_C(1) << 31, .pow = 31 },
		{ .uint64 = UINT64_C(1) << 32, .pow = 32 },
		{ .uint64 = UINT64_C(1) << 33, .pow = 33 },
		{ .uint64 = UINT64_C(1) << 63, .pow = 63 }
	};

	strollut_pow2_check_lower64(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low64_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = (UINT64_C(1) << 1) + 1,  .pow  = 1 },
		{ .uint64 = (UINT64_C(1) << 2) + 1,  .pow  = 2 },
		{ .uint64 = (UINT64_C(1) << 31) + 1, .pow = 31 },
		{ .uint64 = (UINT64_C(1) << 32) + 1, .pow = 32 },
		{ .uint64 = (UINT64_C(1) << 33) + 1, .pow = 33 },
		{ .uint64 = (UINT64_C(1) << 62) + 1, .pow = 62 },
		{ .uint64 = (UINT64_C(1) << 63) + 1, .pow = 63 }
	};

	strollut_pow2_check_lower64(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low64_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = (UINT64_C(1) << 2) - 1,  .pow  = 1 },
		{ .uint64 = (UINT64_C(1) << 3) - 1,  .pow  = 2 },
		{ .uint64 = (UINT64_C(1) << 32) - 1, .pow = 31 },
		{ .uint64 = (UINT64_C(1) << 33) - 1, .pow = 32 },
		{ .uint64 = (UINT64_C(1) << 34) - 1, .pow = 33 },
		{ .uint64 = (UINT64_C(1) << 63) - 1, .pow = 62 },
		{ .uint64 = UINT64_MAX,              .pow = 63 }
	};

	strollut_pow2_check_lower64(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_upper64(const struct strollut_pow2_test * tests,
                            unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_upper64(tests[t].uint64),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_up64_assert)
{
	cute_expect_assertion(stroll_pow2_upper64(0));
}
#else
CUTE_TEST(strollut_pow2_up64_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_up64_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = UINT64_C(1) << 0,  .pow  = 0 },
		{ .uint64 = UINT64_C(1) << 1,  .pow  = 1 },
		{ .uint64 = UINT64_C(1) << 2,  .pow  = 2 },
		{ .uint64 = UINT64_C(1) << 31, .pow = 31 },
		{ .uint64 = UINT64_C(1) << 32, .pow = 32 },
		{ .uint64 = UINT64_C(1) << 33, .pow = 33 },
		{ .uint64 = UINT64_C(1) << 63, .pow = 63 }
	};

	strollut_pow2_check_upper64(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up64_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = (UINT64_C(1) << 1) + 1,  .pow  = 2 },
		{ .uint64 = (UINT64_C(1) << 2) + 1,  .pow  = 3 },
		{ .uint64 = (UINT64_C(1) << 31) + 1, .pow = 32 },
		{ .uint64 = (UINT64_C(1) << 32) + 1, .pow = 33 },
		{ .uint64 = (UINT64_C(1) << 33) + 1, .pow = 34 },
		{ .uint64 = (UINT64_C(1) << 62) + 1, .pow = 63 },
		{ .uint64 = (UINT64_C(1) << 63) + 1, .pow = 64 }
	};

	strollut_pow2_check_upper64(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up64_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = (UINT64_C(1) << 2) - 1,  .pow  = 2 },
		{ .uint64 = (UINT64_C(1) << 3) - 1,  .pow  = 3 },
		{ .uint64 = (UINT64_C(1) << 32) - 1, .pow = 32 },
		{ .uint64 = (UINT64_C(1) << 33) - 1, .pow = 33 },
		{ .uint64 = (UINT64_C(1) << 34) - 1, .pow = 34 },
		{ .uint64 = (UINT64_C(1) << 63) - 1, .pow = 63 },
		{ .uint64 = UINT64_MAX,              .pow = 64 }
	};

	strollut_pow2_check_upper64(tests, stroll_array_nr(tests));
}

CUTE_GROUP(strollut_pow2_group) = {
	CUTE_REF(strollut_pow2_low32_assert),
	CUTE_REF(strollut_pow2_low32_round),
	CUTE_REF(strollut_pow2_low32_plus),
	CUTE_REF(strollut_pow2_low32_minus),

	CUTE_REF(strollut_pow2_up32_assert),
	CUTE_REF(strollut_pow2_up32_round),
	CUTE_REF(strollut_pow2_up32_plus),
	CUTE_REF(strollut_pow2_up32_minus),

	CUTE_REF(strollut_pow2_low64_assert),
	CUTE_REF(strollut_pow2_low64_round),
	CUTE_REF(strollut_pow2_low64_plus),
	CUTE_REF(strollut_pow2_low64_minus),

	CUTE_REF(strollut_pow2_up64_assert),
	CUTE_REF(strollut_pow2_up64_round),
	CUTE_REF(strollut_pow2_up64_plus),
	CUTE_REF(strollut_pow2_up64_minus),
};

CUTE_SUITE_EXTERN(strollut_pow2_suite,
                  strollut_pow2_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
