
/**
 * @file debug.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#include <mirac/debug.h>

#ifndef NDEBUG
#	include <stdio.h>

void mirac_debug_assert_impl(
	const bool_t expression,
	const char_t* const expression_string,
	const char_t* const file,
	const uint64_t line)
{
	if (!expression)
	{
		(void)fprintf(stderr,
			"\033[91m" "\033[1m" "%s" "\033[0m" ": assertion '%s' failed at %s:%lu\n",
			"fatal", expression_string, file, line);
		mirac_c_exit(-1);
	}
}
#else
_Static_assert(1, ""); // note: to prevent empty translation unit error.
#endif
