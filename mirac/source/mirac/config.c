
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

#include <getopt.h>

static mirac_string_view_s g_program = mirac_string_view_static("");

static mirac_string_view_s g_supported_architectures[] =
{
	mirac_string_view_static("nasm_x86_64_linux"),
	mirac_string_view_static("fasm_x86_64_linux")
};

static const char* const g_usage_banner =
	"usage: " mirac_sv_fmt " [options] <files...>\n"
	"\n"
	"options:\n"
	"    -h, --help                 print the help message\n"
	"    -v, --version              print version and exit\n"
	"    -a, --arch <target>        set the architecture for the output\n"
	"    -e, --entry <symbol>       set the entry symbol\n"
	"    -o, --output <path>        set output file name\n"
	"\n"
	"notice:\n"
	"    this executable is distributed under the \"mira gplv1\" license.\n";

mirac_config_s mirac_config_from_cli(
	const int32_t argc,
	const char** const argv,
	uint64_t* const config_end_index)
{
	mirac_debug_assert(argc > 0);
	mirac_debug_assert(argv != NULL);
	mirac_debug_assert(config_end_index != NULL);

	g_program = mirac_string_view_from_cstring(argv[0]);

	if (argc <= 1)
	{
		mirac_logger_error("no arguments were provided.");
		mirac_config_usage();
		mirac_c_exit(-1);
	}

	typedef struct option option_s;
	static const option_s options[] =
	{
		{ "help",       no_argument,       0, 'h' },
		{ "version",    no_argument,       0, 'v' },
		{ "arch",       required_argument, 0, 'a' },
		{ "entry",      required_argument, 0, 'e' },
		{ "output",     required_argument, 0, 'o' },
		{ 0, 0, 0, 0 }
	};

	mirac_config_s config = {0};
	int32_t opt = -1;

	while ((opt = (int32_t)getopt_long(argc, (char* const *)argv, "hva:e:o:", options, NULL)) != -1)
	{
		switch (opt)
		{
			case 'h':
			{
				mirac_config_usage();
				mirac_c_exit(0);
			} break;

			case 'v':
			{
				mirac_logger_log("%s " mirac_version_fmt, argv[0], mirac_version_arg);
				mirac_c_exit(0);
			} break;

			case 'a':
			{
				config.arch = mirac_string_view_from_cstring((const char*)optarg);
			} break;

			case 'e':
			{
				config.entry = mirac_string_view_from_cstring((const char*)optarg);
			} break;

			case 'o':
			{
				config.output = mirac_string_view_from_cstring((const char*)optarg);
			} break;

			default:
			{
				mirac_logger_error("invalid command line option.");
				mirac_config_usage();
				mirac_c_exit(-1);
			} break;
		}
	}

	if (config.arch.length <= 0)
	{
		mirac_logger_error("no architecture was provided.");
		mirac_config_usage();
		mirac_c_exit(-1);
	}
	else
	{
		bool valid_architecture = false;
		const uint64_t supported_architectures_count =
			sizeof(g_supported_architectures) / sizeof(g_supported_architectures[0]);

		for (uint8_t index = 0; index < supported_architectures_count; ++index)
		{
			if (mirac_string_view_equal(config.arch, g_supported_architectures[index]))
			{
				valid_architecture = true;
				break;
			}
		}

		if (!valid_architecture)
		{
			mirac_logger_error("invalid architecture '" mirac_sv_fmt "' was provided.", mirac_sv_arg(config.arch));
			mirac_config_usage();
			mirac_c_exit(-1);
		}
	}

	// NOTE: If entry symbol is not provided, default to 'main'.
	if (config.entry.length <= 0)
	{
		config.entry = mirac_string_view_from_cstring("main");
	}

	*config_end_index = (uint64_t)optind;
	return config;
}

void mirac_config_usage(
	void)
{
	mirac_logger_log(g_usage_banner, mirac_sv_arg(g_program));

	const uint64_t supported_architectures_count =
		sizeof(g_supported_architectures) / sizeof(g_supported_architectures[0]);
	mirac_logger_log("supported architectures:");

	for (uint8_t index = 0; index < supported_architectures_count; ++index)
	{
		mirac_logger_log("    - " mirac_sv_fmt, mirac_sv_arg(g_supported_architectures[index]));
	}

	mirac_logger_log(" ");
}
