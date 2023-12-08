
/**
 * @file string_view.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-06
 */

#ifndef __mirac__include__mirac__string_view_h__
#define __mirac__include__mirac__string_view_h__

#include <mirac/c_common.h>

/**
 * @brief String view structure.
 * 
 * It holds a const-pointer to a string or sequence of characters and a length.
 */
typedef struct
{
	const char* data;
	uint64_t length;
} string_view_s;

/**
 * @brief String view format for printf-like functions.
 */
#define sv_fmt "%.*s"

/**
 * @brief String view argument for printf-like functions.
 */
#define sv_arg(_string_view) ((signed int) (_string_view).length), (_string_view).data

/**
 * @brief String view format for printf-like functions.
 */
#define string_view_static(_string) (string_view_s) { .data = _string, .length = (sizeof(_string) / sizeof(char)) }

/**
 * @brief Create string view from a const-pointer to a string or sequence of characters and its length.
 * 
 * @param data[in]   pointer to a string or sequence of characters
 * @param length[in] length of the string or sequence of characters
 * 
 * @return string_view_s
 */
string_view_s string_view_from_parts(
	const char* const data,
	const uint64_t length);

/**
 * @brief Create string view from a cstring.
 * 
 * @param cstring[in] pointer to a cstring
 * 
 * @return string_view_s
 */
string_view_s string_view_from_cstring(
	const char* const cstring);

/**
 * @brief Check if two string views are equal for provided length (range).
 * 
 * @param left[in]   left comparison string view
 * @param right[in]  right comparison string view
 * @param length[in] length of a range to check for equality
 * 
 * @return bool
 */
bool string_view_equal_range(
	const string_view_s left,
	const string_view_s right,
	const uint64_t length);

/**
 * @brief Check if two string views are equal.
 * 
 * @param left[in]  left comparison string view
 * @param right[in] right comparison string view
 * 
 * @return bool
 */
bool string_view_equal(
	const string_view_s left,
	const string_view_s right);

/**
 * @brief Trim left side of the string view of provided char.
 * 
 * @param string_view[in]  string view to trim
 * @param char_to_trim[in] char to trim
 * 
 * @return string_view_s
 */
string_view_s string_view_trim_left(
	const string_view_s string_view,
	const char char_to_trim);

/**
 * @brief Trim right side of the string view of provided char.
 * 
 * @param string_view[in]  string view to trim
 * @param char_to_trim[in] char to trim
 * 
 * @return string_view_s
 */
string_view_s string_view_trim_right(
	const string_view_s string_view,
	const char char_to_trim);

/**
 * @brief Trim left and right sides of the string view of provided char.
 * 
 * @param string_view[in]  string view to trim
 * @param char_to_trim[in] char to trim
 * 
 * @return string_view_s
 */
string_view_s string_view_trim(
	const string_view_s string_view,
	const char char_to_trim);

/**
 * @brief Trim white spaces on the left side of the string view of provided char.
 * 
 * @param string_view[in] string view to trim
 * 
 * @return string_view_s
 */
string_view_s string_view_trim_left_white_space(
	const string_view_s string_view);

/**
 * @brief Trim white spaces on the right side of the string view of provided char.
 * 
 * @param string_view[in] string view to trim
 * 
 * @return string_view_s
 */
string_view_s string_view_trim_right_white_space(
	const string_view_s string_view);

/**
 * @brief Trim white spaces on the left and right sides of the string view of
 * provided char.
 * 
 * @param string_view[in] string view to trim
 * 
 * @return string_view_s
 */
string_view_s string_view_trim_white_space(
	const string_view_s string_view);

#endif
