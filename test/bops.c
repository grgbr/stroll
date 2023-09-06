#include "stroll/bops.h"
#include "utest.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bops32_fls_assert)
{
	cute_expect_assertion(stroll_bops32_fls(0U));
}
#else
CUTE_TEST(strollut_bops32_fls_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bops32_fls)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		cute_check_uint(stroll_bops32_fls(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops32_fls(word), equal, bit + 1);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bops_uint_fls_assert)
{
	cute_expect_assertion(stroll_bops_fls(0U));
}
#else
CUTE_TEST(strollut_bops_uint_fls_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bops_uint_fls)
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
CUTE_TEST(strollut_bops64_fls_assert)
{
	cute_expect_assertion(stroll_bops64_fls(0U));
}
#else
CUTE_TEST(strollut_bops64_fls_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bops64_fls)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		cute_check_uint(stroll_bops64_fls(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops64_fls(word), equal, bit + 1);
	}
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_bops_ulong_fls_assert)
{
	cute_expect_assertion(stroll_bops_fls(0UL));
}
#else
CUTE_TEST(strollut_bops_ulong_fls_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_bops_ulong_fls)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		cute_check_uint(stroll_bops_fls(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_fls(word), equal, bit + 1);
	}
}

CUTE_TEST(strollut_bops32_ffs)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		cute_check_uint(stroll_bops32_ffs(word), equal, bit + 1);
	}

	word = UINT32_MAX;
	for (bit = 0; bit < 32; bit++) {
		cute_check_uint(stroll_bops32_ffs(word), equal, bit + 1);
		word <<= 1;
	}
}

CUTE_TEST(strollut_bops_uint_ffs)
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

CUTE_TEST(strollut_bops64_ffs)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		cute_check_uint(stroll_bops64_ffs(word), equal, bit + 1);
	}

	word = UINT64_MAX;
	for (bit = 0; bit < 64; bit++) {
		cute_check_uint(stroll_bops64_ffs(word), equal, bit + 1);
		word <<= 1;
	}
}

CUTE_TEST(strollut_bops_ulong_ffs)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		cute_check_uint(stroll_bops_ffs(word), equal, bit + 1);
	}

	word = ULONG_MAX;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		cute_check_uint(stroll_bops_ffs(word), equal, bit + 1);
		word <<= 1;
	}
}

CUTE_TEST(strollut_bops32_ffc)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = ~(UINT32_C(1) << bit);
		cute_check_uint(stroll_bops32_ffc(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		cute_check_uint(stroll_bops32_ffc(word), equal, bit + 1);
		word = (word << 1) | 1;
	}
}

CUTE_TEST(strollut_bops_uint_ffc)
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

CUTE_TEST(strollut_bops64_ffc)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = ~(UINT64_C(1) << bit);
		cute_check_uint(stroll_bops64_ffc(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		cute_check_uint(stroll_bops64_ffc(word), equal, bit + 1);
		word = (word << 1) | 1;
	}
}

CUTE_TEST(strollut_bops_ulong_ffc)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = ~(1UL << bit);
		cute_check_uint(stroll_bops_ffc(word), equal, bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		cute_check_uint(stroll_bops_ffc(word), equal, bit + 1);
		word = (word << 1) | 1;
	}
}

CUTE_TEST(strollut_bops32_hweight)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		cute_check_uint(stroll_bops32_hweight(word), equal, 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops32_hweight(word), equal, bit + 1);
	}
}

CUTE_TEST(strollut_bops_uint_hweight)
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

CUTE_TEST(strollut_bops64_hweight)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		cute_check_uint(stroll_bops64_hweight(word), equal, 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops64_hweight(word), equal, bit + 1);
	}
}

CUTE_TEST(strollut_bops_ulong_hweight)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		cute_check_uint(stroll_bops_hweight(word), equal, 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		cute_check_uint(stroll_bops_hweight(word), equal, bit + 1);
	}
}

CUTE_GROUP(strollut_bops_group) = {
	CUTE_REF(strollut_bops32_fls_assert),
	CUTE_REF(strollut_bops32_fls),
	CUTE_REF(strollut_bops_uint_fls_assert),
	CUTE_REF(strollut_bops_uint_fls),
	CUTE_REF(strollut_bops64_fls_assert),
	CUTE_REF(strollut_bops64_fls),
	CUTE_REF(strollut_bops_ulong_fls_assert),
	CUTE_REF(strollut_bops_ulong_fls),

	CUTE_REF(strollut_bops32_ffs),
	CUTE_REF(strollut_bops_uint_ffs),
	CUTE_REF(strollut_bops64_ffs),
	CUTE_REF(strollut_bops_ulong_ffs),

	CUTE_REF(strollut_bops32_ffc),
	CUTE_REF(strollut_bops_uint_ffc),
	CUTE_REF(strollut_bops64_ffc),
	CUTE_REF(strollut_bops_ulong_ffc),

	CUTE_REF(strollut_bops32_hweight),
	CUTE_REF(strollut_bops_uint_hweight),
	CUTE_REF(strollut_bops64_hweight),
	CUTE_REF(strollut_bops_ulong_hweight),
};

CUTE_SUITE_EXTERN(strollut_bops_suite,
                  strollut_bops_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
