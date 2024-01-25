
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

#include <mirac/debug.h>
#include <mirac/c_common.h>

/**
 * @brief Log tagless level formattable messages.
 * 
 * @param format format of the log
 * @param ...    arguments of the log
 */
void mirac_logger_log(
	const char_t* const format,
	...) __attribute__ ((format (printf, 1, 2)));

#ifndef NDEBUG
/**
 * @brief Log debug level formattable messages.
 * 
 * @param format format of the log
 * @param ...    arguments of the log
 */
void mirac_logger_debug(
	const char_t* const format,
	...) __attribute__ ((format (printf, 1, 2)));
#else
#	define mirac_logger_debug(_format, ...) (void)(_format)
#endif

/**
 * @brief Log info level formattable messages.
 * 
 * @param format format of the log
 * @param ...    arguments of the log
 */
void mirac_logger_info(
	const char_t* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log warn level formattable messages.
 * 
 * @param format format of the log
 * @param ...    arguments of the log
 */
void mirac_logger_warn(
	const char_t* const format,
	...) __attribute__ ((format (printf, 1, 2)));

/**
 * @brief Log error level formattable messages.
 * 
 * @param format format of the log
 * @param ...    arguments of the log
 */
void mirac_logger_error(
	const char_t* const format,
	...) __attribute__ ((format (printf, 1, 2)));

#endif
