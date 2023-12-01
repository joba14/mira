
/**
 * @file config.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-01
 */

#include <mirac/config.h>

#include <mirac/version.h>
#include <mirac/debug.h>
#include <mirac/logger.h>
#include <mirac/utils.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <getopt.h>

static const char* g_architectures[] =
{
	"nasm_linux_x86_64",
	"fasm_linux_x86_64",
	"nasm_windows_x86_64",
	"fasm_windows_x86_64"
};

static const char* const g_usage_banner =
	"usage: %s [options] <files...>\n"
	"\n"
	"options:\n"
	"    -h, --help                 print the help message\n"
	"    -v, --version              print version and exit\n"
	"    -l, --list-archs           list the supported architectures\n"
	"    -a, --arch <target>        set the architecture for the output\n"
	"    -e, --entry <symbol>       set the entry symbol\n"
	"    -o, --output <path>        set output file name\n"
	"\n"
	"notice:\n"
	"    this executable is distributed under the \"prime gplv1\" license.\n";

static void usage(
	const char* const program);

static void architectures(
	void);

mirac_config_s mirac_config_from_cli(
	const int32_t argc,
	const char** const argv,
	uint64_t* const config_end_index)
{
	mirac_debug_assert(argc > 0);
	mirac_debug_assert(argv != NULL);
	mirac_debug_assert(config_end_index != NULL);

	typedef struct option option_s;
	static const option_s options[] =
	{
		{ "help",       no_argument,       0, 'h' },
		{ "version",    no_argument,       0, 'v' },
		{ "list-archs", no_argument,       0, 'l' },
		{ "arch",       required_argument, 0, 'a' },
		{ "entry",      required_argument, 0, 'e' },
		{ "output",     required_argument, 0, 'o' },
		{ 0, 0, 0, 0 }
	};

	mirac_config_s config = {0};
	int32_t opt = -1;

	while ((opt = (int32_t)getopt_long(argc, (char* const *)argv, "hvla:e:o:", options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'h':
			{
				usage(argv[0]);
				exit(0);
			} break;

			case 'v':
			{
				mirac_logger_log("%s " mirac_version_fmt, argv[0], mirac_version_arg);
				exit(0);
			} break;

			case 'l':
			{
				architectures();
				exit(0);
			} break;

			case 'a':
			{
				config.arch = (const char*)optarg;
			} break;

			case 'e':
			{
				config.entry = (const char*)optarg;
			} break;

			case 'o':
			{
				config.output = (const char*)optarg;
			} break;

			default:
			{
				mirac_logger_error("invalid command line option -- see '--help'.");
				exit(-1);
			} break;
		}
	}

	if (NULL == config.arch)
	{
		mirac_logger_error("no architecture was provided -- see '--help'.");
		exit(-1);
	}
	else
	{
		bool valid_architecture = false;

		for (uint8_t index = 0; index < (sizeof(g_architectures) / sizeof(g_architectures[0])); ++index)
		{
			if (mirac_utils_strcmp(config.arch, g_architectures[index]) == 0)
			{
				valid_architecture = true;
				break;
			}
		}

		if (!valid_architecture)
		{
			mirac_logger_error("invalid architecture '%s' was provided -- see '--list-archs'.", config.arch);
			exit(-1);
		}
	}

	// If entry symbol is not provided, default to 'main'.
	if (NULL == config.entry)
	{
		config.entry = "main";
	}

	*config_end_index = (uint64_t)optind;
	return config;
}

static void usage(
	const char* const program)
{
	mirac_debug_assert(program != NULL);
	mirac_logger_log(g_usage_banner, program);
}

static void architectures(
	void)
{
	mirac_logger_log("architectures:");

	for (uint8_t index = 0; index < (sizeof(g_architectures) / sizeof(g_architectures[0])); ++index)
	{
		mirac_logger_log(" - %s", g_architectures[index]);
	}
}
