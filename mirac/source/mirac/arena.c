
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

#include <stdlib.h>

mirac_node_s* mirac_node_from_size(
	const uint64_t size)
{
	mirac_debug_assert(size > 0);

	mirac_node_s* node = (mirac_node_s*)malloc(sizeof(mirac_node_s));

	if (NULL == node)
	{
		mirac_logger_error("internal failure -- failed to allocate memory node.");
		exit(-1);
	}

	node->pointer = (void*)malloc(size);

	if (NULL == node->pointer)
	{
		mirac_logger_error("internal failure -- failed to allocate memory node's pointer.");
		exit(-1);
	}

	node->size = size;
	node->next = NULL;
	return node;
}

void mirac_node_destroy(
	mirac_node_s* const node)
{
	mirac_debug_assert(node != NULL);
	free(node->pointer);
	free(node);
}

mirac_arena_s mirac_arena_create(
	void)
{
	mirac_arena_s arena = {0};
	return arena;
}

void mirac_arena_destroy(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
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
