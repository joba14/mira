
/**
 * @file linked_list.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2024-01-02
 */

#ifndef __mirac__include__mirac__linked_list_h__
#define __mirac__include__mirac__linked_list_h__

#include <mirac/c_common.h>
#include <mirac/debug.h>
#include <mirac/arena.h>

// todo: write unit tests!

#define mirac_define_linked_list_type(_type_name, _element_type)               \
	typedef struct _type_name ## _node_s _type_name ## _node_s;                \
	                                                                           \
	struct _type_name ## _node_s                                               \
	{                                                                          \
		_element_type data;                                                    \
		_type_name ## _node_s* next;                                           \
		_type_name ## _node_s* prev;                                           \
	};                                                                         \
	                                                                           \
	typedef struct                                                             \
	{                                                                          \
		mirac_arena_s* arena;                                                  \
		_type_name ## _node_s* begin;                                          \
		_type_name ## _node_s* end;                                            \
		uint64_t count;                                                        \
	} _type_name ## _s;                                                        \
	                                                                           \
	_type_name ## _s _type_name ## _from_parts(                                \
		mirac_arena_s* const arena);                                           \
	                                                                           \
	void _type_name ## _push(                                                  \
		_type_name ## _s* const linked_list,                                   \
		_element_type data);                                                   \
	                                                                           \
	_Static_assert(1, "") // note: left for ';' support after calling the macro.

#define mirac_implement_linked_list_type(_type_name, _element_type)            \
	_type_name ## _s _type_name ## _from_parts(                                \
		mirac_arena_s* const arena)                                            \
	{                                                                          \
		_type_name ## _s linked_list = {0};                                    \
		linked_list.arena = arena;                                             \
		return linked_list;                                                    \
	}                                                                          \
	                                                                           \
	void _type_name ## _push(                                                  \
		_type_name ## _s* const linked_list,                                   \
		_element_type data)                                                    \
	{                                                                          \
		mirac_debug_assert(linked_list != mirac_null);                         \
		                                                                       \
		_type_name ## _node_s* node = (_type_name ## _node_s*)mirac_c_malloc(  \
			sizeof(_type_name ## _node_s));                                    \
		mirac_debug_assert(node != mirac_null);                                \
		                                                                       \
		node->data = data;                                                     \
		node->next = mirac_null;                                               \
		node->prev = mirac_null;                                               \
		                                                                       \
		if (mirac_null == linked_list->end)                                    \
		{                                                                      \
			mirac_debug_assert(mirac_null == linked_list->begin);              \
			linked_list->end = node;                                           \
			mirac_debug_assert(linked_list->end != mirac_null);                \
			linked_list->begin = linked_list->end;                             \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			while (linked_list->end->next != mirac_null)                       \
			{                                                                  \
				linked_list->end = linked_list->end->next;                     \
			}                                                                  \
			                                                                   \
			node->prev = linked_list->end;                                     \
			linked_list->end->next = node;                                     \
			mirac_debug_assert(linked_list->end->next != mirac_null);          \
			linked_list->end = linked_list->end->next;                         \
		}                                                                      \
		                                                                       \
		linked_list->count++;                                                  \
	}                                                                          \
	                                                                           \
	_Static_assert(1, "") // note: left for ';' support after calling the macro.

#endif
