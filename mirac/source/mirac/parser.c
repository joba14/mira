
/**
 * @file parser.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#include <mirac/parser.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

#include <stddef.h>

mirac_implement_vector_type(mirac_tokens_vector, mirac_token_s);
mirac_implement_vector_type(mirac_token_refs_vector, mirac_token_s*);
mirac_implement_vector_type(mirac_globals_vector, mirac_global_s);

#define log_parser_error_and_exit(_location, _format, ...)                     \
	do {                                                                       \
		(void)fprintf(stderr, "%s:%lu:%lu: ",                                  \
			(_location).file, (_location).line, (_location).column);           \
		mirac_logger_error(_format, ## __VA_ARGS__);                           \
		exit(-1);                                                              \
	} while (0)

static mirac_global_function_s try_parse_function(
	mirac_parser_s* const parser);

static mirac_global_memory_s try_parse_memory(
	mirac_parser_s* const parser);

static void collect_string_literals(
	mirac_unit_s* const unit);

static void perform_cross_reference(
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
	const mirac_global_s* const global)
{
	mirac_debug_assert(global != NULL);
	(void)printf("Global[type='%s'", mirac_global_type_to_string(global->type));

	switch (global->type)
	{
		case mirac_global_type_function:
		{
			(void)printf("\n    ident: '");
				mirac_token_print(&global->as_function.identifier);
			(void)printf("'\n");

			(void)printf("    req (%lu):\n", global->as_function.req_tokens.count);

			for (uint64_t index = 0; index < global->as_function.req_tokens.count; ++index)
			{
				(void)printf("        [%lu]: '", index);
				mirac_token_print(&global->as_function.req_tokens.data[index]);
				(void)printf("'\n");
			}

			(void)printf("    ret (%lu):\n", global->as_function.ret_tokens.count);

			for (uint64_t index = 0; index < global->as_function.ret_tokens.count; ++index)
			{
				(void)printf("        [%lu]: '", index);
				mirac_token_print(&global->as_function.ret_tokens.data[index]);
				(void)printf("'\n");
			}

			(void)printf("    body (%lu):\n", global->as_function.body_tokens.count);

			for (uint64_t index = 0; index < global->as_function.body_tokens.count; ++index)
			{
				(void)printf("        [%lu]: '", index);
				mirac_token_print(&global->as_function.body_tokens.data[index]);
				(void)printf("'\n");
			}

			(void)printf("    inlined: %s\n", (global->as_function.is_inlined ? "yes" : "no"));
			(void)printf("    entry: %s\n]", (global->as_function.is_entry ? "yes" : "no"));
		} break;

		case mirac_global_type_memory:
		{
			(void)printf("\n    ident: '");
			mirac_token_print(&global->as_memory.identifier);
			(void)printf("'\n");
			(void)printf("    capacity: '");
			mirac_token_print(&global->as_memory.capacity);
			(void)printf("'\n]");
		} break;

		default:
		{
			mirac_debug_assert(0);
		} break;
	}
}

mirac_parser_s mirac_parser_from_parts(
	mirac_config_s* const config,
	mirac_lexer_s* const lexer)
{
	mirac_debug_assert(config != NULL);
	mirac_debug_assert(lexer != NULL);

	mirac_parser_s parser;
	parser.config = config;
	parser.lexer = lexer;
	return parser;
}

mirac_unit_s mirac_parser_parse(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_unit_s unit;
	unit.globals = mirac_globals_vector_from_parts(4);
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
	{
		switch (token.type)
		{
			case mirac_token_type_keyword_inl:
			case mirac_token_type_keyword_func:
			{
				mirac_lexer_unlex(parser->lexer, &token);
				const mirac_global_function_s function = try_parse_function(parser);
				const mirac_global_s global = { .type = mirac_global_type_function, .as_function = function };
				mirac_globals_vector_push(&unit.globals, global);
			} break;

			case mirac_token_type_keyword_mem:
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

	collect_string_literals(&unit);
	perform_cross_reference(&unit);
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

	if (mirac_token_type_keyword_inl == token.type)
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

	if (token.type != mirac_token_type_keyword_func)
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
		function.is_entry = (mirac_utils_strcmp(function.identifier.as_ident.data, parser->config->entry) == 0);

		if (function.is_entry && function.is_inlined)
		{
			log_parser_error_and_exit(token.location,
				"entry function `%.*s` cannot be inlined.",
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

	if (mirac_token_type_keyword_req == token.type)
	{
		function.req_tokens = mirac_tokens_vector_from_parts(4);

		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
		{
			if (mirac_token_is_type_keyword(&token))
			{
				mirac_tokens_vector_push(&function.req_tokens, token);
			}
			else if (mirac_token_type_keyword_ret == token.type ||
					 mirac_token_type_keyword_do == token.type)
			{
				if (function.req_tokens.count <= 0)
				{
					log_parser_error_and_exit(token.location,
						"no type specifiers we found after 'req' keyword."
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
					"invalid token `%.*s` encountered instead of a type, 'ret', or 'do' keywords.",
					(signed int)token.source.length, token.source.data
				);
			}
		}
	}

	if (mirac_token_type_keyword_ret == token.type)
	{
		function.ret_tokens = mirac_tokens_vector_from_parts(4);

		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
		{
			if (mirac_token_is_type_keyword(&token))
			{
				mirac_tokens_vector_push(&function.ret_tokens, token);
			}
			else if (mirac_token_type_keyword_do == token.type)
			{
				if (function.ret_tokens.count <= 0)
				{
					log_parser_error_and_exit(token.location,
						"no type specifiers we found after 'ret' keyword."
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
					"invalid token `%.*s` encountered instead of a type, or 'do' keyword.",
					(signed int)token.source.length, token.source.data
				);
			}
		}
	}

	if (token.type != mirac_token_type_keyword_do)
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
				case mirac_token_type_keyword_inl:
				case mirac_token_type_keyword_func:
				case mirac_token_type_keyword_mem:
				{
					log_parser_error_and_exit(token.location,
						"invalid keyword '%.*s' encountered in function's body.",
						(signed int)token.source.length, token.source.data
					);
				} break;

				case mirac_token_type_keyword_if:
				{
					increase_scope_counter = true;
				} break;

				case mirac_token_type_keyword_loop:
				{
					increase_scope_counter = true;
				} break;

				case mirac_token_type_keyword_do:
				{
					if (increase_scope_counter)
					{
						increase_scope_counter = false;
						++scopes_counter;
					}
				} break;

				case mirac_token_type_keyword_end:
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
			"expected identifier token after 'mem' keyword, but found token '%.*s'.",
			(signed int)token.source.length, token.source.data
		);
	}
	else
	{
		memory.identifier = token;

		mirac_token_s temp_token = mirac_token_from_type(mirac_token_type_none);
		if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &temp_token)))
		{
			log_parser_error_and_exit(token.location,
				"missing capacity specifier after '%.*s' token.",
				(signed int)token.source.length, token.source.data
			);
		}
		else
		{
			token = temp_token;
		}
	}

	if (!mirac_token_is_unsigned_integer_literal(&token) && token.as_uval <= 0)
	{
		log_parser_error_and_exit(token.location,
			"unsigned capacity specifier '%.*s' must be higher than 0.",
			(signed int)token.source.length, token.source.data
		);
	}
	else if (!mirac_token_is_signed_integer_literal(&token) && token.as_ival <= 0)
	{
		log_parser_error_and_exit(token.location,
			"signed capacity specifier '%.*s' must be higher than 0.",
			(signed int)token.source.length, token.source.data
		);
	}
	else
	{
		memory.capacity = token;

		mirac_token_s temp_token = mirac_token_from_type(mirac_token_type_none);
		if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &temp_token)))
		{
			log_parser_error_and_exit(token.location,
				"missing capacity specifier after '%.*s' token.",
				(signed int)token.source.length, token.source.data
			);
		}
		else
		{
			token = temp_token;
		}
	}

	if (token.type != mirac_token_type_keyword_end)
	{
		log_parser_error_and_exit(token.location,
			"expected 'end' keyword after memorie's capacity token, but found token '%.*s'.",
			(signed int)token.source.length, token.source.data
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
				case mirac_token_type_literal_cstr:
				{
					mirac_tokens_vector_push(&unit->strings, *token);
				} break;

				default:
				{
				} break;
			}
		}
	}
}

static void perform_cross_reference(
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

		mirac_token_refs_vector_s stack = mirac_token_refs_vector_from_parts(64);

		for (uint64_t body_index = 0; body_index < function->body_tokens.count; ++body_index)
		{
			mirac_token_s* const token = &function->body_tokens.data[body_index];
			mirac_debug_assert(token != NULL);

			switch (token->type)
			{
				case mirac_token_type_keyword_if:
				case mirac_token_type_keyword_loop:
				{
					mirac_token_s* popped = NULL;

					if (mirac_token_refs_vector_pop(&stack, &popped))
					{
						if (mirac_token_type_keyword_if == popped->type ||
							mirac_token_type_keyword_loop == popped->type)
						{
							log_parser_error_and_exit(token->location,
								"encounetered an invalid token LALALALA"
							);
						}
					}

					mirac_token_refs_vector_push(&stack, token);
				} break;

				case mirac_token_type_keyword_elif:
				{
					mirac_token_s* popped = NULL;

					if (!mirac_token_refs_vector_pop(&stack, &popped))
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token ZAZAZAZA"
						);
					}

					if (NULL == popped->prev_ref)
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token XAXAXAXA"
						);
					}

					if (mirac_token_type_keyword_do == popped->type &&
						(mirac_token_type_keyword_if == popped->prev_ref->type ||
						 mirac_token_type_keyword_elif == popped->prev_ref->type))
					{
						popped->next_ref = token;
						token->prev_ref = popped;
						mirac_token_refs_vector_push(&stack, token);
					}
					else
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token SASASASA"
						);
					}
				} break;

				case mirac_token_type_keyword_else:
				{
					mirac_token_s* popped = NULL;

					if (!mirac_token_refs_vector_pop(&stack, &popped))
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token HAHAHAHA"
						);
					}

					if (NULL == popped->prev_ref)
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token KAKAKAKA"
						);
					}

					if (mirac_token_type_keyword_do == popped->type &&
						(mirac_token_type_keyword_if == popped->prev_ref->type ||
						 mirac_token_type_keyword_elif == popped->prev_ref->type))
					{
						popped->next_ref = token;
						token->prev_ref = popped;
						mirac_token_refs_vector_push(&stack, token);
					}
					else
					{
						// TODO: remove:
						mirac_token_print(popped);

						log_parser_error_and_exit(token->location,
							"encounetered an invalid token RARARARA"
						);
					}
				} break;

				case mirac_token_type_keyword_do:
				{
					mirac_token_s* popped = NULL;

					if (!mirac_token_refs_vector_pop(&stack, &popped))
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token BABABABA"
						);
					}

					if (mirac_token_type_keyword_if == popped->type ||
						mirac_token_type_keyword_elif == popped->type ||
						mirac_token_type_keyword_loop == popped->type)
					{
						popped->next_ref = token;
						token->prev_ref = popped;
						mirac_token_refs_vector_push(&stack, token);
					}
					else
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token QAQAQAQA"
						);
					}
				} break;

				case mirac_token_type_keyword_end:
				{
					mirac_token_s* popped = NULL;

					if (!mirac_token_refs_vector_pop(&stack, &popped))
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token BABABABA"
						);
					}

					if (mirac_token_type_keyword_do == popped->type)
					{
						if (mirac_token_type_keyword_if == popped->prev_ref->type)
						{
							popped->next_ref = token;
							token->prev_ref = popped;
						}
						else if (mirac_token_type_keyword_loop == popped->prev_ref->type)
						{
							popped->next_ref = token;
							token->prev_ref = popped;
							token->next_ref = popped->prev_ref;
						}
						else
						{
							log_parser_error_and_exit(token->location,
								"encounetered an invalid token GAGAGAGA"
							);
						}
					}
					else if (mirac_token_type_keyword_else == popped->type)
					{
						popped->next_ref = token;
						token->prev_ref = popped;
					}
					else
					{
						log_parser_error_and_exit(token->location,
							"encounetered an invalid token JAJAJAJA"
						);
					}
				} break;

				default:
				{
				} break;
			}

			/*
			mirac_logger_debug("Stack:");
			mirac_logger_debug("  count=%lu", stack.count);
			mirac_logger_debug("  capacity=%lu", stack.capacity);
			for (uint64_t stack_index = 0; stack_index < stack.count; ++stack_index)
			{
				mirac_token_s* const stack_token = stack.data[stack_index];
				mirac_logger_debug("  [%lu]: %s", stack_index, mirac_token_to_string(stack_token));
			}
			mirac_logger_debug("--- ");
			getchar();
			*/
		}

		mirac_token_refs_vector_destroy(&stack);
	}
}
