
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

mirac_implement_linked_list_type(mirac_token_list, mirac_token_s);
mirac_implement_linked_list_type(mirac_ast_block_list, mirac_ast_block_s*);
mirac_implement_linked_list_type(mirac_ast_def_list, mirac_ast_def_s*);

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

// TODO: Document!
static mirac_ast_block_expr_s create_ast_block_expr(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_block_ident_s create_ast_block_ident(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_block_call_s create_ast_block_call(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_block_as_s create_ast_block_as(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_block_scope_s create_ast_block_scope(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_block_s* create_ast_block(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_def_func_s create_ast_def_func(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_def_mem_s create_ast_def_mem(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_def_str_s create_ast_def_str(
	mirac_arena_s* const arena);

// TODO: Document!
static mirac_ast_def_s* create_ast_def(
	mirac_arena_s* const arena);

// TODO: Document!
static bool is_token_valid_expr_block_token_by_type(
	const mirac_token_type_e type);

// TODO: Document!
static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_block_ident_s parse_ast_block_ident(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_block_call_s parse_ast_block_call(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_block_as_s parse_ast_block_as(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_block_scope_s parse_ast_block_scope(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_block_s* parse_ast_block(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_def_func_s parse_ast_def_func(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_def_mem_s parse_ast_def_mem(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_def_str_s parse_ast_def_str(
	mirac_parser_s* const parser);

// TODO: Document!
static mirac_ast_def_s* parse_ast_def(
	mirac_parser_s* const parser);

// TODO: Document!
static void print_ast_block_expr(
	const mirac_ast_block_expr_s* const expr_block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_block_ident(
	const mirac_ast_block_ident_s* const ident_block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_block_call(
	const mirac_ast_block_call_s* const call_block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_block_as(
	const mirac_ast_block_as_s* const as_block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_block_scope(
	const mirac_ast_block_scope_s* const scope_block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_block_if(
	const mirac_ast_block_if_s* const if_block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_block_else(
	const mirac_ast_block_else_s* const else_block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_block_loop(
	const mirac_ast_block_loop_s* const loop_block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_block(
	const mirac_ast_block_s* const block,
	const uint64_t indent);

// TODO: Document!
static void print_ast_def_func(
	const mirac_ast_def_func_s* const func_def,
	const uint64_t indent);

// TODO: Document!
static void print_ast_def_mem(
	const mirac_ast_def_mem_s* const mem_def,
	const uint64_t indent);

// TODO: Document!
static void print_ast_def_str(
	const mirac_ast_def_str_s* const str_def,
	const uint64_t indent);

// TODO: Document!
static void print_ast_def(
	const mirac_ast_def_s* const def,
	const uint64_t indent);

mirac_string_view_s mirac_ast_block_scope_type_to_string_view(
	const mirac_ast_block_scope_type_e type)
{
	switch (type)
	{
		case mirac_ast_block_scope_type_parentheses: { return mirac_string_view_from_cstring("parentheses"); } break;
		case mirac_ast_block_scope_type_brackets:    { return mirac_string_view_from_cstring("brackets");    } break;
		case mirac_ast_block_scope_type_braces:      { return mirac_string_view_from_cstring("braces");      } break;

		default:
		{
			mirac_logger_debug("encountered an unknown 'mirac_ast_block_scope_type_e' type with raw value of '%u'.", type);
			mirac_debug_assert(0); // NOTE: Should never reach this block.
			return mirac_string_view_from_parts("", 0);
		} break;
	}
}

mirac_string_view_s mirac_ast_block_type_to_string_view(
	const mirac_ast_block_type_e type)
{
	switch (type)
	{
		case mirac_ast_block_type_expr:  { return mirac_string_view_from_cstring("expr");  } break;
		case mirac_ast_block_type_ident: { return mirac_string_view_from_cstring("ident"); } break;
		case mirac_ast_block_type_call:  { return mirac_string_view_from_cstring("call");  } break;
		case mirac_ast_block_type_as:    { return mirac_string_view_from_cstring("as");    } break;
		case mirac_ast_block_type_scope: { return mirac_string_view_from_cstring("scope"); } break;
		case mirac_ast_block_type_if:    { return mirac_string_view_from_cstring("if");    } break;
		case mirac_ast_block_type_else:  { return mirac_string_view_from_cstring("else");  } break;
		case mirac_ast_block_type_loop:  { return mirac_string_view_from_cstring("loop");  } break;
		case mirac_ast_block_type_eou:   { return mirac_string_view_from_cstring("eou");   } break;

		default:
		{
			mirac_logger_debug("encountered an unknown 'mirac_ast_block_type_e' type with raw value of '%u'.", type);
			mirac_debug_assert(0); // NOTE: Should never reach this block.
			return mirac_string_view_from_parts("", 0);
		} break;
	}
}

mirac_string_view_s mirac_ast_def_type_to_string_view(
	const mirac_ast_def_type_e type)
{
	switch (type)
	{
		case mirac_ast_def_type_func: { return mirac_string_view_from_cstring("func"); } break;
		case mirac_ast_def_type_mem:  { return mirac_string_view_from_cstring("mem");  } break;
		case mirac_ast_def_type_str:  { return mirac_string_view_from_cstring("str");  } break;

		default:
		{
			mirac_logger_debug("encountered an unknown 'mirac_ast_def_type_e' type with raw value of '%u'.", type);
			mirac_debug_assert(0); // NOTE: Should never reach this block.
			return mirac_string_view_from_parts("", 0);
		} break;
	}
}

mirac_token_s mirac_ast_def_get_identifier_token(
	const mirac_ast_def_s* const def)
{
	mirac_debug_assert(def != NULL);

	switch (def->type)
	{
		case mirac_ast_def_type_func: { return def->as.func_def.identifier; } break;
		case mirac_ast_def_type_mem:  { return def->as.mem_def.identifier;  } break;
		case mirac_ast_def_type_str:  { return def->as.str_def.identifier;  } break;

		default:
		{
			mirac_logger_debug("encountered an unknown 'mirac_ast_def_type_e' type with raw value of '%u'.", def->type);
			mirac_debug_assert(0); // NOTE: Should never reach this block.
			return mirac_token_from_type(mirac_token_type_none);
		} break;
	}
}

mirac_ast_unit_s mirac_ast_unit_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_unit_s unit = {0};
	unit.defs = mirac_ast_def_list_from_parts(arena);
	return unit;
}

void mirac_ast_unit_print(
	const mirac_ast_unit_s* const unit,
	const uint64_t indent)
{
	mirac_debug_assert(unit != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("Unit[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("defs:\n");
	for (const mirac_ast_def_list_node_s* defs_iterator = unit->defs.begin; defs_iterator != NULL; defs_iterator = defs_iterator->next)
	{
		print_ast_def(defs_iterator->data, indent + 2);
	}

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
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
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_def_s* def = NULL;
	while ((def = parse_ast_def(parser)) != NULL)
	{
		if (mirac_ast_def_type_none == def->type)
		{
			break;
		}

		const mirac_token_s current_def_identifier_token = mirac_ast_def_get_identifier_token(def);
		for (const mirac_ast_def_list_node_s* defs_iterator = parser->unit.defs.begin; defs_iterator != NULL; defs_iterator = defs_iterator->next)
		{
			mirac_debug_assert(defs_iterator != NULL);
			mirac_debug_assert(defs_iterator->data != NULL);

			const mirac_token_s existing_def_identifier_token = mirac_ast_def_get_identifier_token(defs_iterator->data);
			if (mirac_string_view_equal(current_def_identifier_token.as.ident, existing_def_identifier_token.as.ident))
			{
				log_parser_error_and_exit(current_def_identifier_token.location,
					"encountered a redefinition of '" mirac_sv_fmt "' identifier.",
					mirac_sv_arg(current_def_identifier_token.as.ident)
				);
			}
		}

		mirac_ast_def_list_push(&parser->unit.defs, def);
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

static mirac_ast_block_ident_s create_ast_block_ident(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_ident_s ident_block = {0};
	return ident_block;
}

static mirac_ast_block_call_s create_ast_block_call(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_call_s call_block = {0};
	return call_block;
}

static mirac_ast_block_as_s create_ast_block_as(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_as_s as_block = {0};
	as_block.type_tokens = mirac_token_list_from_parts(arena);
	return as_block;
}

static mirac_ast_block_scope_s create_ast_block_scope(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_scope_s scope_block = {0};
	scope_block.blocks = mirac_ast_block_list_from_parts(arena);
	return scope_block;
}

static mirac_ast_block_if_s create_ast_block_if(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_if_s if_block = {0};
	return if_block;
}

static mirac_ast_block_else_s create_ast_block_else(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_else_s else_block = {0};
	return else_block;
}

static mirac_ast_block_loop_s create_ast_block_loop(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_loop_s loop_block = {0};
	return loop_block;
}

static mirac_ast_block_s* create_ast_block(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_s* const block = mirac_arena_malloc(arena, sizeof(mirac_ast_block_s));
	*block = (mirac_ast_block_s) {0};
	return block;
}

static mirac_ast_def_func_s create_ast_def_func(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_def_func_s func_def = {0};
	func_def.req_tokens = mirac_token_list_from_parts(arena);
	func_def.ret_tokens = mirac_token_list_from_parts(arena);
	return func_def;
}

static mirac_ast_def_mem_s create_ast_def_mem(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_def_mem_s mem_def = {0};
	return mem_def;
}

static mirac_ast_def_str_s create_ast_def_str(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_def_str_s str_def = {0};
	return str_def;
}

static mirac_ast_def_s* create_ast_def(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_def_s* const def = mirac_arena_malloc(arena, sizeof(mirac_ast_def_s));
	*def = (mirac_ast_def_s) {0};
	return def;
}

static bool is_token_valid_expr_block_token_by_type(
	const mirac_token_type_e type)
{
	return (
		(mirac_token_type_reserved_i08               != type ) &&
		(mirac_token_type_reserved_i16               != type ) &&
		(mirac_token_type_reserved_i32               != type ) &&
		(mirac_token_type_reserved_i64               != type ) &&
		(mirac_token_type_reserved_u08               != type ) &&
		(mirac_token_type_reserved_u16               != type ) &&
		(mirac_token_type_reserved_u32               != type ) &&
		(mirac_token_type_reserved_u64               != type ) &&
		(mirac_token_type_reserved_ptr               != type ) &&
		(mirac_token_type_reserved_sec               != type ) &&
		(mirac_token_type_reserved_str               != type ) &&
		(mirac_token_type_reserved_mem               != type ) &&
		(mirac_token_type_reserved_func              != type ) &&
		(mirac_token_type_reserved_if                != type ) &&
		(mirac_token_type_reserved_else              != type ) &&
		(mirac_token_type_reserved_loop              != type ) &&
		(mirac_token_type_reserved_req               != type ) &&
		(mirac_token_type_reserved_ret               != type ) &&
		(mirac_token_type_reserved_call              != type ) &&
		(mirac_token_type_reserved_as                != type ) &&
		(mirac_token_type_reserved_left_parenthesis  != type) &&
		(mirac_token_type_reserved_right_parenthesis != type) &&
		(mirac_token_type_reserved_left_bracket      != type) &&
		(mirac_token_type_reserved_right_bracket     != type) &&
		(mirac_token_type_reserved_left_brace        != type) &&
		(mirac_token_type_reserved_right_brace       != type) &&
		(mirac_token_type_literal_str                != type) &&
		(mirac_token_type_identifier                 != type)
	);
}

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_block_expr_s expr_block = create_ast_block_expr(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (!is_token_valid_expr_block_token_by_type(token.type))
	{
		log_parser_error_and_exit(token.location,
			"encountered an invalid 'expr' block token '" mirac_sv_fmt "'.",
			mirac_sv_arg(token.text)
		);
	}

	expr_block.token = token;
	return expr_block;
}

static mirac_ast_block_ident_s parse_ast_block_ident(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_block_ident_s ident_block = create_ast_block_ident(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_identifier == token.type);

	for (const mirac_ast_def_list_node_s* defs_iterator = parser->unit.defs.begin; defs_iterator != NULL; defs_iterator = defs_iterator->next)
	{
		mirac_debug_assert(defs_iterator != NULL);
		mirac_debug_assert(defs_iterator->data != NULL);

		const mirac_token_s existing_def_identifier_token = mirac_ast_def_get_identifier_token(defs_iterator->data);
		if (mirac_string_view_equal(token.as.ident, existing_def_identifier_token.as.ident))
		{
			ident_block.def = defs_iterator->data;
			ident_block.def->is_used = true;
			goto found_matching_idendtifier;
		}
	}

	log_parser_error_and_exit(token.location,
		"encountered an undefined identifier '" mirac_sv_fmt "' token.",
		mirac_sv_arg(token.as.ident)
	);

found_matching_idendtifier:
	ident_block.token = token;
	return ident_block;
}

static mirac_ast_block_call_s parse_ast_block_call(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_block_call_s call_block = create_ast_block_call(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_call == token.type);

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_ident)
	{
		mirac_debug_assert(block != NULL);
		log_parser_error_and_exit(block->location,
			"expected 'ident' block after 'call' block, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	const mirac_ast_block_ident_s* const ident_block = &block->as.ident_block;
	mirac_debug_assert(ident_block != NULL);

	if (ident_block->def->type != mirac_ast_def_type_func)
	{
		log_parser_error_and_exit(block->location,
			"expected 'func' def identifier after 'call' block, but found '" mirac_sv_fmt "' def identifier.",
			mirac_sv_arg(mirac_ast_def_type_to_string_view(ident_block->def->type))
		);
	}

	call_block.ident = block;
	return call_block;
}

static mirac_ast_block_as_s parse_ast_block_as(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

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

		mirac_token_list_push(&as_block.type_tokens, token);
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
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_block_scope_s scope_block = create_ast_block_scope(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert((mirac_token_type_reserved_left_parenthesis == token.type) ||
					   (mirac_token_type_reserved_left_bracket     == token.type) ||
					   (mirac_token_type_reserved_left_brace       == token.type));

	mirac_token_type_e scope_end_token_type = mirac_token_type_none;
	switch (token.type)
	{
		case mirac_token_type_reserved_left_parenthesis:
		{
			scope_block.type = mirac_ast_block_scope_type_parentheses;
			scope_end_token_type = mirac_token_type_reserved_right_parenthesis;
		} break;

		case mirac_token_type_reserved_left_bracket:
		{
			scope_block.type = mirac_ast_block_scope_type_brackets;
			scope_end_token_type = mirac_token_type_reserved_right_bracket;
		} break;

		case mirac_token_type_reserved_left_brace:
		{
			scope_block.type = mirac_ast_block_scope_type_braces;
			scope_end_token_type = mirac_token_type_reserved_right_brace;
		} break;

		default:
		{
			mirac_logger_debug("encountered an unknown 'mirac_token_type_e' type with raw value of '%u'.", token.type);
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}

	mirac_debug_assert(scope_end_token_type != mirac_token_type_none);

	while (1)
	{
		(void)mirac_lexer_lex_next(parser->lexer, &token);
		if (scope_end_token_type == token.type) { break; }
		mirac_lexer_unlex(parser->lexer, &token);

		block = parse_ast_block(parser);
		mirac_debug_assert(block != NULL);

		if ((mirac_ast_block_type_eou  == block->type) ||
			(mirac_ast_block_type_none == block->type))
		{
			log_parser_error_and_exit(block->location,
				"expected scope closing '" mirac_sv_fmt "' token, but found '" mirac_sv_fmt "' block.",
				mirac_sv_arg(mirac_token_type_to_string_view(scope_end_token_type)),
				mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
			);
		}

		mirac_ast_block_list_push(&scope_block.blocks, block);
	}

	mirac_ast_block_s* block_ref = NULL;
	mirac_ast_block_s* prev_block_ref = NULL;

	for (mirac_ast_block_list_node_s* blocks_iterator = scope_block.blocks.begin; blocks_iterator != NULL; blocks_iterator = blocks_iterator->next)
	{
		mirac_debug_assert(blocks_iterator != NULL);
		mirac_debug_assert(blocks_iterator->data != NULL);

		prev_block_ref = block_ref;
		block_ref = blocks_iterator->data;
		mirac_debug_assert(block_ref != NULL);

		if (mirac_ast_block_type_else == block_ref->type)
		{
			if (NULL == prev_block_ref)
			{
				log_parser_error_and_exit(block_ref->location,
					"missing 'if' block prior to '" mirac_sv_fmt "' block.",
					mirac_sv_arg(mirac_ast_block_type_to_string_view(block_ref->type))
				);
			}
			else
			{
				if (prev_block_ref->type != mirac_ast_block_type_if)
				{
					log_parser_error_and_exit(block_ref->location,
						"expected 'if' block prior to '" mirac_sv_fmt "' block, but found '" mirac_sv_fmt"' block.",
						mirac_sv_arg(mirac_ast_block_type_to_string_view(block_ref->type)),
						mirac_sv_arg(mirac_ast_block_type_to_string_view(prev_block_ref->type))
					);
				}
			}
		}

		if ((prev_block_ref != NULL) && (block_ref != NULL))
		{
			if ((mirac_ast_block_type_if == prev_block_ref->type) &&
				(mirac_ast_block_type_else == block_ref->type))
			{
				prev_block_ref->as.if_block.next = block_ref;
			}
		}
	}

	return scope_block;
}

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_block_if_s if_block = create_ast_block_if(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_if == token.type);

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		mirac_debug_assert(block != NULL);
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'if' block's condition, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	if_block.cond = block;

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		mirac_debug_assert(block != NULL);
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'if' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	if_block.body = block;

	if_block.index = parser->if_count++;
	return if_block;
}

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_block_else_s else_block = create_ast_block_else(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_else == token.type);

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		mirac_debug_assert(block != NULL);
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'else' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	else_block.body = block;

	else_block.index = parser->else_count++;
	return else_block;
}

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_block_loop_s loop_block = create_ast_block_loop(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_loop == token.type);

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		mirac_debug_assert(block != NULL);
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'loop' block's condition, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	loop_block.cond = block;

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		mirac_debug_assert(block != NULL);
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'loop' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	loop_block.body = block;

	loop_block.index = parser->loop_count++;
	return loop_block;
}

static mirac_ast_block_s* parse_ast_block(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_block_s* const block = create_ast_block(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	block->location = token.location;

	if (!mirac_lexer_should_stop_lexing(token.type))
	{
		mirac_lexer_unlex(parser->lexer, &token);
	}

	switch (token.type)
	{
		case mirac_token_type_identifier:
		{
			block->type = mirac_ast_block_type_ident;
			block->as.ident_block = parse_ast_block_ident(parser);
		} break;

		case mirac_token_type_reserved_call:
		{
			block->type = mirac_ast_block_type_call;
			block->as.call_block = parse_ast_block_call(parser);
		} break;

		case mirac_token_type_reserved_as:
		{
			block->type = mirac_ast_block_type_as;
			block->as.as_block = parse_ast_block_as(parser);
		} break;

		case mirac_token_type_reserved_left_parenthesis:
		case mirac_token_type_reserved_left_bracket:
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

	return block;
}

static mirac_ast_def_func_s parse_ast_def_func(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_def_func_s func_def = create_ast_def_func(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	if (mirac_lexer_lex_next(parser->lexer, &token) != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'func' token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	func_def.identifier = token;
	func_def.is_entry = mirac_string_view_equal(func_def.identifier.as.ident, parser->config->entry);

	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (mirac_token_type_reserved_req == token.type)
	{
		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
		{
			if (!mirac_token_is_type_token(&token))
			{
				break;
			}

			mirac_token_list_push(&func_def.req_tokens, token);
		}

		if (func_def.req_tokens.count <= 0)
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

			mirac_token_list_push(&func_def.ret_tokens, token);
		}

		if (func_def.ret_tokens.count <= 0)
		{
			log_parser_error_and_exit(token.location,
				"no type specifiers were provided after 'ret' token."
			);
		}
	}

	mirac_lexer_unlex(parser->lexer, &token);
	mirac_ast_block_s* block = NULL;

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		mirac_debug_assert(block != NULL);
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'func' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	func_def.body = block;
	func_def.index = parser->func_count++;
	return func_def;
}

static mirac_ast_def_mem_s parse_ast_def_mem(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_def_mem_s mem_def = create_ast_def_mem(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	if (mirac_lexer_lex_next(parser->lexer, &token) != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'mem' token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	mem_def.identifier = token;
	(void)mirac_lexer_lex_next(parser->lexer, &token);

	if (!mirac_token_is_unsigned_numeric_literal(&token))
	{
		log_parser_error_and_exit(token.location,
			"expected capacity token after 'mem' identifier token to be unsigned integer literal token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	if (token.as.uval <= 0)
	{
		log_parser_error_and_exit(token.location,
			"provided capacity token '" mirac_sv_fmt "' must be a positive integer value.",
			mirac_sv_arg(token.text)
		);
	}

	mem_def.capacity = token;
	mem_def.index = parser->mem_count++;
	return mem_def;
}

static mirac_ast_def_str_s parse_ast_def_str(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_def_str_s str_def = create_ast_def_str(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	if (mirac_lexer_lex_next(parser->lexer, &token) != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'str' token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	str_def.identifier = token;

	if (mirac_lexer_lex_next(parser->lexer, &token) != mirac_token_type_literal_str)
	{
		log_parser_error_and_exit(token.location,
			"expected 'str literal' token after 'str' identifier token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	str_def.literal = token;
	str_def.index = parser->str_count++;
	return str_def;
}

static mirac_ast_def_s* parse_ast_def(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->config != NULL);
	mirac_debug_assert(parser->arena != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_ast_def_s* const def = create_ast_def(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	if (mirac_lexer_should_stop_lexing(token.type))
	{ goto parse_def_by_token; }

	if (token.type != mirac_token_type_reserved_sec)
	{
		log_parser_error_and_exit(token.location,
			"expected 'sec' token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	if (mirac_lexer_should_stop_lexing(token.type))
	{ goto parse_def_by_token; }

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected 'str literal' token after 'sec' token, but found '" mirac_sv_fmt "' token.",
			mirac_sv_arg(token.text)
		);
	}

	def->section = token;
	def->is_used = false;

	(void)mirac_lexer_lex_next(parser->lexer, &token);

parse_def_by_token:
	switch (token.type)
	{
		case mirac_token_type_reserved_func:
		{
			def->type = mirac_ast_def_type_func;
			def->as.func_def = parse_ast_def_func(parser);
			// NOTE: If the func is an entry, it is marked as used to prevent error in the cross referencing:
			if (def->as.func_def.is_entry) { def->is_used = true; }
		} break;

		case mirac_token_type_reserved_mem:
		{
			def->type = mirac_ast_def_type_mem;
			def->as.mem_def = parse_ast_def_mem(parser);
		} break;

		case mirac_token_type_reserved_str:
		{
			def->type = mirac_ast_def_type_str;
			def->as.str_def = parse_ast_def_str(parser);
		} break;

		case mirac_token_type_eof:
		case mirac_token_type_none:
		{
			def->type = mirac_ast_def_type_none;
		} break;

		default:
		{
			log_parser_error_and_exit(token.location,
				"expected 'func', 'mem', or 'str' token after 'sec' definition, but found '" mirac_sv_fmt "' token",
				mirac_sv_arg(token.text)
			);
		} break;
	}

	return def;
}

static void print_ast_block_expr(
	const mirac_ast_block_expr_s* const expr_block,
	const uint64_t indent)
{
	mirac_debug_assert(expr_block != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("ExprBlock[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("token:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&expr_block->token)));

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_block_ident(
	const mirac_ast_block_ident_s* const ident_block,
	const uint64_t indent)
{
	mirac_debug_assert(ident_block != NULL);
	mirac_debug_assert(ident_block->def != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("IdentBlock[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("token:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&ident_block->token)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("def:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_ast_def_get_identifier_token(ident_block->def).text));

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_block_call(
	const mirac_ast_block_call_s* const call_block,
	const uint64_t indent)
{
	mirac_debug_assert(call_block != NULL);
	mirac_debug_assert(call_block->ident != NULL);
	mirac_debug_assert(mirac_ast_block_type_ident == call_block->ident->type);

	const mirac_ast_block_ident_s* const ident_block = &call_block->ident->as.ident_block;
	mirac_debug_assert(ident_block != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("CallBlock[\n");

	print_ast_block_ident(ident_block, indent + 1);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_block_as(
	const mirac_ast_block_as_s* const as_block,
	const uint64_t indent)
{
	mirac_debug_assert(as_block != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("AsBlock[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("types:\n");
	for (mirac_token_list_node_s* types_iterator = as_block->type_tokens.begin; types_iterator != NULL; types_iterator = types_iterator->next)
	{
		for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&types_iterator->data)));
	}

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_block_scope(
	const mirac_ast_block_scope_s* const scope_block,
	const uint64_t indent)
{
	mirac_debug_assert(scope_block != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("ScopeBlock[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("type:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_ast_block_scope_type_to_string_view(scope_block->type)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("blocks:\n");
	for (mirac_ast_block_list_node_s* blocks_iterator = scope_block->blocks.begin; blocks_iterator != NULL; blocks_iterator = blocks_iterator->next)
	{
		mirac_debug_assert(blocks_iterator != NULL);
		mirac_debug_assert(blocks_iterator->data != NULL);
		print_ast_block(blocks_iterator->data, indent + 2);
	}

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_block_if(
	const mirac_ast_block_if_s* const if_block,
	const uint64_t indent)
{
	mirac_debug_assert(if_block != NULL);
	mirac_debug_assert(if_block->cond != NULL);
	mirac_debug_assert(if_block->body != NULL);
	mirac_debug_assert(mirac_ast_block_type_scope == if_block->cond->type);
	mirac_debug_assert(mirac_ast_block_type_scope == if_block->body->type);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("IfBlock[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("cond:\n");
	print_ast_block(if_block->cond, indent + 2);

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("body:\n");
	print_ast_block(if_block->body, indent + 2);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_block_else(
	const mirac_ast_block_else_s* const else_block,
	const uint64_t indent)
{
	mirac_debug_assert(else_block != NULL);
	mirac_debug_assert(else_block->body != NULL);
	mirac_debug_assert(mirac_ast_block_type_scope == else_block->body->type);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("ElseBlock[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("body:\n");
	print_ast_block(else_block->body, indent + 2);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_block_loop(
	const mirac_ast_block_loop_s* const loop_block,
	const uint64_t indent)
{
	mirac_debug_assert(loop_block != NULL);
	mirac_debug_assert(loop_block->cond != NULL);
	mirac_debug_assert(loop_block->body != NULL);
	mirac_debug_assert(mirac_ast_block_type_scope == loop_block->cond->type);
	mirac_debug_assert(mirac_ast_block_type_scope == loop_block->body->type);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("LoopBlock[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("cond:\n");
	print_ast_block(loop_block->cond, indent + 2);

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("body:\n");
	print_ast_block(loop_block->body, indent + 2);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_block(
	const mirac_ast_block_s* const block,
	const uint64_t indent)
{
	mirac_debug_assert(block != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("Block[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("type: '" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("location: '" mirac_location_fmt "'\n", mirac_location_arg(block->location));

	switch (block->type)
	{
		case mirac_ast_block_type_expr:  { print_ast_block_expr(&block->as.expr_block, indent + 1);   } break;
		case mirac_ast_block_type_ident: { print_ast_block_ident(&block->as.ident_block, indent + 1); } break;
		case mirac_ast_block_type_call:  { print_ast_block_call(&block->as.call_block, indent + 1);   } break;
		case mirac_ast_block_type_as:    { print_ast_block_as(&block->as.as_block, indent + 1);       } break;
		case mirac_ast_block_type_scope: { print_ast_block_scope(&block->as.scope_block, indent + 1); } break;
		case mirac_ast_block_type_if:    { print_ast_block_if(&block->as.if_block, indent + 1);       } break;
		case mirac_ast_block_type_else:  { print_ast_block_else(&block->as.else_block, indent + 1);   } break;
		case mirac_ast_block_type_loop:  { print_ast_block_loop(&block->as.loop_block, indent + 1);   } break;

		default:
		{
			mirac_logger_debug("encountered an unknown 'mirac_ast_block_type_e' type with raw value of '%u'.", block->type);
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_def_func(
	const mirac_ast_def_func_s* const func_def,
	const uint64_t indent)
{
	mirac_debug_assert(func_def != NULL);
	mirac_debug_assert(func_def->body != NULL);
	mirac_debug_assert(mirac_ast_block_type_scope == func_def->body->type);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("FuncDef[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("identifier:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&func_def->identifier)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("req_tokens:\n");
	for (const mirac_token_list_node_s* reqs_iterator = func_def->req_tokens.begin; reqs_iterator != NULL; reqs_iterator = reqs_iterator->next)
	{
		for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&reqs_iterator->data)));
	}
	
	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("ret_tokens:\n");
	for (const mirac_token_list_node_s* rets_iterator = func_def->ret_tokens.begin; rets_iterator != NULL; rets_iterator = rets_iterator->next)
	{
		for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
		printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&rets_iterator->data)));
	}

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("body:\n");
	print_ast_block(func_def->body, indent + 2);

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("is_entry: %s\n", func_def->is_entry ? "yes" : "no");

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_def_mem(
	const mirac_ast_def_mem_s* const mem_def,
	const uint64_t indent)
{
	mirac_debug_assert(mem_def != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("MemDef[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("identifier:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&mem_def->identifier)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("capacity:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&mem_def->capacity)));

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_def_str(
	const mirac_ast_def_str_s* const str_def,
	const uint64_t indent)
{
	mirac_debug_assert(str_def != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("StrDef[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("identifier:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&str_def->identifier)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("literal:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&str_def->literal)));

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}

static void print_ast_def(
	const mirac_ast_def_s* const def,
	const uint64_t indent)
{
	mirac_debug_assert(def != NULL);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("Def[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("section:\n");
	for (uint8_t indent_index = 0; indent_index < (indent + 2); ++indent_index) printf("\t");
	printf(mirac_sv_fmt "\n", mirac_sv_arg(mirac_token_to_string_view(&def->section)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("type: '" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_def_type_to_string_view(def->type)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("is_used: %s\n", def->is_used ? "yes" : "no");

	switch (def->type)
	{
		case mirac_ast_def_type_func: { print_ast_def_func(&def->as.func_def, indent + 1); } break;
		case mirac_ast_def_type_mem:  { print_ast_def_mem(&def->as.mem_def, indent + 1);   } break;
		case mirac_ast_def_type_str:  { print_ast_def_str(&def->as.str_def, indent + 1);   } break;

		default:
		{
			mirac_logger_debug("encountered an unknown 'mirac_ast_def_type_e' type with raw value of '%u'.", def->type);
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}
