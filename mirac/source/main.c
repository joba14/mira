
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
#include <mirac/checker.h>

int32_t main(
	const int32_t argc,
	const char** const argv)
{
	uint64_t options_index;
	mirac_config_s config = mirac_config_from_cli(argc, argv, &options_index);
	const char** const source_files = argv + options_index;
	const uint64_t source_files_count = (uint64_t)argc - options_index;

	if (source_files_count <= 0)
	{
		mirac_logger_error("no source files were provided.");
		mirac_config_usage();
		return -1;
	}

	for (uint64_t source_file_index = 0; source_file_index < source_files_count; ++source_file_index)
	{
		const char* const source_file_pointer = source_files[source_file_index];
		mirac_debug_assert(source_file_pointer != NULL);
		const mirac_string_view_s source_file_path = mirac_string_view_from_cstring(source_file_pointer);

		mirac_arena_s arena = mirac_arena_from_parts();
		mirac_lexer_s lexer = mirac_lexer_from_file_path(&config, &arena, source_file_path);
		mirac_parser_s parser = mirac_parser_from_parts(&config, &arena, &lexer);
		mirac_ast_unit_s ast_unit = mirac_parser_parse_ast_unit(&parser);

		// TODO: remove:
		// [
			for (uint64_t block_index = 0; block_index < ast_unit.blocks.count; ++block_index)
			{
				mirac_ast_block_print(&ast_unit.blocks.data[block_index], 0);
			}
		// ]

		// TODO: implement the checker!
		mirac_checker_s checker = mirac_checker_from_parts();
		(void)checker;

		mirac_arena_destroy(&arena);
	}

	return 0;
}
