
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
mirac_implement_heap_array_type(mirac_blocks_refs_vector, mirac_ast_block_s*);

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

// TODO: document!
static mirac_ast_block_expr_s create_ast_block_expr(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_as_s create_ast_block_as(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_scope_s create_ast_block_scope(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_func_s create_ast_block_func(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_mem_s create_ast_block_mem(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_str_s create_ast_block_str(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_as_s parse_ast_block_as(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_scope_s parse_ast_block_scope(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_str_s parse_ast_block_str(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_block_type_e parse_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block);

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
static void validate_ast_block_elif(
	mirac_parser_s* const parser,
	mirac_ast_block_elif_s* const elif_block,
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
static bool should_stop_parsing(
	const mirac_ast_block_type_e type);

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

void mirac_ast_block_as_print(
	const mirac_ast_block_as_s* const as_block,
	const uint64_t indent)
{
	mirac_debug_assert(as_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_as)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("types:\n");
	for (uint64_t type_index = 0; type_index < as_block->type_tokens.count; ++type_index)
	{
		for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&as_block->type_tokens.data[type_index])));
	}

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
	printf("cond_scope:\n");
	mirac_ast_block_scope_print(&if_block->cond_scope, indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_scope:\n");
	mirac_ast_block_scope_print(&if_block->body_scope, indent + 2);

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
	printf("cond_scope:\n");
	mirac_ast_block_scope_print(&elif_block->cond_scope, indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_scope:\n");
	mirac_ast_block_scope_print(&elif_block->body_scope, indent + 2);

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
	printf("body_scope:\n");
	mirac_ast_block_scope_print(&else_block->body_scope, indent + 2);

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
	printf("cond_scope:\n");
	mirac_ast_block_scope_print(&loop_block->cond_scope, indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_scope:\n");
	mirac_ast_block_scope_print(&loop_block->body_scope, indent + 2);

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
	mirac_ast_block_scope_print(&func_block->body_scope, indent + 2);

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

// TODO: document!
void mirac_ast_block_str_print(
	const mirac_ast_block_str_s* const str_block,
	const uint64_t indent)
{
	mirac_debug_assert(str_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type='" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(mirac_ast_block_type_str)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("identifier:\n");
	for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&str_block->identifier)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("literal:\n");
	for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&str_block->literal)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("is_used: %s\n", str_block->is_used ? "yes" : "no");

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

mirac_string_view_s mirac_ast_block_type_to_string_view(
	const mirac_ast_block_type_e type)
{
	switch (type)
	{
		case mirac_ast_block_type_expr:  { return mirac_string_view_from_cstring("expr");  } break;
		case mirac_ast_block_type_as:    { return mirac_string_view_from_cstring("as");    } break;
		case mirac_ast_block_type_scope: { return mirac_string_view_from_cstring("scope"); } break;
		case mirac_ast_block_type_if:    { return mirac_string_view_from_cstring("if");    } break;
		case mirac_ast_block_type_elif:  { return mirac_string_view_from_cstring("elif");  } break;
		case mirac_ast_block_type_else:  { return mirac_string_view_from_cstring("else");  } break;
		case mirac_ast_block_type_loop:  { return mirac_string_view_from_cstring("loop");  } break;
		case mirac_ast_block_type_func:  { return mirac_string_view_from_cstring("func");  } break;
		case mirac_ast_block_type_mem:   { return mirac_string_view_from_cstring("mem");   } break;
		case mirac_ast_block_type_str:   { return mirac_string_view_from_cstring("str");   } break;

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
		case mirac_ast_block_type_expr:  { mirac_ast_block_expr_print(&block->as.expr_block, indent);   } break;
		case mirac_ast_block_type_as:    { mirac_ast_block_as_print(&block->as.as_block, indent);       } break;
		case mirac_ast_block_type_scope: { mirac_ast_block_scope_print(&block->as.scope_block, indent); } break;
		case mirac_ast_block_type_if:    { mirac_ast_block_if_print(&block->as.if_block, indent);       } break;
		case mirac_ast_block_type_elif:  { mirac_ast_block_elif_print(&block->as.elif_block, indent);   } break;
		case mirac_ast_block_type_else:  { mirac_ast_block_else_print(&block->as.else_block, indent);   } break;
		case mirac_ast_block_type_loop:  { mirac_ast_block_loop_print(&block->as.loop_block, indent);   } break;
		case mirac_ast_block_type_func:  { mirac_ast_block_func_print(&block->as.func_block, indent);   } break;
		case mirac_ast_block_type_mem:   { mirac_ast_block_mem_print(&block->as.mem_block, indent);     } break;
		case mirac_ast_block_type_str:   { mirac_ast_block_str_print(&block->as.str_block, indent);     } break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}
}

mirac_ast_unit_s mirac_ast_unit_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_unit_s unit = {0};
	unit.blocks = mirac_blocks_vector_from_parts(arena, 1);
	unit.str_refs = mirac_blocks_refs_vector_from_parts(arena, 1);
	unit.func_refs = mirac_blocks_refs_vector_from_parts(arena, 1);
	unit.mem_refs = mirac_blocks_refs_vector_from_parts(arena, 1);
	return unit;
}

void mirac_ast_unit_print(
	const mirac_ast_unit_s* const unit,
	const uint64_t indent)
{
	mirac_debug_assert(unit != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Unit[\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("blocks:\n");
	for (uint64_t block_index = 0; block_index < unit->blocks.count; ++block_index)
	{
		mirac_ast_block_print(&unit->blocks.data[block_index], indent + 2);
	}

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("str_refs:\n");
	for (uint64_t str_ref_index = 0; str_ref_index < unit->str_refs.count; ++str_ref_index)
	{
		mirac_ast_block_s* const block = unit->str_refs.data[str_ref_index];
		mirac_debug_assert(block != NULL);

		for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&block->as.str_block.identifier)));
	}

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("func_refs:\n");
	for (uint64_t func_ref_index = 0; func_ref_index < unit->func_refs.count; ++func_ref_index)
	{
		mirac_ast_block_s* const block = unit->func_refs.data[func_ref_index];
		mirac_debug_assert(block != NULL);

		for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&block->as.func_block.identifier)));
	}

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("mem_refs:\n");
	for (uint64_t mem_ref_index = 0; mem_ref_index < unit->mem_refs.count; ++mem_ref_index)
	{
		mirac_ast_block_s* const block = unit->mem_refs.data[mem_ref_index];
		mirac_debug_assert(block != NULL);

		for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&block->as.mem_block.identifier)));
	}

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
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
	parser.unit = mirac_ast_unit_from_parts(arena);
	return parser;
}

void mirac_parser_destroy(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	*parser = (mirac_parser_s) {0};
}

mirac_ast_unit_s mirac_parser_parse_ast_unit(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);

	while (!should_stop_parsing(parse_ast_block(parser, &block)))
	{
		mirac_blocks_vector_push(&parser->unit.blocks, block);
	}

	for (uint64_t block_index = 0; block_index < parser->unit.blocks.count; ++block_index)
	{
		validate_ast_block(parser, &parser->unit.blocks.data[block_index], 0);
	}

	return parser->unit;
}

static mirac_ast_block_expr_s create_ast_block_expr(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_expr_s expr_block = {0};
	return expr_block;
}

static mirac_ast_block_as_s create_ast_block_as(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_as_s as_block = {0};
	as_block.type_tokens = mirac_tokens_vector_from_parts(arena, 1);
	return as_block;
}

static mirac_ast_block_scope_s create_ast_block_scope(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_scope_s scope_block = {0};
	scope_block.blocks = mirac_blocks_vector_from_parts(arena, 1);
	return scope_block;
}

static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_if_s if_block = {0};
	if_block.cond_scope = create_ast_block_scope(arena);
	if_block.body_scope = create_ast_block_scope(arena);
	return if_block;
}

static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_elif_s elif_block = {0};
	elif_block.cond_scope = create_ast_block_scope(arena);
	elif_block.body_scope = create_ast_block_scope(arena);
	return elif_block;
}

static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_else_s else_block = {0};
	else_block.body_scope = create_ast_block_scope(arena);
	return else_block;
}

static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_loop_s loop_block = {0};
	loop_block.cond_scope = create_ast_block_scope(arena);
	loop_block.body_scope = create_ast_block_scope(arena);
	return loop_block;
}

static mirac_ast_block_func_s create_ast_block_func(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_func_s func_block = {0};
	func_block.req_tokens = mirac_tokens_vector_from_parts(arena, 1);
	func_block.ret_tokens = mirac_tokens_vector_from_parts(arena, 1);
	func_block.body_scope = create_ast_block_scope(arena);
	return func_block;
}

static mirac_ast_block_mem_s create_ast_block_mem(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_mem_s mem_block = {0};
	return mem_block;
}

static mirac_ast_block_str_s create_ast_block_str(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_str_s str_block = {0};
	return str_block;
}

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_expr_s expr_block = create_ast_block_expr(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	// TODO: check for invalid tokens for expr block?

	expr_block.token = token;
	return expr_block;
}

static mirac_ast_block_as_s parse_ast_block_as(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_as_s as_block = create_ast_block_as(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_as == token.type);

	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
	{
		if (!mirac_token_is_type_token(&token))
		{
			mirac_lexer_unlex(parser->lexer, &token);
			break;
		}

		mirac_tokens_vector_push(&as_block.type_tokens, token);
	}

	if (as_block.type_tokens.count <= 0)
	{
		log_parser_error_and_exit(token.location,
			"no type specifiers were provided after 'as' token."
		);
	}

	return as_block;
}

static mirac_ast_block_scope_s parse_ast_block_scope(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_scope_s scope_block = create_ast_block_scope(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_left_brace == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	while (!should_stop_parsing(parse_ast_block(parser, &block)))
	{
		if ((mirac_ast_block_type_expr == block.type) &&
			(mirac_token_type_reserved_right_brace == block.as.expr_block.token.type))
		{
			break;
		}

		mirac_blocks_vector_push(&scope_block.blocks, block);
	}

	return scope_block;
}

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_if_s if_block = create_ast_block_if(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_if == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)parse_ast_block(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected condition 'scope' block after 'if' token, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	if_block.cond_scope = block.as.scope_block;
	(void)parse_ast_block(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected body 'scope' block after 'if''s condition scope, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	if_block.body_scope = block.as.scope_block;
	return if_block;
}

static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_elif_s elif_block = create_ast_block_elif(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_elif == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)parse_ast_block(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected condition 'scope' block after 'elif' token, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	elif_block.cond_scope = block.as.scope_block;
	(void)parse_ast_block(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected body 'scope' block after 'elif''s condition scope, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	elif_block.body_scope = block.as.scope_block;
	return elif_block;
}

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_else_s else_block = create_ast_block_else(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_else == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)parse_ast_block(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected body 'scope' block after 'else' token, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	else_block.body_scope = block.as.scope_block;
	return else_block;
}

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_loop_s loop_block = create_ast_block_loop(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_loop == token.type);

	mirac_ast_block_s block = mirac_ast_block_from_type(mirac_ast_block_type_none);
	(void)parse_ast_block(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected condition 'scope' block after 'loop' token, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	loop_block.cond_scope = block.as.scope_block;
	(void)parse_ast_block(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected body 'scope' block after 'loop''s condition scope, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	loop_block.body_scope = block.as.scope_block;
	return loop_block;
}

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_func_s func_block = create_ast_block_func(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert((mirac_token_type_reserved_inl  == token.type)
					|| (mirac_token_type_reserved_func == token.type));

	if (mirac_token_type_reserved_inl == token.type)
	{
		func_block.is_inlined = true;
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

	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'func' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	func_block.identifier = token;
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_token_type_reserved_req == token.type)
	{
		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
		{
			if (!mirac_token_is_type_token(&token))
			{
				break;
			}

			mirac_tokens_vector_push(&func_block.req_tokens, token);
		}

		if (func_block.req_tokens.count <= 0)
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
				break;
			}

			mirac_tokens_vector_push(&func_block.ret_tokens, token);
		}

		if (func_block.ret_tokens.count <= 0)
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
	(void)parse_ast_block(parser, &block);

	if (block.type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block.location,
			"expected body 'scope' block after function identifier, req, or ret specifiers, but encountered '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block.type))
		);
	}

	func_block.body_scope = block.as.scope_block;
	return func_block;
}

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_mem_s mem_block = create_ast_block_mem(parser->arena);

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

static mirac_ast_block_str_s parse_ast_block_str(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser);
	mirac_ast_block_str_s str_block = create_ast_block_str(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_str == token.type);

	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'str' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	str_block.identifier = token;
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_literal_str)
	{
		log_parser_error_and_exit(token.location,
			"expected string literal token after 'str''s identifier token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	str_block.literal = token;
	return str_block;
}

static mirac_ast_block_type_e parse_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(block != NULL);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	block->location = token.location;

	if (!mirac_lexer_should_stop_lexing(token.type))
	{
		mirac_lexer_unlex(parser->lexer, &token);
	}

	switch (token.type)
	{
		case mirac_token_type_reserved_as:
		{
			block->type = mirac_ast_block_type_as;
			block->as.as_block = parse_ast_block_as(parser);
		} break;

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

		case mirac_token_type_reserved_str:
		{
			block->type = mirac_ast_block_type_str;
			block->as.str_block = parse_ast_block_str(parser);
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

static void validate_ast_block_expr(
	mirac_parser_s* const parser,
	mirac_ast_block_expr_s* const expr_block,
	const uint64_t depth)
{
	(void)depth;
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(expr_block != NULL);

	/*
	if (mirac_token_type_literal_str == expr_block->token.type)
	{
		for (uint64_t str_ref_index = 0; str_ref_index < parser->unit.str_refs.count; ++str_ref_index)
		{
			if (mirac_string_view_equal(expr_block->token.as.str, (parser->unit.str_refs.data[str_ref_index])->token.as.str))
			{
				return;
			}
		}

		mirac_expr_blocks_refs_vector_push(&parser->unit.str_refs, expr_block);
	}
	*/
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
		mirac_ast_block_s* const block = &scope_block->blocks.data[block_index];
		mirac_debug_assert(block != NULL);
		mirac_ast_block_s* const prev_block = (block_index > 0 ? &scope_block->blocks.data[block_index - 1] : NULL);

		if ((mirac_ast_block_type_elif == block->type) ||
			(mirac_ast_block_type_else == block->type))
		{
			if (NULL == prev_block)
			{
				log_parser_error_and_exit(block->location,
					"missing 'if' block prior to '" mirac_sv_fmt "' block.",
					mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
				);
			}
			else
			{
				if ((prev_block->type != mirac_ast_block_type_if) &&
					(prev_block->type != mirac_ast_block_type_elif))
				{
					log_parser_error_and_exit(block->location,
						"expected 'if' block prior to '" mirac_sv_fmt "' block, but found '" mirac_sv_fmt"' block.",
						mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type)),
						mirac_sv_arg(mirac_ast_block_type_to_string_view(prev_block->type))
					);
				}
			}
		}

		validate_ast_block(parser, block, depth);
	}
}

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

static void validate_ast_block_elif(
	mirac_parser_s* const parser,
	mirac_ast_block_elif_s* const elif_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(elif_block != NULL);

	validate_ast_block_scope(parser, &elif_block->cond_scope, depth);
	validate_ast_block_scope(parser, &elif_block->body_scope, depth);
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
	for (uint64_t func_ref_index = 0; func_ref_index < parser->unit.func_refs.count; ++func_ref_index)
	{
		if (mirac_string_view_equal(func_block->identifier.as.str, (parser->unit.func_refs.data[func_ref_index])->identifier.as.str))
		{
			log_parser_error_and_exit(func_block->identifier.location,
				"encountered a redefinition of a function '" mirac_sv_fmt "'.",
				mirac_sv_arg(func_block->identifier.text)
			);
		}
	}

	mirac_func_blocks_refs_vector_push(&parser->unit.func_refs, func_block);
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
	for (uint64_t mem_ref_index = 0; mem_ref_index < parser->unit.mem_refs.count; ++mem_ref_index)
	{
		if (mirac_string_view_equal(mem_block->identifier.as.str, (parser->unit.mem_refs.data[mem_ref_index])->identifier.as.str))
		{
			log_parser_error_and_exit(mem_block->identifier.location,
				"encountered a redefinition of a memory '" mirac_sv_fmt "'.",
				mirac_sv_arg(mem_block->identifier.text)
			);
		}
	}

	mirac_mem_blocks_refs_vector_push(&parser->unit.mem_refs, mem_block);
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
			(block->type != mirac_ast_block_type_mem)  &&
			(block->type != mirac_ast_block_type_str))
		{
			log_parser_error_and_exit(block->location,
				"encountered an invalid global '" mirac_sv_fmt "' block.",
				mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
			);
		}
	}
	else
	{
		if ((mirac_ast_block_type_func == block->type) ||
			(mirac_ast_block_type_mem  == block->type) ||
			(mirac_ast_block_type_str  == block->type))
		{
			log_parser_error_and_exit(block->location,
				"encountered an invalid local '" mirac_sv_fmt "' block.",
				mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
			);
		}
	}

	switch (block->type)
	{
		case mirac_ast_block_type_expr:  { validate_ast_block_expr(parser, &block->as.expr_block, depth + 1);   } break;
		case mirac_ast_block_type_as:    { validate_ast_block_as(parser, &block->as.as_block, depth + 1);       } break;
		case mirac_ast_block_type_scope: { validate_ast_block_scope(parser, &block->as.scope_block, depth + 1); } break;
		case mirac_ast_block_type_if:    { validate_ast_block_if(parser, &block->as.if_block, depth + 1);       } break;
		case mirac_ast_block_type_elif:  { validate_ast_block_elif(parser, &block->as.elif_block, depth + 1);   } break;
		case mirac_ast_block_type_else:  { validate_ast_block_else(parser, &block->as.else_block, depth + 1);   } break;
		case mirac_ast_block_type_loop:  { validate_ast_block_loop(parser, &block->as.loop_block, depth + 1);   } break;
		case mirac_ast_block_type_func:  { validate_ast_block_func(parser, &block->as.func_block, depth + 1);   } break;
		case mirac_ast_block_type_mem:   { validate_ast_block_mem(parser, &block->as.mem_block, depth + 1);     } break;

		default:
		{
		} break;
	}
}

static bool should_stop_parsing(
	const mirac_ast_block_type_e type)
{
	return (
		(mirac_ast_block_type_eou  == type) ||
		(mirac_ast_block_type_none == type)
	);
}
