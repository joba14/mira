
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

static const mirac_string_view_s g_reserved_token_types_map[] =
{
	[mirac_token_type_reserved_lnot] = mirac_string_view_static("!"),
	[mirac_token_type_reserved_neq] = mirac_string_view_static("!="),
	[mirac_token_type_reserved_mod] = mirac_string_view_static("%"),
	[mirac_token_type_reserved_band] = mirac_string_view_static("&"),
	[mirac_token_type_reserved_land] = mirac_string_view_static("&&"),
	[mirac_token_type_reserved_mul] = mirac_string_view_static("*"),
	[mirac_token_type_reserved_add] = mirac_string_view_static("+"),
	[mirac_token_type_reserved_inc] = mirac_string_view_static("++"),
	[mirac_token_type_reserved_sub] = mirac_string_view_static("-"),
	[mirac_token_type_reserved_dec] = mirac_string_view_static("--"),
	[mirac_token_type_reserved_div] = mirac_string_view_static("/"),
	[mirac_token_type_reserved_ls] = mirac_string_view_static("<"),
	[mirac_token_type_reserved_shl] = mirac_string_view_static("<<"),
	[mirac_token_type_reserved_lseq] = mirac_string_view_static("<="),
	[mirac_token_type_reserved_eq] = mirac_string_view_static("=="),
	[mirac_token_type_reserved_gt] = mirac_string_view_static(">"),
	[mirac_token_type_reserved_gteq] = mirac_string_view_static(">="),
	[mirac_token_type_reserved_shr] = mirac_string_view_static(">>"),
	[mirac_token_type_reserved_drop] = mirac_string_view_static("\\!."),
	[mirac_token_type_reserved_sys1] = mirac_string_view_static("\\$1"),
	[mirac_token_type_reserved_sys2] = mirac_string_view_static("\\$2"),
	[mirac_token_type_reserved_sys3] = mirac_string_view_static("\\$3"),
	[mirac_token_type_reserved_sys4] = mirac_string_view_static("\\$4"),
	[mirac_token_type_reserved_sys5] = mirac_string_view_static("\\$5"),
	[mirac_token_type_reserved_sys6] = mirac_string_view_static("\\$6"),
	[mirac_token_type_reserved_st08] = mirac_string_view_static("\\&08"),
	[mirac_token_type_reserved_st16] = mirac_string_view_static("\\&16"),
	[mirac_token_type_reserved_st32] = mirac_string_view_static("\\&32"),
	[mirac_token_type_reserved_st64] = mirac_string_view_static("\\&64"),
	[mirac_token_type_reserved_ld08] = mirac_string_view_static("\\*08"),
	[mirac_token_type_reserved_ld16] = mirac_string_view_static("\\*16"),
	[mirac_token_type_reserved_ld32] = mirac_string_view_static("\\*32"),
	[mirac_token_type_reserved_ld64] = mirac_string_view_static("\\*64"),
	[mirac_token_type_reserved_dup] = mirac_string_view_static("\\+."),
	[mirac_token_type_reserved_over] = mirac_string_view_static("\\>."),
	[mirac_token_type_reserved_rot] = mirac_string_view_static("\\>3"),
	[mirac_token_type_reserved_swap] = mirac_string_view_static("\\><"),
	[mirac_token_type_reserved_bxor] = mirac_string_view_static("^"),
	[mirac_token_type_reserved_lxor] = mirac_string_view_static("^^"),
	[mirac_token_type_reserved_as] = mirac_string_view_static("as"),
	[mirac_token_type_reserved_elif] = mirac_string_view_static("elif"),
	[mirac_token_type_reserved_else] = mirac_string_view_static("else"),
	[mirac_token_type_reserved_f32] = mirac_string_view_static("f32"),
	[mirac_token_type_reserved_f64] = mirac_string_view_static("f64"),
	[mirac_token_type_reserved_func] = mirac_string_view_static("func"),
	[mirac_token_type_reserved_i08] = mirac_string_view_static("i08"),
	[mirac_token_type_reserved_i16] = mirac_string_view_static("i16"),
	[mirac_token_type_reserved_i32] = mirac_string_view_static("i32"),
	[mirac_token_type_reserved_i64] = mirac_string_view_static("i64"),
	[mirac_token_type_reserved_if] = mirac_string_view_static("if"),
	[mirac_token_type_reserved_inl] = mirac_string_view_static("inl"),
	[mirac_token_type_reserved_let] = mirac_string_view_static("let"),
	[mirac_token_type_reserved_loop] = mirac_string_view_static("loop"),
	[mirac_token_type_reserved_mem] = mirac_string_view_static("mem"),
	[mirac_token_type_reserved_ptr] = mirac_string_view_static("ptr"),
	[mirac_token_type_reserved_reg] = mirac_string_view_static("reg"),
	[mirac_token_type_reserved_req] = mirac_string_view_static("req"),
	[mirac_token_type_reserved_ret] = mirac_string_view_static("ret"),
	[mirac_token_type_reserved_u08] = mirac_string_view_static("u08"),
	[mirac_token_type_reserved_u16] = mirac_string_view_static("u16"),
	[mirac_token_type_reserved_u32] = mirac_string_view_static("u32"),
	[mirac_token_type_reserved_u64] = mirac_string_view_static("u64"),
	[mirac_token_type_reserved_left_brace] = mirac_string_view_static("{"),
	[mirac_token_type_reserved_bor] = mirac_string_view_static("|"),
	[mirac_token_type_reserved_lor] = mirac_string_view_static("||"),
	[mirac_token_type_reserved_right_brace] = mirac_string_view_static("}"),
	[mirac_token_type_reserved_bnot] = mirac_string_view_static("~")
};

mirac_string_view_s mirac_token_type_to_string_view(
	const mirac_token_type_e token_type)
{
	switch (token_type)
	{
		case mirac_token_type_literal_i08: { return mirac_string_view_from_cstring("literal_i8");  } break;
		case mirac_token_type_literal_i16: { return mirac_string_view_from_cstring("literal_i16"); } break;
		case mirac_token_type_literal_i32: { return mirac_string_view_from_cstring("literal_i32"); } break;
		case mirac_token_type_literal_i64: { return mirac_string_view_from_cstring("literal_i64"); } break;
		case mirac_token_type_literal_u08: { return mirac_string_view_from_cstring("literal_u8");  } break;
		case mirac_token_type_literal_u16: { return mirac_string_view_from_cstring("literal_u16"); } break;
		case mirac_token_type_literal_u32: { return mirac_string_view_from_cstring("literal_u32"); } break;
		case mirac_token_type_literal_u64: { return mirac_string_view_from_cstring("literal_u64"); } break;
		case mirac_token_type_literal_f32: { return mirac_string_view_from_cstring("literal_f32"); } break;
		case mirac_token_type_literal_f64: { return mirac_string_view_from_cstring("literal_f64"); } break;
		case mirac_token_type_literal_ptr: { return mirac_string_view_from_cstring("literal_ptr"); } break;
		case mirac_token_type_literal_str: { return mirac_string_view_from_cstring("literal_str"); } break;
		case mirac_token_type_identifier:  { return mirac_string_view_from_cstring("identifier");  } break;
		case mirac_token_type_eof:         { return mirac_string_view_from_cstring("eof");         } break;
		case mirac_token_type_none:        { return mirac_string_view_from_cstring("none");        } break;

		default:
		{
			mirac_debug_assert(token_type < (sizeof(g_reserved_token_types_map) / sizeof(g_reserved_token_types_map[0])));
			return g_reserved_token_types_map[token_type];
		} break;
	}
}

mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e token_type,
	const mirac_location_s location,
	const uint64_t index,
	const mirac_string_view_s text)
{
	mirac_token_s token = {0};
	token.type = token_type;
	token.location = location;
	token.index = index;
	token.text = text;
	return token;
}

mirac_token_s mirac_token_from_type(
	const mirac_token_type_e token_type)
{
	mirac_token_s token = {0};
	token.type = token_type;
	return token;
}

void mirac_token_destroy(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	*token = (mirac_token_s) {0};
	token->type = mirac_token_type_none;
}

mirac_string_view_s mirac_token_to_string_view(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	#define token_string_buffer_capacity 1024
	static char token_string_buffer[token_string_buffer_capacity + 1];

	uint64_t written = (uint64_t)snprintf(
		token_string_buffer, token_string_buffer_capacity,
		"Token[type='" mirac_sv_fmt "', location='" mirac_location_fmt "', index='%lu', text='" mirac_sv_fmt "'",
		mirac_sv_arg(mirac_token_type_to_string_view(token->type)),
		mirac_location_arg(token->location),
		token->index,
		mirac_sv_arg(token->text)
	);

	switch (token->type)
	{
		case mirac_token_type_literal_i08:
		case mirac_token_type_literal_i16:
		case mirac_token_type_literal_i32:
		case mirac_token_type_literal_i64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%li']", token->as.ival
			);
		} break;

		case mirac_token_type_literal_u08:
		case mirac_token_type_literal_u16:
		case mirac_token_type_literal_u32:
		case mirac_token_type_literal_u64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%lu']", token->as.uval
			);
		} break;

		case mirac_token_type_literal_f32:
		case mirac_token_type_literal_f64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%Lf']", token->as.fval
			);
		} break;

		case mirac_token_type_literal_str:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='" mirac_sv_fmt "']", mirac_sv_arg(token->as.str)
			);
		} break;

		case mirac_token_type_identifier:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='" mirac_sv_fmt "']", mirac_sv_arg(token->as.ident)
			);
		} break;

		default:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written, "]"
			);
		} break;
	}

	token_string_buffer[written++] = 0;
	return mirac_string_view_from_parts(token_string_buffer, written);
}

#define log_lexer_error_and_exit(_location, _format, ...)                      \
	do {                                                                       \
		(void)fprintf(stderr, mirac_sv_fmt ":%lu:%lu: ",                       \
			mirac_sv_arg((_location).file),                                    \
			(_location).line,                                                  \
			(_location).column);                                               \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		mirac_c_exit(-1);                                                      \
	} while (0)

/**
 * @brief Validate file path and open file for reading.
 * 
 * @param file_path[in] path of the file to be opened
 * 
 * @return FILE*
 */
static FILE* validate_and_open_file_for_reading(
	const mirac_string_view_s file_path);

/**
 * @brief Get the next token as text from the lexer's buffer.
 * 
 * @param lexer[in/out] lexer instance
 * 
 * @return mirac_string_view_s
 */
static mirac_string_view_s get_next_token_as_text(
	mirac_lexer_s* const lexer);

/**
 * @brief Parse string literal token from the token's text.
 * 
 * @param lexer[in/out] lexer instance
 * @param token[in/out] token to parse
 * 
 * @return mirac_token_type_e
 */
static mirac_token_type_e parse_string_literal_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

/**
 * @brief Parse numeric literal token from the token's text.
 * 
 * @param lexer[in/out] lexer instance
 * @param token[in/out] token to parse
 * 
 * @return mirac_token_type_e
 */
static mirac_token_type_e parse_numeric_literal_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

/**
 * @brief Compare two string views alphabetically.
 * 
 * @param left[in]  pointer to the left string view
 * @param right[in] pointer to the right string view
 * 
 * @return int32_t
 */
static int32_t compare_text_with_reserved_token(
	const void* const left,
	const void* const right);

/**
 * @brief Parse reserved token from the token's text.
 * 
 * @param lexer[in/out] lexer instance
 * @param token[in/out] token to parse
 * 
 * @return mirac_token_type_e
 */
static mirac_token_type_e parse_reserved_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

/**
 * @brief Parse identifier token from the token's text.
 * 
 * @param lexer[in/out] lexer instance
 * @param token[in/out] token to parse
 * 
 * @return mirac_token_type_e
 */
static mirac_token_type_e parse_identifier_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

mirac_lexer_s mirac_lexer_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	const mirac_string_view_s file_path)
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
	lexer.buffer = mirac_string_view_from_parts(buffer, length);
	(void)fclose(file);

	lexer.line = (mirac_string_view_s) {0};
	return lexer;
}

void mirac_lexer_destroy(
	mirac_lexer_s* const lexer)
{
	*lexer = (mirac_lexer_s) {0};
}

mirac_token_type_e mirac_lexer_lex_next(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(token != NULL);

	if (lexer->token.type != mirac_token_type_none)
	{
		*token = lexer->token;
		lexer->token = mirac_token_from_type(mirac_token_type_none);
		return token->type;
	}

	const mirac_string_view_s text = get_next_token_as_text(lexer);
	if (text.length <= 0) { return mirac_token_type_eof; }

	char* const text_copy = (char* const)mirac_arena_malloc(lexer->arena, text.length);
	mirac_c_memcpy(text_copy, text.data, text.length);
	*token = mirac_token_from_parts(mirac_token_type_none,
		lexer->locations[0], lexer->tokens_count++, mirac_string_view_from_parts(text_copy, text.length)
	);

	if (parse_string_literal_token_from_text(lexer, token) != mirac_token_type_none)
	{
		return token->type;
	}

	if (parse_numeric_literal_token_from_text(lexer, token) != mirac_token_type_none)
	{
		return token->type;
	}

	if (parse_reserved_token_from_text(lexer, token) != mirac_token_type_none)
	{
		return token->type;
	}

	if (parse_identifier_token_from_text(lexer, token) != mirac_token_type_none)
	{
		return token->type;
	}

	log_lexer_error_and_exit(lexer->locations[0], "encountered unknown token '" mirac_sv_fmt "'.", mirac_sv_arg(text));
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
	const mirac_string_view_s file_path)
{
	typedef struct stat file_stats_s;
	file_stats_s file_stats = {0};

	if (stat(file_path.data, &file_stats) != 0)
	{
		switch (errno)
		{
			case ENOENT:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- file not found.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			case EACCES:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- permission denied.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			case ENAMETOOLONG:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- path name exceeds the system-defined maximum length.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			default:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- failed to stat.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;
		}
	}

	if (S_ISDIR(file_stats.st_mode))
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- it is a directory.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	FILE* const file = fopen(file_path.data, "rt");

	if (NULL == file)
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- failed to open.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	return file;
}

static mirac_string_view_s get_next_token_as_text(
	mirac_lexer_s* const lexer)
{
	mirac_debug_assert(lexer != NULL);
	lexer->locations[0] = lexer->locations[1];

fetch_line:
	while ((lexer->line.length <= 0) && (lexer->buffer.length > 0))
	{
		lexer->line = mirac_string_view_split_left(&lexer->buffer, '\n');
		lexer->locations[0].line++;
		lexer->locations[0].column = 1;
	}

	if ((lexer->line.length <= 0) && (lexer->buffer.length <= 0))
	{
		return mirac_string_view_from_parts("", 0);
	}

	uint64_t white_space_length = 0;
	lexer->line = mirac_string_view_trim_left_white_space(lexer->line, &white_space_length);
	lexer->locations[0].column += white_space_length;

	if (mirac_string_view_equal_range(lexer->line, mirac_string_view_from_parts(";",  1), 1) ||
		mirac_string_view_equal_range(lexer->line, mirac_string_view_from_parts("//", 2), 2))
	{
		(void)mirac_string_view_split_left(&lexer->line, '\n');
		lexer->line.length = 0; // NOTE: Hack to force the line to be empty for successful refetch.
		goto fetch_line;
	}

	mirac_string_view_s text = {0};

	if ('\"' == lexer->line.data[0])
	{
		mirac_string_view_s left = {0};
		mirac_string_view_s right = mirac_string_view_from_parts(lexer->line.data + 1, lexer->line.length - 1);
		text = mirac_string_view_from_parts(lexer->line.data, 1);

search_for_quote_1:
		left = mirac_string_view_split_left(&right, '\"');
		text.length += left.length;

		if ((left.length > 0) && ('\\' == left.data[left.length - 1]))
		{
			text.length += 1;
			goto search_for_quote_1;
		}

		text.length++;
		white_space_length = 0;
		left = mirac_string_view_split_left_white_space(&right, &white_space_length);
		text.length += left.length;
		lexer->line = right;
	}
	else
	{
		white_space_length = 0;
		text = mirac_string_view_split_left_white_space(&lexer->line, &white_space_length);
	}

	lexer->locations[1] = lexer->locations[0];
	lexer->locations[1].column += text.length + white_space_length;
	return text;
}

static mirac_token_type_e parse_string_literal_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(lexer->config != NULL);
	mirac_debug_assert(lexer->arena != NULL);
	mirac_debug_assert(token != NULL);
	mirac_debug_assert(token->text.length > 0);

	if (token->text.data[0] != '\"') // NOTE: At this point lexer does not perform the length check as it can vary depending on the string literal type.
	{
		return mirac_token_type_none;
	}

	mirac_string_view_s left = {0};
	mirac_string_view_s right = mirac_string_view_from_parts(token->text.data + 1, token->text.length - 1);
	mirac_string_view_s result = mirac_string_view_from_parts(right.data, 0);

search_for_quote_2:
	left = mirac_string_view_split_left(&right, '\"');
	result.length += left.length;

	if ((left.length > 0) && ('\\' == left.data[left.length - 1]))
	{
		result.length += 1;
		goto search_for_quote_2;
	}

	result.length++;

	if (right.length > 0)
	{
		log_lexer_error_and_exit(token->location,
			"encountered an invalid suffix '" mirac_sv_fmt "' after the string literal.\n"
			"note: every token in the language's syntax must be separated by a single white space or a sequence of them.",
			mirac_sv_arg(right)
		);
	}

	if (result.length <= 0)
	{
		log_lexer_error_and_exit(token->location,
			"encountered an empty string which is not valid.\n"
			"note: unlike in c-like languages, string literals do not have implicit zero terminator '\\0' at the end."
		);
	}

	uint64_t string_literal_length = 0;
	char* const string_literal = (char* const)mirac_arena_malloc(lexer->arena, result.length);

	for (uint64_t index = 0; index < result.length; ++index)
	{
		const char curr_char = result.data[index];

		switch (curr_char)
		{
			case '\\':
			{
				if (index >= result.length - 1)
				{
					log_lexer_error_and_exit(token->location,
						"encountered an incomplete escape character sequence '%c' in string '" mirac_sv_fmt "'.",
						curr_char, mirac_sv_arg(result)
					);
				}

				const char next_char = result.data[++index];

				switch (next_char)
				{
					case '0':  { string_literal[string_literal_length] = '\0'; } break;
					case 'a':  { string_literal[string_literal_length] = '\a'; } break;
					case 'b':  { string_literal[string_literal_length] = '\b'; } break;
					case 'f':  { string_literal[string_literal_length] = '\f'; } break;
					case 'n':  { string_literal[string_literal_length] = '\n'; } break;
					case 'r':  { string_literal[string_literal_length] = '\r'; } break;
					case 't':  { string_literal[string_literal_length] = '\t'; } break;
					case 'v':  { string_literal[string_literal_length] = '\v'; } break;
					case '\\': { string_literal[string_literal_length] = '\\'; } break;
					case '\'': { string_literal[string_literal_length] = '\''; } break;
					case '\"': { string_literal[string_literal_length] = '\"'; } break;

					default:
					{
						log_lexer_error_and_exit(token->location,
							"encountered an invalid escape character sequence '%c%c' in string '" mirac_sv_fmt "'.",
							curr_char, next_char, mirac_sv_arg(result)
						);
					} break;
				}
			} break;

			default:
			{
				string_literal[string_literal_length] = curr_char;
			} break;
		}

		++string_literal_length;
	}

	token->type = mirac_token_type_literal_str;
	token->as.str = mirac_string_view_from_parts(string_literal, string_literal_length);
	return token->type;
}

static mirac_token_type_e parse_numeric_literal_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(lexer->config != NULL);
	mirac_debug_assert(lexer->arena != NULL);
	mirac_debug_assert(token != NULL);
	mirac_debug_assert(token->text.length > 0);

	if ((token->text.data[0] != '-') &&
		(token->text.data[0] != '+') &&
		!isdigit(token->text.data[0])) // NOTE: At this point lexer does not perform the length check as it can vary depending on the numeric literal type.
	{
		return mirac_token_type_none;
	}

	const bool has_sign = ('-' == token->text.data[0]) || ('+' == token->text.data[0]);

	if (has_sign && (token->text.length <= 1))
	{
		return mirac_token_type_none;
	}

	bool has_dot = false;

	for (uint64_t index = (uint8_t)has_sign; index < token->text.length; ++index)
	{
		const char curr_char = token->text.data[index];

		if (isdigit(curr_char) || ('.' == curr_char))
		{
			if ('.' == curr_char)
			{
				if (has_dot)
				{
					log_lexer_error_and_exit(token->location,
						"encountered multiple '%c' symbols in numeric literal.",
						curr_char
					);
				}
				else
				{
					has_dot = true;
				}
			}
		}
		else
		{
			log_lexer_error_and_exit(token->location,
				"encountered invalid symbol '%c' in numeric literal.",
				curr_char
			);
		}
	}

	token->type = (has_dot ? mirac_token_type_literal_f64 : mirac_token_type_literal_i64);
	errno = 0;

	if (mirac_token_type_literal_f64 == token->type)
	{
		token->as.fval = strtold(token->text.data, NULL);
	}
	else
	{
		token->as.ival = strtoll(token->text.data, NULL, 10);
	}

	if (errno != 0)
	{
		log_lexer_error_and_exit(token->location,
			"numeric literal overflow."
		);
	}

	return token->type;
}

static int32_t compare_text_with_reserved_token(
	const void* const left,
	const void* const right)
{
	const mirac_string_view_s* left_string_view = (const mirac_string_view_s*)left;
	const mirac_string_view_s* right_string_view = (const mirac_string_view_s*)right;
	return mirac_c_strncmp(left_string_view->data, right_string_view->data,
		(left_string_view->length < right_string_view->length ?
			left_string_view->length : right_string_view->length)
	);
}

static mirac_token_type_e parse_reserved_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(lexer->config != NULL);
	mirac_debug_assert(lexer->arena != NULL);
	mirac_debug_assert(token != NULL);
	mirac_debug_assert(token->text.length > 0);

	const void* const found_token = (const void* const)mirac_c_bsearch(
		&token->text, g_reserved_token_types_map, mirac_token_type_reserved_count + 1,
		sizeof(g_reserved_token_types_map[0]), compare_text_with_reserved_token
	);

	if (NULL == found_token)
	{
		return mirac_token_type_none;
	}
	else
	{
		token->type = (mirac_token_type_e)((const mirac_string_view_s*)found_token - g_reserved_token_types_map);
		return token->type;
	}
}

static mirac_token_type_e parse_identifier_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(lexer->config != NULL);
	mirac_debug_assert(lexer->arena != NULL);
	mirac_debug_assert(token != NULL);
	mirac_debug_assert(token->text.length > 0);

	if (isdigit(token->text.data[0])  ||
		('\"' == token->text.data[0]) ||
		('\'' == token->text.data[0]) ||
		('`'  == token->text.data[0]))
	{
		return mirac_token_type_none;
	}

	for (uint64_t index = 1; index < token->text.length; ++index)
	{
		if (('\"' == token->text.data[index]) ||
			('\'' == token->text.data[index]) ||
			('`'  == token->text.data[index]))
		{
			return mirac_token_type_none;
		}
	}

	char* const ident_string = (char* const)mirac_arena_malloc(lexer->arena, token->text.length);
	mirac_c_memcpy(ident_string, token->text.data, token->text.length);

	token->type = mirac_token_type_identifier;
	token->as.ident = mirac_string_view_from_parts(ident_string, token->text.length);
	return token->type;
}
