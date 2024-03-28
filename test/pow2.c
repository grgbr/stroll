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
		unsigned int  uint;
		uint64_t      uint64;
		unsigned long ulong;
	};
	unsigned int          pow;
};

static void
strollut_pow2_check_low32(const struct strollut_pow2_test * tests,
                          unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_low32(tests[t].uint32),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_low32_assert)
{
	cute_expect_assertion(stroll_pow2_low32(0));
}
#else
CUTE_TEST(strollut_pow2_low32_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_low32_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = UINT32_C(1) << 0,  .pow = 0 },
		{ .uint32 = UINT32_C(1) << 1,  .pow = 1 },
		{ .uint32 = UINT32_C(1) << 2,  .pow = 2 },
		{ .uint32 = UINT32_C(1) << 15, .pow = 15 },
		{ .uint32 = UINT32_C(1) << 16, .pow = 16 },
		{ .uint32 = UINT32_C(1) << 17, .pow = 17 },
		{ .uint32 = UINT32_C(1) << 31, .pow = 31 }
	};

	strollut_pow2_check_low32(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low32_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = (UINT32_C(1) << 1) + 1,  .pow = 1 },
		{ .uint32 = (UINT32_C(1) << 2) + 1,  .pow = 2 },
		{ .uint32 = (UINT32_C(1) << 15) + 1, .pow = 15 },
		{ .uint32 = (UINT32_C(1) << 16) + 1, .pow = 16 },
		{ .uint32 = (UINT32_C(1) << 17) + 1, .pow = 17 },
		{ .uint32 = (UINT32_C(1) << 30) + 1, .pow = 30 },
		{ .uint32 = (UINT32_C(1) << 31) + 1, .pow = 31 }
	};

	strollut_pow2_check_low32(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low32_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = (UINT32_C(1) << 2) - 1,  .pow = 1 },
		{ .uint32 = (UINT32_C(1) << 3) - 1,  .pow = 2 },
		{ .uint32 = (UINT32_C(1) << 16) - 1, .pow = 15 },
		{ .uint32 = (UINT32_C(1) << 17) - 1, .pow = 16 },
		{ .uint32 = (UINT32_C(1) << 18) - 1, .pow = 17 },
		{ .uint32 = (UINT32_C(1) << 31) - 1, .pow = 30 },
		{ .uint32 = UINT32_MAX,              .pow = 31 }
	};

	strollut_pow2_check_low32(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_up32(const struct strollut_pow2_test * tests,
                         unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_up32(tests[t].uint32),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_up32_assert)
{
	cute_expect_assertion(stroll_pow2_up32(0));
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
		{ .uint32 = UINT32_C(1) << 0,  .pow = 0 },
		{ .uint32 = UINT32_C(1) << 1,  .pow = 1 },
		{ .uint32 = UINT32_C(1) << 2,  .pow = 2 },
		{ .uint32 = UINT32_C(1) << 15, .pow = 15 },
		{ .uint32 = UINT32_C(1) << 16, .pow = 16 },
		{ .uint32 = UINT32_C(1) << 17, .pow = 17 },
		{ .uint32 = UINT32_C(1) << 31, .pow = 31 }
	};

	strollut_pow2_check_up32(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up32_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = (UINT32_C(1) << 1) + 1,  .pow = 2 },
		{ .uint32 = (UINT32_C(1) << 2) + 1,  .pow = 3 },
		{ .uint32 = (UINT32_C(1) << 15) + 1, .pow = 16 },
		{ .uint32 = (UINT32_C(1) << 16) + 1, .pow = 17 },
		{ .uint32 = (UINT32_C(1) << 17) + 1, .pow = 18 },
		{ .uint32 = (UINT32_C(1) << 30) + 1, .pow = 31 },
		{ .uint32 = (UINT32_C(1) << 31) + 1, .pow = 32 }
	};

	strollut_pow2_check_up32(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up32_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint32 = (UINT32_C(1) << 2) - 1,  .pow = 2 },
		{ .uint32 = (UINT32_C(1) << 3) - 1,  .pow = 3 },
		{ .uint32 = (UINT32_C(1) << 16) - 1, .pow = 16 },
		{ .uint32 = (UINT32_C(1) << 17) - 1, .pow = 17 },
		{ .uint32 = (UINT32_C(1) << 18) - 1, .pow = 18 },
		{ .uint32 = (UINT32_C(1) << 31) - 1, .pow = 31 },
		{ .uint32 = UINT32_MAX,              .pow = 32 }
	};

	strollut_pow2_check_up32(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_low64(const struct strollut_pow2_test * tests,
                          unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_low64(tests[t].uint64),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_low64_assert)
{
	cute_expect_assertion(stroll_pow2_low64(0));
}
#else
CUTE_TEST(strollut_pow2_low64_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_low64_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = UINT64_C(1) << 0,  .pow = 0 },
		{ .uint64 = UINT64_C(1) << 1,  .pow = 1 },
		{ .uint64 = UINT64_C(1) << 2,  .pow = 2 },
		{ .uint64 = UINT64_C(1) << 31, .pow = 31 },
		{ .uint64 = UINT64_C(1) << 32, .pow = 32 },
		{ .uint64 = UINT64_C(1) << 33, .pow = 33 },
		{ .uint64 = UINT64_C(1) << 63, .pow = 63 }
	};

	strollut_pow2_check_low64(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low64_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = (UINT64_C(1) << 1) + 1,  .pow = 1 },
		{ .uint64 = (UINT64_C(1) << 2) + 1,  .pow = 2 },
		{ .uint64 = (UINT64_C(1) << 31) + 1, .pow = 31 },
		{ .uint64 = (UINT64_C(1) << 32) + 1, .pow = 32 },
		{ .uint64 = (UINT64_C(1) << 33) + 1, .pow = 33 },
		{ .uint64 = (UINT64_C(1) << 62) + 1, .pow = 62 },
		{ .uint64 = (UINT64_C(1) << 63) + 1, .pow = 63 }
	};

	strollut_pow2_check_low64(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low64_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = (UINT64_C(1) << 2) - 1,  .pow = 1 },
		{ .uint64 = (UINT64_C(1) << 3) - 1,  .pow = 2 },
		{ .uint64 = (UINT64_C(1) << 32) - 1, .pow = 31 },
		{ .uint64 = (UINT64_C(1) << 33) - 1, .pow = 32 },
		{ .uint64 = (UINT64_C(1) << 34) - 1, .pow = 33 },
		{ .uint64 = (UINT64_C(1) << 63) - 1, .pow = 62 },
		{ .uint64 = UINT64_MAX,              .pow = 63 }
	};

	strollut_pow2_check_low64(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_up64(const struct strollut_pow2_test * tests,
                         unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_up64(tests[t].uint64),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_up64_assert)
{
	cute_expect_assertion(stroll_pow2_up64(0));
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
		{ .uint64 = UINT64_C(1) << 0,  .pow = 0 },
		{ .uint64 = UINT64_C(1) << 1,  .pow = 1 },
		{ .uint64 = UINT64_C(1) << 2,  .pow = 2 },
		{ .uint64 = UINT64_C(1) << 31, .pow = 31 },
		{ .uint64 = UINT64_C(1) << 32, .pow = 32 },
		{ .uint64 = UINT64_C(1) << 33, .pow = 33 },
		{ .uint64 = UINT64_C(1) << 63, .pow = 63 }
	};

	strollut_pow2_check_up64(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up64_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = (UINT64_C(1) << 1) + 1,  .pow = 2 },
		{ .uint64 = (UINT64_C(1) << 2) + 1,  .pow = 3 },
		{ .uint64 = (UINT64_C(1) << 31) + 1, .pow = 32 },
		{ .uint64 = (UINT64_C(1) << 32) + 1, .pow = 33 },
		{ .uint64 = (UINT64_C(1) << 33) + 1, .pow = 34 },
		{ .uint64 = (UINT64_C(1) << 62) + 1, .pow = 63 },
		{ .uint64 = (UINT64_C(1) << 63) + 1, .pow = 64 }
	};

	strollut_pow2_check_up64(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up64_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint64 = (UINT64_C(1) << 2) - 1,  .pow = 2 },
		{ .uint64 = (UINT64_C(1) << 3) - 1,  .pow = 3 },
		{ .uint64 = (UINT64_C(1) << 32) - 1, .pow = 32 },
		{ .uint64 = (UINT64_C(1) << 33) - 1, .pow = 33 },
		{ .uint64 = (UINT64_C(1) << 34) - 1, .pow = 34 },
		{ .uint64 = (UINT64_C(1) << 63) - 1, .pow = 63 },
		{ .uint64 = UINT64_MAX,              .pow = 64 }
	};

	strollut_pow2_check_up64(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_low(const struct strollut_pow2_test * tests,
                        unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_low(tests[t].uint),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_low_assert)
{
	cute_expect_assertion(stroll_pow2_low(0));
}
#else
CUTE_TEST(strollut_pow2_low_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_low_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint = 1U << 0,  .pow = 0 },
		{ .uint = 1U << 1,  .pow = 1 },
		{ .uint = 1U << 2,  .pow = 2 },
		{ .uint = 1U << 16, .pow = 16 },
		{ .uint = 1U << 31, .pow = 31 }
	};

	strollut_pow2_check_low(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint = (1U << 1) + 1,  .pow = 1 },
		{ .uint = (1U << 2) + 1,  .pow = 2 },
		{ .uint = (1U << 16) + 1, .pow = 16 },
		{ .uint = (1U << 31) + 1, .pow = 31 }
	};

	strollut_pow2_check_low(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_low_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint = (1U << 2) - 1,  .pow = 1 },
		{ .uint = (1U << 3) - 1,  .pow = 2 },
		{ .uint = (1U << 17) - 1, .pow = 16 },
		{ .uint = (1U << 31) - 1, .pow = 30 },
		{ .uint = UINT_MAX,       .pow = 31 }
	};

	strollut_pow2_check_low(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_up(const struct strollut_pow2_test * tests,
                       unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_up(tests[t].uint),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_up_assert)
{
	cute_expect_assertion(stroll_pow2_up(0));
}
#else
CUTE_TEST(strollut_pow2_up_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_up_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint = 1U << 0,  .pow = 0 },
		{ .uint = 1U << 1,  .pow = 1 },
		{ .uint = 1U << 2,  .pow = 2 },
		{ .uint = 1U << 16, .pow = 16 },
		{ .uint = 1U << 31, .pow = 31 }
	};

	strollut_pow2_check_up(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint = (1UL << 1) + 1,  .pow = 2 },
		{ .uint = (1UL << 2) + 1,  .pow = 3 },
		{ .uint = (1UL << 15) + 1, .pow = 16 },
		{ .uint = (1UL << 31) + 1, .pow = 32 },
	};

	strollut_pow2_check_up(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_up_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .uint = (1UL << 2) - 1,  .pow = 2 },
		{ .uint = (1UL << 3) - 1,  .pow = 3 },
		{ .uint = (1UL << 16) - 1, .pow = 16 },
		{ .uint = (1UL << 31) - 1, .pow = 31 },
		{ .uint = UINT_MAX,        .pow = 32 }
	};

	strollut_pow2_check_up(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_lowul(const struct strollut_pow2_test * tests,
                          unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_lowul(tests[t].ulong),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_lowul_assert)
{
	cute_expect_assertion(stroll_pow2_lowul(0));
}
#else
CUTE_TEST(strollut_pow2_lowul_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_lowul_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .ulong = 1UL << 0,  .pow = 0 },
		{ .ulong = 1UL << 1,  .pow = 1 },
		{ .ulong = 1UL << 2,  .pow = 2 },
		{ .ulong = 1UL << 31, .pow = 31 },
#if __WORDSIZE == 64
		{ .ulong = 1UL << 32, .pow = 32 },
		{ .ulong = 1UL << 33, .pow = 33 },
		{ .ulong = 1UL << 63, .pow = 63 }
#endif /* __WORDSIZE == 64 */
	};

	strollut_pow2_check_lowul(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_lowul_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .ulong = (1UL << 1) + 1,  .pow = 1 },
		{ .ulong = (1UL << 2) + 1,  .pow = 2 },
		{ .ulong = (1UL << 31) + 1, .pow = 31 },
#if __WORDSIZE == 64
		{ .ulong = (1UL << 32) + 1, .pow = 32 },
		{ .ulong = (1UL << 33) + 1, .pow = 33 },
		{ .ulong = (1UL << 62) + 1, .pow = 62 },
		{ .ulong = (1UL << 63) + 1, .pow = 63 }
#endif /* __WORDSIZE == 64 */
	};

	strollut_pow2_check_lowul(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_lowul_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .ulong = (1UL << 2) - 1,  .pow = 1 },
		{ .ulong = (1UL << 3) - 1,  .pow = 2 },
		{ .ulong = (1UL << 31) - 1, .pow = 30 },
#if __WORDSIZE == 64
		{ .ulong = (1UL << 32) - 1, .pow = 31 },
		{ .ulong = (1UL << 33) - 1, .pow = 32 },
		{ .ulong = (1UL << 34) - 1, .pow = 33 },
		{ .ulong = (1UL << 63) - 1, .pow = 62 },
		{ .ulong = ULONG_MAX,       .pow = 63 }
#else /* __WORDSIZE != 64 */
		{ .ulong = ULONG_MAX,       .pow = 31 }
#endif /* __WORDSIZE == 64 */
	};

	strollut_pow2_check_lowul(tests, stroll_array_nr(tests));
}

static void
strollut_pow2_check_upul(const struct strollut_pow2_test * tests,
                         unsigned int                      nr)
{
	unsigned int t;

	for (t = 0; t < nr; t++)
		cute_check_uint(stroll_pow2_upul(tests[t].ulong),
		                equal,
		                tests[t].pow);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_pow2_upul_assert)
{
	cute_expect_assertion(stroll_pow2_upul(0));
}
#else
CUTE_TEST(strollut_pow2_upul_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_pow2_upul_round)
{
	const struct strollut_pow2_test tests[] = {
		{ .ulong = 1UL << 0,  .pow = 0 },
		{ .ulong = 1UL << 1,  .pow = 1 },
		{ .ulong = 1UL << 2,  .pow = 2 },
		{ .ulong = 1UL << 31, .pow = 31 },
#if __WORDSIZE == 64
		{ .ulong = 1UL << 32, .pow = 32 },
		{ .ulong = 1UL << 33, .pow = 33 },
		{ .ulong = 1UL << 63, .pow = 63 }
#endif /* __WORDSIZE == 64 */
	};

	strollut_pow2_check_upul(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_upul_plus)
{
	const struct strollut_pow2_test tests[] = {
		{ .ulong = (1UL << 1) + 1,  .pow = 2 },
		{ .ulong = (1UL << 2) + 1,  .pow = 3 },
		{ .ulong = (1UL << 31) + 1, .pow = 32 },
#if __WORDSIZE == 64
		{ .ulong = (1UL << 32) + 1, .pow = 33 },
		{ .ulong = (1UL << 33) + 1, .pow = 34 },
		{ .ulong = (1UL << 62) + 1, .pow = 63 },
		{ .ulong = (1UL << 63) + 1, .pow = 64 }
#endif /* __WORDSIZE == 64 */
	};

	strollut_pow2_check_upul(tests, stroll_array_nr(tests));
}

CUTE_TEST(strollut_pow2_upul_minus)
{
	const struct strollut_pow2_test tests[] = {
		{ .ulong = (1UL << 2) - 1,  .pow = 2 },
		{ .ulong = (1UL << 3) - 1,  .pow = 3 },
		{ .ulong = (1UL << 31) - 1, .pow = 31 },
#if __WORDSIZE == 64
		{ .ulong = (1UL << 32) - 1, .pow = 32 },
		{ .ulong = (1UL << 33) - 1, .pow = 33 },
		{ .ulong = (1UL << 34) - 1, .pow = 34 },
		{ .ulong = (1UL << 63) - 1, .pow = 63 },
		{ .ulong = ULONG_MAX,       .pow = 64 }
#else  /* __WORDSIZE != 64 */
		{ .ulong = ULONG_MAX,       .pow = 32 }
#endif /* __WORDSIZE == 64 */
	};

	strollut_pow2_check_upul(tests, stroll_array_nr(tests));
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

	CUTE_REF(strollut_pow2_low_assert),
	CUTE_REF(strollut_pow2_low_round),
	CUTE_REF(strollut_pow2_low_plus),
	CUTE_REF(strollut_pow2_low_minus),
	CUTE_REF(strollut_pow2_up_assert),
	CUTE_REF(strollut_pow2_up_round),
	CUTE_REF(strollut_pow2_up_plus),
	CUTE_REF(strollut_pow2_up_minus),

	CUTE_REF(strollut_pow2_lowul_assert),
	CUTE_REF(strollut_pow2_lowul_round),
	CUTE_REF(strollut_pow2_lowul_plus),
	CUTE_REF(strollut_pow2_lowul_minus),
	CUTE_REF(strollut_pow2_upul_assert),
	CUTE_REF(strollut_pow2_upul_round),
	CUTE_REF(strollut_pow2_upul_plus),
	CUTE_REF(strollut_pow2_upul_minus),
};

CUTE_SUITE_EXTERN(strollut_pow2_suite,
                  strollut_pow2_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
