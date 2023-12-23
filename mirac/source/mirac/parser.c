
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

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser);

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser);

static void parse_ast_unit(
	mirac_parser_s* const parser,
	mirac_ast_unit_s* const ast_unit);

mirac_ast_block_expr_s mirac_ast_block_expr_from_parts(
	void)
{
	mirac_ast_block_expr_s expr_block = {0};
	return expr_block;
}

mirac_ast_block_if_s mirac_ast_block_if_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_if_s if_block = {0};
	if_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	if_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return if_block;
}

mirac_ast_block_elif_s mirac_ast_block_elif_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_elif_s elif_block = {0};
	elif_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	elif_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return elif_block;
}

mirac_ast_block_else_s mirac_ast_block_else_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_else_s else_block = {0};
	else_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return else_block;
}

mirac_ast_block_loop_s mirac_ast_block_loop_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_loop_s loop_block = {0};
	loop_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	loop_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return loop_block;
}

mirac_ast_block_let_s mirac_ast_block_let_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_let_s let_block = {0};
	let_block.identifier_blocks = mirac_blocks_vector_from_parts(arena, 1);
	let_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return let_block;
}

mirac_ast_block_func_s mirac_ast_block_func_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_func_s func_block = {0};
	func_block.req_blocks = mirac_blocks_vector_from_parts(arena, 1);
	func_block.ret_blocks = mirac_blocks_vector_from_parts(arena, 1);
	func_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return func_block;
}

mirac_ast_block_mem_s mirac_ast_block_mem_from_parts(
	void)
{
	mirac_ast_block_mem_s mem_block = {0};
	return mem_block;
}

mirac_ast_unit_s mirac_ast_unit_from_parts(
	mirac_arena_s* const arena,
	const mirac_string_view_s identifier)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_unit_s ast_unit = {0};
	ast_unit.identifier = identifier;
	ast_unit.global_blocks = mirac_blocks_vector_from_parts(arena, 1);
	ast_unit.strings = mirac_tokens_refs_vector_from_parts(arena, 1);
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
	mirac_ast_unit_s ast_unit = mirac_ast_unit_from_parts(parser->arena, parser->lexer->file_path);
	parse_ast_unit(parser, &ast_unit);
	return ast_unit;
}

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_func_s func_block = mirac_ast_block_func_from_parts(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_lexer_should_stop_lexing(token.type))
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token for the func definition, but reached the end of file."
		);
	}

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token for the func definition, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	func_block.identifier = token;

	token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_lexer_should_stop_lexing(token.type))
	{
		log_parser_error_and_exit(token.location,
			"expected 'req', 'res', or '{' token after function identifier, but reached the end of file."
		);
	}

	if ((token.type != mirac_token_type_reserved_req) &&
		(token.type != mirac_token_type_reserved_ret) &&
		(token.type != mirac_token_type_reserved_left_brace))
	{
		log_parser_error_and_exit(token.location,
			"expected 'req', 'res', or '{' token after function identifier, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	if (mirac_token_type_reserved_req == token.type)
	{
		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
		{
			if (mirac_token_is_reserved_type(&token))
			{
				mirac_ast_block_s new_block = {0};
				new_block.type = mirac_ast_block_type_expr;
				new_block.as.expr_block.token = token;
				mirac_blocks_vector_push(&func_block.req_blocks, new_block);
			}
			else if ((mirac_token_type_reserved_ret == token.type) ||
					 (mirac_token_type_reserved_left_brace == token.type))
			{
				break;
			}
			else
			{
				log_parser_error_and_exit(token.location,
					"expected 'res', '{', or type specifier token(s) after 'req' token, but found '" mirac_sv_fmt "' token.",
					mirac_sv_arg(token.text)
				);
			}
		}
	}

	if (mirac_lexer_should_stop_lexing(token.type))
	{
		log_parser_error_and_exit(token.location,
			"expected 'res' or '{' token, but reached the end of file."
		);
	}

	if ((token.type != mirac_token_type_reserved_ret) &&
		(token.type != mirac_token_type_reserved_left_brace))
	{
		log_parser_error_and_exit(token.location,
			"expected 'res' or '{' token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	if (mirac_token_type_reserved_ret == token.type)
	{
		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
		{
			if (mirac_token_is_reserved_type(&token))
			{
				mirac_ast_block_s new_block = {0};
				new_block.type = mirac_ast_block_type_expr;
				new_block.as.expr_block.token = token;
				mirac_blocks_vector_push(&func_block.ret_blocks, new_block);
			}
			else if (mirac_token_type_reserved_left_brace == token.type)
			{
				break;
			}
			else
			{
				log_parser_error_and_exit(token.location,
					"expected '{' or type specifier token(s) after 'res' token, but found '" mirac_sv_fmt "' token.",
					mirac_sv_arg(token.text)
				);
			}
		}
	}

	if (mirac_lexer_should_stop_lexing(token.type))
	{
		log_parser_error_and_exit(token.location,
			"expected '{' token, but reached the end of file."
		);
	}

	if (token.type != mirac_token_type_reserved_left_brace)
	{
		log_parser_error_and_exit(token.location,
			"expected '{' token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	do
	{
		// TODO: parse the body!
	} while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)));

	return func_block;
}

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_mem_s mem_block = mirac_ast_block_mem_from_parts();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_lexer_should_stop_lexing(token.type))
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token for the mem definition, but reached the end of file."
		);
	}

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token for the mem definition, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	mem_block.identifier = token;

	token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_lexer_should_stop_lexing(token.type))
	{
		log_parser_error_and_exit(token.location,
			"expected capacity token for the mem definition, but reached the end of file."
		);
	}

	if (!mirac_token_is_signed_numeric_literal(&token) && !mirac_token_is_unsigned_numeric_literal(&token))
	{
		log_parser_error_and_exit(token.location,
			"expected capacity token for the mem definition to be either signed or unsigned integer literal, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	if (mirac_token_is_signed_numeric_literal(&token) && (token.as.ival <= 0))
	{
		log_parser_error_and_exit(token.location,
			"provided signed integer capacity token must have a positive value, but found '%li'.",
			token.as.ival
		);
	}
	else if (mirac_token_is_unsigned_numeric_literal(&token) && (token.as.uval <= 0))
	{
		log_parser_error_and_exit(token.location,
			"provided unsigned integer capacity token must have a positive value, but found '%lu'.",
			token.as.uval
		);
	}

	mem_block.capacity = token;
	return mem_block;
}

static void parse_ast_unit(
	mirac_parser_s* const parser,
	mirac_ast_unit_s* const ast_unit)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_unit != NULL);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
	{
		// TODO: remove:
		mirac_logger_debug(mirac_sv_fmt, mirac_sv_arg(mirac_token_to_string_view(&token)));

		switch (token.type)
		{
			case mirac_token_type_reserved_inl:
			case mirac_token_type_reserved_func:
			{
				mirac_ast_block_s new_block = {0};
				new_block.type = mirac_ast_block_type_func;
				new_block.as.func_block = parse_ast_block_func(parser);
				mirac_blocks_vector_push(&ast_unit->global_blocks, new_block);
			} break;

			case mirac_token_type_reserved_mem:
			{
				mirac_ast_block_s new_block = {0};
				new_block.type = mirac_ast_block_type_mem;
				new_block.as.mem_block = parse_ast_block_mem(parser);
				mirac_blocks_vector_push(&ast_unit->global_blocks, new_block);
			} break;

			default:
			{
				log_parser_error_and_exit(token.location,
					"invalid token '" mirac_sv_fmt "' in global space",
					mirac_sv_arg(token.text)
				);
			} break;
		}
	}
}
