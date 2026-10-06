#if !defined(FCODER_5ED_LOG_HELPERS_H)
#define FCODER_5ED_LOG_HELPERS_H

#define LogEventStr(log_call, arena, B, V, T, E) \
Stmnt(Temp_Memory temp_LOG = begin_temp(arena); \
String_Const_u8 M = log_event(arena, E, \
string_u8_litexpr(__FILE__), \
__LINE__, (B), (V), (T));    \
log_call; \
end_temp(temp_LOG); )
#define LogEventLit(log_call, arena, B, V, T, Elit) \
LogEventStr(log_call, arena, (B), (V), (T), string_u8_litexpr(Elit))
#define LogEventF(log_call, arena, B, V, T, Ef, ...) \
Stmnt(Temp_Memory temp_LOG_F = begin_temp(arena); \
String_Const_u8 E = push_u8_stringf(arena, Ef, __VA_ARGS__); \
LogEventStr(log_call, arena, B, V, T, E); \
end_temp(temp_LOG_F); )

String_Const_u8
log_event(Arena *arena, String_Const_u8 event_name, String_Const_u8 src_name, i32 line_number, i32 buffer, i32 view, i32 thread_id);

#endif
