
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

	if (NULL == pointer)
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

	if (NULL == pointer)
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
	pointer = NULL;
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
	mirac_debug_assert(pointer != NULL);
	(void)memset((void*)pointer, value, length);
}

void mirac_c_memcpy(
	void* const destination,
	const void* const source,
	const uint64_t length)
{
	mirac_debug_assert(destination != NULL);
	mirac_debug_assert(source != NULL);
	mirac_debug_assert(length > 0);
	(void)memcpy((void*)destination, (const void*)source, length);
}

int32_t mirac_c_strcmp(
	const char* const left,
	const char* const right)
{
	mirac_debug_assert(left != NULL);
	mirac_debug_assert(right != NULL);
	return strcmp((const char*)left, (const char*)right);
}

int32_t mirac_c_strncmp(
	const char* const left,
	const char* const right,
	const uint64_t length)
{
	mirac_debug_assert(left != NULL);
	mirac_debug_assert(right != NULL);
	return strncmp((const char*)left, (const char*)right, (size_t)length);
}

char* mirac_c_strchr(
	const char* const string,
	const int32_t c)
{
	mirac_debug_assert(string != NULL);
	return strchr((const char*)string, c);
}

void* mirac_c_bsearch(
	const void* key,
	const void* base,
	const uint64_t members_count,
	const uint64_t member_size,
	int32_t(*compare)(const void*, const void*))
{
	return bsearch(key, base, members_count, member_size, compare);
}
