
/**
 * @file hash_map.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2024-01-03
 */

#ifndef __mirac__include__mirac__hash_map_h__
#define __mirac__include__mirac__hash_map_h__

#include <mirac/c_common.h>
#include <mirac/debug.h>
#include <mirac/arena.h>

// TODO: implement the hash map - very needed in the parser for cross referencing!!!!!!

#define mirac_define_hash_map_type(_type_name, _element_type)                  \
	typedef struct                                                             \
	{                                                                          \
		
	} _type_name ## _s;                                                        \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.

#endif
