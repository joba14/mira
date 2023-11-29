
/**
 * @file vector.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-29
 */

#ifndef __mirac__include__mirac__vector_h__
#define __mirac__include__mirac__vector_h__

#include <mirac/debug.h>
#include <mirac/utils.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define mirac_define_vector_type(_type_name, _element_type)                    \
	typedef struct                                                             \
	{                                                                          \
		_element_type* data;                                                   \
		uint64_t capacity;                                                     \
		uint64_t count;                                                        \
	} _type_name ## _s;                                                        \
	                                                                           \
	_type_name ## _s _type_name ## _from_parts(                                \
		const uint64_t capacity);                                              \
	                                                                           \
	void _type_name ## _destroy(                                               \
		_type_name ## _s* const vector);                                       \
	                                                                           \
	void _type_name ## _append(                                                \
		_type_name ## _s* const vector,                                        \
		const _element_type* const element);                                   \
	                                                                           \
	void _type_name ## _shrink(                                                \
		_type_name ## _s* const vector);                                       \
	                                                                           \
	_element_type* _type_name ## _at(                                          \
		_type_name ## _s* const vector,                                        \
		const uint64_t index);                                                 \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.

#define mirac_implement_vector_type(_type_name, _element_type)                 \
	_type_name ## _s _type_name ## _from_parts(                                \
		const uint64_t capacity)                                               \
	{                                                                          \
		mirac_debug_assert(capacity > 0);                                      \
		                                                                       \
		_type_name ## _s vector = {0};                                         \
		vector.data = (_element_type*)mirac_utils_malloc(                      \
			capacity * sizeof(_element_type));                                 \
		mirac_debug_assert(vector.data != NULL);                               \
		                                                                       \
		vector.capacity = capacity;                                            \
		vector.count = 0;                                                      \
		return vector;                                                         \
	}                                                                          \
	                                                                           \
	void _type_name ## _destroy(                                               \
		_type_name ## _s* const vector)                                        \
	{                                                                          \
		mirac_debug_assert(vector != NULL);                                    \
		mirac_utils_free(vector->data);                                        \
		mirac_utils_memset(vector, 0, sizeof(_type_name ## _s));               \
	}                                                                          \
	                                                                           \
	void _type_name ## _append(                                                \
		_type_name ## _s* const vector,                                        \
		const _element_type* const element)                                    \
	{                                                                          \
		mirac_debug_assert(vector != NULL);                                    \
		mirac_debug_assert(element != NULL);                                   \
		                                                                       \
		if (vector->count + 1 >= vector->capacity)                             \
		{                                                                      \
			const uint64_t new_capacity = (uint64_t)(                          \
				vector->capacity + (vector->capacity / 2)                      \
			);                                                                 \
			                                                                   \
			vector->data = (_element_type*)mirac_utils_realloc(                \
				vector->data, new_capacity * sizeof(_element_type)             \
			);                                                                 \
			                                                                   \
			mirac_debug_assert(vector->data != NULL);                          \
			vector->capacity = new_capacity;                                   \
		}                                                                      \
		                                                                       \
		vector->data[vector->count++] = *element;                              \
	}                                                                          \
	                                                                           \
	void _type_name ## _shrink(                                                \
		_type_name ## _s* const vector)                                        \
	{                                                                          \
		mirac_debug_assert(vector != NULL);                                    \
		                                                                       \
		if (vector->count <= 0)                                                \
		{                                                                      \
			return;                                                            \
		}                                                                      \
		                                                                       \
		if (vector->count >= vector->capacity)                                 \
		{                                                                      \
			return;                                                            \
		}                                                                      \
		                                                                       \
		vector->data = (_element_type*)mirac_utils_realloc(                    \
			vector->data, vector->count * sizeof(_element_type)                \
		);                                                                     \
		                                                                       \
		mirac_debug_assert(vector->data != NULL);                              \
		vector->capacity = vector->count;                                      \
	}                                                                          \
	                                                                           \
	_element_type* _type_name ## _at(                                          \
		_type_name ## _s* const vector,                                        \
		const uint64_t index)                                                  \
	{                                                                          \
		mirac_debug_assert(vector != NULL);                                    \
		mirac_debug_assert(index < vector->count);                             \
		return &vector->data[index];                                           \
	}                                                                          \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.

#endif
