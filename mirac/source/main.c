
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
#include <mirac/timer.h>
#include <mirac/config.h>
#include <mirac/arena.h>
#include <mirac/lexer.h>
#include <mirac/parser.h>
#include <mirac/checker.h>
#include <mirac/compiler.h>

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

		const mirac_seconds_t start_time = mirac_timer_get_time_in_seconds();
		{
			mirac_lexer_s lexer = mirac_lexer_from_file_path(&config, &arena, source_file_path);
			mirac_parser_s parser = mirac_parser_from_parts(&config, &arena, &lexer);
			mirac_ast_unit_s unit = mirac_parser_parse_ast_unit(&parser);

			// TODO: implement the checker!
			mirac_checker_s checker = mirac_checker_from_parts(&config, &arena, &unit);
			mirac_checker_type_check_ast_unit(&checker);

			// TODO: implement the compiler!
			mirac_compiler_s compiler = mirac_compiler_from_parts(&config, &arena, &unit);
			mirac_compiler_compile_ast_unit(&compiler);

			// TODO: remove:
			mirac_ast_unit_print(&unit, 0);
		}
		const mirac_seconds_t elapsed_time = mirac_timer_get_elapsed_time_in_seconds(start_time);

		mirac_logger_info("file '" mirac_sv_fmt "' was compiled in %.3Lf secs.", mirac_sv_arg(source_file_path), elapsed_time);
		mirac_arena_destroy(&arena);
	}

	return 0;
}
