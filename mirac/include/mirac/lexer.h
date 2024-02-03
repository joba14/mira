
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

#include <mirac/c_common.h>
#include <mirac/string_view.h>
#include <mirac/config.h>
#include <mirac/arena.h>

typedef struct
{
	mirac_string_view_s file;
	uint64_t line;
	uint64_t column;
} mirac_location_s;

/**
 * @brief Location formatting macro for printf-like functions.
 */
#define mirac_location_fmt mirac_sv_fmt ":%lu:%lu"

/**
 * @brief Location formatting argument macro for printf-like functions.
 */
#define mirac_location_arg(_location) \
	mirac_sv_arg((_location).file), (_location).line, (_location).column

typedef enum
{
	mirac_token_type_reserved_lnot = 0,
	mirac_token_type_reserved_land,
	mirac_token_type_reserved_lor,
	mirac_token_type_reserved_lxor,

	mirac_token_type_reserved_bnot,
	mirac_token_type_reserved_band,
	mirac_token_type_reserved_bor,
	mirac_token_type_reserved_bxor,
	mirac_token_type_reserved_shl,
	mirac_token_type_reserved_shr,

	mirac_token_type_reserved_add,
	mirac_token_type_reserved_inc,
	mirac_token_type_reserved_sub,
	mirac_token_type_reserved_dec,
	mirac_token_type_reserved_mul,
	mirac_token_type_reserved_div,
	mirac_token_type_reserved_mod,
	mirac_token_type_reserved_divmod,

	mirac_token_type_reserved_eq,
	mirac_token_type_reserved_neq,
	mirac_token_type_reserved_gt,
	mirac_token_type_reserved_gteq,
	mirac_token_type_reserved_ls,
	mirac_token_type_reserved_lseq,

	mirac_token_type_reserved_drop,
	mirac_token_type_reserved_dup,
	mirac_token_type_reserved_over,
	mirac_token_type_reserved_rot,
	mirac_token_type_reserved_swap,

	mirac_token_type_reserved_ld08,
	mirac_token_type_reserved_ld16,
	mirac_token_type_reserved_ld32,
	mirac_token_type_reserved_ld64,
	mirac_token_type_reserved_st08,
	mirac_token_type_reserved_st16,
	mirac_token_type_reserved_st32,
	mirac_token_type_reserved_st64,

	mirac_token_type_reserved_sys1,
	mirac_token_type_reserved_sys2,
	mirac_token_type_reserved_sys3,
	mirac_token_type_reserved_sys4,
	mirac_token_type_reserved_sys5,
	mirac_token_type_reserved_sys6,

	mirac_token_type_reserved_i08,
	mirac_token_type_reserved_i16,
	mirac_token_type_reserved_i32,
	mirac_token_type_reserved_i64,
	mirac_token_type_reserved_u08,
	mirac_token_type_reserved_u16,
	mirac_token_type_reserved_u32,
	mirac_token_type_reserved_u64,
	mirac_token_type_reserved_ptr,

	mirac_token_type_reserved_sec,
	mirac_token_type_reserved_str,
	mirac_token_type_reserved_mem,
	mirac_token_type_reserved_func,
	mirac_token_type_reserved_if,
	mirac_token_type_reserved_else,
	mirac_token_type_reserved_loop,
	mirac_token_type_reserved_req,
	mirac_token_type_reserved_ret,
	mirac_token_type_reserved_call,
	mirac_token_type_reserved_as,

	mirac_token_type_reserved_left_parenthesis,
	mirac_token_type_reserved_right_parenthesis,
	mirac_token_type_reserved_left_bracket,
	mirac_token_type_reserved_right_bracket,
	mirac_token_type_reserved_left_brace,
	mirac_token_type_reserved_right_brace,

	mirac_token_type_reserved_count = mirac_token_type_reserved_right_brace,

	// Tokens with additional information
	mirac_token_type_literal_i08,
	mirac_token_type_literal_i16,
	mirac_token_type_literal_i32,
	mirac_token_type_literal_i64,
	mirac_token_type_literal_u08,
	mirac_token_type_literal_u16,
	mirac_token_type_literal_u32,
	mirac_token_type_literal_u64,
	mirac_token_type_literal_ptr,
	mirac_token_type_literal_str,

	mirac_token_type_identifier,

	// Magic tokens
	mirac_token_type_eof,
	mirac_token_type_none
} mirac_token_type_e;

// todo: write unit tests!
/**
 * @brief Stringify token type and return the string view.
 * 
 * @param token_type token type to stringify
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_token_type_to_string_view(
	const mirac_token_type_e token_type);

typedef struct mirac_token_s mirac_token_s;

struct mirac_token_s
{
	mirac_token_type_e type;
	mirac_location_s location;
	uint64_t index;

	union
	{
		int64_t ival;
		uint64_t uval;
		long double fval;
		uintptr_t ptr;
		mirac_string_view_s str;
		mirac_string_view_s ident;
	} as;

	mirac_string_view_s text;
};

// todo: write unit tests!
/**
 * @brief Create token with provided token type and location.
 * 
 * @note All the rest of the fields will be initialized to 0.
 * 
 * @param type     token type to assign to the new token
 * @param location location to set to the token
 * @param index    global index of the token
 * @param text     text representation from the file
 * 
 * @return mirac_token_s
 */
mirac_token_s mirac_token_from_parts(
	const mirac_token_type_e type,
	const mirac_location_s location,
	const uint64_t index,
	const mirac_string_view_s text);

// todo: write unit tests!
/**
 * @brief Create a token object with provided token type.
 * 
 * @note All the rest of the fields will be initialized to 0.
 * 
 * @param type token type to assign to the new token
 * 
 * @return mirac_token_s
 */
mirac_token_s mirac_token_from_type(
	const mirac_token_type_e type);

// todo: write unit tests!
/**
 * @brief Destroy (reset all fields to 0) token.
 * 
 * @note The memory that was allocated for the token's fields will not be freed
 * with this function. It can be freed only with the arena that was used as the
 * allocator in the lexer.
 * 
 * @param token token to destroy
 */
void mirac_token_destroy(
	mirac_token_s* const token);

// todo: write unit tests!
/**
 * @brief Stringify token and return the string view.
 * 
 * @param token token type to stringify
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_token_to_string_view(
	const mirac_token_s* const token);

// todo: write unit tests!
/**
 * @brief Check if a token is unsigned numeric literal.
 * 
 * @param token token to check
 * 
 * @return bool_t
 */
bool_t mirac_token_is_unsigned_numeric_literal(
	const mirac_token_s* const token);

// todo: write unit tests!
/**
 * @brief Check if a token is a type token.
 * 
 * @param token token to check
 * 
 * @return bool_t
 */
bool_t mirac_token_is_type_token(
	const mirac_token_s* const token);

typedef struct
{
	mirac_config_s* config;
	mirac_arena_s* arena;
	mirac_string_view_s file_path;
	mirac_location_s locations[2];
	uint64_t tokens_count;
	mirac_token_s token;
	mirac_string_view_s buffer;
	mirac_string_view_s line;
} mirac_lexer_s;

// todo: write unit tests!
/**
 * @brief Create a lexer with provided source string.
 * 
 * @param config    config instance
 * @param arena     arena instance for memory management
 * @param file_path file path
 * @param file      file for lexing
 * 
 * @return mirac_lexer_s
 */
mirac_lexer_s mirac_lexer_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	const mirac_string_view_s file_path,
	mirac_file_t* const file);

// todo: write unit tests!
/**
 * @brief Destroy the lexer.
 * 
 * This function deallocates the internal lexer's buffer and resets all its fields
 * to zero.
 * 
 * @warning This function does not close the file, used by lexer! It is left for the
 * user of the lexer to close the file after finishing with the lexer.
 * 
 * @param lexer lexer instance
 */
void mirac_lexer_destroy(
	mirac_lexer_s* const lexer);

// todo: write unit tests!
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
 * see @ref mirac_lexer_should_stop_lexing().
 * 
 * @param lexer lexer instance
 * @param token token to be lexed
 * 
 * @return mirac_token_type_e
 */
mirac_token_type_e mirac_lexer_lex_next(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

// todo: write unit tests!
/**
 * @brief Check if lexer has reached end of file or failed to lex a token.
 * 
 * @param type newly lexed token type
 * 
 * @return bool_t
 */
bool_t mirac_lexer_should_stop_lexing(
	const mirac_token_type_e type);

// todo: write unit tests!
/**
 * @brief Cache a token in the lexer.
 * 
 * @param lexer lexer instance
 * @param token token to cache
 */
void mirac_lexer_unlex(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

#endif
