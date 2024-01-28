
/**
 * @file debug.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__debug_h__
#define __mirac__include__mirac__debug_h__

#ifndef mirac_ndebug
#	include <mirac/c_common.h>

/**
 * @brief Debug assert implementation.
 * 
 * @param expression        expression to evaluate
 * @param expression_string stringified expression
 * @param file              file in which assert exists
 * @param line              line in which assert exists
 */
void mirac_debug_assert_impl(
	const bool_t expression,
	const char_t* const expression_string,
	const char_t* const file,
	const uint64_t line);

/**
 * @brief Debug assert wrapper.
 * 
 * @note It abstracts the @ref mirac_debug_assert_impl function and passes line,
 * file, and stringified expression to the implementation function.
 */
#	define mirac_debug_assert(_expression)                                     \
		mirac_debug_assert_impl(                                               \
			_expression,                                                       \
			#_expression,                                                      \
			(const char_t* const)__FILE__,                                       \
			(const uint64_t)__LINE__                                           \
		)
#else
/**
 * @brief Debug assert wrapper.
 * 
 * @note It abstracts the @ref mirac_debug_assert_impl function and passes line,
 * file, and stringified expression to the implementation function.
 */
#	define mirac_debug_assert(_expression) ((void)(_expression))
#endif

#endif
