
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

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

static const mirac_string_view_s g_reserved_token_types_map[mirac_token_type_reserved_count + 1] =
{
	[mirac_token_type_reserved_lnot] = mirac_string_view_static("!") ,
	[mirac_token_type_reserved_land] = mirac_string_view_static("&&"),
	[mirac_token_type_reserved_lor]  = mirac_string_view_static("||"),
	[mirac_token_type_reserved_lxor] = mirac_string_view_static("^^"),

	[mirac_token_type_reserved_bnot] = mirac_string_view_static("~") ,
	[mirac_token_type_reserved_band] = mirac_string_view_static("&") ,
	[mirac_token_type_reserved_bor]  = mirac_string_view_static("|") ,
	[mirac_token_type_reserved_bxor] = mirac_string_view_static("^") ,
	[mirac_token_type_reserved_shl]  = mirac_string_view_static("<<"),
	[mirac_token_type_reserved_shr]  = mirac_string_view_static(">>"),

	[mirac_token_type_reserved_add]    = mirac_string_view_static("+") ,
	[mirac_token_type_reserved_inc]    = mirac_string_view_static("++"),
	[mirac_token_type_reserved_sub]    = mirac_string_view_static("-") ,
	[mirac_token_type_reserved_dec]    = mirac_string_view_static("--"),
	[mirac_token_type_reserved_mul]    = mirac_string_view_static("*") ,
	[mirac_token_type_reserved_div]    = mirac_string_view_static("/") ,
	[mirac_token_type_reserved_mod]    = mirac_string_view_static("%") ,
	[mirac_token_type_reserved_divmod] = mirac_string_view_static("/%"),

	[mirac_token_type_reserved_eq]   = mirac_string_view_static("=="),
	[mirac_token_type_reserved_neq]  = mirac_string_view_static("!="),
	[mirac_token_type_reserved_gt]   = mirac_string_view_static(">") ,
	[mirac_token_type_reserved_gteq] = mirac_string_view_static(">="),
	[mirac_token_type_reserved_ls]   = mirac_string_view_static("<") ,
	[mirac_token_type_reserved_lseq] = mirac_string_view_static("<="),

	[mirac_token_type_reserved_drop] = mirac_string_view_static("drop"),
	[mirac_token_type_reserved_dup]  = mirac_string_view_static("dup") ,
	[mirac_token_type_reserved_over] = mirac_string_view_static("over"),
	[mirac_token_type_reserved_rot]  = mirac_string_view_static("rot") ,
	[mirac_token_type_reserved_swap] = mirac_string_view_static("swap"),

	[mirac_token_type_reserved_ld08] = mirac_string_view_static("ld08"),
	[mirac_token_type_reserved_ld16] = mirac_string_view_static("ld16"),
	[mirac_token_type_reserved_ld32] = mirac_string_view_static("ld32"),
	[mirac_token_type_reserved_ld64] = mirac_string_view_static("ld64"),
	[mirac_token_type_reserved_st08] = mirac_string_view_static("st08"),
	[mirac_token_type_reserved_st16] = mirac_string_view_static("st16"),
	[mirac_token_type_reserved_st32] = mirac_string_view_static("st32"),
	[mirac_token_type_reserved_st64] = mirac_string_view_static("st64"),

	[mirac_token_type_reserved_sys1] = mirac_string_view_static("sys1"),
	[mirac_token_type_reserved_sys2] = mirac_string_view_static("sys2"),
	[mirac_token_type_reserved_sys3] = mirac_string_view_static("sys3"),
	[mirac_token_type_reserved_sys4] = mirac_string_view_static("sys4"),
	[mirac_token_type_reserved_sys5] = mirac_string_view_static("sys5"),
	[mirac_token_type_reserved_sys6] = mirac_string_view_static("sys6"),

	[mirac_token_type_reserved_true]  = mirac_string_view_static("true") ,
	[mirac_token_type_reserved_false] = mirac_string_view_static("false"),

	[mirac_token_type_reserved_i08] = mirac_string_view_static("i08"),
	[mirac_token_type_reserved_i16] = mirac_string_view_static("i16"),
	[mirac_token_type_reserved_i32] = mirac_string_view_static("i32"),
	[mirac_token_type_reserved_i64] = mirac_string_view_static("i64"),
	[mirac_token_type_reserved_u08] = mirac_string_view_static("u08"),
	[mirac_token_type_reserved_u16] = mirac_string_view_static("u16"),
	[mirac_token_type_reserved_u32] = mirac_string_view_static("u32"),
	[mirac_token_type_reserved_u64] = mirac_string_view_static("u64"),
	[mirac_token_type_reserved_ptr] = mirac_string_view_static("ptr"),

	[mirac_token_type_reserved_sec]  = mirac_string_view_static("sec") ,
	[mirac_token_type_reserved_str]  = mirac_string_view_static("str") ,
	[mirac_token_type_reserved_mem]  = mirac_string_view_static("mem") ,
	[mirac_token_type_reserved_fun] = mirac_string_view_static("fun") ,
	[mirac_token_type_reserved_if]   = mirac_string_view_static("if")  ,
	[mirac_token_type_reserved_else] = mirac_string_view_static("else"),
	[mirac_token_type_reserved_loop] = mirac_string_view_static("loop"),
	[mirac_token_type_reserved_req]  = mirac_string_view_static("req") ,
	[mirac_token_type_reserved_ret]  = mirac_string_view_static("ret") ,
	[mirac_token_type_reserved_call] = mirac_string_view_static("call"),
	[mirac_token_type_reserved_as]   = mirac_string_view_static("as")  ,
	[mirac_token_type_reserved_asm]  = mirac_string_view_static("asm") ,

	[mirac_token_type_reserved_left_parenthesis]  = mirac_string_view_static("("),
	[mirac_token_type_reserved_right_parenthesis] = mirac_string_view_static(")"),
	[mirac_token_type_reserved_left_bracket]      = mirac_string_view_static("["),
	[mirac_token_type_reserved_right_bracket]     = mirac_string_view_static("]"),
	[mirac_token_type_reserved_left_brace]        = mirac_string_view_static("{"),
	[mirac_token_type_reserved_right_brace]       = mirac_string_view_static("}"),
};

#define log_lexer_error_and_exit(_location, _format, ...)                      \
	do                                                                         \
	{                                                                          \
		(void)fprintf(stderr, mirac_sv_fmt ":%lu:%lu: ",                       \
			mirac_sv_arg((_location).file),                                    \
			(_location).line,                                                  \
			(_location).column);                                               \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		mirac_c_exit(-1);                                                      \
	} while (0)

/**
 * @brief Check if provided char is any quote (', ", or `).
 * 
 * @param ch char to check
 * 
 * @return bool_t
 */
static bool_t is_char_any_quote(
	const char_t ch);

/**
 * @brief Get the next token as text from the lexer's buffer.
 * 
 * @param lexer lexer instance
 * 
 * @return mirac_string_view_s
 */
static mirac_string_view_s get_next_token_as_text(
	mirac_lexer_s* const lexer);

/**
 * @brief Parse string literal token from the token's text.
 * 
 * @param lexer lexer instance
 * @param token token to parse
 * 
 * @return mirac_token_type_e
 */
static mirac_token_type_e parse_string_literal_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

/**
 * @brief Parse numeric literal token from the token's text.
 * 
 * @param lexer lexer instance
 * @param token token to parse
 * 
 * @return mirac_token_type_e
 */
static mirac_token_type_e parse_numeric_literal_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

/**
 * @brief Parse reserved token from the token's text.
 * 
 * @param lexer lexer instance
 * @param token token to parse
 * 
 * @return mirac_token_type_e
 */
static mirac_token_type_e parse_reserved_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

/**
 * @brief Parse identifier token from the token's text.
 * 
 * @param lexer lexer instance
 * @param token token to parse
 * 
 * @return mirac_token_type_e
 */
static mirac_token_type_e parse_identifier_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

mirac_string_view_s mirac_token_type_to_string_view(
	const mirac_token_type_e token_type)
{
	switch (token_type)
	{
		case mirac_token_type_literal_i08: { return mirac_string_view_from_parts("literal_i08", 11); } break;
		case mirac_token_type_literal_i16: { return mirac_string_view_from_parts("literal_i16", 11); } break;
		case mirac_token_type_literal_i32: { return mirac_string_view_from_parts("literal_i32", 11); } break;
		case mirac_token_type_literal_i64: { return mirac_string_view_from_parts("literal_i64", 11); } break;
		case mirac_token_type_literal_u08: { return mirac_string_view_from_parts("literal_u08", 11); } break;
		case mirac_token_type_literal_u16: { return mirac_string_view_from_parts("literal_u16", 11); } break;
		case mirac_token_type_literal_u32: { return mirac_string_view_from_parts("literal_u32", 11); } break;
		case mirac_token_type_literal_u64: { return mirac_string_view_from_parts("literal_u64", 11); } break;
		case mirac_token_type_literal_ptr: { return mirac_string_view_from_parts("literal_ptr", 11); } break;
		case mirac_token_type_literal_str: { return mirac_string_view_from_parts("literal_str", 11); } break;
		case mirac_token_type_identifier:  { return mirac_string_view_from_parts("identifier", 10);  } break;
		case mirac_token_type_eof:         { return mirac_string_view_from_parts("eof", 3);          } break;
		case mirac_token_type_none:        { return mirac_string_view_from_parts("none", 4);         } break;

		default:
		{
			mirac_debug_assert(token_type < (sizeof(g_reserved_token_types_map) / sizeof(g_reserved_token_types_map[0])));
			return g_reserved_token_types_map[token_type];
		} break;
	}
}

mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e type,
	const mirac_location_s location,
	const uint64_t index,
	const mirac_string_view_s text)
{
	return (mirac_token_s)
	{
		.type     = type,
		.location = location,
		.index    = index,
		.text     = text
	};
}

mirac_token_s mirac_token_from_type(
	const mirac_token_type_e type)
{
	return (mirac_token_s)
	{
		.type = type
	};
}

void mirac_token_destroy(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != mirac_null);
	*token = (mirac_token_s) {0};
	token->type = mirac_token_type_none;
}

mirac_string_view_s mirac_token_to_string_view(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != mirac_null);
	#define token_string_buffer_capacity 1024
	static char_t token_string_buffer[token_string_buffer_capacity + 1];

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

bool_t mirac_token_is_unsigned_numeric_literal(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != mirac_null);
	return (
		(mirac_token_type_literal_u08 == token->type) ||
		(mirac_token_type_literal_u16 == token->type) ||
		(mirac_token_type_literal_u32 == token->type) ||
		(mirac_token_type_literal_u64 == token->type)
	);
}

bool_t mirac_token_is_type_token(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != mirac_null);
	return (
		(mirac_token_type_reserved_i08 == token->type) ||
		(mirac_token_type_reserved_i16 == token->type) ||
		(mirac_token_type_reserved_i32 == token->type) ||
		(mirac_token_type_reserved_i64 == token->type) ||
		(mirac_token_type_reserved_u08 == token->type) ||
		(mirac_token_type_reserved_u16 == token->type) ||
		(mirac_token_type_reserved_u32 == token->type) ||
		(mirac_token_type_reserved_u64 == token->type) ||
		(mirac_token_type_reserved_ptr == token->type)
	);
}

mirac_lexer_s mirac_lexer_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	const mirac_string_view_s file_path,
	mirac_file_t* const file)
{
	mirac_debug_assert(config != mirac_null);
	mirac_debug_assert(arena != mirac_null);
	mirac_debug_assert(file != mirac_null);

	(void)fseek(file, 0, SEEK_END);
	const uint64_t length = (uint64_t)ftell(file) + 1;
	(void)fseek(file, 0, SEEK_SET);

	char_t* const buffer = (char_t* const)mirac_c_malloc((length + 1) * sizeof(char_t));
	mirac_debug_assert(buffer != mirac_null);

	const size_t read = fread(buffer, 1, length, file);
	mirac_debug_assert((length - 1) == read);

	buffer[length - 1] = '\n';
	buffer[length] = '\0';

	const mirac_location_s location = (const mirac_location_s)
	{
		.file   = file_path,
		.line   = 1,
		.column = 1
	};

	return (mirac_lexer_s)
	{
		.config = config,
		.arena = arena,
		.locations[0] = location,
		.locations[1] = location,
		.token = mirac_token_from_type(mirac_token_type_none),
		.buffer = mirac_string_view_from_parts(buffer, length),
		.line = (mirac_string_view_s) {0}
	};
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
	mirac_debug_assert(lexer != mirac_null);
	mirac_debug_assert(token != mirac_null);

	if (lexer->token.type != mirac_token_type_none)
	{
		*token = lexer->token;
		lexer->token = mirac_token_from_type(mirac_token_type_none);
		return token->type;
	}

	const mirac_string_view_s text = get_next_token_as_text(lexer);

	if (text.length <= 0)
	{
		token->type = mirac_token_type_eof;
		token->location = lexer->locations[0];
		return token->type;
	}

	char_t* const text_copy = (char_t* const)mirac_arena_malloc(lexer->arena, text.length);
	mirac_debug_assert(text_copy != mirac_null);
	mirac_c_memcpy(text_copy, text.data, text.length);

	*token = mirac_token_from_parts(mirac_token_type_none,
		lexer->locations[0], lexer->tokens_count++, mirac_string_view_from_parts(text_copy, text.length)
	);

	if (parse_reserved_token_from_text(lexer, token) != mirac_token_type_none)
	{
		return token->type;
	}

	if (parse_string_literal_token_from_text(lexer, token) != mirac_token_type_none)
	{
		return token->type;
	}

	if (parse_numeric_literal_token_from_text(lexer, token) != mirac_token_type_none)
	{
		return token->type;
	}

	if (parse_identifier_token_from_text(lexer, token) != mirac_token_type_none)
	{
		return token->type;
	}

	log_lexer_error_and_exit(lexer->locations[0], "encountered unknown token '" mirac_sv_fmt "'.", mirac_sv_arg(text));
	return mirac_token_type_none; // note: to prevent compiler error '-Werror=return-type'.
}

bool_t mirac_lexer_should_stop_lexing(
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
	mirac_debug_assert(lexer != mirac_null);
	mirac_debug_assert(token != mirac_null);
	lexer->token = *token;
}

static bool_t is_char_any_quote(
	const char_t ch)
{
	return (
		('\"' == ch) ||
		('\'' == ch) ||
		('`'  == ch)
	);
}

static mirac_string_view_s get_next_token_as_text(
	mirac_lexer_s* const lexer)
{
	mirac_debug_assert(lexer != mirac_null);
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

	if ((lexer->line.length <= 0) && (lexer->buffer.length > 0))
	{
		goto fetch_line;
	}

	if (mirac_string_view_equal_range(lexer->line, mirac_string_view_from_parts(";",  1), 1) ||
		mirac_string_view_equal_range(lexer->line, mirac_string_view_from_parts("//", 2), 2))
	{
		(void)mirac_string_view_split_left(&lexer->line, '\n');
		lexer->line.length = 0; // note: hack to force the line to be empty for successful refetch.
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
			++(text.length);
			goto search_for_quote_1;
		}

		++(text.length);
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
	lexer->locations[1].column += (text.length + white_space_length);
	return text;
}

static mirac_token_type_e parse_string_literal_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != mirac_null);
	mirac_debug_assert(lexer->config != mirac_null);
	mirac_debug_assert(lexer->arena != mirac_null);
	mirac_debug_assert(token != mirac_null);
	mirac_debug_assert(token->text.length > 0);

	if (token->text.data[0] != '\"') // note: At this point lexer does not perform the length check as it can vary depending on the string literal type.
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
		++(result.length);
		goto search_for_quote_2;
	}

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

	char_t* const string_literal = (char_t* const)mirac_arena_malloc(lexer->arena, result.length);
	mirac_debug_assert(string_literal != mirac_null);
	uint64_t string_literal_length = 0;

	for (uint64_t index = 0; index < result.length; ++index)
	{
		const char_t curr_char = result.data[index];

		switch (curr_char)
		{
			case '\\':
			{
				if (index >= (result.length - 1))
				{
					log_lexer_error_and_exit(token->location,
						"encountered an incomplete escape character sequence '%c' in string '" mirac_sv_fmt "'.",
						curr_char, mirac_sv_arg(result)
					);
				}

				const char_t next_char = result.data[++index];

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
	mirac_debug_assert(lexer != mirac_null);
	mirac_debug_assert(lexer->config != mirac_null);
	mirac_debug_assert(lexer->arena != mirac_null);
	mirac_debug_assert(token != mirac_null);
	mirac_debug_assert(token->text.length > 0);

	char_t* endptr = mirac_null;
	errno = 0;

	if (mirac_string_view_equal_range(token->text, mirac_string_view_from_parts("0b", 2), 2))
	{
		token->as.uval = strtoul(token->text.data + 2, &endptr, 2);

		if (*endptr != '\0')
		{
			return mirac_token_type_none;
		}
	}
	else if (mirac_string_view_equal_range(token->text, mirac_string_view_from_parts("0o", 2), 2))
	{
		token->as.uval = strtoul(token->text.data + 2, &endptr, 8);

		if (*endptr != '\0')
		{
			return mirac_token_type_none;
		}
	}
	else if (mirac_string_view_equal_range(token->text, mirac_string_view_from_parts("0x", 2), 2))
	{
		token->as.uval = strtoul(token->text.data + 2, &endptr, 16);

		if (*endptr != '\0')
		{
			return mirac_token_type_none;
		}
	}
	else
	{
		token->as.uval = strtoul(token->text.data, &endptr, 10);

		if (*endptr != '\0')
		{
			return mirac_token_type_none;
		}
	}

	if (errno != 0)
	{
		log_lexer_error_and_exit(token->location,
			"encountered a numeric literal overflow at '" mirac_sv_fmt "' value.",
			mirac_sv_arg(token->text)
		);
	}

	token->type = mirac_token_type_literal_u64;
	return token->type;
}

static mirac_token_type_e parse_reserved_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != mirac_null);
	mirac_debug_assert(lexer->config != mirac_null);
	mirac_debug_assert(lexer->arena != mirac_null);
	mirac_debug_assert(token != mirac_null);
	mirac_debug_assert(token->text.length > 0);

	for (uint64_t reserved_token_index = 0; reserved_token_index < (mirac_token_type_reserved_count + 1); ++reserved_token_index)
	{
		if (mirac_string_view_equal(token->text, g_reserved_token_types_map[reserved_token_index]))
		{
			token->type = reserved_token_index;
			return token->type;
		}
	}

	return mirac_token_type_none;
}

static mirac_token_type_e parse_identifier_token_from_text(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != mirac_null);
	mirac_debug_assert(lexer->config != mirac_null);
	mirac_debug_assert(lexer->arena != mirac_null);
	mirac_debug_assert(token != mirac_null);
	mirac_debug_assert(token->text.length > 0);

	if (isdigit(token->text.data[0]) || is_char_any_quote(token->text.data[0]))
	{
		return mirac_token_type_none;
	}

	for (uint64_t index = 1; index < token->text.length; ++index)
	{
		if (is_char_any_quote(token->text.data[index]))
		{
			return mirac_token_type_none;
		}
	}

	char_t* const ident_string = (char_t* const)mirac_arena_malloc(lexer->arena, token->text.length);
	mirac_debug_assert(ident_string != mirac_null);
	mirac_c_memcpy(ident_string, token->text.data, token->text.length);

	token->type = mirac_token_type_identifier;
	token->as.ident = mirac_string_view_from_parts(ident_string, token->text.length);
	return token->type;
}
