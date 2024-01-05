
/**
 * @file heap_array.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-12-10
 */

#ifndef __mirac__include__mirac__heap_array_h__
#define __mirac__include__mirac__heap_array_h__

#include <mirac/c_common.h>
#include <mirac/debug.h>
#include <mirac/arena.h>

// TODO: write unit tests!

#define mirac_define_heap_array_type(_type_name, _element_type)                \
	typedef struct                                                             \
	{                                                                          \
		mirac_arena_s* arena;                                                  \
		_element_type* data;                                                   \
		uint64_t capacity;                                                     \
		uint64_t count;                                                        \
	} _type_name ## _s;                                                        \
	                                                                           \
	_type_name ## _s _type_name ## _from_parts(                                \
		mirac_arena_s* const arena,                                            \
		const uint64_t capacity);                                              \
	                                                                           \
	void _type_name ## _push(                                                  \
		_type_name ## _s* const heap_array,                                    \
		_element_type element);                                                \
	                                                                           \
	bool _type_name ## _pop(                                                   \
		_type_name ## _s* const heap_array,                                    \
		_element_type* const element);                                         \
	                                                                           \
	_element_type* _type_name ## _at(                                          \
		_type_name ## _s* const heap_array,                                    \
		const uint64_t index);                                                 \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left for ';' support after calling the macro.

#define mirac_implement_heap_array_type(_type_name, _element_type)             \
	_type_name ## _s _type_name ## _from_parts(                                \
		mirac_arena_s* const arena,                                            \
		const uint64_t capacity)                                               \
	{                                                                          \
		mirac_debug_assert(capacity > 0);                                      \
		                                                                       \
		_type_name ## _s heap_array = {0};                                     \
		heap_array.arena = arena;                                              \
		                                                                       \
		heap_array.data = (_element_type*)mirac_arena_malloc(                  \
			heap_array.arena, capacity * sizeof(_element_type));               \
		                                                                       \
		heap_array.capacity = capacity;                                        \
		heap_array.count = 0;                                                  \
		return heap_array;                                                     \
	}                                                                          \
	                                                                           \
	void _type_name ## _push(                                                  \
		_type_name ## _s* const heap_array,                                    \
		_element_type element)                                                 \
	{                                                                          \
		mirac_debug_assert(heap_array != NULL);                                \
		                                                                       \
		if (heap_array->count + 1 >= heap_array->capacity)                     \
		{                                                                      \
			const uint64_t new_capacity = (uint64_t)(                          \
				heap_array->capacity + (heap_array->capacity / 2) + 1          \
			);                                                                 \
			                                                                   \
			_element_type* new_data = mirac_arena_malloc(                      \
				heap_array->arena, new_capacity * sizeof(_element_type));      \
			                                                                   \
			if (heap_array->count > 0)                                         \
			{                                                                  \
				mirac_c_memcpy(                                                \
					new_data,                                                  \
					heap_array->data,                                          \
					heap_array->count * sizeof(_element_type)                  \
				);                                                             \
			}                                                                  \
			                                                                   \
			heap_array->data = new_data;                                       \
			heap_array->capacity = new_capacity;                               \
		}                                                                      \
		                                                                       \
		heap_array->data[heap_array->count++] = element;                       \
	}                                                                          \
	                                                                           \
	bool _type_name ## _pop(                                                   \
		_type_name ## _s* const heap_array,                                    \
		_element_type* const element)                                          \
	{                                                                          \
		mirac_debug_assert(heap_array != NULL);                                \
		                                                                       \
		if (heap_array->count <= 0)                                            \
		{                                                                      \
			return false;                                                      \
		}                                                                      \
		                                                                       \
		*element = heap_array->data[heap_array->count--];                      \
		return true;                                                           \
	}                                                                          \
	                                                                           \
	_element_type* _type_name ## _at(                                          \
		_type_name ## _s* const heap_array,                                    \
		const uint64_t index)                                                  \
	{                                                                          \
		mirac_debug_assert(heap_array != NULL);                                \
		mirac_debug_assert(index < heap_array->count);                         \
		return &heap_array->data[index];                                       \
	}                                                                          \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left for ';' support after calling the macro.

#endif