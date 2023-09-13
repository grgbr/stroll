/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _STROLL_UTEST_H
#define _STROLL_UTEST_H

#include "stroll/assert.h"
#include <sys/types.h>

extern void free(void * ptr);
extern void strollut_expect_free(const void * parm, size_t size);

#endif /* _STROLL_UTEST_H */
