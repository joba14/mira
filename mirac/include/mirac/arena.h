
/**
 * @file arena.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-02
 */

#ifndef __mirac__include__mirac__arena_h__
#define __mirac__include__mirac__arena_h__

#include <mirac/c_common.h>

typedef struct mirac_node_s mirac_node_s;

struct mirac_node_s
{
	void* pointer;
	uint64_t size;
	mirac_node_s* next;
};

// todo: write unit tests!
/**
 * @brief Create arena node, allocate it's pointer with provided size.
 * 
 * @param size size of to-be-allocated memory block
 * 
 * @return mirac_node_s*
 */
mirac_node_s* mirac_node_from_size(
	const uint64_t size);

// todo: write unit tests!
/**
 * @brief Destroy and deallocate the inner pointer and the node itself.
 * 
 * @param node node to destroy
 */
void mirac_node_destroy(
	mirac_node_s* const node);

typedef struct
{
	mirac_node_s* begin;
	mirac_node_s* end;
	bool_t is_used;
} mirac_arena_s;

// todo: write unit tests!
/**
 * @brief Create arena object.
 * 
 * @return mirac_arena_s
 */
mirac_arena_s mirac_arena_from_parts(
	void);

// todo: write unit tests!
/**
 * @brief Destroy and deallocate the entire arena (and it's nodes).
 * 
 * @param arena arena instance
 */
void mirac_arena_destroy(
	mirac_arena_s* const arena);

// todo: write unit tests!
/**
 * @brief Allocate a region of memory with provided size and store the pointer
 * to it wihtin arena's node.
 * 
 * @param arena arena instance
 * @param size  size of to-be-allocated memory block
 * 
 * @return void*
 */
void* mirac_arena_malloc(
	mirac_arena_s* const arena,
	const uint64_t size);

#endif
