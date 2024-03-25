/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "utest.h"
#include "stroll/fbmap.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>
#include <stdlib.h>
#include <string.h>

#if (__WORDSIZE != 32) && (__WORDSIZE != 64)
#error "Unsupported machine word size !"
#endif

#define STROLLUT_FBMAP_NOASSERT(_test) \
	CUTE_TEST(_test) { cute_skip("assertion unsupported"); }

#define STROLLUT_FBMAP_NO64BITS(_test) \
	CUTE_TEST(_test) { cute_skip("32-bits machine word only"); }

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_nr_assert)
{
	unsigned int nr __unused;

	cute_expect_assertion(nr = stroll_fbmap_nr(NULL));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_nr_assert)
#endif

static const struct stroll_fbmap strollut_fbmap_14 = {
	.nr   = 14,
	.bits = ((unsigned long []){ 0x2f0aUL })
};

static const struct stroll_fbmap strollut_fbmap_32 = {
	.nr   = 32,
	.bits = ((unsigned long []){ 0xf0a05001UL })
};

static const struct stroll_fbmap strollut_fbmap_33 = {
	.nr   = 33,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0x70a05001UL, 1UL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0x170a05001UL })
#endif
};

static const struct stroll_fbmap strollut_fbmap_63 = {
	.nr   = 63,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0x70a05001UL, 0x40010001UL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0x4001000170a05001UL })
#endif
};

static const struct stroll_fbmap strollut_fbmap_64 = {
	.nr   = 64,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0x70a05001UL, 0x80010001UL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0x8001000170a05001UL })
#endif
};

static const struct stroll_fbmap strollut_fbmap_65 = {
	.nr   = 65,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0x70a05001UL, 0x80010001UL, 1UL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0x8001000170a05001UL, 1UL })
#endif
};

#if __WORDSIZE == 64
static const struct stroll_fbmap strollut_fbmap_127 = {
	.nr   = 127,
	.bits = ((unsigned long []){ 0x8001000170a05001UL,
	                             0x4001000170a05001UL })
};

static const struct stroll_fbmap strollut_fbmap_128 = {
	.nr   = 128,
	.bits = ((unsigned long []){ 0x8001000170a05001UL,
	                             0x8001000170a05001UL })
};

static const struct stroll_fbmap strollut_fbmap_129 = {
	.nr   = 129,
	.bits = ((unsigned long []){ 0x8001000170a05001UL,
	                             0x8001000170a05001UL,
	                             1UL })
};
#endif /* __WORDSIZE == 64 */

static const struct stroll_fbmap strollut_fbmap_null_14 = {
	.nr   = 14,
	.bits = ((unsigned long []){ 0xffffc000UL })
};

static const struct stroll_fbmap strollut_fbmap_null_32 = {
	.nr   = 32,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0UL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0xffffffff00000000UL })
#endif
};

static const struct stroll_fbmap strollut_fbmap_null_33 = {
	.nr   = 33,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0UL, 0xffffffeUL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0xfffffffe00000000UL })
#endif
};

static const struct stroll_fbmap strollut_fbmap_null_63 = {
	.nr   = 63,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0UL, 0x80000000UL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0x8000000000000000UL })
#endif
};

static const struct stroll_fbmap strollut_fbmap_null_64 = {
	.nr   = 63,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0UL, 0UL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0UL })
#endif
};

static const struct stroll_fbmap strollut_fbmap_null_65 = {
	.nr   = 65,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0UL, 0UL, 0xfffffffeUL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0UL, 0xfffffffffffffffeUL })
#endif
};

static const struct stroll_fbmap strollut_fbmap_null_129 = {
	.nr   = 129,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0UL, 0UL, 0UL, 0UL, 0xfffffffeUL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0UL, 0UL, 0xfffffffffffffffeUL })
#endif
};

CUTE_TEST(strollut_fbmap_nr)
{
	cute_check_uint(stroll_fbmap_nr(&strollut_fbmap_14), equal, 14);
}

static void
strollut_fbmap_check_test(const struct stroll_fbmap * bmap,
                          const unsigned int          ref[],
                          unsigned int                count)
{
	unsigned int c, b;

	cute_check_uint(count, greater, 0);

	for (b = c = 0; c < count; c++) {
		while (b < ref[c]) {
			cute_check_uint(ref[c], lower, bmap->nr);
			cute_check_bool(stroll_fbmap_test(bmap, b), is, false);
			b++;
		}

		cute_check_uint(b, equal, ref[c]);
		cute_check_bool(stroll_fbmap_test(bmap, b), is, true);

		b++;
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_test_assert)
{
	unsigned int nr __unused;

	cute_expect_assertion(nr = stroll_fbmap_test(NULL, 0));
	cute_expect_assertion(nr = stroll_fbmap_test(&strollut_fbmap_14, 14));
	cute_expect_assertion(nr = stroll_fbmap_test(&strollut_fbmap_14,
	                                             UINT_MAX));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_test_assert)
#endif

CUTE_TEST(strollut_fbmap_test_14)
{
	const unsigned int ref[] = { 1, 3, 8, 9, 10, 11, 13 };

	strollut_fbmap_check_test(&strollut_fbmap_14, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_test_32)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 31};

	strollut_fbmap_check_test(&strollut_fbmap_32, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_test_33)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32};

	strollut_fbmap_check_test(&strollut_fbmap_33, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_test_63)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 62};

	strollut_fbmap_check_test(&strollut_fbmap_63, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_test_64)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63};

	strollut_fbmap_check_test(&strollut_fbmap_64, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_test_65)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63,
	                             64};

	strollut_fbmap_check_test(&strollut_fbmap_65, ref, array_nr(ref));
}

#if __WORDSIZE == 64
CUTE_TEST(strollut_fbmap_test_127)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 126 };

	strollut_fbmap_check_test(&strollut_fbmap_127, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_test_128)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127 };

	strollut_fbmap_check_test(&strollut_fbmap_128, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_test_129)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127,
	                             128 };

	strollut_fbmap_check_test(&strollut_fbmap_129, ref, array_nr(ref));
}
#else  /* __WORDSIZE != 64 */
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_test_127)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_test_128)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_test_129)
#endif /* __WORDSIZE == 64 */

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_hweight_assert)
{
	unsigned int nr __unused;

	cute_expect_assertion(nr = stroll_fbmap_hweight(NULL));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_hweight_assert)
#endif

CUTE_TEST(strollut_fbmap_hweight_14)
{
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_null_14),
	                equal,
	                0);
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_14), equal, 7U);
}

CUTE_TEST(strollut_fbmap_hweight_32)
{
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_null_32),
	                equal,
	                0);
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_32), equal, 9U);
}

CUTE_TEST(strollut_fbmap_hweight_33)
{
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_null_33),
	                equal,
	                0);
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_32), equal, 9U);
}

CUTE_TEST(strollut_fbmap_hweight_63)
{
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_null_63),
	                equal,
	                0);
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_63), equal, 11U);
}

CUTE_TEST(strollut_fbmap_hweight_64)
{
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_null_64),
	                equal,
	                0);
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_64), equal, 11U);
}

CUTE_TEST(strollut_fbmap_hweight_65)
{
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_null_65),
	                equal,
	                0);
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_65), equal, 12U);
}

CUTE_TEST(strollut_fbmap_hweight_129)
{
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_null_129),
	                equal,
	                0);
	cute_check_uint(stroll_fbmap_hweight(&strollut_fbmap_129), equal, 23U);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_test_all_assert)
{
	unsigned int nr __unused;

	cute_expect_assertion(nr = stroll_fbmap_test_all(NULL));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_test_all_assert)
#endif

CUTE_TEST(strollut_fbmap_test_all_null_14)
{
	cute_check_bool(stroll_fbmap_test_all(&strollut_fbmap_null_14),
	                is,
	                false);
}

CUTE_TEST(strollut_fbmap_test_all_null_33)
{
	cute_check_bool(stroll_fbmap_test_all(&strollut_fbmap_null_33),
	                is,
	                false);
}

CUTE_TEST(strollut_fbmap_test_all_null_65)
{
	cute_check_bool(stroll_fbmap_test_all(&strollut_fbmap_null_65),
	                is,
	                false);
}

CUTE_TEST(strollut_fbmap_test_all_null_129)
{
	cute_check_bool(stroll_fbmap_test_all(&strollut_fbmap_null_129),
	                is,
	                false);
}

struct strollut_fbmap_range {
	unsigned int start_bit;
	unsigned int bit_count;
};

static void
strollut_fbmap_check_test_range(const struct stroll_fbmap * bmap,
                          const struct strollut_fbmap_range ref[],
                          unsigned int                      count)
{
	unsigned int c = 0;

	cute_check_uint(count, greater, 0);

	if (ref[c].start_bit) {
		cute_check_uint(ref[c].start_bit, lower, bmap->nr);
		cute_check_bool(
			stroll_fbmap_test_range(bmap, 0, ref[c].start_bit),
			is,
			false);
	}

	while (c < count) {
		unsigned int start = ref[c].start_bit;
		unsigned int cnt = ref[c].bit_count;

		cute_check_uint(start, lower, bmap->nr);
		cute_check_uint(start + cnt, lower_equal, bmap->nr);
		cute_check_bool(stroll_fbmap_test_range(bmap, start, cnt),
		                is,
		                true);

		start = ref[c].start_bit + ref[c].bit_count;
		cute_check_uint(start, lower_equal, bmap->nr);
		if (start == bmap->nr)
			return;

		cute_check_uint(c + 1, lower, count);
		cnt = ref[c + 1].start_bit - start;
		cute_check_uint(cnt, greater, 0);
		cute_check_uint(start + cnt, lower_equal, bmap->nr);

		cute_check_bool(stroll_fbmap_test_range(bmap, start, cnt),
		                is,
		                false);

		c++;
	}
}

CUTE_TEST(strollut_fbmap_test_range_14)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 1, .bit_count = 1 },
		{ .start_bit = 3, .bit_count = 1 },
		{ .start_bit = 8, .bit_count = 4 },
		{ .start_bit = 13, .bit_count = 1 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_14,
	                                ranges,
	                                array_nr(ranges));
}

CUTE_TEST(strollut_fbmap_test_range_32)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 0, .bit_count = 1 },
		{ .start_bit = 12, .bit_count = 1 },
		{ .start_bit = 14, .bit_count = 1 },
		{ .start_bit = 21, .bit_count = 1 },
		{ .start_bit = 23, .bit_count = 1 },
		{ .start_bit = 28, .bit_count = 4 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_32,
	                                ranges,
	                                array_nr(ranges));
}

CUTE_TEST(strollut_fbmap_test_range_33)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 0, .bit_count = 1 },
		{ .start_bit = 12, .bit_count = 1 },
		{ .start_bit = 14, .bit_count = 1 },
		{ .start_bit = 21, .bit_count = 1 },
		{ .start_bit = 23, .bit_count = 1 },
		{ .start_bit = 28, .bit_count = 3 },
		{ .start_bit = 32, .bit_count = 1 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_33,
	                                ranges,
	                                array_nr(ranges));
}

CUTE_TEST(strollut_fbmap_test_range_63)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 0, .bit_count = 1 },
		{ .start_bit = 12, .bit_count = 1 },
		{ .start_bit = 14, .bit_count = 1 },
		{ .start_bit = 21, .bit_count = 1 },
		{ .start_bit = 23, .bit_count = 1 },
		{ .start_bit = 28, .bit_count = 3 },
		{ .start_bit = 32, .bit_count = 1 },
		{ .start_bit = 48, .bit_count = 1 },
		{ .start_bit = 62, .bit_count = 1 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_63,
	                                ranges,
	                                array_nr(ranges));
}

CUTE_TEST(strollut_fbmap_test_range_64)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 0, .bit_count = 1 },
		{ .start_bit = 12, .bit_count = 1 },
		{ .start_bit = 14, .bit_count = 1 },
		{ .start_bit = 21, .bit_count = 1 },
		{ .start_bit = 23, .bit_count = 1 },
		{ .start_bit = 28, .bit_count = 3 },
		{ .start_bit = 32, .bit_count = 1 },
		{ .start_bit = 48, .bit_count = 1 },
		{ .start_bit = 63, .bit_count = 1 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_64,
	                                ranges,
	                                array_nr(ranges));
}

CUTE_TEST(strollut_fbmap_test_range_65)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 0, .bit_count = 1 },
		{ .start_bit = 12, .bit_count = 1 },
		{ .start_bit = 14, .bit_count = 1 },
		{ .start_bit = 21, .bit_count = 1 },
		{ .start_bit = 23, .bit_count = 1 },
		{ .start_bit = 28, .bit_count = 3 },
		{ .start_bit = 32, .bit_count = 1 },
		{ .start_bit = 48, .bit_count = 1 },
		{ .start_bit = 63, .bit_count = 2 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_65,
	                                ranges,
	                                array_nr(ranges));
}

#if __WORDSIZE == 64
CUTE_TEST(strollut_fbmap_test_range_127)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 0, .bit_count = 1 },
		{ .start_bit = 12, .bit_count = 1 },
		{ .start_bit = 14, .bit_count = 1 },
		{ .start_bit = 21, .bit_count = 1 },
		{ .start_bit = 23, .bit_count = 1 },
		{ .start_bit = 28, .bit_count = 3 },
		{ .start_bit = 32, .bit_count = 1 },
		{ .start_bit = 48, .bit_count = 1 },
		{ .start_bit = 63, .bit_count = 2 },

		{ .start_bit = 76, .bit_count = 1 },
		{ .start_bit = 78, .bit_count = 1 },
		{ .start_bit = 85, .bit_count = 1 },
		{ .start_bit = 87, .bit_count = 1 },
		{ .start_bit = 92, .bit_count = 3 },
		{ .start_bit = 96, .bit_count = 1 },
		{ .start_bit = 112, .bit_count = 1 },
		{ .start_bit = 126, .bit_count = 1 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_127,
	                                ranges,
	                                array_nr(ranges));
}

CUTE_TEST(strollut_fbmap_test_range_128)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 0, .bit_count = 1 },
		{ .start_bit = 12, .bit_count = 1 },
		{ .start_bit = 14, .bit_count = 1 },
		{ .start_bit = 21, .bit_count = 1 },
		{ .start_bit = 23, .bit_count = 1 },
		{ .start_bit = 28, .bit_count = 3 },
		{ .start_bit = 32, .bit_count = 1 },
		{ .start_bit = 48, .bit_count = 1 },
		{ .start_bit = 63, .bit_count = 2 },

		{ .start_bit = 76, .bit_count = 1 },
		{ .start_bit = 78, .bit_count = 1 },
		{ .start_bit = 85, .bit_count = 1 },
		{ .start_bit = 87, .bit_count = 1 },
		{ .start_bit = 92, .bit_count = 3 },
		{ .start_bit = 96, .bit_count = 1 },
		{ .start_bit = 112, .bit_count = 1 },
		{ .start_bit = 127, .bit_count = 1 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_128,
	                                ranges,
	                                array_nr(ranges));
}

CUTE_TEST(strollut_fbmap_test_range_129)
{
	const struct strollut_fbmap_range ranges[] = {
		{ .start_bit = 0, .bit_count = 1 },
		{ .start_bit = 12, .bit_count = 1 },
		{ .start_bit = 14, .bit_count = 1 },
		{ .start_bit = 21, .bit_count = 1 },
		{ .start_bit = 23, .bit_count = 1 },
		{ .start_bit = 28, .bit_count = 3 },
		{ .start_bit = 32, .bit_count = 1 },
		{ .start_bit = 48, .bit_count = 1 },
		{ .start_bit = 63, .bit_count = 2 },

		{ .start_bit = 76, .bit_count = 1 },
		{ .start_bit = 78, .bit_count = 1 },
		{ .start_bit = 85, .bit_count = 1 },
		{ .start_bit = 87, .bit_count = 1 },
		{ .start_bit = 92, .bit_count = 3 },
		{ .start_bit = 96, .bit_count = 1 },
		{ .start_bit = 112, .bit_count = 1 },
		{ .start_bit = 127, .bit_count = 2 }
	};

	strollut_fbmap_check_test_range(&strollut_fbmap_129,
	                                ranges,
	                                array_nr(ranges));
}
#else  /* __WORDSIZE != 64 */
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_test_range_127)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_test_range_128)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_test_range_129)
#endif /* __WORDSIZE == 64 */

static struct stroll_fbmap strollut_fbmap;

static void
strollut_fbmap_teardown(void)
{
	stroll_fbmap_fini(&strollut_fbmap);
}

static void
strollut_fbmap_check_value(unsigned int bit_count, unsigned char value)
{
	const unsigned char * ptr;
	unsigned int          b;

	for (b = 0, ptr = (const unsigned char *)strollut_fbmap.bits;
	     b < ((bit_count + CHAR_BIT - 1) / CHAR_BIT);
	     b++)
		cute_check_sint(ptr[b], equal, value);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_init_clear_assert)
{
	int err __unused;

	cute_expect_assertion(err = stroll_fbmap_init_clear(NULL, 32));
	cute_expect_assertion(err = stroll_fbmap_init_clear(&strollut_fbmap,
	                                                    0));
	cute_expect_assertion(err = stroll_fbmap_init_clear(&strollut_fbmap,
	                                                    UINT_MAX));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_init_clear_assert)
#endif

CUTE_TEST_STATIC(strollut_fbmap_init_clear_14,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 14),
	                equal,
	                0);
	strollut_fbmap_check_value(14, 0);
}

CUTE_TEST_STATIC(strollut_fbmap_init_clear_191,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 191),
	                equal,
	                0);
	strollut_fbmap_check_value(191, 0);
}

CUTE_TEST_STATIC(strollut_fbmap_init_clear_192,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 192),
	                equal,
	                0);
	strollut_fbmap_check_value(192, 0);
}

CUTE_TEST_STATIC(strollut_fbmap_init_clear_193,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 193),
	                equal,
	                0);
	strollut_fbmap_check_value(193, 0);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_init_set_assert)
{
	int err __unused;

	cute_expect_assertion(err = stroll_fbmap_init_set(NULL, 32));
	cute_expect_assertion(err = stroll_fbmap_init_set(&strollut_fbmap, 0));
	cute_expect_assertion(err = stroll_fbmap_init_set(&strollut_fbmap,
	                                                  UINT_MAX));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_init_set_assert)
#endif

CUTE_TEST_STATIC(strollut_fbmap_init_set_14,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_sint(stroll_fbmap_init_set(&strollut_fbmap, 14),
	                equal,
	                0);
	strollut_fbmap_check_value(14, 0xff);
}

CUTE_TEST_STATIC(strollut_fbmap_init_set_191,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_sint(stroll_fbmap_init_set(&strollut_fbmap, 191),
	                equal,
	                0);
	strollut_fbmap_check_value(191, 0xff);
}

CUTE_TEST_STATIC(strollut_fbmap_init_set_192,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_sint(stroll_fbmap_init_set(&strollut_fbmap, 192),
	                equal,
	                0);
	strollut_fbmap_check_value(192, 0xff);
}

CUTE_TEST_STATIC(strollut_fbmap_init_set_193,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_sint(stroll_fbmap_init_set(&strollut_fbmap, 193),
	                equal,
	                0);
	strollut_fbmap_check_value(193, 0xff);
}

static void
strollut_fbmap_check_set_all(unsigned int bit_count)
{
	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, bit_count),
	                equal,
	                0);
	stroll_fbmap_set_all(&strollut_fbmap);
	strollut_fbmap_check_value(bit_count, 0xff);

}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST_STATIC(strollut_fbmap_set_all_assert,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap.nr = 1;
	strollut_fbmap.bits = NULL;
	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 32), equal, 0);

	cute_expect_assertion(stroll_fbmap_set_all(NULL));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_set_all_assert)
#endif

CUTE_TEST_STATIC(strollut_fbmap_set_all_14,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap_check_set_all(14);
}

CUTE_TEST_STATIC(strollut_fbmap_set_all_191,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap_check_set_all(191);
}

CUTE_TEST_STATIC(strollut_fbmap_set_all_192,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap_check_set_all(192);
}

CUTE_TEST_STATIC(strollut_fbmap_set_all_193,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap_check_set_all(193);
}

static void
strollut_fbmap_check_clear_all(unsigned int bit_count)
{
	cute_check_sint(stroll_fbmap_init_set(&strollut_fbmap, bit_count),
	                equal,
	                0);
	stroll_fbmap_clear_all(&strollut_fbmap);
	strollut_fbmap_check_value(bit_count, 0);

}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST_STATIC(strollut_fbmap_clear_all_assert,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap.nr = 1;
	strollut_fbmap.bits = NULL;
	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 32), equal, 0);

	cute_expect_assertion(stroll_fbmap_clear_all(NULL));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_clear_all_assert)
#endif

CUTE_TEST_STATIC(strollut_fbmap_clear_all_14,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap_check_clear_all(14);
}

CUTE_TEST_STATIC(strollut_fbmap_clear_all_191,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap_check_clear_all(191);
}

CUTE_TEST_STATIC(strollut_fbmap_clear_all_192,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap_check_clear_all(192);
}

CUTE_TEST_STATIC(strollut_fbmap_clear_all_193,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap_check_clear_all(193);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST_STATIC(strollut_fbmap_set_assert,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 14), equal, 0);

	cute_expect_assertion(stroll_fbmap_set(NULL, 1));
	cute_expect_assertion(stroll_fbmap_set(&strollut_fbmap, 14));
	cute_expect_assertion(stroll_fbmap_set(&strollut_fbmap, UINT_MAX));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_set_assert)
#endif

static void
strollut_fbmap_check_set(const struct stroll_fbmap * ref_bmap,
                         const unsigned int          ref_bits[],
                         unsigned int                ref_count)
{
	unsigned int c;

	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, ref_bmap->nr),
	                equal,
	                0);

	for (c = 0; c < ref_count; c++)
		stroll_fbmap_set(&strollut_fbmap, ref_bits[c]);

	strollut_fbmap_check_test(&strollut_fbmap, ref_bits, ref_count);
}

CUTE_TEST_STATIC(strollut_fbmap_set_14,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 1, 3, 8, 9, 10, 11, 13 };

	strollut_fbmap_check_set(&strollut_fbmap_14, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_set_32,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 31};

	strollut_fbmap_check_set(&strollut_fbmap_32, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_set_33,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32};

	strollut_fbmap_check_set(&strollut_fbmap_33, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_set_63,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 62};

	strollut_fbmap_check_set(&strollut_fbmap_63, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_set_64,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63};

	strollut_fbmap_check_set(&strollut_fbmap_64, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_set_65,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63,
	                             64};

	strollut_fbmap_check_set(&strollut_fbmap_65, ref, array_nr(ref));
}

#if __WORDSIZE == 64
CUTE_TEST_STATIC(strollut_fbmap_set_127,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 126 };

	strollut_fbmap_check_set(&strollut_fbmap_127, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_set_128,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127 };

	strollut_fbmap_check_set(&strollut_fbmap_128, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_set_129,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127,
	                             128 };

	strollut_fbmap_check_set(&strollut_fbmap_129, ref, array_nr(ref));
}
#else  /* __WORDSIZE != 64 */
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_set_127)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_set_128)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_set_129)
#endif /* __WORDSIZE == 64 */

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST_STATIC(strollut_fbmap_clear_assert,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_set(&strollut_fbmap, 14), equal, 0);

	cute_expect_assertion(stroll_fbmap_clear(NULL, 1));
	cute_expect_assertion(stroll_fbmap_clear(&strollut_fbmap, 14));
	cute_expect_assertion(stroll_fbmap_clear(&strollut_fbmap, UINT_MAX));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_clear_assert)
#endif

static void
strollut_fbmap_check_test_not(const struct stroll_fbmap * bmap,
                              const unsigned int          ref[],
                              unsigned int                count)
{
	unsigned int c, b;

	cute_check_uint(count, greater, 0);

	for (b = c = 0; c < count; c++) {
		while (b < ref[c]) {
			cute_check_uint(ref[c], lower, bmap->nr);
			cute_check_bool(stroll_fbmap_test(bmap, b), is, true);
			b++;
		}

		cute_check_uint(b, equal, ref[c]);
		cute_check_bool(stroll_fbmap_test(bmap, b), is, false);

		b++;
	}
}

static void
strollut_fbmap_check_clear(const struct stroll_fbmap * ref_bmap,
                           const unsigned int          ref_bits[],
                           unsigned int                ref_count)
{
	unsigned int c;

	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_set(&strollut_fbmap, ref_bmap->nr),
	                equal,
	                0);

	for (c = 0; c < ref_count; c++)
		stroll_fbmap_clear(&strollut_fbmap, ref_bits[c]);

	strollut_fbmap_check_test_not(&strollut_fbmap, ref_bits, ref_count);
}

CUTE_TEST_STATIC(strollut_fbmap_clear_14,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 1, 3, 8, 9, 10, 11, 13 };

	strollut_fbmap_check_clear(&strollut_fbmap_14, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_clear_32,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 31};

	strollut_fbmap_check_clear(&strollut_fbmap_32, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_clear_33,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32};

	strollut_fbmap_check_clear(&strollut_fbmap_33, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_clear_63,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 62};

	strollut_fbmap_check_clear(&strollut_fbmap_63, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_clear_64,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63};

	strollut_fbmap_check_clear(&strollut_fbmap_64, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_clear_65,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63,
	                             64};

	strollut_fbmap_check_clear(&strollut_fbmap_65, ref, array_nr(ref));
}

#if __WORDSIZE == 64
CUTE_TEST_STATIC(strollut_fbmap_clear_127,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 126 };

	strollut_fbmap_check_clear(&strollut_fbmap_127, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_clear_128,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127 };

	strollut_fbmap_check_clear(&strollut_fbmap_128, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_clear_129,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127,
	                             128 };

	strollut_fbmap_check_clear(&strollut_fbmap_129, ref, array_nr(ref));
}
#else  /* __WORDSIZE != 64 */
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_clear_127)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_clear_128)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_clear_129)
#endif /* __WORDSIZE == 64 */

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST_STATIC(strollut_fbmap_toggle_assert,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 14), equal, 0);

	cute_expect_assertion(stroll_fbmap_toggle(NULL, 1));
	cute_expect_assertion(stroll_fbmap_toggle(&strollut_fbmap, 14));
	cute_expect_assertion(stroll_fbmap_toggle(&strollut_fbmap, UINT_MAX));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_toggle_assert)
#endif

static void
strollut_fbmap_check_toggle(const struct stroll_fbmap * ref_bmap,
                            const unsigned int          ref_bits[],
                            unsigned int                ref_count)
{
	unsigned int c;

	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, ref_bmap->nr),
	                equal,
	                0);
	for (c = 0; c < ref_count; c++)
		stroll_fbmap_toggle(&strollut_fbmap, ref_bits[c]);
	strollut_fbmap_check_test(&strollut_fbmap, ref_bits, ref_count);

	stroll_fbmap_set_all(&strollut_fbmap);
	for (c = 0; c < ref_count; c++)
		stroll_fbmap_toggle(&strollut_fbmap, ref_bits[c]);
	strollut_fbmap_check_test_not(&strollut_fbmap, ref_bits, ref_count);
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_14,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 1, 3, 8, 9, 10, 11, 13 };

	strollut_fbmap_check_toggle(&strollut_fbmap_14, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_32,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 31};

	strollut_fbmap_check_toggle(&strollut_fbmap_32, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_33,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32};

	strollut_fbmap_check_toggle(&strollut_fbmap_33, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_63,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 62};

	strollut_fbmap_check_toggle(&strollut_fbmap_63, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_64,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63};

	strollut_fbmap_check_toggle(&strollut_fbmap_64, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_65,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63,
	                             64};

	strollut_fbmap_check_toggle(&strollut_fbmap_65, ref, array_nr(ref));
}

#if __WORDSIZE == 64
CUTE_TEST_STATIC(strollut_fbmap_toggle_127,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 126 };

	strollut_fbmap_check_toggle(&strollut_fbmap_127, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_128,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127 };

	strollut_fbmap_check_toggle(&strollut_fbmap_128, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_129,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127,
	                             128 };

	strollut_fbmap_check_toggle(&strollut_fbmap_129, ref, array_nr(ref));
}
#else  /* __WORDSIZE != 64 */
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_toggle_127)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_toggle_128)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_toggle_129)
#endif /* __WORDSIZE == 64 */

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_init_dup_assert)
{
	int                 err __unused;
	struct stroll_fbmap bmap;

	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_expect_assertion(err = stroll_fbmap_init_dup(NULL,
	                                                  &strollut_fbmap_14));
	cute_expect_assertion(err = stroll_fbmap_init_dup(&strollut_fbmap,
	                                                  NULL));

	bmap.nr = 0;
	bmap.bits = strollut_fbmap_14.bits;
	cute_expect_assertion(err = stroll_fbmap_init_dup(&strollut_fbmap,
	                                                  &bmap));

	bmap.nr = UINT_MAX;
	bmap.bits = strollut_fbmap_14.bits;
	cute_expect_assertion(err = stroll_fbmap_init_dup(&strollut_fbmap,
	                                                  &bmap));

	bmap.nr = 14;
	bmap.bits = NULL;
	cute_expect_assertion(err = stroll_fbmap_init_dup(&strollut_fbmap,
	                                                  &bmap));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_init_dup_assert)
#endif

CUTE_TEST_STATIC(strollut_fbmap_init_dup,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_dup(&strollut_fbmap,
	                                      &strollut_fbmap_65),
	                equal,
	                0);

	cute_check_uint(strollut_fbmap.nr, equal, strollut_fbmap_65.nr);
	cute_check_mem(strollut_fbmap.bits,
	               equal,
	               strollut_fbmap_65.bits,
	               stroll_fbmap_word_nr(strollut_fbmap_65.nr) *
	               sizeof(strollut_fbmap_65.bits[0]));
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST_STATIC(strollut_fbmap_toggle_all_assert,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_clear(&strollut_fbmap, 14), equal, 0);

	cute_expect_assertion(stroll_fbmap_toggle_all(NULL));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_toggle_all_assert)
#endif

static void
strollut_fbmap_check_toggle_all(const struct stroll_fbmap * ref_bmap,
                                const unsigned int          ref_bits[],
                                unsigned int                ref_count)
{
	strollut_fbmap.nr = 0;
	strollut_fbmap.bits = NULL;

	cute_check_sint(stroll_fbmap_init_dup(&strollut_fbmap, ref_bmap),
	                equal,
	                0);
	stroll_fbmap_toggle_all(&strollut_fbmap);
	strollut_fbmap_check_test_not(&strollut_fbmap, ref_bits, ref_count);

	stroll_fbmap_toggle_all(&strollut_fbmap);
	strollut_fbmap_check_test(&strollut_fbmap, ref_bits, ref_count);
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_all_14,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 1, 3, 8, 9, 10, 11, 13 };

	strollut_fbmap_check_toggle_all(&strollut_fbmap_14, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_all_32,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 31};

	strollut_fbmap_check_toggle_all(&strollut_fbmap_32, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_all_33,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32};

	strollut_fbmap_check_toggle_all(&strollut_fbmap_33, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_all_63,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 62};

	strollut_fbmap_check_toggle_all(&strollut_fbmap_63, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_all_64,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63};

	strollut_fbmap_check_toggle_all(&strollut_fbmap_64, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_all_65,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63,
	                             64};

	strollut_fbmap_check_toggle_all(&strollut_fbmap_65, ref, array_nr(ref));
}

#if __WORDSIZE == 64
CUTE_TEST_STATIC(strollut_fbmap_toggle_all_127,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 126 };

	strollut_fbmap_check_toggle_all(&strollut_fbmap_127, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_all_128,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127 };

	strollut_fbmap_check_toggle_all(&strollut_fbmap_128, ref, array_nr(ref));
}

CUTE_TEST_STATIC(strollut_fbmap_toggle_all_129,
                 CUTE_NULL_SETUP,
                 strollut_fbmap_teardown,
                 CUTE_DFLT_TMOUT)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127,
	                             128 };

	strollut_fbmap_check_toggle_all(&strollut_fbmap_129, ref, array_nr(ref));
}
#else  /* __WORDSIZE != 64 */
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_toggle_all_127)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_toggle_all_128)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_toggle_all_129)
#endif /* __WORDSIZE == 64 */

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_iter_assert)
{
	int                      err __unused;
	struct stroll_fbmap_iter iter;
	struct stroll_fbmap      bmap;

	cute_expect_assertion(
		err = stroll_fbmap_init_iter_set(NULL, &strollut_fbmap_14));
	cute_expect_assertion(err = stroll_fbmap_init_iter_set(&iter, NULL));


	bmap.nr = 0;
	bmap.bits = strollut_fbmap_14.bits;
	cute_expect_assertion(err = stroll_fbmap_init_iter_set(&iter, &bmap));

	bmap.nr = UINT_MAX;
	bmap.bits = strollut_fbmap_14.bits;
	cute_expect_assertion(err = stroll_fbmap_init_iter_set(&iter, &bmap));

	bmap.nr = 14;
	bmap.bits = NULL;
	cute_expect_assertion(err = stroll_fbmap_init_iter_set(&iter, &bmap));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_iter_assert)
#endif

static void
strollut_fbmap_check_iter(const struct stroll_fbmap * bmap,
                          const unsigned int          ref[],
                          unsigned int                count)
{
	struct stroll_fbmap_iter iter;
	unsigned int             c = 0;
	int                      b;

	stroll_fbmap_foreach_set(&iter, bmap, b) {
		cute_check_uint(c, lower, count);
		cute_check_uint((unsigned int)b, equal, ref[c]);
		c++;
	}

	cute_check_uint(c, equal, count);
}

CUTE_TEST(strollut_fbmap_iter_14)
{
	const unsigned int ref[] = { 1, 3, 8, 9, 10, 11, 13 };

	strollut_fbmap_check_iter(&strollut_fbmap_14, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_32)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 31};

	strollut_fbmap_check_iter(&strollut_fbmap_32, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_33)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32};

	strollut_fbmap_check_iter(&strollut_fbmap_33, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_63)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 62};

	strollut_fbmap_check_iter(&strollut_fbmap_63, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_64)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63};

	strollut_fbmap_check_iter(&strollut_fbmap_64, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_65)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32, 48, 63,
	                             64};

	strollut_fbmap_check_iter(&strollut_fbmap_65, ref, array_nr(ref));
}

#if __WORDSIZE == 64
CUTE_TEST(strollut_fbmap_iter_127)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 126 };

	strollut_fbmap_check_iter(&strollut_fbmap_127, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_128)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127 };

	strollut_fbmap_check_iter(&strollut_fbmap_128, ref, array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_129)
{
	const unsigned int ref[] = { 0, 12, 14, 21, 23, 28, 29, 30, 32,
	                             48, 63, 64,
	                             76, 78, 85, 87, 92, 93, 94,
	                             96, 112, 127,
	                             128 };

	strollut_fbmap_check_iter(&strollut_fbmap_129, ref, array_nr(ref));
}
#else  /* __WORDSIZE != 64 */
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_iter_127)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_iter_128)
STROLLUT_FBMAP_NO64BITS(strollut_fbmap_iter_129)
#endif /* __WORDSIZE == 64 */

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_fbmap_iter_range_assert)
{
	int                      err __unused;
	struct stroll_fbmap_iter iter;
	struct stroll_fbmap      bmap;

	cute_expect_assertion(
		err = stroll_fbmap_init_range_iter_set(NULL,
		                                       &strollut_fbmap_14,
		                                       2,
		                                       8));
	cute_expect_assertion(
		err = stroll_fbmap_init_range_iter_set(&iter,
		                                       NULL,
		                                       2,
		                                       8));
	bmap.nr = 0;
	bmap.bits = strollut_fbmap_14.bits;
	cute_expect_assertion(err = stroll_fbmap_init_range_iter_set(&iter,
	                                                             &bmap,
	                                                             2,
	                                                             8));
	bmap.nr = 1;
	bmap.bits = strollut_fbmap_14.bits;
	cute_expect_assertion(err = stroll_fbmap_init_range_iter_set(&iter,
	                                                             &bmap,
	                                                             2,
	                                                             8));
	bmap.nr = 3;
	bmap.bits = strollut_fbmap_14.bits;
	cute_expect_assertion(err = stroll_fbmap_init_range_iter_set(&iter,
	                                                             &bmap,
	                                                             2,
	                                                             8));
	bmap.nr = UINT_MAX;
	bmap.bits = strollut_fbmap_14.bits;
	cute_expect_assertion(err = stroll_fbmap_init_range_iter_set(&iter,
	                                                             &bmap,
	                                                             2,
	                                                             8));
	bmap.nr = 14;
	bmap.bits = NULL;
	cute_expect_assertion(err = stroll_fbmap_init_range_iter_set(&iter,
	                                                             &bmap,
	                                                             2,
	                                                             8));
}
#else
STROLLUT_FBMAP_NOASSERT(strollut_fbmap_iter_range_assert)
#endif

static void
strollut_fbmap_check_range_iter(const struct stroll_fbmap * bmap,
                                unsigned int                start_bit,
                                unsigned int                bit_count,
                                const unsigned int          ref[],
                                unsigned int                ref_count)
{
	struct stroll_fbmap_iter iter;
	unsigned int             c = 0;
	int                      b;

	stroll_fbmap_foreach_range_set(&iter, bmap, start_bit, bit_count, b) {
		cute_check_uint(c, lower, ref_count);
		cute_check_uint((unsigned int)b, equal, ref[c]);
		c++;
	}

	cute_check_uint(c, equal, ref_count);
}

CUTE_TEST(strollut_fbmap_iter_start_range_14)
{
	const unsigned int ref[] = { 1, 3, 8 };

	strollut_fbmap_check_range_iter(&strollut_fbmap_14,
	                                0,
	                                9,
	                                ref,
	                                array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_middle_range_14)
{
	const unsigned int ref[] = { 3, 8, 9 };

	strollut_fbmap_check_range_iter(&strollut_fbmap_14,
	                                3,
	                                7,
	                                ref,
	                                array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_end_range_14)
{
	const unsigned int ref[] = { 3, 8, 9, 10, 11, 13 };

	strollut_fbmap_check_range_iter(&strollut_fbmap_14,
	                                2,
	                                12,
	                                ref,
	                                array_nr(ref));
}

static const struct stroll_fbmap strollut_fbmap_192 = {
	.nr   = 192,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0x00000001UL, 0x00018000UL,
	                             0x80000000UL, 0x00000001UL,
	                             0x00018000UL, 0x80000004UL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0x0001800000000001UL,
	                             0x0000000180000000UL,
	                             0x8000000400018000UL })
#endif
};

CUTE_TEST(strollut_fbmap_iter_start_range_192)
{
	const unsigned int ref[] = { 0, 47, 48 };

	strollut_fbmap_check_range_iter(&strollut_fbmap_192,
	                                0,
	                                56,
	                                ref,
	                                array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_middle_range_192)
{
	const unsigned int ref[] = { 48, 95, 96, 143 };

	strollut_fbmap_check_range_iter(&strollut_fbmap_192,
	                                48,
	                                96,
	                                ref,
	                                array_nr(ref));
}

CUTE_TEST(strollut_fbmap_iter_end_range_192)
{
	const unsigned int ref[] = { 162, 191 };

	strollut_fbmap_check_range_iter(&strollut_fbmap_192,
	                                161,
	                                31,
	                                ref,
	                                array_nr(ref));
}

/******************************************************************************
 * Top-level bitmap support
 ******************************************************************************/

CUTE_GROUP(strollut_fbmap_group) = {
	CUTE_REF(strollut_fbmap_nr_assert),
	CUTE_REF(strollut_fbmap_nr),

	CUTE_REF(strollut_fbmap_test_assert),
	CUTE_REF(strollut_fbmap_test_14),
	CUTE_REF(strollut_fbmap_test_32),
	CUTE_REF(strollut_fbmap_test_33),
	CUTE_REF(strollut_fbmap_test_63),
	CUTE_REF(strollut_fbmap_test_64),
	CUTE_REF(strollut_fbmap_test_65),
	CUTE_REF(strollut_fbmap_test_127),
	CUTE_REF(strollut_fbmap_test_128),
	CUTE_REF(strollut_fbmap_test_129),

	CUTE_REF(strollut_fbmap_hweight_assert),
	CUTE_REF(strollut_fbmap_hweight_14),
	CUTE_REF(strollut_fbmap_hweight_32),
	CUTE_REF(strollut_fbmap_hweight_33),
	CUTE_REF(strollut_fbmap_hweight_63),
	CUTE_REF(strollut_fbmap_hweight_64),
	CUTE_REF(strollut_fbmap_hweight_65),
	CUTE_REF(strollut_fbmap_hweight_129),

	CUTE_REF(strollut_fbmap_test_all_assert),
	CUTE_REF(strollut_fbmap_test_all_null_14),
	CUTE_REF(strollut_fbmap_test_all_null_33),
	CUTE_REF(strollut_fbmap_test_all_null_65),
	CUTE_REF(strollut_fbmap_test_all_null_129),

	CUTE_REF(strollut_fbmap_test_range_14),
	CUTE_REF(strollut_fbmap_test_range_32),
	CUTE_REF(strollut_fbmap_test_range_33),
	CUTE_REF(strollut_fbmap_test_range_63),
	CUTE_REF(strollut_fbmap_test_range_64),
	CUTE_REF(strollut_fbmap_test_range_65),
	CUTE_REF(strollut_fbmap_test_range_127),
	CUTE_REF(strollut_fbmap_test_range_128),
	CUTE_REF(strollut_fbmap_test_range_129),

	CUTE_REF(strollut_fbmap_init_clear_assert),
	CUTE_REF(strollut_fbmap_init_clear_14),
	CUTE_REF(strollut_fbmap_init_clear_191),
	CUTE_REF(strollut_fbmap_init_clear_192),
	CUTE_REF(strollut_fbmap_init_clear_193),

	CUTE_REF(strollut_fbmap_init_set_assert),
	CUTE_REF(strollut_fbmap_init_set_14),
	CUTE_REF(strollut_fbmap_init_set_191),
	CUTE_REF(strollut_fbmap_init_set_192),
	CUTE_REF(strollut_fbmap_init_set_193),

	CUTE_REF(strollut_fbmap_set_all_assert),
	CUTE_REF(strollut_fbmap_set_all_14),
	CUTE_REF(strollut_fbmap_set_all_191),
	CUTE_REF(strollut_fbmap_set_all_192),
	CUTE_REF(strollut_fbmap_set_all_193),

	CUTE_REF(strollut_fbmap_clear_all_assert),
	CUTE_REF(strollut_fbmap_clear_all_14),
	CUTE_REF(strollut_fbmap_clear_all_191),
	CUTE_REF(strollut_fbmap_clear_all_192),
	CUTE_REF(strollut_fbmap_clear_all_193),

	CUTE_REF(strollut_fbmap_set_assert),
	CUTE_REF(strollut_fbmap_set_14),
	CUTE_REF(strollut_fbmap_set_32),
	CUTE_REF(strollut_fbmap_set_33),
	CUTE_REF(strollut_fbmap_set_63),
	CUTE_REF(strollut_fbmap_set_64),
	CUTE_REF(strollut_fbmap_set_65),
	CUTE_REF(strollut_fbmap_set_127),
	CUTE_REF(strollut_fbmap_set_128),
	CUTE_REF(strollut_fbmap_set_129),

	CUTE_REF(strollut_fbmap_clear_assert),
	CUTE_REF(strollut_fbmap_clear_14),
	CUTE_REF(strollut_fbmap_clear_32),
	CUTE_REF(strollut_fbmap_clear_33),
	CUTE_REF(strollut_fbmap_clear_63),
	CUTE_REF(strollut_fbmap_clear_64),
	CUTE_REF(strollut_fbmap_clear_65),
	CUTE_REF(strollut_fbmap_clear_127),
	CUTE_REF(strollut_fbmap_clear_128),
	CUTE_REF(strollut_fbmap_clear_129),

	CUTE_REF(strollut_fbmap_toggle_assert),
	CUTE_REF(strollut_fbmap_toggle_14),
	CUTE_REF(strollut_fbmap_toggle_32),
	CUTE_REF(strollut_fbmap_toggle_33),
	CUTE_REF(strollut_fbmap_toggle_63),
	CUTE_REF(strollut_fbmap_toggle_64),
	CUTE_REF(strollut_fbmap_toggle_65),
	CUTE_REF(strollut_fbmap_toggle_127),
	CUTE_REF(strollut_fbmap_toggle_128),
	CUTE_REF(strollut_fbmap_toggle_129),

	CUTE_REF(strollut_fbmap_init_dup_assert),
	CUTE_REF(strollut_fbmap_init_dup),

	CUTE_REF(strollut_fbmap_toggle_all_assert),
	CUTE_REF(strollut_fbmap_toggle_all_14),
	CUTE_REF(strollut_fbmap_toggle_all_32),
	CUTE_REF(strollut_fbmap_toggle_all_33),
	CUTE_REF(strollut_fbmap_toggle_all_63),
	CUTE_REF(strollut_fbmap_toggle_all_64),
	CUTE_REF(strollut_fbmap_toggle_all_65),
	CUTE_REF(strollut_fbmap_toggle_all_127),
	CUTE_REF(strollut_fbmap_toggle_all_128),
	CUTE_REF(strollut_fbmap_toggle_all_129),

	CUTE_REF(strollut_fbmap_iter_assert),
	CUTE_REF(strollut_fbmap_iter_14),
	CUTE_REF(strollut_fbmap_iter_32),
	CUTE_REF(strollut_fbmap_iter_33),
	CUTE_REF(strollut_fbmap_iter_63),
	CUTE_REF(strollut_fbmap_iter_64),
	CUTE_REF(strollut_fbmap_iter_65),
	CUTE_REF(strollut_fbmap_iter_127),
	CUTE_REF(strollut_fbmap_iter_128),
	CUTE_REF(strollut_fbmap_iter_129),

	CUTE_REF(strollut_fbmap_iter_range_assert),
	CUTE_REF(strollut_fbmap_iter_start_range_14),
	CUTE_REF(strollut_fbmap_iter_middle_range_14),
	CUTE_REF(strollut_fbmap_iter_end_range_14),
	CUTE_REF(strollut_fbmap_iter_start_range_192),
	CUTE_REF(strollut_fbmap_iter_middle_range_192),
	CUTE_REF(strollut_fbmap_iter_end_range_192)
};

CUTE_SUITE_EXTERN(strollut_fbmap_suite,
                  strollut_fbmap_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
