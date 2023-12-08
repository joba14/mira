
/**
 * @file lexer.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#include <mirac/lexer.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#define log_lexer_error_and_exit(_location, _format, ...)                      \
	do {                                                                       \
		(void)fprintf(stderr, "%s:%lu:%lu: ",                                  \
			(_location).file, (_location).line, (_location).column);           \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		mirac_c_exit(-1);                                                      \
	} while (0)

mirac_lexer_s mirac_lexer_from_parts(
	mirac_arena_s* const arena,
	const char* const file_path,
	FILE* const file)
{
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(file_path != NULL);
	mirac_debug_assert(file != NULL);

	mirac_lexer_s lexer;
	lexer.arena = arena;
	lexer.file_path = file_path;
	lexer.location.file = file_path;
	lexer.location.line = 1;
	lexer.location.column = 0;
	lexer.tokens_count = 0;
	lexer.token = mirac_token_from_type(mirac_token_type_none);

	(void)fseek(file, 0, SEEK_END);
	const uint64_t length = (uint64_t)ftell(file);
	(void)fseek(file, 0, SEEK_SET);
	char* const buffer = (char* const)mirac_c_malloc((length + 1) * sizeof(char));
	(void)fread(buffer, length, 1, file);
	lexer.buffer = string_view_from_parts(buffer, length);
	return lexer;
}

void mirac_lexer_destroy(
	mirac_lexer_s* const lexer)
{
	mirac_c_memset((void*)lexer, 0, sizeof(mirac_lexer_s));
}

mirac_token_type_e mirac_lexer_lex(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(token != NULL);

	// Token caching handling.
	if (lexer->token.type != mirac_token_type_none)
	{
		*token = lexer->token;
		lexer->token = mirac_token_from_type(mirac_token_type_none);
		return token->type;
	}

	lexer->buffer = string_view_trim_left_white_space(lexer->buffer);
	mirac_logger_debug(sv_fmt, sv_arg(lexer->buffer));

	if (mirac_reserved_token_type_from_string(lexer->buffer))
	{
		return 
	}

	// TODO: split buffer by first whitespace symbol.
	// TODO: parse the left split part.

	return mirac_token_type_none;
}

bool mirac_lexer_should_stop_lexing(
	const mirac_token_type_e type)
{
	return (
		(mirac_token_type_none == type) ||
		(mirac_token_type_eof  == type)
	);
}

void mirac_lexer_unlex(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(token != NULL);
	lexer->token = *token;
}
