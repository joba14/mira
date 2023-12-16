
/**
 * @file suite.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-16
 */

#ifndef __utester__include__utester__suite_h__
#define __utester__include__utester__suite_h__

#include <utester/logger.h>
#include <utester/test.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <assert.h>
#include <stdio.h>

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
				utester_logger_info("|   test passed");                        \
				passed_count++;                                                \
			}                                                                  \
			else                                                               \
			{                                                                  \
				utester_logger_error("|   test failed");                       \
				failed_count++;                                                \
			}                                                                  \
		}                                                                      \
		                                                                       \
		if (failed_count > 0)                                                  \
		{                                                                      \
			utester_logger_error("| Done: %lu tests passed and %lu failed...", \
				passed_count, failed_count                                     \
			);                                                                 \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			utester_logger_info("| Done: %lu tests passed and %lu failed...",  \
				passed_count, failed_count                                     \
			);                                                                 \
		}                                                                      \
		                                                                       \
		return 0;                                                              \
	}                                                                          \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.

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
