
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

#include "./archs/nasm_x86_64_linux.h"

mirac_compiler_s mirac_compiler_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_ast_unit_s* const unit,
	mirac_file_t* const file)
{
	mirac_debug_assert(config != mirac_null);
	mirac_debug_assert(arena != mirac_null);
	mirac_debug_assert(unit != mirac_null);
	mirac_debug_assert(file != mirac_null);

	return (mirac_compiler_s)
	{
		.config = config,
		.arena  = arena,
		.unit   = unit,
		.file   = file
	};
}

void mirac_compiler_compile_ast_unit(
	mirac_compiler_s* const compiler)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	switch (compiler->config->arch)
	{
		case mirac_config_arch_type_nasm_x86_64_linux: { nasm_x86_64_linux_compile_ast_unit(compiler); } break;

		default:
		{
			mirac_debug_assert(0); // note: should never reach this block.
		} break;
	}
}
