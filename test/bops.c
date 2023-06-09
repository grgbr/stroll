#include "utest.h"
#include "stroll/bops.h"

static void
stroll_bops32_utest_fls(void ** state __unused)
{
	uint32_t     word;
	unsigned int bit;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bops32_fls(0U));
#endif

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		assert_int_equal(stroll_bops32_fls(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops32_fls(word), bit + 1);
	}
}

static void
stroll_bops_utest_uint_fls(void ** state __unused)
{
	unsigned int word;
	unsigned int bit;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bops_fls(0U));
#endif

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1U << bit;
		assert_int_equal(stroll_bops_fls(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops_fls(word), bit + 1);
	}
}

static void
stroll_bops64_utest_fls(void ** state __unused)
{
	uint64_t     word;
	unsigned int bit;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bops64_fls(0U));
#endif

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		assert_int_equal(stroll_bops64_fls(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops64_fls(word), bit + 1);
	}
}

static void
stroll_bops_utest_ulong_fls(void ** state __unused)
{
	unsigned long word;
	unsigned int  bit;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bops_fls(0U));
#endif

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		assert_int_equal(stroll_bops_fls(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops_fls(word), bit + 1);
	}
}

static void
stroll_bops32_utest_ffs(void ** state __unused)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		assert_int_equal(stroll_bops32_ffs(word), bit + 1);
	}

	word = UINT32_MAX;
	for (bit = 0; bit < 32; bit++) {
		assert_int_equal(stroll_bops32_ffs(word), bit + 1);
		word <<= 1;
	}
}

static void
stroll_bops_utest_uint_ffs(void ** state __unused)
{
	unsigned int word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1U << bit;
		assert_int_equal(stroll_bops_ffs(word), bit + 1);
	}

	word = UINT32_MAX;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		assert_int_equal(stroll_bops_ffs(word), bit + 1);
		word <<= 1;
	}
}

static void
stroll_bops64_utest_ffs(void ** state __unused)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		assert_int_equal(stroll_bops64_ffs(word), bit + 1);
	}

	word = UINT64_MAX;
	for (bit = 0; bit < 64; bit++) {
		assert_int_equal(stroll_bops64_ffs(word), bit + 1);
		word <<= 1;
	}
}

static void
stroll_bops_utest_ulong_ffs(void ** state __unused)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		assert_int_equal(stroll_bops_ffs(word), bit + 1);
	}

	word = ULONG_MAX;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		assert_int_equal(stroll_bops_ffs(word), bit + 1);
		word <<= 1;
	}
}

static void
stroll_bops32_utest_ffc(void ** state __unused)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = ~(UINT32_C(1) << bit);
		assert_int_equal(stroll_bops32_ffc(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		assert_int_equal(stroll_bops32_ffc(word), bit + 1);
		word = (word << 1) | 1;
	}
}

static void
stroll_bops_utest_uint_ffc(void ** state __unused)
{
	unsigned int word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = ~(1U << bit);
		assert_int_equal(stroll_bops_ffc(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		assert_int_equal(stroll_bops_ffc(word), bit + 1);
		word = (word << 1) | 1;
	}
}

static void
stroll_bops64_utest_ffc(void ** state __unused)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = ~(UINT64_C(1) << bit);
		assert_int_equal(stroll_bops64_ffc(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		assert_int_equal(stroll_bops64_ffc(word), bit + 1);
		word = (word << 1) | 1;
	}
}

static void
stroll_bops_utest_ulong_ffc(void ** state __unused)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = ~(1UL << bit);
		assert_int_equal(stroll_bops_ffc(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		assert_int_equal(stroll_bops_ffc(word), bit + 1);
		word = (word << 1) | 1;
	}
}

static void
stroll_bops32_utest_hweight(void ** state __unused)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < 32; bit++) {
		word = UINT32_C(1) << bit;
		assert_int_equal(stroll_bops32_hweight(word), 1);
	}

	word = 0;
	for (bit = 0; bit < 32; bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops32_hweight(word), bit + 1);
	}
}

static void
stroll_bops_utest_uint_hweight(void ** state __unused)
{
	unsigned int word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1U << bit;
		assert_int_equal(stroll_bops_hweight(word), 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops_hweight(word), bit + 1);
	}
}

static void
stroll_bops64_utest_hweight(void ** state __unused)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < 64; bit++) {
		word = UINT64_C(1) << bit;
		assert_int_equal(stroll_bops64_hweight(word), 1);
	}

	word = 0;
	for (bit = 0; bit < 64; bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops64_hweight(word), bit + 1);
	}
}

static void
stroll_bops_utest_ulong_hweight(void ** state __unused)
{
	unsigned long word;
	unsigned int  bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = 1UL << bit;
		assert_int_equal(stroll_bops_hweight(word), 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops_hweight(word), bit + 1);
	}
}

static const struct CMUnitTest stroll_bops_utests[] = {
	cmocka_unit_test(stroll_bops32_utest_fls),
	cmocka_unit_test(stroll_bops_utest_uint_fls),
	cmocka_unit_test(stroll_bops64_utest_fls),
	cmocka_unit_test(stroll_bops_utest_ulong_fls),

	cmocka_unit_test(stroll_bops32_utest_ffs),
	cmocka_unit_test(stroll_bops_utest_uint_ffs),
	cmocka_unit_test(stroll_bops64_utest_ffs),
	cmocka_unit_test(stroll_bops_utest_ulong_ffs),

	cmocka_unit_test(stroll_bops32_utest_hweight),
	cmocka_unit_test(stroll_bops_utest_uint_hweight),
	cmocka_unit_test(stroll_bops64_utest_hweight),
	cmocka_unit_test(stroll_bops_utest_ulong_hweight),

	cmocka_unit_test(stroll_bops32_utest_ffc),
	cmocka_unit_test(stroll_bops_utest_uint_ffc),
	cmocka_unit_test(stroll_bops64_utest_ffc),
	cmocka_unit_test(stroll_bops_utest_ulong_ffc),
};

int
main(void)
{
	return cmocka_run_group_tests_name("Bit operations (bops)",
	                                   stroll_bops_utests,
	                                   NULL,
	                                   NULL);
}
