
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
#include <mirac/c_common.h>

#include <getopt.h>

static mirac_string_view_s g_program = mirac_string_view_static("");
static mirac_string_view_s g_supported_architectures[] =
{
	[mirac_config_arch_type_fasm_x86_64_linux] = mirac_string_view_static("fasm_x86_64_linux"),
	[mirac_config_arch_type_nasm_x86_64_linux] = mirac_string_view_static("nasm_x86_64_linux")
};

static const char* const g_usage_banner =
	"usage: " mirac_sv_fmt " [options] <in+out files...>\n"
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

static int32_t compare_text_with_supported_architecture(
	const void* const left,
	const void* const right);

static int32_t compare_text_with_supported_architecture(
	const void* const left,
	const void* const right)
{
	const mirac_string_view_s* left_string_view = (const mirac_string_view_s*)left;
	const mirac_string_view_s* right_string_view = (const mirac_string_view_s*)right;
	return mirac_c_strncmp(left_string_view->data, right_string_view->data, left_string_view->length);
}

mirac_string_view_s mirac_config_arch_type_to_string_view(
	const mirac_config_arch_type_e type)
{
	switch (type)
	{
		case mirac_config_arch_type_fasm_x86_64_linux: { return mirac_string_view_from_cstring("fasm_x86_64_linux"); } break;
		case mirac_config_arch_type_nasm_x86_64_linux: { return mirac_string_view_from_cstring("nasm_x86_64_linux"); } break;

		default:
		{
			mirac_debug_assert(0);
			return mirac_string_view_from_parts("", 0);
		} break;
	}
}

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
				mirac_string_view_s arch_as_string = mirac_string_view_from_cstring((const char*)optarg);
				const void* const found_architecture = (const void* const)mirac_c_bsearch(
					&arch_as_string, g_supported_architectures, mirac_config_arch_types_count + 1,
					sizeof(g_supported_architectures[0]), compare_text_with_supported_architecture
				);

				if (NULL == found_architecture)
				{
					config.arch = mirac_config_arch_type_none;
				}
				else
				{
					config.arch = (mirac_config_arch_type_e)((const mirac_string_view_s*)found_architecture - g_supported_architectures);
				}
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

	if (mirac_config_arch_type_none == config.arch)
	{
		mirac_logger_error("invalid architecture was provided.");
		mirac_config_usage();
		mirac_c_exit(-1);
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
	mirac_logger_log("supported architectures:");

	for (uint64_t index = 0; index < mirac_config_arch_types_count + 1; ++index)
	{
		mirac_logger_log("    - " mirac_sv_fmt, mirac_sv_arg(g_supported_architectures[index]));
	}

	mirac_logger_log(" ");
}
