
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

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib malloc function.
 * 
 * @note It will exit with code -1 if malloc fails to allocate heap region.
 * 
 * @param size[in] size of to-be-allocated memory region
 * 
 * @return void*
 */
void* mirac_c_malloc(
	const uint64_t size);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib realloc function.
 * 
 * @note It will exit with code -1 if malloc fails to reallocate heap region.
 * 
 * @param pointer[in/out] pointer to the memory region to reallocate
 * @param size[in]        size of to-be-allocated memory region
 * 
 * @return void*
 */
void* mirac_c_realloc(
	void* pointer,
	const uint64_t size);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib free function.
 * 
 * @note After freeing the memory, the pointer will be set to NULL.
 * 
 * @param pointer[in/out] pointer to the memory region to free
 */
void mirac_c_free(
	const void* pointer);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib exit function.
 * 
 * @param code[in] code to exit with
 */
void mirac_c_exit(
	const int32_t code);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib memset function.
 * 
 * @param pointer[in/out] pointer to a memory region to memset
 * @param value[in]       value to set throughout the memory region 
 * @param length[in]      length of the memory region
 */
void mirac_c_memset(
	void* const pointer,
	const uint8_t value,
	const uint64_t length);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib memcpy function.
 * 
 * @param destination[in/out] pointer to the destination memory region
 * @param source[in]          pointer to the source memory region
 * @param length[in]          length of the memory region to copy over
 */
void mirac_c_memcpy(
	void* const destination,
	const void* const source,
	const uint64_t length);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib memcmp function.
 * 
 * @param left[in]   left memory region
 * @param right[in]  right memory region
 * @param length[in] length of the memory regions
 * 
 * @return int32_t
 */
int32_t mirac_c_memcmp(
	const uint8_t* const left,
	const uint8_t* const right,
	const uint64_t length);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib strlen function.
 * 
 * @param cstring[in] pointer to a c string to have the length calculated of
 * 
 * @return uint64_t
 */
uint64_t mirac_c_strlen(
	const char* const cstring);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib strcmp function.
 * 
 * @param left[in]  left c string
 * @param right[in] right c string
 * 
 * @return int32_t
 */
int32_t mirac_c_strcmp(
	const char* const left,
	const char* const right);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib strcmp function.
 * 
 * @param left[in]   left c string
 * @param right[in]  right c string
 * @param length[in] length of the strings
 * 
 * @return int32_t
 */
int32_t mirac_c_strncmp(
	const char* const left,
	const char* const right,
	const uint64_t length);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib strchr function.
 * 
 * @param string[in] string to search in
 * @param c[in]      char to search
 * 
 * @return char*
 */
char* mirac_c_strchr(
	const char* const string,
	const int32_t c);

// TODO: write unit tests!
/**
 * @brief Wrapper for c's stdlib bsearch function.
 * 
 * @param key[in]           key to look for in the search
 * @param base[in]          pointer to the collection to look into
 * @param members_count[in] collections members count
 * @param member_size[in]   each member size
 * @param compare[in]       comparison function pointer
 * 
 * @return void*
 */
void* mirac_c_bsearch(
	const void* key,
	const void* base,
	const uint64_t members_count,
	const uint64_t member_size,
	int32_t(*compare)(const void*, const void*));

#endif
