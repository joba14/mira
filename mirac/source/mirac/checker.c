
/**
 * @file checker.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-28
 */

#include <mirac/checker.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

mirac_implement_heap_array_type(mirac_types_stack, mirac_token_type_e);

// TODO: document!
static void type_check_ast_block_expr(
	mirac_checker_s* const checker,
	mirac_ast_block_expr_s* const expr_block);

// TODO: document!
static void type_check_ast_block_as(
	mirac_checker_s* const checker,
	mirac_ast_block_as_s* const as_block);

// TODO: document!
static void type_check_ast_block_scope(
	mirac_checker_s* const checker,
	mirac_ast_block_scope_s* const scope_block);

// TODO: document!
static void type_check_ast_block_if(
	mirac_checker_s* const checker,
	mirac_ast_block_if_s* const if_block);

// TODO: document!
static void type_check_ast_block_elif(
	mirac_checker_s* const checker,
	mirac_ast_block_elif_s* const elif_block);

// TODO: document!
static void type_check_ast_block_else(
	mirac_checker_s* const checker,
	mirac_ast_block_else_s* const else_block);

// TODO: document!
static void type_check_ast_block_loop(
	mirac_checker_s* const checker,
	mirac_ast_block_loop_s* const loop_block);

// TODO: document!
static void type_check_ast_block_func(
	mirac_checker_s* const checker,
	mirac_ast_block_func_s* const func_block);

// TODO: document!
static void type_check_ast_block_mem(
	mirac_checker_s* const checker,
	mirac_ast_block_mem_s* const mem_block);

// TODO: document!
static void type_check_ast_block(
	mirac_checker_s* const checker,
	mirac_ast_block_s* const block);

mirac_checker_s mirac_checker_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_ast_unit_s* const unit)
{
	mirac_debug_assert(config != NULL);
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(unit != NULL);
	mirac_checker_s checker = {0};
	checker.config = config;
	checker.arena = arena;
	checker.unit = unit;
	checker.stack = mirac_types_stack_from_parts(arena, 64);
	return checker;
}

void mirac_checker_type_check_ast_unit(
	mirac_checker_s* const checker)
{
	mirac_debug_assert(checker != NULL);

	for (uint64_t block_index = 0; block_index < checker->unit->blocks.count; ++block_index)
	{
		type_check_ast_block(checker, &checker->unit->blocks.data[block_index]);
	}
}

static void type_check_ast_block_expr(
	mirac_checker_s* const checker,
	mirac_ast_block_expr_s* const expr_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(expr_block != NULL);

	// ... 
}

static void type_check_ast_block_as(
	mirac_checker_s* const checker,
	mirac_ast_block_as_s* const as_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(as_block != NULL);

	// ... 
}

static void type_check_ast_block_scope(
	mirac_checker_s* const checker,
	mirac_ast_block_scope_s* const scope_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(scope_block != NULL);

	for (uint64_t block_index = 0; block_index < scope_block->blocks.count; ++block_index)
	{
		type_check_ast_block(checker, &scope_block->blocks.data[block_index]);
	}
}

static void type_check_ast_block_if(
	mirac_checker_s* const checker,
	mirac_ast_block_if_s* const if_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(if_block != NULL);

	// ... 
	type_check_ast_block_scope(checker, &if_block->scope);
}

static void type_check_ast_block_elif(
	mirac_checker_s* const checker,
	mirac_ast_block_elif_s* const elif_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(elif_block != NULL);

	// ... 
	type_check_ast_block_scope(checker, &elif_block->scope);
}

static void type_check_ast_block_else(
	mirac_checker_s* const checker,
	mirac_ast_block_else_s* const else_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(else_block != NULL);

	// ... 
	type_check_ast_block_scope(checker, &else_block->scope);
}

static void type_check_ast_block_loop(
	mirac_checker_s* const checker,
	mirac_ast_block_loop_s* const loop_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(loop_block != NULL);

	// ... 
	type_check_ast_block_scope(checker, &loop_block->scope);
}

static void type_check_ast_block_func(
	mirac_checker_s* const checker,
	mirac_ast_block_func_s* const func_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(func_block != NULL);

	// ... 
}

static void type_check_ast_block_mem(
	mirac_checker_s* const checker,
	mirac_ast_block_mem_s* const mem_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(mem_block != NULL);
}

static void type_check_ast_block(
	mirac_checker_s* const checker,
	mirac_ast_block_s* const block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(block != NULL);

	switch (block->type)
	{
		case mirac_ast_block_type_expr:  { type_check_ast_block_expr(checker, &block->as.expr_block);   } break;
		case mirac_ast_block_type_as:    { type_check_ast_block_as(checker, &block->as.as_block);       } break;
		case mirac_ast_block_type_scope: { type_check_ast_block_scope(checker, &block->as.scope_block); } break;
		case mirac_ast_block_type_if:    { type_check_ast_block_if(checker, &block->as.if_block);       } break;
		case mirac_ast_block_type_elif:  { type_check_ast_block_elif(checker, &block->as.elif_block);   } break;
		case mirac_ast_block_type_else:  { type_check_ast_block_else(checker, &block->as.else_block);   } break;
		case mirac_ast_block_type_loop:  { type_check_ast_block_loop(checker, &block->as.loop_block);   } break;
		case mirac_ast_block_type_func:  { type_check_ast_block_func(checker, &block->as.func_block);   } break;
		case mirac_ast_block_type_mem:   { type_check_ast_block_mem(checker, &block->as.mem_block);     } break;

		default:
		{
		} break;
	}
}
