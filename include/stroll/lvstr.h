/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Length-Value String interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      02 Apr 2023
 * @copyright Copyright (C) 2017-2023 Grégor Boirie.
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */
#ifndef _STROLL_LVSTR_H
#define _STROLL_LVSTR_H

#include <stroll/cdefs.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_lvstr_assert_api(_expr) \
	stroll_assert("stroll:lvstr", _expr)

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_lvstr_assert_api(_expr)

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_lvstr_assert_api_ncstr(_lvstr, _cstr, _len) \
	stroll_lvstr_assert_api(_lvstr); \
	stroll_lvstr_assert_api(_cstr); \
	stroll_lvstr_assert_api((_len) <= STROLL_LVSTR_LEN_MAX); \
	stroll_lvstr_assert_api(strnlen(_cstr, (_len) + 1) == (_len))

#define STROLL_LVSTR_LEASER (0U)
#define STROLL_LVSTR_OWNER  (1U)

/**
 * Maximum length of a C string that may be registered into a stroll_lvstr.
 */
#define STROLL_LVSTR_LEN_MAX \
	STROLL_CONST_MIN(SIZE_MAX >> 1, \
	                 STROLL_CONST_MIN(SSIZE_MAX, PTRDIFF_MAX - 1))

/**
 * Length-Value string.
 *
 * Holds data to ease the management of C strings life-cycle.
 *
 * In addition, it caches the length of string registered into it to mitigate
 * client code string length computation overhead.
 */
struct stroll_lvstr {
	/**
	 * @internal
	 *
	 * Least significant bit holds ownership state, i.e, when `1`,
	 * stroll_lvstr owns registered stroll_lvstr::cstr string.
	 *
	 * Remaining most significant bits hold the length of stroll_lvstr::cstr
	 * excluding terminating NULL byte such that length is
	 * `stroll_lvstr::len >> 1`.
	 */
	size_t   len;
	/**
	 * @internal
	 *
	 * Registered NULL terminated C string.
	 */
	union {
		char *       rwstr;
		const char * rostr;
	};
};

/**
 * Static initialize an empty stroll_lvstr.
 *
 * Behavior is similar to stroll_lvstr_init() but for static
 * initialization purpose.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 */
#define STROLL_LVSTR_INIT \
	{ .rostr = NULL }

/**
 * Static initialize a non-owning stroll_lvstr with a string which length is
 * known.
 *
 * @param[in] _cstr  C string to register
 * @param[in] _len   length of @p _cstr
 *
 * Behavior is similar to stroll_lvstr_init_nlend() but for static
 * initialization purpose.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 */
#define STROLL_LVSTR_INIT_NLEND(_cstr, _len) \
	{ .len = ((_len) << 1) | STROLL_LVSTR_LEASER, .rostr = _cstr }

/**
 * Static initialize a non-owning stroll_lvstr with a string which length is
 * not known.
 *
 * @param[in] _cstr A C string **literal** to register
 *
 * Behavior is similar to stroll_lvstr_init_lend() but for static initialization
 * purpose.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 *
 * @warning
 * Will generate an error at compile time if @p _cstr is not a constant string
 * literal.
 */
#define STROLL_LVSTR_INIT_LEND(_cstr) \
	STROLL_LVSTR_INIT_NLEND(_cstr, \
	                        compile_eval(_is_const(_cstr), \
	                                     sizeof(_cstr)  - 1, \
	                                     "string literal expected"))

/**
 * Static initialize an owning stroll_lvstr with a string which length is
 * known.
 *
 * @param[in] _cstr  C string to register
 * @param[in] _len   length of @p _cstr
 *
 * Behavior is similar to stroll_lvstr_init_ncede() but for static
 * initialization purpose. @p _cstr *MUST NOT* be a constant string since it
 * will be @man{free(3)}'ed at release time.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 *
 * @warning
 * Will generate an error at compile time if @p _cstr is a constant string
 * literal.
 */
#define STROLL_LVSTR_INIT_NCEDE(_cstr, _len) \
	{ \
		.len = ((_len) << 1) | STROLL_LVSTR_OWNER, \
		.rwstr = compile_eval(!_is_const(_cstr), \
		                      _cstr, \
		                      "string literal unexpected") \
	}

/**
 * Return registered C string.
 *
 * @param[in] lvstr Length-Value String
 *
 * @return Registered NULL terminated string or NULL if no string is registered.
 */
static inline
const char * __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
stroll_lvstr_cstr(const struct stroll_lvstr * __restrict lvstr)
{
	stroll_lvstr_assert_api(lvstr);

	return lvstr->rostr;
}

/**
 * Return length of registered C string.
 *
 * @param[in] lvstr Length-Value String
 *
 * @return Length of registered string excluding the terminating NULL byte
 *
 * @warning
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 * there is no registered C string, result is undefined. An assertion is
 * triggered otherwise.
 *
 * @see
 * stroll_lvstr_cstr()
 */
static inline
size_t __stroll_nonull(1) __stroll_pure __stroll_nothrow __warn_result
stroll_lvstr_len(const struct stroll_lvstr * __restrict lvstr)
{
	stroll_lvstr_assert_api(lvstr);
	stroll_lvstr_assert_api(lvstr->rostr);

	return lvstr->len >> 1;
}

/**
 * Register a C string which length is known.
 *
 * @param[inout] lvstr Length-Value String
 * @param[in]    cstr  C string to register
 * @param[in]    len   length of @p cstr
 *
 * Register a NULL terminated string but do not make @p lvstr the owner of it.
 *
 * @p cstr *MUST* be a valid C string, i.e. a NULL byte terminated string which
 * length @p len is smaller than or equal to #STROLL_LVSTR_LEN_MAX.
 *
 * @p lvstr is instructed to avoid freeing @p cstr at release time, i.e., when
 * the registered string is replaced or when stroll_lvstr_fini() is called. Use
 * this when @p lvstr *should not* handle @p cstr life-cycle at all.
 *
 * A previously registered C string is released before registration of @p cstr.
 * Release operation may involve calling @man{free(3)} on the previously
 * registered string if @p lvstr was made the owner of it.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p cstr is invalid, result is undefined. An assertion is triggered
 *   otherwise.
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p len is greater than #STROLL_LVSTR_LEN_MAX, result is undefined. An
 *   assertion is triggered otherwise.
 *
 * @see
 * - STROLL_LVSTR_INIT_NLEND
 * - stroll_lvstr_init_nlend()
 * - stroll_lvstr_lend()
 */
extern void
stroll_lvstr_nlend(struct stroll_lvstr * __restrict lvstr,
                   const char *                     cstr,
                   size_t                           len)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf;

/**
 * Register a C string which length is not known.
 *
 * @param[inout] lvstr Length-Value String
 * @param[in]    cstr  C string to register
 *
 * @return an errno-like error code
 * @retval 0      success
 * @retval -E2BIG @p cstr is invalid
 *
 * Register a NULL terminated string but do not make @p lvstr the owner of it.
 *
 * @p cstr *MUST* be a valid C string, i.e., a NULL byte terminated string which
 * length is smaller than or equal to #STROLL_LVSTR_LEN_MAX.
 *
 * @p lvstr is instructed to avoid freeing @p cstr at release time, i.e., when
 * the registered string is replaced or when stroll_lvstr_fini() is called. Use
 * this when @p lvstr *should not* handle @p cstr life-cycle at all.
 *
 * A previously registered C string is released before registration of @p cstr.
 * Release operation may involve calling free() on the previously registered
 * string if @p lvstr was made the owner of it.
 *
 * @see
 * - stroll_lvstr_nlend()
 * - stroll_lvstr_init_lend()
 */
extern int
stroll_lvstr_lend(struct stroll_lvstr * __restrict lvstr, const char * cstr)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf;

/**
 * Register a C string which length is known and grant lvstr ownership of it.
 *
 * @param[inout] lvstr Length-Value String
 * @param[in]    cstr  C string to register
 * @param[in]    len   length of @p cstr
 *
 * Register a NULL terminated string and make @p lvstr the owner of it.
 *
 * @p cstr *MUST* be a valid C string, i.e., a NULL byte terminated string which
 * length @p len is smaller than or equal to #STROLL_LVSTR_LEN_MAX.
 *
 * @p lvstr is allowed to free @p cstr at release time, i.e., whenever
 * the registered string is replaced or when stroll_lvstr_fini() is called. Use
 * this when @p lvstr *should* handle @p cstr life-cycle.
 *
 * A previously registered C string is released before registration of @p cstr.
 * Release operation may involve calling free() on the previously registered
 * string if @p lvstr was made the owner of it.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p cstr is invalid, result is undefined. An assertion is triggered
 *   otherwise.
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p len is greater than #STROLL_LVSTR_LEN_MAX, result is undefined. An
 *   assertion is triggered otherwise.
 *
 * @see
 * - STROLL_LVSTR_INIT_NCEDE
 * - stroll_lvstr_init_ncede()
 * - stroll_lvstr_cede()
 */
extern void
stroll_lvstr_ncede(struct stroll_lvstr * __restrict lvstr,
                   char *                           cstr,
                   size_t                           len)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf;

/**
 * Register a C string which length is not known and grant lvstr ownership of
 * it.
 *
 * @param[inout] lvstr Length-Value String
 * @param[in]    cstr  C string to register
 *
 * @return an errno-like error code
 * @retval 0      success
 * @retval -E2BIG @p cstr is invalid
 *
 * Register a NULL terminated string and make @p lvstr the owner of it.
 *
 * @p cstr *MUST* be a valid C string, i.e., a NULL byte terminated string which
 * length is smaller than or equal to #STROLL_LVSTR_LEN_MAX.
 *
 * @p lvstr is allowed to free @p cstr at release time, i.e., whenever
 * the registered string is replaced or when stroll_lvstr_fini() is called. Use
 * this when @p lvstr *should* handle @p cstr life-cycle.
 *
 * A previously registered C string is released before registration of @p cstr.
 * Release operation may involve calling free() on the previously registered
 * string if @p lvstr was made the owner of it.
 *
 * @see
 * - stroll_lvstr_init_cede()
 * - stroll_lvstr_ncede()
 */
extern int
stroll_lvstr_cede(struct stroll_lvstr * __restrict lvstr, char * cstr)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf;

/**
 * Duplicate a C string which length is known and grant lvstr ownership of it.
 *
 * @param[inout] lvstr Length-Value String
 * @param[in]    cstr  C string to register
 * @param[in]    len   length of @p cstr
 *
 * @return an errno-like error code
 * @retval 0       success
 * @retval -ENOMEM duplicate allocation failed
 *
 * Duplicate a NULL terminated string into newly allocated memory and make
 * @p lvstr the owner of duplicated string.
 *
 * @p cstr *MUST* be a valid C string, i.e., a NULL byte terminated string which
 * length is greater than or equal to @p len, with @p len smaller than or equal
 * to #STROLL_LVSTR_LEN_MAX.
 * The duplicated string will contain characters over the range [0..len] only
 * and will be terminated by a NULL byte.
 *
 * @p lvstr is allowed to free @p cstr at release time, i.e., whenever
 * the registered string is replaced or when stroll_lvstr_fini() is called. Use
 * this when @p lvstr *should* handle @p cstr life-cycle.
 *
 * A previously registered C string is released before registration of @p cstr
 * duplicate.
 * Release operation may involve calling free() on the previously registered
 * string if @p lvstr was made the owner of it.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p cstr is invalid, result is undefined. An assertion is triggered
 *   otherwise.
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p len is greater than #STROLL_LVSTR_LEN_MAX, result is undefined. An
 *   assertion is triggered otherwise.
 *
 * @see
 * - stroll_lvstr_init_ndup()
 * - stroll_lvstr_dup()
 * - stroll_lvstr_ncede()
 */
extern int
stroll_lvstr_ndup(struct stroll_lvstr * __restrict lvstr,
                  const char *                     cstr,
                  size_t                           len)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __warn_result;

/**
 * Duplicate a C string which length is not known and grant lvstr ownership of
 * it.
 *
 * @param[inout] lvstr Length-Value String
 * @param[in]    cstr  C string to register
 *
 * @return an errno-like error code
 * @retval 0       success
 * @retval -E2BIG  @p cstr is invalid
 * @retval -ENOMEM duplicate allocation failed
 *
 * Duplicate a NULL terminated string into newly allocated memory and make
 * @p lvstr the owner of duplicated string.
 *
 * @p cstr *MUST* be a valid C string, i.e., a NULL byte terminated string which
 * length is smaller than or equal to #STROLL_LVSTR_LEN_MAX.
 *
 * @p lvstr is allowed to free @p cstr at release time, i.e., whenever
 * the registered string is replaced or when stroll_lvstr_fini() is called. Use
 * this when @p lvstr *should* handle @p cstr life-cycle.
 *
 * A previously registered C string is released before registration of @p cstr
 * duplicate.
 * Release operation may involve calling free() on the previously registered
 * string if @p lvstr was made the owner of it.
 *
 * @see
 * - stroll_lvstr_init_dup()
 * - stroll_lvstr_ndup()
 * - stroll_lvstr_cede()
 */
extern int
stroll_lvstr_dup(struct stroll_lvstr * __restrict lvstr, const char * cstr)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __warn_result;

/**
 * Initialize a non-owning stroll_lvstr with a string which length is known.
 *
 * @param[out] lvstr Length-Value String
 * @param[in]  cstr  C string to register
 * @param[in]  len   length of @p cstr
 *
 * This function is similar to stroll_lvstr_nlend() but do not release
 * previously registered C string.
 *
 * It is meant for stroll_lvstr initialization / first usage purpose only.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p cstr is invalid, result is undefined. An assertion is triggered
 *   otherwise.
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p len is greater than #STROLL_LVSTR_LEN_MAX, result is undefined. An
 *   assertion is triggered otherwise.
 *
 * @see
 * - STROLL_LVSTR_INIT_NLEND()
 * - stroll_lvstr_init_lend()
 */
static inline void __stroll_nonull(1, 2) __stroll_nothrow
stroll_lvstr_init_nlend(struct stroll_lvstr * __restrict lvstr,
                        const char *                     cstr,
                        size_t                           len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	lvstr->len = len << 1 | STROLL_LVSTR_LEASER;
	lvstr->rostr = cstr;
}

/**
 * Initialize a non-owning stroll_lvstr with a string which length is not known.
 *
 * @param[out] lvstr Length-Value String
 * @param[in]  cstr  C string to register
 *
 * @return an errno-like error code
 * @retval 0      success
 * @retval -E2BIG @p cstr is invalid
 *
 * This function is similar to stroll_lvstr_lend() but do not release previously
 * registered C string.
 *
 * It is meant for stroll_lvstr initialization / first usage purpose only.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 *
 * @see
 * stroll_lvstr_init_nlend()
 */
extern int
stroll_lvstr_init_lend(struct stroll_lvstr * __restrict lvstr,
                       const char *                     cstr)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf;

/**
 * Initialize an owning stroll_lvstr with a string which length is known.
 *
 * @param[out] lvstr Length-Value String
 * @param[in]  cstr  C string to register
 * @param[in]  len   length of @p cstr
 *
 * This function is similar to stroll_lvstr_ncede() but do not release
 * previously registered C string.
 *
 * It is meant for stroll_lvstr initialization / first usage purpose only.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p cstr is invalid, result is undefined. An assertion is triggered
 *   otherwise.
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p len is greater than #STROLL_LVSTR_LEN_MAX, result is undefined. An
 *   assertion is triggered otherwise.
 *
 * @see
 * - STROLL_LVSTR_INIT_NCEDE()
 * - stroll_lvstr_init_cede()
 */
static inline void __stroll_nonull(1, 2) __stroll_nothrow
stroll_lvstr_init_ncede(struct stroll_lvstr * __restrict lvstr,
                        char *                           cstr,
                        size_t                           len)
{
	stroll_lvstr_assert_api_ncstr(lvstr, cstr, len);

	lvstr->len = (len << 1) | STROLL_LVSTR_OWNER;
	lvstr->rwstr = cstr;
}

/**
 * Initialize an owning stroll_lvstr with a string which length is not known.
 *
 * @param[out] lvstr Length-Value String
 * @param[in]  cstr  C string to register
 *
 * @return an errno-like error code
 * @retval 0      success
 * @retval -E2BIG @p cstr is invalid
 *
 * This function is similar to stroll_lvstr_cede() but do not release previously
 * registered C string.
 *
 * It is meant for stroll_lvstr initialization / first usage purpose only.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 *
 * @see
 * stroll_lvstr_init_ncede()
 */
extern int
stroll_lvstr_init_cede(struct stroll_lvstr * __restrict lvstr, char * cstr)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf;

/**
 * Initialize an owning stroll_lvstr with a duplicated string which length is
 * known.
 *
 * @param[out] lvstr Length-Value String
 * @param[in]  cstr  C string to register
 * @param[in]  len   length of @p cstr
 *
 * @return an errno-like error code
 * @retval 0       success
 * @retval -ENOMEM duplicate allocation failed
 *
 * This function is similar to stroll_lvstr_ndup() but do not release previously
 * registered C string.
 *
 * It is meant for stroll_lvstr initialization / first usage purpose only.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 *
 * @warning
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p cstr is invalid, result is undefined. An assertion is triggered
 *   otherwise.
 * - When compiled with the #CONFIG_STROLL_ASSERT_API build option disabled and
 *   @p len is greater than #STROLL_LVSTR_LEN_MAX, result is undefined. An
 *   assertion is triggered otherwise.
 *
 * @see
 * stroll_lvstr_init_dup()
 */
extern int
stroll_lvstr_init_ndup(struct stroll_lvstr * __restrict lvstr,
                       const char *                     cstr,
                       size_t                           len)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __warn_result;

/**
 * Initialize an owning stroll_lvstr with a duplicated string which length is
 * not known.
 *
 * @param[out] lvstr Length-Value String
 * @param[in]  cstr  C string to register
 *
 * @return an errno-like error code
 * @retval 0       success
 * @retval -E2BIG  @p cstr is invalid
 * @retval -ENOMEM duplicate allocation failed
 *
 * This function is similar to stroll_lvstr_ndup() but do not release previously
 * registered C string.
 *
 * It is meant for stroll_lvstr initialization / first usage purpose only.
 *
 * @note
 * Once client code is done with the stroll_lvstr, it *MUST* call
 * stroll_lvstr_fini() to release allocated resources.
 *
 * @see
 * stroll_lvstr_init_ndup()
 */
extern int
stroll_lvstr_init_dup(struct stroll_lvstr * __restrict lvstr,
                      const char *                     cstr)
	__stroll_nonull(1, 2) __stroll_nothrow __leaf __warn_result;

/**
 * Release a previously registered string.
 *
 * @param[out] lvstr Length-Value String
 *
 * Give the caller a way to explictly release allocated resources (when needed)
 * of a previously registered C string. Internal reference to the latter is
 * dropped.
 *
 * Unless re-initialized, any subsequent calls to stroll_lvstr_cstr() will
 * return NULL.
 *
 * @see
 * stroll_lvstr_fini()
 */
extern void
stroll_lvstr_drop(struct stroll_lvstr * __restrict lvstr)
	__stroll_nonull(1) __stroll_nothrow __leaf;

/**
 * Initialize an empty stroll_lvstr.
 *
 * @param[out] lvstr Length-Value String
 *
 * @see
 * STROLL_LVSTR_INIT
 */
static inline void __stroll_nonull(1) __stroll_nothrow
stroll_lvstr_init(struct stroll_lvstr * __restrict lvstr)
{
	stroll_lvstr_assert_api(lvstr);

	lvstr->rostr = NULL;
}

/**
 * Finalize an empty stroll_lvstr.
 *
 * @param[inout] lvstr Length-Value String
 *
 * Release resources allocated for @p lvstr when required. Once called, you
 * *MUST NOT* re-use @p lvstr unless re-initialized first.
 *
 * If @p lvstr in an owning instance of stroll_lvstr, a previously registered C
 * string (if any) will be @man{free(3)}'ed.
 *
 * @see
 * - STROLL_LVSTR_INIT
 * - STROLL_LVSTR_INIT_NLEND()
 * - STROLL_LVSTR_INIT_NCEDE()
 * - stroll_lvstr_init()
 * - stroll_lvstr_init_cede()
 * - stroll_lvstr_init_dup()
 * - stroll_lvstr_init_lend()
 * - stroll_lvstr_init_ncede()
 * - stroll_lvstr_init_ndup()
 * - stroll_lvstr_init_nlend()
 * - stroll_lvstr_drop()
 */
extern void
stroll_lvstr_fini(struct stroll_lvstr * __restrict lvstr)
	__stroll_nonull(1) __stroll_nothrow __leaf;

#endif /* _STROLL_LVSTR_H */
