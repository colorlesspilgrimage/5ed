/*
 * 5ed token types
 */

// TOP

#if !defined(FCODER_TOKEN_H)
#define FCODER_TOKEN_H

typedef i16 Token_Base_Kind;
enum{
    TokenBaseKind_EOF = 0,
    TokenBaseKind_Whitespace = 1,
    TokenBaseKind_LexError = 2,
    TokenBaseKind_Comment = 3,
    TokenBaseKind_Keyword = 4,
    TokenBaseKind_Preprocessor = 5,
    TokenBaseKind_Identifier = 6,
    TokenBaseKind_Operator = 7,
    TokenBaseKind_LiteralInteger = 8,
    TokenBaseKind_LiteralFloat = 9,
    TokenBaseKind_LiteralString = 10,
    TokenBaseKind_ScopeOpen = 11,
    TokenBaseKind_ScopeClose = 12,
    TokenBaseKind_ParentheticalOpen = 13,
    TokenBaseKind_ParentheticalClose = 14,
    TokenBaseKind_StatementClose = 15,
    
    TokenBaseKind_COUNT = 16,
};

static char *token_base_kind_names[] ={
    "EOF",
    "Whitespace",
    "LexError",
    "Comment",
    "Keyword",
    "Preprocessor",
    "Identifier",
    "Operator",
    "LiteralInteger",
    "LiteralFloat",
    "LiteralString",
    "ScopeOpen",
    "ScopeClose",
    "ParentheticalOpen",
    "ParentheticalClose",
};

typedef u16 Token_Base_Flag;
enum{
    TokenBaseFlag_PreprocessorBody = 1,
};

struct Token{
    i64 pos;
    i64 size;
    Token_Base_Kind kind;
    Token_Base_Flag flags;
    i16 sub_kind;
    u16 sub_flags;
};

struct Token_Pair{
    Token a;
    Token b;
};

struct Token_Array{
    Token *tokens;
    i64 count;
    i64 max;
};

struct Token_Block{
    Token_Block *next;
    Token_Block *prev;
    Token *tokens;
    i64 count;
    i64 max;
};

struct Token_List{
    Token_Block *first;
    Token_Block *last;
    i64 node_count;
    i64 total_count;
};

struct Token_Relex{
    b32 successful_resync;
    i64 first_resync_index;
};

struct Token_Iterator_Array{
    u64 user_id;
    Token *ptr;
    Token *tokens;
    i64 count;
};

struct Token_Iterator_List{
    u64 user_id;
    i64 index;
    Token *ptr;
    Token_Block *block;
    Token_Block *first;
    Token_Block *last;
    i64 node_count;
    i64 total_count;
};

typedef i32 Token_Iterator_Kind;
enum{
    TokenIterator_Array,
    TokenIterator_List,
};

struct Token_Iterator{
    Token_Iterator_Kind kind;
    union{
        Token_Iterator_Array array;
        Token_Iterator_List list;
    };
};


Range_i64
Ii64(Token *token);
void
token_list_push(Arena *arena, Token_List *list, Token *token);
void
token_fill_memory_from_list(Token *dst, Token_List *list, i64 count);
void
token_fill_memory_from_list(Token *dst, Token_List *list);
Token_Array
token_array_from_list_always_copy(Arena *arena, Token_List *list);
Token_Array
token_array_from_list(Arena *arena, Token_List *list);
i64
token_index_from_pos(Token *tokens, i64 count, i64 pos);
i64
token_index_from_pos(Token_Array *tokens, u64 pos);
Token*
token_from_pos(Token *tokens, i64 count, i64 pos);
Token*
token_from_pos(Token_Array *tokens, u64 pos);
Token_Iterator_Array
token_iterator_index(u64 user_id, Token *tokens, i64 count, i64 token_index);
Token_Iterator_Array
token_iterator_index(u64 user_id, Token_Array *tokens, i64 token_index);
Token_Iterator_Array
token_iterator(u64 user_id, Token *tokens, i64 count, Token *token);
Token_Iterator_Array
token_iterator(u64 user_id, Token_Array *tokens, Token *token);
Token_Iterator_Array
token_iterator(u64 user_id, Token *tokens, i64 count);
Token_Iterator_Array
token_iterator(u64 user_id, Token_Array *tokens);
Token_Iterator_Array
token_iterator_pos(u64 user_id, Token *tokens, i64 count, i64 pos);
Token_Iterator_Array
token_iterator_pos(u64 user_id, Token_Array *tokens, i64 pos);
Token*
token_it_read(Token_Iterator_Array *it);
i64
token_it_index(Token_Iterator_Array *it);
b32
token_it_inc_all(Token_Iterator_Array *it);
b32
token_it_dec_all(Token_Iterator_Array *it);
b32
token_it_inc_non_whitespace(Token_Iterator_Array *it);
b32
token_it_dec_non_whitespace(Token_Iterator_Array *it);
b32
token_it_inc(Token_Iterator_Array *it);
b32
token_it_dec(Token_Iterator_Array *it);
Token_Iterator_List
token_iterator_index(u64 user_id, Token_List *list, i64 index);
Token_Iterator_List
token_iterator(u64 user_id, Token_List *list);
Token_Iterator_List
token_iterator_pos(u64 user_id, Token_List *list, i64 pos);
Token*
token_it_read(Token_Iterator_List *it);
i64
token_it_index(Token_Iterator_List *it);
b32
token_it_inc_all(Token_Iterator_List *it);
b32
token_it_dec_all(Token_Iterator_List *it);
b32
token_it_inc_non_whitespace(Token_Iterator_List *it);
b32
token_it_dec_non_whitespace(Token_Iterator_List *it);
b32
token_it_inc(Token_Iterator_List *it);
b32
token_it_dec(Token_Iterator_List *it);
Token_Iterator
token_iterator(Token_Iterator_Array it);
Token_Iterator
token_iterator(Token_Iterator_List it);
Token*
token_it_read(Token_Iterator *it);
i64
token_it_index(Token_Iterator *it);
b32
token_it_inc_all(Token_Iterator *it);
b32
token_it_dec_all(Token_Iterator *it);
b32
token_it_inc_non_whitespace(Token_Iterator *it);
b32
token_it_dec_non_whitespace(Token_Iterator *it);
b32
token_it_inc(Token_Iterator *it);
b32
token_it_dec(Token_Iterator *it);
void
token_drop_eof(Token_List *list);
i64
token_relex_first(Token_Array *tokens, i64 edit_range_first, i64 backup_repeats);
i64
token_relex_resync(Token_Array *tokens, i64 edit_range_first, i64 look_ahead_repeats);
Token_Relex
token_relex(Token_List relex_list, i64 new_pos_to_old_pos_shift, Token *tokens, i64 relex_first, i64 relex_last);
#endif

// BOTTOM

