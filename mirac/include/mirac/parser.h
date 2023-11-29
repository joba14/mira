
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
	mirac_token_s* identifier;
	// TODO: req list
	// TODO: ret list
	// TODO: function body
} mirac_global_function_s;

typedef struct
{
	mirac_token_s* identifier;
	mirac_token_s* capacity;
} mirac_global_memory_s;

typedef struct
{
	mirac_token_s* literal;
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

typedef struct
{
	mirac_lexer_s* lexer;
} mirac_parser_s;

mirac_parser_s mirac_parser_from_parts(
	mirac_lexer_s* const lexer);

void mirac_parser_parse(
	mirac_parser_s* const parser);

#endif
