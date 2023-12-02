
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

#include <mirac/global_arena.h>

#include <mirac/debug.h>
#include <mirac/arena.h>

static mirac_arena_s g_arena;
static bool g_is_arena_created = false;

void mirac_global_arena_create(
	void)
{
	mirac_debug_assert(!g_is_arena_created);
	g_arena = mirac_arena_create();
	g_is_arena_created = true;
}

void mirac_global_arena_destroy(
	void)
{
	mirac_debug_assert(g_is_arena_created);
	mirac_arena_destroy(&g_arena);
}

void* mirac_global_arena_malloc(
	const uint64_t size)
{
	mirac_debug_assert(g_is_arena_created);
	void* const result = mirac_arena_malloc(&g_arena, size);
	mirac_debug_assert(result != NULL);
	return result;
}
