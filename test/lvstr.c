#include "utest.h"
#include "stroll/lvstr.h"

static void
stroll_lvstr_utest_empty(void ** state __unused)
{
	struct stroll_lvstr lvstr = STROLL_LVSTR_INIT;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_init(NULL));
	expect_assert_failure(stroll_lvstr_cstr(NULL));
	expect_assert_failure(stroll_lvstr_len(NULL));
	expect_assert_failure(stroll_lvstr_fini(NULL));
#endif

	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.cstr = (void *)0xdeadbeef;

	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);
}

static const struct CMUnitTest stroll_lvstr_utests[] = {
	cmocka_unit_test(stroll_lvstr_utest_empty),
};

int
main(void)
{
	return cmocka_run_group_tests_name("Length-Value Strings (lvstr)",
	                                   stroll_lvstr_utests,
	                                   NULL,
	                                   NULL);
}
