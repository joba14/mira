
/**
 * @file logger.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-16
 */

#ifndef __utester__include__utester__logger_h__
#define __utester__include__utester__logger_h__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#define ansi_red    "\033[91m"
#define ansi_green  "\033[92m"
#define ansi_yellow "\033[93m"
#define ansi_brown  "\033[33m"
#define ansi_reset  "\033[0m"

#define utester_logger_log(_stream, _tag, _format, ...)                        \
	do                                                                         \
	{                                                                          \
		assert(_format != NULL);                                               \
		                                                                       \
		if ((_tag) != NULL)                                                    \
		{                                                                      \
			(void)fprintf(_stream, "%s: ", _tag);                              \
		}                                                                      \
		                                                                       \
		(void)fprintf(_stream, _format, ## __VA_ARGS__);                       \
		(void)fprintf(_stream, "\n");                                          \
	} while (0)

#define utester_logger_debug(_format, ...)                                     \
	do                                                                         \
	{                                                                          \
		utester_logger_log(stdout,                                             \
			ansi_brown "debug" ansi_reset, _format, ## __VA_ARGS__             \
		);                                                                     \
	} while (0)

#define utester_logger_info(_format, ...)                                      \
	do                                                                         \
	{                                                                          \
		utester_logger_log(stdout,                                             \
			ansi_green "info " ansi_reset, _format, ## __VA_ARGS__              \
		);                                                                     \
	} while (0)

#define utester_logger_warn(_format, ...)                                      \
	do                                                                         \
	{                                                                          \
		utester_logger_log(stdout,                                             \
			ansi_yellow "warn " ansi_reset, _format, ## __VA_ARGS__             \
		);                                                                     \
	} while (0)

#define utester_logger_error(_format, ...)                                     \
	do                                                                         \
	{                                                                          \
		utester_logger_log(stderr,                                             \
			ansi_red "error" ansi_reset, _format, ## __VA_ARGS__               \
		);                                                                     \
	} while (0)

#endif
