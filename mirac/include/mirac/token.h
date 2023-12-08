
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

typedef struct
{
	const char* file;
	uint64_t line;
	uint64_t column;
} mirac_location_s;

/**
 * @brief Location formatting macro for printf-like functions.
 */
#define mirac_location_fmt "%s:%lu:%lu"

/**
 * @brief Location formatting argument macro for printf-like functions.
 */
#define mirac_location_arg(_location) \
	(_location).file, (_location).line, (_location).column

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
 * @brief Try to parse token type from a string.
 */
mirac_token_type_e mirac_reserved_token_type_from_string_view(
	const string_view_s string_view);

/**
 * @brief Stringify token type.
 */
const char* mirac_token_type_to_string(
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

		struct
		{
			char* data;
			uint64_t length;
		} as_str;

		struct
		{
			char* data;
			uint64_t length;
		} as_ident;
	};

	struct
	{
		char* data;
		uint64_t length;
	} source;

	mirac_token_s* next_ref;
	mirac_token_s* prev_ref;
};

/**
 * @brief Create token with provided token type and location.
 * 
 * @note All the other fields will be set to 0.
 */
mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e token_type,
	const mirac_location_s location);

/**
 * @brief Create token with provided token type.
 * 
 * @note All the other fields will be set to 0.
 */
mirac_token_s mirac_token_from_type(
	const mirac_token_type_e token_type);

/**
 * @brief Destroy (reset) token.
 */
void mirac_token_destroy(
	mirac_token_s* const token);

/**
 * @brief Check if token is a type keyword.
 */
bool mirac_token_is_type_keyword(
	mirac_token_s* const token);

/**
 * @brief Check if token is a signd integer literal.
 */
bool mirac_token_is_signed_integer_literal(
	mirac_token_s* const token);

/**
 * @brief Check if token is an unsignd integer literal.
 */
bool mirac_token_is_unsigned_integer_literal(
	mirac_token_s* const token);

/**
 * @brief Check if token is a string literal.
 */
bool mirac_token_is_string_literal(
	mirac_token_s* const token);

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
