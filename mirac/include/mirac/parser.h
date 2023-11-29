
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

#include <mirac/vector.h>
#include <mirac/lexer.h>

mirac_define_vector_type(mirac_tokens_vector, mirac_token_s);

typedef enum
{
	mirac_global_type_function = 0,
	mirac_global_type_memory,
	mirac_global_type_string
} mirac_global_type_e;

const char* mirac_global_type_to_string(
	const mirac_global_type_e global_type);

typedef struct
{
	mirac_token_s identifier;
	mirac_tokens_vector_s req_tokens;
	mirac_tokens_vector_s ret_tokens;
	mirac_tokens_vector_s body_tokens;
	bool is_inlined;
	bool is_entry;
} mirac_global_function_s;

typedef struct
{
	mirac_token_s identifier;
	mirac_token_s capacity;
} mirac_global_memory_s;

typedef struct
{
	mirac_token_s literal;
} mirac_global_string_s;

typedef struct
{
	mirac_global_type_e type;

	union
	{
		mirac_global_function_s function;
		mirac_global_memory_s memory;
		mirac_global_string_s string;
	};
} mirac_global_s;

mirac_define_vector_type(mirac_globals_vector, mirac_global_s);

const char* mirac_global_to_string(
	const mirac_global_s* const global);

typedef struct
{
	mirac_lexer_s* lexer;
} mirac_parser_s;

mirac_parser_s mirac_parser_from_parts(
	mirac_lexer_s* const lexer);

mirac_globals_vector_s mirac_parser_parse(
	mirac_parser_s* const parser);

#endif
