
/**
 * @file timer.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-30
 */

#ifndef __mirac__include__mirac__timer_h__
#define __mirac__include__mirac__timer_h__

typedef long double mirac_seconds_t;

mirac_seconds_t mirac_timer_get_time_in_seconds(
	void);

mirac_seconds_t mirac_timer_get_elapsed_time_in_seconds(
	const mirac_seconds_t time_in_sec_since);

#endif
