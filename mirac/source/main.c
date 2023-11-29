
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

#include <mirac/version.h>
#include <mirac/debug.h>
#include <mirac/logger.h>
#include <mirac/lexer.h>
#include <mirac/parser.h>

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

// TODO: add supported architectures
static const char* const g_usage_banner =
	"usage: %s [options] <files...>\n"
	"\n"
	"options:\n"
	"    -h, --help                 print the help message\n"
	"    -v, --version              print version and exit\n"
	"    -a, --arch <target>        set the architecture for the output\n"
	"    -e, --entry <symbol>       set the entry symbol\n"
	"    -o, --output <path>        set output file name\n"
	"\n"
	"notice:\n"
	"    this executable is distributed under the \"prime gplv1\" license.\n";

static void usage(
	const char* const program);

static int32_t parse_command_line(
	const int32_t argc,
	const char** const argv,
	const char** const arch,
	const char** const entry,
	const char** const output);

static FILE* validate_and_open_file_for_reading(
	const char* const file_path);

int32_t main(
	const int32_t argc,
	const char** const argv)
{
	const char* arch = NULL;
	const char* entry = "main";
	const char* output = NULL;

	const int32_t options_index = parse_command_line(argc, argv, &arch, &entry, &output);
	if (options_index <= 0) { return options_index; }

	// TODO: check provided architecture
	if (NULL == arch)
	{
		mirac_logger_error("no architecture was provided -- see '--help'.");
		return -1;
	}

	const char** const source_files = argv + (uint64_t)options_index;
	const uint64_t source_files_count = (uint64_t)argc - (uint64_t)options_index;

	if (source_files_count <= 0)
	{
		mirac_logger_error("no source files were provided -- see '--help'.");
		return -1;
	}

	for (uint64_t source_file_index = 0; source_file_index < source_files_count; ++source_file_index)
	{
		const char* const source_file_path = source_files[source_file_index];
		mirac_debug_assert(source_file_path != NULL);

		const uint64_t source_file_path_length = strlen(source_file_path);
		mirac_debug_assert(source_file_path_length > 0);

		FILE* const source_file = validate_and_open_file_for_reading(source_file_path);
		if (!source_file) { continue; }

		mirac_lexer_s lexer = mirac_lexer_from_parts(source_file_path, source_file);
		mirac_parser_s parser = mirac_parser_from_parts(&lexer);
		mirac_globals_vector_s globals = mirac_parser_parse(&parser);

		for (uint64_t global_index = 0; global_index < globals.count; ++global_index)
		{
			mirac_logger_debug("[%lu] %s\n", global_index,
				mirac_global_to_string(&globals.data[global_index])
			);
		}

		(void)fclose(source_file);
	}

	return 0;
}

static void usage(
	const char* const program)
{
	mirac_debug_assert(program != NULL);
	mirac_logger_log(g_usage_banner, program);
}

static int32_t parse_command_line(
	const int32_t argc,
	const char** const argv,
	const char** const arch,
	const char** const entry,
	const char** const output)
{
	mirac_debug_assert(argv != NULL);
	mirac_debug_assert(arch != NULL);
	mirac_debug_assert(entry != NULL);
	mirac_debug_assert(output != NULL);

	typedef struct option option_s;
	static const option_s options[] =
	{
		{ "help", no_argument, 0, 'h' },
		{ "version", no_argument, 0, 'v' },
		{ "arch", required_argument, 0, 'a' },
		{ "entry", required_argument, 0, 'e' },
		{ "output", required_argument, 0, 'o' },
		{ 0, 0, 0, 0 }
	};

	int32_t opt = -1;
	while ((opt = (int32_t)getopt_long(argc, (char* const *)argv, "hva:e:o:", options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'h':
			{
				usage(argv[0]);
				return 0;
			} break;

			case 'v':
			{
				mirac_logger_log("%s " mirac_version_fmt, argv[0], mirac_version_arg);
				return 0;
			} break;

			case 'a':
			{
				*arch = (const char*)optarg;
			} break;

			case 'e':
			{
				*entry = (const char*)optarg;
			} break;

			case 'o':
			{
				*output = (const char*)optarg;
			} break;

			default:
			{
				mirac_logger_error("invalid command line option -- see '--help'.");
				return -1;
			} break;
		}
	}

	return (int32_t)optind;
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
