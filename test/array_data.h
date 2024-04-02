/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _STROLL_TEST_ARRAY_DATA_H
#define _STROLL_TEST_ARRAY_DATA_H

struct strollut_array_elem {
	int                                key;
	const struct strollut_array_elem * ptr;
};

#define STROLLUT_ARRAY_NR (257U)

extern const struct strollut_array_elem
strollut_array_input[STROLLUT_ARRAY_NR];

extern const struct strollut_array_elem
strollut_array_inorder[STROLLUT_ARRAY_NR];

extern const struct strollut_array_elem
strollut_array_postorder[STROLLUT_ARRAY_NR];

#endif /* _STROLL_TEST_ARRAY_DATA_H */
