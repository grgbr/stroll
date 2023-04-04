/*
 * Redefine stroll_assert() for easy cmocka unit testing support.
 */
#ifndef _STROLL_UTEST_H
#define _STROLL_UTEST_H

#if defined(_STROLL_ASSERT_H)
#error "stroll/assert.h" already included: include "utest_assert.h" first !
#endif

#include "stroll/assert.h"

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#undef stroll_assert

extern void
mock_assert(const int          result,
            const char * const expression,
            const char * const file,
            const int          line);

#define stroll_assert(_prefix, _expr) \
	 mock_assert(!!(_expr), \
	             "{utest assert}" _prefix ": " __STRING(_expr), \
	             __FILE__, \
	             __LINE__)

#endif /* _STROLL_UTEST_H */
