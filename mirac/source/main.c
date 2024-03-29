
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

#include <mirac/c_common.h>
#include <mirac/logger.h>
#include <mirac/config.h>
#include <mirac/arena.h>
#include <mirac/lexer.h>
#include <mirac/parser.h>
#include <mirac/compiler.h>

#include <sys/stat.h>
#include <errno.h>

/**
 * @brief Validate file path and open file for reading.
 * 
 * @param file_path path of the file to be opened
 * 
 * @return mirac_file_t*
 */
static mirac_file_t* validate_and_open_file_for_reading(
	const mirac_string_view_s file_path);

/**
 * @brief Validate file path and open file for writing.
 * 
 * @param file_path path of the file to be opened
 * 
 * @return mirac_file_t*
 */
static mirac_file_t* validate_and_open_file_for_writing(
	const mirac_string_view_s file_path);

// todo: document!
// todo: write unit tests!
static void process_source_file_into_output_file(
	const mirac_string_view_s source_file_path,
	const mirac_string_view_s output_file_path,
	mirac_config_s* const config);

int32_t main(
	const int32_t argc,
	const char_t** const argv)
{
	mirac_debug_assert(argc > 0);
	mirac_debug_assert(argv != mirac_null);

	uint64_t options_index = 0;
	mirac_config_s config = mirac_config_from_cli(argc, argv, &options_index);

	const char_t** const source_files = argv + options_index;
	mirac_debug_assert(source_files != mirac_null);
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
		const char_t* const source_file_pointer = source_files[source_file_index + 0];
		mirac_debug_assert(source_file_pointer != mirac_null);

		const char_t* const output_file_pointer = source_files[source_file_index + 1];
		mirac_debug_assert(output_file_pointer != mirac_null);

		const mirac_string_view_s source_file_path = mirac_string_view_from_cstring(source_file_pointer);
		const mirac_string_view_s output_file_path = mirac_string_view_from_cstring(output_file_pointer);

		process_source_file_into_output_file(source_file_path, output_file_path, &config);
	}

	return 0;
}

static mirac_file_t* validate_and_open_file_for_reading(
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

	mirac_file_t* const file = fopen(file_path.data, "rt");

	if (mirac_null == file)
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for reading -- failed to open.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	return file;
}

static mirac_file_t* validate_and_open_file_for_writing(
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
			} break;
		}
	}

	if (S_ISDIR(file_stats.st_mode))
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for writing -- it is a directory.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	mirac_file_t* const file = fopen(file_path.data, "wt");

	if (mirac_null == file)
	{
		mirac_logger_error("unable to open " mirac_sv_fmt " for writing -- failed to open.", mirac_sv_arg(file_path));
		mirac_c_exit(-1);
	}

	return file;
}

static void process_source_file_into_output_file(
	const mirac_string_view_s source_file_path,
	const mirac_string_view_s output_file_path,
	mirac_config_s* const config)
{
	mirac_debug_assert(config != mirac_null);

	mirac_file_t* const source_file = validate_and_open_file_for_reading(source_file_path);
	mirac_debug_assert(source_file != mirac_null);

	mirac_file_t* const output_file = validate_and_open_file_for_writing(output_file_path);
	mirac_debug_assert(output_file != mirac_null);

	mirac_arena_s arena = mirac_arena_from_parts();
	mirac_lexer_s lexer = mirac_lexer_from_parts(config, &arena, source_file_path, source_file);
	mirac_parser_s parser = mirac_parser_from_parts(config, &arena, &lexer);
	mirac_ast_unit_s unit = mirac_parser_parse_ast_unit(&parser);

	if (config->dump_ast)
	{
		// todo: rework this to be safe:
		// [
		char_t ast_dump_file_path[256] = {0};
		uint64_t ast_dump_file_path_length = 0;

		mirac_c_memcpy(ast_dump_file_path + ast_dump_file_path_length, output_file_path.data, output_file_path.length);
		ast_dump_file_path_length += output_file_path.length;

		mirac_c_memcpy(ast_dump_file_path + ast_dump_file_path_length, ".ast.txt", 8);
		ast_dump_file_path_length += 8;

		mirac_file_t* const ast_dump_file = validate_and_open_file_for_writing(
			mirac_string_view_from_parts(ast_dump_file_path, ast_dump_file_path_length));
		mirac_debug_assert(ast_dump_file != mirac_null);

		mirac_ast_unit_print(ast_dump_file, &unit, 0);
		(void)fclose(ast_dump_file);
		// ]
	}

	if (!config->unsafe)
	{
		// todo: implement checker and use it here!
	}

	mirac_compiler_s compiler = mirac_compiler_from_parts(config, &arena, &unit, output_file);
	mirac_compiler_compile_ast_unit(&compiler);

	mirac_arena_destroy(&arena);
	(void)fclose(source_file);
	(void)fclose(output_file);
}
