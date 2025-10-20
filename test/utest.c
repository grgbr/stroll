/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "utest.h"
#include <cute/cute.h>
#include <cute/expect.h>
#include <stdio.h>
#include <stdlib.h>

static char strollut_assert_msg[LINE_MAX];

/*
 * Override libstroll's stroll_assert_fail() and use cute_mock_assert() to
 * validate assertions.
 */
void
stroll_assert_fail(const char * __restrict prefix,
                   const char * __restrict expr,
                   const char * __restrict file,
                   unsigned int            line,
                   const char * __restrict func)
{
	int    ret;
	size_t sz = sizeof(strollut_assert_msg) - 1;

	/*
	 * cute_mock_assert() does not really "return". It uses a nonlocal goto
	 * logic to restore program / stack state that existed before the code
	 * under test called us. This is the way CUTe allows checking for
	 * assertions.
	 * This means that the code below will never reach the abort() call
	 * below (which is just there to prevent GCC from warning us since
	 * stroll_assert_fail() is declared as a function that cannot return).
	 *
	 * Since cute_mock_assert() does not give control back to us, we MUST
	 * use a statically allocated buffer to store assertion messages. We
	 * would not have the opportunity to free(3) a previously allocated
	 * buffer otherwise.
	 * In other words, Valgrind memory leak checker should be happy with
	 * this...
	 */
	ret = snprintf(strollut_assert_msg,
	               sz,
	               "{utest assert} %s:%s:%u:%s:\'%s\'",
	               prefix,
	               file,
	               line,
	               func,
	               expr);
	if (ret > 0) {
		if ((size_t)ret >= sz)
			strollut_assert_msg[sz - 1] = '\0';

		cute_mock_assert(strollut_assert_msg, file, line, func);
	}
	else
		cute_mock_assert("{utest assert} ??", file, line, func);

	/* Not reached (see comment above)... */
	abort();
}

static bool strollut_free_wrapped;

/*
 * Mock Glibc's free(3) for verification purposes.
 *
 * Set strollut_free_wrapped to true from client testing code to enable
 * free(3) argument checking logic.
 */
void
free(void * ptr)
{
	if (strollut_free_wrapped) {
		/*
		 * Disable checking logic implicitly. Client testing code will
		 * have to re-enable it by setting strollut_free_wrapped to
		 * true to perform subsequent validation.
		 *
		 * Watch out ! This MUST be done before calling any
		 * cute_mock_...() function is called since they all rely upon a
		 * working free(3). We would otherwise wrap CUTe's internal
		 * calls to free(3) !
		 */
		strollut_free_wrapped = false;
		/*
		 * free(3) argument checking logic is enabled: do the check
		 * using standard CUTe's cute_mock_ptr_parm() /
		 * cute_mock_mem_parm().
		 * First check pointer value, then content of memory pointed to.
		 */
		cute_mock_ptr_parm(ptr);
		cute_mock_mem_parm(ptr);
	}

	/* Now call the original free(3) function. */
#if defined __GLIBC__
	extern void __libc_free(void *);
	__libc_free(ptr);
#else
#error Glibc is the only C library supported for now !
#endif
}

#if defined(CONFIG_STROLL_VALGRIND)
#include <valgrind/valgrind.h>
#endif

void
strollut_expect_free(const void * parm, size_t size)
{
#if defined(CONFIG_STROLL_VALGRIND)
	/*
	 * As Valgrind overrides C library's malloc(3) / realloc(3) / free(3)
	 * functions, it bypasses our own free(3) wrapper implemented above.
	 * This breaks our mocked free(3) testing mechanism and leads to test
	 * failures.
	 * Inhibit our mocked free(3) based tests when running testsuite under
	 * Valgrind. We may still run the entire testsuite without Valgrind
	 * anyway.
	 */
	if (RUNNING_ON_VALGRIND)
		return;
#endif

	/* Request checking of pointer value. */
	cute_expect_ptr_parm(free, ptr, equal, parm);
	/* Request checking of pointed to memory content. */
	cute_expect_mem_parm(free, ptr, equal, parm, size);

	/* Instruct free() function above to perform checking of arguments. */
	strollut_free_wrapped = true;
}

extern CUTE_SUITE_DECL(strollut_cdefs_suite);
#if defined(CONFIG_STROLL_BOPS)
extern CUTE_SUITE_DECL(strollut_bops_suite);
#endif
#if defined(CONFIG_STROLL_BMAP)
extern CUTE_SUITE_DECL(strollut_bmap_suite);
#endif
#if defined(CONFIG_STROLL_FBMAP)
extern CUTE_SUITE_DECL(strollut_fbmap_suite);
#endif
#if defined(CONFIG_STROLL_LVSTR)
extern CUTE_SUITE_DECL(strollut_lvstr_suite);
#endif
#if defined(CONFIG_STROLL_POW2)
extern CUTE_SUITE_DECL(strollut_pow2_suite);
#endif
#if defined(CONFIG_STROLL_ARRAY)
extern CUTE_SUITE_DECL(strollut_array_suite);
#endif
#if defined(CONFIG_STROLL_HEAP)
extern CUTE_SUITE_DECL(strollut_heap_suite);
#endif
#if defined(CONFIG_STROLL_SLIST)
extern CUTE_SUITE_DECL(strollut_slist_suite);
#endif
#if defined(CONFIG_STROLL_DLIST)
extern CUTE_SUITE_DECL(strollut_dlist_suite);
#endif
#if defined(CONFIG_STROLL_SPRHEAP) || \
    defined(CONFIG_STROLL_DPRHEAP) || \
    defined(CONFIG_STROLL_BNHEAP)
extern CUTE_SUITE_DECL(strollut_theap_suite);
#endif
#if defined(CONFIG_STROLL_MSG)
extern CUTE_SUITE_DECL(strollut_message_suite);
#endif

CUTE_GROUP(strollut_group) = {
	CUTE_REF(strollut_cdefs_suite),
#if defined(CONFIG_STROLL_BOPS)
	CUTE_REF(strollut_bops_suite),
#endif
#if defined(CONFIG_STROLL_BMAP)
	CUTE_REF(strollut_bmap_suite),
#endif
#if defined(CONFIG_STROLL_FBMAP)
	CUTE_REF(strollut_fbmap_suite),
#endif
#if defined(CONFIG_STROLL_LVSTR)
	CUTE_REF(strollut_lvstr_suite),
#endif
#if defined(CONFIG_STROLL_ARRAY)
	CUTE_REF(strollut_array_suite),
#endif
#if defined(CONFIG_STROLL_POW2)
	CUTE_REF(strollut_pow2_suite),
#endif
#if defined(CONFIG_STROLL_HEAP)
	CUTE_REF(strollut_heap_suite),
#endif
#if defined(CONFIG_STROLL_SLIST)
	CUTE_REF(strollut_slist_suite),
#endif
#if defined(CONFIG_STROLL_DLIST)
	CUTE_REF(strollut_dlist_suite),
#endif
#if defined(CONFIG_STROLL_SPRHEAP) || \
    defined(CONFIG_STROLL_DPRHEAP) || \
    defined(CONFIG_STROLL_BNHEAP)
	CUTE_REF(strollut_theap_suite),
#endif
#if defined(CONFIG_STROLL_MSG)
	CUTE_REF(strollut_message_suite),
#endif
};

CUTE_SUITE(strollut_suite, strollut_group);

CUTE_MAIN(strollut_suite, "Stroll", STROLL_VERSION_STRING)
