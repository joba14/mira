
/**
 * @file config.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-01
 */

#ifndef __mirac__include__mirac__config_h__
#define __mirac__include__mirac__config_h__

#include <mirac/c_common.h>
#include <mirac/string_view.h>

typedef enum
{
	mirac_config_arch_type_x86_64 = 0,
	mirac_config_arch_types_count,

	mirac_config_arch_type_none
} mirac_config_arch_type_e;

mirac_string_view_s mirac_config_arch_type_to_string_view(
	const mirac_config_arch_type_e type);

typedef enum
{
	mirac_config_format_type_nasm = 0,
	mirac_config_format_types_count,

	mirac_config_format_type_none
} mirac_config_format_type_e;

mirac_string_view_s mirac_config_format_type_to_string_view(
	const mirac_config_format_type_e type);

typedef struct
{
	mirac_config_arch_type_e arch;
	mirac_config_format_type_e format;
	mirac_string_view_s entry;
	bool_t dump_ast;
	bool_t unsafe;
	bool_t strip;
} mirac_config_s;

// todo: write unit tests!
/**
 * @brief Create config object by parsing cli arguments.
 * 
 * @param argc             command line arguments count
 * @param argv             command line arguments pointer
 * @param config_end_index options end index
 * 
 * @return mirac_config_s
 */
mirac_config_s mirac_config_from_cli(
	const int32_t argc,
	const char_t** const argv,
	uint64_t* const config_end_index);

// todo: write unit tests!
/**
 * @brief Print the usage banner.
 */
void mirac_config_usage(
	void);

#endif
