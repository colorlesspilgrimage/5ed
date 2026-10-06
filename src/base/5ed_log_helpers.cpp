/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 14.08.2019
 *
 * Logging helpers.
 *
 */

// TOP

#include "base/5ed_base.h"



String_Const_u8
log_event(Arena *arena, String_Const_u8 event_name, String_Const_u8 src_name, i32 line_number, i32 buffer, i32 view, i32 thread_id){
    List_String_Const_u8 list = {};
    string_list_pushf(arena, &list, "%.*s:%d: %.*s",
                      string_expand(src_name), line_number, string_expand(event_name));
    if (thread_id != 0){
        string_list_pushf(arena, &list, " [thread=%d]", thread_id);
    }
    if (buffer != 0){
        string_list_pushf(arena, &list, " [buffer=%d]", buffer);
    }
    if (view != 0){
        string_list_pushf(arena, &list, " [view=%d]", view);
    }
    string_list_push(arena, &list, string_u8_litexpr("\n"));
    return(string_list_flatten(arena, list));
}





// BOTTOM

