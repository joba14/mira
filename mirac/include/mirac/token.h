
/**
 * @file token.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-02
 */

#ifndef __mirac__include__mirac__token_h__
#define __mirac__include__mirac__token_h__

#include <mirac/c_common.h>
#include <mirac/string_view.h>

typedef struct
{
	string_view_s file;
	uint64_t line;
	uint64_t column;
} mirac_location_s;

/**
 * @brief Location formatting macro for printf-like functions.
 */
#define mirac_location_fmt sv_fmt ":%lu:%lu"

/**
 * @brief Location formatting argument macro for printf-like functions.
 */
#define mirac_location_arg(_location) \
	sv_arg((_location).file), (_location).line, (_location).column

typedef enum
{
	mirac_token_type_reserved_lnot = 0,
	mirac_token_type_reserved_neq,
	mirac_token_type_reserved_mod,
	mirac_token_type_reserved_band,
	mirac_token_type_reserved_land,
	mirac_token_type_reserved_mul,
	mirac_token_type_reserved_add,
	mirac_token_type_reserved_sub,
	mirac_token_type_reserved_div,
	mirac_token_type_reserved_ls,
	mirac_token_type_reserved_shl,
	mirac_token_type_reserved_lseq,
	mirac_token_type_reserved_eq,
	mirac_token_type_reserved_gt,
	mirac_token_type_reserved_gteq,
	mirac_token_type_reserved_shr,
	mirac_token_type_reserved_bxor,
	mirac_token_type_reserved_lxor,
	mirac_token_type_reserved_as,
	mirac_token_type_reserved_do,
	mirac_token_type_reserved_drop,
	mirac_token_type_reserved_dup,
	mirac_token_type_reserved_elif,
	mirac_token_type_reserved_else,
	mirac_token_type_reserved_end,
	mirac_token_type_reserved_f32,
	mirac_token_type_reserved_f64,
	mirac_token_type_reserved_func,
	mirac_token_type_reserved_i08,
	mirac_token_type_reserved_i16,
	mirac_token_type_reserved_i32,
	mirac_token_type_reserved_i64,
	mirac_token_type_reserved_if,
	mirac_token_type_reserved_inl,
	mirac_token_type_reserved_ld08,
	mirac_token_type_reserved_ld16,
	mirac_token_type_reserved_ld32,
	mirac_token_type_reserved_ld64,
	mirac_token_type_reserved_let,
	mirac_token_type_reserved_loop,
	mirac_token_type_reserved_mem,
	mirac_token_type_reserved_over,
	mirac_token_type_reserved_ptr,
	mirac_token_type_reserved_reg,
	mirac_token_type_reserved_req,
	mirac_token_type_reserved_ret,
	mirac_token_type_reserved_rot,
	mirac_token_type_reserved_st08,
	mirac_token_type_reserved_st16,
	mirac_token_type_reserved_st32,
	mirac_token_type_reserved_st64,
	mirac_token_type_reserved_swap,
	mirac_token_type_reserved_sys1,
	mirac_token_type_reserved_sys2,
	mirac_token_type_reserved_sys3,
	mirac_token_type_reserved_sys4,
	mirac_token_type_reserved_sys5,
	mirac_token_type_reserved_sys6,
	mirac_token_type_reserved_u08,
	mirac_token_type_reserved_u16,
	mirac_token_type_reserved_u32,
	mirac_token_type_reserved_u64,
	mirac_token_type_reserved_bor,
	mirac_token_type_reserved_lor,
	mirac_token_type_reserved_bnot,
	mirac_token_type_reserved_count = mirac_token_type_reserved_bnot,

	// Tokens with additional information
	mirac_token_type_literal_i08,
	mirac_token_type_literal_i16,
	mirac_token_type_literal_i32,
	mirac_token_type_literal_i64,
	mirac_token_type_literal_u08,
	mirac_token_type_literal_u16,
	mirac_token_type_literal_u32,
	mirac_token_type_literal_u64,
	mirac_token_type_literal_f32,
	mirac_token_type_literal_f64,
	mirac_token_type_literal_str,
	mirac_token_type_literal_cstr,

	mirac_token_type_identifier,

	// Magic tokens
	mirac_token_type_eof,
	mirac_token_type_none
} mirac_token_type_e;

/**
 * @brief Stringify token type and return the string view.
 * 
 * @param token_type[in] token type to stringify
 * 
 * @return string_view_s
 */
string_view_s mirac_token_type_to_string_view(
	const mirac_token_type_e token_type);

typedef struct mirac_token_s mirac_token_s;

struct mirac_token_s
{
	mirac_token_type_e type;
	mirac_location_s location;
	uint64_t index;

	union
	{
		int64_t as_ival;
		uint64_t as_uval;
		long double as_fval;
		string_view_s as_str;
		string_view_s as_ident;
	};

	string_view_s text;
};

/**
 * @brief Create token with provided token type and location.
 * 
 * @note All the rest of the fields will be initialized to 0.
 * 
 * @param token_type[in] token type to assign to the new token
 * @param location[in]   location to set to the token
 * @param text[in]       text representation from the file
 * 
 * @return mirac_token_s 
 */
mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e token_type,
	const mirac_location_s location,
	const string_view_s text);

/**
 * @brief Create a token object with provided token type.
 * 
 * @note All the rest of the fields will be initialized to 0.
 * 
 * @param token_type[in] token type to assign to the new token
 * 
 * @return mirac_token_s
 */
mirac_token_s mirac_token_from_type(
	const mirac_token_type_e token_type);

/**
 * @brief Destroy (reset all fields to 0) token.
 * 
 * @note The memory that was allocated for the token's fields will not be freed
 * with this function. It can be freed only with the arena that was used as the
 * allocator in the lexer.
 * 
 * @param token[in] token to destroy
 */
void mirac_token_destroy(
	mirac_token_s* const token);

// TODO: document!
mirac_token_type_e mirac_token_parse_string_literal_from_string_view(
	mirac_token_s* const token,
	const string_view_s string_view);

// TODO: document!
mirac_token_type_e mirac_token_parse_numeric_literal_from_string_view(
	mirac_token_s* const token,
	const string_view_s string_view);

/**
 * @brief Match reserved token type with provided string view.
 * 
 * @note If match is found in the reserved tokens map, "mirac_token_type_none"
 * type will be returned to indicate that the match failed.
 * 
 * @param token[in/out]   token to set the type if matched
 * @param string_view[in] string view to match with
 * 
 * @return mirac_token_type_e
 */
mirac_token_type_e mirac_token_match_string_view_to_reserved_type(
	mirac_token_s* const token,
	const string_view_s string_view);

// TODO: document!
mirac_token_type_e mirac_token_parse_identifier_from_string_view(
	mirac_token_s* const token,
	const string_view_s string_view);

#if 0
/**
 * @brief Stringify token.
 */
const char* mirac_token_to_string(
	const mirac_token_s* const token);

/**
 * @brief Print the token to the stdout.
 */
void mirac_token_print(
	const mirac_token_s* const token);
#endif

#endif
