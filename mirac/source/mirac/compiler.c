
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
#include <mirac/logger.h>

// TODO: document!
static void compile_ast_block_expr(
	mirac_compiler_s* const compiler,
	mirac_ast_block_expr_s* const expr_block);

// TODO: document!
static void compile_ast_block_as(
	mirac_compiler_s* const compiler,
	mirac_ast_block_as_s* const as_block);

// TODO: document!
static void compile_ast_block_scope(
	mirac_compiler_s* const compiler,
	mirac_ast_block_scope_s* const scope_block);

// TODO: document!
static void compile_ast_block_if(
	mirac_compiler_s* const compiler,
	mirac_ast_block_if_s* const if_block);

// TODO: document!
static void compile_ast_block_elif(
	mirac_compiler_s* const compiler,
	mirac_ast_block_elif_s* const elif_block);

// TODO: document!
static void compile_ast_block_else(
	mirac_compiler_s* const compiler,
	mirac_ast_block_else_s* const else_block);

// TODO: document!
static void compile_ast_block_loop(
	mirac_compiler_s* const compiler,
	mirac_ast_block_loop_s* const loop_block);

// TODO: document!
static void compile_ast_block_func(
	mirac_compiler_s* const compiler,
	mirac_ast_block_func_s* const func_block);

// TODO: document!
static void compile_ast_block_mem(
	mirac_compiler_s* const compiler,
	mirac_ast_block_mem_s* const mem_block);

// TODO: document!
static void compile_ast_block(
	mirac_compiler_s* const compiler,
	mirac_ast_block_s* const block);

mirac_compiler_s mirac_compiler_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_ast_unit_s* const unit)
{
	mirac_debug_assert(config != NULL);
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(unit != NULL);
	mirac_compiler_s compiler = {0};
	compiler.config = config;
	compiler.arena = arena;
	compiler.unit = unit;
	return compiler;
}

void mirac_compiler_compile_ast_unit(
	mirac_compiler_s* const compiler)
{
	mirac_debug_assert(compiler != NULL);

	for (uint64_t block_index = 0; block_index < compiler->unit->blocks.count; ++block_index)
	{
		compile_ast_block(compiler, &compiler->unit->blocks.data[block_index]);
	}
}

static void compile_ast_block_expr(
	mirac_compiler_s* const compiler,
	mirac_ast_block_expr_s* const expr_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(expr_block != NULL);
	// ... 
}

static void compile_ast_block_as(
	mirac_compiler_s* const compiler,
	mirac_ast_block_as_s* const as_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(as_block != NULL);
	// ... 
}

static void compile_ast_block_scope(
	mirac_compiler_s* const compiler,
	mirac_ast_block_scope_s* const scope_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(scope_block != NULL);
	// ... 
}

static void compile_ast_block_if(
	mirac_compiler_s* const compiler,
	mirac_ast_block_if_s* const if_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(if_block != NULL);
	// ... 
}

static void compile_ast_block_elif(
	mirac_compiler_s* const compiler,
	mirac_ast_block_elif_s* const elif_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(elif_block != NULL);
	// ... 
}

static void compile_ast_block_else(
	mirac_compiler_s* const compiler,
	mirac_ast_block_else_s* const else_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(else_block != NULL);
	// ... 
}

static void compile_ast_block_loop(
	mirac_compiler_s* const compiler,
	mirac_ast_block_loop_s* const loop_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(loop_block != NULL);
	// ... 
}

static void compile_ast_block_func(
	mirac_compiler_s* const compiler,
	mirac_ast_block_func_s* const func_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(func_block != NULL);
	// ... 
}

static void compile_ast_block_mem(
	mirac_compiler_s* const compiler,
	mirac_ast_block_mem_s* const mem_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(mem_block != NULL);
	// ... 
}

static void compile_ast_block(
	mirac_compiler_s* const compiler,
	mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(block != NULL);

	switch (block->type)
	{
		case mirac_ast_block_type_expr:  { compile_ast_block_expr(compiler, &block->as.expr_block);   } break;
		case mirac_ast_block_type_as:    { compile_ast_block_as(compiler, &block->as.as_block);       } break;
		case mirac_ast_block_type_scope: { compile_ast_block_scope(compiler, &block->as.scope_block); } break;
		case mirac_ast_block_type_if:    { compile_ast_block_if(compiler, &block->as.if_block);       } break;
		case mirac_ast_block_type_elif:  { compile_ast_block_elif(compiler, &block->as.elif_block);   } break;
		case mirac_ast_block_type_else:  { compile_ast_block_else(compiler, &block->as.else_block);   } break;
		case mirac_ast_block_type_loop:  { compile_ast_block_loop(compiler, &block->as.loop_block);   } break;
		case mirac_ast_block_type_func:  { compile_ast_block_func(compiler, &block->as.func_block);   } break;
		case mirac_ast_block_type_mem:   { compile_ast_block_mem(compiler, &block->as.mem_block);     } break;

		default:
		{
		} break;
	}
}
