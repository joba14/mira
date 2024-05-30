
/**
 * @file nasm_x86_64_linux.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2024-01-05
 */

#include "./nasm_x86_64_linux.h"

#include <mirac/debug.h>
#include <mirac/logger.h>

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block_expr(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block_ident(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block_call(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block_as(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block_scope(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block_if(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block_else(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block_loop(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_block(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_def_fun(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_def_mem(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_def_str(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def);

// todo: write unit tests!
// todo: document!
static void nasm_x86_64_linux_compile_ast_def(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def);

void nasm_x86_64_linux_compile_ast_unit(
	mirac_compiler_s* const compiler)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	(void)fprintf(compiler->file, "\n");
	(void)fprintf(compiler->file, "global " mirac_sv_fmt "\n", mirac_sv_arg(compiler->config->entry));
	(void)fprintf(compiler->file, "\n");

	for (const mirac_ast_def_list_node_s* defs_iterator = compiler->unit->defs.begin; defs_iterator != mirac_null; defs_iterator = defs_iterator->next)
	{
		mirac_debug_assert(defs_iterator != mirac_null);
		mirac_debug_assert(defs_iterator->data != mirac_null);
		nasm_x86_64_linux_compile_ast_def(compiler, defs_iterator->data);
	}

	// todo(#001): this should be reworked to be more dynamic in regard to specific functions.
	//             In other words - it should NOT be pre-hard-coded!
	(void)fprintf(compiler->file, "\n");
	(void)fprintf(compiler->file, "section .bss\n");
	// (void)fprintf(compiler->file, "\targs_ptr: resq 1\n");
	(void)fprintf(compiler->file, "\t__ret_stack_rsp: resq 1\n");
	(void)fprintf(compiler->file, "\t__ret_stack: resb 4096\n");
	(void)fprintf(compiler->file, "\t__ret_stack_end:\n");
	(void)fprintf(compiler->file, "\n");
}

static void nasm_x86_64_linux_compile_ast_block_expr(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_expr == block->type);

	const mirac_ast_block_expr_s* const expr_block = &block->as.expr_block;
	mirac_debug_assert(expr_block != mirac_null);

	(void)fprintf(compiler->file, "\t;; --- " mirac_sv_fmt " --- \n",
		mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
	);

	switch (expr_block->token.type)
	{
		case mirac_token_type_reserved_lnot:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tcmp rax, 0\n");
			(void)fprintf(compiler->file, "\tsete al\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_land:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tand rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_lor:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tor rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_lxor:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\txor rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_bnot:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tnot rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_band:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tand rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_bor:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tor rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_bxor:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\txor rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_shl:
		{
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tshl rbx, cl\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_shr:
		{
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tshr rbx, cl\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_add:
		{
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tadd rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_inc:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tinc rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sub:
		{
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tsub rax, rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_dec:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tdec rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_mul:
		{
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tmul rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_div:
		{
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tmov rdx, 0\n");
			(void)fprintf(compiler->file, "\tdiv rcx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_mod:
		{
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tmov rdx, 0\n");
			(void)fprintf(compiler->file, "\tdiv rcx\n");
			(void)fprintf(compiler->file, "\tpush rdx\n");
		} break;

		case mirac_token_type_reserved_divmod:
		{
			(void)fprintf(compiler->file, "\txor rdx, rdx\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tdiv rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rdx\n");
		} break;

		case mirac_token_type_reserved_eq:
		{
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
			(void)fprintf(compiler->file, "\tmov rcx, 0\n");
			(void)fprintf(compiler->file, "\tmov rdx, 1\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmovg rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_gteq:
		{
			(void)fprintf(compiler->file, "\tmov rcx, 0\n");
			(void)fprintf(compiler->file, "\tmov rdx, 1\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmovge rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_ls:
		{
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
			(void)fprintf(compiler->file, "\tmov rcx, 0\n");
			(void)fprintf(compiler->file, "\tmov rdx, 1\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tcmp rax, rbx\n");
			(void)fprintf(compiler->file, "\tcmovle rcx, rdx\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_drop:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
		} break;

		case mirac_token_type_reserved_dup:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_over:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_rot:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpop rcx\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rcx\n");
		} break;

		case mirac_token_type_reserved_swap:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_ld08:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\txor rbx, rbx\n");
			(void)fprintf(compiler->file, "\tmov bl, [rax]\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_ld16:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\txor rbx, rbx\n");
			(void)fprintf(compiler->file, "\tmov bx, [rax]\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_ld32:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\txor rbx, rbx\n");
			(void)fprintf(compiler->file, "\tmov ebx, [rax]\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_ld64:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\txor rbx, rbx\n");
			(void)fprintf(compiler->file, "\tmov rbx, [rax]\n");
			(void)fprintf(compiler->file, "\tpush rbx\n");
		} break;

		case mirac_token_type_reserved_st08:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tmov [rax], bl\n");
		} break;

		case mirac_token_type_reserved_st16:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tmov [rax], bx\n");
		} break;

		case mirac_token_type_reserved_st32:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tmov [rax], ebx\n");
		} break;

		case mirac_token_type_reserved_st64:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rbx\n");
			(void)fprintf(compiler->file, "\tmov [rax], rbx\n");
		} break;

		case mirac_token_type_reserved_sys1:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sys2:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tpop rsi\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sys3:
		{
			(void)fprintf(compiler->file, "\tpop rax\n");
			(void)fprintf(compiler->file, "\tpop rdi\n");
			(void)fprintf(compiler->file, "\tpop rsi\n");
			(void)fprintf(compiler->file, "\tpop rdx\n");
			(void)fprintf(compiler->file, "\tsyscall\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_sys4:
		{
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

		case mirac_token_type_reserved_true:
		{
			(void)fprintf(compiler->file, "\tmov rax, 1\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_reserved_false:
		{
			(void)fprintf(compiler->file, "\tmov rax, 0\n");
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_i08:
		{
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ival);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_i16:
		{
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ival);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_i32:
		{
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ival);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_i64:
		{
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ival);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_u08:
		{
			(void)fprintf(compiler->file, "\tmov rax, %lu\n", expr_block->token.as.uval);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_u16:
		{
			(void)fprintf(compiler->file, "\tmov rax, %lu\n", expr_block->token.as.uval);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_u32:
		{
			(void)fprintf(compiler->file, "\tmov rax, %lu\n", expr_block->token.as.uval);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_u64:
		{
			(void)fprintf(compiler->file, "\tmov rax, %lu\n", expr_block->token.as.uval);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		case mirac_token_type_literal_ptr:
		{
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ptr);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		default:
		{
			mirac_logger_debug("encountered an invalid token '" mirac_sv_fmt "' while compiling expr block.",
				mirac_sv_arg(mirac_token_to_string_view(&expr_block->token)));
			mirac_debug_assert(0); // note: should never reach this block.
		} break;
	}
}

static void nasm_x86_64_linux_compile_ast_block_ident(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_ident == block->type);

	const mirac_ast_block_ident_s* const ident_block = &block->as.ident_block;
	mirac_debug_assert(ident_block != mirac_null);
	mirac_debug_assert(ident_block->def != mirac_null);

	(void)fprintf(compiler->file, "\t;; --- ident --- \n");

	switch (ident_block->def->type)
	{
		case mirac_ast_def_type_fun:
		{
			(void)fprintf(compiler->file, "\tpush "mirac_sv_fmt"\n", mirac_sv_arg(ident_block->def->as.fun_def.identifier.as.ident));
		} break;

		case mirac_ast_def_type_mem:
		{
			(void)fprintf(compiler->file, "\tpush "mirac_sv_fmt"\n", mirac_sv_arg(ident_block->def->as.mem_def.identifier.as.ident));
		} break;

		case mirac_ast_def_type_str:
		{
			(void)fprintf(compiler->file, "\tpush "mirac_sv_fmt"\n", mirac_sv_arg(ident_block->def->as.str_def.identifier.as.ident));
		} break;

		default:
		{
			mirac_debug_assert(0); // note: should never reach this block.
		} break;
	}
}

static void nasm_x86_64_linux_compile_ast_block_call(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_call == block->type);

	const mirac_ast_block_call_s* const call_block = &block->as.call_block;
	mirac_debug_assert(call_block != mirac_null);
	mirac_debug_assert(call_block->ident != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_ident == call_block->ident->type);

	const mirac_ast_block_ident_s* const ident_block = &call_block->ident->as.ident_block;
	mirac_debug_assert(ident_block != mirac_null);
	mirac_debug_assert(ident_block->def != mirac_null);
	mirac_debug_assert(mirac_ast_def_type_fun == ident_block->def->type);

	(void)fprintf(compiler->file, "\t;; --- call --- \n");

	switch (ident_block->def->type)
	{
		case mirac_ast_def_type_fun:
		{
			// todo(#001): this should be reworked as well, since it is part of #001 todo.
			(void)fprintf(compiler->file, "\tmov rax, rsp\n");
			(void)fprintf(compiler->file, "\tmov rsp, [__ret_stack_rsp]\n");
			(void)fprintf(compiler->file, "\tcall "mirac_sv_fmt"\n", mirac_sv_arg(ident_block->def->as.fun_def.identifier.as.ident));
			(void)fprintf(compiler->file, "\tmov [__ret_stack_rsp], rsp\n");
			(void)fprintf(compiler->file, "\tmov rsp, rax\n");
		} break;

		default:
		{
			mirac_debug_assert(0); // note: should never reach this block.
		} break;
	}
}

static void nasm_x86_64_linux_compile_ast_block_as(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_as == block->type);
}

static void nasm_x86_64_linux_compile_ast_block_scope(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_scope == block->type);

	const mirac_ast_block_scope_s* const scope_block = &block->as.scope_block;
	mirac_debug_assert(scope_block != mirac_null);

	for (mirac_ast_block_list_node_s* blocks_iterator = scope_block->blocks.begin; blocks_iterator != mirac_null; blocks_iterator = blocks_iterator->next)
	{
		mirac_debug_assert(blocks_iterator != mirac_null);
		mirac_debug_assert(blocks_iterator->data != mirac_null);
		nasm_x86_64_linux_compile_ast_block(compiler, blocks_iterator->data);
	}
}

static void nasm_x86_64_linux_compile_ast_block_if(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_if == block->type);

	const mirac_ast_block_if_s* const if_block = &block->as.if_block;
	mirac_debug_assert(if_block != mirac_null);
	mirac_debug_assert(if_block->cond != mirac_null);
	mirac_debug_assert(if_block->body != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_scope == if_block->cond->type);
	mirac_debug_assert(mirac_ast_block_type_scope == if_block->body->type);
	
	(void)fprintf(compiler->file, "\t;; --- if --- \n");

	(void)fprintf(compiler->file, "__prior_if_cond_%lu:\n", if_block->index);
	nasm_x86_64_linux_compile_ast_block(compiler, if_block->cond);
	(void)fprintf(compiler->file, "__after_if_cond_%lu:\n", if_block->index);

	(void)fprintf(compiler->file, "\tpop rax\n");
	(void)fprintf(compiler->file, "\ttest rax, rax\n");

	if (if_block->next != mirac_null)
	{
		mirac_debug_assert(mirac_ast_block_type_else == if_block->next->type);
		(void)fprintf(compiler->file, "\tjz __prior_else_body_%lu\n", if_block->next->as.else_block.index);
	}
	else
	{
		(void)fprintf(compiler->file, "\tjz __after_if_body_%lu\n", if_block->index);
	}

	(void)fprintf(compiler->file, "__prior_if_body_%lu:\n", if_block->index);
	nasm_x86_64_linux_compile_ast_block(compiler, if_block->body);

	if (if_block->next != mirac_null)
	{
		mirac_debug_assert(mirac_ast_block_type_else == if_block->next->type);
		(void)fprintf(compiler->file, "\tjmp __after_else_body_%lu\n", if_block->next->as.else_block.index);
	}
	else
	{
		(void)fprintf(compiler->file, "\tjmp __after_if_body_%lu\n", if_block->index);
	}

	(void)fprintf(compiler->file, "__after_if_body_%lu:\n", if_block->index);
}

static void nasm_x86_64_linux_compile_ast_block_else(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_else == block->type);

	const mirac_ast_block_else_s* const else_block = &block->as.else_block;
	mirac_debug_assert(else_block != mirac_null);
	mirac_debug_assert(else_block->body != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_scope == else_block->body->type);

	(void)fprintf(compiler->file, "\t;; --- else --- \n");

	(void)fprintf(compiler->file, "__prior_else_body_%lu:\n", else_block->index);
	nasm_x86_64_linux_compile_ast_block(compiler, else_block->body);
	(void)fprintf(compiler->file, "__after_else_body_%lu:\n", else_block->index);
}

static void nasm_x86_64_linux_compile_ast_block_loop(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_loop == block->type);

	const mirac_ast_block_loop_s* const loop_block = &block->as.loop_block;
	mirac_debug_assert(loop_block != mirac_null);
	mirac_debug_assert(loop_block->cond != mirac_null);
	mirac_debug_assert(loop_block->body != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_scope == loop_block->cond->type);
	mirac_debug_assert(mirac_ast_block_type_scope == loop_block->body->type);

	(void)fprintf(compiler->file, "\t;; --- loop --- \n");

	(void)fprintf(compiler->file, "__prior_loop_cond_%lu:\n", loop_block->index);
	nasm_x86_64_linux_compile_ast_block(compiler, loop_block->cond);
	(void)fprintf(compiler->file, "__after_loop_cond_%lu:\n", loop_block->index);

	(void)fprintf(compiler->file, "\tpop rax\n");
	(void)fprintf(compiler->file, "\ttest rax, rax\n");
	(void)fprintf(compiler->file, "\tjz __after_loop_body_%lu\n", loop_block->index);

	(void)fprintf(compiler->file, "__prior_loop_body_%lu:\n", loop_block->index);
	nasm_x86_64_linux_compile_ast_block(compiler, loop_block->body);
	(void)fprintf(compiler->file, "\tjmp __prior_loop_cond_%lu\n", loop_block->index);
	(void)fprintf(compiler->file, "__after_loop_body_%lu:\n", loop_block->index);
}

static void nasm_x86_64_linux_compile_ast_block(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(block != mirac_null);

	switch (block->type)
	{
		case mirac_ast_block_type_expr:  { nasm_x86_64_linux_compile_ast_block_expr(compiler, block);  } break;
		case mirac_ast_block_type_ident: { nasm_x86_64_linux_compile_ast_block_ident(compiler, block); } break;
		case mirac_ast_block_type_call:  { nasm_x86_64_linux_compile_ast_block_call(compiler, block);  } break;
		case mirac_ast_block_type_as:    { nasm_x86_64_linux_compile_ast_block_as(compiler, block);    } break;
		case mirac_ast_block_type_scope: { nasm_x86_64_linux_compile_ast_block_scope(compiler, block); } break;
		case mirac_ast_block_type_if:    { nasm_x86_64_linux_compile_ast_block_if(compiler, block);    } break;
		case mirac_ast_block_type_else:  { nasm_x86_64_linux_compile_ast_block_else(compiler, block);  } break;
		case mirac_ast_block_type_loop:  { nasm_x86_64_linux_compile_ast_block_loop(compiler, block);  } break;

		default:
		{
			mirac_debug_assert(0); // note: should never reach this block.
		} break;
	}
}

static void nasm_x86_64_linux_compile_ast_def_fun(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(def != mirac_null);
	mirac_debug_assert(mirac_ast_def_type_fun == def->type);

	const mirac_ast_def_func_s* const fun_def = &def->as.fun_def;
	mirac_debug_assert(fun_def != mirac_null);
	mirac_debug_assert(fun_def->body != mirac_null);
	mirac_debug_assert(mirac_ast_block_type_scope == fun_def->body->type);

	if (fun_def->is_entry)
	{
		// todo(#001): this should be reworked as well, since it is part of #001 todo.
		(void)fprintf(compiler->file, ";; --- entry --- \n");
		(void)fprintf(compiler->file, mirac_sv_fmt ":\n", mirac_sv_arg(fun_def->identifier.as.ident));
		(void)fprintf(compiler->file, "\tmov rax, __ret_stack_end\n");
		(void)fprintf(compiler->file, "\tmov [__ret_stack_rsp], rax\n");
	}
	else
	{
		// todo(#001): this should be reworked as well, since it is part of #001 todo.
		(void)fprintf(compiler->file, ";; --- fun --- \n");
		(void)fprintf(compiler->file, mirac_sv_fmt ":\n", mirac_sv_arg(fun_def->identifier.as.ident));
		(void)fprintf(compiler->file, "\tmov [__ret_stack_rsp], rsp\n");
		(void)fprintf(compiler->file, "\tmov rsp, rax\n");
	}

	nasm_x86_64_linux_compile_ast_block(compiler, fun_def->body);

	if (!fun_def->is_entry)
	{
		// todo(#001): this should be reworked as well, since it is part of #001 todo.
		(void)fprintf(compiler->file, "\t;; --- fun-ret --- \n");
		(void)fprintf(compiler->file, "\tmov rax, rsp\n");
		(void)fprintf(compiler->file, "\tmov rsp, [__ret_stack_rsp]\n");
		(void)fprintf(compiler->file, "\tret\n");
	}
}

static void nasm_x86_64_linux_compile_ast_def_mem(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(def != mirac_null);
	mirac_debug_assert(mirac_ast_def_type_mem == def->type);

	const mirac_ast_def_mem_s* const mem_def = &def->as.mem_def;
	mirac_debug_assert(mem_def != mirac_null);

	(void)fprintf(compiler->file, "\t"mirac_sv_fmt" resb %lu\n", mirac_sv_arg(mem_def->identifier.as.ident), mem_def->capacity.as.uval);
}

static void nasm_x86_64_linux_compile_ast_def_str(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(def != mirac_null);
	mirac_debug_assert(mirac_ast_def_type_str == def->type);

	const mirac_ast_def_str_s* const str_def = &def->as.str_def;
	mirac_debug_assert(str_def != mirac_null);

	(void)fprintf(compiler->file, "\t"mirac_sv_fmt" db ", mirac_sv_arg(str_def->identifier.as.ident));

	for (uint64_t char_index = 0; char_index < str_def->literal.as.str.length; ++char_index)
	{
		(void)fprintf(compiler->file, "%i", (int32_t)str_def->literal.as.str.data[char_index]);
		(void)fprintf(compiler->file, (char_index < str_def->literal.as.str.length - 1) ? ", " : "\n");
	}
}

static void nasm_x86_64_linux_compile_ast_def(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def)
{
	mirac_debug_assert(compiler != mirac_null);
	mirac_debug_assert(compiler->config != mirac_null);
	mirac_debug_assert(compiler->arena != mirac_null);
	mirac_debug_assert(compiler->unit != mirac_null);
	mirac_debug_assert(compiler->file != mirac_null);

	mirac_debug_assert(def != mirac_null);

	if (compiler->config->strip && !def->is_used)
	{
		return;
	}

	(void)fprintf(compiler->file, "section " mirac_sv_fmt "\n",
		mirac_sv_arg(def->section.as.ident)
	);

	switch (def->type)
	{
		case mirac_ast_def_type_fun: { nasm_x86_64_linux_compile_ast_def_fun(compiler, def); } break;
		case mirac_ast_def_type_mem: { nasm_x86_64_linux_compile_ast_def_mem(compiler, def); } break;
		case mirac_ast_def_type_str: { nasm_x86_64_linux_compile_ast_def_str(compiler, def); } break;

		default:
		{
			mirac_debug_assert(0); // note: should never reach this block.
		} break;
	}

	(void)fprintf(compiler->file, "\n");
}
