
/**
 * @file token.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-02
 */

#include <mirac/token.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

#include <stdio.h>

static const char* const g_token_type_to_string_map[] =
{
	[mirac_token_type_keyword_add] = "add",
	[mirac_token_type_keyword_as] = "as",
	[mirac_token_type_keyword_band] = "band",
	[mirac_token_type_keyword_bnot] = "bnot",
	[mirac_token_type_keyword_bor] = "bor",
	[mirac_token_type_keyword_bxor] = "bxor",
	[mirac_token_type_keyword_div] = "div",
	[mirac_token_type_keyword_do] = "do",
	[mirac_token_type_keyword_drop] = "drop",
	[mirac_token_type_keyword_dup] = "dup",
	[mirac_token_type_keyword_elif] = "elif",
	[mirac_token_type_keyword_else] = "else",
	[mirac_token_type_keyword_end] = "end",
	[mirac_token_type_keyword_eq] = "eq",
	[mirac_token_type_keyword_f32] = "f32",
	[mirac_token_type_keyword_f64] = "f64",
	[mirac_token_type_keyword_func] = "func",
	[mirac_token_type_keyword_gt] = "gt",
	[mirac_token_type_keyword_gteq] = "gteq",
	[mirac_token_type_keyword_i08] = "i08",
	[mirac_token_type_keyword_i16] = "i16",
	[mirac_token_type_keyword_i32] = "i32",
	[mirac_token_type_keyword_i64] = "i64",
	[mirac_token_type_keyword_if] = "if",
	[mirac_token_type_keyword_inl] = "inl",
	[mirac_token_type_keyword_land] = "land",
	[mirac_token_type_keyword_ld08] = "ld08",
	[mirac_token_type_keyword_ld16] = "ld16",
	[mirac_token_type_keyword_ld32] = "ld32",
	[mirac_token_type_keyword_ld64] = "ld64",
	[mirac_token_type_keyword_let] = "let",
	[mirac_token_type_keyword_lnot] = "lnot",
	[mirac_token_type_keyword_loop] = "loop",
	[mirac_token_type_keyword_lor] = "lor",
	[mirac_token_type_keyword_ls] = "ls",
	[mirac_token_type_keyword_lseq] = "lseq",
	[mirac_token_type_keyword_lxor] = "lxor",
	[mirac_token_type_keyword_mem] = "mem",
	[mirac_token_type_keyword_mod] = "mod",
	[mirac_token_type_keyword_mul] = "mul",
	[mirac_token_type_keyword_neq] = "neq",
	[mirac_token_type_keyword_over] = "over",
	[mirac_token_type_keyword_ptr] = "ptr",
	[mirac_token_type_keyword_reg] = "reg",
	[mirac_token_type_keyword_req] = "req",
	[mirac_token_type_keyword_ret] = "ret",
	[mirac_token_type_keyword_rot] = "rot",
	[mirac_token_type_keyword_shl] = "shl",
	[mirac_token_type_keyword_shr] = "shr",
	[mirac_token_type_keyword_sys1] = "sys1",
	[mirac_token_type_keyword_sys2] = "sys2",
	[mirac_token_type_keyword_sys3] = "sys3",
	[mirac_token_type_keyword_sys4] = "sys4",
	[mirac_token_type_keyword_sys5] = "sys5",
	[mirac_token_type_keyword_sys6] = "sys6",
	[mirac_token_type_keyword_st08] = "st08",
	[mirac_token_type_keyword_st16] = "st16",
	[mirac_token_type_keyword_st32] = "st32",
	[mirac_token_type_keyword_st64] = "st64",
	[mirac_token_type_keyword_sub] = "sub",
	[mirac_token_type_keyword_swap] = "swap",
	[mirac_token_type_keyword_u08] = "u08",
	[mirac_token_type_keyword_u16] = "u16",
	[mirac_token_type_keyword_u32] = "u32",
	[mirac_token_type_keyword_u64] = "u64"
};

/**
 * @brief Compare two keyword tokens if they are the same or not.
 */
static int32_t compare_keyword_tokens(
	const void* const left,
	const void* const right);

mirac_token_type_e mirac_token_type_from_string(
	const char* const string)
{
	const void* const found_token = (const void* const)mirac_c_bsearch(
		&string, g_token_type_to_string_map, mirac_token_type_keywords_count + 1,
		sizeof(g_token_type_to_string_map[0]), compare_keyword_tokens
	);

	return !found_token ? mirac_token_type_identifier :
		(mirac_token_type_e)((const char**)found_token - g_token_type_to_string_map);
}

const char* mirac_token_type_to_string(
	const mirac_token_type_e token_type)
{
	switch (token_type)
	{
		case mirac_token_type_literal_i8:
		{
			return "literal_i8";
		} break;

		case mirac_token_type_literal_i16:
		{
			return "literal_i16";
		} break;

		case mirac_token_type_literal_i32:
		{
			return "literal_i32";
		} break;

		case mirac_token_type_literal_i64:
		{
			return "literal_i64";
		} break;

		case mirac_token_type_literal_u8:
		{
			return "literal_u8";
		} break;

		case mirac_token_type_literal_u16:
		{
			return "literal_u16";
		} break;

		case mirac_token_type_literal_u32:
		{
			return "literal_u32";
		} break;

		case mirac_token_type_literal_u64:
		{
			return "literal_u64";
		} break;

		case mirac_token_type_literal_f32:
		{
			return "literal_f32";
		} break;

		case mirac_token_type_literal_f64:
		{
			return "literal_f64";
		} break;

		case mirac_token_type_literal_str:
		{
			return "literal_str";
		} break;

		case mirac_token_type_literal_cstr:
		{
			return "literal_cstr";
		} break;

		case mirac_token_type_identifier:
		{
			return "identifier";
		} break;

		case mirac_token_type_eof:
		{
			return "eof";
		} break;

		case mirac_token_type_none:
		{
			return "none";
		} break;

		default:
		{
			mirac_debug_assert(token_type < (sizeof(g_token_type_to_string_map) / sizeof(g_token_type_to_string_map[0])));
			const char* const stringified_type = (const char* const)g_token_type_to_string_map[token_type];
			mirac_debug_assert(stringified_type != NULL);
			return stringified_type;
		} break;
	}
}

mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e token_type,
	const mirac_location_s location)
{
	mirac_token_s token;
	mirac_c_memset(
		(void* const)&token, 0, sizeof(mirac_token_s)
	);

	token.type = token_type;
	token.location = location;
	return token;
}

mirac_token_s mirac_token_from_type(
	const mirac_token_type_e token_type)
{
	mirac_token_s token;
	mirac_c_memset(
		(void* const)&token, 0, sizeof(mirac_token_s)
	);

	token.type = token_type;
	return token;
}

void mirac_token_destroy(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	mirac_c_memset((void* const)token, 0, sizeof(mirac_token_s));
	token->type = mirac_token_type_none;
}

bool mirac_token_is_type_keyword(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	return (
		(mirac_token_type_keyword_i08 == token->type) ||
		(mirac_token_type_keyword_i16 == token->type) ||
		(mirac_token_type_keyword_i32 == token->type) ||
		(mirac_token_type_keyword_i64 == token->type) ||
		(mirac_token_type_keyword_u08 == token->type) ||
		(mirac_token_type_keyword_u16 == token->type) ||
		(mirac_token_type_keyword_u32 == token->type) ||
		(mirac_token_type_keyword_u64 == token->type) ||
		(mirac_token_type_keyword_f32 == token->type) ||
		(mirac_token_type_keyword_f64 == token->type) ||
		(mirac_token_type_keyword_ptr == token->type)
	);
}

bool mirac_token_is_signed_integer_literal(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	return (
		(mirac_token_type_keyword_i08 == token->type) ||
		(mirac_token_type_keyword_i16 == token->type) ||
		(mirac_token_type_keyword_i32 == token->type) ||
		(mirac_token_type_keyword_i64 == token->type)
	);
}

bool mirac_token_is_unsigned_integer_literal(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	return (
		(mirac_token_type_keyword_u08 == token->type) ||
		(mirac_token_type_keyword_u16 == token->type) ||
		(mirac_token_type_keyword_u32 == token->type) ||
		(mirac_token_type_keyword_u64 == token->type)
	);
}

bool mirac_token_is_string_literal(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	return (
		mirac_token_type_literal_str == token->type ||
		mirac_token_type_literal_cstr == token->type
	);
}

const char* mirac_token_to_string(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	#define token_string_buffer_capacity 4096
	static char token_string_buffer[token_string_buffer_capacity + 1];
	token_string_buffer[0] = 0;

	uint64_t written = (uint64_t)snprintf(
		token_string_buffer, token_string_buffer_capacity,
		"Token[type='%s', location='" mirac_location_fmt "', index='%lu', source='%.*s'",
		mirac_token_type_to_string(token->type),
		mirac_location_arg(token->location),
		token->index,
		(signed int)token->source.length,
		token->source.data
	);

	switch (token->type)
	{
		case mirac_token_type_literal_i8:
		case mirac_token_type_literal_i16:
		case mirac_token_type_literal_i32:
		case mirac_token_type_literal_i64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%li'", token->as_ival
			);
		} break;

		case mirac_token_type_literal_u8:
		case mirac_token_type_literal_u16:
		case mirac_token_type_literal_u32:
		case mirac_token_type_literal_u64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%lu'", token->as_uval
			);
		} break;

		case mirac_token_type_literal_f32:
		case mirac_token_type_literal_f64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%Lf'", token->as_fval
			);
		} break;

		case mirac_token_type_literal_str:
		case mirac_token_type_literal_cstr:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%.*s'", (signed int)token->as_str.length, token->as_str.data
			);
		} break;

		case mirac_token_type_identifier:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%.*s'", (signed int)token->as_ident.length, token->as_ident.data
			);
		} break;

		default:
		{
		} break;
	}

	if (token->prev_ref != NULL)
	{
		written += (uint64_t)snprintf(
			token_string_buffer + written, token_string_buffer_capacity - written,
			", prev_ref='TokenRef[type='%s', index='%lu']'",
			mirac_token_type_to_string(token->prev_ref->type),
			token->prev_ref->index
		);
	}

	if (token->next_ref != NULL)
	{
		written += (uint64_t)snprintf(
			token_string_buffer + written, token_string_buffer_capacity - written,
			", next_ref='TokenRef[type='%s', index='%lu']'",
			mirac_token_type_to_string(token->next_ref->type),
			token->next_ref->index
		);
	}

	written += (uint64_t)snprintf(
		token_string_buffer + written, token_string_buffer_capacity - written, "]"
	);

	token_string_buffer[written] = 0;
	return token_string_buffer;
}

void mirac_token_print(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);

	(void)printf(
		"Token[type='%s', location='" mirac_location_fmt "', index='%lu', source='%.*s'",
		mirac_token_type_to_string(token->type),
		mirac_location_arg(token->location),
		token->index,
		(signed int)token->source.length, token->source.data
	);

	switch (token->type)
	{
		case mirac_token_type_literal_i8:
		case mirac_token_type_literal_i16:
		case mirac_token_type_literal_i32:
		case mirac_token_type_literal_i64:
		{
			(void)printf(", value='%li']", token->as_ival);
		} break;

		case mirac_token_type_literal_u8:
		case mirac_token_type_literal_u16:
		case mirac_token_type_literal_u32:
		case mirac_token_type_literal_u64:
		{
			(void)printf(", value='%lu']", token->as_uval);
		} break;

		case mirac_token_type_literal_f32:
		case mirac_token_type_literal_f64:
		{
			(void)printf(", value='%Lf']", token->as_fval);
		} break;

		case mirac_token_type_literal_str:
		case mirac_token_type_literal_cstr:
		{
			(void)printf(", value='%.*s']", (signed int)token->as_str.length, token->as_str.data);
		} break;

		case mirac_token_type_identifier:
		{
			(void)printf(", value='%.*s']", (signed int)token->as_ident.length, token->as_ident.data);
		} break;

		default:
		{
		} break;
	}

	if (token->prev_ref != NULL)
	{
		(void)printf(", prev_ref='TokenRef[type='%s', index='%lu']'",
			mirac_token_type_to_string(token->prev_ref->type), token->prev_ref->index
		);
	}

	if (token->next_ref != NULL)
	{
		(void)printf(", next_ref='TokenRef[type='%s', index='%lu']'",
			mirac_token_type_to_string(token->next_ref->type), token->next_ref->index
		);
	}

	(void)printf("]\n");
}

static int32_t compare_keyword_tokens(
	const void* const left,
	const void* const right)
{
	return mirac_c_strcmp(
		*(const char**)left, *(const char**)right
	);
}
