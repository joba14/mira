
/**
 * @file logger.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#include <mirac/logger.h>

#include <mirac/debug.h>

#include <stdarg.h>
#include <stdio.h>

#define ansi_gray   "\033[90m"
#define ansi_red    "\033[91m"
#define ansi_green  "\033[92m"
#define ansi_yellow "\033[93m"
#define ansi_brown  "\033[33m"
#define ansi_reset  "\033[0m"

#define tag_debug "debug"
#define tag_info  "info"
#define tag_warn  "warn"
#define tag_error "error"

static void log_with_tag(
	FILE* const stream,
	const char* const tag,
	const char* const format,
	va_list args);

void mirac_logger_log(
	const char* const format,
	...)
{
	mirac_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stdout, NULL, format, args);
	va_end(args);
}

void mirac_logger_debug(
	const char* const format,
	...)
{
	mirac_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stdout, ansi_brown tag_debug ansi_reset, format, args);
	va_end(args);
}

void mirac_logger_info(
	const char* const format,
	...)
{
	mirac_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stdout, ansi_green tag_info ansi_reset, format, args);
	va_end(args);
}

void mirac_logger_warn(
	const char* const format,
	...)
{
	mirac_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stderr, ansi_yellow tag_warn ansi_reset, format, args);
	va_end(args);
}

void mirac_logger_error(
	const char* const format,
	...)
{
	mirac_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stderr, ansi_red tag_error ansi_reset, format, args);
	va_end(args);
}

static void log_with_tag(
	FILE* const stream,
	const char* const tag,
	const char* const format,
	va_list args)
{
	mirac_debug_assert(stream != NULL);
	mirac_debug_assert(format != NULL);

	if (tag != NULL)
	{
		(void)fprintf(stream, "%s: ", tag);
	}

	(void)vfprintf(stream, format, args);
	(void)fprintf(stream, "\n");
}
