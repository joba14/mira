
/**
 * @file c_common.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-03
 */

#include <mirac/c_common.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

#include <stdlib.h>
#include <memory.h>
#include <string.h>

void* mirac_c_malloc(
	const uint64_t size)
{
	mirac_debug_assert(size > 0);
	void* const pointer = (void* const)malloc(size);

	if (mirac_null == pointer)
	{
		mirac_logger_error("internal failure -- failed to allocate memory.");
		mirac_c_exit(-1);
	}

	return pointer;
}

void* mirac_c_realloc(
	void* pointer,
	const uint64_t size)
{
	mirac_debug_assert(size > 0);
	pointer = (void*)realloc(pointer, size);

	if (mirac_null == pointer)
	{
		mirac_logger_error("internal failure -- failed to reallocate memory.");
		mirac_c_exit(-1);
	}

	return pointer;
}

void mirac_c_free(
	const void* pointer)
{
	free((void*)pointer);
	pointer = mirac_null;
}

void mirac_c_exit(
	const int32_t code)
{
	exit(code);
}

void mirac_c_memset(
	void* const pointer,
	const uint8_t value,
	const uint64_t length)
{
	mirac_debug_assert(pointer != mirac_null);
	(void)memset((void*)pointer, value, length);
}

void mirac_c_memcpy(
	void* const destination,
	const void* const source,
	const uint64_t length)
{
	mirac_debug_assert(destination != mirac_null);
	mirac_debug_assert(source != mirac_null);
	mirac_debug_assert(length > 0);
	(void)memcpy((void*)destination, (const void*)source, length);
}

int32_t mirac_c_memcmp(
	const uint8_t* const left,
	const uint8_t* const right,
	const uint64_t length)
{
	return (int32_t)(memcmp(left, right, length));
}

uint64_t mirac_c_strlen(
	const char_t* const cstring)
{
	mirac_debug_assert(cstring != mirac_null);
	return (uint64_t)strlen(cstring);
}

int32_t mirac_c_strcmp(
	const char_t* const left,
	const char_t* const right)
{
	mirac_debug_assert(left != mirac_null);
	mirac_debug_assert(right != mirac_null);
	return strcmp((const char_t*)left, (const char_t*)right);
}

int32_t mirac_c_strncmp(
	const char_t* const left,
	const char_t* const right,
	const uint64_t length)
{
	mirac_debug_assert(left != mirac_null);
	mirac_debug_assert(right != mirac_null);
	return strncmp((const char_t*)left, (const char_t*)right, (size_t)length);
}
