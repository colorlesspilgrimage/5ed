/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 30.07.2019
 *
 * Types for operating on the String_Match and String_Match_List types.
 *
 */

// TOP

#if !defined(FCODER_STRING_MATCH_H)
#define FCODER_STRING_MATCH_H

typedef b32 Buffer_Predicate(Application_Links *app, Buffer_ID buffer);


void
string_match_list_push(Arena *arena, String_Match_List *list,
                       Buffer_ID buffer, i32 string_id, String_Match_Flag flags, Range_i64 range);
void
string_match_list_push(Arena *arena, String_Match_List *list,
                       Buffer_ID buffer, i32 string_id, String_Match_Flag flags, i64 start, i64 length);
String_Match_List
string_match_list_join(String_Match_List *a, String_Match_List *b);
void
string_match_list_filter_flags(String_Match_List *list, String_Match_Flag must_have_flags, String_Match_Flag must_not_have_flags);
void
string_match_list_filter_remove_buffer(String_Match_List *list, Buffer_ID buffer);
void
string_match_list_filter_remove_buffer_predicate(Application_Links *app, String_Match_List *list, Buffer_Predicate *predicate);
String_Match_List
string_match_list_merge_nearest(String_Match_List *a, String_Match_List *b, Range_i64 range);
String_Match_List
string_match_list_merge_front_to_back(String_Match_List *a, String_Match_List *b);
#endif

// BOTTOM