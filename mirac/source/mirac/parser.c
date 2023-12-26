
/**
 * @file parser.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-15
 */

#include <mirac/parser.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

mirac_implement_heap_array_type(mirac_tokens_refs_vector, mirac_token_s*);
mirac_implement_heap_array_type(mirac_blocks_vector, mirac_ast_block_s);

#define log_parser_error_and_exit(_location, _format, ...)                     \
	do                                                                         \
	{                                                                          \
		(void)fprintf(stderr, mirac_sv_fmt ":%lu:%lu: ",                       \
			mirac_sv_arg((_location).file),                                    \
			(_location).line,                                                  \
			(_location).column);                                               \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		mirac_c_exit(-1);                                                      \
	} while (0)

static bool is_token_expression_block_token(
	const mirac_token_s* const token);

static mirac_ast_block_expr_s create_ast_block_expr(
	void);

static mirac_ast_block_begin_scope_s create_ast_block_begin_scope(
	void);

static mirac_ast_block_end_scope_s create_ast_block_end_scope(
	void);

static mirac_ast_block_end_unit_s create_ast_block_end_unit(
	void);

static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena);

static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena);

static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena);

static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena);

static mirac_ast_block_let_s create_ast_block_let(
	mirac_arena_s* const arena);

static mirac_ast_block_func_s create_ast_block_func(
	mirac_arena_s* const arena);

static mirac_ast_block_mem_s create_ast_block_mem(
	void);

static mirac_ast_block_s create_ast_block_any(
	void);

static mirac_ast_unit_s create_ast_unit(
	mirac_arena_s* const arena);

static void print_ast_block_expr(
	const mirac_ast_block_expr_s* const expr_block,
	const uint8_t indent);

static void print_ast_block_begin_scope(
	const mirac_ast_block_begin_scope_s* const begin_scope_block,
	const uint8_t indent);

static void print_ast_block_end_scope(
	const mirac_ast_block_end_scope_s* const end_scope_block,
	const uint8_t indent);

static void print_ast_block_end_unit(
	const mirac_ast_block_end_unit_s* const end_unit_block,
	const uint8_t indent);

static void print_ast_block_if(
	const mirac_ast_block_if_s* const if_block,
	const uint8_t indent);

static void print_ast_block_elif(
	const mirac_ast_block_elif_s* const elif_block,
	const uint8_t indent);

static void print_ast_block_else(
	const mirac_ast_block_else_s* const else_block,
	const uint8_t indent);

static void print_ast_block_loop(
	const mirac_ast_block_loop_s* const loop_block,
	const uint8_t indent);

static void print_ast_block_let(
	const mirac_ast_block_let_s* const let_block,
	const uint8_t indent);

static void print_ast_block_func(
	const mirac_ast_block_func_s* const func_block,
	const uint8_t indent);

static void print_ast_block_mem(
	const mirac_ast_block_mem_s* const mem_block,
	const uint8_t indent);

static void print_ast_block_any(
	const mirac_ast_block_s* const block,
	const uint8_t indent);

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser);

static mirac_ast_block_begin_scope_s parse_ast_block_begin_scope(
	mirac_parser_s* const parser);

static mirac_ast_block_end_scope_s parse_ast_block_end_scope(
	mirac_parser_s* const parser);

static mirac_ast_block_end_unit_s parse_ast_block_end_unit(
	mirac_parser_s* const parser);

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser);

static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser);

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser);

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser);

static mirac_ast_block_let_s parse_ast_block_let(
	mirac_parser_s* const parser);

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser);

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser);

static mirac_ast_block_s parse_ast_block_any(
	mirac_parser_s* const parser);

static void parse_ast_unit(
	mirac_parser_s* const parser,
	mirac_ast_unit_s* const ast_unit);

mirac_parser_s mirac_parser_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_lexer_s* const lexer)
{
	mirac_debug_assert(config != NULL);
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(lexer != NULL);
	mirac_parser_s parser = {0};
	parser.config = config;
	parser.arena = arena;
	parser.lexer = lexer;
	return parser;
}

mirac_ast_unit_s mirac_parser_parse_ast_unit(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_unit_s ast_unit = create_ast_unit(parser->arena);
	ast_unit.identifier = parser->lexer->file_path;
	parse_ast_unit(parser, &ast_unit);
	return ast_unit;
}

void mirac_parser_print_ast_unit(
	const mirac_ast_unit_s* const ast_unit)
{
	mirac_debug_assert(ast_unit != NULL);
	uint8_t indent = 0;

	for (uint64_t index = 0; index < ast_unit->blocks.count; ++index)
	{
		print_ast_block_any(&(ast_unit->blocks.data[index]), indent);
	}
}

static bool is_token_expression_block_token(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	return (
		(mirac_token_type_reserved_lnot == token->type) ||
		(mirac_token_type_reserved_neq  == token->type) ||
		(mirac_token_type_reserved_mod  == token->type) ||
		(mirac_token_type_reserved_band == token->type) ||
		(mirac_token_type_reserved_land == token->type) ||
		(mirac_token_type_reserved_mul  == token->type) ||
		(mirac_token_type_reserved_add  == token->type) ||
		(mirac_token_type_reserved_inc  == token->type) ||
		(mirac_token_type_reserved_sub  == token->type) ||
		(mirac_token_type_reserved_dec  == token->type) ||
		(mirac_token_type_reserved_div  == token->type) ||
		(mirac_token_type_reserved_ls   == token->type) ||
		(mirac_token_type_reserved_shl  == token->type) ||
		(mirac_token_type_reserved_lseq == token->type) ||
		(mirac_token_type_reserved_eq   == token->type) ||
		(mirac_token_type_reserved_gt   == token->type) ||
		(mirac_token_type_reserved_gteq == token->type) ||
		(mirac_token_type_reserved_shr  == token->type) ||
		(mirac_token_type_reserved_drop == token->type) ||
		(mirac_token_type_reserved_sys1 == token->type) ||
		(mirac_token_type_reserved_sys2 == token->type) ||
		(mirac_token_type_reserved_sys3 == token->type) ||
		(mirac_token_type_reserved_sys4 == token->type) ||
		(mirac_token_type_reserved_sys5 == token->type) ||
		(mirac_token_type_reserved_sys6 == token->type) ||
		(mirac_token_type_reserved_st08 == token->type) ||
		(mirac_token_type_reserved_st16 == token->type) ||
		(mirac_token_type_reserved_st32 == token->type) ||
		(mirac_token_type_reserved_st64 == token->type) ||
		(mirac_token_type_reserved_ld08 == token->type) ||
		(mirac_token_type_reserved_ld16 == token->type) ||
		(mirac_token_type_reserved_ld32 == token->type) ||
		(mirac_token_type_reserved_ld64 == token->type) ||
		(mirac_token_type_reserved_dup  == token->type) ||
		(mirac_token_type_reserved_over == token->type) ||
		(mirac_token_type_reserved_rot  == token->type) ||
		(mirac_token_type_reserved_swap == token->type) ||
		(mirac_token_type_reserved_bxor == token->type) ||
		(mirac_token_type_reserved_lxor == token->type) ||
		(mirac_token_type_reserved_as   == token->type) ||
		(mirac_token_type_reserved_f32  == token->type) ||
		(mirac_token_type_reserved_f64  == token->type) ||
		(mirac_token_type_reserved_i08  == token->type) ||
		(mirac_token_type_reserved_i16  == token->type) ||
		(mirac_token_type_reserved_i32  == token->type) ||
		(mirac_token_type_reserved_i64  == token->type) ||
		(mirac_token_type_reserved_ptr  == token->type) ||
		(mirac_token_type_reserved_u08  == token->type) ||
		(mirac_token_type_reserved_u16  == token->type) ||
		(mirac_token_type_reserved_u32  == token->type) ||
		(mirac_token_type_reserved_u64  == token->type) ||
		(mirac_token_type_reserved_bor  == token->type) ||
		(mirac_token_type_reserved_lor  == token->type) ||
		(mirac_token_type_reserved_bnot == token->type) ||
		(mirac_token_type_literal_i08   == token->type) ||
		(mirac_token_type_literal_i16   == token->type) ||
		(mirac_token_type_literal_i32   == token->type) ||
		(mirac_token_type_literal_i64   == token->type) ||
		(mirac_token_type_literal_u08   == token->type) ||
		(mirac_token_type_literal_u16   == token->type) ||
		(mirac_token_type_literal_u32   == token->type) ||
		(mirac_token_type_literal_u64   == token->type) ||
		(mirac_token_type_literal_f32   == token->type) ||
		(mirac_token_type_literal_f64   == token->type) ||
		(mirac_token_type_literal_ptr   == token->type) ||
		(mirac_token_type_literal_str   == token->type) ||
		(mirac_token_type_identifier    == token->type)
	);
}

static mirac_ast_block_expr_s create_ast_block_expr(
	void)
{
	mirac_ast_block_expr_s expr_block = {0};
	return expr_block;
}

static mirac_ast_block_begin_scope_s create_ast_block_begin_scope(
	void)
{
	mirac_ast_block_begin_scope_s begin_scope_block = {0};
	return begin_scope_block;
}

static mirac_ast_block_end_scope_s create_ast_block_end_scope(
	void)
{
	mirac_ast_block_end_scope_s end_scope_block = {0};
	return end_scope_block;
}

static mirac_ast_block_end_unit_s create_ast_block_end_unit(
	void)
{
	mirac_ast_block_end_unit_s end_unit_block = {0};
	return end_unit_block;
}

static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_if_s if_block = {0};
	if_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	if_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return if_block;
}

static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_elif_s elif_block = {0};
	elif_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	elif_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return elif_block;
}

static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_else_s else_block = {0};
	else_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return else_block;
}

static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_loop_s loop_block = {0};
	loop_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	loop_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return loop_block;
}

static mirac_ast_block_let_s create_ast_block_let(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_let_s let_block = {0};
	let_block.identifier_blocks = mirac_blocks_vector_from_parts(arena, 1);
	let_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return let_block;
}

static mirac_ast_block_func_s create_ast_block_func(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_func_s func_block = {0};
	func_block.req_blocks = mirac_blocks_vector_from_parts(arena, 1);
	func_block.ret_blocks = mirac_blocks_vector_from_parts(arena, 1);
	func_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return func_block;
}

static mirac_ast_block_mem_s create_ast_block_mem(
	void)
{
	mirac_ast_block_mem_s mem_block = {0};
	return mem_block;
}

static mirac_ast_block_s create_ast_block_any(
	void)
{
	mirac_ast_block_s block = {0};
	return block;
}

static mirac_ast_unit_s create_ast_unit(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_unit_s ast_unit = {0};
	ast_unit.blocks = mirac_blocks_vector_from_parts(arena, 1);
	ast_unit.strings = mirac_tokens_refs_vector_from_parts(arena, 1);
	return ast_unit;
}

static void print_ast_block_expr(
	const mirac_ast_block_expr_s* const expr_block,
	const uint8_t indent)
{
	mirac_debug_assert(expr_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_expr\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&expr_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_begin_scope(
	const mirac_ast_block_begin_scope_s* const begin_scope_block,
	const uint8_t indent)
{
	mirac_debug_assert(begin_scope_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_begin_scope\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&begin_scope_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_end_scope(
	const mirac_ast_block_end_scope_s* const end_scope_block,
	const uint8_t indent)
{
	mirac_debug_assert(end_scope_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_end_scope\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&end_scope_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_end_unit(
	const mirac_ast_block_end_unit_s* const end_unit_block,
	const uint8_t indent)
{
	mirac_debug_assert(end_unit_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_end_unit\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&end_unit_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_if(
	const mirac_ast_block_if_s* const if_block,
	const uint8_t indent)
{
	mirac_debug_assert(if_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_if\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("condition_blocks:\n");
	for (uint64_t index = 0; index < if_block->condition_blocks.count; ++index)
		print_ast_block_any(&if_block->condition_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < if_block->body_blocks.count; ++index)
		print_ast_block_any(&if_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_elif(
	const mirac_ast_block_elif_s* const elif_block,
	const uint8_t indent)
{
	mirac_debug_assert(elif_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_elif\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("condition_blocks:\n");
	for (uint64_t index = 0; index < elif_block->condition_blocks.count; ++index)
		print_ast_block_any(&elif_block->condition_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < elif_block->body_blocks.count; ++index)
		print_ast_block_any(&elif_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_else(
	const mirac_ast_block_else_s* const else_block,
	const uint8_t indent)
{
	mirac_debug_assert(else_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_else\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < else_block->body_blocks.count; ++index)
		print_ast_block_any(&else_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_loop(
	const mirac_ast_block_loop_s* const loop_block,
	const uint8_t indent)
{
	mirac_debug_assert(loop_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_loop\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("condition_blocks:\n");
	for (uint64_t index = 0; index < loop_block->condition_blocks.count; ++index)
		print_ast_block_any(&loop_block->condition_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < loop_block->body_blocks.count; ++index)
		print_ast_block_any(&loop_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_let(
	const mirac_ast_block_let_s* const let_block,
	const uint8_t indent)
{
	mirac_debug_assert(let_block != NULL);
	// TODO: implement!
	mirac_debug_assert(!"print_ast_block_let() is not implemented yet!");
	(void)indent;
}

static void print_ast_block_func(
	const mirac_ast_block_func_s* const func_block,
	const uint8_t indent)
{
	mirac_debug_assert(func_block != NULL);
	// TODO: implement!
	mirac_debug_assert(!"print_ast_block_func() is not implemented yet!");
	(void)indent;
}

static void print_ast_block_mem(
	const mirac_ast_block_mem_s* const mem_block,
	const uint8_t indent)
{
	mirac_debug_assert(mem_block != NULL);
	// TODO: implement!
	mirac_debug_assert(!"print_ast_block_mem() is not implemented yet!");
	(void)indent;
}

static void print_ast_block_any(
	const mirac_ast_block_s* const block,
	const uint8_t indent)
{
	mirac_debug_assert(block != NULL);

	switch (block->type)
	{
		case mirac_ast_block_type_expr:
		{
			print_ast_block_expr(&block->as.expr_block, indent);
		} break;

		case mirac_ast_block_type_begin_scope:
		{
			print_ast_block_begin_scope(&block->as.begin_scope_block, indent);
		} break;

		case mirac_ast_block_type_end_scope:
		{
			print_ast_block_end_scope(&block->as.end_scope_block, indent);
		} break;

		case mirac_ast_block_type_end_unit:
		{
			print_ast_block_end_unit(&block->as.end_unit_block, indent);
		} break;

		case mirac_ast_block_type_if:
		{
			print_ast_block_if(&block->as.if_block, indent);
		} break;

		case mirac_ast_block_type_elif:
		{
			print_ast_block_elif(&block->as.elif_block, indent);
		} break;

		case mirac_ast_block_type_else:
		{
			print_ast_block_else(&block->as.else_block, indent);
		} break;

		case mirac_ast_block_type_loop:
		{
			print_ast_block_loop(&block->as.loop_block, indent);
		} break;

		case mirac_ast_block_type_let:
		{
			print_ast_block_let(&block->as.let_block, indent);
		} break;

		case mirac_ast_block_type_func:
		{
			print_ast_block_func(&block->as.func_block, indent);
		} break;

		case mirac_ast_block_type_mem:
		{
			print_ast_block_mem(&block->as.mem_block, indent);
		} break;

		default:
		{
			mirac_debug_assert(!"Should never reach this block.");
		} break;
	}
}

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_expr_s expr_block = create_ast_block_expr();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (!is_token_expression_block_token(&token))
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'expr' block.",
			mirac_sv_arg(token.text)
		);
	}

	// TODO: figure out if I need separate block for casts (as) since they are
	//       only for the type checker!

	expr_block.token = token;
	return expr_block;
}

static mirac_ast_block_begin_scope_s parse_ast_block_begin_scope(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_begin_scope_s begin_scope_block = create_ast_block_begin_scope();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_lexer_should_stop_lexing(token.type) ||
		(mirac_token_type_reserved_left_brace != token.type))
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'begin-scope' block.",
			mirac_sv_arg(token.text)
		);
	}

	begin_scope_block.token = token;
	return begin_scope_block;
}

static mirac_ast_block_end_scope_s parse_ast_block_end_scope(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_end_scope_s end_scope_block = create_ast_block_end_scope();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_lexer_should_stop_lexing(token.type) ||
		(mirac_token_type_reserved_right_brace != token.type))
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'end-scope' block.",
			mirac_sv_arg(token.text)
		);
	}

	end_scope_block.token = token;
	return end_scope_block;
}

static mirac_ast_block_end_unit_s parse_ast_block_end_unit(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_end_unit_s end_unit_block = create_ast_block_end_unit();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_eof)
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'end-unit' block.",
			mirac_sv_arg(token.text)
		);
	}

	end_unit_block.token = token;
	return end_unit_block;
}

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_if_s if_block = create_ast_block_if(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_if == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block_any(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'if' block's condition."
				);
			} break;

			case mirac_ast_block_type_begin_scope:
			{
				goto after_if_condition_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&if_block.condition_blocks, block);
			} break;
		}
	}

after_if_condition_blocks:
	while (1)
	{
		mirac_ast_block_s block = parse_ast_block_any(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'if' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_if_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&if_block.body_blocks, block);
			} break;
		}
	}

after_if_body_blocks:
	return if_block;
}

static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_elif_s elif_block = create_ast_block_elif(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_elif == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block_any(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'elif' block's condition."
				);
			} break;

			case mirac_ast_block_type_begin_scope:
			{
				goto after_elif_condition_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&elif_block.condition_blocks, block);
			} break;
		}
	}

after_elif_condition_blocks:
	while (1)
	{
		mirac_ast_block_s block = parse_ast_block_any(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'elif' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_elif_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&elif_block.body_blocks, block);
			} break;
		}
	}

after_elif_body_blocks:
	return elif_block;
}

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_else_s else_block = create_ast_block_else(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_else == token.type);

	if (mirac_lexer_lex_next(parser->lexer, &token) != mirac_token_type_reserved_left_brace)
	{
		log_parser_error_and_exit(token.location,
			"expected '{' token after 'else' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block_any(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'else' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_else_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&else_block.body_blocks, block);
			} break;
		}
	}

after_else_body_blocks:
	return else_block;
}

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_loop_s loop_block = create_ast_block_loop(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_loop == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block_any(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'loop' block's condition."
				);
			} break;

			case mirac_ast_block_type_begin_scope:
			{
				goto after_loop_condition_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&loop_block.condition_blocks, block);
			} break;
		}
	}

after_loop_condition_blocks:
	while (1)
	{
		mirac_ast_block_s block = parse_ast_block_any(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'loop' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_loop_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&loop_block.body_blocks, block);
			} break;
		}
	}

after_loop_body_blocks:
	return loop_block;
}

static mirac_ast_block_let_s parse_ast_block_let(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_let_s let_block = create_ast_block_let(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_let == token.type);

	// TODO: implement!
	mirac_debug_assert(!"parse_ast_block_let() is not implemented yet!");
	return let_block;
}

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_func_s func_block = create_ast_block_func(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert((mirac_token_type_reserved_inl  == token.type)
					|| (mirac_token_type_reserved_func == token.type));

	// TODO: implement!
	mirac_debug_assert(!"parse_ast_block_func() is not implemented yet!");
	return func_block;
}

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_mem_s mem_block = create_ast_block_mem();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_mem == token.type);

	// TODO: implement!
	mirac_debug_assert(!"parse_ast_block_mem() is not implemented yet!");
	return mem_block;
}

static mirac_ast_block_s parse_ast_block_any(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_s block = create_ast_block_any();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (!mirac_lexer_should_stop_lexing(token.type))
	{
		mirac_lexer_unlex(parser->lexer, &token);

		/*
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'any' block.",
			mirac_sv_arg(token.text)
		);
		*/
	}


	switch (token.type)
	{
		case mirac_token_type_reserved_left_brace:
		{
			block.type = mirac_ast_block_type_begin_scope;
			block.as.begin_scope_block = parse_ast_block_begin_scope(parser);
		} break;

		case mirac_token_type_reserved_right_brace:
		{
			block.type = mirac_ast_block_type_end_scope;
			block.as.end_scope_block = parse_ast_block_end_scope(parser);
		} break;

		case mirac_token_type_eof:
		{
			block.type = mirac_ast_block_type_end_unit;
			block.as.end_unit_block = parse_ast_block_end_unit(parser);
		} break;

		case mirac_token_type_reserved_if:
		{
			block.type = mirac_ast_block_type_if;
			block.as.if_block = parse_ast_block_if(parser);
		} break;

		case mirac_token_type_reserved_elif:
		{
			block.type = mirac_ast_block_type_elif;
			block.as.elif_block = parse_ast_block_elif(parser);
		} break;

		case mirac_token_type_reserved_else:
		{
			block.type = mirac_ast_block_type_else;
			block.as.else_block = parse_ast_block_else(parser);
		} break;

		case mirac_token_type_reserved_loop:
		{
			block.type = mirac_ast_block_type_loop;
			block.as.loop_block = parse_ast_block_loop(parser);
		} break;

		case mirac_token_type_reserved_let:
		{
			block.type = mirac_ast_block_type_let;
			block.as.let_block = parse_ast_block_let(parser);
		} break;

		case mirac_token_type_reserved_inl:
		case mirac_token_type_reserved_func:
		{
			block.type = mirac_ast_block_type_func;
			block.as.func_block = parse_ast_block_func(parser);
		} break;

		case mirac_token_type_reserved_mem:
		{
			block.type = mirac_ast_block_type_mem;
			block.as.mem_block = parse_ast_block_mem(parser);
		} break;

		default:
		{
			block.type = mirac_ast_block_type_expr;
			block.as.expr_block = parse_ast_block_expr(parser);
		} break;
	}

	return block;
}

static void parse_ast_unit(
	mirac_parser_s* const parser,
	mirac_ast_unit_s* const ast_unit)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_unit != NULL);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block_any(parser);
		// TODO: remove:
		// [
		// print_ast_block_any(&block, 0);
		// ]
		if (mirac_ast_block_type_end_unit == block.type) { break; }
		mirac_blocks_vector_push(&ast_unit->blocks, block);
	}
}
