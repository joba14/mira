
/**
 * @file global_arena.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-02
 */

#ifndef __mirac__include__mirac__global_arena_h__
#define __mirac__include__mirac__global_arena_h__

#include <mirac/c_common.h>

/**
 * @brief Create global arena object.
 */
void mirac_global_arena_create(
	void);

/**
 * @brief Destroy and deallocate the entire global arena (and it's nodes).
 */
void mirac_global_arena_destroy(
	void);

/**
 * @brief Allocate a region of memory with provided size and store the pointer
 * to it wihtin the global arena's node.
 */
void* mirac_global_arena_malloc(
	const uint64_t size);

#endif
