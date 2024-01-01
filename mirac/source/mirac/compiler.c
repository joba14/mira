
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
static void compile_ast_block_call(
	mirac_compiler_s* const compiler,
	mirac_ast_block_call_s* const call_block);

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
static void compile_ast_block_str(
	mirac_compiler_s* const compiler,
	mirac_ast_block_str_s* const str_block);

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

	(void)fprintf(compiler->file, "\n");
	(void)fprintf(compiler->file, "format ELF64 executable 3\n");
	(void)fprintf(compiler->file, "segment readable executable\n");
	(void)fprintf(compiler->file, "entry " mirac_sv_fmt "\n", mirac_sv_arg(compiler->config->entry));
	(void)fprintf(compiler->file, "\n");

	for (uint64_t block_index = 0; block_index < compiler->unit->blocks.count; ++block_index)
	{
		compile_ast_block(compiler, &compiler->unit->blocks.data[block_index]);
	}

	// TODO: implement!
	(void)fprintf(compiler->file, "\n");
	(void)fprintf(compiler->file, "segment .bss\n");
	(void)fprintf(compiler->file, "\targs_ptr: resq 1\n");
	(void)fprintf(compiler->file, "\tret_stack_rsp: resq 1\n");
	(void)fprintf(compiler->file, "\tret_stack: resb 4096\n");
	(void)fprintf(compiler->file, "\tret_stack_end:\n");
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
			(void)fprintf(compiler->file, ";; --- lnot --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tcmp rax, 0\n");
			(void)fprintf(compiler->file, "\tsete al\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_land:
		{
			(void)fprintf(compiler->file, ";; --- land --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tand rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_lor:
		{
			(void)fprintf(compiler->file, ";; --- lor --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tor rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_lxor:
		{
			(void)fprintf(compiler->file, ";; --- lxor --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\txor rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_bnot:
		{
			(void)fprintf(compiler->file, ";; --- bnot --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tnot rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_band:
		{
			(void)fprintf(compiler->file, ";; --- band --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tand rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_bor:
		{
			(void)fprintf(compiler->file, ";; --- bor --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tor rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_bxor:
		{
			(void)fprintf(compiler->file, ";; --- bxor --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\txor rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_shl:
		{
			(void)fprintf(compiler->file, ";; --- shl --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tshl rbx, cl\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_shr:
		{
			(void)fprintf(compiler->file, ";; --- shr --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tshr rbx, cl\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_eq:
		{
			(void)fprintf(compiler->file, ";; --- eq --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rcx, 0\n");
			(void)fprintf(compiler->file, "\tmov rdx, 1\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmove rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_neq:
		{
			(void)fprintf(compiler->file, ";; --- neq --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rcx, 1\n");
			(void)fprintf(compiler->file, "\tmov rdx, 0\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmove rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_gt:
		{
			(void)fprintf(compiler->file, ";; --- gt --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rcx, 0\n");
			(void)fprintf(compiler->file, "\tmov rdx, 1\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmovg rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_gteq:
		{
			(void)fprintf(compiler->file, ";; --- gteq --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rcx, 0\n");
			(void)fprintf(compiler->file, "\tmov rdx, 1\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmovge rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_ls:
		{
			(void)fprintf(compiler->file, ";; --- ls --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rcx, 0\n");
			(void)fprintf(compiler->file, "\tmov rdx, 1\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmovl rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_lseq:
		{
			(void)fprintf(compiler->file, ";; --- lseq --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rcx, 0\n");
			(void)fprintf(compiler->file, "\tmov rdx, 1\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmovle rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_add:
		{
			(void)fprintf(compiler->file, ";; --- add --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tadd rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_inc:
		{
			(void)fprintf(compiler->file, ";; --- inc --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tinc rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sub:
		{
			(void)fprintf(compiler->file, ";; --- sub --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tsub rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_dec:
		{
			(void)fprintf(compiler->file, ";; --- dec --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tdec rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_mul:
		{
			(void)fprintf(compiler->file, ";; --- mul --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tmul rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_div:
		{
			(void)fprintf(compiler->file, ";; --- div --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tmov rdx, 0\n");
			(void)fprintf(compiler->file, "\tdiv rcx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_mod:
		{
			(void)fprintf(compiler->file, ";; --- mod --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tmov rdx, 0\n");
			(void)fprintf(compiler->file, "\tdiv rcx\n");
			(void)fprintf(compiler->file, "\tpush rdx\n");
		} break;

		case mirac_token_type_reserved_ld08:
		{
			(void)fprintf(compiler->file, ";; --- ld08 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\txor rbx, rbx\n");
			(void)fprintf(compiler->file, "\tmov bl, [rax]\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_ld16:
		{
			(void)fprintf(compiler->file, ";; --- ld16 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\txor rbx, rbx\n");
			(void)fprintf(compiler->file, "\tmov bx, [rax]\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_ld32:
		{
			(void)fprintf(compiler->file, ";; --- ld32 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\txor rbx, rbx\n");
			(void)fprintf(compiler->file, "\tmov ebx, [rax]\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_ld64:
		{
			(void)fprintf(compiler->file, ";; --- ld64 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\txor rbx, rbx\n");
			(void)fprintf(compiler->file, "\tmov rbx, [rax]\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_st08:
		{
			(void)fprintf(compiler->file, ";; --- st08 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tmov [rax], bl\n");
		} break;

		case mirac_token_type_reserved_st16:
		{
			(void)fprintf(compiler->file, ";; --- st16 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tmov [rax], bx\n");
		} break;

		case mirac_token_type_reserved_st32:
		{
			(void)fprintf(compiler->file, ";; --- st32 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tmov [rax], ebx\n");
		} break;

		case mirac_token_type_reserved_st64:
		{
			(void)fprintf(compiler->file, ";; --- st64 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tmov [rax], rbx\n");
		} break;

		case mirac_token_type_reserved_sys1:
		{
			(void)fprintf(compiler->file, ";; --- sys1 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sys2:
		{
			(void)fprintf(compiler->file, ";; --- sys2 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tpop rsi\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sys3:
		{
			(void)fprintf(compiler->file, ";; --- sys3 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tpop rsi\n");
			(void)fprintf(compiler->file, "\tpop rdx\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sys4:
		{
			(void)fprintf(compiler->file, ";; --- sys4 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tpop rsi\n");
			(void)fprintf(compiler->file, "\tpop rdx\n");
			(void)fprintf(compiler->file, "\tpop r10\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sys5:
		{
			(void)fprintf(compiler->file, ";; --- sys5 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tpop rsi\n");
			(void)fprintf(compiler->file, "\tpop rdx\n");
			(void)fprintf(compiler->file, "\tpop r10\n");
			(void)fprintf(compiler->file, "\tpop r5\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sys6:
		{
			(void)fprintf(compiler->file, ";; --- sys6 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tpop rsi\n");
			(void)fprintf(compiler->file, "\tpop rdx\n");
			(void)fprintf(compiler->file, "\tpop r10\n");
			(void)fprintf(compiler->file, "\tpop r5\n");
			(void)fprintf(compiler->file, "\tpop r9\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_drop:
		{
			(void)fprintf(compiler->file, ";; --- drop --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
		} break;

		case mirac_token_type_reserved_dup:
		{
			(void)fprintf(compiler->file, ";; --- dup --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_over:
		{
			(void)fprintf(compiler->file, ";; --- over --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_rot:
		{
			(void)fprintf(compiler->file, ";; --- rot --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_swap:
		{
			(void)fprintf(compiler->file, ";; --- swap --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_literal_i08:
		{
			(void)fprintf(compiler->file, ";; --- literal_i08 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ival);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_i16:
		{
			(void)fprintf(compiler->file, ";; --- literal_i16 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ival);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_i32:
		{
			(void)fprintf(compiler->file, ";; --- literal_i32 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ival);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_i64:
		{
			(void)fprintf(compiler->file, ";; --- literal_i64 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ival);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_u08:
		{
			(void)fprintf(compiler->file, ";; --- literal_u08 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rax, %lu\n", expr_block->token.as.uval);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_u16:
		{
			(void)fprintf(compiler->file, ";; --- literal_u16 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rax, %lu\n", expr_block->token.as.uval);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_u32:
		{
			(void)fprintf(compiler->file, ";; --- literal_u32 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rax, %lu\n", expr_block->token.as.uval);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_u64:
		{
			(void)fprintf(compiler->file, ";; --- literal_u64 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			(void)fprintf(compiler->file, "\tmov rax, %lu\n", expr_block->token.as.uval);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_f32:
		{
			(void)fprintf(compiler->file, ";; --- literal_f32 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_f64:
		{
			(void)fprintf(compiler->file, ";; --- literal_f64 --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		case mirac_token_type_literal_ptr:
		{
			(void)fprintf(compiler->file, ";; --- literal_ptr --- \n");
			(void)fprintf(compiler->file, "addr_%lu:\n", expr_block->token.index);
			// TODO: implement!
		} break;

		default:
		{
			// TODO: implement!
		} break;
	}
}

static void compile_ast_block_call(
	mirac_compiler_s* const compiler,
	mirac_ast_block_call_s* const call_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(call_block != NULL);

	// TODO: implement!
}

static void compile_ast_block_as(
	mirac_compiler_s* const compiler,
	mirac_ast_block_as_s* const as_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(as_block != NULL);
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
	compile_ast_block_scope(compiler, &if_block->cond_scope);
	compile_ast_block_scope(compiler, &if_block->body_scope);
}

static void compile_ast_block_elif(
	mirac_compiler_s* const compiler,
	mirac_ast_block_elif_s* const elif_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(elif_block != NULL);

	// TODO: implement!
	compile_ast_block_scope(compiler, &elif_block->cond_scope);
	compile_ast_block_scope(compiler, &elif_block->body_scope);
}

static void compile_ast_block_else(
	mirac_compiler_s* const compiler,
	mirac_ast_block_else_s* const else_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(else_block != NULL);

	// TODO: implement!
	compile_ast_block_scope(compiler, &else_block->body_scope);
}

static void compile_ast_block_loop(
	mirac_compiler_s* const compiler,
	mirac_ast_block_loop_s* const loop_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(loop_block != NULL);

	// TODO: implement!
	compile_ast_block_scope(compiler, &loop_block->cond_scope);
	compile_ast_block_scope(compiler, &loop_block->body_scope);
}

static void compile_ast_block_func(
	mirac_compiler_s* const compiler,
	mirac_ast_block_func_s* const func_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(func_block != NULL);

	// TODO: handle inline functions!
	// TODO: handle unused functions!

	// TODO: implement the func in all supported archs!

	if (func_block->is_entry)
	{
		(void)fprintf(compiler->file, ";; --- entry --- \n");
		(void)fprintf(compiler->file, mirac_sv_fmt ":\n", mirac_sv_arg(func_block->identifier.as.ident));
		(void)fprintf(compiler->file, "\tmov [args_ptr], rsp\n");
		(void)fprintf(compiler->file, "\tmov rax, ret_stack_end\n");
		(void)fprintf(compiler->file, "\tmov [ret_stack_rsp], rax\n");
	}
	else
	{
		(void)fprintf(compiler->file, ";; --- func --- \n");
		(void)fprintf(compiler->file, "func_%lu:\n", func_block->identifier.index);
		(void)fprintf(compiler->file, "\tmov [ret_stack_rsp], rsp\n");
		(void)fprintf(compiler->file, "\tmov rsp, rax\n");
	}

	// TODO: implement!
	compile_ast_block_scope(compiler, &func_block->body_scope);

	if (func_block->is_entry)
	{
		(void)fprintf(compiler->file, ";; --- entry-end --- \n");
		(void)fprintf(compiler->file, "\tmov rax, 60\n");
		(void)fprintf(compiler->file, "\tmov rdi, 0\n");
		(void)fprintf(compiler->file, "\tsyscall\n");
	}
	else
	{
		(void)fprintf(compiler->file, "\t;; --- func-ret --- \n");
		(void)fprintf(compiler->file, "\tmov rax, rsp\n");
		(void)fprintf(compiler->file, "\tmov rsp, [ret_stack_rsp]\n");
		(void)fprintf(compiler->file, "\tret\n");
	}
}

static void compile_ast_block_mem(
	mirac_compiler_s* const compiler,
	mirac_ast_block_mem_s* const mem_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(mem_block != NULL);

	// TODO: handle unused mems!
	// TODO: implement the mem in all supported archs!
}

static void compile_ast_block_str(
	mirac_compiler_s* const compiler,
	mirac_ast_block_str_s* const str_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(str_block != NULL);

	// TODO: handle unused strs!
	// TODO: implement the str in all supported archs!
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
		case mirac_ast_block_type_call:  { compile_ast_block_call(compiler, &block->as.call_block);   } break;
		case mirac_ast_block_type_as:    { compile_ast_block_as(compiler, &block->as.as_block);       } break;
		case mirac_ast_block_type_scope: { compile_ast_block_scope(compiler, &block->as.scope_block); } break;
		case mirac_ast_block_type_if:    { compile_ast_block_if(compiler, &block->as.if_block);       } break;
		case mirac_ast_block_type_elif:  { compile_ast_block_elif(compiler, &block->as.elif_block);   } break;
		case mirac_ast_block_type_else:  { compile_ast_block_else(compiler, &block->as.else_block);   } break;
		case mirac_ast_block_type_loop:  { compile_ast_block_loop(compiler, &block->as.loop_block);   } break;
		case mirac_ast_block_type_func:  { compile_ast_block_func(compiler, &block->as.func_block);   } break;
		case mirac_ast_block_type_mem:   { compile_ast_block_mem(compiler, &block->as.mem_block);     } break;
		case mirac_ast_block_type_str:   { compile_ast_block_str(compiler, &block->as.str_block);     } break;

		default:
		{
		} break;
	}
}
