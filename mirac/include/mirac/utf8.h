
/**
 * @file utf8.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__utf8_h__
#define __mirac__include__mirac__utf8_h__

#include <stdint.h>
#include <limits.h>
#include <stdio.h>

typedef uint32_t utf8char_t;
#define mirac_utf8_max_size sizeof(utf8char_t)
#define mirac_utf8_invalid UINT32_MAX

/**
 * @brief Decode utf-8 char from provided string.
 * 
 * @note In case function fails to decode utf-8 char from the provided string,
 * it returns "mirac_utf8_invalid".
 */
utf8char_t mirac_utf8_decode(
	const char** const string);

/**
 * @brief Encode utf-8 char into a string.
 * 
 * @note The max size of the encoded buffer can be 4 bytes. The actual size the
 * function required to encode the utf8 char is returned from the function.
 */
uint8_t mirac_utf8_encode(
	char* const string,
	utf8char_t utf8char);

/**
 * @brief Get utf-8 char from provided file stream.
 * 
 * @note In case function fails to decode utf-8 char from the provided string,
 * it returns "mirac_utf8_invalid".
 */
utf8char_t mirac_utf8_get(
	FILE* const file);

#endif
