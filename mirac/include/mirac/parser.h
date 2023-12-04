
/**
 * @file parser.h
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#ifndef __mirac__include__mirac__parser_h__
#define __mirac__include__mirac__parser_h__

#include <mirac/c_common.h>
#include <mirac/vector.h>
#include <mirac/config.h>
#include <mirac/lexer.h>

mirac_define_vector_type(mirac_tokens_vector, mirac_token_s);
mirac_define_vector_type(mirac_token_refs_vector, mirac_token_s*);

typedef enum
{
	mirac_global_type_function = 0,
	mirac_global_type_memory
} mirac_global_type_e;

const char* mirac_global_type_to_string(
	const mirac_global_type_e global_type);

typedef struct
{
	mirac_token_s identifier;
	mirac_tokens_vector_s req_tokens;
	mirac_tokens_vector_s ret_tokens;
	mirac_tokens_vector_s body_tokens;
	bool is_inlined;
	bool is_entry;
} mirac_global_function_s;

typedef struct
{
	mirac_token_s identifier;
	mirac_token_s capacity;
} mirac_global_memory_s;

typedef struct
{
	mirac_token_s literal;
} mirac_global_string_s;

typedef struct
{
	mirac_global_type_e type;

	union
	{
		mirac_global_function_s as_function;
		mirac_global_memory_s as_memory;
	};
} mirac_global_s;

void mirac_global_print(
	const uint64_t global_index,
	const mirac_global_s* const global);

mirac_define_vector_type(mirac_globals_vector, mirac_global_s);

typedef struct
{
	const char* identifier;
	mirac_globals_vector_s globals;
	mirac_token_refs_vector_s strings;
} mirac_unit_s;

mirac_unit_s mirac_unit_from_parts(
	const char* identifier);

void mirac_unit_print(
	mirac_unit_s* const unit);

typedef struct
{
	mirac_config_s* config;
	mirac_lexer_s* lexer;
} mirac_parser_s;

mirac_parser_s mirac_parser_from_parts(
	mirac_config_s* const config,
	mirac_lexer_s* const lexer);

mirac_unit_s mirac_parser_parse(
	mirac_parser_s* const parser);

// TODO: rework the entire parser.
//       
//       The Great Rework of the Mirac Parser.
//       
//       Since the current version of the parser is working, this task is on a
//       low priority. However, this version is sub-optimal, not very flexible,
//       and "just working" with the emphasis on "just". The new version of the
//       parser will work with actual abstract syntax tree and will be far more
//       flexible that the current state of the parsed units.
//       
//       The Plan
//       1. Define AST types for: expressions, if-elif-else-block, loop-block,
//          let-block, reg-block, func-block, and mem-block.
//       2. Define rules of global AST blocks and local (scoped) AST blocks.
//       3. Integrate types system in the AST blocks that would be used by the
//          type checker at the later stage of source code compilation.
//       

#endif
