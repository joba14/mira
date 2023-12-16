
/**
 * @file test.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-16
 */

#ifndef __utester__include__utester__test_h__
#define __utester__include__utester__test_h__

#include <utester/logger.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct utester_test_s utester_test_s;

struct utester_test_s
{
	const char* name;
	void(*function)(utester_test_s* const);
	bool status;
};

#define utester_define_test(_test_name)                                        \
	static void _test_name ## _func(utester_test_s* const test);               \
	                                                                           \
	static utester_test_s _test_name =                                         \
	{ #_test_name, _test_name ## _func, false };                               \
	                                                                           \
	static void _test_name ## _func(utester_test_s* const test)

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
		}                                                                      \
		else                                                                   \
		{                                                                      \
			utester_logger_info("|   assert '%s' passed.", #_expression);      \
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

#endif
