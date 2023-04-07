#include "stroll/cdefs.h"
#include "utest.h"

static void
stroll_cdefs_utest_min(void ** state __unused)
{
	int a, b, c;

	assert_int_equal(stroll_min(0, 0), 0);
	assert_int_equal(stroll_min(0, 1), 0);
	assert_int_equal(stroll_min(-1, 1), -1);
	assert_int_equal(stroll_min(-2, -1), -2);

	a = 0, b = 0;
	assert_int_equal(stroll_min(a, b), 0);
	a = 0, b = 1;
	assert_int_equal(stroll_min(a, b), 0);
	a = -1, b = 1;
	assert_int_equal(stroll_min(a, b), -1);
	a = -2, b = -1;
	assert_int_equal(stroll_min(a, b), -2);

	assert_int_equal(stroll_min(stroll_min(0, 1), -1), -1);
	a = -2, b = 1, c = 0;
	assert_int_equal(stroll_min(stroll_min(c, a), b), -2);
}

static void
stroll_cdefs_utest_max(void ** state __unused)
{
	int a, b, c;

	assert_int_equal(stroll_max(0, 0), 0);
	assert_int_equal(stroll_max(0, 1), 1);
	assert_int_equal(stroll_max(-1, 1), 1);
	assert_int_equal(stroll_max(-2, -1), -1);

	a = 0, b = 0;
	assert_int_equal(stroll_max(a, b), 0);
	a = 0, b = 1;
	assert_int_equal(stroll_max(a, b), 1);
	a = -1, b = 1;
	assert_int_equal(stroll_max(a, b), 1);
	a = -2, b = -1;
	assert_int_equal(stroll_max(a, b), -1);

	assert_int_equal(stroll_max(stroll_max(0, 1), -1), 1);
	a = -2, b = 1, c = 0;
	assert_int_equal(stroll_max(stroll_max(c, a), b), 1);
}

static void
stroll_cdefs_utest_abs(void ** state __unused)
{
	int a, b;

	assert_int_equal(stroll_abs(0), 0);
	assert_int_equal(stroll_abs(1), 1);
	assert_int_equal(stroll_abs(-2), 2);

	a = 0;
	assert_int_equal(stroll_abs(a), 0);
	a = 1;
	assert_int_equal(stroll_abs(a), 1);
	a = -2;
	assert_int_equal(stroll_abs(a), 2);

	assert_int_equal(stroll_max(stroll_abs(-1), 1), 1);
	a = -1, b = 2;
	assert_int_equal(stroll_max(stroll_abs(a), b), 2);
	a = -1, b = 2;
	assert_int_equal(stroll_abs(stroll_min(a, b)), 1);
}

static const struct CMUnitTest stroll_cdefs_utests[] = {
	cmocka_unit_test(stroll_cdefs_utest_min),
	cmocka_unit_test(stroll_cdefs_utest_max),
	cmocka_unit_test(stroll_cdefs_utest_abs)
};

int
main(void)
{
	return cmocka_run_group_tests_name("Common definitions (cdefs)",
	                                   stroll_cdefs_utests,
	                                   NULL,
	                                   NULL);
}
