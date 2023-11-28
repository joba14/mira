
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
#include <mirac/utils.h>

#include <inttypes.h>

#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#define log_lexer_error_and_exit(_location, _format, ...)                      \
	do {                                                                       \
		(void)fprintf(stderr, "%s:%lu:%lu: ",                                  \
			(_location).file, (_location).line, (_location).column);           \
		mirac_logger_error(_format, ## __VA_ARGS__);                          \
		exit(-1);                                                              \
	} while (0)

static void update_location(
	mirac_location_s* const location,
	const utf8char_t utf8char);

static void append_buffer(
	mirac_lexer_s* const lexer,
	const char* const buffer,
	const uint64_t size);

static utf8char_t next_utf8char(
	mirac_lexer_s* const lexer,
	mirac_location_s* const location,
	const bool buffer);

static bool is_symbol_a_white_space(
	const utf8char_t utf8char);

static utf8char_t get_utf8char(
	mirac_lexer_s* const lexer,
	mirac_location_s* const location);

static void clear_buffer(
	mirac_lexer_s* const lexer);

static void consume_buffer(
	mirac_lexer_s* const lexer,
	const uint64_t length);

static void push_utf8char(
	mirac_lexer_s* const lexer,
	const utf8char_t utf8char,
	const bool buffer);

static bool is_symbol_first_of_identifier_or_keyword(
	const utf8char_t utf8char);

static bool is_symbol_not_first_of_identifier_or_keyword(
	const utf8char_t utf8char);

static bool is_symbol_first_of_numeric_literal(
	const utf8char_t utf8char);

static mirac_token_type_e lex_identifier_or_keyword(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

static bool lex_numeric_literal_token(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

static uint8_t lex_possible_rune(
	mirac_lexer_s* const lexer,
	char* const rune);

static mirac_token_type_e lex_string_literal_token(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

mirac_lexer_s mirac_lexer_from_parts(
	const char* const file_path,
	FILE* const file)
{
	mirac_debug_assert(file_path != NULL);
	mirac_debug_assert(file != NULL);

	mirac_lexer_s lexer;
	lexer.file = file;
	lexer.location.file = file_path;
	lexer.location.line = 1;
	lexer.location.column = 0;

	lexer.buffer.capacity = 256;
	lexer.buffer.data = mirac_utils_malloc(lexer.buffer.capacity * sizeof(char));
	lexer.buffer.length = 0;
	lexer.cache[0] = mirac_utf8_invalid;
	lexer.cache[1] = mirac_utf8_invalid;
	lexer.require_int = false;
	return lexer;
}

void mirac_lexer_destroy(
	mirac_lexer_s* const lexer)
{
	mirac_utils_free(lexer->buffer.data);
	mirac_utils_memset((void*)lexer, 0, sizeof(mirac_lexer_s));
}

mirac_token_type_e mirac_lexer_lex(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(token != NULL);

	utf8char_t utf8char = get_utf8char(lexer, &token->location);

	if (mirac_utf8_invalid == utf8char)
	{
		return mirac_token_type_eof;
	}

	// Skipping white spaces
	while (utf8char != mirac_utf8_invalid)
	{
		if (!is_symbol_a_white_space(utf8char))
		{
			break;
		}

		utf8char = next_utf8char(lexer, NULL, false);
	}

	// Single and multiline Comments
	if ('/' == utf8char)
	{
		switch ((utf8char = next_utf8char(lexer, NULL, false)))
		{
			case '/':
			{
				while ((utf8char = next_utf8char(lexer, NULL, false)) != mirac_utf8_invalid && utf8char != '\n');
				return mirac_lexer_lex(lexer, token);
			} break;

			case '*':
			{
				utf8char_t last_utf8char = utf8char;
				utf8char = next_utf8char(lexer, NULL, false);

				while (last_utf8char != mirac_utf8_invalid && utf8char != mirac_utf8_invalid
					&& (last_utf8char != '*' || utf8char != '/'))
				{
					last_utf8char = utf8char;
					utf8char = next_utf8char(lexer, NULL, false);
				}

				return mirac_lexer_lex(lexer, token);
			} break;

			default:
			{
				char invalid[4];
				const uint8_t length = mirac_utf8_encode(invalid, utf8char);
				log_lexer_error_and_exit(token->location, "invalid token encountered: `%.*s`",
					(signed int)length, invalid);
				return mirac_token_type_none;
			} break;
		}
	}

	// String literals
	if ('\"' == utf8char)
	{
		push_utf8char(lexer, utf8char, false);
		return lex_string_literal_token(lexer, token);
	}

	// Numeric literals
	if (is_symbol_first_of_numeric_literal(utf8char))
	{
		push_utf8char(lexer, utf8char, false);

		if (lex_numeric_literal_token(lexer, token))
		{
			return token->type;
		}
	}

	lexer->require_int = false;

	// Keywords and identifiers
	if (is_symbol_first_of_identifier_or_keyword(utf8char))
	{
		push_utf8char(lexer, utf8char, false);
		return lex_identifier_or_keyword(lexer, token);
	}

	// Invalid token
	char invalid[4];
	const uint8_t length = mirac_utf8_encode(invalid, utf8char);
	log_lexer_error_and_exit(token->location, "invalid token encountered: `%.*s`",
		(signed int)length, invalid);
	return mirac_token_type_none;
}

bool mirac_lexer_should_stop_lexing(
	const mirac_token_type_e type)
{
	return mirac_token_type_none == type || mirac_token_type_eof == type;
}

static void update_location(
	mirac_location_s* const location,
	const utf8char_t utf8char)
{
	mirac_debug_assert(location != NULL);

	if ('\n' == utf8char)
	{
		location->line++;
		location->column = 0;
	}
	else
	{
		location->column++;
	}
}

static void append_buffer(
	mirac_lexer_s* const lexer,
	const char* const buffer,
	const uint64_t size)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(buffer != NULL);
	mirac_debug_assert(size > 0);

	if (lexer->buffer.length + size >= lexer->buffer.capacity)
	{
		lexer->buffer.capacity *= 2;
		lexer->buffer.data = mirac_utils_realloc(lexer->buffer.data, lexer->buffer.capacity);
	}

	mirac_utils_memcpy(lexer->buffer.data + lexer->buffer.length, buffer, size);
	lexer->buffer.length += size;
	lexer->buffer.data[lexer->buffer.length] = 0;
}

static utf8char_t next_utf8char(
	mirac_lexer_s* const lexer,
	mirac_location_s* const location,
	const bool buffer)
{
	mirac_debug_assert(lexer != NULL);
	utf8char_t utf8char = mirac_utf8_invalid;

	if (lexer->cache[0] != mirac_utf8_invalid)
	{
		utf8char = lexer->cache[0];
		lexer->cache[0] = lexer->cache[1];
		lexer->cache[1] = mirac_utf8_invalid;
	}
	else
	{
		utf8char = mirac_utf8_get(lexer->file);
		update_location(&lexer->location, utf8char);

		if (mirac_utf8_invalid == utf8char && !feof(lexer->file))
		{
			log_lexer_error_and_exit(lexer->location, "invalid utf-8 sequence encountered.");
		}
	}

	if (location != NULL)
	{
		*location = lexer->location;

		for (uint8_t index = 0; index < 2 && lexer->cache[index] != mirac_utf8_invalid; ++index)
		{
			update_location(&lexer->location, lexer->cache[index]);
		}
	}

	if (mirac_utf8_invalid == utf8char || !buffer)
	{
		return utf8char;
	}

	char utf8_buffer[mirac_utf8_max_size];
	const uint8_t size = mirac_utf8_encode(utf8_buffer, utf8char);
	append_buffer(lexer, utf8_buffer, size);
	return utf8char;
}

static bool is_symbol_a_white_space(
	const utf8char_t utf8char)
{
	return ('\t' == utf8char) || ('\n' == utf8char) || ('\r' == utf8char) || (' ' == utf8char);
}

static utf8char_t get_utf8char(
	mirac_lexer_s* const lexer,
	mirac_location_s* const location)
{
	mirac_debug_assert(lexer != NULL);
	utf8char_t utf8char = mirac_utf8_invalid;
	while ((utf8char = next_utf8char(lexer, location, false)) != mirac_utf8_invalid && is_symbol_a_white_space(utf8char));
	return utf8char;
}

static void clear_buffer(
	mirac_lexer_s* const lexer)
{
	mirac_debug_assert(lexer != NULL);
	lexer->buffer.length = 0;
	lexer->buffer.data[0] = 0;
}

static void consume_buffer(
	mirac_lexer_s* const lexer,
	const uint64_t length)
{
	for (uint64_t index = 0; index < length; ++index)
	{
		while (0x80 == (lexer->buffer.data[--lexer->buffer.length] & 0xC0));
	}

	lexer->buffer.data[lexer->buffer.length] = 0;
}

static void push_utf8char(
	mirac_lexer_s* const lexer,
	const utf8char_t utf8char,
	const bool buffer)
{
	mirac_debug_assert(lexer != NULL);

	mirac_debug_assert(mirac_utf8_invalid == lexer->cache[1]);
	lexer->cache[1] = lexer->cache[0];
	lexer->cache[0] = utf8char;

	if (buffer)
	{
		consume_buffer(lexer, 1);
	}
}

static bool is_symbol_first_of_identifier_or_keyword(
	const utf8char_t utf8char)
{
	mirac_debug_assert(utf8char != mirac_utf8_invalid);
	return (utf8char <= 0x7F) && (
		isalpha(utf8char) ||
		('_' == utf8char) ||
		('?' == utf8char) ||
		('!' == utf8char) ||
		(',' == utf8char) ||
		('.' == utf8char) ||
		('$' == utf8char) ||
		('@' == utf8char) ||
		('<' == utf8char) ||
		('>' == utf8char)
	);
}

static bool is_symbol_not_first_of_identifier_or_keyword(
	const utf8char_t utf8char)
{
	mirac_debug_assert(utf8char != mirac_utf8_invalid);
	return (utf8char <= 0x7F) && (
		isalnum(utf8char) ||
		('_' == utf8char) ||
		('?' == utf8char) ||
		('!' == utf8char) ||
		(',' == utf8char) ||
		('.' == utf8char) ||
		('$' == utf8char) ||
		('@' == utf8char) ||
		('<' == utf8char) ||
		('>' == utf8char) ||
		('-' == utf8char) ||
		('(' == utf8char) ||
		(')' == utf8char) ||
		('[' == utf8char) ||
		(']' == utf8char) ||
		('{' == utf8char) ||
		('}' == utf8char)
	);
}

static bool is_symbol_first_of_numeric_literal(
	const utf8char_t utf8char)
{
	mirac_debug_assert(utf8char != mirac_utf8_invalid);
	return (utf8char <= 0x7F) && (
		isdigit(utf8char) ||
		'+' == utf8char ||
		'-' == utf8char
	);
}

static mirac_token_type_e lex_identifier_or_keyword(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(token != NULL);

	utf8char_t utf8char = next_utf8char(lexer, &token->location, true);
	mirac_debug_assert(token != NULL);

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	mirac_debug_assert(
		is_symbol_first_of_identifier_or_keyword(utf8char)
	); // Sanity check for developers.

	while ((utf8char = next_utf8char(lexer, NULL, true)) != mirac_utf8_invalid)
	{
		if (!is_symbol_not_first_of_identifier_or_keyword(utf8char))
		{
			push_utf8char(lexer, utf8char, true);
			break;
		}
	}

	token->type = mirac_token_type_from_string(lexer->buffer.data);
	token->ident.data = mirac_utils_strndup(lexer->buffer.data, lexer->buffer.length);
	token->ident.length = lexer->buffer.length;

	if (!is_symbol_a_white_space(utf8char))
	{
		log_lexer_error_and_exit(
			lexer->location, "invalid symbol `%c` following identifier `%.*s`.",
			(char)utf8char, (signed int)token->ident.length, token->ident.data
		);
	}

	clear_buffer(lexer);
	return token->type;
}

static uint64_t compute_exponent(
	uint64_t n,
	uint64_t exponent,
	const bool is_signed)
{
	if (0 == n)
	{
		return 0;
	}

	for (uint64_t index = 0; index < exponent; ++index)
	{
		uint64_t old = n;
		n *= 10;

		if (n / 10 != old)
		{
			errno = ERANGE;
			return INT64_MAX;
		}
	}

	if (is_signed && n > (uint64_t)INT64_MIN)
	{
		errno = ERANGE;
		return INT64_MAX;
	}

	return n;
}

static bool lex_numeric_literal_token(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(token != NULL);

	enum { base_bin = 1, base_oct, base_hex, base_dec = 0x07, base_mask = base_dec };
	_Static_assert((base_bin | base_oct | base_hex | base_dec) == base_dec,
		"base_dec bits must be a superset of all other bases" );
	enum { flag_flt = 3, flag_exp, flag_suff, flag_dig };

	static const char chrs[][24] =
	{
		[base_bin] = "01",
		[base_oct] = "01234567",
		[base_dec] = "0123456789",
		[base_hex] = "0123456789abcdefABCDEF"
	};

	static const char matching_states[0x80][6] =
	{
		['.'] = { base_dec, base_hex, 0 },
		['e'] = { base_dec, base_dec | 1 << flag_flt, 0 },
		['E'] = { base_dec, base_dec | 1 << flag_flt, 0 },
		['p'] = { base_hex, base_hex | 1 << flag_flt, 0 },
		['P'] = { base_hex, base_hex | 1 << flag_flt, 0 },
		['+'] = { base_dec | 1 << flag_dig, base_dec | 1 << flag_flt | 1 << flag_dig, 0 },
		['-'] = { base_dec | 1 << flag_dig, base_dec | 1 << flag_flt | 1 << flag_dig, 0 },
		['i'] = { base_bin, base_oct, base_hex, base_dec, base_dec, 0 },
		['u'] = { base_bin, base_oct, base_hex, base_dec, base_dec, 0 },
		['f'] = { base_dec, base_dec | 1 << flag_flt, base_dec, base_dec | 1 << flag_flt, 0 },
	};

	int32_t state = base_dec;
	int32_t old_state = base_dec;
	int32_t base = 10;

	utf8char_t utf8char = next_utf8char(lexer, &token->location, true);
	utf8char_t last = 0;

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	mirac_debug_assert(
		is_symbol_first_of_numeric_literal(utf8char)
	); // Sanity check for developers.

	utf8char_t sign_symbol = 0;

	if ('+' == utf8char || '-' == utf8char)
	{
		utf8char = next_utf8char(lexer, NULL, true);
		sign_symbol = utf8char;
	}

	if ('0' == utf8char)
	{
		utf8char = next_utf8char(lexer, NULL, true);

		if (is_symbol_first_of_numeric_literal(utf8char))
		{
			log_lexer_error_and_exit(token->location, "leading zero in base 10 numeric literal.");
		}

		if ('b' == utf8char)
		{
			if (sign_symbol != 0)
			{
				log_lexer_error_and_exit(token->location, "sign symbol `%c` before binary notation is not allowed.", (char)sign_symbol);
			}

			state = base_bin | 1 << flag_dig;
			base = 2;
		}
		else if ('o' == utf8char)
		{
			if (sign_symbol != 0)
			{
				log_lexer_error_and_exit(token->location, "sign symbol `%c` before octal notation is not allowed.", (char)sign_symbol);
			}

			state = base_oct | 1 << flag_dig;
			base = 8;
		}
		else if ('x' == utf8char)
		{
			if (sign_symbol != 0)
			{
				log_lexer_error_and_exit(token->location, "sign symbol `%c` before hexadecimal notation is not allowed.", (char)sign_symbol);
			}

			state = base_hex | 1 << flag_dig;
			base = 16;
		}
	}

	if (state != base_dec)
	{
		last = utf8char;
		utf8char = next_utf8char(lexer, NULL, true);
	}

	uint64_t exponent_start = 0;
	uint64_t suffix_start = 0;

	do
	{
		if (mirac_utils_strchr(chrs[state & base_mask], (int32_t)utf8char))
		{
			state &= ~(1 << flag_dig);
			last = utf8char;
			continue;
		}
		else if (utf8char > 0x7F || !mirac_utils_strchr(matching_states[utf8char], state))
		{
			goto end;
		}

		old_state = state;

		switch (utf8char)
		{
			case '.':
			{
				if (lexer->require_int)
				{
					goto want_int;
				}

				state |= 1 << flag_flt;
			} break;

			case '-':
			case 'p':
			case 'P':
			{
				state |= 1 << flag_flt;
			} /* fallthrough */

			case 'e':
			case 'E':
			case '+':
			{
				state |= base_dec | 1 << flag_exp;
				exponent_start = lexer->buffer.length - 1;
			} break;

			case 'f':
			{
				state |= 1 << flag_flt;
			} /* fallthrough */

			case 'i':
			case 'u':
			{
				state |= base_dec | 1 << flag_suff;
				suffix_start = lexer->buffer.length - 1;
			} break;

			default:
			{
				goto end;
			} break;
		}

		if (state & 1 << flag_flt && lexer->require_int)
		{
			log_lexer_error_and_exit(token->location, "expected integer literal.");
		}

		last = utf8char;
		state |= 1 << flag_dig;
	} while ((utf8char = next_utf8char(lexer, NULL, true)) != mirac_utf8_invalid);

	last = 0;

end:
	if (last && !mirac_utils_strchr("iu", (int32_t)last) &&
		!mirac_utils_strchr(chrs[state & base_mask], (int32_t)last))
	{
		state = old_state;
		push_utf8char(lexer, utf8char, true);
		push_utf8char(lexer, last, true);
	}
	else if (utf8char != mirac_utf8_invalid && !is_symbol_a_white_space(utf8char))
	{
		log_lexer_error_and_exit(lexer->location, "invalid symbol `%c` after numeric literal.", (char)utf8char);

want_int:
		push_utf8char(lexer, utf8char, true);
	}

	if (sign_symbol != 0 && lexer->buffer.length < 3)
	{
		log_lexer_error_and_exit(lexer->location, "missing the numeric literal.");
	}

	lexer->require_int = false;

	typedef enum
	{
		kind_unknown = -1,
		kind_iconst,
		kind_signed,
		kind_unsigned,
		kind_float,
	} kind_e;

	kind_e kind = kind_unknown;

	static const struct
	{
		const char suffix[4];
		uint8_t suffix_length;
		kind_e kind;
		mirac_token_type_e type;
	} literals[] =
	{
		{ "i8",  2, kind_signed,   mirac_token_type_literal_i8  },
		{ "i16", 3, kind_signed,   mirac_token_type_literal_i16 },
		{ "i32", 3, kind_signed,   mirac_token_type_literal_i32 },
		{ "i64", 3, kind_signed,   mirac_token_type_literal_i64 },
		{ "u8",  2, kind_unsigned, mirac_token_type_literal_u8  },
		{ "u16", 3, kind_unsigned, mirac_token_type_literal_u16 },
		{ "u32", 3, kind_unsigned, mirac_token_type_literal_u32 },
		{ "u64", 3, kind_unsigned, mirac_token_type_literal_u64 },
		{ "f32", 3, kind_float,    mirac_token_type_literal_f32 },
		{ "f64", 3, kind_float,    mirac_token_type_literal_f64 }
	};

	if (suffix_start)
	{
		for (uint8_t index = 0; index < (sizeof(literals) / sizeof(literals[0])); ++index)
		{
			if (!mirac_utils_strncmp(literals[index].suffix, lexer->buffer.data + suffix_start, literals[index].suffix_length))
			{
				token->type = literals[index].type;
				kind = literals[index].kind;
				break;
			}
		}

		if (kind_unknown == kind)
		{
			log_lexer_error_and_exit(
				token->location, "invalid suffix `%s`.", lexer->buffer.data + suffix_start
			);
		}
	}

	if (state & 1 << flag_flt)
	{
		if (kind_unknown == kind)
		{
			token->type = mirac_token_type_literal_f64;
		}
		else if (kind != kind_float)
		{
			log_lexer_error_and_exit(token->location, "unexpected decimal point in integer literal");
		}

		token->fval = strtod(lexer->buffer.data, NULL);
		clear_buffer(lexer);
		return true;
	}

	if (kind_unknown == kind)
	{
		kind = kind_iconst;
		token->type = mirac_token_type_literal_i64;
	}

	uint64_t exponent = 0;
	errno = 0;

	if (exponent_start)
	{
		exponent = strtoumax(lexer->buffer.data + exponent_start + 1, NULL, 10);
	}

	const uint64_t offset = (uint64_t)(10 == base ? 0 : 2) + (sign_symbol != 0 ? 1 : 0);
	token->uval = strtoumax(lexer->buffer.data + offset, NULL, base);
	token->uval = compute_exponent(token->uval, exponent, kind_signed == kind);

	if (ERANGE == errno)
	{
		log_lexer_error_and_exit(token->location, "numeric literal overflow.");
	}

	if (kind_iconst == kind && token->uval > (uint64_t)INT64_MAX)
	{
		token->type = mirac_token_type_literal_u64;
	}
	else if (kind_signed == kind && (uint64_t)INT64_MIN == token->uval)
	{
		token->ival = INT64_MIN;
	}
	else if (kind != kind_unsigned)
	{
		token->ival = (int64_t)token->uval;

		if ('-' == sign_symbol)
		{
			token->ival *= -1;
		}
	}

	clear_buffer(lexer);
	return true;
}

static uint8_t lex_possible_rune(
	mirac_lexer_s* const lexer,
	char* const rune)
{
	mirac_debug_assert(lexer != NULL);
	mirac_debug_assert(rune != NULL);

	utf8char_t utf8char = next_utf8char(lexer, NULL, false);
	mirac_debug_assert(utf8char != mirac_utf8_invalid);

	switch (utf8char)
	{
		case '\\':
		{
			const mirac_location_s location = lexer->location;
			utf8char = next_utf8char(lexer, NULL, false);

			char buffer[9];
			char* end_pointer = NULL;

			switch (utf8char)
			{
				case '0':
				{
					rune[0] = '\0';
					return 1;
				} break;

				case 'a':
				{
					rune[0] = '\a';
					return 1;
				} break;

				case 'b':
				{
					rune[0] = '\b';
					return 1;
				} break;

				case 'f':
				{
					rune[0] = '\f';
					return 1;
				} break;

				case 'n':
				{
					rune[0] = '\n';
					return 1;
				} break;

				case 'r':
				{
					rune[0] = '\r';
					return 1;
				} break;

				case 't':
				{
					rune[0] = '\t';
					return 1;
				} break;

				case 'v':
				{
					rune[0] = '\v';
					return 1;
				} break;

				case '\\':
				{
					rune[0] = '\\';
					return 1;
				} break;

				case '\'':
				{
					rune[0] = '\'';
					return 1;
				} break;

				case '\"':
				{
					rune[0] = '\"';
					return 1;
				} break;

				case 'x':
				{
					buffer[0] = (char)next_utf8char(lexer, NULL, false);
					buffer[1] = (char)next_utf8char(lexer, NULL, false);
					buffer[2] = '\0';

					utf8char = (utf8char_t)strtoul(&buffer[0], &end_pointer, 16);

					if (*end_pointer != '\0')
					{
						log_lexer_error_and_exit(location, "invalid hex literal.");
					}

					rune[0] = (char)utf8char;
					return 1;
				} break;

				case 'u':
				{
					buffer[0] = (char)next_utf8char(lexer, NULL, false);
					buffer[1] = (char)next_utf8char(lexer, NULL, false);
					buffer[2] = (char)next_utf8char(lexer, NULL, false);
					buffer[3] = (char)next_utf8char(lexer, NULL, false);
					buffer[4] = '\0';

					utf8char = (utf8char_t)strtoul(&buffer[0], &end_pointer, 16);

					if (*end_pointer != '\0')
					{
						log_lexer_error_and_exit(location, "invalid hex literal.");
					}

					return mirac_utf8_encode(rune, utf8char);
				} break;

				case 'U':
				{
					buffer[0] = (char)next_utf8char(lexer, NULL, false);
					buffer[1] = (char)next_utf8char(lexer, NULL, false);
					buffer[2] = (char)next_utf8char(lexer, NULL, false);
					buffer[3] = (char)next_utf8char(lexer, NULL, false);
					buffer[4] = (char)next_utf8char(lexer, NULL, false);
					buffer[5] = (char)next_utf8char(lexer, NULL, false);
					buffer[6] = (char)next_utf8char(lexer, NULL, false);
					buffer[7] = (char)next_utf8char(lexer, NULL, false);
					buffer[8] = '\0';

					utf8char = (utf8char_t)strtoul(&buffer[0], &end_pointer, 16);

					if (*end_pointer != '\0')
					{
						log_lexer_error_and_exit(location, "invalid hex literal.");
					}

					return mirac_utf8_encode(rune, utf8char);
				} break;

				case mirac_utf8_invalid:
				{
					log_lexer_error_and_exit(lexer->location, "unexpected end of file.");
				} break;

				default:
				{
					log_lexer_error_and_exit(location, "invalid escape '\\%c'.", (char)utf8char);
				} break;
			}

			mirac_debug_assert(0);
			return 0;
		} break;

		default:
		{
			return mirac_utf8_encode(rune, utf8char);
		} break;
	}

	mirac_debug_assert(0);
	return 0;
}

static mirac_token_type_e lex_string_literal_token(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token)
{
	utf8char_t delimeter = mirac_utf8_invalid;
	utf8char_t utf8char = next_utf8char(lexer, &token->location, false);
	char buffer[mirac_utf8_max_size + 1];

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	mirac_debug_assert(
		utf8char != mirac_utf8_invalid && '\"' == utf8char
	); // Sanity check for developers.

	switch (utf8char)
	{
		case '\"':
		{
			delimeter = utf8char;

			while ((utf8char = next_utf8char(lexer, NULL, false)) != delimeter)
			{
				if (utf8char == mirac_utf8_invalid)
				{
					log_lexer_error_and_exit(lexer->location, "unexpected end of file.");
				}

				push_utf8char(lexer, utf8char, false);

				if ('\"' == delimeter)
				{
					const uint8_t size = lex_possible_rune(lexer, buffer);
					append_buffer(lexer, buffer, size);
				}
				else
				{
					next_utf8char(lexer, NULL, true);
				}
			}

			char* const string = mirac_utils_malloc((lexer->buffer.length + 1) * sizeof(char));
			mirac_utils_memcpy(string, lexer->buffer.data, lexer->buffer.length);

			utf8char_t suffix = next_utf8char(lexer, NULL, true);
			if ('c' == suffix)
			{
				token->type = mirac_token_type_literal_cstr;
				suffix = next_utf8char(lexer, NULL, true);
			}
			else
			{
				token->type = mirac_token_type_literal_str;
			}

			if (!is_symbol_a_white_space(suffix))
			{
				log_lexer_error_and_exit(lexer->location, "invalid suffix `%c` after string literal.", (char)suffix);
			}

			token->str.data = string;
			token->str.length = lexer->buffer.length;

			clear_buffer(lexer);
			return token->type;
		} break;

		default:
		{
			// NOTE: Should never ever happen as this function will get symbols
			//       that are already verified to be correct ones!
			mirac_debug_assert(0); // Sanity check for developers.
			return mirac_token_type_none; // To prevent compiler error.
		} break;
	}

	// NOTE: Should never ever happen as this function will get symbols
	//       that are already verified to be correct ones!
	mirac_debug_assert(0); // Sanity check for developers.
	return mirac_token_type_none; // To prevent compiler error.
}
