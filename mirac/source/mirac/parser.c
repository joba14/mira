
/**
 * @file parser.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-15
 */

#include <mirac/parser.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

mirac_implement_heap_array_type(mirac_tokens_vector, mirac_token_s);
mirac_implement_heap_array_type(mirac_blocks_vector, mirac_ast_block_s);

#define log_parser_error_and_exit(_location, _format, ...)                     \
	do                                                                         \
	{                                                                          \
		(void)fprintf(stderr, mirac_sv_fmt ":%lu:%lu: ",                       \
			mirac_sv_arg((_location).file),                                    \
			(_location).line,                                                  \
			(_location).column);                                               \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		mirac_c_exit(-1);                                                      \
	} while (0)

#if 0
static bool is_token_expr_block_token(
	const mirac_token_s* const token);
#endif

static mirac_ast_block_expr_s create_ast_block_expr(
	mirac_arena_s* const arena);

static mirac_ast_block_scope_s create_ast_block_scope(
	mirac_arena_s* const arena);

static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena);

static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena);

static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena);

static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena);

static mirac_ast_block_func_s create_ast_block_func(
	mirac_arena_s* const arena);

static mirac_ast_block_mem_s create_ast_block_mem(
	mirac_arena_s* const arena);

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser);

static mirac_ast_block_scope_s parse_ast_block_scope(
	mirac_parser_s* const parser);

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser);

static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser);

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser);

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser);

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser);

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser);

void mirac_ast_block_expr_print(
	const mirac_ast_block_expr_s* const expr_block,
	const uint64_t indent)
{
	mirac_debug_assert(expr_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_expr)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&expr_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

void mirac_ast_block_scope_print(
	const mirac_ast_block_scope_s* const scope_block,
	const uint64_t indent)
{
	mirac_debug_assert(scope_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_scope)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("blocks:\n");
	for (uint64_t index = 0; index < scope_block->blocks.count; ++index)
		mirac_ast_block_print(&scope_block->blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

void mirac_ast_block_if_print(
	const mirac_ast_block_if_s* const if_block,
	const uint64_t indent)
{
	mirac_debug_assert(if_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_if)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("blocks:\n");
	mirac_ast_block_scope_print(&if_block->scope, indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

void mirac_ast_block_elif_print(
	const mirac_ast_block_elif_s* const elif_block,
	const uint64_t indent)
{
	mirac_debug_assert(elif_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_elif)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("scope:\n");
	mirac_ast_block_scope_print(&elif_block->scope, indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

void mirac_ast_block_else_print(
	const mirac_ast_block_else_s* const else_block,
	const uint64_t indent)
{
	mirac_debug_assert(else_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_else)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("blocks:\n");
	mirac_ast_block_scope_print(&else_block->scope, indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

void mirac_ast_block_loop_print(
	const mirac_ast_block_loop_s* const loop_block,
	const uint64_t indent)
{
	mirac_debug_assert(loop_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_loop)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("scope:\n");
	mirac_ast_block_scope_print(&loop_block->scope, indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

void mirac_ast_block_func_print(
	const mirac_ast_block_func_s* const func_block,
	const uint64_t indent)
{
	mirac_debug_assert(func_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_func)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("identifier:\n");
	for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&func_block->identifier)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("req_tokens:\n");
	for (uint64_t req_index = 0; req_index < func_block->req_tokens.count; ++req_index)
	{
		for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&func_block->req_tokens.data[req_index])));
	}
	
	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("ret_tokens:\n");
	for (uint64_t ret_index = 0; ret_index < func_block->ret_tokens.count; ++ret_index)
	{
		for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&func_block->ret_tokens.data[ret_index])));
	}

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("scope:\n");
	mirac_ast_block_scope_print(&func_block->scope, indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("is_inlined: %s\n", func_block->is_inlined ? "yes" : "no");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("is_entry: %s\n", func_block->is_entry ? "yes" : "no");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("is_used: %s\n", func_block->is_used ? "yes" : "no");

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

void mirac_ast_block_mem_print(
	const mirac_ast_block_mem_s* const mem_block,
	const uint64_t indent)
{
	mirac_debug_assert(mem_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_mem)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("identifier:\n");
	for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&mem_block->identifier)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("capacity:\n");
	for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&mem_block->capacity)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("is_used: %s\n", mem_block->is_used ? "yes" : "no");

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

mirac_string_view_s mirac_ast_block_type_to_string_view(
	const mirac_ast_block_type_e type)
{
	switch (type)
	{
		case mirac_ast_block_type_expr:  { return mirac_string_view_from_cstring("ast_block_type_expr");  } break;
		case mirac_ast_block_type_scope: { return mirac_string_view_from_cstring("ast_block_type_scope"); } break;
		case mirac_ast_block_type_if:    { return mirac_string_view_from_cstring("ast_block_type_if");    } break;
		case mirac_ast_block_type_elif:  { return mirac_string_view_from_cstring("ast_block_type_elif");  } break;
		case mirac_ast_block_type_else:  { return mirac_string_view_from_cstring("ast_block_type_else");  } break;
		case mirac_ast_block_type_loop:  { return mirac_string_view_from_cstring("ast_block_type_loop");  } break;
		case mirac_ast_block_type_func:  { return mirac_string_view_from_cstring("ast_block_type_func");  } break;
		case mirac_ast_block_type_mem:   { return mirac_string_view_from_cstring("ast_block_type_mem");   } break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
			return mirac_string_view_from_parts("", 0);
		} break;
	}
}

mirac_ast_block_s mirac_ast_block_from_type(
	const mirac_ast_block_type_e type)
{
	mirac_ast_block_s block = {0};
	block.type = type;
	return block;
}

void mirac_ast_block_print(
	const mirac_ast_block_s* const block,
	const uint64_t indent)
{
	mirac_debug_assert(block != NULL);

	switch (block->type)
	{
		case mirac_ast_block_type_expr:
		{
			mirac_ast_block_expr_print(&block->as.expr_block, indent);
		} break;

		case mirac_ast_block_type_scope:
		{
			mirac_ast_block_scope_print(&block->as.scope_block, indent);
		} break;

		case mirac_ast_block_type_if:
		{
			mirac_ast_block_if_print(&block->as.if_block, indent);
		} break;

		case mirac_ast_block_type_elif:
		{
			mirac_ast_block_elif_print(&block->as.elif_block, indent);
		} break;

		case mirac_ast_block_type_else:
		{
			mirac_ast_block_else_print(&block->as.else_block, indent);
		} break;

		case mirac_ast_block_type_loop:
		{
			mirac_ast_block_loop_print(&block->as.loop_block, indent);
		} break;

		case mirac_ast_block_type_func:
		{
			mirac_ast_block_func_print(&block->as.func_block, indent);
		} break;

		case mirac_ast_block_type_mem:
		{
			mirac_ast_block_mem_print(&block->as.mem_block, indent);
		} break;

		default:
		{
			mirac_debug_assert(!"Should never reach this block.");
		} break;
	}
}

mirac_parser_s mirac_parser_from_parts(
	mirac_config_s* const config,
	mirac_arena_s* const arena,
	mirac_lexer_s* const lexer)
{
	mirac_debug_assert(config != NULL);
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(lexer != NULL);
	mirac_parser_s parser = {0};
	parser.config = config;
	parser.arena = arena;
	parser.lexer = lexer;
	parser.block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	return parser;
}

void mirac_parser_destroy(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	*parser = (mirac_parser_s) {0};
}

mirac_ast_block_type_e mirac_parser_parse_next(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(block != NULL);

	if (parser->block.type != mirac_ast_block_type_none)
	{
		*block = parser->block;
		parser->block.type = mirac_ast_block_type_none;
		return block->type;
	}

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	block->location = token.location;

	if (!mirac_lexer_should_stop_lexing(token.type))
	{
		mirac_lexer_unlex(parser->lexer, &token);
	}

	switch (token.type)
	{
		case mirac_token_type_reserved_left_brace:
		{
			block->type = mirac_ast_block_type_scope;
			block->as.scope_block = parse_ast_block_scope(parser);
		} break;

		case mirac_token_type_reserved_if:
		{
			block->type = mirac_ast_block_type_if;
			block->as.if_block = parse_ast_block_if(parser);
		} break;

		case mirac_token_type_reserved_elif:
		{
			block->type = mirac_ast_block_type_elif;
			block->as.elif_block = parse_ast_block_elif(parser);
		} break;

		case mirac_token_type_reserved_else:
		{
			block->type = mirac_ast_block_type_else;
			block->as.else_block = parse_ast_block_else(parser);
		} break;

		case mirac_token_type_reserved_loop:
		{
			block->type = mirac_ast_block_type_loop;
			block->as.loop_block = parse_ast_block_loop(parser);
		} break;

		case mirac_token_type_reserved_inl:
		case mirac_token_type_reserved_func:
		{
			block->type = mirac_ast_block_type_func;
			block->as.func_block = parse_ast_block_func(parser);
		} break;

		case mirac_token_type_reserved_mem:
		{
			block->type = mirac_ast_block_type_mem;
			block->as.mem_block = parse_ast_block_mem(parser);
		} break;

		case mirac_token_type_eof:
		{
			block->type = mirac_ast_block_type_eou;
		} break;

		case mirac_token_type_none:
		{
			block->type = mirac_ast_block_type_none;
		} break;

		default:
		{
			block->type = mirac_ast_block_type_expr;
			block->as.expr_block = parse_ast_block_expr(parser);
		} break;
	}

	return block->type;
}

bool mirac_parser_should_stop_parsing(
	const mirac_ast_block_type_e type)
{
	return (
		(mirac_ast_block_type_eou  == type) ||
		(mirac_ast_block_type_none == type)
	);
}

void mirac_parser_unparse(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(block != NULL);
	parser->block = *block;
}

static mirac_ast_block_expr_s create_ast_block_expr(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_expr_s block_expr = {0};
	return block_expr;
}

static mirac_ast_block_scope_s create_ast_block_scope(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_scope_s block_scope = {0};
	block_scope.blocks = mirac_blocks_vector_from_parts(arena, 1);
	return block_scope;
}

static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_if_s block_if = {0};
	block_if.scope = create_ast_block_scope(arena);
	return block_if;
}

static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_elif_s block_elif = {0};
	block_elif.scope = create_ast_block_scope(arena);
	return block_elif;
}

static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_else_s block_else = {0};
	block_else.scope = create_ast_block_scope(arena);
	return block_else;
}

static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_loop_s block_loop = {0};
	block_loop.scope = create_ast_block_scope(arena);
	return block_loop;
}

static mirac_ast_block_func_s create_ast_block_func(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_func_s block_func = {0};
	block_func.req_tokens = mirac_tokens_vector_from_parts(arena, 1);
	block_func.ret_tokens = mirac_tokens_vector_from_parts(arena, 1);
	block_func.scope = create_ast_block_scope(arena);
	return block_func;
}

static mirac_ast_block_mem_s create_ast_block_mem(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_mem_s mem_block = {0};
	return mem_block;
}

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_expr_s block_expr = create_ast_block_expr(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	block_expr.token = token;
	return block_expr;
}

static mirac_ast_block_scope_s parse_ast_block_scope(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_scope_s block_scope = create_ast_block_scope(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_left_brace == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	while (!mirac_parser_should_stop_parsing(mirac_parser_parse_next(parser, &block)))
	{
		if ((mirac_ast_block_type_expr == block.type) &&
			(mirac_token_type_reserved_right_brace == block.as.expr_block.token.type))
		{
			break;
		}

		mirac_blocks_vector_push(&block_scope.blocks, block);
	}

	return block_scope;
}

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_if_s block_if = create_ast_block_if(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_if == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)mirac_parser_parse_next(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected 'scope' block after 'if' token, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	block_if.scope = block.as.scope_block;
	return block_if;
}

static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_elif_s block_elif = create_ast_block_elif(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_elif == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)mirac_parser_parse_next(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected 'scope' block after 'elif' token, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	block_elif.scope = block.as.scope_block;
	return block_elif;
}

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_else_s block_else = create_ast_block_else(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_else == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)mirac_parser_parse_next(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected 'scope' block after 'else' token, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	block_else.scope = block.as.scope_block;
	return block_else;
}

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_loop_s block_loop = create_ast_block_loop(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_loop == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)mirac_parser_parse_next(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected 'scope' block after 'loop' token, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	block_loop.scope = block.as.scope_block;
	return block_loop;
}

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_func_s block_func = create_ast_block_func(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert((mirac_token_type_reserved_inl  == token.type)
					|| (mirac_token_type_reserved_func == token.type));

	if (mirac_token_type_reserved_inl == token.type)
	{
		block_func.is_inlined = true;
		mirac_token_from_type(mirac_token_type_none);
		(void)mirac_lexer_lex_next(parser->lexer, &token);
	}

	if (token.type != mirac_token_type_reserved_func)
	{
		log_parser_error_and_exit(token.location,
			"expected 'func' token after 'inl' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(mirac_token_type_to_string_view(token.type))
		);
	}

	token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'func' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	block_func.identifier = token;
	token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_token_type_reserved_req == token.type)
	{
		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
		{
			if (!mirac_token_is_type_token(&token))
			{
				// mirac_lexer_unlex(parser->lexer, &token);
				break;
			}

			mirac_tokens_vector_push(&block_func.req_tokens, token);
		}

		if (block_func.req_tokens.count <= 0)
		{
			log_parser_error_and_exit(token.location,
				"no type specifiers were provided after 'req' token."
			);
		}
	}

	if (mirac_token_type_reserved_ret == token.type)
	{
		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
		{
			if (!mirac_token_is_type_token(&token))
			{
				// mirac_lexer_unlex(parser->lexer, &token);
				break;
			}

			mirac_tokens_vector_push(&block_func.ret_tokens, token);
		}

		if (block_func.ret_tokens.count <= 0)
		{
			log_parser_error_and_exit(token.location,
				"no type specifiers were provided after 'ret' token."
			);
		}
	}

	if (token.type != mirac_token_type_reserved_left_brace)
	{
		log_parser_error_and_exit(token.location,
			"expected '{' token after function identifier, req, or ret specifiers, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	mirac_lexer_unlex(parser->lexer, &token);
	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)mirac_parser_parse_next(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected 'scope' block after function identifier, req, or ret specifiers, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	block_func.scope = block.as.scope_block;
	return block_func;
}

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_mem_s mem_block = create_ast_block_mem(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_mem == token.type);

	token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'mem' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	mem_block.identifier = token;
	token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (!mirac_token_is_signed_numeric_literal(&token) &&
		!mirac_token_is_unsigned_numeric_literal(&token))
	{
		log_parser_error_and_exit(token.location,
			"expected capacity token after 'mem''s identifier token to be signed or unsigned integer literal token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	if ((mirac_token_is_signed_numeric_literal(&token) && (token.as.ival <= 0)) ||
		(mirac_token_is_signed_numeric_literal(&token) && (token.as.ival <= 0)))
	{
		log_parser_error_and_exit(token.location,
			"provided capacity token '" mirac_sv_fmt "' must be a positive integer value.",
			mirac_sv_arg(token.text)
		);
	}

	mem_block.capacity = token;
	return mem_block;
}






















// *********************************************** //

#if 0
static bool is_token_expr_block_token(
	const mirac_token_s* const token)
{
	mirac_debug_assert(token != NULL);
	return (
		(mirac_token_type_reserved_lnot == token->type) ||
		(mirac_token_type_reserved_neq  == token->type) ||
		(mirac_token_type_reserved_mod  == token->type) ||
		(mirac_token_type_reserved_band == token->type) ||
		(mirac_token_type_reserved_land == token->type) ||
		(mirac_token_type_reserved_mul  == token->type) ||
		(mirac_token_type_reserved_add  == token->type) ||
		(mirac_token_type_reserved_inc  == token->type) ||
		(mirac_token_type_reserved_sub  == token->type) ||
		(mirac_token_type_reserved_dec  == token->type) ||
		(mirac_token_type_reserved_div  == token->type) ||
		(mirac_token_type_reserved_ls   == token->type) ||
		(mirac_token_type_reserved_shl  == token->type) ||
		(mirac_token_type_reserved_lseq == token->type) ||
		(mirac_token_type_reserved_eq   == token->type) ||
		(mirac_token_type_reserved_gt   == token->type) ||
		(mirac_token_type_reserved_gteq == token->type) ||
		(mirac_token_type_reserved_shr  == token->type) ||
		(mirac_token_type_reserved_drop == token->type) ||
		(mirac_token_type_reserved_sys1 == token->type) ||
		(mirac_token_type_reserved_sys2 == token->type) ||
		(mirac_token_type_reserved_sys3 == token->type) ||
		(mirac_token_type_reserved_sys4 == token->type) ||
		(mirac_token_type_reserved_sys5 == token->type) ||
		(mirac_token_type_reserved_sys6 == token->type) ||
		(mirac_token_type_reserved_st08 == token->type) ||
		(mirac_token_type_reserved_st16 == token->type) ||
		(mirac_token_type_reserved_st32 == token->type) ||
		(mirac_token_type_reserved_st64 == token->type) ||
		(mirac_token_type_reserved_ld08 == token->type) ||
		(mirac_token_type_reserved_ld16 == token->type) ||
		(mirac_token_type_reserved_ld32 == token->type) ||
		(mirac_token_type_reserved_ld64 == token->type) ||
		(mirac_token_type_reserved_dup  == token->type) ||
		(mirac_token_type_reserved_over == token->type) ||
		(mirac_token_type_reserved_rot  == token->type) ||
		(mirac_token_type_reserved_swap == token->type) ||
		(mirac_token_type_reserved_bxor == token->type) ||
		(mirac_token_type_reserved_lxor == token->type) ||
		(mirac_token_type_reserved_as   == token->type) ||
		(mirac_token_type_reserved_f32  == token->type) ||
		(mirac_token_type_reserved_f64  == token->type) ||
		(mirac_token_type_reserved_i08  == token->type) ||
		(mirac_token_type_reserved_i16  == token->type) ||
		(mirac_token_type_reserved_i32  == token->type) ||
		(mirac_token_type_reserved_i64  == token->type) ||
		(mirac_token_type_reserved_ptr  == token->type) ||
		(mirac_token_type_reserved_u08  == token->type) ||
		(mirac_token_type_reserved_u16  == token->type) ||
		(mirac_token_type_reserved_u32  == token->type) ||
		(mirac_token_type_reserved_u64  == token->type) ||
		(mirac_token_type_reserved_bor  == token->type) ||
		(mirac_token_type_reserved_lor  == token->type) ||
		(mirac_token_type_reserved_bnot == token->type) ||
		(mirac_token_type_literal_i08   == token->type) ||
		(mirac_token_type_literal_i16   == token->type) ||
		(mirac_token_type_literal_i32   == token->type) ||
		(mirac_token_type_literal_i64   == token->type) ||
		(mirac_token_type_literal_u08   == token->type) ||
		(mirac_token_type_literal_u16   == token->type) ||
		(mirac_token_type_literal_u32   == token->type) ||
		(mirac_token_type_literal_u64   == token->type) ||
		(mirac_token_type_literal_f32   == token->type) ||
		(mirac_token_type_literal_f64   == token->type) ||
		(mirac_token_type_literal_ptr   == token->type) ||
		(mirac_token_type_literal_str   == token->type) ||
		(mirac_token_type_identifier    == token->type)
	);
}

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_func_s func_block = create_ast_block_func(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert((mirac_token_type_reserved_inl  == token.type)
					|| (mirac_token_type_reserved_func == token.type));

	if (mirac_token_type_reserved_inl == token.type)
	{
		func_block.is_inlined = true;
		(void)mirac_lexer_lex_next(parser->lexer, &token);
	}

	if (mirac_token_type_reserved_func != token.type)
	{
		if (func_block.is_inlined)
		{
			log_parser_error_and_exit(token.location,
				"expected 'func' token after 'inl' token, but encountered '" mirac_sv_fmt "' token.",
				mirac_sv_arg(token.text)
			);
		}
		else
		{
			log_parser_error_and_exit(token.location,
				"encountered an invalid token '" mirac_sv_fmt "' while parsing 'func' block.",
				mirac_sv_arg(token.text)
			);
		}
	}

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'func' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	func_block.is_entry = mirac_string_view_equal(parser->config->entry, token.as.ident);
	func_block.identifier = token;

	if (mirac_lexer_lex_next(parser->lexer, &token) == mirac_token_type_reserved_req)
	{
		mirac_lexer_unlex(parser->lexer, &token);
		func_block.req_block = parse_ast_block_req(parser);
	}

	if (mirac_lexer_lex_next(parser->lexer, &token) == mirac_token_type_reserved_ret)
	{
		mirac_lexer_unlex(parser->lexer, &token);
		func_block.ret_block = parse_ast_block_ret(parser);
	}

	{
		mirac_ast_block_s block = parse_ast_block(parser);
		if (block.type != mirac_ast_block_type_begin_scope)
		{
			log_parser_error_and_exit(token.location,
				"expected '{' token."
			);
		}
	}

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'func' block's rets."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_func_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&func_block.blocks, block);
			} break;
		}
	}

after_func_body_blocks:
	return func_block;
}

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_mem_s mem_block = create_ast_block_mem();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_mem == token.type);

	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'mem' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	mem_block.identifier = token;

	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (!mirac_token_is_signed_numeric_literal(&token) &&
		!mirac_token_is_unsigned_numeric_literal(&token))
	{
		log_parser_error_and_exit(token.location,
			"expected capacity token after 'mem''s identifier token to be signed or unsigned integer literal token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	if ((mirac_token_is_signed_numeric_literal(&token) && (token.as.ival <= 0)) ||
		(mirac_token_is_signed_numeric_literal(&token) && (token.as.ival <= 0)))
	{
		log_parser_error_and_exit(token.location,
			"provided capacity token '" mirac_sv_fmt "' must be a positive integer value.",
			mirac_sv_arg(token.text)
		);
	}

	mem_block.capacity = token;
	return mem_block;
}

#endif
