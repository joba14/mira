
/**
 * @file checker.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-28
 */

#ifndef __mirac__include__mirac__checker_h__
#define __mirac__include__mirac__checker_h__

#include <mirac/c_common.h>
#include <mirac/heap_array.h>
#include <mirac/config.h>
#include <mirac/arena.h>
#include <mirac/lexer.h>
#include <mirac/parser.h>

mirac_define_heap_array_type(mirac_types_stack, mirac_token_type_e);

typedef struct
{
	mirac_config_s* config;
	mirac_arena_s* arena;
	mirac_ast_unit_s* unit;
	mirac_types_stack_s stack;
} mirac_checker_s;

mirac_checker_s mirac_checker_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_ast_unit_s* const unit);

void mirac_checker_type_check_ast_unit(
	mirac_checker_s* const checker);

#endif
