/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2025 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "alloc.h"
#include <stdlib.h>

void
stroll_alloc_destroy(struct stroll_alloc * __restrict allocator)
{
	stroll_alloc_assert_api(allocator);
	stroll_alloc_assert_ops_api(allocator->ops);

	allocator->ops->fini(allocator);

	free(allocator);
}
