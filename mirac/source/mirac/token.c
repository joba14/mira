
/**
 * @file token.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#include <mirac/token.h>

#include <mirac/debug.h>
#include <mirac/logger.h>
#include <mirac/logger.h>
#include <mirac/utils.h>

#include <stddef.h>
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
	[mirac_token_type_keyword_i16] = "i16",
	[mirac_token_type_keyword_i32] = "i32",
	[mirac_token_type_keyword_i64] = "i64",
	[mirac_token_type_keyword_i8] = "i8",
	[mirac_token_type_keyword_if] = "if",
	[mirac_token_type_keyword_land] = "land",
	[mirac_token_type_keyword_ld16] = "ld16",
	[mirac_token_type_keyword_ld32] = "ld32",
	[mirac_token_type_keyword_ld64] = "ld64",
	[mirac_token_type_keyword_ld8] = "ld8",
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
	[mirac_token_type_keyword_st16] = "st16",
	[mirac_token_type_keyword_st32] = "st32",
	[mirac_token_type_keyword_st64] = "st64",
	[mirac_token_type_keyword_st8] = "st8",
	[mirac_token_type_keyword_sub] = "sub",
	[mirac_token_type_keyword_swap] = "swap",
	[mirac_token_type_keyword_u16] = "u16",
	[mirac_token_type_keyword_u32] = "u32",
	[mirac_token_type_keyword_u64] = "u64",
	[mirac_token_type_keyword_u8] = "u8"
};

static int32_t compare_keyword_tokens(
	const void* const left,
	const void* const right);

mirac_token_type_e mirac_token_type_from_string(
	const char* const string)
{
	const void* const found_token = (const void* const)mirac_utils_bsearch(
		&string, g_token_type_to_string_map, mirac_token_type_keywords_count + 1,
		sizeof(g_token_type_to_string_map[0]), compare_keyword_tokens
	);

	return !found_token ? mirac_token_type_identifier :
		(mirac_token_type_e)((const char**)found_token - g_token_type_to_string_map);
}

const char* mirac_token_type_to_string(
	const mirac_token_type_e type)
{
	switch (type)
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

		case mirac_token_type_invalid:
		{
			return "invalid";
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
			mirac_debug_assert(type < (sizeof(g_token_type_to_string_map) / sizeof(g_token_type_to_string_map[0])));
			const char* const stringified_type = (const char* const)g_token_type_to_string_map[type];
			mirac_debug_assert(stringified_type != NULL);
			return stringified_type;
		} break;
	}
}

mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e type,
	const mirac_location_s location)
{
	mirac_token_s token;
	mirac_utils_memset(
		(void* const)&token, 0, sizeof(mirac_token_s)
	);

	token.type = type;
	token.location = location;
	return token;
}

mirac_token_s mirac_token_from_type(
	const mirac_token_type_e type)
{
	mirac_token_s token;
	mirac_utils_memset(
		(void* const)&token, 0, sizeof(mirac_token_s)
	);

	token.type = type;
	return token;
}

void mirac_token_destroy(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);

	switch (token->type)
	{
		case mirac_token_type_literal_str:
		{
			mirac_utils_free(token->str.data);
		} break;

		case mirac_token_type_identifier:
		{
			mirac_utils_free(token->ident.data);
		} break;
	
		default:
		{
		} break;
	}

	mirac_utils_memset(
		(void* const)token, 0, sizeof(mirac_token_s));
	token->type = mirac_token_type_none;
}

const char* mirac_token_to_string(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	#define token_string_buffer_capacity 1024
	static char token_string_buffer[token_string_buffer_capacity + 1];
	token_string_buffer[0] = 0;

	uint64_t written = (uint64_t)snprintf(
		token_string_buffer, token_string_buffer_capacity,
		"Token[type=`%s`, location=`" mirac_location_fmt "`",
		mirac_token_type_to_string(token->type),
		mirac_location_arg(token->location)
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
				", value=`%li`]", token->ival
			);
		} break;

		case mirac_token_type_literal_u8:
		case mirac_token_type_literal_u16:
		case mirac_token_type_literal_u32:
		case mirac_token_type_literal_u64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value=`%lu`]", token->uval
			);
		} break;

		case mirac_token_type_literal_f32:
		case mirac_token_type_literal_f64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value=`%Lf`]", token->fval
			);
		} break;

		case mirac_token_type_literal_str:
		case mirac_token_type_literal_cstr:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value=`%.*s`]", (signed int)token->str.length, token->str.data
			);
		} break;

		case mirac_token_type_identifier:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value=`%.*s`]", (signed int)token->ident.length, token->ident.data
			);
		} break;
	
		default:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written, "]"
			);
		} break;
	}

	token_string_buffer[written] = 0;
	return token_string_buffer;
}

static int32_t compare_keyword_tokens(
	const void* const left,
	const void* const right)
{
	return mirac_utils_strcmp(
		*(const char**)left, *(const char**)right
	);
}
