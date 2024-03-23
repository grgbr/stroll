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

#if (__WORDSIZE != 32) && (__WORDSIZE != 64)
#error "Unsupported machine word size !"
#endif

#define STROLLUT_FBMAP_NOASSERT(_test) \
	CUTE_TEST(_test) { cute_skip("assertion unsupported"); }

#define STROLLUT_FBMAP_NO64BITS(_test) \
	CUTE_TEST(_test) { cute_skip("32-bits machine word only"); }

#if 0
bool
stroll_fbmap_test_range(const struct stroll_fbmap * __restrict bmap,
                        unsigned int                           start_bit,
                        unsigned int                           bit_count)
void
stroll_fbmap_set(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
void
stroll_fbmap_set_all(struct stroll_fbmap * __restrict bmap)
void
stroll_fbmap_clear(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
void
stroll_fbmap_clear_all(struct stroll_fbmap * __restrict bmap)
void
stroll_fbmap_toggle(struct stroll_fbmap * __restrict bmap, unsigned int bit_no)
void
stroll_fbmap_toggle_all(struct stroll_fbmap * __restrict bmap)
int
stroll_fbmap_init_set(struct stroll_fbmap * __restrict bmap,
                      unsigned int                     bit_nr)
int
stroll_fbmap_init_clear(struct stroll_fbmap * __restrict bmap,
                        unsigned int                     bit_nr)
void
stroll_fbmap_fini(struct stroll_fbmap * __restrict bmap)
int
stroll_fbmap_step_iter(struct stroll_fbmap_iter * __restrict iter)
int
stroll_fbmap_init_range_iter(struct stroll_fbmap_iter * __restrict  iter,
                             const struct stroll_fbmap * __restrict bmap,
                             unsigned int                           start_bit,
                             unsigned int                           bit_count)
stroll_fbmap_foreach_range_bit(_iter, _bmap, _start_bit, _bit_count, _bit_no)
int
stroll_fbmap_init_iter(struct stroll_fbmap_iter * __restrict  iter,
                       const struct stroll_fbmap * __restrict bmap)
stroll_fbmap_foreach_bit(_iter, _bmap, _bit_no)
#endif

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

static const struct stroll_fbmap strollut_fbmap_null_33 = {
	.nr   = 33,
#if __WORDSIZE == 32
	.bits = ((unsigned long []){ 0UL, 0xffffffeUL })
#elif __WORDSIZE == 64
	.bits = ((unsigned long []){ 0xfffffffe00000000UL })
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
};

CUTE_SUITE_EXTERN(strollut_fbmap_suite,
                  strollut_fbmap_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
