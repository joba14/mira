
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

#include <mirac/c_types.h>

void mirac_global_arena_create(
	void);

void mirac_global_arena_destroy(
	void);

void* mirac_global_arena_malloc(
	const uint64_t size);

#endif
