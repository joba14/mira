
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

mirac_implement_heap_array_type(mirac_tokens_refs_vector, mirac_token_s*);
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

static bool is_token_expr_block_token(
	const mirac_token_s* const token);

static mirac_ast_block_expr_s create_ast_block_expr(
	void);

static mirac_ast_block_begin_scope_s create_ast_block_begin_scope(
	void);

static mirac_ast_block_end_scope_s create_ast_block_end_scope(
	void);

static mirac_ast_block_end_unit_s create_ast_block_end_unit(
	void);

static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena);

static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena);

static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena);

static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena);

static mirac_ast_block_let_s create_ast_block_let(
	mirac_arena_s* const arena);

static mirac_ast_block_req_s create_ast_block_req(
	mirac_arena_s* const arena);

static mirac_ast_block_ret_s create_ast_block_ret(
	mirac_arena_s* const arena);

static mirac_ast_block_func_s create_ast_block_func(
	mirac_arena_s* const arena);

static mirac_ast_block_mem_s create_ast_block_mem(
	void);

static mirac_ast_block_s create_ast_block(
	void);

static mirac_ast_unit_s create_ast_unit(
	mirac_arena_s* const arena);

static void print_ast_block_expr(
	const mirac_ast_block_expr_s* const expr_block,
	const uint8_t indent);

static void print_ast_block_begin_scope(
	const mirac_ast_block_begin_scope_s* const begin_scope_block,
	const uint8_t indent);

static void print_ast_block_end_scope(
	const mirac_ast_block_end_scope_s* const end_scope_block,
	const uint8_t indent);

static void print_ast_block_end_unit(
	const mirac_ast_block_end_unit_s* const end_unit_block,
	const uint8_t indent);

static void print_ast_block_if(
	const mirac_ast_block_if_s* const if_block,
	const uint8_t indent);

static void print_ast_block_elif(
	const mirac_ast_block_elif_s* const elif_block,
	const uint8_t indent);

static void print_ast_block_else(
	const mirac_ast_block_else_s* const else_block,
	const uint8_t indent);

static void print_ast_block_loop(
	const mirac_ast_block_loop_s* const loop_block,
	const uint8_t indent);

static void print_ast_block_let(
	const mirac_ast_block_let_s* const let_block,
	const uint8_t indent);

static void print_ast_block_req(
	const mirac_ast_block_req_s* const req_block,
	const uint8_t indent);

static void print_ast_block_ret(
	const mirac_ast_block_ret_s* const ret_block,
	const uint8_t indent);

static void print_ast_block_func(
	const mirac_ast_block_func_s* const func_block,
	const uint8_t indent);

static void print_ast_block_mem(
	const mirac_ast_block_mem_s* const mem_block,
	const uint8_t indent);

static void print_ast_block(
	const mirac_ast_block_s* const block,
	const uint8_t indent);

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser);

static mirac_ast_block_begin_scope_s parse_ast_block_begin_scope(
	mirac_parser_s* const parser);

static mirac_ast_block_end_scope_s parse_ast_block_end_scope(
	mirac_parser_s* const parser);

static mirac_ast_block_end_unit_s parse_ast_block_end_unit(
	mirac_parser_s* const parser);

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser);

static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser);

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser);

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser);

static mirac_ast_block_let_s parse_ast_block_let(
	mirac_parser_s* const parser);

static mirac_ast_block_req_s parse_ast_block_req(
	mirac_parser_s* const parser);

static mirac_ast_block_ret_s parse_ast_block_ret(
	mirac_parser_s* const parser);

static mirac_ast_block_func_s parse_ast_block_func(
	mirac_parser_s* const parser);

static mirac_ast_block_mem_s parse_ast_block_mem(
	mirac_parser_s* const parser);

static mirac_ast_block_s parse_ast_block(
	mirac_parser_s* const parser);

static void unparse_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block);

static void parse_ast_unit(
	mirac_parser_s* const parser,
	mirac_ast_unit_s* const ast_unit);

mirac_string_view_s mirac_ast_block_type_to_string_view(
	const mirac_ast_block_type_e block_type)
{
	switch (block_type)
	{
		case mirac_ast_block_type_expr:        { return mirac_string_view_from_cstring("ast_block_type_expr");        } break;
		case mirac_ast_block_type_begin_scope: { return mirac_string_view_from_cstring("ast_block_type_begin_scope"); } break;
		case mirac_ast_block_type_end_scope:   { return mirac_string_view_from_cstring("ast_block_type_end_scope");   } break;
		case mirac_ast_block_type_end_unit:    { return mirac_string_view_from_cstring("ast_block_type_end_unit");    } break;
		case mirac_ast_block_type_if:          { return mirac_string_view_from_cstring("ast_block_type_if");          } break;
		case mirac_ast_block_type_elif:        { return mirac_string_view_from_cstring("ast_block_type_elif");        } break;
		case mirac_ast_block_type_else:        { return mirac_string_view_from_cstring("ast_block_type_else");        } break;
		case mirac_ast_block_type_loop:        { return mirac_string_view_from_cstring("ast_block_type_loop");        } break;
		case mirac_ast_block_type_let:         { return mirac_string_view_from_cstring("ast_block_type_let");         } break;
		case mirac_ast_block_type_func:        { return mirac_string_view_from_cstring("ast_block_type_func");        } break;
		case mirac_ast_block_type_mem:         { return mirac_string_view_from_cstring("ast_block_type_mem");         } break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
			return mirac_string_view_from_parts("", 0);
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
	parser.block.type = mirac_ast_block_type_none; // TODO: use a constructor?
	return parser;
}

mirac_ast_unit_s mirac_parser_parse_ast_unit(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_unit_s ast_unit = create_ast_unit(parser->arena);
	ast_unit.identifier = parser->lexer->file_path;
	parse_ast_unit(parser, &ast_unit);
	return ast_unit;
}

void mirac_parser_print_ast_unit(
	const mirac_ast_unit_s* const ast_unit)
{
	mirac_debug_assert(ast_unit != NULL);
	uint8_t indent = 0;

	for (uint64_t index = 0; index < ast_unit->blocks.count; ++index)
	{
		print_ast_block(&(ast_unit->blocks.data[index]), indent);
	}
}

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

static mirac_ast_block_expr_s create_ast_block_expr(
	void)
{
	mirac_ast_block_expr_s expr_block = {0};
	return expr_block;
}

static mirac_ast_block_begin_scope_s create_ast_block_begin_scope(
	void)
{
	mirac_ast_block_begin_scope_s begin_scope_block = {0};
	return begin_scope_block;
}

static mirac_ast_block_end_scope_s create_ast_block_end_scope(
	void)
{
	mirac_ast_block_end_scope_s end_scope_block = {0};
	return end_scope_block;
}

static mirac_ast_block_end_unit_s create_ast_block_end_unit(
	void)
{
	mirac_ast_block_end_unit_s end_unit_block = {0};
	return end_unit_block;
}

static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_if_s if_block = {0};
	if_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	if_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return if_block;
}

static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_elif_s elif_block = {0};
	elif_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	elif_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return elif_block;
}

static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_else_s else_block = {0};
	else_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return else_block;
}

static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_loop_s loop_block = {0};
	loop_block.condition_blocks = mirac_blocks_vector_from_parts(arena, 1);
	loop_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return loop_block;
}

static mirac_ast_block_let_s create_ast_block_let(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_let_s let_block = {0};
	let_block.identifier_blocks = mirac_blocks_vector_from_parts(arena, 1);
	let_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return let_block;
}

static mirac_ast_block_req_s create_ast_block_req(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_req_s req_block = {0};
	req_block.type_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return req_block;
}

static mirac_ast_block_ret_s create_ast_block_ret(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_ret_s ret_block = {0};
	ret_block.type_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return ret_block;
}

static mirac_ast_block_func_s create_ast_block_func(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_func_s func_block = {0};
	func_block.req_block = create_ast_block_req(arena);
	func_block.ret_block = create_ast_block_ret(arena);
	func_block.body_blocks = mirac_blocks_vector_from_parts(arena, 1);
	return func_block;
}

static mirac_ast_block_mem_s create_ast_block_mem(
	void)
{
	mirac_ast_block_mem_s mem_block = {0};
	return mem_block;
}

static mirac_ast_block_s create_ast_block(
	void)
{
	mirac_ast_block_s block = {0};
	return block;
}

static mirac_ast_unit_s create_ast_unit(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_unit_s ast_unit = {0};
	ast_unit.blocks = mirac_blocks_vector_from_parts(arena, 1);
	ast_unit.strings = mirac_tokens_refs_vector_from_parts(arena, 1);
	return ast_unit;
}

static void print_ast_block_expr(
	const mirac_ast_block_expr_s* const expr_block,
	const uint8_t indent)
{
	mirac_debug_assert(expr_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_expr\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&expr_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_begin_scope(
	const mirac_ast_block_begin_scope_s* const begin_scope_block,
	const uint8_t indent)
{
	mirac_debug_assert(begin_scope_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_begin_scope\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&begin_scope_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_end_scope(
	const mirac_ast_block_end_scope_s* const end_scope_block,
	const uint8_t indent)
{
	mirac_debug_assert(end_scope_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_end_scope\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&end_scope_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_end_unit(
	const mirac_ast_block_end_unit_s* const end_unit_block,
	const uint8_t indent)
{
	mirac_debug_assert(end_unit_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_end_unit\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&end_unit_block->token)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_if(
	const mirac_ast_block_if_s* const if_block,
	const uint8_t indent)
{
	mirac_debug_assert(if_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_if\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("condition_blocks:\n");
	for (uint64_t index = 0; index < if_block->condition_blocks.count; ++index)
		print_ast_block(&if_block->condition_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < if_block->body_blocks.count; ++index)
		print_ast_block(&if_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_elif(
	const mirac_ast_block_elif_s* const elif_block,
	const uint8_t indent)
{
	mirac_debug_assert(elif_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_elif\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("condition_blocks:\n");
	for (uint64_t index = 0; index < elif_block->condition_blocks.count; ++index)
		print_ast_block(&elif_block->condition_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < elif_block->body_blocks.count; ++index)
		print_ast_block(&elif_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_else(
	const mirac_ast_block_else_s* const else_block,
	const uint8_t indent)
{
	mirac_debug_assert(else_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_else\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < else_block->body_blocks.count; ++index)
		print_ast_block(&else_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_loop(
	const mirac_ast_block_loop_s* const loop_block,
	const uint8_t indent)
{
	mirac_debug_assert(loop_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_loop\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("condition_blocks:\n");
	for (uint64_t index = 0; index < loop_block->condition_blocks.count; ++index)
		print_ast_block(&loop_block->condition_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < loop_block->body_blocks.count; ++index)
		print_ast_block(&loop_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_let(
	const mirac_ast_block_let_s* const let_block,
	const uint8_t indent)
{
	mirac_debug_assert(let_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_let\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("identifier_blocks:\n");
	for (uint64_t index = 0; index < let_block->identifier_blocks.count; ++index)
		print_ast_block(&let_block->identifier_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < let_block->body_blocks.count; ++index)
		print_ast_block(&let_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_req(
	const mirac_ast_block_req_s* const req_block,
	const uint8_t indent)
{
	mirac_debug_assert(req_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_req\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("type_blocks:\n");
	for (uint64_t index = 0; index < req_block->type_blocks.count; ++index)
		print_ast_block(&req_block->type_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_ret(
	const mirac_ast_block_ret_s* const ret_block,
	const uint8_t indent)
{
	mirac_debug_assert(ret_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_ret\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("type_blocks:\n");
	for (uint64_t index = 0; index < ret_block->type_blocks.count; ++index)
		print_ast_block(&ret_block->type_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_func(
	const mirac_ast_block_func_s* const func_block,
	const uint8_t indent)
{
	mirac_debug_assert(func_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_func\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("identifier:\n");
	for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&func_block->identifier)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("req_block:\n");
	print_ast_block_req(&func_block->req_block, indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("ret_block:\n");
	print_ast_block_ret(&func_block->ret_block, indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("body_blocks:\n");
	for (uint64_t index = 0; index < func_block->body_blocks.count; ++index)
		print_ast_block(&func_block->body_blocks.data[index], indent + 2);

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("is_inlined: %s\n", func_block->is_inlined ? "yes" : "no");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("is_entry: %s\n", func_block->is_entry ? "yes" : "no");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("is_used: %s\n", func_block->is_used ? "yes" : "no");

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block_mem(
	const mirac_ast_block_mem_s* const mem_block,
	const uint8_t indent)
{
	mirac_debug_assert(mem_block != NULL);

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("Block[type=ast_block_mem\n");

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("identifier:\n");
	for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&mem_block->identifier)));

	for (uint8_t index = 0; index < (indent + 1); ++index) printf("\t");
	printf("capacity:\n");
	for (uint8_t index = 0; index < (indent + 2); ++index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&mem_block->capacity)));

	for (uint8_t index = 0; index < indent; ++index) printf("\t");
	printf("]\n");
}

static void print_ast_block(
	const mirac_ast_block_s* const block,
	const uint8_t indent)
{
	mirac_debug_assert(block != NULL);

	switch (block->type)
	{
		case mirac_ast_block_type_expr:
		{
			print_ast_block_expr(&block->as.expr_block, indent);
		} break;

		case mirac_ast_block_type_begin_scope:
		{
			print_ast_block_begin_scope(&block->as.begin_scope_block, indent);
		} break;

		case mirac_ast_block_type_end_scope:
		{
			print_ast_block_end_scope(&block->as.end_scope_block, indent);
		} break;

		case mirac_ast_block_type_end_unit:
		{
			print_ast_block_end_unit(&block->as.end_unit_block, indent);
		} break;

		case mirac_ast_block_type_if:
		{
			print_ast_block_if(&block->as.if_block, indent);
		} break;

		case mirac_ast_block_type_elif:
		{
			print_ast_block_elif(&block->as.elif_block, indent);
		} break;

		case mirac_ast_block_type_else:
		{
			print_ast_block_else(&block->as.else_block, indent);
		} break;

		case mirac_ast_block_type_loop:
		{
			print_ast_block_loop(&block->as.loop_block, indent);
		} break;

		case mirac_ast_block_type_let:
		{
			print_ast_block_let(&block->as.let_block, indent);
		} break;

		case mirac_ast_block_type_req:
		{
			print_ast_block_req(&block->as.req_block, indent);
		} break;

		case mirac_ast_block_type_ret:
		{
			print_ast_block_ret(&block->as.ret_block, indent);
		} break;

		case mirac_ast_block_type_func:
		{
			print_ast_block_func(&block->as.func_block, indent);
		} break;

		case mirac_ast_block_type_mem:
		{
			print_ast_block_mem(&block->as.mem_block, indent);
		} break;

		default:
		{
			mirac_debug_assert(!"Should never reach this block.");
		} break;
	}
}

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_expr_s expr_block = create_ast_block_expr();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (!is_token_expr_block_token(&token))
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'expr' block.",
			mirac_sv_arg(token.text)
		);
	}

	// TODO: figure out if I need separate block for casts (as) since they are
	//       only for the type checker!

	expr_block.token = token;
	return expr_block;
}

static mirac_ast_block_begin_scope_s parse_ast_block_begin_scope(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_begin_scope_s begin_scope_block = create_ast_block_begin_scope();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_lexer_should_stop_lexing(token.type) ||
		(mirac_token_type_reserved_left_brace != token.type))
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'begin-scope' block.",
			mirac_sv_arg(token.text)
		);
	}

	begin_scope_block.token = token;
	return begin_scope_block;
}

static mirac_ast_block_end_scope_s parse_ast_block_end_scope(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_end_scope_s end_scope_block = create_ast_block_end_scope();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_lexer_should_stop_lexing(token.type) ||
		(mirac_token_type_reserved_right_brace != token.type))
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'end-scope' block.",
			mirac_sv_arg(token.text)
		);
	}

	end_scope_block.token = token;
	return end_scope_block;
}

static mirac_ast_block_end_unit_s parse_ast_block_end_unit(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_end_unit_s end_unit_block = create_ast_block_end_unit();

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (token.type != mirac_token_type_eof)
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid token '" mirac_sv_fmt "' while parsing 'end-unit' block.",
			mirac_sv_arg(token.text)
		);
	}

	end_unit_block.token = token;
	return end_unit_block;
}

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_if_s if_block = create_ast_block_if(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_if == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'if' block's condition."
				);
			} break;

			case mirac_ast_block_type_begin_scope:
			{
				goto after_if_condition_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&if_block.condition_blocks, block);
			} break;
		}
	}

after_if_condition_blocks:
	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'if' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_if_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&if_block.body_blocks, block);
			} break;
		}
	}

after_if_body_blocks:
	return if_block;
}

static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_elif_s elif_block = create_ast_block_elif(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_elif == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'elif' block's condition."
				);
			} break;

			case mirac_ast_block_type_begin_scope:
			{
				goto after_elif_condition_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&elif_block.condition_blocks, block);
			} break;
		}
	}

after_elif_condition_blocks:
	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'elif' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_elif_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&elif_block.body_blocks, block);
			} break;
		}
	}

after_elif_body_blocks:
	return elif_block;
}

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_else_s else_block = create_ast_block_else(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_else == token.type);

	if (mirac_lexer_lex_next(parser->lexer, &token) != mirac_token_type_reserved_left_brace)
	{
		log_parser_error_and_exit(token.location,
			"expected '{' token after 'else' token, but encountered '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'else' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_else_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&else_block.body_blocks, block);
			} break;
		}
	}

after_else_body_blocks:
	return else_block;
}

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_loop_s loop_block = create_ast_block_loop(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_loop == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'loop' block's condition."
				);
			} break;

			case mirac_ast_block_type_begin_scope:
			{
				goto after_loop_condition_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&loop_block.condition_blocks, block);
			} break;
		}
	}

after_loop_condition_blocks:
	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'loop' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_loop_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&loop_block.body_blocks, block);
			} break;
		}
	}

after_loop_body_blocks:
	return loop_block;
}

static mirac_ast_block_let_s parse_ast_block_let(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_let_s let_block = create_ast_block_let(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_let == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'let' block's condition."
				);
			} break;

			case mirac_ast_block_type_begin_scope:
			{
				goto after_let_identifier_blocks;
			} break;

			default:
			{
				if ((mirac_ast_block_type_expr == block.type) &&
					(mirac_token_type_identifier == block.as.expr_block.token.type))
				{
					mirac_blocks_vector_push(&let_block.identifier_blocks, block);
				}
				else
				{
					log_parser_error_and_exit(token.location,
						"encountered an invalid token '" mirac_sv_fmt "' while parsing 'let' block's identifiers.",
						mirac_sv_arg(block.as.expr_block.token.text)
					);
				}
			} break;
		}
	}

after_let_identifier_blocks:
	if (let_block.identifier_blocks.count <= 0)
	{
		log_parser_error_and_exit(token.location,
			"no identifiers were provided in the 'let' block's identifiers section."
		);
	}

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_end_unit:
			{
				log_parser_error_and_exit(token.location,
					"encountered an unexpected end of unit while parsing 'let' block's body."
				);
			} break;

			case mirac_ast_block_type_end_scope:
			{
				goto after_let_body_blocks;
			} break;

			default:
			{
				mirac_blocks_vector_push(&let_block.body_blocks, block);
			} break;
		}
	}

after_let_body_blocks:
	return let_block;
}

static mirac_ast_block_req_s parse_ast_block_req(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_req_s req_block = create_ast_block_req(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_req == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_expr:
			{
				if (mirac_token_is_type_token(&block.as.expr_block.token))
				{
					mirac_blocks_vector_push(&req_block.type_blocks, block);
				}
				else
				{
					unparse_ast_block(parser, &block);
					goto after_req_type_blocks;
				}
			} break;

			default:
			{
				unparse_ast_block(parser, &block);
				goto after_req_type_blocks;
			} break;
		}
	}

after_req_type_blocks:
	if (req_block.type_blocks.count <= 0)
	{
		log_parser_error_and_exit(token.location,
			"no type specifiers were provided in the 'req' block's types section."
		);
	}

	return req_block;
}

static mirac_ast_block_ret_s parse_ast_block_ret(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_ast_block_ret_s ret_block = create_ast_block_ret(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_ret == token.type);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);

		switch (block.type)
		{
			case mirac_ast_block_type_expr:
			{
				if (mirac_token_is_type_token(&block.as.expr_block.token))
				{
					mirac_blocks_vector_push(&ret_block.type_blocks, block);
				}
				else
				{
					unparse_ast_block(parser, &block);
					goto after_ret_type_blocks;
				}
			} break;

			default:
			{
				unparse_ast_block(parser, &block);
				goto after_ret_type_blocks;
			} break;
		}
	}

after_ret_type_blocks:
	if (ret_block.type_blocks.count <= 0)
	{
		log_parser_error_and_exit(token.location,
			"no type specifiers were provided in the 'req' block's types section."
		);
	}

	return ret_block;
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
				mirac_blocks_vector_push(&func_block.body_blocks, block);
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

static mirac_ast_block_s parse_ast_block(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

	if (parser->block.type != mirac_ast_block_type_none)
	{
		mirac_ast_block_s block = parser->block;
		parser->block.type = mirac_ast_block_type_none;
		return block;
	}

	mirac_ast_block_s block = create_ast_block();
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (!mirac_lexer_should_stop_lexing(token.type))
	{
		mirac_lexer_unlex(parser->lexer, &token);
	}

	switch (token.type)
	{
		case mirac_token_type_reserved_left_brace:
		{
			block.type = mirac_ast_block_type_begin_scope;
			block.as.begin_scope_block = parse_ast_block_begin_scope(parser);
		} break;

		case mirac_token_type_reserved_right_brace:
		{
			block.type = mirac_ast_block_type_end_scope;
			block.as.end_scope_block = parse_ast_block_end_scope(parser);
		} break;

		case mirac_token_type_eof:
		{
			block.type = mirac_ast_block_type_end_unit;
			block.as.end_unit_block = parse_ast_block_end_unit(parser);
		} break;

		case mirac_token_type_reserved_if:
		{
			block.type = mirac_ast_block_type_if;
			block.as.if_block = parse_ast_block_if(parser);
		} break;

		case mirac_token_type_reserved_elif:
		{
			block.type = mirac_ast_block_type_elif;
			block.as.elif_block = parse_ast_block_elif(parser);
		} break;

		case mirac_token_type_reserved_else:
		{
			block.type = mirac_ast_block_type_else;
			block.as.else_block = parse_ast_block_else(parser);
		} break;

		case mirac_token_type_reserved_loop:
		{
			block.type = mirac_ast_block_type_loop;
			block.as.loop_block = parse_ast_block_loop(parser);
		} break;

		case mirac_token_type_reserved_let:
		{
			block.type = mirac_ast_block_type_let;
			block.as.let_block = parse_ast_block_let(parser);
		} break;

		case mirac_token_type_reserved_req:
		{
			block.type = mirac_ast_block_type_let;
			block.as.req_block = parse_ast_block_req(parser);
		} break;

		case mirac_token_type_reserved_ret:
		{
			block.type = mirac_ast_block_type_ret;
			block.as.ret_block = parse_ast_block_ret(parser);
		} break;

		case mirac_token_type_reserved_inl:
		case mirac_token_type_reserved_func:
		{
			block.type = mirac_ast_block_type_func;
			block.as.func_block = parse_ast_block_func(parser);
		} break;

		case mirac_token_type_reserved_mem:
		{
			block.type = mirac_ast_block_type_mem;
			block.as.mem_block = parse_ast_block_mem(parser);
		} break;

		default:
		{
			block.type = mirac_ast_block_type_expr;
			block.as.expr_block = parse_ast_block_expr(parser);
		} break;
	}

	return block;
}

static void unparse_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(block != NULL);
	parser->block = *block;
}

static void parse_ast_unit(
	mirac_parser_s* const parser,
	mirac_ast_unit_s* const ast_unit)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(ast_unit != NULL);

	while (1)
	{
		mirac_ast_block_s block = parse_ast_block(parser);
		if (mirac_ast_block_type_end_unit == block.type) { break; }
		mirac_blocks_vector_push(&ast_unit->blocks, block);
	}
}
