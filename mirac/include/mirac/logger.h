
/**
 * @file logger.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__logger_h__
#define __mirac__include__mirac__logger_h__

#include <mirac/c_common.h>

/**
 * @brief Log tagless level formattable messages.
 */
void mirac_logger_log(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log debug level formattable messages.
 */
void mirac_logger_debug(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log info level formattable messages.
 */
void mirac_logger_info(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log warn level formattable messages.
 */
void mirac_logger_warn(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log error level formattable messages.
 * 
 */
void mirac_logger_error(
	const char* const format,
	...) __attribute__ ((format (printf, 1, 2)));

#endif
