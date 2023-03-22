#include "stroll/bitops.h"

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

static void
stroll_bops_utest_fls32(void ** state __unused)
{
	uint32_t     word;
	unsigned int bit;

	expect_assert_failure(stroll_bops_fls32(0));

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (uint32_t)1 << bit;
		assert_int_equal(stroll_bops_fls32(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops_fls32(word), bit + 1);
	}
}

static void
stroll_bops_utest_uint_fls(void ** state __unused)
{
	unsigned int word;
	unsigned int bit;

	expect_assert_failure(stroll_bops_fls(0));

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
stroll_bops_utest_fls64(void ** state __unused)
{
	uint64_t     word;
	unsigned int bit;

	expect_assert_failure(stroll_bops_fls64(0));

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (uint64_t)1 << bit;
		assert_int_equal(stroll_bops_fls64(word), bit + 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops_fls64(word), bit + 1);
	}
}

static void
stroll_bops_utest_ulong_fls(void ** state __unused)
{
	unsigned long word;
	unsigned int  bit;

	expect_assert_failure(stroll_bops_fls(0));

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
stroll_bops_utest_ffs32(void ** state __unused)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (uint32_t)1 << bit;
		assert_int_equal(stroll_bops_ffs32(word), bit + 1);
	}

	word = UINT32_MAX;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		assert_int_equal(stroll_bops_ffs32(word), bit + 1);
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
stroll_bops_utest_ffs64(void ** state __unused)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (uint64_t)1 << bit;
		assert_int_equal(stroll_bops_ffs64(word), bit + 1);
	}

	word = UINT64_MAX;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		assert_int_equal(stroll_bops_ffs64(word), bit + 1);
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
stroll_bops_utest_hweight32(void ** state __unused)
{
	uint32_t     word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (uint32_t)1 << bit;
		assert_int_equal(stroll_bops_hweight32(word), 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops_hweight32(word), bit + 1);
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
stroll_bops_utest_hweight64(void ** state __unused)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (uint64_t)1 << bit;
		assert_int_equal(stroll_bops_hweight64(word), 1);
	}

	word = 0;
	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (word << 1) | 1;
		assert_int_equal(stroll_bops_hweight64(word), bit + 1);
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
	cmocka_unit_test(stroll_bops_utest_fls32),
	cmocka_unit_test(stroll_bops_utest_uint_fls),
	cmocka_unit_test(stroll_bops_utest_fls64),
	cmocka_unit_test(stroll_bops_utest_ulong_fls),
	cmocka_unit_test(stroll_bops_utest_ffs32),
	cmocka_unit_test(stroll_bops_utest_uint_ffs),
	cmocka_unit_test(stroll_bops_utest_ffs64),
	cmocka_unit_test(stroll_bops_utest_ulong_ffs),
	cmocka_unit_test(stroll_bops_utest_hweight32),
	cmocka_unit_test(stroll_bops_utest_uint_hweight),
	cmocka_unit_test(stroll_bops_utest_hweight64),
	cmocka_unit_test(stroll_bops_utest_ulong_hweight),
};

int
main(void)
{
	return cmocka_run_group_tests_name("Bit operations (bops)",
	                                   stroll_bops_utests,
	                                   NULL,
	                                   NULL);
}
