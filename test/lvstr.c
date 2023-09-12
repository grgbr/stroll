#include "utest.h"
#include "stroll/lvstr.h"
#include <cute/cute.h>
#include <cute/check.h>
#include <cute/expect.h>

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_lvstr_empty_assert)
{
	const char * str __unused;
	size_t       len __unused;

	cute_expect_assertion(stroll_lvstr_init(NULL));
	cute_expect_assertion(str = stroll_lvstr_cstr(NULL));
	cute_expect_assertion(len = stroll_lvstr_len(NULL));
	cute_expect_assertion(stroll_lvstr_fini(NULL));
}
#else
CUTE_TEST(strollut_lvstr_empty_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_lvstr_empty)
{
	struct stroll_lvstr lvstr = STROLL_LVSTR_INIT;

	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_lvstr_nlend_assert)
{
	const char *        str = "test";
	size_t              len = strlen(str);
	struct stroll_lvstr lvstr;

	cute_expect_assertion(stroll_lvstr_init_nlend(NULL, str, len));
	cute_expect_assertion(stroll_lvstr_init_nlend(&lvstr, NULL, len));
	cute_expect_assertion(stroll_lvstr_init_nlend(&lvstr, str, len - 1));

	cute_expect_assertion(stroll_lvstr_nlend(NULL, str, len));
	cute_expect_assertion(stroll_lvstr_nlend(&lvstr, NULL, len));
	cute_expect_assertion(stroll_lvstr_nlend(&lvstr, str, len - 1));
}
#else
CUTE_TEST(strollut_lvstr_nlend_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_lvstr_nlend)
{
	const char          * str = "test";
	const char          * str2 = "test2";
	size_t                len = strlen(str);
	struct stroll_lvstr   lvstr = STROLL_LVSTR_INIT_NLEND(str, len);

	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init_nlend(&lvstr, str, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init_nlend(&lvstr, str, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);

	len = strlen(str2);
	stroll_lvstr_nlend(&lvstr, str2, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);

	stroll_lvstr_fini(&lvstr);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_lvstr_lend_assert)
{
	const char          * str = "test";
	struct stroll_lvstr   lvstr;

	cute_expect_assertion(stroll_lvstr_init_lend(NULL, str));
	cute_expect_assertion(stroll_lvstr_init_lend(&lvstr, NULL));

	cute_expect_assertion(stroll_lvstr_lend(NULL, str));
	cute_expect_assertion(stroll_lvstr_lend(&lvstr, NULL));
}
#else
CUTE_TEST(strollut_lvstr_lend_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_lvstr_lend)
{

	struct stroll_lvstr   lvstr = STROLL_LVSTR_INIT_LEND("test");
	const char          * str = "test";
	const char          * str2 = "test2";
	size_t                len = strlen(str);

	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init_lend(&lvstr, str);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rostr = (void *)0xdeadbeef;

	stroll_lvstr_init_lend(&lvstr, str);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);

	len = strlen(str2);
	stroll_lvstr_lend(&lvstr, str2);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);

	stroll_lvstr_fini(&lvstr);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_lvstr_ncede_assert)
{
	char *              str = "test";
	size_t              len = strlen(str);
	struct stroll_lvstr lvstr;

	cute_expect_assertion(stroll_lvstr_init_ncede(NULL, str, len));
	cute_expect_assertion(stroll_lvstr_init_ncede(&lvstr, NULL, len));
	cute_expect_assertion(stroll_lvstr_init_ncede(&lvstr, str, len - 1));

	cute_expect_assertion(stroll_lvstr_ncede(NULL, str, len));
	cute_expect_assertion(stroll_lvstr_ncede(&lvstr, NULL, len));
	cute_expect_assertion(stroll_lvstr_ncede(&lvstr, str, len - 1));
}
#else
CUTE_TEST(strollut_lvstr_ncede_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_lvstr_ncede)
{
	char * volatile       str = strdup("test");
	char                * str2 = strdup("test2");
	volatile size_t       len = strlen(str);
	struct stroll_lvstr   lvstr = STROLL_LVSTR_INIT_NCEDE(str, len);

	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(str, len + 1);
	stroll_lvstr_fini(&lvstr);

	str = strdup("test");
	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_ncede(&lvstr, str, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(str, len + 1);
	stroll_lvstr_fini(&lvstr);

	str = strdup("test");
	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_ncede(&lvstr, str, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);

	len = strlen(str2);
	stroll_lvstr_ncede(&lvstr, str2, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);

	strollut_expect_free(str2, len + 1);
	stroll_lvstr_fini(&lvstr);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_lvstr_cede_assert)
{
	char * volatile     str1 = strdup("test");
	struct stroll_lvstr lvstr;

	cute_expect_assertion(stroll_lvstr_init_cede(NULL, str1));
	cute_expect_assertion(stroll_lvstr_init_cede(&lvstr, NULL));
	cute_expect_assertion(stroll_lvstr_init_cede(NULL, NULL));

	cute_expect_assertion(stroll_lvstr_cede(NULL, str1));
	cute_expect_assertion(stroll_lvstr_cede(&lvstr, NULL));
	cute_expect_assertion(stroll_lvstr_cede(NULL, NULL));
}
#else
CUTE_TEST(strollut_lvstr_cede_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_lvstr_cede)
{
	char * volatile     str1 = strdup("test");
	size_t volatile     len1 = strlen(str1);
	char * volatile     str2 = strdup("test2");
	size_t              len2 = strlen(str2);
	struct stroll_lvstr lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_cede(&lvstr, str1);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);
	strollut_expect_free(str1, len1 + 1);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_cede(&lvstr, str2);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2);
	strollut_expect_free(str2, len2 + 1);
	stroll_lvstr_fini(&lvstr);

	str1 = strdup("test");
	str2 = strdup("test2");
	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	stroll_lvstr_init_cede(&lvstr, str1);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);

	strollut_expect_free(str1, len1 + 1);
	stroll_lvstr_cede(&lvstr, str2);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2);

	strollut_expect_free(str2, len2 + 1);
	stroll_lvstr_fini(&lvstr);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_lvstr_ndup_assert)
{
	const char *        str1 = "test";
	size_t              len1 = strlen(str1);
	const char *        str2 = "test2";
	size_t              len2 = strlen(str2);
	struct stroll_lvstr lvstr;
	int                 ret __unused;

	cute_expect_assertion(ret = stroll_lvstr_init_ndup(NULL, str1, len1));
	cute_expect_assertion(ret = stroll_lvstr_init_ndup(&lvstr, NULL, len1));

	cute_expect_assertion(ret = stroll_lvstr_ndup(NULL, str2, len2));
	cute_expect_assertion(ret = stroll_lvstr_ndup(&lvstr, NULL, len2));
}
#else
CUTE_TEST(strollut_lvstr_ndup_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_lvstr_ndup)
{
	const char          * str1 = "test";
	size_t                len1 = strlen(str1);
	const char          * str2 = "test2";
	size_t                len2 = strlen(str2);
	struct stroll_lvstr   lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	cute_check_sint(stroll_lvstr_init_ndup(&lvstr, str1, len1), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);
	strollut_expect_free(lvstr.rwstr, len1 + 1);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	cute_check_sint(stroll_lvstr_init_ndup(&lvstr, str2, len2 - 1),
	                equal,
	                0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2 - 1);
	strollut_expect_free(lvstr.rwstr, len2 - 1);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	cute_check_sint(stroll_lvstr_init_ndup(&lvstr, str1, len1), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);

	cute_check_sint(stroll_lvstr_ndup(&lvstr, str2, len2), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2);

	strollut_expect_free(lvstr.rwstr, len2 + 1);
	stroll_lvstr_fini(&lvstr);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_lvstr_dup_assert)
{
	const char          * str1 = "test";
	const char          * str2 = "test2";
	struct stroll_lvstr   lvstr;
	int                   ret __unused;

	cute_expect_assertion(ret = stroll_lvstr_init_dup(NULL, str1));
	cute_expect_assertion(ret = stroll_lvstr_init_dup(&lvstr, NULL));

	cute_expect_assertion(ret = stroll_lvstr_dup(NULL, str2));
	cute_expect_assertion(ret = stroll_lvstr_dup(&lvstr, NULL));
}
#else
CUTE_TEST(strollut_lvstr_dup_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_lvstr_dup)
{
	const char          * str1 = "test";
	size_t                len1 = strlen(str1);
	const char          * str2 = "test2";
	size_t                len2 = strlen(str2);
	struct stroll_lvstr   lvstr;
	int                   ret __unused;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	cute_check_sint(stroll_lvstr_init_dup(&lvstr, str1), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);
	strollut_expect_free(lvstr.rwstr, len1 + 1);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;

	cute_check_sint(stroll_lvstr_init_dup(&lvstr, str1), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);

	cute_check_sint(stroll_lvstr_ndup(&lvstr, str2, len2), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2);

	strollut_expect_free(lvstr.rwstr, len2 + 1);
	stroll_lvstr_fini(&lvstr);
}

#if defined(CONFIG_STROLL_ASSERT_API)
CUTE_TEST(strollut_lvstr_drop_init_assert)
{
	cute_expect_assertion(stroll_lvstr_drop(NULL));
}
#else
CUTE_TEST(strollut_lvstr_drop_init_assert)
{
	cute_skip("assertion unsupported");
}
#endif

CUTE_TEST(strollut_lvstr_drop_init)
{
	const char          * str = "test";
	char                * tmp;
	size_t                len = strlen("test");
	struct stroll_lvstr   lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init_nlend(&lvstr, str, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init_lend(&lvstr, str);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	tmp = strdup(str);
	stroll_lvstr_init_ncede(&lvstr, tmp, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(tmp, len + 1);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	tmp = strdup(str);
	stroll_lvstr_init_cede(&lvstr, tmp);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(tmp, len + 1);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	cute_check_sint(stroll_lvstr_init_ndup(&lvstr, str, len), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(lvstr.rwstr, len + 1);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	cute_check_sint(stroll_lvstr_init_dup(&lvstr, str), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(lvstr.rwstr, len + 1);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);
}

CUTE_TEST(strollut_lvstr_drop)
{
	const char          * str = "test";
	char                * tmp;
	size_t                len = strlen("test");
	struct stroll_lvstr   lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_nlend(&lvstr, str, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_lend(&lvstr, str);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	tmp = strdup(str);
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_ncede(&lvstr, tmp, len);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(tmp, len + 1);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	tmp = strdup(str);
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_cede(&lvstr, tmp);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(tmp, len + 1);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	cute_check_sint(stroll_lvstr_ndup(&lvstr, str, len), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(lvstr.rwstr, len + 1);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	cute_check_sint(stroll_lvstr_dup(&lvstr, str), equal, 0);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len);
	strollut_expect_free(lvstr.rwstr, len + 1);
	stroll_lvstr_drop(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_fini(&lvstr);
}

CUTE_TEST(strollut_lvstr_cede_release)
{
	char                * str1 = strdup("test");
	size_t                len1 = strlen(str1);
	const char          * str2 = "test2";
	size_t                len2 = strlen(str2);
	struct stroll_lvstr   lvstr;

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_ncede(&lvstr, str1, len1);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);

	strollut_expect_free(str1, len1 + 1);
	stroll_lvstr_lend(&lvstr, str2);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2);
	stroll_lvstr_fini(&lvstr);

	str1 = strdup("test");
	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	stroll_lvstr_cede(&lvstr, str1);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str1);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);

	strollut_expect_free(str1, len1 + 1);
	stroll_lvstr_lend(&lvstr, str2);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2);
	stroll_lvstr_fini(&lvstr);
}

CUTE_TEST(strollut_lvstr_dup_release)
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
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	cute_check_sint(stroll_lvstr_ndup(&lvstr, str1, len1), equal, 0);
	tmp = stroll_lvstr_cstr(&lvstr);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, tmp);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);

	strollut_expect_free(tmp, len1 + 1);
	stroll_lvstr_lend(&lvstr, str2);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2);
	stroll_lvstr_fini(&lvstr);

	lvstr.len = 0xdeadbeef;
	lvstr.rwstr = (void *)0xdeadbeef;
	stroll_lvstr_init(&lvstr);
	cute_check_ptr(stroll_lvstr_cstr(&lvstr), equal, NULL);
	cute_check_sint(stroll_lvstr_dup(&lvstr, str1), equal, 0);
	tmp = stroll_lvstr_cstr(&lvstr);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, tmp);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len1);

	strollut_expect_free(tmp, len1 + 1);
	stroll_lvstr_lend(&lvstr, str2);
	cute_check_str(stroll_lvstr_cstr(&lvstr), equal, str2);
	cute_check_uint(stroll_lvstr_len(&lvstr), equal, len2);
	stroll_lvstr_fini(&lvstr);
}

/******************************************************************************
 * Top-level bitmap support
 ******************************************************************************/

CUTE_GROUP(strollut_lvstr_group) = {
	CUTE_REF(strollut_lvstr_empty_assert),
	CUTE_REF(strollut_lvstr_empty),
	CUTE_REF(strollut_lvstr_nlend_assert),
	CUTE_REF(strollut_lvstr_nlend),
	CUTE_REF(strollut_lvstr_lend_assert),
	CUTE_REF(strollut_lvstr_lend),
	CUTE_REF(strollut_lvstr_ncede_assert),
	CUTE_REF(strollut_lvstr_ncede),
	CUTE_REF(strollut_lvstr_cede_assert),
	CUTE_REF(strollut_lvstr_cede),
	CUTE_REF(strollut_lvstr_ndup_assert),
	CUTE_REF(strollut_lvstr_ndup),
	CUTE_REF(strollut_lvstr_dup_assert),
	CUTE_REF(strollut_lvstr_dup),
	CUTE_REF(strollut_lvstr_drop_init_assert),
	CUTE_REF(strollut_lvstr_drop_init),
	CUTE_REF(strollut_lvstr_drop),
	CUTE_REF(strollut_lvstr_cede_release),
	CUTE_REF(strollut_lvstr_dup_release)
};

CUTE_SUITE_EXTERN(strollut_lvstr_suite,
                  strollut_lvstr_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
