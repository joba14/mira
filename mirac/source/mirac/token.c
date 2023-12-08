
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
#include <mirac/string_view.h>

#include <stdio.h>

static const string_view_s g_reserved_token_types_map[] =
{
	[mirac_token_type_reserved_lnot] = string_view_static("!"),
	[mirac_token_type_reserved_neq] = string_view_static("!="),
	[mirac_token_type_reserved_mod] = string_view_static("%"),
	[mirac_token_type_reserved_band] = string_view_static("&"),
	[mirac_token_type_reserved_land] = string_view_static("&&"),
	[mirac_token_type_reserved_mul] = string_view_static("*"),
	[mirac_token_type_reserved_add] = string_view_static("+"),
	[mirac_token_type_reserved_sub] = string_view_static("-"),
	[mirac_token_type_reserved_div] = string_view_static("/"),
	[mirac_token_type_reserved_ls] = string_view_static("<"),
	[mirac_token_type_reserved_shl] = string_view_static("<<"),
	[mirac_token_type_reserved_lseq] = string_view_static("<="),
	[mirac_token_type_reserved_eq] = string_view_static("=="),
	[mirac_token_type_reserved_gt] = string_view_static(">"),
	[mirac_token_type_reserved_gteq] = string_view_static(">="),
	[mirac_token_type_reserved_shr] = string_view_static(">>"),
	[mirac_token_type_reserved_bxor] = string_view_static("^"),
	[mirac_token_type_reserved_lxor] = string_view_static("^^"),
	[mirac_token_type_reserved_as] = string_view_static("as"),
	[mirac_token_type_reserved_do] = string_view_static("do"),
	[mirac_token_type_reserved_drop] = string_view_static("drop"),
	[mirac_token_type_reserved_dup] = string_view_static("dup"),
	[mirac_token_type_reserved_elif] = string_view_static("elif"),
	[mirac_token_type_reserved_else] = string_view_static("else"),
	[mirac_token_type_reserved_end] = string_view_static("end"),
	[mirac_token_type_reserved_f32] = string_view_static("f32"),
	[mirac_token_type_reserved_f64] = string_view_static("f64"),
	[mirac_token_type_reserved_func] = string_view_static("func"),
	[mirac_token_type_reserved_i08] = string_view_static("i08"),
	[mirac_token_type_reserved_i16] = string_view_static("i16"),
	[mirac_token_type_reserved_i32] = string_view_static("i32"),
	[mirac_token_type_reserved_i64] = string_view_static("i64"),
	[mirac_token_type_reserved_if] = string_view_static("if"),
	[mirac_token_type_reserved_inl] = string_view_static("inl"),
	[mirac_token_type_reserved_ld08] = string_view_static("ld08"),
	[mirac_token_type_reserved_ld16] = string_view_static("ld16"),
	[mirac_token_type_reserved_ld32] = string_view_static("ld32"),
	[mirac_token_type_reserved_ld64] = string_view_static("ld64"),
	[mirac_token_type_reserved_let] = string_view_static("let"),
	[mirac_token_type_reserved_loop] = string_view_static("loop"),
	[mirac_token_type_reserved_mem] = string_view_static("mem"),
	[mirac_token_type_reserved_over] = string_view_static("over"),
	[mirac_token_type_reserved_ptr] = string_view_static("ptr"),
	[mirac_token_type_reserved_reg] = string_view_static("reg"),
	[mirac_token_type_reserved_req] = string_view_static("req"),
	[mirac_token_type_reserved_ret] = string_view_static("ret"),
	[mirac_token_type_reserved_rot] = string_view_static("rot"),
	[mirac_token_type_reserved_st08] = string_view_static("st08"),
	[mirac_token_type_reserved_st16] = string_view_static("st16"),
	[mirac_token_type_reserved_st32] = string_view_static("st32"),
	[mirac_token_type_reserved_st64] = string_view_static("st64"),
	[mirac_token_type_reserved_swap] = string_view_static("swap"),
	[mirac_token_type_reserved_sys1] = string_view_static("sys1"),
	[mirac_token_type_reserved_sys2] = string_view_static("sys2"),
	[mirac_token_type_reserved_sys3] = string_view_static("sys3"),
	[mirac_token_type_reserved_sys4] = string_view_static("sys4"),
	[mirac_token_type_reserved_sys5] = string_view_static("sys5"),
	[mirac_token_type_reserved_sys6] = string_view_static("sys6"),
	[mirac_token_type_reserved_u08] = string_view_static("u08"),
	[mirac_token_type_reserved_u16] = string_view_static("u16"),
	[mirac_token_type_reserved_u32] = string_view_static("u32"),
	[mirac_token_type_reserved_u64] = string_view_static("u64"),
	[mirac_token_type_reserved_bor] = string_view_static("|"),
	[mirac_token_type_reserved_lor] = string_view_static("||"),
	[mirac_token_type_reserved_bnot] = string_view_static("~")
};

/**
 * @brief Compare two keyword tokens if they are the same or not.
 */
static int32_t compare_reserved_tokens(
	const void* const left,
	const void* const right);

mirac_token_type_e mirac_reserved_token_type_from_string_view(
	const string_view_s string_view)
{
	const void* const found_token = (const void* const)mirac_c_bsearch(
		&string_view, g_reserved_token_types_map, mirac_token_type_reserved_count + 1,
		sizeof(g_reserved_token_types_map[0]), compare_reserved_tokens
	);

	return !found_token ? mirac_token_type_none :
		(mirac_token_type_e)((const char**)found_token - g_reserved_token_types_map);
}

const char* mirac_token_type_to_string(
	const mirac_token_type_e token_type)
{
	switch (token_type)
	{
		case mirac_token_type_literal_i08:
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

		case mirac_token_type_literal_u08:
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
			mirac_debug_assert(token_type < (sizeof(g_reserved_token_types_map) / sizeof(g_reserved_token_types_map[0])));
			const string_view_s stringified_type = g_reserved_token_types_map[token_type];
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
	mirac_c_memset((void* const)&token, 0, sizeof(mirac_token_s));
	token.type = token_type;
	token.location = location;
	return token;
}

mirac_token_s mirac_token_from_type(
	const mirac_token_type_e token_type)
{
	mirac_token_s token;
	mirac_c_memset((void* const)&token, 0, sizeof(mirac_token_s));
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
		(mirac_token_type_reserved_i08 == token->type) ||
		(mirac_token_type_reserved_i16 == token->type) ||
		(mirac_token_type_reserved_i32 == token->type) ||
		(mirac_token_type_reserved_i64 == token->type) ||
		(mirac_token_type_reserved_u08 == token->type) ||
		(mirac_token_type_reserved_u16 == token->type) ||
		(mirac_token_type_reserved_u32 == token->type) ||
		(mirac_token_type_reserved_u64 == token->type) ||
		(mirac_token_type_reserved_f32 == token->type) ||
		(mirac_token_type_reserved_f64 == token->type) ||
		(mirac_token_type_reserved_ptr == token->type)
	);
}

bool mirac_token_is_signed_integer_literal(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	return (
		(mirac_token_type_literal_i08 == token->type) ||
		(mirac_token_type_literal_i16 == token->type) ||
		(mirac_token_type_literal_i32 == token->type) ||
		(mirac_token_type_literal_i64 == token->type)
	);
}

bool mirac_token_is_unsigned_integer_literal(
	mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	return (
		(mirac_token_type_literal_u08 == token->type) ||
		(mirac_token_type_literal_u16 == token->type) ||
		(mirac_token_type_literal_u32 == token->type) ||
		(mirac_token_type_literal_u64 == token->type)
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
		case mirac_token_type_literal_i08:
		case mirac_token_type_literal_i16:
		case mirac_token_type_literal_i32:
		case mirac_token_type_literal_i64:
		{
			written += (uint64_t)snprintf(
				token_string_buffer + written, token_string_buffer_capacity - written,
				", value='%li'", token->as_ival
			);
		} break;

		case mirac_token_type_literal_u08:
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
		case mirac_token_type_literal_i08:
		case mirac_token_type_literal_i16:
		case mirac_token_type_literal_i32:
		case mirac_token_type_literal_i64:
		{
			(void)printf(", value='%li']", token->as_ival);
		} break;

		case mirac_token_type_literal_u08:
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

static int32_t compare_reserved_tokens(
	const void* const left,
	const void* const right)
{
	return string_view_compare(
		*(const string_view_s*)left, *(const string_view_s*)right
	);
}
