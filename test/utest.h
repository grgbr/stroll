#ifndef _STROLL_UTEST_H
#define _STROLL_UTEST_H

#include "stroll/assert.h"

#if 0
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>

extern bool stroll_utest_free_wrapped;
extern void free(void * ptr);
extern void stroll_utest_expect_free_arg(const void * data, size_t size);
#endif

#endif /* _STROLL_UTEST_H */
