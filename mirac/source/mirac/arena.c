
/**
 * @file arena.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-02
 */

#include <mirac/arena.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

mirac_node_s* mirac_node_from_size(
	const uint64_t size)
{
	mirac_debug_assert(size > 0);
	mirac_node_s* node = (mirac_node_s*)mirac_c_malloc(sizeof(mirac_node_s));

	if (NULL == node)
	{
		mirac_logger_error("internal failure -- failed to allocate memory node.");
		mirac_c_exit(-1);
	}

	node->pointer = (void*)mirac_c_malloc(size);

	if (NULL == node->pointer)
	{
		mirac_logger_error("internal failure -- failed to allocate memory node's pointer.");
		mirac_c_exit(-1);
	}

	node->size = size;
	node->next = NULL;
	return node;
}

void mirac_node_destroy(
	mirac_node_s* const node)
{
	mirac_debug_assert(node != NULL);
	mirac_c_free(node->pointer);
	mirac_c_free(node);
}

mirac_arena_s mirac_arena_from_parts(
	void)
{
	mirac_arena_s arena = {0};
	return arena;
}

void mirac_arena_destroy(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);

	if (!arena->is_used)
	{
		return;
	}

	const mirac_node_s* node_iterator = arena->begin;
	mirac_debug_assert(node_iterator != NULL);

	while (node_iterator)
	{
		const mirac_node_s* const node = node_iterator;
		node_iterator = node_iterator->next;
		mirac_node_destroy((mirac_node_s* const)node);
	}

	arena->begin = NULL;
	arena->end = NULL;
}

void* mirac_arena_malloc(
	mirac_arena_s* const arena,
	const uint64_t size)
{
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(size > 0);
	arena->is_used = true;

	if (NULL == arena->end)
	{
		mirac_debug_assert(NULL == arena->begin);
		arena->end = mirac_node_from_size(size);
		mirac_debug_assert(arena->end != NULL);
		arena->begin = arena->end;
	}
	else
	{
		while (arena->end->next != NULL)
		{
			arena->end = arena->end->next;
		}

		arena->end->next = mirac_node_from_size(size);
		mirac_debug_assert(arena->end->next != NULL);
		arena->end = arena->end->next;
	}

	void* const result = arena->end->pointer;
	mirac_debug_assert(result != NULL);
	return result;
}
