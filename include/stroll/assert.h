/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of Stroll.
 * Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Assertion interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      29 Aug 2017
 * @copyright Copyright (C) 2017-2023 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _STROLL_ASSERT_H
#define _STROLL_ASSERT_H

#include <stroll/cdefs.h>

/**
 * @internal
 *
 * Output a failed assertion message and abort.
 *
 * @param[in] prefix message prefix
 * @param[in] expr   expression that failed
 * @param[in] file   filename containing expr definition
 * @param[in] line   line number where expr is defined
 * @param[in] func   function that uses expr
 */
extern void
stroll_assert_fail(const char * __restrict prefix,
                   const char * __restrict expr,
                   const char * __restrict file,
                   unsigned int            line,
                   const char * __restrict func) __nonull(1, 2, 3, 5)
                                                 __noreturn
						 __leaf;

/**
 * @internal
 *
 * Output a failed assertion message and abort.
 *
 * @param[in] prefix  message prefix
 * @param[in] message assertion message to output
 */
extern void
stroll_assert_fail_msg(const char * __restrict prefix,
                       const char * __restrict message) __nonull(1, 2)
                                                        __noreturn
						        __leaf;

/**
 * Check an assertion.
 *
 * @param[in] _prefix prefix prepended to message generated when assertion fails
 * @param[in] _expr   expression to check assertion for
 *
 * @hideinitializer
 */
#define stroll_assert(_prefix, _expr) \
	((_expr) ? \
	 (void)(0) : \
	 stroll_assert_fail(_prefix, \
	                    __STRING(_expr), \
	                    __FILE__, \
	                    __LINE__, \
	                    __FUNCTION__))

#endif /* _STROLL_ASSERT_H */
