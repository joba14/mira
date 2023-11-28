
/**
 * @file location.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__location_h__
#define __mirac__include__mirac__location_h__

#include <stdint.h>

typedef struct
{
	const char* file;
	uint64_t line;
	uint64_t column;
} mirac_location_s;

/**
 * @brief Location formatting macro for printf-like functions.
 */
#define mirac_location_fmt "%s:%lu:%lu"

/**
 * @brief Location formatting argument macro for printf-like functions.
 */
#define mirac_location_arg(_location) \
	(_location).file, (_location).line, (_location).column

#endif
