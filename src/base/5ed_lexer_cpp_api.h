/*
 * Declarations for the C++ lexer compiled once in the base library.
 */

// TOP

#if !defined(FRED_LEXER_CPP_API_H)
#define FRED_LEXER_CPP_API_H

struct Lex_State_Cpp{
    u32 flags_ZF0;
    u32 flags_KF0;
    u16 flags_KB0;
    u8 *base;
    u8 *delim_first;
    u8 *delim_one_past_last;
    u8 *emit_ptr;
    u8 *ptr;
    u8 *opl_ptr;
};

void
lex_full_input_cpp_init(Lex_State_Cpp *state_ptr, String_Const_u8 input);
b32
lex_full_input_cpp_breaks(Arena *arena, Token_List *list, Lex_State_Cpp *state_ptr, u64 max);
Token_List
lex_full_input_cpp(Arena *arena, String_Const_u8 input);

#endif

// BOTTOM
