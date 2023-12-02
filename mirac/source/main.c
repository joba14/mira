
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

#include <mirac/debug.h>
#include <mirac/logger.h>
#include <mirac/global_arena.h>
#include <mirac/config.h>
#include <mirac/lexer.h>
#include <mirac/parser.h>
#include <mirac/checker.h>

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>

static FILE* validate_and_open_file_for_reading(
	const char* const file_path);

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
		mirac_logger_error("no source files were provided -- see '--help'.");
		return -1;
	}

	mirac_global_arena_create();

	// Iterating over the source files.
	for (uint64_t source_file_index = 0; source_file_index < source_files_count; ++source_file_index)
	{
		const char* const source_file_path = source_files[source_file_index];
		mirac_debug_assert(source_file_path != NULL);

		const uint64_t source_file_path_length = strlen(source_file_path);
		mirac_debug_assert(source_file_path_length > 0);

		FILE* const source_file = validate_and_open_file_for_reading(source_file_path);
		if (!source_file) { continue; } // NOTE: The failed to open file is logged ~~^

		mirac_lexer_s lexer = mirac_lexer_from_parts(source_file_path, source_file);
		mirac_parser_s parser = mirac_parser_from_parts(&config, &lexer);
		mirac_unit_s unit = mirac_parser_parse(&parser);

		// TODO: remove:
		// [
		for (uint64_t global_index = 0; global_index < unit.globals.count; ++global_index)
		{
			mirac_global_print(&unit.globals.data[global_index]);
		}

		for (uint64_t string_index = 0; string_index < unit.strings.count; ++string_index)
		{
			mirac_token_print(unit.strings.data[string_index]);
		}
		// ]

		mirac_global_arena_destroy();

		(void)fclose(source_file);
	}

	return 0;
}

static FILE* validate_and_open_file_for_reading(
	const char* const file_path)
{
	typedef struct stat file_stats_s;
	file_stats_s file_stats = {0};

	if (stat(file_path, &file_stats) != 0)
	{
		switch (errno)
		{
			case ENOENT:
			{
				mirac_logger_error("unable to open %s for reading -- file not found.", file_path);
			} break;

			case EACCES:
			{
				mirac_logger_error("unable to open %s for reading -- permission denied.", file_path);
			} break;

			case ENAMETOOLONG:
			{
				mirac_logger_error("unable to open %s for reading -- path name exceeds the system-defined maximum length.", file_path);
			} break;

			default:
			{
				mirac_logger_error("unable to open %s for reading -- failed to stat.", file_path);
			} break;
		}

		return NULL;
	}

	if (S_ISDIR(file_stats.st_mode))
	{
		mirac_logger_error("unable to open %s for reading -- it is a directory.", file_path);
		return NULL;
	}

	FILE* const file = fopen(file_path, "rt");

	if (NULL == file)
	{
		mirac_logger_error("unable to open %s for reading -- failed to open.", file_path);
		return NULL;
	}

	return file;
}
