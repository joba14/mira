
/**
 * @file timer.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-30
 */

#include <mirac/timer.h>

#include <time.h>

mirac_seconds_t mirac_timer_get_time_in_seconds(
	void)
{
	struct timespec current_time;
	clock_gettime(CLOCK_MONOTONIC, &current_time);
	signed long long now = current_time.tv_sec * 1000000000LL + current_time.tv_nsec;
	return (mirac_seconds_t)now / 1000000000.0;
}

mirac_seconds_t mirac_timer_get_elapsed_time_in_seconds(
	const mirac_seconds_t time_in_sec_since)
{
	const mirac_seconds_t now = mirac_timer_get_time_in_seconds();
	return now - time_in_sec_since;
}
