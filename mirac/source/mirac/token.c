
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

#include <ctype.h>
#include <stdio.h>

static const string_view_s g_reserved_token_types_map[] =
{
	[mirac_token_type_reserved_lnot] = string_view_static("!"),
	[mirac_token_type_reserved_neq]  = string_view_static("!="),
	[mirac_token_type_reserved_mod]  = string_view_static("%"),
	[mirac_token_type_reserved_band] = string_view_static("&"),
	[mirac_token_type_reserved_land] = string_view_static("&&"),
	[mirac_token_type_reserved_mul]  = string_view_static("*"),
	[mirac_token_type_reserved_add]  = string_view_static("+"),
	[mirac_token_type_reserved_sub]  = string_view_static("-"),
	[mirac_token_type_reserved_div]  = string_view_static("/"),
	[mirac_token_type_reserved_ls]   = string_view_static("<"),
	[mirac_token_type_reserved_shl]  = string_view_static("<<"),
	[mirac_token_type_reserved_lseq] = string_view_static("<="),
	[mirac_token_type_reserved_eq]   = string_view_static("=="),
	[mirac_token_type_reserved_gt]   = string_view_static(">"),
	[mirac_token_type_reserved_gteq] = string_view_static(">="),
	[mirac_token_type_reserved_shr]  = string_view_static(">>"),
	[mirac_token_type_reserved_bxor] = string_view_static("^"),
	[mirac_token_type_reserved_lxor] = string_view_static("^^"),
	[mirac_token_type_reserved_as]   = string_view_static("as"),
	[mirac_token_type_reserved_do]   = string_view_static("do"),
	[mirac_token_type_reserved_drop] = string_view_static("drop"),
	[mirac_token_type_reserved_dup]  = string_view_static("dup"),
	[mirac_token_type_reserved_elif] = string_view_static("elif"),
	[mirac_token_type_reserved_else] = string_view_static("else"),
	[mirac_token_type_reserved_end]  = string_view_static("end"),
	[mirac_token_type_reserved_f32]  = string_view_static("f32"),
	[mirac_token_type_reserved_f64]  = string_view_static("f64"),
	[mirac_token_type_reserved_func] = string_view_static("func"),
	[mirac_token_type_reserved_i08]  = string_view_static("i08"),
	[mirac_token_type_reserved_i16]  = string_view_static("i16"),
	[mirac_token_type_reserved_i32]  = string_view_static("i32"),
	[mirac_token_type_reserved_i64]  = string_view_static("i64"),
	[mirac_token_type_reserved_if]   = string_view_static("if"),
	[mirac_token_type_reserved_inl]  = string_view_static("inl"),
	[mirac_token_type_reserved_ld08] = string_view_static("ld08"),
	[mirac_token_type_reserved_ld16] = string_view_static("ld16"),
	[mirac_token_type_reserved_ld32] = string_view_static("ld32"),
	[mirac_token_type_reserved_ld64] = string_view_static("ld64"),
	[mirac_token_type_reserved_let]  = string_view_static("let"),
	[mirac_token_type_reserved_loop] = string_view_static("loop"),
	[mirac_token_type_reserved_mem]  = string_view_static("mem"),
	[mirac_token_type_reserved_over] = string_view_static("over"),
	[mirac_token_type_reserved_ptr]  = string_view_static("ptr"),
	[mirac_token_type_reserved_reg]  = string_view_static("reg"),
	[mirac_token_type_reserved_req]  = string_view_static("req"),
	[mirac_token_type_reserved_ret]  = string_view_static("ret"),
	[mirac_token_type_reserved_rot]  = string_view_static("rot"),
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
	[mirac_token_type_reserved_u08]  = string_view_static("u08"),
	[mirac_token_type_reserved_u16]  = string_view_static("u16"),
	[mirac_token_type_reserved_u32]  = string_view_static("u32"),
	[mirac_token_type_reserved_u64]  = string_view_static("u64"),
	[mirac_token_type_reserved_bor]  = string_view_static("|"),
	[mirac_token_type_reserved_lor]  = string_view_static("||"),
	[mirac_token_type_reserved_bnot] = string_view_static("~")
};

string_view_s mirac_token_type_to_string_view(
	const mirac_token_type_e token_type)
{
	switch (token_type)
	{
		case mirac_token_type_literal_i08:  { return string_view_from_cstring("literal_i8");   } break;
		case mirac_token_type_literal_i16:  { return string_view_from_cstring("literal_i16");  } break;
		case mirac_token_type_literal_i32:  { return string_view_from_cstring("literal_i32");  } break;
		case mirac_token_type_literal_i64:  { return string_view_from_cstring("literal_i64");  } break;
		case mirac_token_type_literal_u08:  { return string_view_from_cstring("literal_u8");   } break;
		case mirac_token_type_literal_u16:  { return string_view_from_cstring("literal_u16");  } break;
		case mirac_token_type_literal_u32:  { return string_view_from_cstring("literal_u32");  } break;
		case mirac_token_type_literal_u64:  { return string_view_from_cstring("literal_u64");  } break;
		case mirac_token_type_literal_f32:  { return string_view_from_cstring("literal_f32");  } break;
		case mirac_token_type_literal_f64:  { return string_view_from_cstring("literal_f64");  } break;
		case mirac_token_type_literal_str:  { return string_view_from_cstring("literal_str");  } break;
		case mirac_token_type_literal_cstr: { return string_view_from_cstring("literal_cstr"); } break;
		case mirac_token_type_identifier:   { return string_view_from_cstring("identifier");   } break;
		case mirac_token_type_eof:          { return string_view_from_cstring("eof");          } break;
		case mirac_token_type_none:         { return string_view_from_cstring("none");         } break;

		default:
		{
			mirac_debug_assert(token_type < (sizeof(g_reserved_token_types_map) / sizeof(g_reserved_token_types_map[0])));
			const string_view_s stringified_type = g_reserved_token_types_map[token_type];
			return stringified_type;
		} break;
	}
}

mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e token_type,
	const mirac_location_s location,
	const string_view_s text)
{
	mirac_token_s token = {0};
	token.type = token_type;
	token.location = location;
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

mirac_token_type_e mirac_token_parse_string_literal_from_string_view(
	mirac_arena_s* const arena,
	mirac_token_s* const token,
	const string_view_s string_view)
{
	// TODO: implement!
	(void)arena;
	(void)token;
	(void)string_view;
	return mirac_token_type_none;
}

mirac_token_type_e mirac_token_parse_numeric_literal_from_string_view(
	mirac_arena_s* const arena,
	mirac_token_s* const token,
	const string_view_s string_view)
{
	// TODO: implement!
	(void)arena;
	(void)token;
	(void)string_view;
	return mirac_token_type_none;
}

mirac_token_type_e mirac_token_parse_reserved_token_from_string_view(
	mirac_arena_s* const arena,
	mirac_token_s* const token,
	const string_view_s string_view)
{
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(token != NULL);

	// TODO: optimize the search (binary search probably should used):
	for (uint64_t index = 0; index < mirac_token_type_reserved_count + 1; ++index)
	{
		if (string_view_equal(g_reserved_token_types_map[index], string_view))
		{
			token->type = (mirac_token_type_e)index;
			break;
		}
	}

	// TODO: decide if text and value should be separately allocated!
	const string_view_s reserved_token = g_reserved_token_types_map[token->type];
	char* const reserved = (char* const)mirac_arena_malloc(arena, reserved_token.length);
	mirac_c_memcpy(reserved, reserved_token.data, reserved_token.length);

	token->as_ident = string_view_from_parts(identifier, string_view.length);
	return token->type;

	return mirac_token_type_none;
}

mirac_token_type_e mirac_token_parse_identifier_from_string_view(
	mirac_arena_s* const arena,
	mirac_token_s* const token,
	const string_view_s string_view)
{
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(token != NULL);

	if (string_view.length <= 0)
	{
		return mirac_token_type_none;
	}

	if (isdigit(string_view.data[0]))
	{
		return mirac_token_type_none;
	}

	for (uint64_t index = 0; index < string_view.length; ++index)
	{
		if (('\"' == string_view.data[index]) ||
			('\'' == string_view.data[index]) ||
			('`'  == string_view.data[index]))
		{
			return mirac_token_type_none;
		}
	}

	char* const identifier = (char* const)mirac_arena_malloc(arena, string_view.length);
	mirac_c_memcpy(identifier, string_view.data, string_view.length);

	token->type = mirac_token_type_identifier;
	token->as_ident = string_view_from_parts(identifier, string_view.length);
	return token->type;
}

#if 0
const char* mirac_token_to_string(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	#define token_string_buffer_capacity 4096
	static char token_string_buffer[token_string_buffer_capacity + 1];
	token_string_buffer[0] = 0;

	uint64_t written = (uint64_t)snprintf(
		token_string_buffer, token_string_buffer_capacity,
		"Token[type='" sv_fmt "', location='" mirac_location_fmt "', index='%lu', source='%.*s'",
		sv_arg(mirac_token_type_to_string_view(token->type)),
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
			", prev_ref='TokenRef[type='" sv_fmt "', index='%lu']'",
			sv_arg(mirac_token_type_to_string_view(token->prev_ref->type)),
			token->prev_ref->index
		);
	}

	if (token->next_ref != NULL)
	{
		written += (uint64_t)snprintf(
			token_string_buffer + written, token_string_buffer_capacity - written,
			", next_ref='TokenRef[type='" sv_fmt "', index='%lu']'",
			sv_arg(mirac_token_type_to_string_view(token->next_ref->type)),
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
		"Token[type='" sv_fmt "', location='" mirac_location_fmt "', index='%lu', source='%.*s'",
		sv_arg(mirac_token_type_to_string_view(token->type)),
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

	(void)printf("]\n");
}
#endif
