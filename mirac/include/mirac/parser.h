
/**
 * @file parser.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__parser_h__
#define __mirac__include__mirac__parser_h__

#include <mirac/lexer.h>

typedef enum
{
	mirac_global_type_function = 0,
	mirac_global_type_memory,
	mirac_global_type_string
} mirac_global_type_e;

typedef struct
{
	void* dummy;
} mirac_global_function_s;

typedef struct
{
	void* dummy;
} mirac_global_memory_s;

typedef struct
{
	void* dummy;
} mirac_global_string_s;

typedef struct
{
	mirac_global_type_e type;

	union
	{
		mirac_global_function_s function;
		mirac_global_string_s string;
	};
} mirac_global_s;

void mirac_parser_parse(
	mirac_lexer_s* const lexer);

#endif
