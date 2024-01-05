
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

#define log_checker_error_and_exit(_location, _format, ...)                    \
	do                                                                         \
	{                                                                          \
		(void)fprintf(stderr, mirac_sv_fmt ":%lu:%lu: ",                       \
			mirac_sv_arg((_location).file),                                    \
			(_location).line,                                                  \
			(_location).column);                                               \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		mirac_c_exit(-1);                                                      \
	} while (0)

// TODO: document!
static void expect_amount_of_arguments(
	mirac_types_stack_s* const stack,
	const mirac_token_s* const token,
	const uint64_t amount);

// TODO: document!
static void type_check_ast_block_expr(
	mirac_checker_s* const checker,
	const mirac_ast_block_expr_s* const expr_block);

// TODO: document!
static void type_check_ast_block_call(
	mirac_checker_s* const checker,
	const mirac_ast_block_call_s* const call_block);

// TODO: document!
static void type_check_ast_block_as(
	mirac_checker_s* const checker,
	const mirac_ast_block_as_s* const as_block);

// TODO: document!
static void type_check_ast_block_scope(
	mirac_checker_s* const checker,
	const mirac_ast_block_scope_s* const scope_block);

// TODO: document!
static void type_check_ast_block_if(
	mirac_checker_s* const checker,
	const mirac_ast_block_if_s* const if_block);

// TODO: document!
static void type_check_ast_block_else(
	mirac_checker_s* const checker,
	const mirac_ast_block_else_s* const else_block);

// TODO: document!
static void type_check_ast_block_loop(
	mirac_checker_s* const checker,
	const mirac_ast_block_loop_s* const loop_block);

// TODO: document!
static void type_check_ast_block(
	mirac_checker_s* const checker,
	const mirac_ast_block_s* const block);

// TODO: document!
static void type_check_ast_def_func(
	mirac_checker_s* const checker,
	const mirac_ast_def_func_s* const func_def);

// TODO: document!
static void type_check_ast_def_mem(
	mirac_checker_s* const checker,
	const mirac_ast_def_mem_s* const mem_def);

// TODO: document!
static void type_check_ast_def_str(
	mirac_checker_s* const checker,
	const mirac_ast_def_str_s* const str_def);

// TODO: document!
static void type_check_ast_def(
	mirac_checker_s* const checker,
	const mirac_ast_def_s* const def);

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

	for (const mirac_ast_def_list_node_s* defs_iterator = checker->unit->defs.begin; defs_iterator != NULL; defs_iterator = defs_iterator->next)
	{
		mirac_debug_assert(defs_iterator != NULL);
		mirac_debug_assert(defs_iterator->data != NULL);
		type_check_ast_def(checker, defs_iterator->data);
	}
}

static void expect_amount_of_arguments(
	mirac_types_stack_s* const stack,
	const mirac_token_s* const token,
	const uint64_t amount)
{
	mirac_debug_assert(stack != NULL);
	mirac_debug_assert(token != NULL);

	if (stack->count < amount)
	{
		log_checker_error_and_exit(token->location,
			"token '" mirac_sv_fmt "' requires %lu argument to be on the stack, but found %lu.",
			mirac_sv_arg(mirac_token_type_to_string_view(token->type)), amount, stack->count
		);
	}
}

static void type_check_ast_block_expr(
	mirac_checker_s* const checker,
	const mirac_ast_block_expr_s* const expr_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(expr_block != NULL);

	switch (expr_block->token.type)
	{
		case mirac_token_type_reserved_lnot:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);

			mirac_token_type_e a;
			(void)mirac_types_stack_pop(&checker->stack, &a);

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_land:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_lor:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_lxor:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_bnot:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);

			mirac_token_type_e a;
			(void)mirac_types_stack_pop(&checker->stack, &a);

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_band:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_bor:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_bxor:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_shl:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_shr:
		{
			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_eq:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_neq:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_gt:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_gteq:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_ls:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_lseq:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_add:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_inc:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);
			checker->stack.count--;
			mirac_types_stack_push(&checker->stack, checker->stack.data[checker->stack.count - 1]);
		} break;

		case mirac_token_type_reserved_sub:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_dec:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);
			checker->stack.count--;
			mirac_types_stack_push(&checker->stack, checker->stack.data[checker->stack.count - 1]);
		} break;

		case mirac_token_type_reserved_mul:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_div:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_mod:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != b)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"encountered different type values: '" mirac_sv_fmt "' and '" mirac_sv_fmt "' on the stack for '" mirac_sv_fmt "' token.",
					mirac_sv_arg(mirac_token_type_to_string_view(a)),
					mirac_sv_arg(mirac_token_type_to_string_view(b)),
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type))
				);
			}

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_ld08:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);

			mirac_token_type_e a;
			(void)mirac_types_stack_pop(&checker->stack, &a);

			if (a != mirac_token_type_reserved_ptr)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'ptr' type to be on top of the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_u08);
		} break;

		case mirac_token_type_reserved_ld16:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);

			mirac_token_type_e a;
			(void)mirac_types_stack_pop(&checker->stack, &a);

			if (a != mirac_token_type_reserved_ptr)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'ptr' type to be on top of the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_u16);
		} break;

		case mirac_token_type_reserved_ld32:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);

			mirac_token_type_e a;
			(void)mirac_types_stack_pop(&checker->stack, &a);

			if (a != mirac_token_type_reserved_ptr)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'ptr' type to be on top of the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_u32);
		} break;

		case mirac_token_type_reserved_ld64:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);

			mirac_token_type_e a;
			(void)mirac_types_stack_pop(&checker->stack, &a);

			if (a != mirac_token_type_reserved_ptr)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'ptr' type to be on top of the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_u64);
		} break;

		case mirac_token_type_reserved_st08:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != mirac_token_type_reserved_ptr)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'ptr' type to be on top of the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}

			if ((b != mirac_token_type_reserved_i08) &&
				(b != mirac_token_type_reserved_u08))
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'i08' or 'u08' type to be the second in the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}
		} break;

		case mirac_token_type_reserved_st16:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != mirac_token_type_reserved_ptr)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'ptr' type to be on top of the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}

			if ((b != mirac_token_type_reserved_i16) &&
				(b != mirac_token_type_reserved_u16))
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'i16' or 'u16' type to be the second in the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}
		} break;

		case mirac_token_type_reserved_st32:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != mirac_token_type_reserved_ptr)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'ptr' type to be on top of the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}

			if ((b != mirac_token_type_reserved_i32) &&
				(b != mirac_token_type_reserved_u32) &&
				(b != mirac_token_type_reserved_f32))
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'i32', 'u32', or'f32' type to be the second in the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}
		} break;

		case mirac_token_type_reserved_st64:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			if (a != mirac_token_type_reserved_ptr)
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'ptr' type to be on top of the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}

			if ((b != mirac_token_type_reserved_i64) &&
				(b != mirac_token_type_reserved_u64) &&
				(b != mirac_token_type_reserved_f64))
			{
				log_checker_error_and_exit(expr_block->token.location,
					"expected 'i64', 'u64', or'f64' type to be the second in the stack for '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' type.",
					mirac_sv_arg(mirac_token_type_to_string_view(expr_block->token.type)),
					mirac_sv_arg(mirac_token_type_to_string_view(a))
				);
			}
		} break;

		case mirac_token_type_reserved_sys1:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_i64);
		} break;

		case mirac_token_type_reserved_sys2:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 3);

			mirac_token_type_e a, b, c;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);
			(void)mirac_types_stack_pop(&checker->stack, &c);

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_i64);
		} break;

		case mirac_token_type_reserved_sys3:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 4);

			mirac_token_type_e a, b, c, d;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);
			(void)mirac_types_stack_pop(&checker->stack, &c);
			(void)mirac_types_stack_pop(&checker->stack, &d);

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_i64);
		} break;

		case mirac_token_type_reserved_sys4:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 5);

			mirac_token_type_e a, b, c, d, e;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);
			(void)mirac_types_stack_pop(&checker->stack, &c);
			(void)mirac_types_stack_pop(&checker->stack, &d);
			(void)mirac_types_stack_pop(&checker->stack, &e);

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_i64);
		} break;

		case mirac_token_type_reserved_sys5:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 6);

			mirac_token_type_e a, b, c, d, e, f;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);
			(void)mirac_types_stack_pop(&checker->stack, &c);
			(void)mirac_types_stack_pop(&checker->stack, &d);
			(void)mirac_types_stack_pop(&checker->stack, &e);
			(void)mirac_types_stack_pop(&checker->stack, &f);

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_i64);
		} break;

		case mirac_token_type_reserved_sys6:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 7);

			mirac_token_type_e a, b, c, d, e, f, g;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);
			(void)mirac_types_stack_pop(&checker->stack, &c);
			(void)mirac_types_stack_pop(&checker->stack, &d);
			(void)mirac_types_stack_pop(&checker->stack, &e);
			(void)mirac_types_stack_pop(&checker->stack, &f);
			(void)mirac_types_stack_pop(&checker->stack, &g);

			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_i64);
		} break;

		case mirac_token_type_reserved_drop:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);

			mirac_token_type_e a;
			(void)mirac_types_stack_pop(&checker->stack, &a);
		} break;

		case mirac_token_type_reserved_dup:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 1);

			mirac_token_type_e a;
			(void)mirac_types_stack_pop(&checker->stack, &a);

			mirac_types_stack_push(&checker->stack, a);
			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_over:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			mirac_types_stack_push(&checker->stack, b);
			mirac_types_stack_push(&checker->stack, a);
			mirac_types_stack_push(&checker->stack, b);
		} break;

		case mirac_token_type_reserved_rot:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 3);

			mirac_token_type_e a, b, c;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);
			(void)mirac_types_stack_pop(&checker->stack, &c);

			mirac_types_stack_push(&checker->stack, b);
			mirac_types_stack_push(&checker->stack, c);
			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_reserved_swap:
		{
			expect_amount_of_arguments(&checker->stack, &expr_block->token, 2);

			mirac_token_type_e a, b;
			(void)mirac_types_stack_pop(&checker->stack, &a);
			(void)mirac_types_stack_pop(&checker->stack, &b);

			mirac_types_stack_push(&checker->stack, a);
		} break;

		case mirac_token_type_literal_i08:
		case mirac_token_type_literal_i16:
		case mirac_token_type_literal_i32:
		case mirac_token_type_literal_i64:
		case mirac_token_type_literal_u08:
		case mirac_token_type_literal_u16:
		case mirac_token_type_literal_u32:
		case mirac_token_type_literal_u64:
		case mirac_token_type_literal_f32:
		case mirac_token_type_literal_f64:
		case mirac_token_type_literal_ptr:
		{
			mirac_types_stack_push(&checker->stack, expr_block->token.type);
		} break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}

static void type_check_ast_block_call(
	mirac_checker_s* const checker,
	const mirac_ast_block_call_s* const call_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(call_block != NULL);

	switch (call_block->def->type)
	{
		case mirac_ast_def_type_func:
		{
			expect_amount_of_arguments(&checker->stack, &call_block->token, call_block->def->as.func_def.req_tokens.count);

			for (mirac_token_list_node_s* reqs_iterator = call_block->def->as.func_def.req_tokens.begin; reqs_iterator != NULL; reqs_iterator = reqs_iterator->next)
			{
				mirac_debug_assert(reqs_iterator != NULL);
				mirac_token_type_e type;
				(void)mirac_types_stack_pop(&checker->stack, &type);
			}

			for (mirac_token_list_node_s* rets_iterator = call_block->def->as.func_def.ret_tokens.begin; rets_iterator != NULL; rets_iterator = rets_iterator->next)
			{
				mirac_debug_assert(rets_iterator != NULL);
				mirac_types_stack_push(&checker->stack, rets_iterator->data.type);
			}
		} break;

		case mirac_ast_def_type_mem:
		{
			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_ptr);
		} break;

		case mirac_ast_def_type_str:
		{
			mirac_types_stack_push(&checker->stack, mirac_token_type_reserved_ptr);
		} break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}

static void type_check_ast_block_as(
	mirac_checker_s* const checker,
	const mirac_ast_block_as_s* const as_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(as_block != NULL);

	mirac_debug_assert(!"type_check_ast_block_as() is not implemented yet!");
}

static void type_check_ast_block_scope(
	mirac_checker_s* const checker,
	const mirac_ast_block_scope_s* const scope_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(scope_block != NULL);

	for (mirac_ast_block_list_node_s* blocks_iterator = scope_block->blocks.begin; blocks_iterator != NULL; blocks_iterator = blocks_iterator->next)
	{
		mirac_debug_assert(blocks_iterator != NULL);
		mirac_debug_assert(blocks_iterator->data != NULL);
		type_check_ast_block(checker, blocks_iterator->data);
	}
}

static void type_check_ast_block_if(
	mirac_checker_s* const checker,
	const mirac_ast_block_if_s* const if_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(if_block != NULL);

	type_check_ast_block(checker, if_block->cond);
	type_check_ast_block(checker, if_block->body);
}

static void type_check_ast_block_else(
	mirac_checker_s* const checker,
	const mirac_ast_block_else_s* const else_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(else_block != NULL);

	type_check_ast_block(checker, else_block->body);
}

static void type_check_ast_block_loop(
	mirac_checker_s* const checker,
	const mirac_ast_block_loop_s* const loop_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(loop_block != NULL);

	type_check_ast_block(checker, loop_block->cond);
	type_check_ast_block(checker, loop_block->body);
}

static void type_check_ast_block(
	mirac_checker_s* const checker,
	const mirac_ast_block_s* const block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(block != NULL);

	switch (block->type)
	{
		case mirac_ast_block_type_expr:  { type_check_ast_block_expr(checker, &block->as.expr_block);   } break;
		case mirac_ast_block_type_call:  { type_check_ast_block_call(checker, &block->as.call_block);   } break;
		case mirac_ast_block_type_as:    { type_check_ast_block_as(checker, &block->as.as_block);       } break;
		case mirac_ast_block_type_scope: { type_check_ast_block_scope(checker, &block->as.scope_block); } break;
		case mirac_ast_block_type_if:    { type_check_ast_block_if(checker, &block->as.if_block);       } break;
		case mirac_ast_block_type_else:  { type_check_ast_block_else(checker, &block->as.else_block);   } break;
		case mirac_ast_block_type_loop:  { type_check_ast_block_loop(checker, &block->as.loop_block);   } break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}

static void type_check_ast_def_func(
	mirac_checker_s* const checker,
	const mirac_ast_def_func_s* const func_def)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(func_def != NULL);

	checker->stack.count = 0;

	for (mirac_token_list_node_s* reqs_iterator = func_def->req_tokens.begin; reqs_iterator != NULL; reqs_iterator = reqs_iterator->next)
	{
		mirac_debug_assert(reqs_iterator != NULL);
		mirac_types_stack_push(&checker->stack, reqs_iterator->data.type);
	}

	type_check_ast_block(checker, func_def->body);

	if (checker->stack.count < func_def->ret_tokens.count)
	{
		log_checker_error_and_exit(func_def->identifier.location,
			"encountered stack underflow in func definition '" mirac_sv_fmt "'.",
			mirac_sv_arg(func_def->identifier.text)
		);
	}
	else if (checker->stack.count > func_def->ret_tokens.count)
	{
		log_checker_error_and_exit(func_def->identifier.location,
			"encountered stack overflow in func definition '" mirac_sv_fmt "'.",
			mirac_sv_arg(func_def->identifier.text)
		);
	}

	for (mirac_token_list_node_s* rets_iterator = func_def->ret_tokens.begin; rets_iterator != NULL; rets_iterator = rets_iterator->next)
	{
		mirac_debug_assert(rets_iterator != NULL);
		// TODO: do the right ting.
	}
}

static void type_check_ast_def_mem(
	mirac_checker_s* const checker,
	const mirac_ast_def_mem_s* const mem_def)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(mem_def != NULL);
}

static void type_check_ast_def_str(
	mirac_checker_s* const checker,
	const mirac_ast_def_str_s* const str_def)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(str_def != NULL);
}

static void type_check_ast_def(
	mirac_checker_s* const checker,
	const mirac_ast_def_s* const def)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(def != NULL);

	switch (def->type)
	{
		case mirac_ast_def_type_func: { type_check_ast_def_func(checker, &def->as.func_def); } break;
		case mirac_ast_def_type_mem:  { type_check_ast_def_mem(checker, &def->as.mem_def);   } break;
		case mirac_ast_def_type_str:  { type_check_ast_def_str(checker, &def->as.str_def);   } break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}
