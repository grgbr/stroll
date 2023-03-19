/**
 * @file
 * Common preprocessing definitions
 *
 * @author       Grégor Boirie <gregor.boirie@free.fr>
 * @date         29 Aug 2017
 * @copyright    Copyright (C) 2017-2023 Grégor Boirie.
 * @licensestart GNU Lesser General Public License (LGPL) v3
 *
 * This file is part of libstroll
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, If not, see <http://www.gnu.org/licenses/>.
 * @licenseend
 */
#ifndef _STROLL_H
#define _STROLL_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* _GNU_SOURCE */

#include <stroll/config.h>
#include <stddef.h>
#include <values.h>
#include <sys/cdefs.h>
#include <assert.h>

/**
 * Tell compiler that a function, variable, type or (goto) label may possibly
 * be unused.
 *
 * May be used to prevent compiler from warning about unused functions,
 * parameters, variables, etc...
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 *      [GCC common variable attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#Common-Variable-Attributes)
 *      [GCC common type attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#Common-Type-Attributes)
 *      [GCC label attributes](https://gcc.gnu.org/onlinedocs/gcc/Label-Attributes.html#Label-Attributes)
 */
#define __unused __attribute__((unused))

/**
 * Declare to the compiler a function return value must be used.
 *
 * Request compiler to emit a warning when a caller of the function with this
 * attribute does not use its return value.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __warn_result __attribute__((warn_unused_result))

/**
 * Tell compiler that a function cannot return.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __noreturn __attribute__((noreturn))

/**
 * Tell compiler that a function return value should be a non-null pointer.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __returns_nonull __attribute__((returns_nonnull))

/**
 * Declare to the compiler a function argument should be a non-null pointer.
 *
 * When applied to a function, tell compiler that the specified arguments must
 * be non-null pointers.
 * @param[in] _arg_index index of first non-null pointer argument
 * @param[in] ...        subsequent non-null pointer argument indices
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __nonull(_arg_index, ...) \
	__attribute__((nonnull(_arg_index, ## __VA_ARGS__)))

/**
 * Tell compiler to check for printf() style format string argument consistency.
 *
 * @param[in] _format_index index of printf() format string argument
 * @param[in] _arg_index    index of first type checked argument
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __printf(_format_index, _arg_index) \
	__attribute__((format(printf, _format_index, _arg_index)))

/**
 * Tell compiler that a function does not throw exceptions.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __nothrow __attribute__((nothrow))

/**
 * Declare a function to the compiler as a leaf.
 *
 * Tell compiler that a call to an external function with this attribute must
 * return to the current compilation unit only by return or by exception
 * handling.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __leaf __attribute__((leaf))

/**
 * Declare a function to the compiler as pure.
 *
 * Tell compiler that a function has no observable effects on the state of the
 * program other than to return a value.
 *
 * Functions declared with the #__pure attribute can safely read any
 * non-volatile objects, and modify the value of objects in a way that does not
 * affect their return value or the observable state of the program.
 *
 * The #__pure attribute imposes similar but looser restrictions on a function’s
 * definition than the #__const attribute: #__pure allows the function to read
 * any non-volatile memory, even if it changes in between successive invocations
 * of the function.
 *
 * Because a #__pure function cannot have any observable side effects it does
 * not make sense for such a function to return void.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __pure __attribute__((pure))

/**
 * Declare a function to the compiler as const.
 *
 * Tell compiler that a function return value is not affected by changes to the
 * observable state of the program and that this function have no observable
 * effects on such state other than to return a value.
 *
 * The #__const attribute prohibits a function from reading objects that affect
 * its return value between successive invocations. However, functions declared
 * with the attribute can safely read objects that do not change their return
 * value, such as non-volatile constants.
 *
 * The #__const attribute imposes greater restrictions on a function’s
 * definition than the similar #__pure attribute. Declaring the same function
 * with both the #__const and the #__pure attribute is diagnosed.
 *
 * Because a #__const function cannot have any observable side effects it does
 * not make sense for it to return void. Declaring such a function is diagnosed.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __const __attribute__((const))

/**
 * Declare a variable or type to the compiler as packed.
 *
 * When assigned to a variable, tell the compiler that a structure member should
 * have the smallest possible alignment—one bit for a bit-field and one byte
 * otherwise, unless a larger value is specified with the aligned attribute. The
 * attribute does not apply to non-member objects.
 *
 * When attached to a struct, union, or C++ class type definition, tell the
 * compiler that each of its members (other than zero-width bit-fields) is
 * placed to minimize the memory required. This is equivalent to specifying the
 * packed attribute on each of the members.
 * When attached to an enum definition, the packed attribute indicates that the
 * smallest integral type should be used. Specifying the -fshort-enums flag on
 * the command line is equivalent to specifying the packed attribute on all enum
 * definitions.
 *
 * @see
 * - [GCC variable attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#Common-Variable-Attributes)
 * - [GCC type attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#Common-Type-Attributes)
 */
#define __packed \
	__attribute__((packed))

/**
 * Declare a variable or type to the compiler as requiring minimum alignment.
 *
 * @param[in] _size required alignment in bytes
 *
 * Tell the compiler that a minimum alignment is required for the variable or
 * structure field, measured in bytes. When specified, alignment must be an
 * integer constant power of 2.
 *
 * @see
 * - [GCC variable attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#Common-Variable-Attributes)
 * - [GCC type attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#Common-Type-Attributes)
 */
#define __align(_size) \
	__attribute__((aligned(_size)))

/**
 * Declare a function to the compiler as a constructor.
 *
 * @param[in] _prio constructor invocation order
 *
 * Tell the compiler that a function must be called automatically before
 * execution enters `main()`.
 * The attributes also accept an integer argument to specify a priority to
 * control the order in which constructor functions are run. A constructor with
 * a smaller priority number runs before a constructor with a larger priority
 * number.
 *
 * @note Priorities 0-100 are reserved.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __ctor(_prio) \
	__attribute__((constructor(_prio)))

/**
 * Declare a function to the compiler as a destructor.
 *
 * @param[in] _prio destructor invocation order
 *
 * Tell the compiler that a function must be called automatically after `main()`
 * completes or `exit()` is called.
 * The attributes also accept an integer argument to specify a priority to
 * control the order in which destructor functions are run. A destructor with
 * a larger priority number runs before a constructor with a smaller priority
 * number.
 *
 * @note Priorities 0-100 are reserved.
 *
 * @see [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 */
#define __dtor(_prio) \
	__attribute__((destructor(_prio)))

#define _compile_eval(_expr, _msg) \
	(!!sizeof(struct { static_assert(_expr, _msg); int _dummy; }))

/**
 * Evaluate constant expression at compile time.
 *
 * @param[in] _expr constant conditional expression
 * @param[in] _stmt constant statement
 * @param[in] _msg  compile error message string
 *
 * @return @p _stmt statement evaluation
 *
 * Generate a compile time error with error message @p _msg when constant
 * expression @p _expr is zero / false.
 * "Returns" value of @p _stmt evaluation otherwise.
 *
 * @note Borrowed from Gnulib's verify.h header file.
 */
#define compile_eval(_expr, _stmt, _msg) \
	(_compile_eval(_expr, "compile_eval(" # _expr "): " _msg) ? (_stmt) : \
	                                                            (_stmt))

/**
 * Abort compilation if constant expression evaluates to zero / false.
 *
 * @param[in] _expr constant conditional expression
 *
 * Generate a compile time error with error message @p _msg when constant
 * expression @p _expr is zero / false.
 *
 * @note Borrowed from Gnulib's verify.h header file.
 */
#define compile_assert(_expr) \
	static_assert(_expr, # _expr)

/**
 * @internal
 *
 * Compare 2 expression types at compile time.
 *
 * @param[in] _a first expression
 * @param[in] _b second expression
 *
 * @return `1` if types are similar, `0` otherwise.
 *
 * Return `true` if unqualified versions of the types of @p _a and @p _b are the
 * same.
 */
#define _is_same_type(_a, _b) \
	__builtin_types_compatible_p(typeof(_a), typeof(_b))

/**
 * @internal
 *
 * Check that an expression is a type of C array.
 *
 * @param[in] _array expression to check
 *
 * @return `1` if @p _array is a type of array.
 *
 * Returns true / nonzero when @p _array is an array whereas
 * `typeof(&(_array)[0])` degrades to a pointer, i.e., meaning that @p _array
 * is an "array type".
 *
 * @note Borrowed from Linux's kernel.h header file.
 */
#define _is_array(_array) \
	(!_is_same_type(_array, &(_array)[0]))

/**
 * Return the number of elements of a constant array expression.
 *
 * @param[in] _array array to retrieve number of elements for
 *
 * @return Number of elements of @p _array.
 *
 * @note Borrowed from Linux's kernel.h header file.
 */
#define array_nr(_array) \
	compile_eval(_is_array(_array), \
	             sizeof(_array) / sizeof((_array)[0]), \
	             "array expected")

#define likely(_expr) \
	__builtin_expect(!!(_expr), 1)

#define unlikely(_expr) \
	__builtin_expect(!!(_expr), 0)


#define PREFETCH_ACCESS_RO     (0)
#define PREFETCH_ACCESS_RW     (1)
#define PREFETCH_LOCALITY_TMP  (0)
#define PREFETCH_LOCALITY_LOW  (1)
#define PREFETCH_LOCALITY_FAIR (2)
#define PREFETCH_LOCALITY_HIGH (3)

#define prefetch(_address, ...) \
	__builtin_prefetch(_address, ## __VA_ARGS__)

#define unreachable() \
	__builtin_unreachable()

#define sizeof_member(_type, _member) \
	(sizeof(((_type *)0)->_member))

#define containerof(_ptr, _type, _member) \
	({ \
		((_type *)((char *)(_ptr) - offsetof(_type, _member))); \
	 })

#define uabs(_a) \
	({ \
		typeof(_a) __a = _a; \
		(__a >= 0) ? __a : -__a; \
	 })

#define umin(_a, _b) \
	({ \
		typeof(_a) __a = _a; \
		typeof(_b) __b = _b; \
		(__a < __b) ? __a : __b; \
	 })

#define umax(_a, _b) \
	({ \
		typeof(_a) __a = _a; \
		typeof(_b) __b = _b; \
		(__a > __b) ? __a : __b; \
	 })

#define __ualign_mask(_value, _align) \
	((typeof(_value))(_align) - 1)

#define ualign_mask(_align) \
	((_align) - 1)

#define ualign_lower(_value, _align) \
	((_value) & ~__ualign_mask(_value, _align))

#define ualign_upper(_value, _align) \
	ualign_lower((_value) + __ualign_mask(_value, _align), _align)

#define uround_upper(_value, _align) \
	({ \
		typeof(_value) __value = _value; \
		typeof(_value) __align = _align; \
		\
		__value = (__value + __align - 1) / __align; \
		__value * __align; \
	 })

#define uround_lower(_value, _align) \
	({ \
		typeof(_value) __value = _value; \
		typeof(_value) __align = _align; \
		\
		__value = __value / __align; \
		__value * __align; \
	 })

#if __WORDSIZE == 64
#define __UINTPTR_C(c) c ## UL
#define UWORD_SHIFT    (6)
#elif __WORDSIZE == 32
#define __UINTPTR_C(c) c ## U
#define UWORD_SHIFT    (5)
#else
#error "Unsupported machine word size !"
#endif

#define _UCONCAT(_a, _b) \
	(_a ## _b)

#define UCONCAT(_a, _b) \
	_UCONCAT(_a, _b)

#define _USTRINGIFY(_expr) \
	# _expr

#define USTRINGIFY(_expr) \
	_USTRINGIFY(_expr) \

#define __VA_ARGS_COUNT__( _c0,  _c1,  _c2,  _c3,  _c4,  _c5,  _c6,  _c7, \
                           _c8,  _c9, _c10, _c11, _c12, _c13, _c14, _c15, \
                          _c16, _c17, _c18, _c19, _c20, _c21, _c22, _c23, \
                          _c24, _c25, _c26, _c27, _c28, _c29, _c30, _c31, \
                          _c32, _c33, _c34, _c35, _c36, _c37, _c38, _C39, \
                          _c40, _c41, _c42, _c43, _c44, _c45, _c46, _c47, \
                          _c48, _c49, _c50, _c51, _c52, _c53, _c54, _c55, \
                          _c56, _c57, _c58, _c59, _c60, _c61, _c62, _c63, \
                          _c64, _cnt, ...) \
	_cnt

#define VA_ARGS_COUNT(...) \
	__VA_ARGS_COUNT__(_dummy, \
	                  ## __VA_ARGS__, \
	                                              64, \
	                  63, 62, 61, 60, 59, 58, 57, 56, \
	                  55, 54, 53, 52, 51, 50, 49, 48, \
	                  47, 46, 45, 44, 43, 42, 41, 40, \
	                  39, 38, 37, 36, 35, 34, 33, 32, \
	                  31, 30, 29, 28, 27, 26, 25, 24, \
	                  23, 22, 21, 20, 19, 18, 17, 16, \
	                  15, 14, 13, 12, 11, 10,  9,  8, \
	                   7,  6,  5,  4,  3,  2,  1,  0)

#endif /* _STROLL_H */
