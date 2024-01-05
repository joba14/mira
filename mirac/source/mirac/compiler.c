
/**
 * @file compiler.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-29
 */

#include <mirac/compiler.h>

#include <mirac/debug.h>

#include "./architectures/nasm_x86_64_linux.h"

mirac_compiler_s mirac_compiler_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_ast_unit_s* const unit,
	FILE* const file)
{
	mirac_debug_assert(config != NULL);
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(file != NULL);
	mirac_compiler_s compiler = {0};
	compiler.config = config;
	compiler.arena = arena;
	compiler.unit = unit;
	compiler.file = file;
	return compiler;
}

void mirac_compiler_compile_ast_unit(
	mirac_compiler_s* const compiler)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(compiler->config != NULL);
	mirac_debug_assert(compiler->arena != NULL);
	mirac_debug_assert(compiler->unit != NULL);
	mirac_debug_assert(compiler->file != NULL);

	switch (compiler->config->arch)
	{
		case mirac_config_arch_type_nasm_x86_64_linux: { nasm_x86_64_linux_compile_ast_unit(compiler); } break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}
