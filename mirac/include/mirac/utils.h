
/**
 * @file utils.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__utils_h__
#define __mirac__include__mirac__utils_h__

#include <mirac/c_types.h>

void mirac_utils_memset(
	void* const pointer,
	const uint8_t value,
	const uint64_t length);

void mirac_utils_memcpy(
	void* const destination,
	const void* const source,
	const uint64_t length);

int32_t mirac_utils_strcmp(
	const char* const left,
	const char* const right);

int32_t mirac_utils_strncmp(
	const char* const left,
	const char* const right,
	const uint64_t length);

char* mirac_utils_strchr(
	const char* const string,
	const int32_t c);

void* mirac_utils_bsearch(
	const void* key,
	const void* base,
	const uint64_t members_count,
	const uint64_t member_size,
	int32_t(*compare)(const void*, const void*));

#endif
