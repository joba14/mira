
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
	mirac_node_s* const node = (mirac_node_s* const)mirac_c_malloc(sizeof(mirac_node_s));
	mirac_debug_assert(node != mirac_null);

	if (mirac_null == node)
	{
		mirac_logger_error("internal failure -- failed to allocate memory node.");
		mirac_c_exit(-1);
	}

	node->pointer = (void*)mirac_c_malloc(size);
	mirac_debug_assert(node->pointer != mirac_null);

	if (mirac_null == node->pointer)
	{
		mirac_logger_error("internal failure -- failed to allocate memory node's pointer.");
		mirac_c_exit(-1);
	}

	node->size = size;
	node->next = mirac_null;
	return node;
}

void mirac_node_destroy(
	mirac_node_s* const node)
{
	mirac_debug_assert(node != mirac_null);
	mirac_debug_assert(node->pointer != mirac_null);
	mirac_c_free(node->pointer);
	mirac_c_free(node);
}

mirac_arena_s mirac_arena_from_parts(
	void)
{
	return (mirac_arena_s) {0};
}

void mirac_arena_destroy(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != mirac_null);

	if (!arena->is_used)
	{
		return;
	}

	const mirac_node_s* node_iterator = arena->begin;
	mirac_debug_assert(node_iterator != mirac_null);

	while (node_iterator)
	{
		const mirac_node_s* const node = node_iterator;
		mirac_debug_assert(node != mirac_null);
		node_iterator = node_iterator->next;
		mirac_node_destroy((mirac_node_s* const)node);
	}

	arena->begin = mirac_null;
	arena->end = mirac_null;
}

void* mirac_arena_malloc(
	mirac_arena_s* const arena,
	const uint64_t size)
{
	mirac_debug_assert(arena != mirac_null);
	mirac_debug_assert(size > 0);
	arena->is_used = true;

	if (mirac_null == arena->end)
	{
		mirac_debug_assert(mirac_null == arena->begin);
		arena->end = mirac_node_from_size(size);
		mirac_debug_assert(arena->end != mirac_null);
		arena->begin = arena->end;
	}
	else
	{
		while (arena->end->next != mirac_null)
		{
			arena->end = arena->end->next;
		}

		arena->end->next = mirac_node_from_size(size);
		mirac_debug_assert(arena->end->next != mirac_null);
		arena->end = arena->end->next;
	}

	void* const result = arena->end->pointer;
	mirac_debug_assert(result != mirac_null);
	return result;
}
