
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

typedef struct mirac_ast_block_s mirac_ast_block_s;
mirac_define_heap_array_type(mirac_tokens_refs_vector, mirac_token_s*);
mirac_define_heap_array_type(mirac_blocks_vector, mirac_ast_block_s);

typedef struct
{
	mirac_token_s token;
} mirac_ast_block_expr_s;

mirac_ast_block_expr_s mirac_ast_block_expr_from_parts(
	void);

typedef struct
{
	mirac_blocks_vector_s condition_blocks;
	mirac_blocks_vector_s body_blocks;
} mirac_ast_block_if_s;

mirac_ast_block_if_s mirac_ast_block_if_from_parts(
	mirac_arena_s* const arena);

typedef struct
{
	mirac_blocks_vector_s condition_blocks;
	mirac_blocks_vector_s body_blocks;
} mirac_ast_block_elif_s;

mirac_ast_block_elif_s mirac_ast_block_elif_from_parts(
	mirac_arena_s* const arena);

typedef struct
{
	mirac_blocks_vector_s body_blocks;
} mirac_ast_block_else_s;

mirac_ast_block_else_s mirac_ast_block_else_from_parts(
	mirac_arena_s* const arena);

typedef struct
{
	mirac_blocks_vector_s condition_blocks;
	mirac_blocks_vector_s body_blocks;
} mirac_ast_block_loop_s;

mirac_ast_block_loop_s mirac_ast_block_loop_from_parts(
	mirac_arena_s* const arena);

typedef struct
{
	mirac_blocks_vector_s identifier_blocks;
	mirac_blocks_vector_s body_blocks;
} mirac_ast_block_let_s;

mirac_ast_block_let_s mirac_ast_block_let_from_parts(
	mirac_arena_s* const arena);

typedef struct
{
	mirac_token_s identifier;
	mirac_blocks_vector_s req_blocks;
	mirac_blocks_vector_s ret_blocks;
	mirac_blocks_vector_s body_blocks;
	bool is_inlined;
	bool is_entry;
	bool is_used;
} mirac_ast_block_func_s;

mirac_ast_block_func_s mirac_ast_block_func_from_parts(
	mirac_arena_s* const arena);

typedef struct
{
	mirac_token_s identifier;
	mirac_token_s capacity;
	bool is_used;
} mirac_ast_block_mem_s;

mirac_ast_block_mem_s mirac_ast_block_mem_from_parts(
	void);

typedef enum
{
	mirac_ast_block_type_expr = 0,
	mirac_ast_block_type_if,
	mirac_ast_block_type_elif,
	mirac_ast_block_type_else,
	mirac_ast_block_type_loop,
	mirac_ast_block_type_let,
	mirac_ast_block_type_func,
	mirac_ast_block_type_mem
} mirac_ast_block_type_e;

struct mirac_ast_block_s
{
	mirac_ast_block_type_e type;

	union
	{
		mirac_ast_block_expr_s expr_block;
		mirac_ast_block_if_s   if_block;
		mirac_ast_block_elif_s elif_block;
		mirac_ast_block_else_s else_block;
		mirac_ast_block_loop_s loop_block;
		mirac_ast_block_let_s  let_block;
		mirac_ast_block_func_s func_block;
		mirac_ast_block_mem_s  mem_block;
	} as;
};

typedef struct
{
	mirac_string_view_s identifier;
	mirac_blocks_vector_s global_blocks;
	mirac_tokens_refs_vector_s strings;
} mirac_ast_unit_s;

mirac_ast_unit_s mirac_ast_unit_from_parts(
	mirac_arena_s* const arena,
	const mirac_string_view_s identifier);

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

mirac_ast_unit_s mirac_parser_parse_ast_unit(
	mirac_parser_s* const parser);

#endif
