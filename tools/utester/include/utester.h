
/**
 * @file utester.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-17
 */

#ifndef __utester__include__utester_h__
#define __utester__include__utester_h__

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
			"info ", _format, ## __VA_ARGS__                                   \
		);                                                                     \
	} while (0)

#define utester_logger_warn(_format, ...)                                      \
	do                                                                         \
	{                                                                          \
		utester_logger_log(stdout,                                             \
			ansi_yellow "warn " ansi_reset, _format, ## __VA_ARGS__            \
		);                                                                     \
	} while (0)

#define utester_logger_error(_format, ...)                                     \
	do                                                                         \
	{                                                                          \
		utester_logger_log(stderr,                                             \
			ansi_red "error" ansi_reset, _format, ## __VA_ARGS__               \
		);                                                                     \
	} while (0)

typedef struct utester_test_s utester_test_s;

struct utester_test_s
{
	const char* name;
	void(*function)(utester_test_s* const);
	bool verbose;
	bool status;
};

#define utester_define_test(_test_name)                                        \
	static void _test_name ## _func(utester_test_s* const test);               \
	                                                                           \
	static utester_test_s _test_name =                                         \
	{ #_test_name, _test_name ## _func, true, false };                         \
	                                                                           \
	static void _test_name ## _func(utester_test_s* const test)

#define set_verbose(_verbose)                                                  \
	do                                                                         \
	{                                                                          \
		test->verbose = _verbose;                                              \
	} while (0)

#define set_status(_status)                                                    \
	do                                                                         \
	{                                                                          \
		test->status = _status;                                                \
	} while (0)

#define allow_empty                                                            \
	do                                                                         \
	{                                                                          \
		test->status = true;                                                   \
		utester_logger_warn("|   passing empty test.");                        \
		return;                                                                \
	} while (0)

#define utester_assert_true(_expression)                                       \
	do                                                                         \
	{                                                                          \
		test->status = (_expression);                                          \
		                                                                       \
		if (!test->status)                                                     \
		{                                                                      \
			utester_logger_error("|   assert '%s' failed at %s:%lu",           \
				#_expression, (const char*)__FILE__, (uint64_t)__LINE__        \
			);                                                                 \
			return;                                                            \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			if (test->verbose)                                                 \
			{                                                                  \
				utester_logger_info("|   assert '%s' passed.", #_expression);  \
			}                                                                  \
		}                                                                      \
	} while (0)

#define utester_assert_false(_expression)                                      \
	do                                                                         \
	{                                                                          \
		utester_assert_true(!(_expression));                                   \
	} while (0)

#define utester_assert_equal(_left, _right)                                    \
	do                                                                         \
	{                                                                          \
		utester_assert_true((_left) == (_right));                              \
	} while (0)

#define utester_assert_not_equal(_left, _right)                                \
	do                                                                         \
	{                                                                          \
		utester_assert_true((_left) != (_right));                              \
	} while (0)

#define _va_tests_to_array(...) ((utester_test_s*[]) { __VA_ARGS__ })
#define _va_tests_get_length(...) ((sizeof((utester_test_s*[]) { __VA_ARGS__ }) / sizeof(utester_test_s*)))

#define utester_run_suite(_suite_name, ...)                                    \
	int32_t main(void);                                                        \
	                                                                           \
	int32_t main(void)                                                         \
	{                                                                          \
		const char* const name = #_suite_name;                                 \
		utester_test_s* const * tests =                                        \
			(utester_test_s* const *)_va_tests_to_array(__VA_ARGS__);          \
		uint64_t tests_count = _va_tests_get_length(__VA_ARGS__);              \
		uint64_t passed_count = 0;                                             \
		uint64_t failed_count = 0;                                             \
		                                                                       \
		utester_logger_info("Running suite '%s':", name);                      \
		                                                                       \
		for (uint64_t index = 0; index < tests_count; ++index)                 \
		{                                                                      \
			utester_test_s* const test = tests[index];                         \
			assert(test != NULL);                                              \
			                                                                   \
			utester_logger_info("| Running test '%s':", test->name);           \
			test->function(test);                                              \
			                                                                   \
			if (test->status)                                                  \
			{                                                                  \
				utester_logger_info(                                           \
					"|   " ansi_green "test passed" ansi_reset);               \
				++passed_count;                                                \
			}                                                                  \
			else                                                               \
			{                                                                  \
				utester_logger_error(                                          \
					"|   " ansi_red "test failed" ansi_reset);                 \
				++failed_count;                                                \
			}                                                                  \
		}                                                                      \
		                                                                       \
		if (failed_count > 0)                                                  \
		{                                                                      \
			utester_logger_error(                                              \
				"| " ansi_red                                                  \
				"Done: %lu tests passed and %lu failed..."                     \
				ansi_reset,                                                    \
				passed_count, failed_count                                     \
			);                                                                 \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			utester_logger_info(                                               \
				"| "                                                           \
				ansi_green                                                     \
				"Done: %lu tests passed and %lu failed..."                     \
				ansi_reset,                                                    \
				passed_count, failed_count                                     \
			);                                                                 \
		}                                                                      \
		                                                                       \
		return !(0 == failed_count);                                           \
	}                                                                          \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left for ';' support after calling the macro.

/**
 * @example
 * utester_define_test(test1)
 * {
 *     utester_assert_true(1 == 1);
 * }
 * 
 * utester_define_test(test2)
 * {
 *     utester_assert_true(1 == 2);
 * }
 * 
 * utester_define_test(test3)
 * {
 *     utester_assert_true(2 == 2);
 * }
 * 
 * utester_run_suite(suite1, &test1, &test2, &test3);
 */

#endif
