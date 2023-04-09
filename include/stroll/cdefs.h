/**
 * @file
 * Common preprocessing definitions
 *
 * @author       Grégor Boirie <gregor.boirie@free.fr>
 * @date         29 Aug 2017
 * @copyright    Copyright (C) 2017-2023 Grégor Boirie.
 * @licensestart GNU Lesser General Public License (LGPL) v3
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
#ifndef _STROLL_CDEFS_H
#define _STROLL_CDEFS_H

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
 * @see
 * - [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 * - [GCC common variable attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#Common-Variable-Attributes)
 * - [GCC common type attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#Common-Type-Attributes)
 * - [GCC label attributes](https://gcc.gnu.org/onlinedocs/gcc/Label-Attributes.html#Label-Attributes)
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
 * Declare to the compiler a that function argument should be a non-null
 * pointer.
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
 * @internal
 *
 * Declare to the compiler that a Stroll function argument should be a non-null
 * pointer.
 *
 * @note
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option enabled, this
 * macro expands as empty.
 *
 * @see #__nonull
 */

#if defined(CONFIG_STROLL_ASSERT_API) || defined(CONFIG_STROLL_ASSERT_INTERN)

#define __stroll_nonull(_arg_index, ...)

#else  /* !(defined(CONFIG_STROLL_ASSERT_API) || \
            defined(CONFIG_STROLL_ASSERT_INTERN)) */

#define __stroll_nonull(_arg_index, ...) __nonull(_arg_index, ## __VA_ARGS__)

#endif /* defined(CONFIG_STROLL_ASSERT_API) || \
          defined(CONFIG_STROLL_ASSERT_INTERN) */

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
 * @internal
 *
 * Tell compiler that a function does not throw exceptions.
 *
 * @note
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option enabled, this
 * macro expands as empty since a function triggering a Stroll assertion calls
 * @man{fprintf(3)} which is itself a cancellation point.
 *
 * @see #__nothrow
 * @see stroll_assert_fail()
 */

#if defined(CONFIG_STROLL_ASSERT_API) || defined(CONFIG_STROLL_ASSERT_INTERN)

#define __stroll_nothrow

#else  /* !(defined(CONFIG_STROLL_ASSERT_API) || \
            defined(CONFIG_STROLL_ASSERT_INTERN)) */

#define __stroll_nothrow __nothrow

#endif /* defined(CONFIG_STROLL_ASSERT_API) || \
          defined(CONFIG_STROLL_ASSERT_INTERN) */

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
 * @internal
 *
 * Declare a Stroll function to the compiler as pure.
 *
 * @note
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option enabled, this
 * macro expands as empty since a function triggering a Stroll assertion, which
 * itself calls @man{fprintf(3)}, cannot be #__pure.
 *
 * @see #__pure
 * @see stroll_assert_fail()
 */

#if defined(CONFIG_STROLL_ASSERT_API) || defined(CONFIG_STROLL_ASSERT_INTERN)

#define __stroll_pure

#else  /* !(defined(CONFIG_STROLL_ASSERT_API) || \
            defined(CONFIG_STROLL_ASSERT_INTERN)) */

#define __stroll_pure __pure

#endif /* defined(CONFIG_STROLL_ASSERT_API) || \
          defined(CONFIG_STROLL_ASSERT_INTERN) */

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
 * @internal
 *
 * Declare a Stroll function to the compiler as const.
 *
 * @note
 * When compiled with the #CONFIG_STROLL_ASSERT_API build option enabled, this
 * macro expands as empty since a function triggering a Stroll assertion, which
 * itself calls @man{fprintf(3)}, cannot be #__const.
 *
 * @see #__const
 * @see stroll_assert_fail()
 */

#if defined(CONFIG_STROLL_ASSERT_API)

#define __stroll_const

#else  /* !(defined(CONFIG_STROLL_ASSERT_API) || \
            defined(CONFIG_STROLL_ASSERT_INTERN)) */

#define __stroll_const __const

#endif /* defined(CONFIG_STROLL_ASSERT_API) || \
          defined(CONFIG_STROLL_ASSERT_INTERN) */

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

/**
 * Assign public visibility to a function, variable or type.
 *
 * Attach "default" visibility to a function, variable or type declaration
 * linkage.
 * On ELF, default visibility means that the declaration is visible to other
 * modules and, in shared libraries, means that the declared entity may be
 * overridden.
 *
 * @see
 * - [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 * - [GCC common variable attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#Common-Variable-Attributes)
 * - [GCC common type attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#Common-Type-Attributes)
 */
#define __export_public \
	__attribute__((visibility("default")))

/**
 * Assign protected visibility to a function, variable or type.
 *
 * Attach "protected" visibility to a function, variable or type declaration
 * linkage.
 * On ELF, protected visibility is like default visibility except that it
 * indicates that references within the defining module bind to the definition
 * in that module. That is, the declared entity cannot be overridden by another
 * module.
 *
 * @see
 * - #__export_public
 * - [GCC common function attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes)
 * - [GCC common variable attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#Common-Variable-Attributes)
 * - [GCC common type attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#Common-Type-Attributes)
 */
#define __export_protect \
	__attribute__((visibility("default")))

#define _STROLL_CONCAT(_a, _b) \
	_a ## _b

/**
 * Concatenate 2 preprocessor tokens.
 *
 * @param[in] _a first token
 * @param[in] _b second token
 *
 * Concatenate @p _a with @p _b during C preprocessor macro expansion.
 *
 * @see [GCC Concatenation](https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html)
 */
#define STROLL_CONCAT(_a, _b) \
	_STROLL_CONCAT(_a, _b)


#define _STROLL_STRING(_token) \
	# _token

/**
 * Convert a preprocessor token into a string constant.
 *
 * @param[in] _token token to convert
 *
 * Convert @p _a into a string during C preprocessor macro expansion.
 *
 * @see [GCC Stringizing](https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html#Stringizing)
 */
#define STROLL_STRING(_token) \
	_STROLL_STRING(_token)

/**
 * Generate unique preprocessor token.
 *
 * @param[in] _token base preprocessor identifier
 *
 * Preprocessor magic generating a unique identifier from a primary token.
 * #STROLL_UNIQ() basically relies upon GCC `__COUNTER__` predefined macro to
 * form a unique identifier composed of the concatenation of :
 * - `__stroll_uniq__` literal,
 * - @p _token,
 * - a unique sequential integral values starting from `0`.
 *
 * This may be usefull to prevent GCC from warning about shadowed variables used
 * into preprocessor macros defining local variables. See #stroll_min() /
 * #stroll_max() for practical examples.
 *
 * @see [GCC Common predefined macros](https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html#Common-Predefined-Macros)
 */
#define STROLL_UNIQ(_token) \
	STROLL_CONCAT(STROLL_CONCAT(__stroll_uniq__, _token), __COUNTER__)

/**
 * Evaluate code depending on the result of a constant expression.
 *
 * @param[in] _expr       constant conditional expression
 * @param[in] _true_stmt  statement to evaluate if @p _expr is true / nonzero
 * @param[in] _false_stmt statement to evaluate if @p _expr is false / zero
 *
 * @return value of selected statement evaluation
 *
 * Use compile_choose() to evaluate code depending on the value @p _expr
 * constant expression. This returns @p _true_stmt if @p _expr, which is an
 * integer constant expression, is nonzero. Otherwise it returns @p _false_stmt.
 *
 * This function macro is analogous to the ‘? :’ operator in C, except that
 * the expression returned has its type unaltered by promotion rules. Also, the
 * compile_choose() does not evaluate the expression that is not chosen.
 *
 * @note
 * the unused statement may still generate syntax errors.
 *
 * @see [GCC Other Built-in Functions](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)
 */
#define compile_choose(_expr, _true_stmt, _false_stmt) \
	__builtin_choose_expr(_expr, _true_stmt, _false_stmt)

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
 * Determine if an expression is known to be constant at compile time.
 *
 * @param[in] _expr Expression to test
 *
 * @return `1` if @p _expr is constant at compile time, `0` otherwise.
 */
#define _is_const(_expr) \
	__builtin_constant_p(_expr)

/**
 * @internal
 *
 * Generate a compile time warning when 2 types are not compatible.
 *
 * Basically, both arguments are tested for equality. The test will generate a
 * warning when argument types are not compatible.
 * Note that comparison is performed using pointer types to prevent from
 * implicit types conversion during equality test.
 *
 * The sizeof operator is applied to ensure this is a constant expression that
 * always evaluates to true (thanks to the `!!`).
 *
 * Idea borrowed from the Linux kernel source tree
 * <linux>/include/linux/minmax.h header.
 *
 * @see [GCC Other Built-in Functions](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)
 */
#define _warn_incompatible_types(_a, _b) \
	(!!sizeof((typeof(_a) *)1 == (typeof(_b) *)1))

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
 * Return `1` if unqualified versions of the types of @p _a and @p _b are the
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

#define _stroll_eval_cmp(_a, _b, _op) \
	(((_a) _op (_b)) ? (_a) : (_b))

#define _stroll_eval_cmp_once(_a, _b, _uniq_a, _uniq_b, _op) \
	({ \
		typeof(_a) _uniq_a = _a; \
		typeof(_b) _uniq_b = _b; \
		_stroll_eval_cmp(_uniq_a, _uniq_b, _op); \
	 })

/**
 * @internal
 *
 * Compare 2 arguments using given compare operator.
 *
 * @param[in] _a  first argument
 * @param[in] _b  second argument
 * @param[in] _op compare operator
 *
 * @return argument value resulting from the compare operation
 *
 * Check that both argument types are compatible and emit a warning if they are
 * not. Then, if both arguments are compile time constants, perform the
 * comparison according to @p _op operator and return the argument value
 * resulting from the compare operation.
 *
 * In case where one or both argument values are not compile time constants,
 * ensure that arguments are evaluated only once using temporary local
 * variables, and return the argument value resulting from the compare
 * operation.
 * To prevent from variable shadowing, argument evaluation is performed using
 * temporary local variable identifiers generated thanks to the #STROLL_UNIQ()
 * macro.
 */
#define _stroll_cmp(_a, _b, _op) \
	compile_choose(_warn_incompatible_types(_a, _b) && \
	               _is_const(_a) && _is_const(_b), \
	               _stroll_eval_cmp(_a, _b, _op), \
	               _stroll_eval_cmp_once(_a, \
	                                     _b, \
	                                     STROLL_UNIQ(__a), \
	                                     STROLL_UNIQ(__b), \
	                                     _op))

/**
 * Return the smallest of 2 arguments.
 *
 * @param[in] _a first argument
 * @param[in] _b second argument
 *
 * @return smallest argument
 *
 * Both argument types *MUST* be compatible. A warning will be generated
 * otherwise.
 * Both arguments are evaluated only once respectively to prevent from runtime
 * side-effects.
 */
#define stroll_min(_a, _b) \
	_stroll_cmp(_a, _b, <)

/**
 * Return the greatest of 2 arguments.
 *
 * @param[in] _a first argument
 * @param[in] _b second argument
 *
 * @return greatest argument
 *
 * Both argument types *MUST* be compatible. A warning will be generated
 * otherwise.
 * Both arguments are evaluated only once respectively to prevent from runtime
 * side-effects.
 */
#define stroll_max(_a, _b) \
	_stroll_cmp(_a, _b, >)

#define _stroll_eval_abs(_a) \
	(((_a) >= 0) ? _a : -_a)

/**
 * @internal
 *
 * Return the absolute value of the given argument.
 *
 * @param[in] _a argument
 *
 * @return absolute value of @p _a
 *
 * Ensure that argument @p _a is evaluated only once using temporary local
 * variables, and return the absolute value of @p _a.
 * To prevent from variable shadowing, argument evaluation is performed using
 * a temporary local variable identifier generated thanks to the #STROLL_UNIQ()
 * macro.
 */
#define _stroll_eval_abs_once(_a, _uniq_a) \
	({ \
		typeof(_a) _uniq_a = _a; \
		_stroll_eval_abs(_uniq_a); \
	 })

/**
 * Return the absolute value of the given argument.
 *
 * @param[in] _a argument
 *
 * @return absolute value of @p _a
 *
 * Argument is evaluated only once to prevent from runtime side-effects.
 */
#define stroll_abs(_a) \
	compile_choose(_is_const(_a), \
	               _stroll_eval_abs(_a), \
	               _stroll_eval_abs_once(_a, STROLL_UNIQ(__a)))

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

#endif /* _STROLL_CDEFS_H */
