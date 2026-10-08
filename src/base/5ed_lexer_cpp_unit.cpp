/*
 * Lexer tables compiled once into the base library.
 */

// TOP

#include "base/5ed_base.h"
#define lex_full_input_cpp_init lex_full_input_cpp_init__static
#define lex_full_input_cpp_breaks lex_full_input_cpp_breaks__static
#define lex_full_input_cpp lex_full_input_cpp__static
#include "base/generated/lexer_cpp.cpp"
#undef lex_full_input_cpp
#undef lex_full_input_cpp_breaks
#undef lex_full_input_cpp_init

void
lex_full_input_cpp_init(Lex_State_Cpp *state_ptr, String_Const_u8 input){
    lex_full_input_cpp_init__static(state_ptr, input);
}

b32
lex_full_input_cpp_breaks(Arena *arena, Token_List *list, Lex_State_Cpp *state_ptr, u64 max){
    return(lex_full_input_cpp_breaks__static(arena, list, state_ptr, max));
}

Token_List
lex_full_input_cpp(Arena *arena, String_Const_u8 input){
    return(lex_full_input_cpp__static(arena, input));
}

// BOTTOM
