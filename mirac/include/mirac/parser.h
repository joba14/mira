
/**
 * @file parser.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-15
 */

#ifndef __mirac__include__mirac__parser_h__
#define __mirac__include__mirac__parser_h__

#include <mirac/c_common.h>
#include <mirac/heap_array.h>
#include <mirac/config.h>
#include <mirac/arena.h>
#include <mirac/lexer.h>

typedef struct mirac_global_s mirac_global_s;
mirac_define_heap_array_type(mirac_tokens_vector, mirac_token_s);
mirac_define_heap_array_type(mirac_tokens_refs_vector, mirac_token_s*);
mirac_define_heap_array_type(mirac_globals_vector, mirac_global_s);

typedef enum
{
	mirac_global_type_function = 0,
	mirac_global_type_memory
} mirac_global_type_e;

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

struct mirac_global_s
{
	mirac_global_type_e type;

	union
	{
		mirac_global_function_s function;
		mirac_global_memory_s memory;
	} as;
};

typedef struct
{
	mirac_string_view_s identifier;
	mirac_globals_vector_s globals;
	mirac_tokens_refs_vector_s strings;
} mirac_unit_s;

mirac_unit_s mirac_unit_from_parts(
	mirac_arena_s* const arena,
	const mirac_string_view_s identifier);

void mirac_unit_print(
	mirac_unit_s* const unit);

typedef struct
{
	mirac_config_s* config;
	mirac_arena_s* arena;
	mirac_lexer_s* lexer;
} mirac_parser_s;

mirac_parser_s mirac_parser_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_lexer_s* const lexer);

mirac_unit_s mirac_parser_parse_unit(
	mirac_parser_s* const parser);

#endif
