#include "utest.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Override libstroll's stroll_asser_fail() and use mock_assert() to validate
 * assertions.
 */
void
stroll_assert_fail(const char * __restrict prefix,
                   const char * __restrict expr,
                   const char * __restrict file,
                   unsigned int            line,
                   const char * __restrict func __unused)
{
	char * msg;
	int    ret;

	ret = asprintf(&msg,
	               "{utest assert} %s:%s:%u:%s:\'%s\'",
	               prefix,
	               file,
	               line,
	               func,
	               expr);
	mock_assert(0, (ret > 0) ? msg : "{utest assert} ??", file, (int)line);

	abort();
}
