
/**
 * @file utils.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#include <mirac/utils.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

// TODO: implement arenas!

/* static */ int64_t g_allocations_counter = 0;
/* static */ uint64_t g_allocations_in_bytes = 0;

void* mirac_utils_malloc(
	const uint64_t size)
{
	mirac_debug_assert(size > 0);
	void* const pointer = (void* const)malloc(size);
	if (!pointer && size) { mirac_logger_panic("internal failure -- failed to allocate memory"); }
	++g_allocations_counter;
	g_allocations_in_bytes += size;
	return pointer;
}

void* mirac_utils_realloc(
	void* pointer,
	const uint64_t size)
{
	mirac_debug_assert(size > 0);
	pointer = (void*)realloc(pointer, size);
	if (!pointer && size) { mirac_logger_panic("internal failure -- failed to reallocate memory"); }
	++g_allocations_counter;
	g_allocations_in_bytes += size;
	return pointer;
}

void mirac_utils_free(
	const void* pointer)
{
	free((void*)pointer);
	pointer = NULL;
}

void mirac_utils_memset(
	void* const pointer,
	const uint8_t value,
	const uint64_t length)
{
	mirac_debug_assert(pointer != NULL);
	(void)memset((void*)pointer, value, length);
}

void mirac_utils_memcpy(
	void* const destination,
	const void* const source,
	const uint64_t length)
{
	mirac_debug_assert(destination != NULL);
	mirac_debug_assert(source != NULL);
	mirac_debug_assert(length > 0);
	(void)memcpy((void*)destination, (const void*)source, length);
}

char* mirac_utils_strdup(
	const char* const string)
{
	mirac_debug_assert(string != NULL);
	char* const result = strdup(string);
	if (!result) { mirac_logger_panic("internal failure -- failed to duplicate (and allocate) string"); }
	return result;
}

char* mirac_utils_strndup(
	const char* const string,
	const uint64_t length)
{
	mirac_debug_assert(string != NULL);
	mirac_debug_assert(length > 0);
	char* const result = strndup(string, length);
	if (!result) { mirac_logger_panic("internal failure -- failed to duplicate (and allocate) string"); }
	return result;
}

int32_t mirac_utils_strcmp(
	const char* const left,
	const char* const right)
{
	mirac_debug_assert(left != NULL);
	mirac_debug_assert(right != NULL);
	return strcmp((const char*)left, (const char*)right);
}

int32_t mirac_utils_strncmp(
	const char* const left,
	const char* const right,
	const uint64_t length)
{
	mirac_debug_assert(left != NULL);
	mirac_debug_assert(right != NULL);
	return strncmp((const char*)left, (const char*)right, (size_t)length);
}

char* mirac_utils_strchr(
	const char* const string,
	const int32_t c)
{
	mirac_debug_assert(string != NULL);
	return strchr((const char*)string, c);
}

void* mirac_utils_bsearch(
	const void* key,
	const void* base,
	const uint64_t members_count,
	const uint64_t member_size,
	int32_t(*compare)(const void*, const void*))
{
	return bsearch(key, base, members_count, member_size, compare);
}
