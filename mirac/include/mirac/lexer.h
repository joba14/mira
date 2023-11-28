
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
#include <mirac/token.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct
{
	FILE* file;
	mirac_location_s location;
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
 * @brief Cache a token back to the lexer.
 */
void mirac_lexer_unlex(
	mirac_lexer_s* const lexer,
	const mirac_token_s* const token);

#endif
