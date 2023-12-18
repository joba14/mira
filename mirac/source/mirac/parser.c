
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

mirac_implement_heap_array_type(mirac_ast_tokens_vector, mirac_token_s);
mirac_implement_heap_array_type(mirac_ast_tokens_refs_vector, mirac_token_s*);
mirac_implement_heap_array_type(mirac_globals_vector, mirac_global_s);

#define log_parser_error_and_exit(_location, _format, ...)                     \
	do {                                                                       \
		(void)fprintf(stderr, mirac_sv_fmt ":%lu:%lu: ",                       \
			mirac_sv_arg((_location).file),                                    \
			(_location).line,                                                  \
			(_location).column);                                               \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		mirac_c_exit(-1);                                                      \
	} while (0)

static mirac_global_function_s try_parse_function(
	mirac_parser_s* const parser);

static mirac_global_memory_s try_parse_memory(
	mirac_parser_s* const parser);

static void collect_string_literals(
	mirac_unit_s* const unit);

static void perform_cross_reference(
	mirac_arena_s* const arena,
	mirac_unit_s* const unit);

const char* mirac_global_type_to_string(
	const mirac_global_type_e global_type)
{
	switch (global_type)
	{
		case mirac_global_type_function:
		{
			return "function";
		} break;

		case mirac_global_type_memory:
		{
			return "memory";
		} break;

		default:
		{
			mirac_debug_assert(0);
			return NULL;
		} break;
	}
}

void mirac_global_print(
	const uint64_t global_index,
	const mirac_global_s* const global)
{
	mirac_debug_assert(global != NULL);
	mirac_logger_log("    [%lu] Global[type='%s'", global_index, mirac_global_type_to_string(global->type));

	switch (global->type)
	{
		case mirac_global_type_function:
		{
			mirac_logger_log("        ident: '%s'", mirac_token_to_string(&global->as_function.identifier));

			mirac_logger_log("        req (%lu):", global->as_function.req_tokens.count);
			for (uint64_t index = 0; index < global->as_function.req_tokens.count; ++index)
				mirac_logger_log("            [%lu]: '%s'", index, mirac_token_to_string(&global->as_function.req_tokens.data[index]));

			mirac_logger_log("        ret (%lu):", global->as_function.ret_tokens.count);
			for (uint64_t index = 0; index < global->as_function.ret_tokens.count; ++index)
				mirac_logger_log("            [%lu]: '%s'", index, mirac_token_to_string(&global->as_function.ret_tokens.data[index]));

			mirac_logger_log("        body (%lu):", global->as_function.body_tokens.count);
			for (uint64_t index = 0; index < global->as_function.body_tokens.count; ++index)
				mirac_logger_log("            [%lu]: '%s'", index, mirac_token_to_string(&global->as_function.body_tokens.data[index]));

			mirac_logger_log("        inlined: '%s'", (global->as_function.is_inlined ? "yes" : "no"));
			mirac_logger_log("        entry: '%s'", (global->as_function.is_entry ? "yes" : "no"));
		} break;

		case mirac_global_type_memory:
		{
			mirac_logger_log("        ident: '%s'", mirac_token_to_string(&global->as_memory.identifier));
			mirac_logger_log("        capacity: '%s'", mirac_token_to_string(&global->as_memory.capacity));
		} break;

		default:
		{
			mirac_debug_assert(0);
		} break;
	}

	mirac_logger_log("    ]");
}

mirac_unit_s mirac_unit_from_parts(
	mirac_arena_s* const arena,
	const char* identifier)
{
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(identifier != NULL);
	mirac_unit_s unit = {0};
	unit.identifier = identifier;
	return unit;
}

void mirac_unit_print(
	mirac_unit_s* const unit)
{
	mirac_debug_assert(unit != NULL);

	mirac_logger_log("Unit[identifier='%s'", unit->identifier);

	for (uint64_t global_index = 0; global_index < unit->globals.count; ++global_index)
	{
		mirac_global_s* const global = &unit->globals.data[global_index];
		mirac_debug_assert(global != NULL);
		mirac_global_print(global_index, global);
	}

	for (uint64_t string_index = 0; string_index < unit->strings.count; ++string_index)
	{
		mirac_token_s** const string = &unit->strings.data[string_index];
		mirac_debug_assert(string != NULL && *string != NULL);
		mirac_logger_log("    [%lu] %s", string_index, mirac_token_to_string(*string));
	}

	mirac_logger_log("]");
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

mirac_unit_s mirac_parser_parse_unit(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);

	mirac_unit_s unit = mirac_unit_from_parts(parser->arena, parser->lexer->file_path);
	unit.globals = mirac_globals_vector_from_parts(4);
	unit.strings = mirac_tokens_refs_vector_from_parts(4);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
	{
		switch (token.type)
		{
			case mirac_token_type_reserved_inl:
			case mirac_token_type_reserved_func:
			{
				mirac_lexer_unlex(parser->lexer, &token);
				const mirac_global_function_s function = try_parse_function(parser);
				const mirac_global_s global = { .type = mirac_global_type_function, .as_function = function };
				mirac_globals_vector_push(&unit.globals, global);
			} break;

			case mirac_token_type_reserved_mem:
			{
				mirac_lexer_unlex(parser->lexer, &token);
				const mirac_global_memory_s memory = try_parse_memory(parser);
				const mirac_global_s global = { .type = mirac_global_type_memory, .as_memory = memory };
				mirac_globals_vector_push(&unit.globals, global);
			} break;

			default:
			{
				log_parser_error_and_exit(token.location,
					"invalid global token '%.*s' encountered.",
					(signed int)token.source.length, token.source.data
				);
			} break;
		}
	}

	perform_cross_reference(&unit);
	collect_string_literals(&unit);
	return unit;
}

static mirac_global_function_s try_parse_function(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_global_function_s function = {0};
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	(void)mirac_lexer_lex(parser->lexer, &token);

	if (mirac_token_type_reserved_inl == token.type)
	{
		function.is_inlined = true;

		mirac_token_s temp_token = mirac_token_from_type(mirac_token_type_none);
		if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &temp_token)))
		{
			log_parser_error_and_exit(token.location,
				"missing keyword 'func' after '%.*s' keyword.",
				(signed int)token.source.length, token.source.data
			);
		}
		else
		{
			token = temp_token;
		}
	}

	if (token.type != mirac_token_type_reserved_func)
	{
		log_parser_error_and_exit(token.location,
			"expected 'func' keyword after 'inl' keyword, but found token '%.*s'.",
			(signed int)token.source.length, token.source.data
		);
	}
	else
	{
		mirac_token_s temp_token = mirac_token_from_type(mirac_token_type_none);
		if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &temp_token)))
		{
			log_parser_error_and_exit(token.location,
				"missing identifier token after '%.*s' keyword.",
				(signed int)token.source.length, token.source.data
			);
		}
		else
		{
			token = temp_token;
		}
	}

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'func' keyword, but found token '%.*s'.",
			(signed int)token.source.length, token.source.data
		);
	}
	else
	{
		function.identifier = token;
		function.is_entry = (mirac_c_strcmp(function.identifier.as_ident.data, parser->config->entry) == 0);

		if (function.is_entry && function.is_inlined)
		{
			log_parser_error_and_exit(token.location,
				"entry function '%.*s' cannot be inlined.",
				(signed int)function.identifier.as_ident.length, function.identifier.as_ident.data
			);
		}

		mirac_token_s temp_token = mirac_token_from_type(mirac_token_type_none);
		if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &temp_token)))
		{
			log_parser_error_and_exit(token.location,
				"missing type specifiers or/and function body after '%.*s' token.",
				(signed int)token.source.length, token.source.data
			);
		}
		else
		{
			token = temp_token;
		}
	}

	if (mirac_token_type_reserved_req == token.type)
	{
		function.req_tokens = mirac_tokens_vector_from_parts(4);

		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
		{
			if (mirac_token_is_type_keyword(&token))
			{
				mirac_tokens_vector_push(&function.req_tokens, token);
			}
			else if (mirac_token_type_reserved_ret == token.type ||
					 mirac_token_type_reserved_do == token.type)
			{
				if (function.req_tokens.count <= 0)
				{
					log_parser_error_and_exit(token.location,
						"no specifier tokens were found after 'req' keyword."
					);
				}
				else
				{
					break;
				}
			}
			else
			{
				log_parser_error_and_exit(token.location,
					"invalid token '%.*s' encountered instead of a type, 'ret', or 'do' keywords.",
					(signed int)token.source.length, token.source.data
				);
			}
		}
	}

	if (mirac_token_type_reserved_ret == token.type)
	{
		function.ret_tokens = mirac_tokens_vector_from_parts(4);

		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
		{
			if (mirac_token_is_type_keyword(&token))
			{
				mirac_tokens_vector_push(&function.ret_tokens, token);
			}
			else if (mirac_token_type_reserved_do == token.type)
			{
				if (function.ret_tokens.count <= 0)
				{
					log_parser_error_and_exit(token.location,
						"no type specifier tokens were found after 'ret' keyword."
					);
				}
				else
				{
					break;
				}
			}
			else
			{
				log_parser_error_and_exit(token.location,
					"invalid token '%.*s' encountered instead of a type, or 'do' keyword.",
					(signed int)token.source.length, token.source.data
				);
			}
		}
	}

	if (token.type != mirac_token_type_reserved_do)
	{
		log_parser_error_and_exit(token.location,
			"expected 'do' keyword after type specifiers or function identifier, but found token '%.*s'.",
			(signed int)token.source.length, token.source.data
		);
	}
	else
	{
		function.body_tokens = mirac_tokens_vector_from_parts(4);
		bool increase_scope_counter = false;
		int64_t scopes_counter = 1;

		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
		{
			switch (token.type)
			{
				case mirac_token_type_reserved_inl:
				case mirac_token_type_reserved_func:
				case mirac_token_type_reserved_mem:
				{
					log_parser_error_and_exit(token.location,
						"invalid keyword '%.*s' encountered in function's body.",
						(signed int)token.source.length, token.source.data
					);
				} break;

				case mirac_token_type_reserved_if:
				case mirac_token_type_reserved_loop:
				case mirac_token_type_reserved_let:
				case mirac_token_type_reserved_reg:
				{
					increase_scope_counter = true;
				} break;

				case mirac_token_type_reserved_do:
				{
					if (increase_scope_counter)
					{
						increase_scope_counter = false;
					}

					++scopes_counter;
				} break;

				case mirac_token_type_reserved_end:
				{
					--scopes_counter;
				} break;

				default:
				{
				} break;
			}

			if (scopes_counter <= 0)
			{
				break;
			}

			mirac_tokens_vector_push(&function.body_tokens, token);
		}

		if (scopes_counter != 0)
		{
			mirac_token_s temp_token = mirac_token_from_type(mirac_token_type_none);
			if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &temp_token)))
			{
				log_parser_error_and_exit(token.location,
					"unclosed scope (too many 'do' keywords) encountered."
				);
			}
			else
			{
				token = temp_token;
			}
		}
	}

	return function;
}

static mirac_global_memory_s try_parse_memory(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_global_memory_s memory = {0};
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	(void)mirac_lexer_lex(parser->lexer, &token);
	(void)mirac_lexer_lex(parser->lexer, &token);

	if (token.type != mirac_token_type_identifier)
	{
		log_parser_error_and_exit(token.location,
			"expected identifier token after 'mem' token, but found token '" mirac_sv_fmt "'.",
			mirac_sv_arg(token->text)
		);
	}
	else
	{
		memory.identifier = token;

		if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
		{
			log_parser_error_and_exit(token.location,
				"missing capacity specifier after '" mirac_sv_fmt "' token.",
				mirac_sv_arg(token->text)
			);
		}
	}

	if (!mirac_token_is_signed_integer_literal(&token) &&
		!mirac_token_is_unsigned_integer_literal(&token))
	{
		log_parser_error_and_exit(token.location,
			"invalid token '" mirac_sv_fmt "' used as capacity specifier.",
			mirac_sv_arg(token->text)
		);
	}

	if (mirac_token_is_unsigned_integer_literal(&token) && token.as_uval <= 0)
	{
		log_parser_error_and_exit(token.location,
			"unsigned capacity specifier '" mirac_sv_fmt "' must be higher than 0.",
			mirac_sv_arg(token->text)
		);
	}
	else if (mirac_token_is_signed_integer_literal(&token) && token.as_ival <= 0)
	{
		log_parser_error_and_exit(token.location,
			"signed capacity specifier '" mirac_sv_fmt "' must be higher than 0.",
			mirac_sv_arg(token->text)
		);
	}
	else
	{
		memory.capacity = token;

		if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
		{
			log_parser_error_and_exit(token.location,
				"missing capacity specifier after '" mirac_sv_fmt "' token.",
				mirac_sv_arg(token->text)
			);
		}
	}

	if (token.type != mirac_token_type_reserved_end)
	{
		log_parser_error_and_exit(token.location,
			"expected 'end' token after memorie's capacity token, but found token '" mirac_sv_fmt "'.",
			mirac_sv_arg(token->text)
		);
	}

	return memory;
}

static void collect_string_literals(
	mirac_unit_s* const unit)
{
	mirac_debug_assert(unit != NULL);

	for (uint64_t global_index = 0; global_index < unit->globals.count; ++global_index)
	{
		mirac_global_s* const global = &unit->globals.data[global_index];
		mirac_debug_assert(global != NULL);

		if (global->type != mirac_global_type_function)
		{
			continue;
		}

		mirac_global_function_s* const function = &global->as_function;
		mirac_debug_assert(function != NULL);

		if (function->body_tokens.count <= 0)
		{
			continue;
		}

		for (uint64_t body_index = 0; body_index < function->body_tokens.count; ++body_index)
		{
			mirac_token_s* const token = &function->body_tokens.data[body_index];
			mirac_debug_assert(token != NULL);

			switch (token->type)
			{
				case mirac_token_type_literal_str:
				{
					bool string_literal_exists = false;

					for (uint64_t string_index = 0; string_index < unit->strings.count; ++string_index)
					{
						mirac_token_s* const string_token = unit->strings.data[string_index];
						mirac_debug_assert(string_token != NULL);
						mirac_debug_assert(mirac_token_is_string_literal(string_token));

						if (mirac_c_strncmp(string_token->as_str.data, token->as_str.data, token->as_str.length) == 0)
						{
							string_literal_exists = true;
							break;
						}
					}

					if (!string_literal_exists)
					{
						mirac_tokens_refs_vector_push(&unit->strings, token);
					}
				} break;

				default:
				{
				} break;
			}
		}
	}
}

static void perform_cross_reference(
	mirac_arena_s* const arena,
	mirac_unit_s* const unit)
{
	mirac_debug_assert(arena != NULL);
	mirac_debug_assert(unit != NULL);

	for (uint64_t global_index = 0; global_index < unit->globals.count; ++global_index)
	{
		mirac_global_s* const global = &unit->globals.data[global_index];
		mirac_debug_assert(global != NULL);

		if (global->type != mirac_global_type_function)
		{
			continue;
		}

		mirac_global_function_s* const function = &global->as_function;
		mirac_debug_assert(function != NULL);

		if (function->body_tokens.count <= 0)
		{
			continue;
		}

		mirac_tokens_refs_vector_s stack = mirac_tokens_refs_vector_from_parts(arena, 64);

		for (uint64_t body_index = 0; body_index < function->body_tokens.count; ++body_index)
		{
			mirac_token_s* const token = &function->body_tokens.data[body_index];
			mirac_debug_assert(token != NULL);

			switch (token->type)
			{
				case mirac_token_type_reserved_if:
				case mirac_token_type_reserved_loop:
				case mirac_token_type_reserved_let:
				case mirac_token_type_reserved_reg:
				{
					mirac_token_s* popped = NULL;

					if (mirac_tokens_refs_vector_pop(&stack, &popped))
					{
						if (mirac_token_type_reserved_if == popped->type ||
							mirac_token_type_reserved_loop == popped->type ||
							mirac_token_type_reserved_let == popped->type ||
							mirac_token_type_reserved_reg == popped->type)
						{
							log_parser_error_and_exit(token->location,
								"encountered an invalid token '" mirac_sv_fmt "', following the '" mirac_sv_fmt "' token.",
								mirac_sv_arg(token->text),
								mirac_sv_arg(popped->text)
							);
						}
						else
						{
							mirac_tokens_refs_vector_push(&stack, popped);
						}
					}

					mirac_tokens_refs_vector_push(&stack, token);
				} break;

				case mirac_token_type_reserved_elif:
				{
					mirac_token_s* popped = NULL;

					if (!mirac_tokens_refs_vector_pop(&stack, &popped))
					{
						log_parser_error_and_exit(token->location,
							"'" mirac_sv_fmt "' keyword must follow 'if <condition> do <body> elif <block> end' sequence.",
							mirac_sv_arg(token->text)
						);
					}

					if (NULL == popped->prev_ref)
					{
						log_parser_error_and_exit(token->location,
							"missing 'if' or 'elif' keyword in 'if <condition> do <body> elif <condition> do <body> end' sequence."
						);
					}

					if (mirac_token_type_reserved_do == popped->type &&
						(mirac_token_type_reserved_if == popped->prev_ref->type ||
						 mirac_token_type_reserved_elif == popped->prev_ref->type))
					{
						popped->next_ref = token;
						token->prev_ref = popped;
						mirac_tokens_refs_vector_push(&stack, token);
					}
					else
					{
						log_parser_error_and_exit(token->location,
							"'" mirac_sv_fmt "' keyword must follow 'if <condition> do <body> elif <condition> do <block> end' sequence.",
							mirac_sv_arg(token->text)
						);
					}
				} break;

				case mirac_token_type_reserved_else:
				{
					mirac_token_s* popped = NULL;

					if (!mirac_tokens_refs_vector_pop(&stack, &popped))
					{
						log_parser_error_and_exit(token->location,
							"'" mirac_sv_fmt "' keyword must follow 'if <condition> do <block>' sequence.",
							mirac_sv_arg(token->text)
						);
					}

					if (NULL == popped->prev_ref)
					{
						log_parser_error_and_exit(token->location,
							"missing 'if' keyword in 'if <condition> do <body> else <body> end' sequence."
						);
					}

					if (mirac_token_type_reserved_do == popped->type &&
						(mirac_token_type_reserved_if == popped->prev_ref->type ||
						 mirac_token_type_reserved_elif == popped->prev_ref->type))
					{
						popped->next_ref = token;
						token->prev_ref = popped;
						mirac_tokens_refs_vector_push(&stack, token);
					}
					else
					{
						log_parser_error_and_exit(token->location,
							"'" mirac_sv_fmt "' keyword must follow 'if <condition> do <block>' sequence",
							mirac_sv_arg(token->text)
						);
					}
				} break;

				case mirac_token_type_reserved_do:
				{
					mirac_token_s* popped = NULL;

					if (!mirac_tokens_refs_vector_pop(&stack, &popped))
					{
						log_parser_error_and_exit(token->location,
							"'" mirac_sv_fmt "' keyword must follow 'if <condition>', 'loop <condition>', or 'let <identifiers...>' sequence.",
							mirac_sv_arg(token->text)
						);
					}

					if (mirac_token_type_reserved_if == popped->type ||
						mirac_token_type_reserved_elif == popped->type ||
						mirac_token_type_reserved_loop == popped->type ||
						mirac_token_type_reserved_let == popped->type ||
						mirac_token_type_reserved_reg == popped->type)
					{
						popped->next_ref = token;
						token->prev_ref = popped;
						mirac_tokens_refs_vector_push(&stack, token);
					}
					else
					{
						log_parser_error_and_exit(token->location,
							"'" mirac_sv_fmt "' keyword must follow 'if <condition>', 'loop <condition>', or 'let <identifiers...>' sequence.",
							mirac_sv_arg(token->text)
						);
					}
				} break;

				case mirac_token_type_reserved_end:
				{
					mirac_token_s* popped = NULL;

					if (!mirac_tokens_refs_vector_pop(&stack, &popped))
					{
						log_parser_error_and_exit(token->location,
							"'" mirac_sv_fmt "' keyword must follow 'if <condition> do <block>', 'if <condition> do <block> else <block> end', 'if <condition> do <block> elif <condition> do else <block> end', 'loop <condition> do <block>', or 'let <identifiers...> do <block> end' sequence.",
							mirac_sv_arg(token->text)
						);
					}

					if (mirac_token_type_reserved_do == popped->type)
					{
						if (mirac_token_type_reserved_if == popped->prev_ref->type)
						{
							popped->next_ref = token;
							token->prev_ref = popped;
						}
						else if (mirac_token_type_reserved_elif == popped->prev_ref->type)
						{
							popped->next_ref = token;
							token->prev_ref = popped;
						}
						else if (mirac_token_type_reserved_loop == popped->prev_ref->type)
						{
							popped->next_ref = token;
							token->prev_ref = popped;
							token->next_ref = popped->prev_ref;
						}
						else if (mirac_token_type_reserved_let == popped->prev_ref->type)
						{
							popped->next_ref = token;
							token->prev_ref = popped;
							token->next_ref = popped->prev_ref;
						}
						else if (mirac_token_type_reserved_reg == popped->prev_ref->type)
						{
							popped->next_ref = token;
							token->prev_ref = popped;
							token->next_ref = popped->prev_ref;
						}
						else
						{
							log_parser_error_and_exit(token->location,
								"encountered invalid keyword '" mirac_sv_fmt "' before the 'do' keyword!",
								mirac_sv_arg(token->text)
							);
						}
					}
					else if (mirac_token_type_reserved_else == popped->type)
					{
						popped->next_ref = token;
						token->prev_ref = popped;
					}
					else
					{
						log_parser_error_and_exit(token->location,
							"'" mirac_sv_fmt "' keyword must follow 'if <condition> do <block>', 'if <condition> do <block> else <block>', 'while <condition> do <block>', or 'let <identifiers...> do <block> end' sequence.",
							mirac_sv_arg(token->text)
						);
					}
				} break;

				case mirac_token_type_reserved_as:
				{
					mirac_tokens_refs_vector_push(&stack, token);
				} break;

				default:
				{
					if (mirac_token_is_type_keyword(token))
					{
						mirac_token_s* popped = NULL;

						if (!mirac_tokens_refs_vector_pop(&stack, &popped))
						{
							log_parser_error_and_exit(token->location,
								"'" mirac_sv_fmt "' type specifier must follow 'as' keyword in the function's body.",
								mirac_sv_arg(token->text)
							);
						}
					}
				} break;
			}
		}

		mirac_tokens_refs_vector_destroy(&stack);
	}
}

#if 0
mirac_unit_s mirac_unit_from_parts(
	mirac_arena_s* const arena)
{
	mirac_debug_assert(arena != NULL);
	mirac_unit_s unit = {0};
	unit.globals = mirac_ast_tokens_vector_from_parts(arena, 4);
	unit.strings = mirac_ast_tokens_refs_vector_from_parts(arena, 4);
	return unit;
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

mirac_unit_s mirac_parser_parse_unit(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_unit_s unit = mirac_unit_from_parts(parser->arena);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex_next(parser->lexer, &token)))
	{
		// TODO: remove(1):
		mirac_logger_debug(mirac_sv_fmt, mirac_sv_arg(mirac_token_to_string_view(&token)));
	}

	return unit;
}
#endif
