
/**
 * @file token.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__token_h__
#define __mirac__include__mirac__token_h__

#include <mirac/location.h>
#include <mirac/utf8.h>

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	mirac_token_type_keyword_add,				// add
	mirac_token_type_keyword_band,				// band
	mirac_token_type_keyword_bnot,				// bnot
	mirac_token_type_keyword_bor,				// bor
	mirac_token_type_keyword_brk,				// brk
	mirac_token_type_keyword_bxor,				// bxor
	mirac_token_type_keyword_cast,				// cast
	mirac_token_type_keyword_div,				// div
	mirac_token_type_keyword_do,				// do
	mirac_token_type_keyword_drop,				// drop
	mirac_token_type_keyword_dup,				// dup
	mirac_token_type_keyword_elif,				// elif
	mirac_token_type_keyword_else,				// else
	mirac_token_type_keyword_end,				// end
	mirac_token_type_keyword_eq,				// eq
	mirac_token_type_keyword_f32,				// f32
	mirac_token_type_keyword_f64,				// f64
	mirac_token_type_keyword_func,				// func
	mirac_token_type_keyword_gt,				// gt
	mirac_token_type_keyword_gteq,				// gteq
	mirac_token_type_keyword_i16,				// i16
	mirac_token_type_keyword_i32,				// i32
	mirac_token_type_keyword_i64,				// i64
	mirac_token_type_keyword_i8,				// i8
	mirac_token_type_keyword_if,				// if
	mirac_token_type_keyword_inl,				// inl
	mirac_token_type_keyword_land,				// land
	mirac_token_type_keyword_ld16,				// ld16
	mirac_token_type_keyword_ld32,				// ld32
	mirac_token_type_keyword_ld64,				// ld64
	mirac_token_type_keyword_ld8,				// ld8
	mirac_token_type_keyword_let,				// let
	mirac_token_type_keyword_lnot,				// lnot
	mirac_token_type_keyword_loop,				// loop
	mirac_token_type_keyword_lor,				// lor
	mirac_token_type_keyword_ls,				// ls
	mirac_token_type_keyword_lseq,				// lseq
	mirac_token_type_keyword_lxor,				// lxor
	mirac_token_type_keyword_mem,				// mem
	mirac_token_type_keyword_mod,				// mod
	mirac_token_type_keyword_mul,				// mul
	mirac_token_type_keyword_mut,				// mut
	mirac_token_type_keyword_neq,				// neq
	mirac_token_type_keyword_over,				// over
	mirac_token_type_keyword_req,				// req
	mirac_token_type_keyword_ret,				// ret
	mirac_token_type_keyword_rot,				// rot
	mirac_token_type_keyword_shl,				// shl
	mirac_token_type_keyword_shr,				// shr
	mirac_token_type_keyword_sys,				// sys
	mirac_token_type_keyword_st16,				// st16
	mirac_token_type_keyword_st32,				// st32
	mirac_token_type_keyword_st64,				// st64
	mirac_token_type_keyword_st8,				// st8
	mirac_token_type_keyword_sub,				// sub
	mirac_token_type_keyword_swap,				// swap
	mirac_token_type_keyword_u16,				// u16
	mirac_token_type_keyword_u32,				// u32
	mirac_token_type_keyword_u64,				// u64
	mirac_token_type_keyword_u8,				// u8
	mirac_token_type_keywords_count = mirac_token_type_keyword_u8,

	// Tokens with additional information
	mirac_token_type_literal_i8,
	mirac_token_type_literal_i16,
	mirac_token_type_literal_i32,
	mirac_token_type_literal_i64,
	mirac_token_type_literal_u8,
	mirac_token_type_literal_u16,
	mirac_token_type_literal_u32,
	mirac_token_type_literal_u64,
	mirac_token_type_literal_f32,
	mirac_token_type_literal_f64,
	mirac_token_type_literal_str,
	mirac_token_type_literal_cstr,

	mirac_token_type_identifier,
	mirac_token_type_invalid,

	// Magic tokens
	mirac_token_type_eof,
	mirac_token_type_none
} mirac_token_type_e;

/**
 * @brief Try to parse token type from a string.
 */
mirac_token_type_e mirac_token_type_from_string(
	const char* const string);

/**
 * @brief Stringify token type.
 */
const char* mirac_token_type_to_string(
	const mirac_token_type_e type);

typedef struct
{
	mirac_token_type_e type;
	mirac_location_s location;

	union
	{
		int64_t ival;
		uint64_t uval;
		long double fval;

		struct
		{
			char* data;
			uint64_t length;
		} str;

		struct
		{
			char* data;
			uint64_t length;
		} ident;
	};
} mirac_token_s;

/**
 * @brief Create token with provided token type and location.
 * 
 * @note All the other fields will be set to 0.
 */
mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e type,
	const mirac_location_s location);

/**
 * @brief Create token with provided token type.
 * 
 * @note All the other fields will be set to 0.
 */
mirac_token_s mirac_token_from_type(
	const mirac_token_type_e type);

/**
 * @brief Destroy token and free all its resources.
 */
void mirac_token_destroy(
	mirac_token_s* const token);

/**
 * @brief Convert token structure into a string representation and return a
 * pointer to it.
 * 
 * @warning The formatted token string is safed in the static buffer that gets
 * overwritten every time this function is called!
 */
const char* mirac_token_to_string(
	const mirac_token_s* const token);

#endif
