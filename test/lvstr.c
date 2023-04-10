#include "utest.h"
#include "stroll/lvstr.h"

static void
stroll_lvstr_utest_empty(void ** state __unused)
{
	struct stroll_lvstr lvstr = STROLL_LVSTR_INIT;

#if defined(CONFIG_STROLL_ASSERT_API)
	const char * str __unused;
	size_t       len __unused;

	expect_assert_failure(stroll_lvstr_init(NULL));
	expect_assert_failure(str = stroll_lvstr_cstr(NULL));
	expect_assert_failure(len = stroll_lvstr_len(NULL));
	expect_assert_failure(stroll_lvstr_fini(NULL));
#endif

	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_nlend(void ** state __unused)
{
	const char          * str = "test";
	const char          * str2 = "test2";
	size_t                len = strlen(str);
	struct stroll_lvstr   lvstr = STROLL_LVSTR_INIT_NLEND(str, len);

	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_lvstr_fini(&lvstr);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_init_nlend(NULL, str, len));
	expect_assert_failure(stroll_lvstr_init_nlend(&lvstr, NULL, len));
	expect_assert_failure(stroll_lvstr_init_nlend(&lvstr, str, len - 1));
#endif

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init_nlend(&lvstr, str, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init_nlend(&lvstr, str, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_nlend(NULL, str, len));
	expect_assert_failure(stroll_lvstr_nlend(&lvstr, NULL, len));
	expect_assert_failure(stroll_lvstr_nlend(&lvstr, str, len - 1));
#endif

	len = strlen(str2);
	stroll_lvstr_nlend(&lvstr, str2, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);

	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_lend(void ** state __unused)
{

	struct stroll_lvstr   lvstr = STROLL_LVSTR_INIT_LEND("test");
	const char          * str = "test";
	const char          * str2 = "test2";
	size_t                len = strlen(str);

	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_init_lend(NULL, str));
	expect_assert_failure(stroll_lvstr_init_lend(&lvstr, NULL));
#endif

	stroll_lvstr_init_lend(&lvstr, str);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init_lend(&lvstr, str);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_lend(NULL, str));
	expect_assert_failure(stroll_lvstr_lend(&lvstr, NULL));
#endif

	len = strlen(str2);
	stroll_lvstr_lend(&lvstr, str2);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);

	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_ncede(void ** state __unused)
{
	char                * str = strdup("test");
	char                * str2 = strdup("test2");
	size_t                len = strlen(str);
	struct stroll_lvstr   lvstr = STROLL_LVSTR_INIT_NCEDE(str, len);

	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(str, len + 1);
	stroll_lvstr_fini(&lvstr);

	str = strdup("test");
	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_ncede(&lvstr, str, len);
#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_init_ncede(NULL, str, len));
	expect_assert_failure(stroll_lvstr_init_ncede(&lvstr, NULL, len));
	expect_assert_failure(stroll_lvstr_init_ncede(&lvstr, str, len - 1));
#endif
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(str, len + 1);
	stroll_lvstr_fini(&lvstr);

	str = strdup("test");
	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_ncede(&lvstr, str, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_ncede(NULL, str, len));
	expect_assert_failure(stroll_lvstr_ncede(&lvstr, NULL, len));
	expect_assert_failure(stroll_lvstr_ncede(&lvstr, str, len - 1));
#endif

	len = strlen(str2);
	stroll_lvstr_ncede(&lvstr, str2, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);

	stroll_utest_expect_free_arg(str2, len + 1);
	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_cede(void ** state __unused)
{
	char                * str1 = strdup("test");
	size_t                len1 = strlen(str1);
	char                * str2 = strdup("test2");
	size_t                len2 = strlen(str2);
	struct stroll_lvstr   lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_init_cede(NULL, str1));
	expect_assert_failure(stroll_lvstr_init_cede(&lvstr, NULL));
	expect_assert_failure(stroll_lvstr_init_cede(NULL, NULL));
#endif

	stroll_lvstr_init_cede(&lvstr, str1);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);
	stroll_utest_expect_free_arg(str1, len1 + 1);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_cede(&lvstr, str2);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2);
	stroll_utest_expect_free_arg(str2, len2 + 1);
	stroll_lvstr_fini(&lvstr);

	str1 = strdup("test");
	str2 = strdup("test2");
	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_cede(&lvstr, str1);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_cede(NULL, str1));
	expect_assert_failure(stroll_lvstr_cede(&lvstr, NULL));
	expect_assert_failure(stroll_lvstr_cede(NULL, NULL));
#endif

	stroll_utest_expect_free_arg(str1, len1 + 1);
	stroll_lvstr_cede(&lvstr, str2);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2);

	stroll_utest_expect_free_arg(str2, len2 + 1);
	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_ndup(void ** state __unused)
{
	const char          * str1 = "test";
	size_t                len1 = strlen(str1);
	const char          * str2 = "test2";
	size_t                len2 = strlen(str2);
	struct stroll_lvstr   lvstr;
	int                   ret __unused;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(ret = stroll_lvstr_init_ndup(NULL, str1, len1));
	expect_assert_failure(ret = stroll_lvstr_init_ndup(&lvstr, NULL, len1));
#endif
	assert_int_equal(stroll_lvstr_init_ndup(&lvstr, str1, len1), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);
	stroll_utest_expect_free_arg(lvstr.rwstr, len1 + 1);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	assert_int_equal(stroll_lvstr_init_ndup(&lvstr, str2, len2 - 1), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2 - 1);
	stroll_utest_expect_free_arg(lvstr.rwstr, len2 - 1);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	assert_int_equal(stroll_lvstr_init_ndup(&lvstr, str1, len1), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(ret = stroll_lvstr_ndup(NULL, str2, len2));
	expect_assert_failure(ret = stroll_lvstr_ndup(&lvstr, NULL, len2));
#endif
	assert_int_equal(stroll_lvstr_ndup(&lvstr, str2, len2), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2);

	stroll_utest_expect_free_arg(lvstr.rwstr, len2 + 1);
	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_dup(void ** state __unused)
{
	const char          * str1 = "test";
	size_t                len1 = strlen(str1);
	const char          * str2 = "test2";
	size_t                len2 = strlen(str2);
	struct stroll_lvstr   lvstr;
	int                   ret __unused;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(ret = stroll_lvstr_init_dup(NULL, str1));
	expect_assert_failure(ret = stroll_lvstr_init_dup(&lvstr, NULL));
#endif
	assert_int_equal(stroll_lvstr_init_dup(&lvstr, str1), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);
	stroll_utest_expect_free_arg(lvstr.rwstr, len1 + 1);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	assert_int_equal(stroll_lvstr_init_dup(&lvstr, str1), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(ret = stroll_lvstr_dup(NULL, str2));
	expect_assert_failure(ret = stroll_lvstr_dup(&lvstr, NULL));
#endif
	assert_int_equal(stroll_lvstr_ndup(&lvstr, str2, len2), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2);

	stroll_utest_expect_free_arg(lvstr.rwstr, len2 + 1);
	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_drop_init(void ** state __unused)
{
	const char          * str = "test";
	char                * tmp;
	size_t                len = strlen("test");
	struct stroll_lvstr   lvstr;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_drop(NULL));
#endif

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init_nlend(&lvstr, str, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init_lend(&lvstr, str);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	tmp = strdup(str);
	stroll_lvstr_init_ncede(&lvstr, tmp, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(tmp, len + 1);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	tmp = strdup(str);
	stroll_lvstr_init_cede(&lvstr, tmp);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(tmp, len + 1);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	assert_int_equal(stroll_lvstr_init_ndup(&lvstr, str, len), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(lvstr.rwstr, len + 1);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	assert_int_equal(stroll_lvstr_init_dup(&lvstr, str), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(lvstr.rwstr, len + 1);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_drop(void ** state __unused)
{
	const char          * str = "test";
	char                * tmp;
	size_t                len = strlen("test");
	struct stroll_lvstr   lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_nlend(&lvstr, str, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_lend(&lvstr, str);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	tmp = strdup(str);
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_ncede(&lvstr, tmp, len);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(tmp, len + 1);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	tmp = strdup(str);
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_cede(&lvstr, tmp);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(tmp, len + 1);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	assert_int_equal(stroll_lvstr_ndup(&lvstr, str, len), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(lvstr.rwstr, len + 1);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	assert_int_equal(stroll_lvstr_dup(&lvstr, str), 0);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str);
	assert_int_equal(stroll_lvstr_len(&lvstr), len);
	stroll_utest_expect_free_arg(lvstr.rwstr, len + 1);
	stroll_lvstr_drop(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_cede_release(void ** state __unused)
{
	char                * str1 = strdup("test");
	size_t                len1 = strlen(str1);
	const char          * str2 = "test2";
	size_t                len2 = strlen(str2);
	struct stroll_lvstr   lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_ncede(&lvstr, str1, len1);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);

	stroll_utest_expect_free_arg(str1, len1 + 1);
	stroll_lvstr_lend(&lvstr, str2);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2);
	stroll_lvstr_fini(&lvstr);

	str1 = strdup("test");
	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	stroll_lvstr_cede(&lvstr, str1);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str1);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);

	stroll_utest_expect_free_arg(str1, len1 + 1);
	stroll_lvstr_lend(&lvstr, str2);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2);
	stroll_lvstr_fini(&lvstr);
}

static void
stroll_lvstr_utest_dup_release(void ** state __unused)
{
	const char          * str1 = "test";
	size_t                len1 = strlen(str1);
	const char          * str2 = "test2";
	size_t                len2 = strlen(str2);
	const char          * tmp;
	struct stroll_lvstr   lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	assert_int_equal(stroll_lvstr_ndup(&lvstr, str1, len1), 0);
	tmp = stroll_lvstr_cstr(&lvstr);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), tmp);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);

	stroll_utest_expect_free_arg(tmp, len1 + 1);
	stroll_lvstr_lend(&lvstr, str2);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	assert_null(stroll_lvstr_cstr(&lvstr));
	assert_int_equal(stroll_lvstr_dup(&lvstr, str1), 0);
	tmp = stroll_lvstr_cstr(&lvstr);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), tmp);
	assert_int_equal(stroll_lvstr_len(&lvstr), len1);

	stroll_utest_expect_free_arg(tmp, len1 + 1);
	stroll_lvstr_lend(&lvstr, str2);
	assert_string_equal(stroll_lvstr_cstr(&lvstr), str2);
	assert_int_equal(stroll_lvstr_len(&lvstr), len2);
	stroll_lvstr_fini(&lvstr);
}

static const struct CMUnitTest stroll_lvstr_utests[] = {
	cmocka_unit_test(stroll_lvstr_utest_empty),
	cmocka_unit_test(stroll_lvstr_utest_nlend),
	cmocka_unit_test(stroll_lvstr_utest_lend),
	cmocka_unit_test(stroll_lvstr_utest_ncede),
	cmocka_unit_test(stroll_lvstr_utest_cede),
	cmocka_unit_test(stroll_lvstr_utest_ndup),
	cmocka_unit_test(stroll_lvstr_utest_dup),
	cmocka_unit_test(stroll_lvstr_utest_drop_init),
	cmocka_unit_test(stroll_lvstr_utest_drop),
	cmocka_unit_test(stroll_lvstr_utest_cede_release),
	cmocka_unit_test(stroll_lvstr_utest_dup_release)
};

int
main(void)
{
	return cmocka_run_group_tests_name("Length-Value Strings (lvstr)",
	                                   stroll_lvstr_utests,
	                                   NULL,
	                                   NULL);
}
