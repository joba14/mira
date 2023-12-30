
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

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>

/**
 * @brief Validate file path and open file for reading.
 * 
 * @param file_path[in] path of the file to be opened
 * 
 * @return FILE*
 */
static FILE* validate_and_open_file_for_reading(
	const mirac_string_view_s file_path);

/**
 * @brief Validate file path and open file for writing.
 * 
 * @param file_path[in] path of the file to be opened
 * 
 * @return FILE*
 */
static FILE* validate_and_open_file_for_writing(
	const mirac_string_view_s file_path);

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

	if (source_files_count % 2 != 0)
	{
		mirac_logger_error("missing output file path for source file '%s'.", source_files[source_files_count - 1]);
		mirac_config_usage();
		return -1;
	}

	for (uint64_t source_file_index = 0; source_file_index < source_files_count; source_file_index += 2)
	{
		const char* const source_file_pointer = source_files[source_file_index + 0];
		const char* const output_file_pointer = source_files[source_file_index + 1];
		mirac_debug_assert(source_file_pointer != NULL);
		mirac_debug_assert(output_file_pointer != NULL);

		const mirac_string_view_s source_file_path = mirac_string_view_from_cstring(source_file_pointer);
		const mirac_string_view_s output_file_path = mirac_string_view_from_cstring(output_file_pointer);
		mirac_arena_s arena = mirac_arena_from_parts();

		FILE* const source_file = validate_and_open_file_for_reading(source_file_path);
		FILE* const output_file = validate_and_open_file_for_writing(output_file_path);
		mirac_debug_assert(source_file != NULL);
		mirac_debug_assert(output_file != NULL);

		const mirac_seconds_t start_time = mirac_timer_get_time_in_seconds();
		{
			mirac_lexer_s lexer = mirac_lexer_from_parts(&config, &arena, source_file_path, source_file);
			mirac_parser_s parser = mirac_parser_from_parts(&config, &arena, &lexer);
			mirac_ast_unit_s unit = mirac_parser_parse_ast_unit(&parser);

			// TODO: remove:
			getchar();
			mirac_ast_unit_print(&unit, 0);
			getchar();

			// TODO: implement the checker!
			// mirac_checker_s checker = mirac_checker_from_parts(&config, &arena, &unit);
			// mirac_checker_type_check_ast_unit(&checker);

			// TODO: implement the compiler!
			// mirac_compiler_s compiler = mirac_compiler_from_parts(&config, &arena, &unit, output_file);
			// mirac_compiler_compile_ast_unit(&compiler);
		}
		const mirac_seconds_t elapsed_time = mirac_timer_get_elapsed_time_in_seconds(start_time);
		mirac_logger_info("file '" mirac_sv_fmt "' was compiled in %.3Lf secs.", mirac_sv_arg(source_file_path), elapsed_time);

		mirac_arena_destroy(&arena);
		fclose(source_file);
		fclose(output_file);
	}

	return 0;
}

static FILE* validate_and_open_file_for_reading(
	const mirac_string_view_s file_path)
{
	typedef struct stat file_stats_s;
	file_stats_s file_stats = {0};

	if (stat(file_path.data, &file_stats) != 0)
	{
		switch (errno)
		{
			case ENOENT:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- file not found.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			case EACCES:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- permission denied.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			case ENAMETOOLONG:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- path name exceeds the system-defined maximum length.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			default:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- failed to stat.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;
		}
	}

	if (S_ISDIR(file_stats.st_mode))
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- it is a directory.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	FILE* const file = fopen(file_path.data, "rt");

	if (NULL == file)
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- failed to open.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	return file;
}

static FILE* validate_and_open_file_for_writing(
	const mirac_string_view_s file_path)
{
	typedef struct stat file_stats_s;
	file_stats_s file_stats = {0};

	if (stat(file_path.data, &file_stats) != 0)
	{
		switch (errno)
		{
			case EACCES:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for writing -- permission denied.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			case ENAMETOOLONG:
			{
				mirac_logger_error("unable to open " mirac_sv_fmt " for writing -- path name exceeds the system-defined maximum length.", mirac_sv_arg(file_path));
				mirac_c_exit(-1);
			} break;

			default:
			{
				// mirac_logger_error("unable to open " mirac_sv_fmt " for writing -- failed to stat.", mirac_sv_arg(file_path));
				// mirac_c_exit(-1);
			} break;
		}
	}

	if (S_ISDIR(file_stats.st_mode))
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for writing -- it is a directory.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	FILE* const file = fopen(file_path.data, "wt");

	if (NULL == file)
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for writing -- failed to open.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	return file;
}
