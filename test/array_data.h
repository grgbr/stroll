/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _STROLL_TEST_ARRAY_DATA_H
#define _STROLL_TEST_ARRAY_DATA_H

struct strollut_array_num {
	int                               key;
	const struct strollut_array_num * ptr;
};

#define STROLLUT_ARRAY_NUM_NR (257U)

extern const struct strollut_array_num
strollut_array_num_input[STROLLUT_ARRAY_NUM_NR];

extern const struct strollut_array_num
strollut_array_num_inorder[STROLLUT_ARRAY_NUM_NR];

extern const struct strollut_array_num
strollut_array_num_postorder[STROLLUT_ARRAY_NUM_NR];

struct strollut_array_str {
	const char *                      word;
	const struct strollut_array_str * ptr;
};

#define STROLLUT_ARRAY_STR_NR (127U)

extern const struct strollut_array_str
strollut_array_str_input[STROLLUT_ARRAY_STR_NR];

extern const struct strollut_array_str
strollut_array_str_inorder[STROLLUT_ARRAY_STR_NR];

extern const struct strollut_array_str
strollut_array_str_postorder[STROLLUT_ARRAY_STR_NR];

#endif /* _STROLL_TEST_ARRAY_DATA_H */
