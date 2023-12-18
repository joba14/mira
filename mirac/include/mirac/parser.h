
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

typedef struct
{
	void* _dummy; // TODO: implement!
} mirac_ast_expression_block_s;

typedef struct
{
	void* _dummy; // TODO: implement!
} mirac_ast_branching_block_s;

typedef struct
{
	void* _dummy; // TODO: implement!
} mirac_ast_loop_block_s;

typedef struct
{
	void* _dummy; // TODO: implement!
} mirac_ast_let_block_s;

typedef struct
{
	void* _dummy; // TODO: implement!
} mirac_ast_reg_block_s;

typedef struct
{
	void* _dummy; // TODO: implement!
} mirac_ast_function_block_s;

typedef struct
{
	void* _dummy; // TODO: implement!
} mirac_ast_memory_block_s;

typedef enum
{
	mirac_ast_block_type_expression = 0,
	mirac_ast_block_type_branching,
	mirac_ast_block_type_loop,
	mirac_ast_block_type_let,
	mirac_ast_block_type_reg
} mirac_ast_block_type_e;

typedef struct
{
	mirac_ast_block_type_e type;

	union
	{
		mirac_ast_expression_block_s as_expression_block;
		mirac_ast_branching_block_s as_branching_block;
		mirac_ast_loop_block_s as_loop_block;
		mirac_ast_let_block_s as_let_block;
		mirac_ast_reg_block_s as_reg_block;
	};
} mirac_ast_block_s;

mirac_define_heap_array_type(mirac_ast_blocks_vector, mirac_ast_block_s);
mirac_define_heap_array_type(mirac_ast_blocks_refs_vector, mirac_ast_block_s*);

typedef struct
{
	mirac_ast_blocks_vector_s globals;
	mirac_ast_blocks_refs_vector_s strings;
} mirac_ast_unit_s;

// TODO: write unit tests!
// TODO: document!
mirac_ast_unit_s mirac_ast_unit_from_parts(
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
mirac_ast_unit_s mirac_parser_parse_ast_unit(
	mirac_parser_s* const parser);

#endif
