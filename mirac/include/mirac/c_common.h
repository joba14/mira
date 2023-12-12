
/**
 * @file c_common.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-03
 */

#ifndef __mirac__include__mirac__c_common_h__
#define __mirac__include__mirac__c_common_h__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

// TODO: document!
void* mirac_c_malloc(
	const uint64_t size);

// TODO: document!
void* mirac_c_realloc(
	void* pointer,
	const uint64_t size);

// TODO: document!
void mirac_c_free(
	const void* pointer);

// TODO: document!
void mirac_c_exit(
	const int32_t code);

// TODO: document!
void mirac_c_memset(
	void* const pointer,
	const uint8_t value,
	const uint64_t length);

// TODO: document!
void mirac_c_memcpy(
	void* const destination,
	const void* const source,
	const uint64_t length);

// TODO: document!
int32_t mirac_c_memcmp(
	const uint8_t* const left,
	const uint8_t* const right,
	const uint64_t length);

// TODO: document!
uint64_t mirac_c_strlen(
	const char* const cstring);

// TODO: document!
int32_t mirac_c_strcmp(
	const char* const left,
	const char* const right);

// TODO: document!
int32_t mirac_c_strncmp(
	const char* const left,
	const char* const right,
	const uint64_t length);

// TODO: document!
char* mirac_c_strchr(
	const char* const string,
	const int32_t c);

// TODO: document!
void* mirac_c_bsearch(
	const void* key,
	const void* base,
	const uint64_t members_count,
	const uint64_t member_size,
	int32_t(*compare)(const void*, const void*));

#endif
