
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

typedef bool bool_t;
typedef char char_t;
#define mirac_null NULL

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib malloc function.
 * 
 * @note It will exit with code -1 if malloc fails to allocate heap region.
 * 
 * @param size size of to-be-allocated memory region
 * 
 * @return void*
 */
void* mirac_c_malloc(
	const uint64_t size);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib realloc function.
 * 
 * @note It will exit with code -1 if malloc fails to reallocate heap region.
 * 
 * @param pointer pointer to the memory region to reallocate
 * @param size    size of to-be-allocated memory region
 * 
 * @return void*
 */
void* mirac_c_realloc(
	void* pointer,
	const uint64_t size);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib free function.
 * 
 * @note After freeing the memory, the pointer will be set to mirac_null.
 * 
 * @param pointer pointer to the memory region to free
 */
void mirac_c_free(
	const void* pointer);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib exit function.
 * 
 * @param code code to exit with
 */
void mirac_c_exit(
	const int32_t code);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib memset function.
 * 
 * @param pointer pointer to a memory region to memset
 * @param value   value to set throughout the memory region 
 * @param length  length of the memory region
 */
void mirac_c_memset(
	void* const pointer,
	const uint8_t value,
	const uint64_t length);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib memcpy function.
 * 
 * @param destination pointer to the destination memory region
 * @param source      pointer to the source memory region
 * @param length      length of the memory region to copy over
 */
void mirac_c_memcpy(
	void* const destination,
	const void* const source,
	const uint64_t length);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib memcmp function.
 * 
 * @param left   left memory region
 * @param right  right memory region
 * @param length length of the memory regions
 * 
 * @return int32_t
 */
int32_t mirac_c_memcmp(
	const uint8_t* const left,
	const uint8_t* const right,
	const uint64_t length);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib strlen function.
 * 
 * @param cstring pointer to a c string to have the length calculated of
 * 
 * @return uint64_t
 */
uint64_t mirac_c_strlen(
	const char_t* const cstring);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib strcmp function.
 * 
 * @param left  left c string
 * @param right right c string
 * 
 * @return int32_t
 */
int32_t mirac_c_strcmp(
	const char_t* const left,
	const char_t* const right);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib strcmp function.
 * 
 * @param left   left c string
 * @param right  right c string
 * @param length length of the strings
 * 
 * @return int32_t
 */
int32_t mirac_c_strncmp(
	const char_t* const left,
	const char_t* const right,
	const uint64_t length);

// todo: write unit tests!
/**
 * @brief Wrapper for c's stdlib strchr function.
 * 
 * @param string string to search in
 * @param c      char to search
 * 
 * @return char_t*
 */
char_t* mirac_c_strchr(
	const char_t* const string,
	const int32_t c);

#endif
