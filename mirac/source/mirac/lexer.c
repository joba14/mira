
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

#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#define log_lexer_error_and_exit(_location, _format, ...)                      \
	do {                                                                       \
		(void)fprintf(stderr, sv_fmt ":%lu:%lu: ",                             \
			sv_arg((_location).file), (_location).line, (_location).column);   \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		mirac_c_exit(-1);                                                      \
	} while (0)

static FILE* validate_and_open_file_for_reading(
	const string_view_s file_path);

static string_view_s get_next_token_as_text(
	mirac_lexer_s* const lexer);

mirac_lexer_s mirac_lexer_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	const string_view_s file_path)
{
	mirac_debug_assert(config != NULL);
	mirac_debug_assert(arena != NULL);

	mirac_lexer_s lexer = {0};

	lexer.config = config;
	lexer.arena = arena;
	lexer.file_path = file_path;

	lexer.locations[0].file = file_path;
	lexer.locations[0].line = 0;
	lexer.locations[0].column = 0;
	lexer.locations[1] = lexer.locations[0];

	lexer.tokens_count = 0;
	lexer.token = mirac_token_from_type(mirac_token_type_none);

	FILE* const file = validate_and_open_file_for_reading(lexer.file_path);
	(void)fseek(file, 0, SEEK_END);
	const uint64_t length = (uint64_t)ftell(file);
	(void)fseek(file, 0, SEEK_SET);
	char* const buffer = (char* const)mirac_c_malloc((length + 1) * sizeof(char));
	(void)fread(buffer, length, 1, file);
	lexer.buffer = string_view_from_parts(buffer, length);
	(void)fclose(file);

	lexer.line = (string_view_s) {0};
	return lexer;
}

void mirac_lexer_destroy(
	mirac_lexer_s* const lexer)
{
	*lexer = (mirac_lexer_s) {0};
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

	const string_view_s token_as_text = get_next_token_as_text(lexer);
	if (token_as_text.length <= 0) { return mirac_token_type_eof; }

	if ((token->type = mirac_token_match_string_view_to_reserved_type(token_as_text)) != mirac_token_type_none)
	{
		token->location = lexer->locations[0];
		token->text = token_as_text;
		return token->type;
	}

	// TODO: lex strings e.g. "hello"
	// TODO: lex numeric literals
	// TODO: lex identifiers

	log_lexer_error_and_exit(lexer->locations[0], "encountered unknown token '" sv_fmt "'.", sv_arg(token_as_text));
	return mirac_token_type_none; // NOTE: To prevent compiler error '-Werror=return-type'.
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

static FILE* validate_and_open_file_for_reading(
	const string_view_s file_path)
{
	typedef struct stat file_stats_s;
	file_stats_s file_stats = {0};

	if (stat(file_path.data, &file_stats) != 0)
	{
		switch (errno)
		{
			case ENOENT:
			{
				mirac_logger_error("unable to open " sv_fmt " for reading -- file not found.", sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			case EACCES:
			{
				mirac_logger_error("unable to open " sv_fmt " for reading -- permission denied.", sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			case ENAMETOOLONG:
			{
				mirac_logger_error("unable to open " sv_fmt " for reading -- path name exceeds the system-defined maximum length.", sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			default:
			{
				mirac_logger_error("unable to open " sv_fmt " for reading -- failed to stat.", sv_arg(file_path));
				mirac_c_exit(-1);
			} break;
		}
	}

	if (S_ISDIR(file_stats.st_mode))
	{
		mirac_logger_error("unable to open " sv_fmt " for reading -- it is a directory.", sv_arg(file_path));
		mirac_c_exit(-1);
	}

	FILE* const file = fopen(file_path.data, "rt");

	if (NULL == file)
	{
		mirac_logger_error("unable to open " sv_fmt " for reading -- failed to open.", sv_arg(file_path));
		mirac_c_exit(-1);
	}

	return file;
}

static string_view_s get_next_token_as_text(
	mirac_lexer_s* const lexer)
{
	mirac_debug_assert(lexer != NULL);

	lexer->locations[0] = lexer->locations[1];

fetch_line:
	while ((lexer->line.length <= 0) && (lexer->buffer.length > 0))
	{
		lexer->line = string_view_split_left(&lexer->buffer, '\n');
		lexer->locations[0].column = 1;
		lexer->locations[0].line++;
	}

	if ((lexer->line.length <= 0) && (lexer->buffer.length <= 0))
	{
		return string_view_from_parts("", 0);
	}

	// TODO: remove:
	// mirac_logger_debug("loc=" mirac_location_fmt ", line='" sv_fmt "'", mirac_location_arg(lexer->location), sv_arg(lexer->line));
	// getchar();

	uint64_t white_space_length = 0;
	lexer->line = string_view_trim_left_white_space(lexer->line, &white_space_length);
	lexer->locations[0].column += white_space_length;

	if ((';' == lexer->line.data[0]) ||
		((lexer->line.length >= 2) && ('/' == lexer->line.data[0]) && ('/' == lexer->line.data[1])))
	{
		(void)string_view_split_left(&lexer->line, '\n');
		lexer->line.length = 0; // NOTE: Hack to force the line to be empty for successful refetch.
		goto fetch_line;
	}

	white_space_length = 0;
	const string_view_s token_as_text = string_view_split_left_white_space(&lexer->line, &white_space_length);
	lexer->locations[1] = lexer->locations[0];
	lexer->locations[1].column += token_as_text.length + white_space_length;
	return token_as_text;
}
