
/**
 * @file string_view_suite.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-16
 */

#include <utester.h>

#include <mirac/arena.h>
#include <mirac/linked_list.h>

mirac_define_linked_list_type(int32_list, int32_t);
mirac_implement_linked_list_type(int32_list, int32_t);

utester_define_test(from_parts)
{
	mirac_arena_s arena = mirac_arena_from_parts();
	int32_list_s ints_list = int32_list_from_parts(&arena);

	utester_assert_true(NULL == ints_list.begin);
	utester_assert_true(NULL == ints_list.end);
	utester_assert_true(0 == ints_list.count);

	mirac_arena_destroy(&arena);
}

utester_run_suite(linked_list_suite,
	&from_parts
);
