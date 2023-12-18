
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

mirac_implement_heap_array_type(mirac_ast_tokens_vector, mirac_token_s);
mirac_implement_heap_array_type(mirac_ast_tokens_refs_vector, mirac_token_s*);

mirac_unit_s mirac_unit_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_unit_s unit = {0};
	unit.globals = mirac_ast_tokens_vector_from_parts(arena, 4);
	unit.strings = mirac_ast_tokens_refs_vector_from_parts(arena, 4);
	return unit;
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

mirac_unit_s mirac_parser_parse_unit(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_unit_s unit = mirac_unit_from_parts(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
	{
		// TODO: remove(1):
		mirac_logger_debug(mirac_sv_fmt, mirac_sv_arg(mirac_token_to_string_view(&token)));
	}

	return unit;
}
