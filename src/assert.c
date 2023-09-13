/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/assert.h"
#include <stdio.h>
#include <stdlib.h>

void
stroll_assert_fail(const char * __restrict prefix,
                   const char * __restrict expr,
                   const char * __restrict file,
                   unsigned int            line,
                   const char * __restrict func)
{
	fflush(NULL);

	fprintf(stderr,
	        "{assert} %s:%s:%u:%s:\'%s\' failed\n",
	        prefix,
	        file,
	        line,
	        func,
	        expr);

	abort();
}

void
stroll_assert_fail_msg(const char * __restrict prefix,
                       const char * __restrict message)
{
	fflush(NULL);

	fprintf(stderr, "{assert} %s:%s\n", prefix, message);

	abort();
}
