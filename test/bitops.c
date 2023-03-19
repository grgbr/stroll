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

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (uint32_t)1 << bit;
		assert_int_equal(stroll_bops_fls32(word), bit + 1);
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
}

static void
stroll_bops_utest_fls64(void ** state __unused)
{
	uint64_t     word;
	unsigned int bit;

	for (bit = 0; bit < (sizeof(word) * CHAR_BIT); bit++) {
		word = (uint64_t)1 << bit;
		assert_int_equal(stroll_bops_fls64(word), bit + 1);
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
}

static const struct CMUnitTest stroll_bops_utests[] = {
	cmocka_unit_test(stroll_bops_utest_fls32),
	cmocka_unit_test(stroll_bops_utest_ffs32),
	cmocka_unit_test(stroll_bops_utest_fls64),
	cmocka_unit_test(stroll_bops_utest_ffs64)
};

int
main(void)
{
	return cmocka_run_group_tests_name("Bit operations (bops)",
	                                   stroll_bops_utests,
	                                   NULL,
	                                   NULL);
}
