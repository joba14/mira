
/**
 * @file string_view.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-06
 */

#include <mirac/string_view.h>

#include <mirac/debug.h>

#include <memory.h>
#include <string.h>

mirac_string_view_s mirac_string_view_from_parts(
	const char_t* const data,
	const uint64_t length)
{
	mirac_debug_assert(data != mirac_null);
	return (mirac_string_view_s)
	{
		.data = data,
		.length = length
	};
}

mirac_string_view_s mirac_string_view_from_cstring(
	const char_t* const cstring)
{
	mirac_debug_assert(cstring != mirac_null);
	const uint64_t length = (uint64_t)strlen(cstring);
	return mirac_string_view_from_parts(cstring, length);
}

bool_t mirac_string_view_equal_range(
	const mirac_string_view_s left,
	const mirac_string_view_s right,
	const uint64_t length)
{
	mirac_debug_assert(left.data != mirac_null);
	mirac_debug_assert(right.data != mirac_null);

	if (left.length < length || right.length < length)
	{
		return false;
	}

	return (mirac_c_memcmp((const uint8_t* const)left.data, (const uint8_t* const)right.data, length) == 0);
}

bool_t mirac_string_view_equal(
	const mirac_string_view_s left,
	const mirac_string_view_s right)
{
	mirac_debug_assert(left.data != mirac_null);
	mirac_debug_assert(right.data != mirac_null);

	if (left.length != right.length)
	{
		return false;
	}

	return mirac_string_view_equal_range(left, right, left.length);
}

mirac_string_view_s mirac_string_view_trim_left(
	const mirac_string_view_s string_view,
	const char_t char_to_trim,
	uint64_t* const trimmed_length)
{
	mirac_debug_assert(string_view.data != mirac_null);

	if (string_view.length <= 0)
	{
		return string_view;
	}

	uint64_t index = 0;

	while ((index < string_view.length) && (char_to_trim == string_view.data[index]))
	{
		++index;
	}

	if (trimmed_length != mirac_null)
	{
		*trimmed_length = index;
	}

	return mirac_string_view_from_parts(string_view.data + index, string_view.length - index);
}

mirac_string_view_s mirac_string_view_trim_right(
	const mirac_string_view_s string_view,
	const char_t char_to_trim,
	uint64_t* const trimmed_length)
{
	mirac_debug_assert(string_view.data != mirac_null);

	if (string_view.length <= 0)
	{
		return string_view;
	}

	uint64_t index = 0;

	while ((index < string_view.length) && (char_to_trim == string_view.data[string_view.length - index - 1]))
	{
		++index;
	}

	if (trimmed_length != mirac_null)
	{
		*trimmed_length = index;
	}

	return mirac_string_view_from_parts(string_view.data, string_view.length - index);
}

mirac_string_view_s mirac_string_view_trim(
	const mirac_string_view_s string_view,
	const char_t char_to_trim)
{
	mirac_debug_assert(string_view.data != mirac_null);
	return mirac_string_view_trim_left(
		mirac_string_view_trim_right(string_view, char_to_trim, mirac_null), char_to_trim, mirac_null
	);
}

mirac_string_view_s mirac_string_view_trim_left_white_space(
	const mirac_string_view_s string_view,
	uint64_t* const trimmed_length)
{
	mirac_debug_assert(string_view.data != mirac_null);

	if (string_view.length <= 0)
	{
		return string_view;
	}

	uint64_t index = 0;

	while ((index < string_view.length) && (
			(' '  == string_view.data[index]) ||
			('\t' == string_view.data[index]) ||
			('\n' == string_view.data[index]) ||
			('\r' == string_view.data[index])
		))
	{
		++index;
	}

	if (trimmed_length != mirac_null)
	{
		*trimmed_length = index;
	}

	return mirac_string_view_from_parts(string_view.data + index, string_view.length - index);
}

mirac_string_view_s mirac_string_view_split_left(
	mirac_string_view_s* const string_view,
	const char_t char_to_split_at)
{
	mirac_debug_assert(string_view != mirac_null);
	mirac_debug_assert(string_view->data != mirac_null);

	if (string_view->length <= 0)
	{
		return *string_view;
	}

	int64_t index = 0;

	while ((index < (int64_t)string_view->length) && (string_view->data[index] != char_to_split_at))
	{
		++index;
	}

	int64_t string_view_length = (int64_t)string_view->length - index - 1;

	if (string_view_length < 0 || index >= (int64_t)string_view->length)
	{
		return mirac_string_view_from_parts("", 0);
	}

	const mirac_string_view_s left = mirac_string_view_from_parts(string_view->data, (uint64_t)index);
	*string_view = mirac_string_view_from_parts(string_view->data + index + 1, (uint64_t)string_view_length);
	return left;
}

mirac_string_view_s mirac_string_view_split_left_white_space(
	mirac_string_view_s* const string_view,
	uint64_t* const white_space_length)
{
	mirac_debug_assert(string_view != mirac_null);
	mirac_debug_assert(string_view->data != mirac_null);

	if (string_view->length <= 0)
	{
		return *string_view;
	}

	uint64_t index = 0;

	while ((index < string_view->length) && (
			string_view->data[index] != ' '  &&
			string_view->data[index] != '\t' &&
			string_view->data[index] != '\n' &&
			string_view->data[index] != '\r'
		))
	{
		++index;
	}

	const mirac_string_view_s left = mirac_string_view_from_parts(string_view->data, index);
	*string_view = mirac_string_view_from_parts(string_view->data + index, string_view->length - index);
	*string_view = mirac_string_view_trim_left_white_space(*string_view, white_space_length);
	return left;
}
