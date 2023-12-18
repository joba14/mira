
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

mirac_define_heap_array_type(mirac_ast_tokens_vector, mirac_token_s);
mirac_define_heap_array_type(mirac_ast_tokens_refs_vector, mirac_token_s*);

typedef struct
{
	mirac_ast_tokens_vector_s globals;
	mirac_ast_tokens_refs_vector_s strings;
} mirac_unit_s;

// TODO: write unit tests!
// TODO: document!
mirac_unit_s mirac_unit_from_parts(
	mirac_arena_s* const arena);

typedef struct
{
	mirac_config_s* config;
	mirac_arena_s* arena;
	mirac_lexer_s* lexer;
} mirac_parser_s;

// TODO: write unit tests!
// TODO: document!
mirac_parser_s mirac_parser_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_lexer_s* const lexer);

// TODO: write unit tests!
// TODO: document!
mirac_unit_s mirac_parser_parse_unit(
	mirac_parser_s* const parser);

#endif
