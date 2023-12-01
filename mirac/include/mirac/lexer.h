
/**
 * @file lexer.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__lexer_h__
#define __mirac__include__mirac__lexer_h__

#include <mirac/utf8.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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
	mirac_token_type_keyword_add,				// add
	mirac_token_type_keyword_as,				// as
	mirac_token_type_keyword_band,				// band
	mirac_token_type_keyword_bnot,				// bnot
	mirac_token_type_keyword_bor,				// bor
	mirac_token_type_keyword_bxor,				// bxor
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
	mirac_token_type_keyword_i08,				// i08
	mirac_token_type_keyword_i16,				// i16
	mirac_token_type_keyword_i32,				// i32
	mirac_token_type_keyword_i64,				// i64
	mirac_token_type_keyword_if,				// if
	mirac_token_type_keyword_inl,				// inl
	mirac_token_type_keyword_land,				// land
	mirac_token_type_keyword_ld08,				// ld08
	mirac_token_type_keyword_ld16,				// ld16
	mirac_token_type_keyword_ld32,				// ld32
	mirac_token_type_keyword_ld64,				// ld64
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
	mirac_token_type_keyword_neq,				// neq
	mirac_token_type_keyword_over,				// over
	mirac_token_type_keyword_ptr,				// ptr
	mirac_token_type_keyword_req,				// req
	mirac_token_type_keyword_ret,				// ret
	mirac_token_type_keyword_rot,				// rot
	mirac_token_type_keyword_shl,				// shl
	mirac_token_type_keyword_shr,				// shr
	mirac_token_type_keyword_sys1,				// sys1
	mirac_token_type_keyword_sys2,				// sys2
	mirac_token_type_keyword_sys3,				// sys3
	mirac_token_type_keyword_sys4,				// sys4
	mirac_token_type_keyword_sys5,				// sys5
	mirac_token_type_keyword_sys6,				// sys6
	mirac_token_type_keyword_st08,				// st08
	mirac_token_type_keyword_st16,				// st16
	mirac_token_type_keyword_st32,				// st32
	mirac_token_type_keyword_st64,				// st64
	mirac_token_type_keyword_sub,				// sub
	mirac_token_type_keyword_swap,				// swap
	mirac_token_type_keyword_u08,				// u08
	mirac_token_type_keyword_u16,				// u16
	mirac_token_type_keyword_u32,				// u32
	mirac_token_type_keyword_u64,				// u64
	mirac_token_type_keywords_count = mirac_token_type_keyword_u08,

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
 * @brief Destroy token and free all its resources.
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

// TODO: document:
bool mirac_token_is_string_literal(
	mirac_token_s* const token);

/**
 * @brief Print the token to the stdout.
 */
void mirac_token_print(
	const mirac_token_s* const token);

typedef struct
{
	FILE* file;
	mirac_location_s location;
	uint64_t tokens_count;
	mirac_token_s token;

	struct
	{
		char* data;
		uint64_t capacity;
		uint64_t length;
	} buffer;

	utf8char_t cache[2];
	bool require_int;
} mirac_lexer_s;

/**
 * @brief Create a lexer with provided file and its path.
 */
mirac_lexer_s mirac_lexer_from_parts(
	const char* const file_path,
	FILE* const file);

/**
 * @brief Destroy the lexer.
 * 
 * This function deallocates the internal lexer's buffer and resets all its fields
 * to zero.
 * 
 * @warning This function does not close the file, used by lexer! It is left for the
 * user of the lexer to close the file after finishing with the lexer.
 */
void mirac_lexer_destroy(
	mirac_lexer_s* const lexer);

/**
 * @brief Lex next token from the file source.
 * 
 * @warning Once the lexer reaches the end of file token it will keep returning it!
 * It is left for the user of this function to handle this case.
 * 
 * @note The easiest way to do this is to check if a returned token has  either the
 * "mirac_token_type_none" type or "mirac_token_type_eof" type and stop lexing when
 * that happens. I have added a helper function just for this - to verify the token
 * and determine if one needs to stop lexing or not:
 * see @ref mirac_lexer_should_stop_lexing()
 */
mirac_token_type_e mirac_lexer_lex(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

/**
 * @brief Check if lexer has reached end of file or failed to lex token.
 */
bool mirac_lexer_should_stop_lexing(
	const mirac_token_type_e type);

/**
 * @brief Cache a token in the lexer.
 */
void mirac_lexer_unlex(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

#endif
