
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

#define mirac_define_linked_list_type(_type_name, _element_type)               \
	typedef struct _type_name ## _node_s _type_name ## _node_s;                \
	                                                                           \
	struct _type_name ## _node_s                                               \
	{                                                                          \
		_element_type data;                                                    \
		_type_name ## _node_s* next;                                           \
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
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.

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
		mirac_debug_assert(linked_list != NULL);                               \
		                                                                       \
		_type_name ## _node_s* node = (_type_name ## _node_s*)mirac_c_malloc(  \
			sizeof(_type_name ## _node_s)                                      \
		);                                                                     \
		                                                                       \
		if (NULL == node)                                                      \
		{                                                                      \
			mirac_logger_error(                                                \
				"internal failure -- failed to allocate linked list node.");   \
			mirac_c_exit(-1);                                                  \
		}                                                                      \
		                                                                       \
		node->data = data;                                                     \
		node->next = NULL;                                                     \
		                                                                       \
		if (NULL == linked_list->end)                                          \
		{                                                                      \
			mirac_debug_assert(NULL == linked_list->begin);                    \
			linked_list->end = node;                                           \
			mirac_debug_assert(linked_list->end != NULL);                      \
			linked_list->begin = linked_list->end;                             \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			while (linked_list->end->next != NULL)                             \
			{                                                                  \
				linked_list->end = linked_list->end->next;                     \
			}                                                                  \
			                                                                   \
			linked_list->end->next = node;                                     \
			mirac_debug_assert(linked_list->end->next != NULL);                \
			linked_list->end = linked_list->end->next;                         \
		}                                                                      \
		                                                                       \
		linked_list->count++;                                                  \
	}                                                                          \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.

#endif
