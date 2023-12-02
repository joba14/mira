
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
#include <memory.h>
#include <stdio.h>

#define ANSI_GRAY   "\033[90m"
#define ANSI_RED    "\033[91m"
#define ANSI_GREEN  "\033[92m"
#define ANSI_YELLOW "\033[93m"
#define ANSI_BROWN  "\033[33m"
#define ANSI_RESET  "\033[0m"

#define TAG_DEBUG "debug"
#define TAG_INFO  "info"
#define TAG_WARN  "warn"
#define TAG_ERROR "error"
#define TAG_PANIC "panic"

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
	log_with_tag(stdout, ANSI_BROWN TAG_DEBUG ANSI_RESET, format, args);
	va_end(args);
}

void mirac_logger_info(
	const char* const format,
	...)
{
	mirac_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stdout, ANSI_GREEN TAG_INFO ANSI_RESET, format, args);
	va_end(args);
}

void mirac_logger_warn(
	const char* const format,
	...)
{
	mirac_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stderr, ANSI_YELLOW TAG_WARN ANSI_RESET, format, args);
	va_end(args);
}

void mirac_logger_error(
	const char* const format,
	...)
{
	mirac_debug_assert(format != NULL);
	va_list args; va_start(args, format);
	log_with_tag(stderr, ANSI_RED TAG_ERROR ANSI_RESET, format, args);
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
