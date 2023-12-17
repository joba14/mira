
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

typedef struct
{
	const char* program;
	const char* arch;
	const char* entry;
	const char* output;
} mirac_config_s;

// TODO: write unit tests!
/**
 * @brief Create config object by parsing cli arguments.
 * 
 * @param argc[in]              command line arguments count
 * @param argv[in]              command line arguments pointer
 * @param config_end_index[out] options end index
 * 
 * @return mirac_config_s
 */
mirac_config_s mirac_config_from_cli(
	const int32_t argc,
	const char** const argv,
	uint64_t* const config_end_index);

// TODO: write unit tests!
/**
 * @brief Print the usage banner.
 */
void mirac_config_usage(
	void);

#endif
