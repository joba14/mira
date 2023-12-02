
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

#include <mirac/c_types.h>

typedef struct mirac_node_s mirac_node_s;
#define mirac_region_default_capacity (8ULL * 1024)

struct mirac_node_s
{
	void* pointer;
	uint64_t size;
	mirac_node_s* next;
};

/**
 * @brief Create arena node, allocate it's pointer with provided size.
 */
mirac_node_s* mirac_node_from_size(
	const uint64_t size);

/**
 * @brief Destroy and deallocate the inner pointer and the node itself.
 * 
 * @param node 
 */
void mirac_node_destroy(
	mirac_node_s* const node);

typedef struct
{
	mirac_node_s* begin;
	mirac_node_s* end;
} mirac_arena_s;

/**
 * @brief Create arena object.
 */
mirac_arena_s mirac_arena_create(
	void);

/**
 * @brief Destroy and deallocate the entire arena (and it's nodes).
 */
void mirac_arena_destroy(
	mirac_arena_s* const arena);

/**
 * @brief Allocate a region of memory with provided size and store the pointer
 * to it wihtin arena's node.
 */
void* mirac_arena_malloc(
	mirac_arena_s* const arena,
	const uint64_t size);

#endif
