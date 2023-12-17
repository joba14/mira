
/**
 * @file main.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#include <main.h>

#include <mirac/logger.h>
#include <mirac/config.h>
#include <mirac/arena.h>
#include <mirac/lexer.h>
#include <mirac/parser.h>

int32_t main(
	const int32_t argc,
	const char** const argv)
{
	// Parsing and validating config from the cli.
	uint64_t options_index;
	mirac_config_s config = mirac_config_from_cli(argc, argv, &options_index);

	// Getting the pointer to the source files sequence.
	const char** const source_files = argv + options_index;
	const uint64_t source_files_count = (uint64_t)argc - options_index;

	if (source_files_count <= 0)
	{
		mirac_logger_error("no source files were provided.");
		mirac_config_usage();
		return -1;
	}

	// Iterating over the source files.
	for (uint64_t source_file_index = 0; source_file_index < source_files_count; ++source_file_index)
	{
		const mirac_string_view_s source_file_path = mirac_string_view_from_cstring(source_files[source_file_index]);

		mirac_arena_s arena = mirac_arena_from_parts();
		mirac_lexer_s lexer = mirac_lexer_from_parts(&config, &arena, source_file_path);
		mirac_parser_s parser = mirac_parser_from_parts(&config, &arena, &lexer);
		mirac_parser_preview_all(&parser);
		mirac_arena_destroy(&arena);
	}

	return 0;
}
