
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

typedef struct mirac_ast_block_func_s mirac_ast_block_func_s;
typedef struct mirac_ast_block_mem_s mirac_ast_block_mem_s;
typedef struct mirac_ast_block_str_s mirac_ast_block_str_s;

mirac_define_heap_array_type(mirac_tokens_vector, mirac_token_s);
mirac_define_heap_array_type(mirac_blocks_vector, mirac_ast_block_s);

mirac_define_heap_array_type(mirac_func_refs_vector, mirac_ast_block_func_s*);
mirac_define_heap_array_type(mirac_mem_refs_vector, mirac_ast_block_mem_s*);
mirac_define_heap_array_type(mirac_str_refs_vector, mirac_ast_block_str_s*);

typedef struct
{
	mirac_token_s token;
} mirac_ast_block_expr_s;

// TODO: document!
void mirac_ast_block_expr_print(
	const mirac_ast_block_expr_s* const expr_block,
	const uint64_t indent);

typedef enum
{
	mirac_ast_block_call_type_func = 0,
	mirac_ast_block_call_type_mem,
	mirac_ast_block_call_type_str
} mirac_ast_block_call_type_e;

typedef struct
{
	mirac_ast_block_call_type_e type;
	mirac_token_s token;

	union
	{
		mirac_ast_block_func_s* func_ref;
		mirac_ast_block_mem_s* mem_ref;
		mirac_ast_block_str_s* str_ref;
	} as;
} mirac_ast_block_call_s;

// TODO: document!
void mirac_ast_block_call_print(
	const mirac_ast_block_call_s* const call_block,
	const uint64_t indent);

typedef struct
{
	mirac_tokens_vector_s type_tokens;
} mirac_ast_block_as_s;

// TODO: document!
void mirac_ast_block_as_print(
	const mirac_ast_block_as_s* const as_block,
	const uint64_t indent);

typedef struct
{
	mirac_blocks_vector_s blocks;
} mirac_ast_block_scope_s;

// TODO: document!
void mirac_ast_block_scope_print(
	const mirac_ast_block_scope_s* const scope_block,
	const uint64_t indent);

typedef struct
{
	mirac_ast_block_scope_s cond_scope;
	mirac_ast_block_scope_s body_scope;
} mirac_ast_block_if_s;

// TODO: document!
void mirac_ast_block_if_print(
	const mirac_ast_block_if_s* const if_block,
	const uint64_t indent);

typedef struct
{
	mirac_ast_block_scope_s cond_scope;
	mirac_ast_block_scope_s body_scope;
} mirac_ast_block_elif_s;

// TODO: document!
void mirac_ast_block_elif_print(
	const mirac_ast_block_elif_s* const elif_block,
	const uint64_t indent);

typedef struct
{
	mirac_ast_block_scope_s body_scope;
} mirac_ast_block_else_s;

// TODO: document!
void mirac_ast_block_else_print(
	const mirac_ast_block_else_s* const else_block,
	const uint64_t indent);

typedef struct
{
	mirac_ast_block_scope_s cond_scope;
	mirac_ast_block_scope_s body_scope;
} mirac_ast_block_loop_s;

// TODO: document!
void mirac_ast_block_loop_print(
	const mirac_ast_block_loop_s* const loop_block,
	const uint64_t indent);

struct mirac_ast_block_func_s
{
	mirac_token_s identifier;
	mirac_tokens_vector_s req_tokens;
	mirac_tokens_vector_s ret_tokens;
	mirac_ast_block_scope_s body_scope;
	bool is_inlined;
	bool is_entry;
	bool is_used;
};

// TODO: document!
void mirac_ast_block_func_print(
	const mirac_ast_block_func_s* const func_block,
	const uint64_t indent);

struct mirac_ast_block_mem_s
{
	mirac_token_s identifier;
	mirac_token_s capacity;
	bool is_used;
};

// TODO: document!
void mirac_ast_block_mem_print(
	const mirac_ast_block_mem_s* const mem_block,
	const uint64_t indent);

struct mirac_ast_block_str_s
{
	mirac_token_s identifier;
	mirac_token_s literal;
	bool is_used;
};

// TODO: document!
void mirac_ast_block_str_print(
	const mirac_ast_block_str_s* const str_block,
	const uint64_t indent);

typedef enum
{
	mirac_ast_block_type_expr = 0,
	mirac_ast_block_type_call,
	mirac_ast_block_type_as,
	mirac_ast_block_type_scope,
	mirac_ast_block_type_if,
	mirac_ast_block_type_elif,
	mirac_ast_block_type_else,
	mirac_ast_block_type_loop,
	mirac_ast_block_type_func,
	mirac_ast_block_type_mem,
	mirac_ast_block_type_str,

	mirac_ast_block_type_eou,
	mirac_ast_block_type_none
} mirac_ast_block_type_e;

// TODO: document!
mirac_string_view_s mirac_ast_block_type_to_string_view(
	const mirac_ast_block_type_e type);

struct mirac_ast_block_s
{
	mirac_ast_block_type_e type;
	mirac_location_s location;

	union
	{
		mirac_ast_block_expr_s  expr_block;
		mirac_ast_block_call_s  call_block;
		mirac_ast_block_as_s    as_block;
		mirac_ast_block_scope_s scope_block;
		mirac_ast_block_if_s    if_block;
		mirac_ast_block_elif_s  elif_block;
		mirac_ast_block_else_s  else_block;
		mirac_ast_block_loop_s  loop_block;
		mirac_ast_block_func_s  func_block;
		mirac_ast_block_mem_s   mem_block;
		mirac_ast_block_str_s   str_block;
	} as;
};

// TODO: document!
mirac_ast_block_s mirac_ast_block_from_type(
	const mirac_ast_block_type_e type);

// TODO: document!
void mirac_ast_block_print(
	const mirac_ast_block_s* const block,
	const uint64_t indent);

typedef struct
{
	mirac_blocks_vector_s blocks;
	mirac_func_refs_vector_s func_refs;
	mirac_mem_refs_vector_s mem_refs;
	mirac_str_refs_vector_s str_refs;
} mirac_ast_unit_s;

// TODO: document!
mirac_ast_unit_s mirac_ast_unit_from_parts(
	mirac_arena_s* const arena);

// TODO: document!
void mirac_ast_unit_print(
	const mirac_ast_unit_s* const unit,
	const uint64_t indent);

typedef struct
{
	mirac_config_s* config;
	mirac_arena_s* arena;
	mirac_lexer_s* lexer;
	mirac_ast_unit_s unit;
} mirac_parser_s;

// TODO: document!
mirac_parser_s mirac_parser_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_lexer_s* const lexer);

// TODO: document!
void mirac_parser_destroy(
	mirac_parser_s* const parser);

// TODO: document!
mirac_ast_unit_s mirac_parser_parse_ast_unit(
	mirac_parser_s* const parser);

#endif
