
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

mirac_implement_heap_array_type(mirac_ast_blocks_vector, mirac_ast_block_s);
mirac_implement_heap_array_type(mirac_ast_blocks_refs_vector, mirac_ast_block_s*);

static bool is_token_valid_expression_token(
	mirac_token_s* const token);

static bool is_token_valid_expression_token(
	mirac_token_s* const token)
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
		(mirac_token_type_reserved_sub  == token->type) ||
		(mirac_token_type_reserved_div  == token->type) ||
		(mirac_token_type_reserved_ls   == token->type) ||
		(mirac_token_type_reserved_shl  == token->type) ||
		(mirac_token_type_reserved_lseq == token->type) ||
		(mirac_token_type_reserved_eq   == token->type) ||
		(mirac_token_type_reserved_gt   == token->type) ||
		(mirac_token_type_reserved_gteq == token->type) ||
		(mirac_token_type_reserved_shr  == token->type) ||
		(mirac_token_type_reserved_drop == token->type) ||
		(mirac_token_type_reserved_dup  == token->type) ||
		(mirac_token_type_reserved_over == token->type) ||
		(mirac_token_type_reserved_rot  == token->type) ||
		(mirac_token_type_reserved_swap == token->type) ||
		(mirac_token_type_reserved_bxor == token->type) ||
		(mirac_token_type_reserved_lxor == token->type) ||
		(mirac_token_type_reserved_as   == token->type) ||
		(mirac_token_type_reserved_do   == token->type) ||
		(mirac_token_type_reserved_elif == token->type) ||
		(mirac_token_type_reserved_else == token->type) ||
		(mirac_token_type_reserved_end  == token->type) ||
		(mirac_token_type_reserved_f32  == token->type) ||
		(mirac_token_type_reserved_f64  == token->type) ||
		(mirac_token_type_reserved_i08  == token->type) ||
		(mirac_token_type_reserved_i16  == token->type) ||
		(mirac_token_type_reserved_i32  == token->type) ||
		(mirac_token_type_reserved_i64  == token->type) ||
		(mirac_token_type_reserved_if   == token->type) ||
		(mirac_token_type_reserved_ld08 == token->type) ||
		(mirac_token_type_reserved_ld16 == token->type) ||
		(mirac_token_type_reserved_ld32 == token->type) ||
		(mirac_token_type_reserved_ld64 == token->type) ||
		(mirac_token_type_reserved_let  == token->type) ||
		(mirac_token_type_reserved_loop == token->type) ||
		(mirac_token_type_reserved_ptr  == token->type) ||
		(mirac_token_type_reserved_reg  == token->type) ||
		(mirac_token_type_reserved_req  == token->type) ||
		(mirac_token_type_reserved_ret  == token->type) ||
		(mirac_token_type_reserved_st08 == token->type) ||
		(mirac_token_type_reserved_st16 == token->type) ||
		(mirac_token_type_reserved_st32 == token->type) ||
		(mirac_token_type_reserved_st64 == token->type) ||
		(mirac_token_type_reserved_sys1 == token->type) ||
		(mirac_token_type_reserved_sys2 == token->type) ||
		(mirac_token_type_reserved_sys3 == token->type) ||
		(mirac_token_type_reserved_sys4 == token->type) ||
		(mirac_token_type_reserved_sys5 == token->type) ||
		(mirac_token_type_reserved_sys6 == token->type) ||
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



static void try_parse_ast_expression_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block);

static void try_parse_ast_branching_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block);

static void try_parse_ast_loop_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block);

static void try_parse_ast_let_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block);

static void try_parse_ast_reg_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block);

static void try_parse_ast_function_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block);

static void try_parse_ast_memory_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block);

static void try_parse_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block);

static void try_parse_ast_expression_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_block != NULL);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	// TODO: remove(1):
	mirac_logger_debug(" ! " mirac_sv_fmt, mirac_sv_arg(mirac_token_to_string_view(&token)));

	ast_block->type = mirac_ast_block_type_expression;
	ast_block->as_expression_block = (mirac_ast_expression_block_s)
	{
		.token = token
	};
}

static void try_parse_ast_branching_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_block != NULL);

	// TODO: try parse - if ... do {block} end
	// TODO: try parse - if ... do {block} elif ... do {block} end
	// TODO: try parse - if ... do {block} else {block} end
	// TODO: try parse - if ... do {block} elif ... do {block} else {block} end
}

static void try_parse_ast_loop_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_block != NULL);

	// TODO: try parse - loop ... do {block} end

	ast_block->type = mirac_ast_block_type_loop;
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_reserved_loop)
	{
		mirac_logger_error("AAA1");
		mirac_c_exit(-1);
	}

	ast_block->as_loop_block.loop_token = token;
	ast_block->as_loop_block.condition_blocks = mirac_ast_blocks_vector_from_parts(parser->arena, 4);

	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
	{
		// TODO: remove(1):
		mirac_logger_debug(" - " mirac_sv_fmt, mirac_sv_arg(mirac_token_to_string_view(&token)));
		(void)getchar();

		if (mirac_token_type_reserved_do == token.type)
		{
			break;
		}

		mirac_ast_block_s condition_block;
		// mirac_lexer_unlex(parser->lexer, &token);
		try_parse_ast_block(parser, &condition_block);
		mirac_ast_blocks_vector_push(&ast_block->as_loop_block.condition_blocks, condition_block);
	}

	if (token.type != mirac_token_type_reserved_do)
	{
		// TODO: remove(1):
		mirac_logger_debug(" + " mirac_sv_fmt, mirac_sv_arg(mirac_token_to_string_view(&token)));

		mirac_logger_error("AAA2");
		mirac_c_exit(-1);
	}

	ast_block->as_loop_block.do_token = token;
	ast_block->as_loop_block.body_blocks = mirac_ast_blocks_vector_from_parts(parser->arena, 4);

	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
	{
		if (mirac_token_type_reserved_end == token.type)
		{
			break;
		}

		mirac_ast_block_s body_block;
		try_parse_ast_block(parser, &body_block);
		mirac_ast_blocks_vector_push(&ast_block->as_loop_block.body_blocks, body_block);
	}

	if (token.type != mirac_token_type_reserved_end)
	{
		mirac_logger_error("AAA3");
		mirac_c_exit(-1);
	}

	ast_block->as_loop_block.end_token = token;
}

static void try_parse_ast_let_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_block != NULL);
}

static void try_parse_ast_reg_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_block != NULL);
}

static void try_parse_ast_function_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_block != NULL);
}

static void try_parse_ast_memory_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_block != NULL);
}

static void try_parse_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const ast_block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_block != NULL);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
	{
		mirac_lexer_unlex(parser->lexer, &token);

		switch (token.type)
		{
			case mirac_token_type_reserved_if:
			{
				try_parse_ast_branching_block(parser, ast_block);
			} break;

			case mirac_token_type_reserved_loop:
			{
				try_parse_ast_loop_block(parser, ast_block);
			} break;

			case mirac_token_type_reserved_let:
			{
				try_parse_ast_let_block(parser, ast_block);
			} break;

			case mirac_token_type_reserved_reg:
			{
				try_parse_ast_reg_block(parser, ast_block);
			} break;

			case mirac_token_type_reserved_inl:
			case mirac_token_type_reserved_func:
			{
				try_parse_ast_function_block(parser, ast_block);
			} break;

			case mirac_token_type_reserved_mem:
			{
				try_parse_ast_memory_block(parser, ast_block);
			} break;

			default:
			{
				if (is_token_valid_expression_token(&token))
				{
					try_parse_ast_expression_block(parser, ast_block);
				}
				else
				{
					// TODO: error!
					mirac_logger_error(mirac_sv_fmt, mirac_sv_arg(mirac_token_to_string_view(&token)));
					mirac_logger_error("BAD BAD very BAD!");
					mirac_c_exit(-1);
				}
			} break;
		}
	}
}

mirac_ast_unit_s mirac_ast_unit_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_unit_s ast_unit = {0};
	ast_unit.globals = mirac_ast_blocks_vector_from_parts(arena, 4);
	ast_unit.strings = mirac_ast_blocks_refs_vector_from_parts(arena, 4);
	return ast_unit;
}

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
	mirac_ast_unit_s ast_unit = mirac_ast_unit_from_parts(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
	{
		// TODO: remove(1):
		mirac_logger_debug(mirac_sv_fmt, mirac_sv_arg(mirac_token_to_string_view(&token)));

		mirac_lexer_unlex(parser->lexer, &token);
		mirac_ast_block_s ast_block; // TODO: constructor <!
		try_parse_ast_block(parser, &ast_block);

		mirac_ast_blocks_vector_push(&ast_unit.globals, ast_block);
	}

	return ast_unit;
}
