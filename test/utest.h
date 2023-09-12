#ifndef _STROLL_UTEST_H
#define _STROLL_UTEST_H

#include "stroll/assert.h"
#include <sys/types.h>

extern void free(void * ptr);
extern void strollut_expect_free(const void * parm, size_t size);

#endif /* _STROLL_UTEST_H */
