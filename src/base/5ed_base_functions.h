#if !defined(FCODER_5ED_BASE_FUNCTIONS_H)
#define FCODER_5ED_BASE_FUNCTIONS_H

#include <math.h>

#define make_data_struct(s) make_data((s), sizeof(*(s)))
#define data_initr(m,s) {(u8*)(m), (s)}
#define data_initr_struct(s) {(u8*)(s), sizeof(*(s))}
#define data_initr_array(a) {(u8*)(a), sizeof(a)}
#define data_initr_string(s) {(u8*)(s), sizeof(s) - 1}
#define block_zero_struct(p) block_zero((p), sizeof(*(p)))
#define block_zero_array(a) block_zero((a), sizeof(a))
#define block_zero_dynamic_array(p,c) block_zero((p), sizeof(*(p))*(c))
#define block_copy_struct(d,s) block_copy((d), (s), sizeof(*(d)))
#define block_copy_array(d,s) block_copy((d), (s), sizeof(d))
#define block_copy_dynamic_array(d,s,c) block_copy((d), (s), sizeof(*(d))*(c))
#define block_match_struct(a,b) block_match((a), (b), sizeof(*(a)))
#define block_match_array(a,b) block_match((a), (b), sizeof(a))
#define block_range_copy(d,s,r,h) block_range_copy__inner((d),(s),Iu64(r),(i64)(h))
#define block_range_copy_sized(d,s,r,h,i) block_range_copy__inner((d),(s),Iu64(r),(i64)(h),(i))
#define block_range_copy_typed(d,s,r,h) block_range_copy_sized((d),(s),(r),(h),sizeof(*(d)))
#define block_copy_array_shift(d,s,r,h) block_copy_array_shift__inner((d),(s),sizeof(*(d)),(r),(h))
#define string_litexpr(s) SCchar((s), sizeof(s) - 1)
#define string_u8_litexpr(s) SCu8((u8*)(s), (u64)(sizeof(s) - 1))
#define string_u16_litexpr(s) SCu16((u16*)(s), (u64)(sizeof(s)/2 - 1))
#define string_expand(s) (i32)(s).size, (char*)(s).str
#define file_name_line_number_lit_u8 string_u8_litexpr(file_name_line_number)
#define base_allocate(a,s) base_allocate__inner((a), (s), file_name_line_number_lit_u8)
#define base_array_loc(a,T,c,l) (T*)(base_allocate__inner((a), sizeof(T)*(c), (l)).str)
#define base_array(a,T,c) base_array_loc(a,T,c, file_name_line_number_lit_u8)
#define push_array(a,T,c) ((T*)linalloc_wrap_unintialized(linalloc_push((a), sizeof(T)*(c), file_name_line_number_lit_u8)))
#define push_array_zero(a,T,c) ((T*)linalloc_wrap_zero(linalloc_push((a), sizeof(T)*(c), file_name_line_number_lit_u8)))
#define push_array_write(a,T,c,s) ((T*)linalloc_wrap_write(linalloc_push((a), sizeof(T)*(c), file_name_line_number_lit_u8), sizeof(T)*(c), (s)))
#define pop_array(a,T,c) (linalloc_pop((a), sizeof(T)*(c)))
#define push_align(a,b) (linalloc_align((a), (b)))
#define push_align_zero(a,b) (linalloc_wrap_zero(linalloc_align((a), (b))))
#define heap__sent_init(s) (s)->next=(s)->prev=(s)
#define heap__insert_next(p,n) ((n)->next=(p)->next,(n)->prev=(p),(n)->next->prev=(n),(p)->next=(n))
#define heap__insert_prev(p,n) ((n)->prev=(p)->prev,(n)->next=(p),(n)->prev->next=(n),(p)->prev=(n))
#define heap__remove(n) ((n)->next->prev=(n)->prev,(n)->prev->next=(n)->next)
#if !defined(DO_HEAP_CHECKS)
#define heap_assert_good(heap) ((void)(heap))
#endif
#define heap_array(heap, T, c) (T*)(heap_allocate((heap), sizeof(T)*(c)))
#define push_string_u8 string_u8_push
#define push_string_u16 string_u16_push
#define push_string_u32 string_u32_push
#define push_string_u64 string_u64_push
#define push_string_const_u8 string_const_u8_push
#define push_string_const_u16 string_const_u16_push
#define push_string_const_u32 string_const_u32_push
#define push_string_const_u64 string_const_u64_push
#define string_list_push_lit(a,l,s) string_list_push((a), (l), string_litexpr(s))
#define string_list_push_u8_lit(a,l,s) string_list_push((a), (l), string_u8_litexpr(s))
#define push_string_list string_list_push
#define push_string_list_lit(a,l,s) string_list_push_lit(a,l,s)
#define push_string_list_u8_lit(a,l,s) string_list_u8_push_lit(a,l,s)
#define push_string_list_overlap(a,l,o,s) string_list_push_overlap(a,l,o,s)

i32
i32_ceil32(f32 v);
i32
i32_floor32(f32 v);
i32
i32_round32(f32 v);
f32
f32_ceil32(f32 v);
f32
f32_floor32(f32 v);
f32
f32_round32(f32 v);
i8
round_up_i8(i8 x, i8 b);
u8
round_up_u8(u8 x, u8 b);
i16
round_up_i16(i16 x, i16 b);
u16
round_up_u16(u16 x, u16 b);
i32
round_up_i32(i32 x, i32 b);
u32
round_up_u32(u32 x, u32 b);
i64
round_up_i64(i64 x, i64 b);
u64
round_up_u64(u64 x, u64 b);
i8
round_down_i8(i8 x, i8 b);
u8
round_down_u8(u8 x, u8 b);
i16
round_down_i16(i16 x, i16 b);
u16
round_down_u16(u16 x, u16 b);
i32
round_down_i32(i32 x, i32 b);
u32
round_down_u32(u32 x, u32 b);
i64
round_down_i64(i64 x, i64 b);
u64
round_down_u64(u64 x, u64 b);
f32
f32_integer(f32 x);
u32
round_up_pot_u32(u32 x);
String_Const_u8
make_data(void *memory, u64 size);
extern const String_Const_u8 zero_data;
void
block_zero(void *mem, u64 size);
void
block_zero(String_Const_u8 data);
void
block_fill_ones(void *mem, u64 size);
void
block_fill_ones(String_Const_u8 data);
void
block_copy(void *dst, const void *src, u64 size);
b32
block_match(void *a, void *b, u64 size);
i32
block_compare(void *a, void *b, u64 size);
void
block_fill_u8(void *a, u64 size, u8 val);
void
block_fill_u16(void *a, u64 size, u16 val);
void
block_fill_u32(void *a, u64 size, u32 val);
void
block_fill_u64(void *a, u64 size, u64 val);
void
block_range_copy__inner(void *dst, void *src, Range_u64 range, i64 shift);
void
block_range_copy__inner(void *dst, void *src, Range_u64 range, i64 shift, u64 item_size);
void
block_copy_array_shift__inner(void *dst, void *src, u64 it_size, Range_i64 range, i64 shift);
void
block_copy_array_shift__inner(void *dst, void *src, u64 it_size, Range_i32 range, i64 shift);
f32
abs_f32(f32 x);
f32
mod_f32(f32 x, i32 m);
f32
sin_f32(f32 x);
f32
cos_f32(f32 x);
Vec2_i8
V2i8(i8 x, i8 y);
Vec3_i8
V3i8(i8 x, i8 y, i8 z);
Vec4_i8
V4i8(i8 x, i8 y, i8 z, i8 w);
Vec2_i16
V2i16(i16 x, i16 y);
Vec3_i16
V3i16(i16 x, i16 y, i16 z);
Vec4_i16
V4i16(i16 x, i16 y, i16 z, i16 w);
Vec2_i32
V2i32(i32 x, i32 y);
Vec3_i32
V3i32(i32 x, i32 y, i32 z);
Vec4_i32
V4i32(i32 x, i32 y, i32 z, i32 w);
Vec2_f32
V2f32(f32 x, f32 y);
Vec3_f32
V3f32(f32 x, f32 y, f32 z);
Vec4_f32
V4f32(f32 x, f32 y, f32 z, f32 w);
Vec2_i8
V2i8(Vec2_i8 o);
Vec2_i8
V2i8(Vec2_i16 o);
Vec2_i8
V2i8(Vec2_i32 o);
Vec2_i8
V2i8(Vec2_f32 o);
Vec3_i8
V3i8(Vec3_i8 o);
Vec3_i8
V3i8(Vec3_i16 o);
Vec3_i8
V3i8(Vec3_i32 o);
Vec3_i8
V3i8(Vec3_f32 o);
Vec4_i8
V4i8(Vec4_i8 o);
Vec4_i8
V4i8(Vec4_i16 o);
Vec4_i8
V4i8(Vec4_i32 o);
Vec4_i8
V4i8(Vec4_f32 o);
Vec2_i16
V2i16(Vec2_i8 o);
Vec2_i16
V2i16(Vec2_i16 o);
Vec2_i16
V2i16(Vec2_i32 o);
Vec2_i16
V2i16(Vec2_f32 o);
Vec3_i16
V3i16(Vec3_i8 o);
Vec3_i16
V3i16(Vec3_i16 o);
Vec3_i16
V3i16(Vec3_i32 o);
Vec3_i16
V3i16(Vec3_f32 o);
Vec4_i16
V4i16(Vec4_i8 o);
Vec4_i16
V4i16(Vec4_i16 o);
Vec4_i16
V4i16(Vec4_i32 o);
Vec4_i16
V4i16(Vec4_f32 o);
Vec2_i32
V2i32(Vec2_i8 o);
Vec2_i32
V2i32(Vec2_i16 o);
Vec2_i32
V2i32(Vec2_i32 o);
Vec2_i32
V2i32(Vec2_f32 o);
Vec3_i32
V3i32(Vec3_i8 o);
Vec3_i32
V3i32(Vec3_i16 o);
Vec3_i32
V3i32(Vec3_i32 o);
Vec3_i32
V3i32(Vec3_f32 o);
Vec4_i32
V4i32(Vec4_i8 o);
Vec4_i32
V4i32(Vec4_i16 o);
Vec4_i32
V4i32(Vec4_i32 o);
Vec4_i32
V4i32(Vec4_f32 o);
Vec2_f32
V2f32(Vec2_i8 o);
Vec2_f32
V2f32(Vec2_i16 o);
Vec2_f32
V2f32(Vec2_i32 o);
Vec2_f32
V2f32(Vec2_f32 o);
Vec3_f32
V3f32(Vec3_i8 o);
Vec3_f32
V3f32(Vec3_i16 o);
Vec3_f32
V3f32(Vec3_i32 o);
Vec3_f32
V3f32(Vec3_f32 o);
Vec4_f32
V4f32(Vec4_i8 o);
Vec4_f32
V4f32(Vec4_i16 o);
Vec4_f32
V4f32(Vec4_i32 o);
Vec4_f32
V4f32(Vec4_f32 o);
Vec2_i8
operator+(Vec2_i8 a, Vec2_i8 b);
Vec3_i8
operator+(Vec3_i8 a, Vec3_i8 b);
Vec4_i8
operator+(Vec4_i8 a, Vec4_i8 b);
Vec2_i16
operator+(Vec2_i16 a, Vec2_i16 b);
Vec3_i16
operator+(Vec3_i16 a, Vec3_i16 b);
Vec4_i16
operator+(Vec4_i16 a, Vec4_i16 b);
Vec2_i32
operator+(Vec2_i32 a, Vec2_i32 b);
Vec3_i32
operator+(Vec3_i32 a, Vec3_i32 b);
Vec4_i32
operator+(Vec4_i32 a, Vec4_i32 b);
Vec2_f32
operator+(Vec2_f32 a, Vec2_f32 b);
Vec3_f32
operator+(Vec3_f32 a, Vec3_f32 b);
Vec4_f32
operator+(Vec4_f32 a, Vec4_f32 b);
Vec2_i8&
operator+=(Vec2_i8 &a, Vec2_i8 b);
Vec3_i8&
operator+=(Vec3_i8 &a, Vec3_i8 b);
Vec4_i8&
operator+=(Vec4_i8 &a, Vec4_i8 b);
Vec2_i16&
operator+=(Vec2_i16 &a, Vec2_i16 b);
Vec3_i16&
operator+=(Vec3_i16 &a, Vec3_i16 b);
Vec4_i16&
operator+=(Vec4_i16 &a, Vec4_i16 b);
Vec2_i32&
operator+=(Vec2_i32 &a, Vec2_i32 b);
Vec3_i32&
operator+=(Vec3_i32 &a, Vec3_i32 b);
Vec4_i32&
operator+=(Vec4_i32 &a, Vec4_i32 b);
Vec2_f32&
operator+=(Vec2_f32 &a, Vec2_f32 b);
Vec3_f32&
operator+=(Vec3_f32 &a, Vec3_f32 b);
Vec4_f32&
operator+=(Vec4_f32 &a, Vec4_f32 b);
Vec2_i8
operator-(Vec2_i8 a, Vec2_i8 b);
Vec3_i8
operator-(Vec3_i8 a, Vec3_i8 b);
Vec4_i8
operator-(Vec4_i8 a, Vec4_i8 b);
Vec2_i16
operator-(Vec2_i16 a, Vec2_i16 b);
Vec3_i16
operator-(Vec3_i16 a, Vec3_i16 b);
Vec4_i16
operator-(Vec4_i16 a, Vec4_i16 b);
Vec2_i32
operator-(Vec2_i32 a, Vec2_i32 b);
Vec3_i32
operator-(Vec3_i32 a, Vec3_i32 b);
Vec4_i32
operator-(Vec4_i32 a, Vec4_i32 b);
Vec2_f32
operator-(Vec2_f32 a, Vec2_f32 b);
Vec3_f32
operator-(Vec3_f32 a, Vec3_f32 b);
Vec4_f32
operator-(Vec4_f32 a, Vec4_f32 b);
Vec2_i8&
operator-=(Vec2_i8 &a, Vec2_i8 b);
Vec3_i8&
operator-=(Vec3_i8 &a, Vec3_i8 b);
Vec4_i8&
operator-=(Vec4_i8 &a, Vec4_i8 b);
Vec2_i16&
operator-=(Vec2_i16 &a, Vec2_i16 b);
Vec3_i16&
operator-=(Vec3_i16 &a, Vec3_i16 b);
Vec4_i16&
operator-=(Vec4_i16 &a, Vec4_i16 b);
Vec2_i32&
operator-=(Vec2_i32 &a, Vec2_i32 b);
Vec3_i32&
operator-=(Vec3_i32 &a, Vec3_i32 b);
Vec4_i32&
operator-=(Vec4_i32 &a, Vec4_i32 b);
Vec2_f32&
operator-=(Vec2_f32 &a, Vec2_f32 b);
Vec3_f32&
operator-=(Vec3_f32 &a, Vec3_f32 b);
Vec4_f32&
operator-=(Vec4_f32 &a, Vec4_f32 b);
Vec2_i8
operator*(i8 s, Vec2_i8 v);
Vec2_i8
operator*(Vec2_i8 v, i8 s);
Vec3_i8
operator*(i8 s, Vec3_i8 v);
Vec3_i8
operator*(Vec3_i8 v, i8 s);
Vec4_i8
operator*(i8 s, Vec4_i8 v);
Vec4_i8
operator*(Vec4_i8 v, i8 s);
Vec2_i16
operator*(i16 s, Vec2_i16 v);
Vec2_i16
operator*(Vec2_i16 v, i16 s);
Vec3_i16
operator*(i16 s, Vec3_i16 v);
Vec3_i16
operator*(Vec3_i16 v, i16 s);
Vec4_i16
operator*(i16 s, Vec4_i16 v);
Vec4_i16
operator*(Vec4_i16 v, i16 s);
Vec2_i32
operator*(i32 s, Vec2_i32 v);
Vec2_i32
operator*(Vec2_i32 v, i32 s);
Vec3_i32
operator*(i32 s, Vec3_i32 v);
Vec3_i32
operator*(Vec3_i32 v, i32 s);
Vec4_i32
operator*(i32 s, Vec4_i32 v);
Vec4_i32
operator*(Vec4_i32 v, i32 s);
Vec2_f32
operator*(f32 s, Vec2_f32 v);
Vec2_f32
operator*(Vec2_f32 v, f32 s);
Vec3_f32
operator*(f32 s, Vec3_f32 v);
Vec3_f32
operator*(Vec3_f32 v, f32 s);
Vec4_f32
operator*(f32 s, Vec4_f32 v);
Vec4_f32
operator*(Vec4_f32 v, f32 s);
Vec2_i8&
operator*=(Vec2_i8 &v, i8 s);
Vec3_i8&
operator*=(Vec3_i8 &v, i8 s);
Vec4_i8&
operator*=(Vec4_i8 &v, i8 s);
Vec2_i16&
operator*=(Vec2_i16 &v, i16 s);
Vec3_i16&
operator*=(Vec3_i16 &v, i16 s);
Vec4_i16&
operator*=(Vec4_i16 &v, i16 s);
Vec2_i32&
operator*=(Vec2_i32 &v, i32 s);
Vec3_i32&
operator*=(Vec3_i32 &v, i32 s);
Vec4_i32&
operator*=(Vec4_i32 &v, i32 s);
Vec2_f32&
operator*=(Vec2_f32 &v, f32 s);
Vec3_f32&
operator*=(Vec3_f32 &v, f32 s);
Vec4_f32&
operator*=(Vec4_f32 &v, f32 s);
Vec2_i8
operator/(Vec2_i8 v, i8 s);
Vec3_i8
operator/(Vec3_i8 v, i8 s);
Vec4_i8
operator/(Vec4_i8 v, i8 s);
Vec2_i16
operator/(Vec2_i16 v, i16 s);
Vec3_i16
operator/(Vec3_i16 v, i16 s);
Vec4_i16
operator/(Vec4_i16 v, i16 s);
Vec2_i32
operator/(Vec2_i32 v, i32 s);
Vec3_i32
operator/(Vec3_i32 v, i32 s);
Vec4_i32
operator/(Vec4_i32 v, i32 s);
Vec2_f32
operator/(Vec2_f32 v, f32 s);
Vec3_f32
operator/(Vec3_f32 v, f32 s);
Vec4_f32
operator/(Vec4_f32 v, f32 s);
Vec2_i8&
operator/=(Vec2_i8 &v, i8 s);
Vec3_i8&
operator/=(Vec3_i8 &v, i8 s);
Vec4_i8&
operator/=(Vec4_i8 &v, i8 s);
Vec2_i16&
operator/=(Vec2_i16 &v, i16 s);
Vec3_i16&
operator/=(Vec3_i16 &v, i16 s);
Vec4_i16&
operator/=(Vec4_i16 &v, i16 s);
Vec2_i32&
operator/=(Vec2_i32 &v, i32 s);
Vec3_i32&
operator/=(Vec3_i32 &v, i32 s);
Vec4_i32&
operator/=(Vec4_i32 &v, i32 s);
Vec2_f32&
operator/=(Vec2_f32 &v, f32 s);
Vec3_f32&
operator/=(Vec3_f32 &v, f32 s);
Vec4_f32&
operator/=(Vec4_f32 &v, f32 s);
b32
operator==(Vec2_i8 a, Vec2_i8 b);
b32
operator==(Vec3_i8 a, Vec3_i8 b);
b32
operator==(Vec4_i8 a, Vec4_i8 b);
b32
operator==(Vec2_i16 a, Vec2_i16 b);
b32
operator==(Vec3_i16 a, Vec3_i16 b);
b32
operator==(Vec4_i16 a, Vec4_i16 b);
b32
operator==(Vec2_i32 a, Vec2_i32 b);
b32
operator==(Vec3_i32 a, Vec3_i32 b);
b32
operator==(Vec4_i32 a, Vec4_i32 b);
b32
operator==(Vec2_f32 a, Vec2_f32 b);
b32
operator==(Vec3_f32 a, Vec3_f32 b);
b32
operator==(Vec4_f32 a, Vec4_f32 b);
b32
operator!=(Vec2_i8 a, Vec2_i8 b);
b32
operator!=(Vec3_i8 a, Vec3_i8 b);
b32
operator!=(Vec4_i8 a, Vec4_i8 b);
b32
operator!=(Vec2_i16 a, Vec2_i16 b);
b32
operator!=(Vec3_i16 a, Vec3_i16 b);
b32
operator!=(Vec4_i16 a, Vec4_i16 b);
b32
operator!=(Vec2_i32 a, Vec2_i32 b);
b32
operator!=(Vec3_i32 a, Vec3_i32 b);
b32
operator!=(Vec4_i32 a, Vec4_i32 b);
b32
operator!=(Vec2_f32 a, Vec2_f32 b);
b32
operator!=(Vec3_f32 a, Vec3_f32 b);
b32
operator!=(Vec4_f32 a, Vec4_f32 b);
b32
near_zero(f32 p, f32 epsilon);
b32
near_zero(Vec2_f32 p, f32 epsilon);
b32
near_zero(Vec3_f32 p, f32 epsilon);
b32
near_zero(Vec4_f32 p, f32 epsilon);
b32
near_zero(f32 p);
b32
near_zero(Vec2_f32 p);
b32
near_zero(Vec3_f32 p);
b32
near_zero(Vec4_f32 p);
Vec2_f32
hadamard(Vec2_f32 a, Vec2_f32 b);
Vec3_f32
hadamard(Vec3_f32 a, Vec3_f32 b);
Vec4_f32
hadamard(Vec4_f32 a, Vec4_f32 b);
f32
lerp(f32 a, f32 t, f32 b);
f32
lerp(f32 t, Range_f32 x);
i32
lerp(i32 a, f32 t, i32 b);
Vec2_f32
lerp(Vec2_f32 a, f32 t, Vec2_f32 b);
Vec3_f32
lerp(Vec3_f32 a, f32 t, Vec3_f32 b);
Vec4_f32
lerp(Vec4_f32 a, f32 t, Vec4_f32 b);
f32
unlerp(f32 a, f32 x, f32 b);
f32
unlerp(u64 a, u64 x, u64 b);
Range_f32
unlerp(f32 a, Range_f32 x, f32 b);
Range_f32
lerp(f32 a, Range_f32 x, f32 b);
f32
lerp(Range_f32 range, f32 t);
f32
clamp_range(Range_f32 range, f32 x);
b32
operator==(Rect_i32 a, Rect_i32 b);
b32
operator==(Rect_f32 a, Rect_f32 b);
b32
operator!=(Rect_i32 a, Rect_i32 b);
b32
operator!=(Rect_f32 a, Rect_f32 b);
Vec4_f32
unpack_color(ARGB_Color color);
ARGB_Color
pack_color(Vec4_f32 color);
ARGB_Color
color_blend(ARGB_Color a, f32 t, ARGB_Color b);
Vec4_f32
rgba_to_hsla(Vec4_f32 rgba);
Vec4_f32
hsla_to_rgba(Vec4_f32 hsla);
Range_i32
Ii32(i32 a, i32 b);
Range_i64
Ii64(i64 a, i64 b);
Range_u64
Iu64(u64 a, u64 b);
Range_f32
If32(f32 a, f32 b);
Range_i32
Ii32_size(i32 pos, i32 size);
Range_i64
Ii64_size(i64 pos, i64 size);
Range_u64
Iu64_size(u64 pos, u64 size);
Range_f32
If32_size(f32 pos, f32 size);
Range_i32
Ii32(i32 a);
Range_i64
Ii64(i64 a);
Range_u64
Iu64(u64 a);
Range_f32
If32(f32 a);
Range_i32
Ii32();
Range_i64
Ii64();
Range_u64
Iu64();
Range_f32
If32();
Range_u64
Iu64(Range_i32 r);
extern Range_i32 Ii32_neg_inf;
extern Range_i64 Ii64_neg_inf;
extern Range_u64 Iu64_neg_inf;
extern Range_f32 If32_neg_inf;
b32
operator==(Range_i32 a, Range_i32 b);
b32
operator==(Range_i64 a, Range_i64 b);
b32
operator==(Range_u64 a, Range_u64 b);
b32
operator==(Range_f32 a, Range_f32 b);
Range_i32
operator+(Range_i32 r, i32 s);
Range_i64
operator+(Range_i64 r, i64 s);
Range_u64
operator+(Range_u64 r, u64 s);
Range_f32
operator+(Range_f32 r, f32 s);
Range_i32
operator-(Range_i32 r, i32 s);
Range_i64
operator-(Range_i64 r, i64 s);
Range_u64
operator-(Range_u64 r, u64 s);
Range_f32
operator-(Range_f32 r, f32 s);
Range_i32&
operator+=(Range_i32 &r, i32 s);
Range_i64&
operator+=(Range_i64 &r, i64 s);
Range_u64&
operator+=(Range_u64 &r, u64 s);
Range_f32&
operator+=(Range_f32 &r, f32 s);
Range_i32&
operator-=(Range_i32 &r, i32 s);
Range_i64&
operator-=(Range_i64 &r, i64 s);
Range_u64&
operator-=(Range_u64 &r, u64 s);
Range_f32&
operator-=(Range_f32 &r, f32 s);
Range_i32
range_margin(Range_i32 range, i32 margin);
Range_i64
range_margin(Range_i64 range, i64 margin);
Range_u64
range_margin(Range_u64 range, u64 margin);
Range_f32
range_margin(Range_f32 range, f32 margin);
b32
range_overlap(Range_i32 a, Range_i32 b);
b32
range_overlap(Range_i64 a, Range_i64 b);
b32
range_overlap(Range_u64 a, Range_u64 b);
b32
range_overlap(Range_f32 a, Range_f32 b);
Range_i32
range_intersect(Range_i32 a, Range_i32 b);
Range_i64
range_intersect(Range_i64 a, Range_i64 b);
Range_u64
range_intersect(Range_u64 a, Range_u64 b);
Range_f32
range_intersect(Range_f32 a, Range_f32 b);
Range_i32
range_union(Range_i32 a, Range_i32 b);
Range_i64
range_union(Range_i64 a, Range_i64 b);
Range_u64
range_union(Range_u64 a, Range_u64 b);
Range_f32
range_union(Range_f32 a, Range_f32 b);
b32
range_contains_inclusive(Range_i32 a, i32 p);
b32
range_contains_inclusive(Range_i64 a, i64 p);
b32
range_contains_inclusive(Range_u64 a, u64 p);
b32
range_inclusive_contains(Range_f32 a, f32 p);
b32
range_contains(Range_i32 a, i32 p);
b32
range_contains(Range_i64 a, i64 p);
b32
range_contains(Range_u64 a, u64 p);
b32
range_contains(Range_f32 a, f32 p);
i32
range_size(Range_i32 a);
i64
range_size(Range_i64 a);
u64
range_size(Range_u64 a);
f32
range_size(Range_f32 a);
i32
range_size_inclusive(Range_i32 a);
i64
range_size_inclusive(Range_i64 a);
u64
range_size_inclusive(Range_u64 a);
f32
range_size_inclusive(Range_f32 a);
Range_i32
rectify(Range_i32 a);
Range_i64
rectify(Range_i64 a);
Range_u64
rectify(Range_u64 a);
Range_f32
rectify(Range_f32 a);
Range_i32
range_clamp_size(Range_i32 a, i32 max_size);
Range_i64
range_clamp_size(Range_i64 a, i64 max_size);
Range_u64
range_clamp_size(Range_u64 a, u64 max_size);
Range_f32
range_clamp_size(Range_f32 a, f32 max_size);
b32
range_is_valid(Range_i32 a);
b32
range_is_valid(Range_i64 a);
b32
range_is_valid(Range_u64 a);
b32
range_is_valid(Range_f32 a);
i32
range_side(Range_i32 a, Side side);
i64
range_side(Range_i64 a, Side side);
u64
range_side(Range_u64 a, Side side);
f32
range_side(Range_f32 a, Side side);
i32
range_distance(Range_i32 a, Range_i32 b);
i64
range_distance(Range_i64 a, Range_i64 b);
u64
range_distance(Range_u64 a, Range_u64 b);
f32
range_distance(Range_f32 a, Range_f32 b);
i32
replace_range_shift(i32 replace_length, i32 insert_length);
i32
replace_range_shift(i32 start, i32 end, i32 insert_length);
i32
replace_range_shift(Range_i32 range, i32 insert_length);
i64
replace_range_shift(i64 replace_length, i64 insert_length);
i64
replace_range_shift(i64 start, i64 end, i64 insert_length);
i64
replace_range_shift(Range_i64 range, i64 insert_length);
i64
replace_range_shift(u64 replace_length, u64 insert_length);
i64
replace_range_shift(i64 start, i64 end, u64 insert_length);
i64
replace_range_shift(Range_i64 range, u64 insert_length);
Rect_i32
Ri32(i32 x0, i32 y0, i32 x1, i32 y1);
Rect_f32
Rf32(f32 x0, f32 y0, f32 x1, f32 y1);
Rect_i32
Ri32(Vec2_i32 p0, Vec2_i32 p1);
Rect_f32
Rf32(Vec2_f32 p0, Vec2_f32 p1);
Rect_i32
Ri32(Rect_f32 o);
Rect_f32
Rf32(Rect_i32 o);
Rect_i32
Ri32_xy_wh(i32 x0, i32 y0, i32 w, i32 h);
Rect_f32
Rf32_xy_wh(f32 x0, f32 y0, f32 w, f32 h);
Rect_i32
Ri32_xy_wh(Vec2_i32 p0, Vec2_i32 d);
Rect_f32
Rf32_xy_wh(Vec2_f32 p0, Vec2_f32 d);
Rect_i32
Ri32(Range_i32 x, Range_i32 y);
Rect_f32
Rf32(Range_f32 x, Range_f32 y);
extern const Rect_f32 Rf32_infinity;
extern const Rect_f32 Rf32_negative_infinity;
extern const Rect_i32 Ri32_infinity;
extern const Rect_i32 Ri32_negative_infinity;
b32
rect_equals(Rect_i32 a, Rect_i32 b);
b32
rect_equals(Rect_f32 a, Rect_f32 b);
b32
rect_contains_point(Rect_i32 a, Vec2_i32 b);
b32
rect_contains_point(Rect_f32 a, Vec2_f32 b);
Rect_i32
rect_inner(Rect_i32 r, i32 m);
Rect_f32
rect_inner(Rect_f32 r, f32 m);
Vec2_i32
rect_dim(Rect_i32 r);
Range_i32
rect_x(Rect_i32 r);
Range_i32
rect_y(Rect_i32 r);
i32
rect_width(Rect_i32 r);
i32
rect_height(Rect_i32 r);
Vec2_f32
rect_dim(Rect_f32 r);
Range_f32
rect_x(Rect_f32 r);
Range_f32
rect_y(Rect_f32 r);
f32
rect_width(Rect_f32 r);
f32
rect_height(Rect_f32 r);
Vec2_i32
rect_center(Rect_i32 r);
Vec2_f32
rect_center(Rect_f32 r);
Range_i32
rect_range_x(Rect_i32 r);
Range_i32
rect_range_y(Rect_i32 r);
Range_f32
rect_range_x(Rect_f32 r);
Range_f32
rect_range_y(Rect_f32 r);
i32
rect_area(Rect_i32 r);
f32
rect_area(Rect_f32 r);
b32
rect_overlap(Rect_i32 a, Rect_i32 b);
b32
rect_overlap(Rect_f32 a, Rect_f32 b);
Vec2_i32
rect_half_dim(Rect_i32 r);
Vec2_f32
rect_half_dim(Rect_f32 r);
Rect_i32
rect_intersect(Rect_i32 a, Rect_i32 b);
Rect_i32
rect_union(Rect_i32 a, Rect_i32 b);
Rect_f32
rect_intersect(Rect_f32 a, Rect_f32 b);
Rect_f32
rect_union(Rect_f32 a, Rect_f32 b);
Rect_f32_Pair
rect_split_top_bottom__inner(Rect_f32 rect, f32 y);
Rect_f32_Pair
rect_split_left_right__inner(Rect_f32 rect, f32 x);
Rect_f32_Pair
rect_split_top_bottom(Rect_f32 rect, f32 y);
Rect_f32_Pair
rect_split_left_right(Rect_f32 rect, f32 x);
Rect_f32_Pair
rect_split_top_bottom_neg(Rect_f32 rect, f32 y);
Rect_f32_Pair
rect_split_left_right_neg(Rect_f32 rect, f32 x);
Rect_f32_Pair
rect_split_top_bottom_lerp(Rect_f32 rect, f32 t);
Rect_f32_Pair
rect_split_left_right_lerp(Rect_f32 rect, f32 t);
Scan_Direction
flip_direction(Scan_Direction direction);
Side
flip_side(Side side);
u64
cstring_length(char *str);
u64
cstring_length(u8 *str);
u64
cstring_length(u16 *str);
u64
cstring_length(u32 *str);
String_char
Schar(char *str, u64 size, u64 cap);
String_u8
Su8(u8 *str, u64 size, u64 cap);
String_u16
Su16(u16 *str, u64 size, u64 cap);
String_u32
Su32(u32 *str, u64 size, u64 cap);
String_Any
Sany(void *str, u64 size, u64 cap, String_Encoding encoding);
String_char
Schar(char *str, u64 size);
String_u8
Su8(u8 *str, u64 size);
String_u16
Su16(u16 *str, u64 size);
String_u32
Su32(u32 *str, u64 size);
String_Any
Sany(void *str, u64 size, String_Encoding encoding);
String_char
Schar(char *str, char *one_past_last);
String_u8
Su8(u8 *str, u8 *one_past_last);
String_u16
Su16(u16 *str, u16 *one_past_last);
String_u32
Su32(u32 *str, u32 *one_past_last);
String_Any
Sany(void *str, void *one_past_last, String_Encoding encoding);
String_char
Schar(char *str);
String_u8
Su8(u8 *str);
String_u16
Su16(u16 *str);
String_u32
Su32(u32 *str);
String_Any
Sany(void *str, String_Encoding encoding);
String_char
Schar(String_Const_char str, u64 cap);
String_u8
Su8(String_Const_u8 str, u64 cap);
String_u16
Su16(String_Const_u16 str, u64 cap);
String_u32
Su32(String_Const_u32 str, u64 cap);
String_Any
SCany(String_char str);
String_Any
SCany(String_u8 str);
String_Any
SCany(String_u16 str);
String_Any
SCany(String_u32 str);
String_Const_char
SCchar(char *str, u64 size);
String_Const_u8
SCu8(u8 *str, u64 size);
String_Const_u16
SCu16(u16 *str, u64 size);
String_Const_u32
SCu32(u32 *str, u64 size);
String_Const_Any
SCany(void *str, u64 size, String_Encoding encoding);
String_Const_char
SCchar(void);
String_Const_u8
SCu8(void);
String_Const_u16
SCu16(void);
String_Const_u32
SCu32(void);
String_Const_char
SCchar(char *str, char *one_past_last);
String_Const_u8
SCu8(u8 *str, u8 *one_past_last);
String_Const_u16
SCu16(u16 *str, u16 *one_past_last);
String_Const_u32
SCu32(u32 *str, u32 *one_past_last);
String_Const_Any
SCany(void *str, void *one_past_last, String_Encoding encoding);
String_Const_char
SCchar(char *str);
String_Const_u8
SCu8(u8 *str);
String_Const_u16
SCu16(u16 *str);
String_Const_u32
SCu32(u32 *str);
String_Const_char
SCchar(String_char string);
String_Const_u8
SCu8(String_u8 string);
String_Const_u16
SCu16(String_u16 string);
String_Const_u32
SCu32(String_u32 string);
String_Const_char
SCchar(String_Const_u8 str);
String_Const_u8
SCu8(String_Const_char str);
String_Const_u8
SCu8(char *str, u64 length);
String_Const_u8
SCu8(char *first, char *one_past_last);
String_Const_u8
SCu8(char *str);
String_Const_u16
SCu16(wchar_t *str, u64 size);
String_Const_u16
SCu16(wchar_t *str);
String_Const_Any
SCany(void *str, String_Encoding encoding);
String_Const_Any
SCany(String_Const_char str);
String_Const_Any
SCany(String_Const_u8 str);
String_Const_Any
SCany(String_Const_u16 str);
String_Const_Any
SCany(String_Const_u32 str);
extern String_Const_char string_empty;
extern String_Const_u8 string_u8_empty;
void*
base_reserve__noop(void *user_data, u64 size, u64 *size_out, String_Const_u8 location);
void
base_commit__noop(void *user_data, void *ptr, u64 size);
void
base_uncommit__noop(void *user_data, void *ptr, u64 size);
void
base_free__noop(void *user_data, void *ptr);
void
base_set_access__noop(void *user_data, void *ptr, u64 size, Access_Flag flags);
Base_Allocator
make_base_allocator(Base_Allocator_Reserve_Signature *func_reserve,
                    Base_Allocator_Commit_Signature *func_commit,
                    Base_Allocator_Uncommit_Signature *func_uncommit,
                    Base_Allocator_Free_Signature *func_free,
                    Base_Allocator_Set_Access_Signature *func_set_access,
                    void *user_data);
String_Const_u8
base_allocate__inner(Base_Allocator *allocator, u64 size, String_Const_u8 location);
void
base_free(Base_Allocator *allocator, void *ptr);
Cursor
make_cursor(void *base, u64 size);
Cursor
make_cursor(String_Const_u8 data);
Cursor
make_cursor(Base_Allocator *allocator, u64 size);
String_Const_u8
linalloc_push(Cursor *cursor, u64 size, String_Const_u8 location);
void
linalloc_pop(Cursor *cursor, u64 size);
String_Const_u8
linalloc_align(Cursor *cursor, u64 alignment);
Temp_Memory_Cursor
linalloc_begin_temp(Cursor *cursor);
void
linalloc_end_temp(Temp_Memory_Cursor temp);
void
linalloc_clear(Cursor *cursor);
Arena
make_arena(Base_Allocator *allocator, u64 chunk_size, u64 alignment);
Arena
make_arena(Base_Allocator *allocator, u64 chunk_size);
Arena
make_arena(Base_Allocator *allocator);
Cursor_Node*
arena__new_node(Arena *arena, u64 min_size, String_Const_u8 location);
String_Const_u8
linalloc_push(Arena *arena, u64 size, String_Const_u8 location);
void
linalloc_pop(Arena *arena, u64 size);
String_Const_u8
linalloc_align(Arena *arena, u64 alignment);
Temp_Memory_Arena
linalloc_begin_temp(Arena *arena);
void
linalloc_end_temp(Temp_Memory_Arena temp);
void
linalloc_clear(Arena *arena);
void*
linalloc_wrap_unintialized(String_Const_u8 data);
void*
linalloc_wrap_zero(String_Const_u8 data);
void*
linalloc_wrap_write(String_Const_u8 data, u64 size, void *src);
Temp_Memory
begin_temp(Cursor *cursor);
Temp_Memory
begin_temp(Arena *arena);
void
end_temp(Temp_Memory temp);
void
thread_ctx_init(Thread_Context *tctx, Thread_Kind kind, Base_Allocator *allocator,
                Base_Allocator *prof_allocator);
void
thread_ctx_release(Thread_Context *tctx);
Arena_Node*
tctx__alloc_arena_node(Thread_Context *tctx);
void
tctx__free_arena_node(Thread_Context *tctx, Arena_Node *node);
Arena*
tctx_reserve(Thread_Context *tctx);
Arena*
tctx_reserve(Thread_Context *tctx, Arena *a1);
Arena*
tctx_reserve(Thread_Context *tctx, Arena *a1, Arena *a2);
Arena*
tctx_reserve(Thread_Context *tctx, Arena *a1, Arena *a2, Arena *a3);
void
tctx_release(Thread_Context *tctx, Arena *arena);
#if defined(DO_HEAP_CHECKS)
void
heap_assert_good(Heap *heap);
#endif
void
heap_init(Heap *heap, Base_Allocator *allocator);
void
heap_init(Heap *heap, Arena *arena);
Base_Allocator*
heap_get_base_allocator(Heap *heap);
void
heap_free_all(Heap *heap);
void
heap__extend(Heap *heap, void *memory, u64 size);
void
heap__extend_automatic(Heap *heap, u64 size);
void*
heap__reserve_chunk(Heap *heap, Heap_Node *node, u64 size);
void*
heap_allocate(Heap *heap, u64 size);
void
heap__merge(Heap *heap, Heap_Node *l, Heap_Node *r);
void
heap_free(Heap *heap, void *memory);
void*
base_reserve__heap(void *user_data, u64 size, u64 *size_out, String_Const_u8 location);
void
base_free__heap(void *user_data, void *ptr);
Base_Allocator
base_allocator_on_heap(Heap *heap);
String_Const_u8
push_data(Arena *arena, u64 size);
String_Const_u8
push_data_copy(Arena *arena, String_Const_u8 data);
b32
data_match(String_Const_u8 a, String_Const_u8 b);
b32
character_is_basic_ascii(char c);
b32
character_is_basic_ascii(u8 c);
b32
character_is_basic_ascii(u16 c);
b32
character_is_basic_ascii(u32 c);
b32
character_is_slash(char c);
b32
character_is_slash(u8 c);
b32
character_is_slash(u16 c);
b32
character_is_slash(u32 c);
b32
character_is_upper(char c);
b32
character_is_upper(u8 c);
b32
character_is_upper(u16 c);
b32
character_is_upper(u32 c);
b32
character_is_lower(char c);
b32
character_is_lower(u8 c);
b32
character_is_lower(u16 c);
b32
character_is_lower(u32 c);
b32
character_is_lower_unicode(u8 c);
b32
character_is_lower_unicode(u16 c);
b32
character_is_lower_unicode(u32 c);
char
character_to_upper(char c);
u8
character_to_upper(u8 c);
u16
character_to_upper(u16 c);
u32
character_to_upper(u32 c);
char
character_to_lower(char c);
u8
character_to_lower(u8 c);
u16
character_to_lower(u16 c);
u32
character_to_lower(u32 c);
b32
character_is_whitespace(char c);
b32
character_is_whitespace(u8 c);
b32
character_is_whitespace(u16 c);
b32
character_is_whitespace(u32 c);
b32
character_is_base10(char c);
b32
character_is_base10(u8 c);
b32
character_is_base10(u16 c);
b32
character_is_base10(u32 c);
b32
character_is_base16(char c);
b32
character_is_base16(u8 c);
b32
character_is_base16(u16 c);
b32
character_is_base16(u32 c);
b32
character_is_base64(char c);
b32
character_is_base64(u8 c);
b32
character_is_base64(u16 c);
b32
character_is_base64(u32 c);
b32
character_is_alpha(char c);
b32
character_is_alpha(u8 c);
b32
character_is_alpha(u16 c);
b32
character_is_alpha(u32 c);
b32
character_is_alpha_numeric(char c);
b32
character_is_alpha_numeric(u8 c);
b32
character_is_alpha_numeric(u16 c);
b32
character_is_alpha_numeric(u32 c);
b32
character_is_alpha_unicode(u8 c);
b32
character_is_alpha_unicode(u16 c);
b32
character_is_alpha_unicode(u32 c);
b32
character_is_alpha_numeric_unicode(u8 c);
b32
character_is_alpha_numeric_unicode(u16 c);
b32
character_is_alpha_numeric_unicode(u32 c);
char
string_get_character(String_Const_char str, u64 i);
u8
string_get_character(String_Const_u8 str, u64 i);
u16
string_get_character(String_Const_u16 str, u64 i);
u32
string_get_character(String_Const_u32 str, u64 i);
String_Const_char
string_prefix(String_Const_char str, u64 size);
String_Const_u8
string_prefix(String_Const_u8 str, u64 size);
String_Const_u16
string_prefix(String_Const_u16 str, u64 size);
String_Const_u32
string_prefix(String_Const_u32 str, u64 size);
String_Const_Any
string_prefix(String_Const_Any str, u64 size);
String_Const_char
string_postfix(String_Const_char str, u64 size);
String_Const_u8
string_postfix(String_Const_u8 str, u64 size);
String_Const_u16
string_postfix(String_Const_u16 str, u64 size);
String_Const_u32
string_postfix(String_Const_u32 str, u64 size);
String_Const_Any
string_postfix(String_Const_Any str, u64 size);
String_Const_char
string_skip(String_Const_char str, u64 n);
String_Const_u8
string_skip(String_Const_u8 str, u64 n);
String_Const_u16
string_skip(String_Const_u16 str, u64 n);
String_Const_u32
string_skip(String_Const_u32 str, u64 n);
String_Const_Any
string_skip(String_Const_Any str, u64 n);
String_Const_char
string_chop(String_Const_char str, u64 n);
String_Const_u8
string_chop(String_Const_u8 str, u64 n);
String_Const_u16
string_chop(String_Const_u16 str, u64 n);
String_Const_u32
string_chop(String_Const_u32 str, u64 n);
String_Const_Any
string_chop(String_Const_Any str, u64 n);
String_Const_char
string_substring(String_Const_char str, Range_i64 range);
String_Const_u8
string_substring(String_Const_u8 str, Range_i64 range);
String_Const_u16
string_substring(String_Const_u16 str, Range_i64 range);
String_Const_u32
string_substring(String_Const_u32 str, Range_i64 range);
u64
string_find_first(String_Const_char str, u64 start_pos, char c);
u64
string_find_first(String_Const_u8 str, u64 start_pos, u8 c);
u64
string_find_first(String_Const_u16 str, u64 start_pos, u16 c);
u64
string_find_first(String_Const_u32 str, u64 start_pos, u32 c);
u64
string_find_first(String_Const_char str, char c);
u64
string_find_first(String_Const_u8 str, u8 c);
u64
string_find_first(String_Const_u16 str, u16 c);
u64
string_find_first(String_Const_u32 str, u32 c);
i64
string_find_last(String_Const_char str, char c);
i64
string_find_last(String_Const_u8 str, u8 c);
i64
string_find_last(String_Const_u16 str, u16 c);
i64
string_find_last(String_Const_u32 str, u32 c);
u64
string_find_first_whitespace(String_Const_char str);
u64
string_find_first_whitespace(String_Const_u8 str);
u64
string_find_first_whitespace(String_Const_u16 str);
u64
string_find_first_whitespace(String_Const_u32 str);
i64
string_find_last_whitespace(String_Const_char str);
i64
string_find_last_whitespace(String_Const_u8 str);
i64
string_find_last_whitespace(String_Const_u16 str);
i64
string_find_last_whitespace(String_Const_u32 str);
u64
string_find_first_non_whitespace(String_Const_char str);
u64
string_find_first_non_whitespace(String_Const_u8 str);
u64
string_find_first_non_whitespace(String_Const_u16 str);
u64
string_find_first_non_whitespace(String_Const_u32 str);
i64
string_find_last_non_whitespace(String_Const_char str);
i64
string_find_last_non_whitespace(String_Const_u8 str);
i64
string_find_last_non_whitespace(String_Const_u16 str);
i64
string_find_last_non_whitespace(String_Const_u32 str);
u64
string_find_first_slash(String_Const_char str);
u64
string_find_first_slash(String_Const_u8 str);
u64
string_find_first_slash(String_Const_u16 str);
u64
string_find_first_slash(String_Const_u32 str);
i64
string_find_last_slash(String_Const_char str);
i64
string_find_last_slash(String_Const_u8 str);
i64
string_find_last_slash(String_Const_u16 str);
i64
string_find_last_slash(String_Const_u32 str);
String_Const_char
string_remove_last_folder(String_Const_char str);
String_Const_u8
string_remove_last_folder(String_Const_u8 str);
String_Const_u16
string_remove_last_folder(String_Const_u16 str);
String_Const_u32
string_remove_last_folder(String_Const_u32 str);
b32
string_looks_like_drive_letter(String_Const_u8 string);
String_Const_char
string_remove_front_of_path(String_Const_char str);
String_Const_u8
string_remove_front_of_path(String_Const_u8 str);
String_Const_u16
string_remove_front_of_path(String_Const_u16 str);
String_Const_u32
string_remove_front_of_path(String_Const_u32 str);
String_Const_char
string_front_of_path(String_Const_char str);
String_Const_u8
string_front_of_path(String_Const_u8 str);
String_Const_u16
string_front_of_path(String_Const_u16 str);
String_Const_u32
string_front_of_path(String_Const_u32 str);
String_Const_u8
string_remove_front_folder_of_path(String_Const_u8 str);
String_Const_u8
string_front_folder_of_path(String_Const_u8 str);
String_Const_char
string_file_extension(String_Const_char string);
String_Const_u8
string_file_extension(String_Const_u8 string);
String_Const_u16
string_file_extension(String_Const_u16 string);
String_Const_u32
string_file_extension(String_Const_u32 string);
String_Const_char
string_file_without_extension(String_Const_char string);
String_Const_u8
string_file_without_extension(String_Const_u8 string);
String_Const_u16
string_file_without_extension(String_Const_u16 string);
String_Const_u32
string_file_without_extension(String_Const_u32 string);
String_Const_char
string_skip_whitespace(String_Const_char str);
String_Const_u8
string_skip_whitespace(String_Const_u8 str);
String_Const_u16
string_skip_whitespace(String_Const_u16 str);
String_Const_u32
string_skip_whitespace(String_Const_u32 str);
String_Const_char
string_chop_whitespace(String_Const_char str);
String_Const_u8
string_chop_whitespace(String_Const_u8 str);
String_Const_u16
string_chop_whitespace(String_Const_u16 str);
String_Const_u32
string_chop_whitespace(String_Const_u32 str);
String_Const_char
string_skip_chop_whitespace(String_Const_char str);
String_Const_u8
string_skip_chop_whitespace(String_Const_u8 str);
String_Const_u16
string_skip_chop_whitespace(String_Const_u16 str);
String_Const_u32
string_skip_chop_whitespace(String_Const_u32 str);
b32
string_match(String_Const_char a, String_Const_char b);
b32
string_match(String_Const_u8 a, String_Const_u8 b);
b32
string_match(String_Const_u16 a, String_Const_u16 b);
b32
string_match(String_Const_u32 a, String_Const_u32 b);
b32
string_match(String_Const_Any a, String_Const_Any b);
b32
string_match_insensitive(String_Const_char a, String_Const_char b);
b32
string_match_insensitive(String_Const_u8 a, String_Const_u8 b);
b32
string_match_insensitive(String_Const_u16 a, String_Const_u16 b);
b32
string_match_insensitive(String_Const_u32 a, String_Const_u32 b);
b32
string_match(String_Const_char a, String_Const_char b, String_Match_Rule rule);
b32
string_match(String_Const_u8 a, String_Const_u8 b, String_Match_Rule rule);
b32
string_match(String_Const_u16 a, String_Const_u16 b, String_Match_Rule rule);
b32
string_match(String_Const_u32 a, String_Const_u32 b, String_Match_Rule rule);
u64
string_find_first(String_Const_char str, String_Const_char needle, String_Match_Rule rule);
u64
string_find_first(String_Const_u8 str, String_Const_u8 needle, String_Match_Rule rule);
u64
string_find_first(String_Const_u16 str, String_Const_u16 needle, String_Match_Rule rule);
u64
string_find_first(String_Const_u32 str, String_Const_u32 needle, String_Match_Rule rule);
u64
string_find_first(String_Const_char str, String_Const_char needle);
u64
string_find_first(String_Const_u8 str, String_Const_u8 needle);
u64
string_find_first(String_Const_u16 str, String_Const_u16 needle);
u64
string_find_first(String_Const_u32 str, String_Const_u32 needle);
u64
string_find_first_insensitive(String_Const_char str, String_Const_char needle);
u64
string_find_first_insensitive(String_Const_u8 str, String_Const_u8 needle);
u64
string_find_first_insensitive(String_Const_u16 str, String_Const_u16 needle);
u64
string_find_first_insensitive(String_Const_u32 str, String_Const_u32 needle);
b32
string_has_substr(String_Const_u8 str, String_Const_u8 needle, String_Match_Rule rule);
b32
string_has_substr(String_Const_u8 str, String_Const_u8 needle);
i32
string_compare(String_Const_char a, String_Const_char b);
i32
string_compare(String_Const_u8 a, String_Const_u8 b);
i32
string_compare(String_Const_u16 a, String_Const_u16 b);
i32
string_compare(String_Const_u32 a, String_Const_u32 b);
i32
string_compare_insensitive(String_Const_char a, String_Const_char b);
i32
string_compare_insensitive(String_Const_u8 a, String_Const_u8 b);
i32
string_compare_insensitive(String_Const_u16 a, String_Const_u16 b);
i32
string_compare_insensitive(String_Const_u32 a, String_Const_u32 b);
String_Const_char
string_mod_upper(String_Const_char str);
String_Const_u8
string_mod_upper(String_Const_u8 str);
String_Const_u16
string_mod_upper(String_Const_u16 str);
String_Const_u32
string_mod_upper(String_Const_u32 str);
String_Const_char
string_mod_lower(String_Const_char str);
String_Const_u8
string_mod_lower(String_Const_u8 str);
String_Const_u16
string_mod_lower(String_Const_u16 str);
String_Const_u32
string_mod_lower(String_Const_u32 str);
String_Const_char
string_mod_replace_character(String_Const_char str, char o, char n);
String_Const_u8
string_mod_replace_character(String_Const_u8 str, u8 o, u8 n);
String_Const_u16
string_mod_replace_character(String_Const_u16 str, u16 o, u16 n);
String_Const_u32
string_mod_replace_character(String_Const_u32 str, u32 o, u32 n);
b32
string_append(String_char *dst, String_Const_char src);
b32
string_append(String_u8 *dst, String_Const_u8 src);
b32
string_append(String_u16 *dst, String_Const_u16 src);
b32
string_append(String_u32 *dst, String_Const_u32 src);
b32
string_append_character(String_char *dst, char c);
b32
string_append_character(String_u8 *dst, u8 c);
b32
string_append_character(String_u16 *dst, u16 c);
b32
string_append_character(String_u32 *dst, u32 c);
b32
string_null_terminate(String_char *str);
b32
string_null_terminate(String_u8 *str);
b32
string_null_terminate(String_u16 *str);
b32
string_null_terminate(String_u32 *str);
String_char
string_char_push(Arena *arena, u64 size);
String_u8
string_u8_push(Arena *arena, u64 size);
String_u16
string_u16_push(Arena *arena, u64 size);
String_u32
string_u32_push(Arena *arena, u64 size);
String_Any
string_any_push(Arena *arena, u64 size, String_Encoding encoding);
String_Const_char
string_const_char_push(Arena *arena, u64 size);
String_Const_u8
string_const_u8_push(Arena *arena, u64 size);
String_Const_u16
string_const_u16_push(Arena *arena, u64 size);
String_Const_u32
string_const_u32_push(Arena *arena, u64 size);
String_Const_Any
string_const_any_push(Arena *arena, u64 size, String_Encoding encoding);
String_Const_char
push_string_copy(Arena *arena, String_Const_char src);
String_Const_u8
push_string_copy(Arena *arena, String_Const_u8 src);
String_Const_u16
push_string_copy(Arena *arena, String_Const_u16 src);
String_Const_u32
push_string_copy(Arena *arena, String_Const_u32 src);
String_Const_Any
push_string_copy(Arena *arena, u64 size, String_Const_Any src);
String_Const_u8_Array
push_string_array_copy(Arena *arena, String_Const_u8_Array src);
void
string_list_push(List_String_Const_char *list, Node_String_Const_char *node);
void
string_list_push(List_String_Const_u8 *list, Node_String_Const_u8 *node);
void
string_list_push(List_String_Const_u16 *list, Node_String_Const_u16 *node);
void
string_list_push(List_String_Const_u32 *list, Node_String_Const_u32 *node);
void
string_list_push(Arena *arena, List_String_Const_char *list, String_Const_char string);
void
string_list_push(Arena *arena, List_String_Const_u8 *list, String_Const_u8 string);
void
string_list_push(Arena *arena, List_String_Const_u16 *list, String_Const_u16 string);
void
string_list_push(Arena *arena, List_String_Const_u32 *list, String_Const_u32 string);
void
string_list_push(Arena *arena, List_String_Const_Any *list, String_Const_Any string);
void
string_list_push(List_String_Const_char *list, List_String_Const_char *src_list);
void
string_list_push(List_String_Const_u8 *list, List_String_Const_u8 *src_list);
void
string_list_push(List_String_Const_u16 *list, List_String_Const_u16 *src_list);
void
string_list_push(List_String_Const_u32 *list, List_String_Const_u32 *src_list);
void
string_list_push(List_String_Const_Any *list, List_String_Const_Any *src_list);
void
string_list_push_overlap(Arena *arena, List_String_Const_char *list, char overlap, String_Const_char string);
void
string_list_push_overlap(Arena *arena, List_String_Const_u8 *list, u8 overlap, String_Const_u8 string);
void
string_list_push_overlap(Arena *arena, List_String_Const_u16 *list, u16 overlap, String_Const_u16 string);
void
string_list_push_overlap(Arena *arena, List_String_Const_u32 *list, u32 overlap, String_Const_u32 string);
typedef String_Const_char String_char_Mod_Function_Type(String_Const_char string);
typedef String_Const_u8 String_u8_Mod_Function_Type(String_Const_u8 string);
typedef String_Const_u16 String_u16_Mod_Function_Type(String_Const_u16 string);
typedef String_Const_u32 String_u32_Mod_Function_Type(String_Const_u32 string);

String_Const_char
string_list_flatten(Arena *arena, List_String_Const_char list, String_char_Mod_Function_Type *mod, String_Const_char separator, String_Separator_Flag separator_flags, String_Fill_Terminate_Rule rule);
String_Const_u8
string_list_flatten(Arena *arena, List_String_Const_u8 list, String_u8_Mod_Function_Type *mod, String_Const_u8 separator, String_Separator_Flag separator_flags, String_Fill_Terminate_Rule rule);
String_Const_u16
string_list_flatten(Arena *arena, List_String_Const_u16 list, String_u16_Mod_Function_Type *mod, String_Const_u16 separator, String_Separator_Flag separator_flags, String_Fill_Terminate_Rule rule);
String_Const_u32
string_list_flatten(Arena *arena, List_String_Const_u32 list, String_u32_Mod_Function_Type *mod, String_Const_u32 separator, String_Separator_Flag separator_flags, String_Fill_Terminate_Rule rule);
String_Const_char
string_list_flatten(Arena *arena, List_String_Const_char list, String_Const_char separator, String_Separator_Flag separator_flags, String_Fill_Terminate_Rule rule);
String_Const_u8
string_list_flatten(Arena *arena, List_String_Const_u8 list, String_Const_u8 separator, String_Separator_Flag separator_flags, String_Fill_Terminate_Rule rule);
String_Const_u16
string_list_flatten(Arena *arena, List_String_Const_u16 list, String_Const_u16 separator, String_Separator_Flag separator_flags, String_Fill_Terminate_Rule rule);
String_Const_u32
string_list_flatten(Arena *arena, List_String_Const_u32 list, String_Const_u32 separator, String_Separator_Flag separator_flags, String_Fill_Terminate_Rule rule);
String_Const_char
string_list_flatten(Arena *arena, List_String_Const_char list, String_char_Mod_Function_Type *mod, String_Fill_Terminate_Rule rule);
String_Const_u8
string_list_flatten(Arena *arena, List_String_Const_u8 list, String_u8_Mod_Function_Type *mod, String_Fill_Terminate_Rule rule);
String_Const_u16
string_list_flatten(Arena *arena, List_String_Const_u16 list, String_u16_Mod_Function_Type *mod, String_Fill_Terminate_Rule rule);
String_Const_u32
string_list_flatten(Arena *arena, List_String_Const_u32 list, String_u32_Mod_Function_Type *mod, String_Fill_Terminate_Rule rule);
String_Const_char
string_list_flatten(Arena *arena, List_String_Const_char string, String_Fill_Terminate_Rule rule);
String_Const_u8
string_list_flatten(Arena *arena, List_String_Const_u8 string, String_Fill_Terminate_Rule rule);
String_Const_u16
string_list_flatten(Arena *arena, List_String_Const_u16 string, String_Fill_Terminate_Rule rule);
String_Const_u32
string_list_flatten(Arena *arena, List_String_Const_u32 string, String_Fill_Terminate_Rule rule);
String_Const_char
string_list_flatten(Arena *arena, List_String_Const_char string);
String_Const_u8
string_list_flatten(Arena *arena, List_String_Const_u8 string);
String_Const_u16
string_list_flatten(Arena *arena, List_String_Const_u16 string);
String_Const_u32
string_list_flatten(Arena *arena, List_String_Const_u32 string);
List_String_Const_char
string_split(Arena *arena, String_Const_char string, char *split_characters, i32 split_character_count);
List_String_Const_u8
string_split(Arena *arena, String_Const_u8 string, u8 *split_characters, i32 split_character_count);
List_String_Const_u16
string_split(Arena *arena, String_Const_u16 string, u16 *split_characters, i32 split_character_count);
List_String_Const_u32
string_split(Arena *arena, String_Const_u32 string, u32 *split_characters, i32 split_character_count);
List_String_Const_char
string_split_needle(Arena *arena, String_Const_char string, String_Const_char needle);
List_String_Const_u8
string_split_needle(Arena *arena, String_Const_u8 string, String_Const_u8 needle);
List_String_Const_u16
string_split_needle(Arena *arena, String_Const_u16 string, String_Const_u16 needle);
List_String_Const_u32
string_split_needle(Arena *arena, String_Const_u32 string, String_Const_u32 needle);
void
string_list_insert_separators(Arena *arena, List_String_Const_char *list, String_Const_char separator, String_Separator_Flag flags);
void
string_list_insert_separators(Arena *arena, List_String_Const_u8 *list, String_Const_u8 separator, String_Separator_Flag flags);
void
string_list_insert_separators(Arena *arena, List_String_Const_u16 *list, String_Const_u16 separator, String_Separator_Flag flags);
void
string_list_insert_separators(Arena *arena, List_String_Const_u32 *list, String_Const_u32 separator, String_Separator_Flag flags);
void
string_list_rewrite_nodes(Arena *arena, List_String_Const_char *list, String_Const_char needle, String_Const_char new_value);
void
string_list_rewrite_nodes(Arena *arena, List_String_Const_u8 *list, String_Const_u8 needle, String_Const_u8 new_value);
void
string_list_rewrite_nodes(Arena *arena, List_String_Const_u16 *list, String_Const_u16 needle, String_Const_u16 new_value);
void
string_list_rewrite_nodes(Arena *arena, List_String_Const_u32 *list, String_Const_u32 needle, String_Const_u32 new_value);
String_Const_char
string_condense_whitespace(Arena *arena, String_Const_char string);
String_Const_u8
string_condense_whitespace(Arena *arena, String_Const_u8 string);
String_Const_u16
string_condense_whitespace(Arena *arena, String_Const_u16 string);
String_Const_u32
string_condense_whitespace(Arena *arena, String_Const_u32 string);
List_String_Const_u8
string_split_wildcards(Arena *arena, String_Const_u8 string);
b32
string_wildcard_match(List_String_Const_u8 list, String_Const_u8 string, String_Match_Rule rule);
b32
string_wildcard_match(List_String_Const_u8 list, String_Const_u8 string);
b32
string_wildcard_match_insensitive(List_String_Const_u8 list, String_Const_u8 string);
void
string_list_reverse(List_String_Const_char *list);
void
string_list_reverse(List_String_Const_u8 *list);
void
string_list_reverse(List_String_Const_u16 *list);
void
string_list_reverse(List_String_Const_u32 *list);
b32
string_list_match(List_String_Const_u8 a, List_String_Const_u8 b);
extern const u8 utf8_class[32];
Character_Consume_Result
utf8_consume(u8 *str, u64 max);
Character_Consume_Result
utf16_consume(u16 *str, u64 max);
u32
utf8_write(u8 *str, u32 codepoint);
u32
utf16_write(u16 *str, u32 codepoint);
String_u8
string_u8_from_string_char(Arena *arena, String_Const_char string, String_Fill_Terminate_Rule rule);
String_u16
string_u16_from_string_char(Arena *arena, String_Const_char string, String_Fill_Terminate_Rule rule);
String_u32
string_u32_from_string_char(Arena *arena, String_Const_char string, String_Fill_Terminate_Rule rule);
String_char
string_char_from_string_u8(Arena *arena, String_Const_u8 string, String_Fill_Terminate_Rule rule);
String_u16
string_u16_from_string_u8(Arena *arena, String_Const_u8 string, String_Fill_Terminate_Rule rule);
String_u32
string_u32_from_string_u8(Arena *arena, String_Const_u8 string, String_Fill_Terminate_Rule rule);
String_char
string_char_from_string_u16(Arena *arena, String_Const_u16 string, String_Fill_Terminate_Rule rule);
String_u8
string_u8_from_string_u16(Arena *arena, String_Const_u16 string, String_Fill_Terminate_Rule rule);
String_u32
string_u32_from_string_u16(Arena *arena, String_Const_u16 string, String_Fill_Terminate_Rule rule);
String_char
string_char_from_string_u32(Arena *arena, String_Const_u32 string, String_Fill_Terminate_Rule rule);
String_u8
string_u8_from_string_u32(Arena *arena, String_Const_u32 string, String_Fill_Terminate_Rule rule);
String_u16
string_u16_from_string_u32(Arena *arena, String_Const_u32 string, String_Fill_Terminate_Rule rule);
String_char
string_char_from_string_u8(Arena *arena, String_Const_u8 string);
String_char
string_char_from_string_u16(Arena *arena, String_Const_u16 string);
String_char
string_char_from_string_u32(Arena *arena, String_Const_u32 string);
String_u8
string_u8_from_string_char(Arena *arena, String_Const_char string);
String_u8
string_u8_from_string_u16(Arena *arena, String_Const_u16 string);
String_u8
string_u8_from_string_u32(Arena *arena, String_Const_u32 string);
String_u16
string_u16_from_string_char(Arena *arena, String_Const_char string);
String_u16
string_u16_from_string_u8(Arena *arena, String_Const_u8 string);
String_u16
string_u16_from_string_u32(Arena *arena, String_Const_u32 string);
String_u32
string_u32_from_string_char(Arena *arena, String_Const_char string);
String_u32
string_u32_from_string_u8(Arena *arena, String_Const_u8 string);
String_u32
string_u32_from_string_u16(Arena *arena, String_Const_u16 string);
String_Const_char
string_char_from_any(Arena *arena, String_Const_Any string);
String_Const_u8
string_u8_from_any(Arena *arena, String_Const_Any string);
String_Const_u16
string_u16_from_any(Arena *arena, String_Const_Any string);
String_Const_u32
string_u32_from_any(Arena *arena, String_Const_Any string);
String_Const_Any
string_any_from_any(Arena *arena, String_Encoding encoding, String_Const_Any string);
List_String_Const_char
string_list_char_from_any(Arena *arena, List_String_Const_Any list);
List_String_Const_u8
string_list_u8_from_any(Arena *arena, List_String_Const_Any list);
List_String_Const_u16
string_list_u16_from_any(Arena *arena, List_String_Const_Any list);
List_String_Const_u32
string_list_u32_from_any(Arena *arena, List_String_Const_Any list);
Line_Ending_Kind
string_guess_line_ending_kind(String_Const_u8 string);
List_String_Const_char
string_replace_list(Arena *arena, String_Const_char source, String_Const_char needle, String_Const_char replacement);
List_String_Const_u8
string_replace_list(Arena *arena, String_Const_u8 source, String_Const_u8 needle, String_Const_u8 replacement);
List_String_Const_u16
string_replace_list(Arena *arena, String_Const_u16 source, String_Const_u16 needle, String_Const_u16 replacement);
List_String_Const_u32
string_replace_list(Arena *arena, String_Const_u32 source, String_Const_u32 needle, String_Const_u32 replacement);
String_Const_char
string_replace(Arena *arena, String_Const_char source, String_Const_char needle, String_Const_char replacement, String_Fill_Terminate_Rule rule);
String_Const_u8
string_replace(Arena *arena, String_Const_u8 source, String_Const_u8 needle, String_Const_u8 replacement, String_Fill_Terminate_Rule rule);
String_Const_u16
string_replace(Arena *arena, String_Const_u16 source, String_Const_u16 needle, String_Const_u16 replacement, String_Fill_Terminate_Rule rule);
String_Const_u32
string_replace(Arena *arena, String_Const_u32 source, String_Const_u32 needle, String_Const_u32 replacement, String_Fill_Terminate_Rule rule);
String_Const_char
string_replace(Arena *arena, String_Const_char source, String_Const_char needle, String_Const_char replacement);
String_Const_u8
string_replace(Arena *arena, String_Const_u8 source, String_Const_u8 needle, String_Const_u8 replacement);
String_Const_u16
string_replace(Arena *arena, String_Const_u16 source, String_Const_u16 needle, String_Const_u16 replacement);
String_Const_u32
string_replace(Arena *arena, String_Const_u32 source, String_Const_u32 needle, String_Const_u32 replacement);
b32
byte_is_ascii(u8 byte);
b32
data_is_ascii(String_Const_u8 data);
String_Const_u8
string_escape(Arena *arena, String_Const_u8 string);
String_Const_char
string_interpret_escapes(Arena *arena, String_Const_char string);
String_Const_u8
string_interpret_escapes(Arena *arena, String_Const_u8 string);
extern const u8 integer_symbols[];
extern const u8 integer_symbol_reverse[128];
extern const u8 base64[64];
extern const u8 base64_reverse[128];
u64
digit_count_from_integer(u64 x, u32 radix);
String_Const_u8
string_from_integer(Arena *arena, u64 x, u32 radix);
b32
string_is_integer(String_Const_u8 string, u32 radix);
u64
string_to_integer(String_Const_u8 string, u32 radix);
u64
string_to_integer(String_Const_char string, u32 radix);
String_Const_u8
string_base64_encode_from_binary(Arena *arena, void *data, u64 size);
String_Const_u8
data_decode_from_base64(Arena *arena, u8 *str, u64 size);
u64
time_stamp_from_date_time(Date_Time *date_time);
Date_Time
date_time_from_time_stamp(u64 time_stamp);

#endif
