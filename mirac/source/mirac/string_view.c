
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
	const char* const data,
	const uint64_t length)
{
	mirac_debug_assert(data != NULL);
	return (mirac_string_view_s)
	{
		.data = data,
		.length = length
	};
}

mirac_string_view_s mirac_string_view_from_cstring(
	const char* const cstring)
{
	mirac_debug_assert(cstring != NULL);
	const uint64_t length = (uint64_t)strlen(cstring);
	return mirac_string_view_from_parts(cstring, length);
}

bool mirac_string_view_equal_range(
	const mirac_string_view_s left,
	const mirac_string_view_s right,
	const uint64_t length)
{
	mirac_debug_assert(left.data != NULL);
	mirac_debug_assert(right.data != NULL);

	if (left.length < length || right.length < length)
	{
		return false;
	}

	return (mirac_c_memcmp((const uint8_t* const)left.data, (const uint8_t* const)right.data, length) == 0);
}

bool mirac_string_view_equal(
	const mirac_string_view_s left,
	const mirac_string_view_s right)
{
	mirac_debug_assert(left.data != NULL);
	mirac_debug_assert(right.data != NULL);

	if (left.length != right.length)
	{
		return false;
	}

	return mirac_string_view_equal_range(left, right, left.length);
}

mirac_string_view_s mirac_string_view_trim_left(
	const mirac_string_view_s string_view,
	const char char_to_trim,
	uint64_t* const trimmed_length)
{
	mirac_debug_assert(string_view.data != NULL);

	if (string_view.length <= 0)
	{
		return string_view;
	}

	uint64_t index = 0;

	while ((index < string_view.length) && (char_to_trim == string_view.data[index]))
	{
		++index;
	}

	if (trimmed_length != NULL)
	{
		*trimmed_length = index;
	}

	return mirac_string_view_from_parts(string_view.data + index, string_view.length - index);
}

mirac_string_view_s mirac_string_view_trim_right(
	const mirac_string_view_s string_view,
	const char char_to_trim,
	uint64_t* const trimmed_length)
{
	mirac_debug_assert(string_view.data != NULL);

	if (string_view.length <= 0)
	{
		return string_view;
	}

	uint64_t index = 0;

	while ((index < string_view.length) && (char_to_trim == string_view.data[string_view.length - index - 1]))
	{
		++index;
	}

	if (trimmed_length != NULL)
	{
		*trimmed_length = index;
	}

	return mirac_string_view_from_parts(string_view.data, string_view.length - index);
}

mirac_string_view_s mirac_string_view_trim(
	const mirac_string_view_s string_view,
	const char char_to_trim)
{
	mirac_debug_assert(string_view.data != NULL);
	return mirac_string_view_trim_left(
		mirac_string_view_trim_right(string_view, char_to_trim, NULL), char_to_trim, NULL
	);
}

mirac_string_view_s mirac_string_view_trim_left_white_space(
	const mirac_string_view_s string_view,
	uint64_t* const trimmed_length)
{
	mirac_debug_assert(string_view.data != NULL);

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

	if (trimmed_length != NULL)
	{
		*trimmed_length = index;
	}

	return mirac_string_view_from_parts(string_view.data + index, string_view.length - index);
}

mirac_string_view_s mirac_string_view_trim_right_white_space(
	const mirac_string_view_s string_view,
	uint64_t* const trimmed_length)
{
	mirac_debug_assert(string_view.data != NULL);

	if (string_view.length <= 0)
	{
		return string_view;
	}

	uint64_t index = 0;

	while ((index < string_view.length) && (
			(' '  == string_view.data[string_view.length - index - 1]) ||
			('\t' == string_view.data[string_view.length - index - 1]) ||
			('\n' == string_view.data[string_view.length - index - 1]) ||
			('\r' == string_view.data[string_view.length - index - 1])
		))
	{
		++index;
	}

	if (trimmed_length != NULL)
	{
		*trimmed_length = index;
	}

	return mirac_string_view_from_parts(string_view.data, string_view.length - index);
}

mirac_string_view_s mirac_string_view_trim_white_space(
	const mirac_string_view_s string_view)
{
	mirac_debug_assert(string_view.data != NULL);
	return mirac_string_view_trim_left_white_space(
		mirac_string_view_trim_right_white_space(string_view, NULL), NULL
	);
}

mirac_string_view_s mirac_string_view_split_left(
	mirac_string_view_s* const string_view,
	const char char_to_split_at,
	uint64_t* const split_index)
{
	mirac_debug_assert(string_view != NULL);
	mirac_debug_assert(string_view->data != NULL);

	if (string_view->length <= 0)
	{
		return *string_view;
	}

	uint64_t index = 0;

	while ((index < string_view->length) && (string_view->data[index] != char_to_split_at))
	{
		++index;
	}

	if (split_index != NULL)
	{
		*split_index = index;
	}

	const mirac_string_view_s left = mirac_string_view_from_parts(string_view->data, index);
	*string_view = mirac_string_view_from_parts(string_view->data + index + 1, string_view->length - index - 1);
	return left;
}

mirac_string_view_s mirac_string_view_split_right(
	mirac_string_view_s* const string_view,
	const char char_to_split_at,
	uint64_t* const split_index)
{
	mirac_debug_assert(string_view != NULL);
	mirac_debug_assert(string_view->data != NULL);

	if (string_view->length <= 0)
	{
		return *string_view;
	}

	uint64_t index = 0;

	while ((index < string_view->length) && (string_view->data[string_view->length - index - 1] != char_to_split_at))
	{
		++index;
	}

	if (split_index != NULL)
	{
		*split_index = index;
	}

	const mirac_string_view_s right = mirac_string_view_from_parts(string_view->data + index + 1, string_view->length - index - 1);
	*string_view = mirac_string_view_from_parts(string_view->data, index);
	return right;
}

mirac_string_view_s mirac_string_view_split_left_white_space(
	mirac_string_view_s* const string_view,
	uint64_t* const white_space_length)
{
	mirac_debug_assert(string_view != NULL);
	mirac_debug_assert(string_view->data != NULL);

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

mirac_string_view_s mirac_string_view_split_right_white_space(
	mirac_string_view_s* const string_view,
	uint64_t* const white_space_length)
{
	mirac_debug_assert(string_view != NULL);
	mirac_debug_assert(string_view->data != NULL);

	if (string_view->length <= 0)
	{
		return *string_view;
	}

	uint64_t index = 0;

	while ((index < string_view->length) && (
			string_view->data[string_view->length - index - 1] != ' '  &&
			string_view->data[string_view->length - index - 1] != '\t' &&
			string_view->data[string_view->length - index - 1] != '\n' &&
			string_view->data[string_view->length - index - 1] != '\r'
		))
	{
		++index;
	}

	const mirac_string_view_s right = mirac_string_view_from_parts(string_view->data + index, string_view->length - index);
	*string_view = mirac_string_view_from_parts(string_view->data, index);
	*string_view = mirac_string_view_trim_right_white_space(*string_view, white_space_length);
	return right;
}
