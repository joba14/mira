
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
	const mirac_ast_block_expr_s* const expr_block);

// TODO: document!
static void compile_ast_block_call(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_call_s* const call_block);

// TODO: document!
static void compile_ast_block_as(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_as_s* const as_block);

// TODO: document!
static void compile_ast_block_scope(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_scope_s* const scope_block);

// TODO: document!
static void compile_ast_block_if(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_if_s* const if_block);

// TODO: document!
static void compile_ast_block_else(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_else_s* const else_block);

// TODO: document!
static void compile_ast_block_loop(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_loop_s* const loop_block);

// TODO: document!
static void compile_ast_block(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block);

// TODO: document!
static void compile_ast_def_func(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_func_s* const func_def);

// TODO: document!
static void compile_ast_def_mem(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_mem_s* const mem_def);

// TODO: document!
static void compile_ast_def_str(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_str_s* const str_def);

// TODO: document!
static void compile_ast_def(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def);

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
	(void)fprintf(compiler->file, "global " mirac_sv_fmt "\n", mirac_sv_arg(compiler->config->entry));
	(void)fprintf(compiler->file, "\n");

	for (const mirac_ast_def_list_node_s* defs_iterator = compiler->unit->defs.begin; defs_iterator != NULL; defs_iterator = defs_iterator->next)
	{
		mirac_debug_assert(defs_iterator != NULL);
		mirac_debug_assert(defs_iterator->data != NULL);
		compile_ast_def(compiler, defs_iterator->data);
	}
}

static void compile_ast_block_expr(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_expr_s* const expr_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(expr_block != NULL);

	(void)fprintf(compiler->file, "\t;; --- " mirac_sv_fmt " (id: %lu) --- \n",
		mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
		expr_block->token.index
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

		case mirac_token_type_literal_f32:
		{
			// TODO: implement!
		} break;

		case mirac_token_type_literal_f64:
		{
			// TODO: implement!
		} break;

		case mirac_token_type_literal_ptr:
		{
			(void)fprintf(compiler->file, "\tmov rax, %li\n", expr_block->token.as.ptr);
			(void)fprintf(compiler->file, "\tpush rax\n");
		} break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}

static void compile_ast_block_call(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_call_s* const call_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(call_block != NULL);
	mirac_debug_assert(call_block->def != NULL);

	(void)fprintf(compiler->file, "\t;; --- call (id: %lu) --- \n",
		call_block->token.index
	);

	// TODO: implement!
	switch (call_block->def->type)
	{
		case mirac_ast_def_type_func:
		{
			(void)fprintf(compiler->file, "\tcall func_%lu\n", call_block->def->as.func_def.index);
		} break;

		case mirac_ast_def_type_mem:
		{
			(void)fprintf(compiler->file, "\tpush mem_%lu\n", call_block->def->as.mem_def.index);
		} break;

		case mirac_ast_def_type_str:
		{
			(void)fprintf(compiler->file, "\tpush str_%lu\n", call_block->def->as.str_def.index);
		} break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}

static void compile_ast_block_as(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_as_s* const as_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(as_block != NULL);
}

static void compile_ast_block_scope(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_scope_s* const scope_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(scope_block != NULL);

	for (mirac_ast_block_list_node_s* blocks_iterator = scope_block->blocks.begin; blocks_iterator != NULL; blocks_iterator = blocks_iterator->next)
	{
		mirac_debug_assert(blocks_iterator != NULL);
		mirac_debug_assert(blocks_iterator->data != NULL);
		compile_ast_block(compiler, blocks_iterator->data);
	}
}

static void compile_ast_block_if(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_if_s* const if_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(if_block != NULL);
	
	(void)fprintf(compiler->file, "\t;; --- if --- \n");

	(void)fprintf(compiler->file, "prior_if_cond_%lu:\n", if_block->index);
	compile_ast_block(compiler, if_block->cond);
	(void)fprintf(compiler->file, "after_if_cond_%lu:\n", if_block->index);

	(void)fprintf(compiler->file, "\tpop rax\n");
	(void)fprintf(compiler->file, "\ttest rax, rax\n");

	if (if_block->next != NULL)
	{
		mirac_debug_assert(mirac_ast_block_type_else == if_block->next->type);
		(void)fprintf(compiler->file, "\tjz prior_else_body_%lu\n", if_block->next->as.else_block.index);
	}
	else
	{
		(void)fprintf(compiler->file, "\tjz after_if_body_%lu\n", if_block->index);
	}

	(void)fprintf(compiler->file, "prior_if_body_%lu:\n", if_block->index);
	compile_ast_block(compiler, if_block->body);

	if (if_block->next != NULL)
	{
		mirac_debug_assert(mirac_ast_block_type_else == if_block->next->type);
		(void)fprintf(compiler->file, "\tjmp after_else_body_%lu\n", if_block->next->as.else_block.index);
	}
	else
	{
		(void)fprintf(compiler->file, "\tjmp after_if_body_%lu\n", if_block->index);
	}

	(void)fprintf(compiler->file, "after_if_body_%lu:\n", if_block->index);
}

static void compile_ast_block_else(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_else_s* const else_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(else_block != NULL);

	(void)fprintf(compiler->file, "\t;; --- else --- \n");

	(void)fprintf(compiler->file, "prior_else_body_%lu:\n", else_block->index);
	compile_ast_block(compiler, else_block->body);
	(void)fprintf(compiler->file, "after_else_body_%lu:\n", else_block->index);
}

static void compile_ast_block_loop(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_loop_s* const loop_block)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(loop_block != NULL);

	(void)fprintf(compiler->file, "\t;; --- loop --- \n");

	(void)fprintf(compiler->file, "prior_loop_cond_%lu:\n", loop_block->index);
	compile_ast_block(compiler, loop_block->cond);
	(void)fprintf(compiler->file, "after_loop_cond_%lu:\n", loop_block->index);

	(void)fprintf(compiler->file, "\tpop rax\n");
	(void)fprintf(compiler->file, "\ttest rax, rax\n");
	(void)fprintf(compiler->file, "\tjz after_loop_body_%lu\n", loop_block->index);

	(void)fprintf(compiler->file, "prior_loop_body_%lu:\n", loop_block->index);
	compile_ast_block(compiler, loop_block->body);
	(void)fprintf(compiler->file, "\tjmp prior_loop_cond_%lu\n", loop_block->index);
	(void)fprintf(compiler->file, "after_loop_body_%lu:\n", loop_block->index);
}

static void compile_ast_block(
	mirac_compiler_s* const compiler,
	const mirac_ast_block_s* const block)
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
		case mirac_ast_block_type_else:  { compile_ast_block_else(compiler, &block->as.else_block);   } break;
		case mirac_ast_block_type_loop:  { compile_ast_block_loop(compiler, &block->as.loop_block);   } break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}

static void compile_ast_def_func(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_func_s* const func_def)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(func_def != NULL);

	if (func_def->is_entry)
	{
		(void)fprintf(compiler->file, ";; --- entry --- \n");
		(void)fprintf(compiler->file, mirac_sv_fmt ":\n", mirac_sv_arg(func_def->identifier.as.ident));
	}
	else
	{
		(void)fprintf(compiler->file, ";; --- func --- \n");
		(void)fprintf(compiler->file, "func_%lu:\n", func_def->index);
		(void)fprintf(compiler->file, "\tpush rbp\n");
		(void)fprintf(compiler->file, "\tmov rbp, rsp\n");
	}

	compile_ast_block(compiler, func_def->body);

	if (!func_def->is_entry)
	{
		(void)fprintf(compiler->file, "\t;; --- func-ret --- \n");
		(void)fprintf(compiler->file, "\tmov rsp, rbp\n");
		(void)fprintf(compiler->file, "\tpop rbp\n");
		(void)fprintf(compiler->file, "\tret\n");
	}
}

static void compile_ast_def_mem(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_mem_s* const mem_def)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(mem_def != NULL);

	// TODO: handle unused!

	(void)fprintf(compiler->file, "\tmem_%lu resb %lu\n", mem_def->index, mem_def->capacity.as.uval);
}

static void compile_ast_def_str(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_str_s* const str_def)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(str_def != NULL);

	// TODO: handle unused!

	(void)fprintf(compiler->file, "\tstr_%lu db ", str_def->index);

	for (uint64_t char_index = 0; char_index < str_def->literal.as.str.length; ++char_index)
	{
		(void)fprintf(compiler->file, "%i", (int32_t)str_def->literal.as.str.data[char_index]);
		(void)fprintf(compiler->file, (char_index < str_def->literal.as.str.length - 1) ? ", " : "\n");
	}
}

static void compile_ast_def(
	mirac_compiler_s* const compiler,
	const mirac_ast_def_s* const def)
{
	mirac_debug_assert(compiler != NULL);
	mirac_debug_assert(def != NULL);

	if (compiler->config->strip && !def->is_used)
	{
		return;
	}

	(void)fprintf(compiler->file, "section " mirac_sv_fmt "\n",
		mirac_sv_arg(def->section.as.ident)
	);

	switch (def->type)
	{
		case mirac_ast_def_type_func: { compile_ast_def_func(compiler, &def->as.func_def); } break;
		case mirac_ast_def_type_mem:  { compile_ast_def_mem(compiler, &def->as.mem_def);   } break;
		case mirac_ast_def_type_str:  { compile_ast_def_str(compiler, &def->as.str_def);   } break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}

	(void)fprintf(compiler->file, "\n");
}
