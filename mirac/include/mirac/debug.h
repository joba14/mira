
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

#ifndef NDEBUG
#	include <mirac/c_common.h>

void mirac_debug_assert_impl(
	const bool expression,
	const char* const expression_string,
	const char* const file,
	const uint64_t line);

#	define mirac_debug_assert(_expression)                                    \
		mirac_debug_assert_impl(                                              \
			_expression,                                                       \
			#_expression,                                                      \
			(const char* const)__FILE__,                                       \
			(const uint64_t)__LINE__                                           \
		)
#else
#	define mirac_debug_assert(_expression)
#endif

#endif
