
tokens = [
	("mirac_token_type_reserved_lnot", "!"),
	("mirac_token_type_reserved_land", "&&"),
	("mirac_token_type_reserved_lor", "||"),
	("mirac_token_type_reserved_lxor", "^^"),

	("mirac_token_type_reserved_bnot", "~"),
	("mirac_token_type_reserved_band", "&"),
	("mirac_token_type_reserved_bor", "|"),
	("mirac_token_type_reserved_bxor", "^"),
	("mirac_token_type_reserved_shl", "<<"),
	("mirac_token_type_reserved_shr", ">>"),

	("mirac_token_type_reserved_eq", "=="),
	("mirac_token_type_reserved_neq", "!="),
	("mirac_token_type_reserved_gt", ">"),
	("mirac_token_type_reserved_gteq", ">="),
	("mirac_token_type_reserved_ls", "<"),
	("mirac_token_type_reserved_lseq", "<="),

	("mirac_token_type_reserved_add", "+"),
	("mirac_token_type_reserved_inc", "++"),
	("mirac_token_type_reserved_sub", "-"),
	("mirac_token_type_reserved_dec", "--"),
	("mirac_token_type_reserved_mul", "*"),
	("mirac_token_type_reserved_div", "/"),
	("mirac_token_type_reserved_mod", "%"),

	("mirac_token_type_reserved_i08", "i08"),
	("mirac_token_type_reserved_i16", "i16"),
	("mirac_token_type_reserved_i32", "i32"),
	("mirac_token_type_reserved_i64", "i64"),
	("mirac_token_type_reserved_u08", "u08"),
	("mirac_token_type_reserved_u16", "u16"),
	("mirac_token_type_reserved_u32", "u32"),
	("mirac_token_type_reserved_u64", "u64"),
	("mirac_token_type_reserved_f32", "f32"),
	("mirac_token_type_reserved_f64", "f64"),
	("mirac_token_type_reserved_ptr", "ptr"),
	("mirac_token_type_reserved_ld08", "\\*08"),
	("mirac_token_type_reserved_ld16", "\\*16"),
	("mirac_token_type_reserved_ld32", "\\*32"),
	("mirac_token_type_reserved_ld64", "\\*64"),
	("mirac_token_type_reserved_st08", "\\&08"),
	("mirac_token_type_reserved_st16", "\\&16"),
	("mirac_token_type_reserved_st32", "\\&32"),
	("mirac_token_type_reserved_st64", "\\&64"),
	("mirac_token_type_reserved_sys1", "\\$1"),
	("mirac_token_type_reserved_sys2", "\\$2"),
	("mirac_token_type_reserved_sys3", "\\$3"),
	("mirac_token_type_reserved_sys4", "\\$4"),
	("mirac_token_type_reserved_sys5", "\\$5"),
	("mirac_token_type_reserved_sys6", "\\$6"),

	("mirac_token_type_reserved_str", "str"),
	("mirac_token_type_reserved_mem", "mem"),
	("mirac_token_type_reserved_inl", "inl"),
	("mirac_token_type_reserved_func", "func"),
	("mirac_token_type_reserved_if", "if"),
	("mirac_token_type_reserved_left_brace", "{"),
	("mirac_token_type_reserved_elif", "elif"),
	("mirac_token_type_reserved_else", "else"),
	("mirac_token_type_reserved_right_brace", "}"),
	("mirac_token_type_reserved_loop", "loop"),
	("mirac_token_type_reserved_as", "as"),
	("mirac_token_type_reserved_req", "req"),
	("mirac_token_type_reserved_ret", "ret"),

	("mirac_token_type_reserved_drop", "\\!."),
	("mirac_token_type_reserved_dup", "\\+."),
	("mirac_token_type_reserved_over", "\\>."),
	("mirac_token_type_reserved_rot", "\\>3"),
	("mirac_token_type_reserved_swap", "\\><"),
]

for token in sorted(tokens, key=lambda x: x[1]):
	print(f"{token[0]}, \"{token[1]}\",")
