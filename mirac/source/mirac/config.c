
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
static mirac_string_view_s g_supported_architectures[mirac_config_arch_types_count] =
{
	[mirac_config_arch_type_nasm_x86_64_linux] = mirac_string_view_static("nasm_x86_64_linux")
};

static const char_t* const g_usage_banner =
	"usage: " mirac_sv_fmt " [options] <src+out files...>\n"
	"\n"
	"options:\n"
	"    -h, --help                 print the help message\n"
	"    -v, --version              print version and exit\n"
	"    -a, --arch <target>        set the architecture for the output\n"
	"    -e, --entry <symbol>       set the entry symbol\n"
	"    -d, --dump_ast             dump generated ast into text file near output file\n"
	"    -u, --unsafe               disable checker\n"
	"    -s, --strip                strip unused code in the output\n"
	"\n"
	"notice:\n"
	"    this executable is distributed under the \"mira gplv1\" license.\n";

mirac_string_view_s mirac_config_arch_type_to_string_view(
	const mirac_config_arch_type_e type)
{
	mirac_debug_assert((type >= 0) && (type < mirac_config_arch_types_count));
	return g_supported_architectures[type];
}

mirac_config_s mirac_config_from_cli(
	const int32_t argc,
	const char_t** const argv,
	uint64_t* const config_end_index)
{
	mirac_debug_assert(argc > 0);
	mirac_debug_assert(argv != mirac_null);
	mirac_debug_assert(config_end_index != mirac_null);

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
		{ "dump_ast",   no_argument,       0, 'd' },
		{ "unsafe",     no_argument,       0, 'u' },
		{ "strip",      no_argument,       0, 's' },
		{ 0, 0, 0, 0 }
	};

	mirac_config_s config = (mirac_config_s)
	{
		.arch     = mirac_config_arch_type_none,
		.entry    = mirac_string_view_from_parts("main", 4),
		.dump_ast = false,
		.unsafe   = false,
		.strip    = false
	};

	mirac_string_view_s parsed_arch = mirac_string_view_from_parts("", 0);
	mirac_string_view_s parsed_entry = mirac_string_view_from_parts("", 0);
	int32_t parsed_option = -1;

	while ((parsed_option = (int32_t)getopt_long(argc, (char_t* const *)argv, "hva:e:dus", options, mirac_null)) != -1)
	{
		switch (parsed_option)
		{
			case 'h':
			{
				mirac_config_usage();
				mirac_c_exit(0);
			} break;

			case 'v':
			{
				mirac_logger_log(mirac_version_fmt, mirac_version_arg);
				mirac_c_exit(0);
			} break;

			case 'a':
			{
				parsed_arch = mirac_string_view_from_cstring((const char_t*)optarg);
			} break;

			case 'e':
			{
				parsed_entry = mirac_string_view_from_cstring((const char_t*)optarg);
			} break;

			case 'd':
			{
				config.dump_ast = true;
			} break;

			case 'u':
			{
				config.unsafe = true;
			} break;

			case 's':
			{
				config.strip = true;
			} break;

			default:
			{
				mirac_logger_error("invalid command line option.");
				mirac_config_usage();
				mirac_c_exit(-1);
			} break;
		}
	}

	{
		if (parsed_arch.length <= 0)
		{
			mirac_logger_error("no architecture was provided.");
			mirac_config_usage();
			mirac_c_exit(-1);
		}

		for (uint64_t arch_index = 0; arch_index < mirac_config_arch_types_count; ++arch_index)
		{
			if (mirac_string_view_equal(parsed_arch, g_supported_architectures[arch_index]))
			{
				config.arch = arch_index;
				break;
			}
		}

		if (mirac_config_arch_type_none == config.arch)
		{
			mirac_logger_error("invalid architecture '" mirac_sv_fmt "' was provided.", mirac_sv_arg(parsed_arch));
			mirac_config_usage();
			mirac_c_exit(-1);
		}
	}

	if (config.entry = parsed_entry, config.entry.length <= 0)
	{
		mirac_logger_error("no entry symbol was provided.");
		mirac_config_usage();
		mirac_c_exit(-1);
	}

	*config_end_index = (uint64_t)optind;
	return config;
}

void mirac_config_usage(
	void)
{
	mirac_logger_log(g_usage_banner, mirac_sv_arg(g_program));
	mirac_logger_log("supported architectures:");

	for (uint64_t arch_index = 0; arch_index < mirac_config_arch_types_count; ++arch_index)
	{
		mirac_logger_log("    - " mirac_sv_fmt, mirac_sv_arg(g_supported_architectures[arch_index]));
	}

	mirac_logger_log(" ");
}
