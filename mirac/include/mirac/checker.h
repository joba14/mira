
/**
 * @file checker.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-28
 */

#ifndef __mirac__include__mirac__checker_h__
#define __mirac__include__mirac__checker_h__

typedef struct
{
	void* _dummy;
} mirac_checker_s;

mirac_checker_s mirac_checker_from_parts(
	void);

#endif
