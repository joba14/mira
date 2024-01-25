
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
	const char_t* data;
	uint64_t length;
} mirac_string_view_s;

/**
 * @brief String view format for printf-like functions.
 */
#define mirac_sv_fmt "%.*s"

/**
 * @brief String view argument for printf-like functions.
 */
#define mirac_sv_arg(_string_view) ((signed int) (_string_view).length), (_string_view).data

/**
 * @brief String view format for printf-like functions.
 */
#define mirac_string_view_static(_string) { (_string), (sizeof(_string) - 1) }

/**
 * @brief Create string view from a const-pointer to a string or sequence of characters and its length.
 * 
 * @param data   pointer to a string or sequence of characters
 * @param length length of the string or sequence of characters
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_string_view_from_parts(
	const char_t* const data,
	const uint64_t length);

/**
 * @brief Create string view from a cstring.
 * 
 * @param cstring pointer to a cstring
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_string_view_from_cstring(
	const char_t* const cstring);

/**
 * @brief Check if two string views are equal for provided length (range).
 * 
 * @param left   left comparison string view
 * @param right  right comparison string view
 * @param length length of a range to check for equality
 * 
 * @return bool_t
 */
bool_t mirac_string_view_equal_range(
	const mirac_string_view_s left,
	const mirac_string_view_s right,
	const uint64_t length);

/**
 * @brief Check if two string views are equal.
 * 
 * @param left  left comparison string view
 * @param right right comparison string view
 * 
 * @return bool_t
 */
bool_t mirac_string_view_equal(
	const mirac_string_view_s left,
	const mirac_string_view_s right);

/**
 * @brief Trim left side of the string view of provided char.
 * 
 * @param string_view    string view to trim
 * @param char_to_trim   char to trim
 * @param trimmed_length length of the trimmed string (optional)
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_string_view_trim_left(
	const mirac_string_view_s string_view,
	const char_t char_to_trim,
	uint64_t* const trimmed_length);

/**
 * @brief Trim right side of the string view of provided char.
 * 
 * @param string_view    string view to trim
 * @param char_to_trim   char to trim
 * @param trimmed_length length of the trimmed string (optional)
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_string_view_trim_right(
	const mirac_string_view_s string_view,
	const char_t char_to_trim,
	uint64_t* const trimmed_length);

/**
 * @brief Trim left and right sides of the string view of provided char.
 * 
 * @param string_view  string view to trim
 * @param char_to_trim char to trim
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_string_view_trim(
	const mirac_string_view_s string_view,
	const char_t char_to_trim);

/**
 * @brief Trim white spaces on the left side of the string view of provided char.
 * 
 * @param string_view    string view to trim
 * @param trimmed_length length of the trimmed string (optional)
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_string_view_trim_left_white_space(
	const mirac_string_view_s string_view,
	uint64_t* const trimmed_length);

/**
 * @brief Split string view at a provided char from the left side.
 * 
 * @note The returned string view is the left split part of the original string
 * view, while the right split part will be set to the original string that is
 * passed by reference.
 * 
 * @param string_view      string view to split
 * @param char_to_split_at char to split at
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_string_view_split_left(
	mirac_string_view_s* const string_view,
	const char_t char_to_split_at);

/**
 * @brief Split string view at the first white space sequence on the left side.
 * 
 * @note The entire white space will be ignored and not included in neither of
 * the string views. To get the length of the ignored white space use optional
 * parameter 'white_space_length'.
 * 
 * @param string_view        string view to split
 * @param white_space_length white space length (optional)
 * 
 * @return mirac_string_view_s
 */
mirac_string_view_s mirac_string_view_split_left_white_space(
	mirac_string_view_s* const string_view,
	uint64_t* const white_space_length);

#endif
