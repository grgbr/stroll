#include "utest.h"
#include "stroll/lvstr.h"

static void
stroll_lvstr_utest_empty(void ** state __unused)
{
	struct stroll_lvstr lvstr = STROLL_LVSTR_INIT;

	assert_null(stroll_lvstr_cstr(&lvstr));

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_lvstr_len(&lvstr));
#endif
}

#if 0
static void
stroll_bops32_utest_fls(void ** state __unused)
{
	uint32_t     word;
	unsigned int bit;

#if defined(CONFIG_STROLL_ASSERT_API)
	expect_assert_failure(stroll_bops32_fls(0U));
#endif

	for (bit = 0; bit < 32; bit++) {
		word = (uint32_t)1 << bit;
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
		word = (uint64_t)1 << bit;
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
		word = (uint32_t)1 << bit;
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
		word = (uint64_t)1 << bit;
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
		word = ~((uint32_t)1 << bit);
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
		word = ~((uint64_t)1 << bit);
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
		word = (uint32_t)1 << bit;
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
		word = (uint64_t)1 << bit;
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
#endif

static const struct CMUnitTest stroll_lvstr_utests[] = {
	cmocka_unit_test(stroll_lvstr_utest_empty),
#if 0
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
#endif
};

int
main(void)
{
	return cmocka_run_group_tests_name("Length-Value Strings (lvstr)",
	                                   stroll_lvstr_utests,
	                                   NULL,
	                                   NULL);
}
