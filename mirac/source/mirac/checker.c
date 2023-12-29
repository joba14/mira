
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
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_expr_s* const expr_block);

// TODO: document!
static void type_check_ast_block_as(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_as_s* const as_block);

// TODO: document!
static void type_check_ast_block_scope(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_scope_s* const scope_block);

// TODO: document!
static void type_check_ast_block_if(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_if_s* const if_block);

// TODO: document!
static void type_check_ast_block_elif(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_elif_s* const elif_block);

// TODO: document!
static void type_check_ast_block_else(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_else_s* const else_block);

// TODO: document!
static void type_check_ast_block_loop(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_loop_s* const loop_block);

// TODO: document!
static void type_check_ast_block_func(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_func_s* const func_block);

// TODO: document!
static void type_check_ast_block_mem(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_mem_s* const mem_block);

// TODO: document!
static void type_check_ast_block(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_s* const block);

mirac_checker_s mirac_checker_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena)
{
	mirac_debug_assert(config != NULL);
	mirac_debug_assert(arena != NULL);
	mirac_checker_s checker = {0};
	checker.config = config;
	checker.arena = arena;
	return checker;
}

void mirac_checker_type_check_ast_unit(
	mirac_checker_s* const checker,
	const mirac_ast_unit_s* const unit)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(unit != NULL);

	for (uint64_t block_index = 0; block_index < unit->blocks.count; ++block_index)
	{
		type_check_ast_block(unit, &unit->blocks.data[block_index]);
	}
}

static void type_check_ast_block_expr(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_expr_s* const expr_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(expr_block != NULL);

	// ... 
}

static void type_check_ast_block_as(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_as_s* const as_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(as_block != NULL);

	// ... 
}

static void type_check_ast_block_scope(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_scope_s* const scope_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(scope_block != NULL);

	for (uint64_t block_index = 0; block_index < scope_block->blocks.count; ++block_index)
	{
		type_check_ast_block(unit, &scope_block->blocks.data[block_index]);
	}
}

static void type_check_ast_block_if(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_if_s* const if_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(if_block != NULL);

	// ... 
	type_check_ast_block_scope(unit, &if_block->scope);
}

static void type_check_ast_block_elif(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_elif_s* const elif_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(elif_block != NULL);

	// ... 
	type_check_ast_block_scope(unit, &elif_block->scope);
}

static void type_check_ast_block_else(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_else_s* const else_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(else_block != NULL);

	// ... 
	type_check_ast_block_scope(unit, &else_block->scope);
}

static void type_check_ast_block_loop(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_loop_s* const loop_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(loop_block != NULL);

	// ... 
	type_check_ast_block_scope(unit, &loop_block->scope);
}

static void type_check_ast_block_func(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_func_s* const func_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(func_block != NULL);

	// ... 
}

static void type_check_ast_block_mem(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_mem_s* const mem_block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(mem_block != NULL);
}

static void type_check_ast_block(
	const mirac_ast_unit_s* const unit,
	mirac_ast_block_s* const block)
{
	mirac_debug_assert(unit != NULL);
	mirac_debug_assert(block != NULL);

	switch (block->type)
	{
		case mirac_ast_block_type_expr:  { type_check_ast_block_expr(unit, &block->as.expr_block);   } break;
		case mirac_ast_block_type_as:    { type_check_ast_block_as(unit, &block->as.as_block);       } break;
		case mirac_ast_block_type_scope: { type_check_ast_block_scope(unit, &block->as.scope_block); } break;
		case mirac_ast_block_type_if:    { type_check_ast_block_if(unit, &block->as.if_block);       } break;
		case mirac_ast_block_type_elif:  { type_check_ast_block_elif(unit, &block->as.elif_block);   } break;
		case mirac_ast_block_type_else:  { type_check_ast_block_else(unit, &block->as.else_block);   } break;
		case mirac_ast_block_type_loop:  { type_check_ast_block_loop(unit, &block->as.loop_block);   } break;
		case mirac_ast_block_type_func:  { type_check_ast_block_func(unit, &block->as.func_block);   } break;
		case mirac_ast_block_type_mem:   { type_check_ast_block_mem(unit, &block->as.mem_block);     } break;

		default:
		{
		} break;
	}
}
