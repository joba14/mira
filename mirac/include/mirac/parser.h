
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
	mirac_token_s token;
	mirac_ast_def_s* def;
} mirac_ast_block_ident_s;

typedef struct
{
	mirac_ast_block_s* ident; // NOTE: Must be ident block.
} mirac_ast_block_call_s;

typedef struct
{
	mirac_token_list_s type_tokens;
} mirac_ast_block_as_s;

typedef enum
{
	mirac_ast_block_scope_type_parentheses = 0,
	mirac_ast_block_scope_type_brackets,
	mirac_ast_block_scope_type_braces,
} mirac_ast_block_scope_type_e;

// TODO: Write unit tests!
/**
 * @brief Stringify ast scope block type and return the string view.
 * 
 * @param type[in] ast scope block type to stringify
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_ast_block_scope_type_to_string_view(
	const mirac_ast_block_scope_type_e type);

typedef struct
{
	mirac_ast_block_scope_type_e type;
	mirac_ast_block_list_s blocks;
} mirac_ast_block_scope_s;

typedef struct
{
	mirac_ast_block_s* cond; // NOTE: Must be scope block.
	mirac_ast_block_s* body; // NOTE: Must be scope block.
	mirac_ast_block_s* next;
	uint64_t index;
} mirac_ast_block_if_s;

typedef struct
{
	mirac_ast_block_s* body; // NOTE: Must be scope block.
	uint64_t index;
} mirac_ast_block_else_s;

typedef struct
{
	mirac_ast_block_s* cond; // NOTE: Must be scope block.
	mirac_ast_block_s* body; // NOTE: Must be scope block.
	uint64_t index;
} mirac_ast_block_loop_s;

typedef enum
{
	mirac_ast_block_type_expr = 0,
	mirac_ast_block_type_ident,
	mirac_ast_block_type_call,
	mirac_ast_block_type_as,
	mirac_ast_block_type_scope,
	mirac_ast_block_type_if,
	mirac_ast_block_type_else,
	mirac_ast_block_type_loop,

	mirac_ast_block_type_eou,
	mirac_ast_block_type_none
} mirac_ast_block_type_e;

// TODO: Write unit tests!
/**
 * @brief Stringify ast block type and return the string view.
 * 
 * @param type[in] ast block type to stringify
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_ast_block_type_to_string_view(
	const mirac_ast_block_type_e type);

struct mirac_ast_block_s
{
	mirac_location_s location;
	mirac_ast_block_type_e type;

	union
	{
		mirac_ast_block_expr_s  expr_block;
		mirac_ast_block_ident_s ident_block;
		mirac_ast_block_call_s  call_block;
		mirac_ast_block_as_s    as_block;
		mirac_ast_block_scope_s scope_block;
		mirac_ast_block_if_s    if_block;
		mirac_ast_block_else_s  else_block;
		mirac_ast_block_loop_s  loop_block;
	} as;
};

typedef struct
{
	mirac_token_s identifier;
	mirac_token_list_s req_tokens;
	mirac_token_list_s ret_tokens;
	mirac_ast_block_s* body; // NOTE: Must be scope block.
	bool is_entry;
	uint64_t index;
} mirac_ast_def_func_s;

typedef struct
{
	mirac_token_s identifier;
	mirac_token_s capacity;
	uint64_t index;
} mirac_ast_def_mem_s;

typedef struct
{
	mirac_token_s identifier;
	mirac_token_s literal;
	uint64_t index;
} mirac_ast_def_str_s;

typedef enum
{
	mirac_ast_def_type_func = 0,
	mirac_ast_def_type_mem,
	mirac_ast_def_type_str,
	mirac_ast_def_type_none
} mirac_ast_def_type_e;

// TODO: Write unit tests!
/**
 * @brief Stringify ast def type and return the string view.
 * 
 * @param type[in] ast def type to stringify
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_ast_def_type_to_string_view(
	const mirac_ast_def_type_e type);

struct mirac_ast_def_s
{
	mirac_location_s location;
	mirac_token_s section;
	mirac_ast_def_type_e type;
	bool is_global;
	bool is_used;

	union
	{
		mirac_ast_def_func_s func_def;
		mirac_ast_def_mem_s mem_def;
		mirac_ast_def_str_s str_def;
	} as;
};

// TODO: Write unit tests!
/**
 * @brief Get identifier token of a provided st def structure.
 * 
 * @param def[in] ast def to fetch identifier token from
 * 
 * @return mirac_token_s
 */
mirac_token_s mirac_ast_def_get_identifier_token(
	const mirac_ast_def_s* const def);

typedef struct
{
	mirac_ast_def_list_s defs;
} mirac_ast_unit_s;

// TODO: Write unit tests!
/**
 * @brief Create ast unit with provided arena.
 * 
 * @param arena[in] arena reference
 * 
 * @return mirac_ast_unit_s
 */
mirac_ast_unit_s mirac_ast_unit_from_parts(
	mirac_arena_s* const arena);

/**
 * @brief Print ast unit.
 * 
 * @param unit[in]   ast unit to print
 * @param indent[in] indent from left side (in tabs)
 */
void mirac_ast_unit_print(
	const mirac_ast_unit_s* const unit,
	const uint64_t indent);

typedef struct
{
	uint64_t if_count;
	uint64_t elif_count;
	uint64_t else_count;
	uint64_t loop_count;
	uint64_t func_count;
	uint64_t mem_count;
	uint64_t str_count;
} mirac_ast_statistics_s;

typedef struct
{
	mirac_config_s* config;
	mirac_arena_s* arena;
	mirac_lexer_s* lexer;
	mirac_ast_statistics_s stats;
	mirac_ast_unit_s unit;
} mirac_parser_s;

// TODO: Write unit tests!
/**
 * @brief Create parser from config, arena, and lexer.
 * 
 * @param config[in] config reference
 * @param arena[in]  arena reference
 * @param lexer[in]  lexer reference
 * 
 * @return mirac_parser_s
 */
mirac_parser_s mirac_parser_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_lexer_s* const lexer);

// TODO: Write unit tests!
/**
 * @brief Destroy parser.
 * 
 * @param parser[in/out] parser to destroy
 */
void mirac_parser_destroy(
	mirac_parser_s* const parser);

// TODO: Write unit tests!
/**
 * @brief Parse ast unit.
 * 
 * @param parser[in] parser reference
 * 
 * @return mirac_ast_unit_s
 */
mirac_ast_unit_s mirac_parser_parse_ast_unit(
	mirac_parser_s* const parser);

#endif
