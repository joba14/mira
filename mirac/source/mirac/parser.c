
/**
 * @file parser.c
 * 
 * @copyright This file is part of the "mira" project and is distributed under
 * "mira gplv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-28
 */

#include <mirac/parser.h>

#include <mirac/debug.h>
#include <mirac/logger.h>

#include <stddef.h>

void mirac_parser_parse(
	mirac_lexer_s* const lexer)
{
	mirac_debug_assert(lexer != NULL);

	mirac_token_s token = mirac_token_from_type(mirac_token_type_none);
	while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(lexer, &token)))
	{
		mirac_logger_log("%s", mirac_token_to_string(&token));

		switch (token.type)
		{
			case mirac_token_type_keyword_func:
			{
				const mirac_token_type_e token_type = mirac_lexer_lex(lexer, &token);

				if (token_type != mirac_token_type_keyword_req
				 && token_type != mirac_token_type_keyword_ret
				 && token_type != mirac_token_type_identifier)
				{
					mirac_logger_panic("invalid token after func!");
				}
			} break;

			case mirac_token_type_keyword_req:
			{
				while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(lexer, &token)))
				{
					if (token.type != mirac_token_type_keyword_i8 &&
						token.type != mirac_token_type_keyword_i16 &&
						token.type != mirac_token_type_keyword_i32 &&
						token.type != mirac_token_type_keyword_i64 &&
						token.type != mirac_token_type_keyword_u8 &&
						token.type != mirac_token_type_keyword_u16 &&
						token.type != mirac_token_type_keyword_u32 &&
						token.type != mirac_token_type_keyword_u64 &&
						token.type != mirac_token_type_keyword_f32 &&
						token.type != mirac_token_type_keyword_f64 &&
						token.type != mirac_token_type_keyword_ptr)
					{
						mirac_logger_panic("invalid token after req!");
					}
					else if (token.type != mirac_token_type_keyword_ret ||
							 token.type != mirac_token_type_keyword_do)
					{
						break;
					}
				}
			} break;

			case mirac_token_type_keyword_ret:
			{
				while (!mirac_lexer_should_stop_lexing(mirac_lexer_lex(lexer, &token)))
				{
					if (token.type != mirac_token_type_keyword_i8 &&
						token.type != mirac_token_type_keyword_i16 &&
						token.type != mirac_token_type_keyword_i32 &&
						token.type != mirac_token_type_keyword_i64 &&
						token.type != mirac_token_type_keyword_u8 &&
						token.type != mirac_token_type_keyword_u16 &&
						token.type != mirac_token_type_keyword_u32 &&
						token.type != mirac_token_type_keyword_u64 &&
						token.type != mirac_token_type_keyword_f32 &&
						token.type != mirac_token_type_keyword_f64 &&
						token.type != mirac_token_type_keyword_ptr)
					{
						mirac_logger_panic("invalid token after req!");
					}
					else if (token.type != mirac_token_type_keyword_do)
					{
						break;
					}
				}
			} break;

			case mirac_token_type_keyword_mem:
			{
			} break;

			case mirac_token_type_keyword_if:
			{
			} break;

			case mirac_token_type_keyword_elif:
			{
			} break;

			case mirac_token_type_keyword_else:
			{
			} break;

			case mirac_token_type_keyword_loop:
			{
			} break;

			case mirac_token_type_keyword_let:
			{
			} break;

			case mirac_token_type_keyword_as:
			{
			} break;

			// mirac_token_type_keyword_i8
			// mirac_token_type_keyword_i16
			// mirac_token_type_keyword_i32
			// mirac_token_type_keyword_i64
			// mirac_token_type_keyword_u8
			// mirac_token_type_keyword_u16
			// mirac_token_type_keyword_u32
			// mirac_token_type_keyword_u64
			// mirac_token_type_keyword_f32
			// mirac_token_type_keyword_f64
			// mirac_token_type_keyword_ptr

			case mirac_token_type_keyword_add:
			case mirac_token_type_keyword_sub:
			case mirac_token_type_keyword_mul:
			case mirac_token_type_keyword_div:
			case mirac_token_type_keyword_mod:
			case mirac_token_type_keyword_eq:
			case mirac_token_type_keyword_neq:
			case mirac_token_type_keyword_gt:
			case mirac_token_type_keyword_ls:
			case mirac_token_type_keyword_gteq:
			case mirac_token_type_keyword_lseq:
			case mirac_token_type_keyword_land:
			case mirac_token_type_keyword_lnot:
			case mirac_token_type_keyword_lor:
			case mirac_token_type_keyword_lxor:
			case mirac_token_type_keyword_band:
			case mirac_token_type_keyword_bnot:
			case mirac_token_type_keyword_bor:
			case mirac_token_type_keyword_bxor:
			case mirac_token_type_keyword_shl:
			case mirac_token_type_keyword_shr:
			case mirac_token_type_keyword_dup:
			case mirac_token_type_keyword_drop:
			case mirac_token_type_keyword_swap:
			case mirac_token_type_keyword_over:
			case mirac_token_type_keyword_rot:
			case mirac_token_type_keyword_ld8:
			case mirac_token_type_keyword_ld16:
			case mirac_token_type_keyword_ld32:
			case mirac_token_type_keyword_ld64:
			case mirac_token_type_keyword_st8:
			case mirac_token_type_keyword_st16:
			case mirac_token_type_keyword_st32:
			case mirac_token_type_keyword_st64:
			case mirac_token_type_keyword_sys1:
			case mirac_token_type_keyword_sys2:
			case mirac_token_type_keyword_sys3:
			case mirac_token_type_keyword_sys4:
			case mirac_token_type_keyword_sys5:
			case mirac_token_type_keyword_sys6:
			{
			} break;

			// mirac_token_type_keyword_do
			// mirac_token_type_keyword_end

			default:
			{
			} break;
		}

		(void)getchar();
	}
}
