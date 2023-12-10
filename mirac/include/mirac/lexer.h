
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

#include <stdio.h>

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
	mirac_token_type_reserved_drop,
	mirac_token_type_reserved_dup,
	mirac_token_type_reserved_over,
	mirac_token_type_reserved_rot,
	mirac_token_type_reserved_swap,
	mirac_token_type_reserved_bxor,
	mirac_token_type_reserved_lxor,
	mirac_token_type_reserved_as,
	mirac_token_type_reserved_do,
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
	mirac_token_type_reserved_ptr,
	mirac_token_type_reserved_reg,
	mirac_token_type_reserved_req,
	mirac_token_type_reserved_ret,
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
		int64_t as_ival;
		uint64_t as_uval;
		long double as_fval;
		mirac_string_view_s as_str;
		mirac_string_view_s as_ident;
	};

	mirac_string_view_s text;
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
	const mirac_string_view_s text);

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

/**
 * @brief Stringify token and return the string view.
 * 
 * @param token[in] token type to stringify
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_token_to_string_view(
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

/**
 * @brief Create a lexer with provided file and its path.
 * 
 * @param config[in]    config instance
 * @param arena[in]     arena instance for memory management
 * @param file_path[in] path of file to lexed
 * 
 * @return mirac_lexer_s
 */
mirac_lexer_s mirac_lexer_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	const mirac_string_view_s file_path);

/**
 * @brief Destroy the lexer.
 * 
 * This function deallocates the internal lexer's buffer and resets all its fields
 * to zero.
 * 
 * @warning This function does not close the file, used by lexer! It is left for the
 * user of the lexer to close the file after finishing with the lexer.
 * 
 * @param lexer[in/out] lexer instance
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
 * see @ref mirac_lexer_should_stop_lexing().
 * 
 * @param lexer[in/out] lexer instance
 * @param token[out]    token to be lexed
 * 
 * @return mirac_token_type_e
 */
mirac_token_type_e mirac_lexer_lex(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

/**
 * @brief Check if lexer has reached end of file or failed to lex a token.
 * 
 * @param type[in] newly lexed token type
 * 
 * @return bool
 */
bool mirac_lexer_should_stop_lexing(
	const mirac_token_type_e type);

/**
 * @brief Cache a token in the lexer.
 * 
 * @param lexer[in/out] lexer instance
 * @param token[in]     token to cache
 */
void mirac_lexer_unlex(
	mirac_lexer_s* const lexer,
	mirac_token_s* const token);

#endif
