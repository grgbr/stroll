/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "stroll/pow2.h"

unsigned int __stroll_const __stroll_nothrow
stroll_pow2_upper32(uint32_t value)
{
	/* Would overflow otherwise... */
	stroll_pow2_assert_api(value);

	if (value > (UINT32_C(1) << 31))
		return 32;

	return stroll_pow2_lower32(value +
	                           (UINT32_C(1) << stroll_pow2_lower32(value)) -
	                           1);
}

unsigned int __stroll_const __stroll_nothrow
stroll_pow2_upper64(uint64_t value)
{
	stroll_pow2_assert_api(value);

	if (value > (UINT64_C(1) << 63))
		return 64;

	return stroll_pow2_lower64(value +
	                           (UINT64_C(1) << stroll_pow2_lower64(value)) -
	                           1);
}
