#ifndef _STROLL_BITOPS_H
#define _STROLL_BITOPS_H

#include <stroll/cdefs.h>
#include <stdint.h>

#if defined(CONFIG_STROLL_ASSERT_API)

#include <stroll/assert.h>

#define stroll_bops_assert_api(_expr) \
	stroll_assert("stroll: bops", _expr)

/* Functions using assertions are not __const. */
#define __stroll_const

#else /* !defined(CONFIG_STROLL_ASSERT_API) */

#define stroll_bops_assert_api(_expr)

#define __stroll_const __const

#endif /* defined(CONFIG_STROLL_ASSERT_API) */

static inline unsigned int __stroll_const __nothrow
stroll_bops_fls32(uint32_t mask)
{
	stroll_bops_assert_api(mask != 0);

	return (sizeof(mask) * CHAR_BIT) - __builtin_clz(mask);
}

static inline unsigned int __stroll_const __nothrow
stroll_bops_ffs32(uint32_t mask)
{
	return __builtin_ffs(mask);
}

#if __WORDSIZE == 64

static inline unsigned int __stroll_const __nothrow
stroll_bops_fls64(uint64_t mask)
{
	stroll_bops_assert_api(mask != 0);

	return (sizeof(mask) * CHAR_BIT) - __builtin_clzl(mask);
}

static inline unsigned int __stroll_const __nothrow
stroll_bops_ffs64(uint64_t mask)
{
	return __builtin_ffsl(mask);
}

#elif __WORDSIZE == 32

static inline unsigned int __stroll_const __nothrow
stroll_bops_fls64(uint64_t mask)
{
	stroll_bops_assert_api(mask != 0);

	return (sizeof(mask) * CHAR_BIT) - __builtin_clzll(mask);
}

static inline unsigned int __stroll_const __nothrow
stroll_bops_ffs64(uint64_t mask)
{
	return __builtin_ffsll(mask);
}

#else

#error "Unsupported machine word size !"

#endif

#define stroll_bops_bitsof(_expr) \
	(sizeof(_expr) * CHAR_BIT)

#define _stroll_bops_fls(_value) \
	((stroll_bops_bitsof(_value) == 32U) ? stroll_bops_fls32(_value) : \
	                                       stroll_bops_fls64(_value))

#define stroll_bops_fls(_value) \
	compile_eval((stroll_bops_bitsof(_value) == 32U) || \
	             (stroll_bops_bitsof(_value) == 64U), \
	             _stroll_bops_fls(_value), \
	             "invalid bitops FLS word size")

#define _stroll_bops_ffs(_value) \
	((stroll_bops_bitsof(_value) == 32U) ? stroll_bops_ffs32(_value) : \
	                                       stroll_bops_ffs64(_value))

#define stroll_bops_ffs(_value) \
	compile_eval((stroll_bops_bitsof(_value) == 32U) || \
	             (stroll_bops_bitsof(_value) == 64U), \
	             _stroll_bops_ffs(_value), \
	             "invalid bitops FFS word size")

#endif /* _STROLL_BITOPS_H */
