
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

static mirac_global_string_s try_parse_string(
	mirac_parser_s* const parser);

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

		case mirac_global_type_string:
		{
			return "string";
		} break;

		default:
		{
			mirac_debug_assert(0);
			return NULL;
		} break;
	}
}

const char* mirac_global_to_string(
	const mirac_global_s* const global)
{
	mirac_debug_assert(global != NULL);
	#define global_string_buffer_capacity 4096
	static char global_string_buffer[global_string_buffer_capacity + 1];
	global_string_buffer[0] = 0;

	uint64_t written = (uint64_t)snprintf(
		global_string_buffer, global_string_buffer_capacity,
		"Global[type='%s'", mirac_global_type_to_string(global->type)
	);

	switch (global->type)
	{
		case mirac_global_type_function:
		{
			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"\n    ident: '%s'\n", mirac_token_to_string(&global->function.identifier)
			);

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"    req (%lu):\n", global->function.req_tokens.count
			);

			for (uint64_t index = 0; index < global->function.req_tokens.count; ++index)
			{
				written += (uint64_t)snprintf(
					global_string_buffer + written, global_string_buffer_capacity - written,
					"        [%lu]: '%s'\n", index, mirac_token_to_string(&global->function.req_tokens.data[index])
				);
			}

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"    ret (%lu):\n", global->function.ret_tokens.count
			);

			for (uint64_t index = 0; index < global->function.ret_tokens.count; ++index)
			{
				written += (uint64_t)snprintf(
					global_string_buffer + written, global_string_buffer_capacity - written,
					"        [%lu]: '%s'\n", index, mirac_token_to_string(&global->function.ret_tokens.data[index])
				);
			}

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"    body (%lu):\n", global->function.body_tokens.count
			);

			for (uint64_t index = 0; index < global->function.body_tokens.count; ++index)
			{
				written += (uint64_t)snprintf(
					global_string_buffer + written, global_string_buffer_capacity - written,
					"        [%lu]: '%s'\n", index, mirac_token_to_string(&global->function.body_tokens.data[index])
				);
			}

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"    inlined: %s\n", (global->function.is_inlined ? "yes" : "no")
			);

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"    entry: %s\n", (global->function.is_entry ? "yes" : "no")
			);

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written, "]"
			);
		} break;

		case mirac_global_type_memory:
		{
			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"\n    ident: '%s'\n", mirac_token_to_string(&global->memory.identifier)
			);

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"    capacity: '%s'\n", mirac_token_to_string(&global->memory.capacity)
			);

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written, "]"
			);
		} break;

		case mirac_global_type_string:
		{
			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written,
				"\n    literal: '%s'\n", mirac_token_to_string(&global->string.literal)
			);

			written += (uint64_t)snprintf(
				global_string_buffer + written, global_string_buffer_capacity - written, "]"
			);
		} break;

		default:
		{
			mirac_debug_assert(0);
			return NULL;
		} break;
	}

	global_string_buffer[written] = 0;
	return global_string_buffer;
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
				const mirac_global_s global = { .type = mirac_global_type_function, .function = function };
				mirac_globals_vector_append(&unit.globals, &global);
			} break;

			case mirac_token_type_keyword_mem:
			{
				mirac_lexer_unlex(parser->lexer, &token);
				const mirac_global_memory_s memory = try_parse_memory(parser);
				const mirac_global_s global = { .type = mirac_global_type_memory, .memory = memory };
				mirac_globals_vector_append(&unit.globals, &global);
			} break;

			// NOTE: The string literals should only be found in unit.globals, and
			//       therefore, they are not check for here.

			default:
			{
				log_parser_error_and_exit(token.location,
					"invalid global token '%.*s' encountered.",
					(signed int)token.source.length, token.source.data
				);
			} break;
		}
	}

	return unit;
}

static mirac_global_function_s try_parse_function(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	mirac_global_function_s function = {0};
	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);

	if (mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
	{
		// TODO: assert?
	}

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
		function.is_entry = (mirac_utils_strcmp(function.identifier.ident.data, parser->config->entry) == 0);

		if (function.is_entry && function.is_inlined)
		{
			log_parser_error_and_exit(token.location,
				"entry function `%.*s` cannot be inlined.",
				(signed int)function.identifier.ident.length, function.identifier.ident.data
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
				mirac_tokens_vector_append(&function.req_tokens, &token);
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
				mirac_tokens_vector_append(&function.ret_tokens, &token);
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

	if (mirac_token_type_keyword_do != token.type)
	{
		log_parser_error_and_exit(token.location,
			"expected 'do' keyword after type specifiers or function identifier, but found token '%.*s'.",
			(signed int)token.source.length, token.source.data
		);
	}
	else
	{
		function.body_tokens = mirac_tokens_vector_from_parts(4);
		int64_t scopes_counter = 1;

		while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(parser->lexer, &token)))
		{
			if (mirac_token_type_keyword_do == token.type)
			{
				++scopes_counter;
			}
			else if (mirac_token_type_keyword_end == token.type)
			{
				--scopes_counter;
			}

			mirac_tokens_vector_append(&function.body_tokens, &token);

			if (scopes_counter <= 0)
			{
				break;
			}
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

	(void)try_parse_string(parser);
	return (mirac_global_memory_s) {0};
}

static mirac_global_string_s try_parse_string(
	mirac_parser_s* const parser)
{
	mirac_debug_assert(parser != NULL);
	mirac_debug_assert(parser->lexer != NULL);

	return (mirac_global_string_s) {0};
}
