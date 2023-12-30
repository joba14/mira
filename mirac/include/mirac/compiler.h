
/**
 * @file compiler.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-29
 */

#ifndef __mirac__include__mirac__compiler_h__
#define __mirac__include__mirac__compiler_h__

#include <mirac/c_common.h>
#include <mirac/heap_array.h>
#include <mirac/config.h>
#include <mirac/arena.h>
#include <mirac/lexer.h>
#include <mirac/parser.h>

#include <stdio.h>

typedef struct
{
	mirac_config_s* config;
	mirac_arena_s* arena;
	mirac_ast_unit_s* unit;
	FILE* file;
} mirac_compiler_s;

mirac_compiler_s mirac_compiler_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_ast_unit_s* const unit,
	FILE* const file);

void mirac_compiler_compile_ast_unit(
	mirac_compiler_s* const compiler);

#endif
