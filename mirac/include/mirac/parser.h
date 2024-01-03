
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
#include <mirac/linked_list.h>
#include <mirac/config.h>
#include <mirac/arena.h>
#include <mirac/lexer.h>

typedef struct mirac_ast_block_s mirac_ast_block_s;
typedef struct mirac_ast_def_s mirac_ast_def_s;

mirac_define_linked_list_type(mirac_token_list, mirac_token_s);
mirac_define_linked_list_type(mirac_ast_block_list, mirac_ast_block_s*);
mirac_define_linked_list_type(mirac_ast_def_list, mirac_ast_def_s*);

typedef struct
{
	mirac_token_s token;
} mirac_ast_block_expr_s;

typedef struct
{
	mirac_token_list_s type_tokens;
} mirac_ast_block_as_s;

typedef struct
{
	mirac_ast_block_list_s blocks;
} mirac_ast_block_scope_s;

typedef struct
{
	mirac_ast_block_scope_s cond_scope;
	mirac_ast_block_scope_s body_scope;
} mirac_ast_block_if_s;

typedef struct
{
	mirac_ast_block_scope_s cond_scope;
	mirac_ast_block_scope_s body_scope;
} mirac_ast_block_elif_s;

typedef struct
{
	mirac_ast_block_scope_s body_scope;
} mirac_ast_block_else_s;

typedef struct
{
	mirac_ast_block_scope_s cond_scope;
	mirac_ast_block_scope_s body_scope;
} mirac_ast_block_loop_s;

typedef enum
{
	mirac_ast_block_type_expr = 0,
	mirac_ast_block_type_as,
	mirac_ast_block_type_scope,
	mirac_ast_block_type_if,
	mirac_ast_block_type_elif,
	mirac_ast_block_type_else,
	mirac_ast_block_type_loop,

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
		mirac_ast_block_as_s    as_block;
		mirac_ast_block_scope_s scope_block;
		mirac_ast_block_if_s    if_block;
		mirac_ast_block_elif_s  elif_block;
		mirac_ast_block_else_s  else_block;
		mirac_ast_block_loop_s  loop_block;
	} as;
};

typedef struct
{
	mirac_token_s identifier;
	mirac_token_list_s req_tokens;
	mirac_token_list_s ret_tokens;
	mirac_ast_block_scope_s body_scope;
	bool is_entry;
} mirac_ast_def_func_s;

typedef struct
{
	mirac_token_s identifier;
	mirac_token_s capacity;
} mirac_ast_def_mem_s;

typedef struct
{
	mirac_token_s identifier;
	mirac_token_s literal;
} mirac_ast_def_str_s;

typedef enum
{
	mirac_ast_def_type_func = 0,
	mirac_ast_def_type_mem,
	mirac_ast_def_type_str,
	mirac_ast_def_type_none
} mirac_ast_def_type_e;

// TODO: document!
mirac_string_view_s mirac_ast_def_type_to_string_view(
	const mirac_ast_def_type_e type);

struct mirac_ast_def_s
{
	mirac_token_s section;
	mirac_ast_def_type_e type;
	bool is_used;

	union
	{
		mirac_ast_def_func_s func_def;
		mirac_ast_def_mem_s mem_def;
		mirac_ast_def_str_s str_def;
	} as;
};

typedef struct
{
	mirac_ast_def_list_s defs;
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
