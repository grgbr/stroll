/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/cdefs.h"
#include <cute/cute.h>
#include <cute/check.h>

CUTE_TEST(strollut_cdefs_min_of_literals)
{
	cute_check_sint(stroll_min( 0,  0), equal,  0);
	cute_check_sint(stroll_min( 0,  1), equal,  0);
	cute_check_sint(stroll_min(-1,  1), equal, -1);
	cute_check_sint(stroll_min(-2, -1), equal, -2);
}

static void
strollut_cdefs_check_min(int first, int second, int expected)
{
	cute_check_sint(stroll_min(first,  second), equal, expected);
}

CUTE_TEST(strollut_cdefs_min_of_variables)
{
	strollut_cdefs_check_min( 0,  0,  0);
	strollut_cdefs_check_min( 0,  1,  0);
	strollut_cdefs_check_min(-1,  1, -1);
	strollut_cdefs_check_min(-2, -1, -2);
}

CUTE_TEST(strollut_cdefs_multi_min)
{
	int a = -2, b = 1, c = 0;

	cute_check_sint(stroll_min(stroll_min(0, 1), -1), equal, -1);
	cute_check_sint(stroll_min(stroll_min(c, a),  b), equal, -2);
}

CUTE_TEST(strollut_cdefs_max_of_literals)
{
	cute_check_sint(stroll_max( 0,  0), equal,  0);
	cute_check_sint(stroll_max( 0,  2), equal,  2);
	cute_check_sint(stroll_max(-1,  1), equal,  1);
	cute_check_sint(stroll_max(-2, -1), equal, -1);
}

static void
strollut_cdefs_check_max(int first, int second, int expected)
{
	cute_check_sint(stroll_max(first,  second), equal, expected);
}

CUTE_TEST(strollut_cdefs_max_of_variables)
{
	strollut_cdefs_check_max( 0,  0,  0);
	strollut_cdefs_check_max( 0,  2,  2);
	strollut_cdefs_check_max(-1,  1,  1);
	strollut_cdefs_check_max(-2, -1, -1);
}

CUTE_TEST(strollut_cdefs_multi_max)
{
	int a = -2, b = 1, c = 0;

	cute_check_sint(stroll_max(stroll_max(0, 1), -1), equal, 1);
	cute_check_sint(stroll_max(stroll_max(c, a),  b), equal, 1);
}

CUTE_TEST(strollut_cdefs_abs_of_literals)
{
	cute_check_sint(stroll_abs( 0), equal, 0);
	cute_check_sint(stroll_abs( 1), equal, 1);
	cute_check_sint(stroll_abs(-2), equal, 2);
}

static void
strollut_cdefs_check_abs(int value, int expected)
{
	cute_check_sint(stroll_abs(value), equal, expected);
}

CUTE_TEST(strollut_cdefs_abs_of_variables)
{
	strollut_cdefs_check_abs( 0, 0);
	strollut_cdefs_check_abs( 1, 1);
	strollut_cdefs_check_abs(-2, 2);
}

CUTE_TEST(strollut_cdefs_mixed_ops_of_literals)
{
	cute_check_sint(stroll_min(stroll_abs(-1), 2), equal, 1);
	cute_check_sint(stroll_max(stroll_abs(-1), 0), equal, 1);

	cute_check_sint(stroll_abs(stroll_min(-1,  0)), equal, 1);
	cute_check_sint(stroll_abs(stroll_max(-2, -1)), equal, 1);
}

CUTE_TEST(strollut_cdefs_mixed_ops_of_variables)
{
	int a = -1, b = 0, c = 2;

	cute_check_sint(stroll_min(stroll_abs(a), c), equal, 1);
	cute_check_sint(stroll_max(stroll_abs(a), b), equal, 1);

	cute_check_sint(stroll_abs(stroll_min(a,  b)), equal, 1);
	cute_check_sint(stroll_abs(stroll_max(-c, a)), equal, 1);
}

CUTE_GROUP(strollut_cdefs_group) = {
	CUTE_REF(strollut_cdefs_min_of_literals),
	CUTE_REF(strollut_cdefs_min_of_variables),
	CUTE_REF(strollut_cdefs_multi_min),
	CUTE_REF(strollut_cdefs_max_of_literals),
	CUTE_REF(strollut_cdefs_max_of_variables),
	CUTE_REF(strollut_cdefs_multi_max),
	CUTE_REF(strollut_cdefs_abs_of_literals),
	CUTE_REF(strollut_cdefs_abs_of_variables),
	CUTE_REF(strollut_cdefs_mixed_ops_of_literals),
	CUTE_REF(strollut_cdefs_mixed_ops_of_variables)
};

CUTE_SUITE_EXTERN(strollut_cdefs_suite,
                  strollut_cdefs_group,
                  CUTE_NULL_SETUP,
                  CUTE_NULL_TEARDOWN,
                  CUTE_DFLT_TMOUT);
