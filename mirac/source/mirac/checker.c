
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
static void validate_ast_block_expr(
	mirac_parser_s* const parser,
	mirac_ast_block_expr_s* const expr_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block_as(
	mirac_parser_s* const parser,
	mirac_ast_block_as_s* const as_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block_scope(
	mirac_parser_s* const parser,
	mirac_ast_block_scope_s* const scope_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block_if(
	mirac_parser_s* const parser,
	mirac_ast_block_if_s* const if_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block_else(
	mirac_parser_s* const parser,
	mirac_ast_block_else_s* const else_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block_loop(
	mirac_parser_s* const parser,
	mirac_ast_block_loop_s* const loop_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block_func(
	mirac_parser_s* const parser,
	mirac_ast_block_func_s* const func_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block_mem(
	mirac_parser_s* const parser,
	mirac_ast_block_mem_s* const mem_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block,
	const uint64_t depth);

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

static void validate_ast_block_expr(
	mirac_parser_s* const parser,
	mirac_ast_block_expr_s* const expr_block,
	const uint64_t depth)
{
	(void)depth;
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(expr_block != NULL);

	if (mirac_token_type_identifier == expr_block->token.type)
	{
		/*
		if (!is_identifier_token_valid_by_ast_unit_refs(&parser->unit, &expr_block->token))
		{
			log_parser_error_and_exit(expr_block->token.location,
				"encountered an undefined identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(expr_block->token.text)
			);
		}
		*/
	}
}

static void validate_ast_block_as(
	mirac_parser_s* const parser,
	mirac_ast_block_as_s* const as_block,
	const uint64_t depth)
{
	(void)depth;
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(as_block != NULL);
}

static void validate_ast_block_scope(
	mirac_parser_s* const parser,
	mirac_ast_block_scope_s* const scope_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(scope_block != NULL);

	for (uint64_t block_index = 0; block_index < scope_block->blocks.count; ++block_index)
	{
		validate_ast_block(parser, &scope_block->blocks.data[block_index], depth);
	}
}

// TODO: add the if, elif, and else order validation to the ast!

static void validate_ast_block_if(
	mirac_parser_s* const parser,
	mirac_ast_block_if_s* const if_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(if_block != NULL);

	validate_ast_block_scope(parser, &if_block->cond_scope, depth);
	validate_ast_block_scope(parser, &if_block->body_scope, depth);
}

static void validate_ast_block_else(
	mirac_parser_s* const parser,
	mirac_ast_block_else_s* const else_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(else_block != NULL);

	validate_ast_block_scope(parser, &else_block->body_scope, depth);
}

static void validate_ast_block_loop(
	mirac_parser_s* const parser,
	mirac_ast_block_loop_s* const loop_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(loop_block != NULL);

	validate_ast_block_scope(parser, &loop_block->cond_scope, depth);
	validate_ast_block_scope(parser, &loop_block->body_scope, depth);
}

static void validate_ast_block_func(
	mirac_parser_s* const parser,
	mirac_ast_block_func_s* const func_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(func_block != NULL);

	/*
	if (is_identifier_token_valid_by_ast_unit_refs(&parser->unit, &func_block->identifier))
	{
		log_parser_error_and_exit(func_block->identifier.location,
			"ERROR X4"
		);
	}
	*/

	validate_ast_block_scope(parser, &func_block->body_scope, depth);
}

static void validate_ast_block_mem(
	mirac_parser_s* const parser,
	mirac_ast_block_mem_s* const mem_block,
	const uint64_t depth)
{
	(void)depth;
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(mem_block != NULL);

	/*
	if (is_identifier_token_valid_by_ast_unit_refs(&parser->unit, &mem_block->identifier))
	{
		log_parser_error_and_exit(mem_block->identifier.location,
			"ERROR X5"
		);
	}
	*/
}

static void validate_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(block != NULL);

	if (depth <= 0)
	{
		if ((block->type != mirac_ast_block_type_func) &&
			(block->type != mirac_ast_block_type_mem))
		{
			/*
			log_parser_error_and_exit(block->location,
				"encountered an invalid global '" mirac_sv_fmt "' block.",
				mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
			);
			*/
		}
	}
	else
	{
		if ((mirac_ast_block_type_func == block->type) ||
			(mirac_ast_block_type_mem  == block->type))
		{
			/*
			log_parser_error_and_exit(block->location,
				"encountered an invalid local '" mirac_sv_fmt "' block.",
				mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
			);
			*/
		}
	}

	switch (block->type)
	{
		case mirac_ast_block_type_expr:  { validate_ast_block_expr(parser, &block->as.expr_block, depth + 1);   } break;
		case mirac_ast_block_type_as:    { validate_ast_block_as(parser, &block->as.as_block, depth + 1);       } break;
		case mirac_ast_block_type_scope: { validate_ast_block_scope(parser, &block->as.scope_block, depth + 1); } break;
		case mirac_ast_block_type_if:    { validate_ast_block_if(parser, &block->as.if_block, depth + 1);       } break;
		case mirac_ast_block_type_else:  { validate_ast_block_else(parser, &block->as.else_block, depth + 1);   } break;
		case mirac_ast_block_type_loop:  { validate_ast_block_loop(parser, &block->as.loop_block, depth + 1);   } break;
		case mirac_ast_block_type_func:  { validate_ast_block_func(parser, &block->as.func_block, depth + 1);   } break;
		case mirac_ast_block_type_mem:   { validate_ast_block_mem(parser, &block->as.mem_block, depth + 1);     } break;

		default:
		{
		} break;
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
	type_check_ast_block_scope(checker, &if_block->cond_scope);
	type_check_ast_block_scope(checker, &if_block->body_scope);
}

static void type_check_ast_block_else(
	mirac_checker_s* const checker,
	mirac_ast_block_else_s* const else_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(else_block != NULL);

	// ... 
	type_check_ast_block_scope(checker, &else_block->body_scope);
}

static void type_check_ast_block_loop(
	mirac_checker_s* const checker,
	mirac_ast_block_loop_s* const loop_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(loop_block != NULL);

	// ... 
	type_check_ast_block_scope(checker, &loop_block->cond_scope);
	type_check_ast_block_scope(checker, &loop_block->body_scope);
}

static void type_check_ast_block_func(
	mirac_checker_s* const checker,
	mirac_ast_block_func_s* const func_block)
{
	mirac_debug_assert(checker != NULL);
	mirac_debug_assert(func_block != NULL);

	// ... 
	type_check_ast_block_scope(checker, &func_block->body_scope);
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
		case mirac_ast_block_type_else:  { type_check_ast_block_else(checker, &block->as.else_block);   } break;
		case mirac_ast_block_type_loop:  { type_check_ast_block_loop(checker, &block->as.loop_block);   } break;
		case mirac_ast_block_type_func:  { type_check_ast_block_func(checker, &block->as.func_block);   } break;
		case mirac_ast_block_type_mem:   { type_check_ast_block_mem(checker, &block->as.mem_block);     } break;

		default:
		{
		} break;
	}
}
