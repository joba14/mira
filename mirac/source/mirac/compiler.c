
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

	switch (expr_block->token.type)
	{
		case mirac_token_type_reserved_lnot:
		{
			fprintf(compiler->file, ";; --- lnot --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_land:
		{
			fprintf(compiler->file, ";; --- land --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_lor:
		{
			fprintf(compiler->file, ";; --- lor --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_lxor:
		{
			fprintf(compiler->file, ";; --- lxor --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;


		case mirac_token_type_reserved_bnot:
		{
			fprintf(compiler->file, ";; --- bnot --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_band:
		{
			fprintf(compiler->file, ";; --- band --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_bor:
		{
			fprintf(compiler->file, ";; --- bor --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_bxor:
		{
			fprintf(compiler->file, ";; --- bxor --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_shl:
		{
			fprintf(compiler->file, ";; --- shl --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tpop rcx\n");
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tshl rbx, cl\n");
			fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_shr:
		{
			fprintf(compiler->file, ";; --- shr --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tpop rcx\n");
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tshr rbx, cl\n");
			fprintf(compiler->file, "\tpush rbx\n");
		} break;


		case mirac_token_type_reserved_eq:
		{
			fprintf(compiler->file, ";; --- eq --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tmov rcx, 0\n");
			fprintf(compiler->file, "\tmov rdx, 1\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tcmp rax, rbx\n");
			fprintf(compiler->file, "\tcmove rcx, rdx\n");
			fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_neq:
		{
			fprintf(compiler->file, ";; --- neq --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tmov rcx, 1\n");
			fprintf(compiler->file, "\tmov rdx, 0\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tcmp rax, rbx\n");
			fprintf(compiler->file, "\tcmove rcx, rdx\n");
			fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_gt:
		{
			fprintf(compiler->file, ";; --- gt --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tmov rcx, 0\n");
			fprintf(compiler->file, "\tmov rdx, 1\n");
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tcmp rax, rbx\n");
			fprintf(compiler->file, "\tcmovg rcx, rdx\n");
			fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_gteq:
		{
			fprintf(compiler->file, ";; --- gteq --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_ls:
		{
			fprintf(compiler->file, ";; --- ls --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tmov rcx, 0\n");
			fprintf(compiler->file, "\tmov rdx, 1\n");
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tcmp rax, rbx\n");
			fprintf(compiler->file, "\tcmovl rcx, rdx\n");
			fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_lseq:
		{
			fprintf(compiler->file, ";; --- lseq --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;


		case mirac_token_type_reserved_add:
		{
			fprintf(compiler->file, ";; --- add --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tadd rax, rbx\n");
			fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_inc:
		{
			fprintf(compiler->file, ";; --- inc --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
		} break;

		case mirac_token_type_reserved_sub:
		{
			fprintf(compiler->file, ";; --- sub --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tsub rax, rbx\n");
			fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_dec:
		{
			fprintf(compiler->file, ";; --- dec --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
		} break;

		case mirac_token_type_reserved_mul:
		{
			fprintf(compiler->file, ";; --- mul --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tpop rbx\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tmul rbx\n");
			fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_div:
		{
			fprintf(compiler->file, ";; --- div --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tpop rcx\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tmov rdx, 0\n");
			fprintf(compiler->file, "\tdiv rcx\n");
			fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_mod:
		{
			fprintf(compiler->file, ";; --- mod --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			fprintf(compiler->file, "\tpop rcx\n");
			fprintf(compiler->file, "\tpop rax\n");
			fprintf(compiler->file, "\tmov rdx, 0\n");
			fprintf(compiler->file, "\tdiv rcx\n");
			fprintf(compiler->file, "\tpush rdx\n");
		} break;


		case mirac_token_type_reserved_ld08:
		{
			fprintf(compiler->file, ";; --- ld08 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_ld16:
		{
			fprintf(compiler->file, ";; --- ld16 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_ld32:
		{
			fprintf(compiler->file, ";; --- ld32 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_ld64:
		{
			fprintf(compiler->file, ";; --- ld64 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_st08:
		{
			fprintf(compiler->file, ";; --- st08 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_st16:
		{
			fprintf(compiler->file, ";; --- st16 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_st32:
		{
			fprintf(compiler->file, ";; --- st32 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_st64:
		{
			fprintf(compiler->file, ";; --- st64 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_sys1:
		{
			fprintf(compiler->file, ";; --- sys1 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_sys2:
		{
			fprintf(compiler->file, ";; --- sys2 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_sys3:
		{
			fprintf(compiler->file, ";; --- sys3 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_sys4:
		{
			fprintf(compiler->file, ";; --- sys4 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_sys5:
		{
			fprintf(compiler->file, ";; --- sys5 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_sys6:
		{
			fprintf(compiler->file, ";; --- sys6 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;


		case mirac_token_type_reserved_drop:
		{
			fprintf(compiler->file, ";; --- drop --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_dup:
		{
			fprintf(compiler->file, ";; --- dup --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_over:
		{
			fprintf(compiler->file, ";; --- over --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_rot:
		{
			fprintf(compiler->file, ";; --- rot --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_reserved_swap:
		{
			fprintf(compiler->file, ";; --- swap --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;


		case mirac_token_type_literal_i08:
		{
			fprintf(compiler->file, ";; --- literal_i08 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_i16:
		{
			fprintf(compiler->file, ";; --- literal_i16 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_i32:
		{
			fprintf(compiler->file, ";; --- literal_i32 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_i64:
		{
			fprintf(compiler->file, ";; --- literal_i64 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_u08:
		{
			fprintf(compiler->file, ";; --- literal_u08 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_u16:
		{
			fprintf(compiler->file, ";; --- literal_u16 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_u32:
		{
			fprintf(compiler->file, ";; --- literal_u32 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_u64:
		{
			fprintf(compiler->file, ";; --- literal_u64 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_f32:
		{
			fprintf(compiler->file, ";; --- literal_f32 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_f64:
		{
			fprintf(compiler->file, ";; --- literal_f64 --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_ptr:
		{
			fprintf(compiler->file, ";; --- literal_ptr --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_str:
		{
			fprintf(compiler->file, ";; --- literal_str --- \n");
			fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_identifier:
		{
			// TODO: implement!
		} break;


		default:
		{
			// TODO: implement!
		} break;
	}
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

	for (uint64_t block_index = 0; block_index < scope_block->blocks.count; ++block_index)
	{
		compile_ast_block(compiler, &scope_block->blocks.data[block_index]);
	}
}

static void compile_ast_block_if(
	mirac_compiler_s* const compiler,
	mirac_ast_block_if_s* const if_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(if_block != NULL);

	// TODO: implement!
	compile_ast_block_scope(compiler, &if_block->scope);
}

static void compile_ast_block_elif(
	mirac_compiler_s* const compiler,
	mirac_ast_block_elif_s* const elif_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(elif_block != NULL);

	// TODO: implement!
	compile_ast_block_scope(compiler, &elif_block->scope);
}

static void compile_ast_block_else(
	mirac_compiler_s* const compiler,
	mirac_ast_block_else_s* const else_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(else_block != NULL);

	// TODO: implement!
	compile_ast_block_scope(compiler, &else_block->scope);
}

static void compile_ast_block_loop(
	mirac_compiler_s* const compiler,
	mirac_ast_block_loop_s* const loop_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(loop_block != NULL);

	// TODO: implement!
	compile_ast_block_scope(compiler, &loop_block->scope);
}

static void compile_ast_block_func(
	mirac_compiler_s* const compiler,
	mirac_ast_block_func_s* const func_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(func_block != NULL);

	// TODO: implement!
	compile_ast_block_scope(compiler, &func_block->scope);
}

static void compile_ast_block_mem(
	mirac_compiler_s* const compiler,
	mirac_ast_block_mem_s* const mem_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(mem_block != NULL);

	// TODO: implement!
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
