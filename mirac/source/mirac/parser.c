
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

#if 0
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
static void validate_ast_block_str(
	mirac_parser_s* const parser,
	mirac_ast_block_str_s* const str_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block,
	const uint64_t depth);

// TODO: document!
static bool should_stop_parsing(

// TODO: document!
static mirac_ast_block_type_e parse_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block);

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
		case mirac_token_type_identifier:
		{
			block->type = mirac_ast_block_type_call;
			block->as.call_block = parse_ast_block_call(parser);
		} break;

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
static void validate_ast_block_str(
	mirac_parser_s* const parser,
	mirac_ast_block_str_s* const str_block,
	const uint64_t depth);

// TODO: document!
static void validate_ast_block(
	mirac_parser_s* const parser,
	mirac_ast_block_s* const block,
	const uint64_t depth);

// TODO: document!
static bool should_stop_parsing(
	const mirac_ast_block_type_e type);
#endif

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
static mirac_ast_block_s* create_ast_block(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_def_func_s create_ast_def_func(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_def_mem_s create_ast_def_mem(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_def_str_s create_ast_def_str(
	mirac_arena_s* const arena);

// TODO: document!
static mirac_ast_def_s* create_ast_def(
	mirac_arena_s* const arena);

// TODO: document!
static bool is_token_valid_expr_block_token_by_type(
	const mirac_token_type_e type);

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
static mirac_ast_block_s* parse_ast_block(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_def_func_s parse_ast_def_func(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_def_mem_s parse_ast_def_mem(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_def_str_s parse_ast_def_str(
	mirac_parser_s* const parser);

// TODO: document!
static mirac_ast_def_s* parse_ast_def(
	mirac_parser_s* const parser);

// TODO: document!
static void print_ast_block_expr(
	const mirac_ast_block_expr_s* const expr_block,
	const uint64_t indent);

// TODO: document!
static void print_ast_block_as(
	const mirac_ast_block_as_s* const as_block,
	const uint64_t indent);

// TODO: document!
static void print_ast_block_scope(
	const mirac_ast_block_scope_s* const scope_block,
	const uint64_t indent);

// TODO: document!
static void print_ast_block_if(
	const mirac_ast_block_if_s* const if_block,
	const uint64_t indent);

// TODO: document!
static void print_ast_block_elif(
	const mirac_ast_block_elif_s* const elif_block,
	const uint64_t indent);

// TODO: document!
static void print_ast_block_else(
	const mirac_ast_block_else_s* const else_block,
	const uint64_t indent);

// TODO: document!
static void print_ast_block_loop(
	const mirac_ast_block_loop_s* const loop_block,
	const uint64_t indent);

// TODO: document!
static void print_ast_block(
	const mirac_ast_block_s* const block,
	const uint64_t indent);

// TODO: document!
static void print_ast_def_func(
	const mirac_ast_def_func_s* const func_def,
	const uint64_t indent);

// TODO: document!
static void print_ast_def_mem(
	const mirac_ast_def_mem_s* const mem_def,
	const uint64_t indent);

// TODO: document!
static void print_ast_def_str(
	const mirac_ast_def_str_s* const str_def,
	const uint64_t indent);

// TODO: document!
static void print_ast_def(
	const mirac_ast_def_s* const def,
	const uint64_t indent);

mirac_string_view_s mirac_ast_block_scope_type_to_string_view(
	const mirac_ast_block_scope_type_e type)
{
	switch (type)
	{
		case mirac_ast_block_scope_type_parentheses:  { return mirac_string_view_from_cstring("parentheses");  } break;
		case mirac_ast_block_scope_type_brackets:     { return mirac_string_view_from_cstring("brackets");     } break;
		case mirac_ast_block_scope_type_braces:       { return mirac_string_view_from_cstring("braces");       } break;

		default:
		{
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
		case mirac_ast_block_type_as:    { return mirac_string_view_from_cstring("as");    } break;
		case mirac_ast_block_type_scope: { return mirac_string_view_from_cstring("scope"); } break;
		case mirac_ast_block_type_if:    { return mirac_string_view_from_cstring("if");    } break;
		case mirac_ast_block_type_elif:  { return mirac_string_view_from_cstring("elif");  } break;
		case mirac_ast_block_type_else:  { return mirac_string_view_from_cstring("else");  } break;
		case mirac_ast_block_type_loop:  { return mirac_string_view_from_cstring("loop");  } break;
		case mirac_ast_block_type_eou:   { return mirac_string_view_from_cstring("eou");   } break;

		default:
		{
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
			mirac_debug_assert(0); // NOTE: Should never reach this block.
			return mirac_string_view_from_parts("", 0);
		} break;
	}
}

static uint64_t hash_identifier(
	const mirac_string_view_s identifier);

static uint64_t hash_identifier(
	const mirac_string_view_s identifier)
{
	const char* data = identifier.data;
	uint64_t nblocks = identifier.length / 8;
	uint64_t hash = 2166136261u;

	for (uint64_t index = 0; index < nblocks; ++index)
	{
		hash ^= (uint64_t)data[0] << 0  | (uint64_t)data[1] << 8  |
				(uint64_t)data[2] << 16 | (uint64_t)data[3] << 24 |
				(uint64_t)data[4] << 32 | (uint64_t)data[5] << 40 |
				(uint64_t)data[6] << 48 | (uint64_t)data[7] << 56;
		hash *= 0xbf58476d1ce4e5b9;
		data += 8;
	}

	uint64_t last = identifier.length & 0xff;
	switch (identifier.length % 8)
	{
		case 7: last |= (uint64_t)data[6] << 56; /* fallthrough */
		case 6: last |= (uint64_t)data[5] << 48; /* fallthrough */
		case 5: last |= (uint64_t)data[4] << 40; /* fallthrough */
		case 4: last |= (uint64_t)data[3] << 32; /* fallthrough */
		case 3: last |= (uint64_t)data[2] << 24; /* fallthrough */
		case 2: last |= (uint64_t)data[1] << 16; /* fallthrough */
		case 1: last |= (uint64_t)data[0] << 8;
			hash ^= last;
			hash *= 0xd6e8feb86659fd93;
	}

	return hash;
}

mirac_ast_def_map_s mirac_ast_def_map_from_parts(
	mirac_arena_s* const arena,
	const uint64_t capacity)
{
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(capacity > 0);

	mirac_ast_def_map_s def_map = {0};
	def_map.arena = arena;
	def_map.nodes = (mirac_ast_def_map_node_s**)mirac_arena_malloc(arena, capacity * sizeof(mirac_ast_def_map_node_s*));
	mirac_c_memset(def_map.nodes, 0, capacity * sizeof(mirac_ast_def_map_node_s*));
	return def_map;
}

bool mirac_ast_def_map_set(
	mirac_ast_def_map_s* const def_map,
	const mirac_string_view_s identifier,
	const mirac_ast_def_s* const def)
{
	mirac_debug_assert(def_map != NULL);
	mirac_debug_assert(def != NULL);

	const uint64_t hash = hash_identifier(identifier) % def_map->capacity;
	mirac_ast_def_map_node_s* node = def_map->nodes[hash];
	mirac_ast_def_map_node_s* prev_node = NULL;

	while (node != NULL)
	{
		if (mirac_string_view_equal(identifier, node->identifier))
		{
			node->def = def;
			return true;
		}

		prev_node = node;
		node = node->next;
	}

	node = mirac_arena_malloc(def_map->arena, sizeof(mirac_ast_def_map_node_s));
	node->identifier = identifier;
	node->def = def;
	node->next = NULL;
	prev_node->next = node;
	return true;
}

bool mirac_ast_def_map_get(
	mirac_ast_def_map_s* const def_map,
	const mirac_string_view_s key,
	const mirac_ast_def_s** const def)
{
	mirac_debug_assert(def_map != NULL);
	mirac_debug_assert(def != NULL);

	uint64_t hash = hash_identifier(key);
	hash = hash % def_map->capacity;
	mirac_ast_def_map_node_s* node = def_map->nodes[hash];

	while (node != NULL)
	{
		if (mirac_string_view_equal(key, node->identifier))
		{
			*def = node->def;
			return true;
		}

		node = node->next;
	}

	return false;
}

mirac_ast_unit_s mirac_ast_unit_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_unit_s unit = {0};
	unit.defs = mirac_ast_def_list_from_parts(arena);
	unit.def_map = mirac_ast_def_map_from_parts(arena, 1000);
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

	mirac_ast_unit_s unit = mirac_ast_unit_from_parts(parser->arena);
	mirac_ast_def_s* def = NULL;

	while ((def = parse_ast_def(parser)) != NULL)
	{
		if (mirac_ast_def_type_none == def->type)
		{
			break;
		}

		const mirac_token_s* identifier_token = NULL;
		switch (def->type)
		{
			case mirac_ast_def_type_func:
			{
				identifier_token = &def->as.func_def.identifier;
			} break;

			case mirac_ast_def_type_mem:
			{
				identifier_token = &def->as.mem_def.identifier;
			} break;

			case mirac_ast_def_type_str:
			{
				identifier_token = &def->as.str_def.identifier;
			} break;

			default:
			{
				mirac_debug_assert(0); // NOTE: Should never reach this block.
			} break;
		}

		const mirac_ast_def_s* existing_def = NULL;
		if (mirac_ast_def_map_get(&unit.def_map, identifier_token->as.ident, &existing_def))
		{
			mirac_debug_assert(existing_def != NULL);
			log_parser_error_and_exit(identifier_token->location,
				"encountered a redefinition of '" mirac_sv_fmt "' identifier.",
				mirac_sv_arg(identifier_token->as.ident)
			);
		}

		mirac_ast_def_list_push(&unit.defs, def);
	}

	return unit;
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

static mirac_ast_block_elif_s create_ast_block_elif(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_ast_block_elif_s elif_block = {0};
	return elif_block;
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
		(mirac_token_type_literal_str                != type) &&
		(mirac_token_type_reserved_sec               != type) &&
		(mirac_token_type_reserved_ret               != type) &&
		(mirac_token_type_reserved_req               != type) &&
		(mirac_token_type_reserved_func              != type) &&
		(mirac_token_type_reserved_mem               != type) &&
		(mirac_token_type_reserved_str               != type) &&
		(mirac_token_type_reserved_as                != type) &&
		(mirac_token_type_reserved_if                != type) &&
		(mirac_token_type_reserved_elif              != type) &&
		(mirac_token_type_reserved_else              != type) &&
		(mirac_token_type_reserved_loop              != type) &&
		(mirac_token_type_reserved_left_parenthesis  != type) &&
		(mirac_token_type_reserved_right_parenthesis != type) &&
		(mirac_token_type_reserved_left_bracket      != type) &&
		(mirac_token_type_reserved_right_bracket     != type) &&
		(mirac_token_type_reserved_left_brace        != type) &&
		(mirac_token_type_reserved_right_brace       != type) &&
		(mirac_token_type_reserved_i08               != type) &&
		(mirac_token_type_reserved_i16               != type) &&
		(mirac_token_type_reserved_i32               != type) &&
		(mirac_token_type_reserved_i64               != type) &&
		(mirac_token_type_reserved_u08               != type) &&
		(mirac_token_type_reserved_u16               != type) &&
		(mirac_token_type_reserved_u32               != type) &&
		(mirac_token_type_reserved_u64               != type) &&
		(mirac_token_type_reserved_f32               != type) &&
		(mirac_token_type_reserved_f64               != type) &&
		(mirac_token_type_reserved_ptr               != type)
	);
}

static mirac_ast_block_expr_s parse_ast_block_expr(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

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

static mirac_ast_block_as_s parse_ast_block_as(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

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

	mirac_ast_block_scope_s scope_block = create_ast_block_scope(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert((mirac_token_type_reserved_left_parenthesis == token.type) ||
					   (mirac_token_type_reserved_left_bracket     == token.type) ||
					   (mirac_token_type_reserved_left_brace       == token.type));

	mirac_token_type_e scope_end_token_type;
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
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}

	while (1)
	{
		(void)mirac_lexer_lex_next(parser->lexer, &token);
		if (scope_end_token_type == token.type) { break; }
		mirac_lexer_unlex(parser->lexer, &token);

		block = parse_ast_block(parser);

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

		if ((mirac_ast_block_type_elif == block_ref->type) ||
			(mirac_ast_block_type_else == block_ref->type))
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
				if ((prev_block_ref->type != mirac_ast_block_type_if) &&
					(prev_block_ref->type != mirac_ast_block_type_elif))
				{
					log_parser_error_and_exit(block_ref->location,
						"expected 'if' block prior to '" mirac_sv_fmt "' block, but found '" mirac_sv_fmt"' block.",
						mirac_sv_arg(mirac_ast_block_type_to_string_view(block_ref->type)),
						mirac_sv_arg(mirac_ast_block_type_to_string_view(prev_block_ref->type))
					);
				}
			}
		}
	}

	return scope_block;
}

static mirac_ast_block_if_s parse_ast_block_if(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

	mirac_ast_block_if_s if_block = create_ast_block_if(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_if == token.type);

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'if' block's condition, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	if_block.cond = block;

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'if' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	if_block.body = block;

	return if_block;
}

static mirac_ast_block_elif_s parse_ast_block_elif(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

	mirac_ast_block_elif_s elif_block = create_ast_block_elif(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_elif == token.type);

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'elif' block's condition, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	elif_block.cond = block;

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'elif' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	elif_block.body = block;

	return elif_block;
}

static mirac_ast_block_else_s parse_ast_block_else(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

	mirac_ast_block_else_s else_block = create_ast_block_else(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_else == token.type);

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'else' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	else_block.body = block;

	return else_block;
}

static mirac_ast_block_loop_s parse_ast_block_loop(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

	mirac_ast_block_loop_s loop_block = create_ast_block_loop(parser->arena);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	mirac_ast_block_s* block = NULL;

	(void)mirac_lexer_lex_next(parser->lexer, &token);
	mirac_debug_assert(mirac_token_type_reserved_loop == token.type);

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'loop' block's condition, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	loop_block.cond = block;

	if ((block = parse_ast_block(parser))->type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'loop' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	loop_block.body = block;

	return loop_block;
}

static mirac_ast_block_s* parse_ast_block(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

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
	mirac_ast_block_s* block = parse_ast_block(parser);

	if (block->type != mirac_ast_block_type_scope)
	{
		log_parser_error_and_exit(block->location,
			"expected 'scope' block as 'func' block's body, but found '" mirac_sv_fmt "' block.",
			mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type))
		);
	}

	func_def.body = block;
	return func_def;
}

static mirac_ast_def_mem_s parse_ast_def_mem(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

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

	if (!mirac_token_is_signed_numeric_literal(&token) &&
		!mirac_token_is_unsigned_numeric_literal(&token))
	{
		log_parser_error_and_exit(token.location,
			"expected capacity token after 'mem' identifier token to be signed or unsigned integer literal token, but found '" mirac_sv_fmt "' token.",
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

	mem_def.capacity = token;
	return mem_def;
}

static mirac_ast_def_str_s parse_ast_def_str(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

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
	return str_def;
}

static mirac_ast_def_s* parse_ast_def(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

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

static void print_ast_block_elif(
	const mirac_ast_block_elif_s* const elif_block,
	const uint64_t indent)
{
	mirac_debug_assert(elif_block != NULL);
	mirac_debug_assert(elif_block->cond != NULL);
	mirac_debug_assert(elif_block->body != NULL);
	mirac_debug_assert(mirac_ast_block_type_scope == elif_block->cond->type);
	mirac_debug_assert(mirac_ast_block_type_scope == elif_block->body->type);

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("ElifBlock[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("cond:\n");
	print_ast_block(elif_block->cond, indent + 2);

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("body:\n");
	print_ast_block(elif_block->body, indent + 2);

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
	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("Block[\n");

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("type: '" mirac_sv_fmt "'\n", mirac_sv_arg(mirac_ast_block_type_to_string_view(block->type)));

	for (uint8_t indent_index = 0; indent_index < (indent + 1); ++indent_index) printf("\t");
	printf("location: '" mirac_location_fmt "'\n", mirac_location_arg(block->location));

	switch (block->type)
	{
		case mirac_ast_block_type_expr:
		{
			print_ast_block_expr(&block->as.expr_block, indent + 1);
		} break;

		case mirac_ast_block_type_as:
		{
			print_ast_block_as(&block->as.as_block, indent + 1);
		} break;

		case mirac_ast_block_type_scope:
		{
			print_ast_block_scope(&block->as.scope_block, indent + 1);
		} break;

		case mirac_ast_block_type_if:
		{
			print_ast_block_if(&block->as.if_block, indent + 1);
		} break;

		case mirac_ast_block_type_elif:
		{
			print_ast_block_elif(&block->as.elif_block, indent + 1);
		} break;

		case mirac_ast_block_type_else:
		{
			print_ast_block_else(&block->as.else_block, indent + 1);
		} break;

		case mirac_ast_block_type_loop:
		{
			print_ast_block_loop(&block->as.loop_block, indent + 1);
		} break;

		default:
		{
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
		case mirac_ast_def_type_func:
		{
			print_ast_def_func(&def->as.func_def, indent + 1);
		} break;

		case mirac_ast_def_type_mem:
		{
			print_ast_def_mem(&def->as.mem_def, indent + 1);
		} break;

		case mirac_ast_def_type_str:
		{
			print_ast_def_str(&def->as.str_def, indent + 1);
		} break;

		default:
		{
			mirac_debug_assert(0); // NOTE: Should never reach this block.
		} break;
	}

	for (uint8_t indent_index = 0; indent_index < indent; ++indent_index) printf("\t");
	printf("]\n");
}



#if 0
static void validate_ast_block_expr(
	mirac_parser_s* const parser,
	mirac_ast_block_expr_s* const expr_block,
	const uint64_t depth)
{
	(void)depth;
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(expr_block != NULL);
}

static void validate_ast_block_call(
	mirac_parser_s* const parser,
	mirac_ast_block_call_s* const call_block,
	const uint64_t depth)
{
	(void)depth;
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(call_block != NULL);

	for (uint64_t func_ref_index = 0; func_ref_index < parser->unit.func_refs.count; ++func_ref_index)
	{
		mirac_ast_block_func_s* const func_ref = parser->unit.func_refs.data[func_ref_index];
		mirac_debug_assert(func_ref != NULL);

		if (mirac_string_view_equal(call_block->token.as.ident, func_ref->identifier.as.ident))
		{
			call_block->type = mirac_ast_block_call_type_func;
			call_block->as.func_ref = func_ref;
			func_ref->is_used = true;
			goto found_match;
		}
	}

	for (uint64_t mem_ref_index = 0; mem_ref_index < parser->unit.mem_refs.count; ++mem_ref_index)
	{
		mirac_ast_block_mem_s* const mem_ref = parser->unit.mem_refs.data[mem_ref_index];
		mirac_debug_assert(mem_ref != NULL);

		if (mirac_string_view_equal(call_block->token.as.ident, mem_ref->identifier.as.ident))
		{
			call_block->type = mirac_ast_block_call_type_mem;
			call_block->as.mem_ref = mem_ref;
			mem_ref->is_used = true;
			goto found_match;
		}
	}

	for (uint64_t str_ref_index = 0; str_ref_index < parser->unit.str_refs.count; ++str_ref_index)
	{
		mirac_ast_block_str_s* const str_ref = parser->unit.str_refs.data[str_ref_index];
		mirac_debug_assert(str_ref != NULL);

		if (mirac_string_view_equal(call_block->token.as.ident, str_ref->identifier.as.ident))
		{
			call_block->type = mirac_ast_block_call_type_str;
			call_block->as.str_ref = str_ref;
			str_ref->is_used = true;
			goto found_match;
		}
	}

	log_parser_error_and_exit(call_block->token.location,
		"encountered an undefined identifier '" mirac_sv_fmt "'.",
		mirac_sv_arg(call_block->token.text)
	);

found_match:
	return;
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

	validate_ast_block_scope(parser, &if_block->cond, depth);
	validate_ast_block_scope(parser, &if_block->body, depth);
}

static void validate_ast_block_elif(
	mirac_parser_s* const parser,
	mirac_ast_block_elif_s* const elif_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(elif_block != NULL);

	validate_ast_block_scope(parser, &elif_block->cond, depth);
	validate_ast_block_scope(parser, &elif_block->body, depth);
}

static void validate_ast_block_else(
	mirac_parser_s* const parser,
	mirac_ast_block_else_s* const else_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(else_block != NULL);

	validate_ast_block_scope(parser, &else_block->body, depth);
}

static void validate_ast_block_loop(
	mirac_parser_s* const parser,
	mirac_ast_block_loop_s* const loop_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(loop_block != NULL);

	validate_ast_block_scope(parser, &loop_block->cond, depth);
	validate_ast_block_scope(parser, &loop_block->body, depth);
}

static void validate_ast_block_func(
	mirac_parser_s* const parser,
	mirac_ast_block_func_s* const func_block,
	const uint64_t depth)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(func_block != NULL);

	for (uint64_t func_ref_index = 0; func_ref_index < parser->unit.func_refs.count; ++func_ref_index)
	{
		mirac_ast_block_func_s* const func_ref = parser->unit.func_refs.data[func_ref_index];
		mirac_debug_assert(func_ref != NULL);

		if (mirac_string_view_equal(func_block->identifier.as.ident, func_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(func_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(func_block->identifier.text)
			);
		}
	}

	for (uint64_t mem_ref_index = 0; mem_ref_index < parser->unit.mem_refs.count; ++mem_ref_index)
	{
		mirac_ast_block_mem_s* const mem_ref = parser->unit.mem_refs.data[mem_ref_index];
		mirac_debug_assert(mem_ref != NULL);

		if (mirac_string_view_equal(func_block->identifier.as.ident, mem_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(func_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(func_block->identifier.text)
			);
		}
	}

	for (uint64_t str_ref_index = 0; str_ref_index < parser->unit.str_refs.count; ++str_ref_index)
	{
		mirac_ast_block_str_s* const str_ref = parser->unit.str_refs.data[str_ref_index];
		mirac_debug_assert(str_ref != NULL);

		if (mirac_string_view_equal(func_block->identifier.as.ident, str_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(func_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(func_block->identifier.text)
			);
		}
	}

	mirac_func_refs_vector_push(&parser->unit.func_refs, func_block);

	validate_ast_block_scope(parser, &func_block->body, depth);
}

static void validate_ast_block_mem(
	mirac_parser_s* const parser,
	mirac_ast_block_mem_s* const mem_block,
	const uint64_t depth)
{
	(void)depth;
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(mem_block != NULL);

	for (uint64_t func_ref_index = 0; func_ref_index < parser->unit.func_refs.count; ++func_ref_index)
	{
		mirac_ast_block_func_s* const func_ref = parser->unit.func_refs.data[func_ref_index];
		mirac_debug_assert(func_ref != NULL);

		if (mirac_string_view_equal(mem_block->identifier.as.ident, func_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(mem_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(mem_block->identifier.text)
			);
		}
	}

	for (uint64_t mem_ref_index = 0; mem_ref_index < parser->unit.mem_refs.count; ++mem_ref_index)
	{
		mirac_ast_block_mem_s* const mem_ref = parser->unit.mem_refs.data[mem_ref_index];
		mirac_debug_assert(mem_ref != NULL);

		if (mirac_string_view_equal(mem_block->identifier.as.ident, mem_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(mem_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(mem_block->identifier.text)
			);
		}
	}

	for (uint64_t str_ref_index = 0; str_ref_index < parser->unit.str_refs.count; ++str_ref_index)
	{
		mirac_ast_block_str_s* const str_ref = parser->unit.str_refs.data[str_ref_index];
		mirac_debug_assert(str_ref != NULL);

		if (mirac_string_view_equal(mem_block->identifier.as.ident, str_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(mem_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(mem_block->identifier.text)
			);
		}
	}

	mirac_mem_refs_vector_push(&parser->unit.mem_refs, mem_block);
}

static void validate_ast_block_str(
	mirac_parser_s* const parser,
	mirac_ast_block_str_s* const str_block,
	const uint64_t depth)
{
	(void)depth;
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(str_block != NULL);

	for (uint64_t func_ref_index = 0; func_ref_index < parser->unit.func_refs.count; ++func_ref_index)
	{
		mirac_ast_block_func_s* const func_ref = parser->unit.func_refs.data[func_ref_index];
		mirac_debug_assert(func_ref != NULL);

		if (mirac_string_view_equal(str_block->identifier.as.ident, func_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(str_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(str_block->identifier.text)
			);
		}
	}

	for (uint64_t mem_ref_index = 0; mem_ref_index < parser->unit.mem_refs.count; ++mem_ref_index)
	{
		mirac_ast_block_mem_s* const mem_ref = parser->unit.mem_refs.data[mem_ref_index];
		mirac_debug_assert(mem_ref != NULL);

		if (mirac_string_view_equal(str_block->identifier.as.ident, mem_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(str_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(str_block->identifier.text)
			);
		}
	}

	for (uint64_t str_ref_index = 0; str_ref_index < parser->unit.str_refs.count; ++str_ref_index)
	{
		mirac_ast_block_str_s* const str_ref = parser->unit.str_refs.data[str_ref_index];
		mirac_debug_assert(str_ref != NULL);

		if (mirac_string_view_equal(str_block->identifier.as.ident, str_ref->identifier.as.ident))
		{
			log_parser_error_and_exit(str_block->identifier.location,
				"encountered a redefinition of identifier '" mirac_sv_fmt "'.",
				mirac_sv_arg(str_block->identifier.text)
			);
		}
	}

	mirac_str_refs_vector_push(&parser->unit.str_refs, str_block);
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
		case mirac_ast_block_type_str:   { validate_ast_block_str(parser, &block->as.str_block, depth + 1);     } break;

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
#endif
