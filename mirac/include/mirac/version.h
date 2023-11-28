
/**
 * @file version.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__version_h__
#define __mirac__include__mirac__version_h__

#include <stdint.h>

#define mirac_version_major 1
#define mirac_version_minor 0
#define mirac_version_patch 0

/**
 * @brief Version formatting macro for printf-like functions.
 */
#define mirac_version_fmt "v%lu.%lu.%lu"

/**
 * @brief Version formatting argument macro for printf-like functions.
 */
#define mirac_version_arg (uint64_t)mirac_version_major, (uint64_t)mirac_version_minor, (uint64_t)mirac_version_patch

#endif
