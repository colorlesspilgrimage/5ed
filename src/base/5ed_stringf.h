#if !defined(FCODER_5ED_STRINGF_H)
#define FCODER_5ED_STRINGF_H

#include <stdarg.h>
String_Const_u8
push_stringfv(Arena *arena, char *format, va_list args);
String_Const_u8
push_stringf(Arena *arena, char *format, ...);
String_Const_u8
push_u8_stringfv(Arena *arena, char *format, va_list args);
String_Const_u8
push_u8_stringf(Arena *arena, char *format, ...);
void
string_list_pushfv(Arena *arena, List_String_Const_char *list, char *format, va_list args);
void
string_list_pushf(Arena *arena, List_String_Const_char *list, char *format, ...);
void
string_list_pushfv(Arena *arena, List_String_Const_u8 *list, char *format, va_list args);
void
string_list_pushf(Arena *arena, List_String_Const_u8 *list, char *format, ...);
void
push_year_full(Arena *arena, List_String_Const_u8 *list, u32 year);
void
push_year_abrev(Arena *arena, List_String_Const_u8 *list, u32 year);
void
push_month_num(Arena *arena, List_String_Const_u8 *list, u8 mon);
void
push_month_num_zeros(Arena *arena, List_String_Const_u8 *list, u8 mon);
void
push_month_name(Arena *arena, List_String_Const_u8 *list, u8 mon);
void
push_month_abrev(Arena *arena, List_String_Const_u8 *list, u8 mon);
void
push_day_num(Arena *arena, List_String_Const_u8 *list, u8 day);
void
push_day_num_zeroes(Arena *arena, List_String_Const_u8 *list, u8 day);
void
push_day_ord(Arena *arena, List_String_Const_u8 *list, u8 day);
void
push_hour_24(Arena *arena, List_String_Const_u8 *list, u8 hour);
void
push_hour_24_zeroes(Arena *arena, List_String_Const_u8 *list, u8 hour);
void
push_hour_12(Arena *arena, List_String_Const_u8 *list, u8 hour);
void
push_hour_12_zeroes(Arena *arena, List_String_Const_u8 *list, u8 hour);
void
push_hour_am_pm(Arena *arena, List_String_Const_u8 *list, u8 hour);
void
push_minute(Arena *arena, List_String_Const_u8 *list, u8 min);
void
push_minute_zeroes(Arena *arena, List_String_Const_u8 *list, u8 min);
void
push_second(Arena *arena, List_String_Const_u8 *list, u8 sec);
void
push_second_zeroes(Arena *arena, List_String_Const_u8 *list, u8 sec);
void
push_millisecond_zeroes(Arena *arena, List_String_Const_u8 *list, u16 msec);
void
date_time_format(Arena *arena, List_String_Const_u8 *list, String_Const_u8 format, Date_Time *date_time);
void
date_time_format(Arena *arena, List_String_Const_u8 *list, char *format, Date_Time *date_time);
String_Const_u8
date_time_format(Arena *arena, String_Const_u8 format, Date_Time *date_time);
String_Const_u8
date_time_format(Arena *arena, char *format, Date_Time *date_time);

#endif
