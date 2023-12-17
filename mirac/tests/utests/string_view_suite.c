
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

#include <mirac/string_view.h>

#include <string.h>

utester_define_test(from_parts)
{
	{
		const mirac_string_view_s view = mirac_string_view_from_parts("test", 4);
		utester_assert_true(view.data != NULL);
		utester_assert_true(4 == view.length);
	}

	{
		const mirac_string_view_s view = mirac_string_view_from_parts("", 0);
		utester_assert_true(view.data != NULL);
		utester_assert_true(0 == view.length);
	}
}

utester_define_test(from_cstring)
{
	{
		const mirac_string_view_s view = mirac_string_view_from_cstring("test");
		utester_assert_true(view.data != NULL);
		utester_assert_true(4 == view.length);
	}

	{
		const mirac_string_view_s view = mirac_string_view_from_cstring("");
		utester_assert_true(view.data != NULL);
		utester_assert_true(0 == view.length);
	}
}

utester_define_test(equal_range)
{
	{
		const mirac_string_view_s left = mirac_string_view_from_parts("left", 4);
		const mirac_string_view_s right = mirac_string_view_from_parts("right", 5);
		const bool are_equal = mirac_string_view_equal_range(left, right, 4);
		utester_assert_true(false == are_equal);
	}

	{
		const mirac_string_view_s left = mirac_string_view_from_parts("world", 5);
		const mirac_string_view_s right = mirac_string_view_from_parts("world", 5);
		const bool are_equal = mirac_string_view_equal_range(left, right, 4);
		utester_assert_true(true == are_equal);
	}
}

utester_define_test(equal)
{
	{
		const mirac_string_view_s left = mirac_string_view_from_parts("left", 4);
		const mirac_string_view_s right = mirac_string_view_from_parts("right", 5);
		const bool are_equal = mirac_string_view_equal(left, right);
		utester_assert_true(false == are_equal);
	}

	{
		const mirac_string_view_s left = mirac_string_view_from_parts("world", 5);
		const mirac_string_view_s right = mirac_string_view_from_parts("world", 5);
		const bool are_equal = mirac_string_view_equal(left, right);
		utester_assert_true(true == are_equal);
	}
}

utester_define_test(trim_left)
{
	{
		const mirac_string_view_s view = mirac_string_view_from_parts("world", 5);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim_left(view, ' ', NULL);
		utester_assert_true(view.data[0] != ' ');
		utester_assert_true(trimmed_view.data[0] != ' ');
	}

	{
		const mirac_string_view_s view = mirac_string_view_from_parts("   world", 8);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim_left(view, ' ', NULL);
		utester_assert_true(' ' == view.data[0]);
		utester_assert_true(trimmed_view.data[0] != ' ');
	}

	{
		uint64_t spaces_count = 0;
		const mirac_string_view_s view = mirac_string_view_from_parts("   world", 8);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim_left(view, ' ', &spaces_count);
		utester_assert_true(' ' == view.data[0]);
		utester_assert_true(trimmed_view.data[0] != ' ');
		utester_assert_true(3 == spaces_count);
	}
}

utester_define_test(trim_right)
{
	{
		const mirac_string_view_s view = mirac_string_view_from_parts("world", 5);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim_right(view, ' ', NULL);
		utester_assert_true(view.data[view.length - 1] != ' ');
		utester_assert_true(trimmed_view.data[trimmed_view.length - 1] != ' ');
	}

	{
		const mirac_string_view_s view = mirac_string_view_from_parts("world   ", 8);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim_right(view, ' ', NULL);
		utester_assert_true(' ' == view.data[view.length - 1]);
		utester_assert_true(trimmed_view.data[trimmed_view.length - 1] != ' ');
	}

	{
		uint64_t spaces_count = 0;
		const mirac_string_view_s view = mirac_string_view_from_parts("world   ", 8);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim_right(view, ' ', &spaces_count);
		utester_assert_true(' ' == view.data[view.length - 1]);
		utester_assert_true(trimmed_view.data[trimmed_view.length - 1] != ' ');
		utester_assert_true(3 == spaces_count);
	}
}

utester_define_test(trim)
{
	{
		const mirac_string_view_s view = mirac_string_view_from_parts("world", 5);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim(view, ' ');
		utester_assert_true(view.data[0] != ' ');
		utester_assert_true(view.data[view.length - 1] != ' ');
		utester_assert_true(trimmed_view.data[0] != ' ');
		utester_assert_true(trimmed_view.data[trimmed_view.length - 1] != ' ');
	}

	{
		const mirac_string_view_s view = mirac_string_view_from_parts("   world   ", 11);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim(view, ' ');
		utester_assert_true(' ' == view.data[0]);
		utester_assert_true(' ' == view.data[view.length - 1]);
		utester_assert_true(trimmed_view.data[0] != ' ');
		utester_assert_true(trimmed_view.data[trimmed_view.length - 1] != ' ');
	}
}

utester_define_test(trim_left_white_space)
{
	{
		uint64_t trimmed_length = 0;
		const mirac_string_view_s view = mirac_string_view_from_parts("world", 5);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim_left_white_space(view, &trimmed_length);
		utester_assert_true(view.data[0] != ' ');
		utester_assert_true(view.data[view.length - 1] != ' ');
		utester_assert_true(trimmed_view.data[0] != ' ');
		utester_assert_true(trimmed_view.data[trimmed_view.length - 1] != ' ');
		utester_assert_true(trimmed_length == 0);
	}

	{
		uint64_t trimmed_length = 0;
		const mirac_string_view_s view = mirac_string_view_from_parts("   world", 8);
		const mirac_string_view_s trimmed_view = mirac_string_view_trim_left_white_space(view, &trimmed_length);
		utester_assert_true(' ' == view.data[0]);
		utester_assert_true(' ' != view.data[view.length - 1]);
		utester_assert_true(trimmed_view.data[0] != ' ');
		utester_assert_true(trimmed_view.data[trimmed_view.length - 1] != ' ');
		utester_assert_true(trimmed_length == 3);
	}
}

utester_define_test(split_left)
{
	const char* test_string = "General Kenobi, you are a bold one!";
	mirac_string_view_s text = mirac_string_view_from_cstring(test_string);
	mirac_string_view_s left = {0};

	left = mirac_string_view_split_left(&text, ' ');
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("General")));
	utester_assert_true(text.length == strlen("Kenobi, you are a bold one!"));

	left = mirac_string_view_split_left(&text, ' ');
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("Kenobi,")));
	utester_assert_true(text.length == strlen("you are a bold one!"));

	left = mirac_string_view_split_left(&text, ' ');
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("you")));
	utester_assert_true(text.length == strlen("are a bold one!"));

	left = mirac_string_view_split_left(&text, ' ');
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("are")));
	utester_assert_true(text.length == strlen("a bold one!"));

	left = mirac_string_view_split_left(&text, ' ');
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("a")));
	utester_assert_true(text.length == strlen("bold one!"));

	left = mirac_string_view_split_left(&text, ' ');
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("bold")));
	utester_assert_true(text.length == strlen("one!"));

	left = mirac_string_view_split_left(&text, ' ');
	utester_assert_true(mirac_string_view_equal(text, mirac_string_view_from_cstring("one!")));
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("")));
	utester_assert_true(text.length == strlen("one!"));
	utester_assert_true(left.length == 0);
}

utester_define_test(split_left_white_space)
{
	const char* test_string = "General Kenobi,  you   are    a     bold      one!";
	mirac_string_view_s text = mirac_string_view_from_cstring(test_string);
	mirac_string_view_s left = {0};
	uint64_t white_space_length;

	white_space_length = 0;
	left = mirac_string_view_split_left_white_space(&text, &white_space_length);
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("General")));
	utester_assert_true(text.length == strlen("Kenobi,  you   are    a     bold      one!"));
	utester_assert_true(white_space_length == 1);

	white_space_length = 0;
	left = mirac_string_view_split_left_white_space(&text, &white_space_length);
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("Kenobi,")));
	utester_assert_true(text.length == strlen("you   are    a     bold      one!"));
	utester_assert_true(white_space_length == 2);

	white_space_length = 0;
	left = mirac_string_view_split_left_white_space(&text, &white_space_length);
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("you")));
	utester_assert_true(text.length == strlen("are    a     bold      one!"));
	utester_assert_true(white_space_length == 3);

	white_space_length = 0;
	left = mirac_string_view_split_left_white_space(&text, &white_space_length);
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("are")));
	utester_assert_true(text.length == strlen("a     bold      one!"));
	utester_assert_true(white_space_length == 4);

	white_space_length = 0;
	left = mirac_string_view_split_left_white_space(&text, &white_space_length);
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("a")));
	utester_assert_true(text.length == strlen("bold      one!"));
	utester_assert_true(white_space_length == 5);

	white_space_length = 0;
	left = mirac_string_view_split_left_white_space(&text, &white_space_length);
	utester_assert_true(mirac_string_view_equal(left, mirac_string_view_from_cstring("bold")));
	utester_assert_true(text.length == strlen("one!"));
	utester_assert_true(white_space_length == 6);
}

utester_run_suite(string_view_suite,
	&from_parts, &from_cstring,
	&equal_range, &equal,
	&trim_left, &trim_right, &trim,
	&trim_left_white_space,
	&split_left,
	&split_left_white_space
);
