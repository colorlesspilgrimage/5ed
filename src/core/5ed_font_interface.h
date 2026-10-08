/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 11.03.2017
 *
 * Font system interface.
 *
 */

// TOP

#if !defined(FCODER_FONT_INTERFACE_H)
#define FCODER_FONT_INTERFACE_H

typedef i32 Texture_Kind;
enum{
    TextureKind_Error,
    TextureKind_Mono,
};


////////////////////////////////

struct Glyph_Bounds{
    Rect_f32 uv;
    f32 w;
    Rect_f32 xy_off;
};

struct Face{
    Face_Description description;
    Face_ID id;
    i32 version_number;
    
    // NOTE(allen): Metrics
    Face_Metrics metrics;
    
    // NOTE(allen): Glyph data
    Face_Advance_Map advance_map;
    Glyph_Bounds *bounds;
    Glyph_Bounds white;
    
    Texture_Kind texture_kind;
    u32 texture;
    Vec3_f32 texture_dim;
};

////////////////////////////////

// The platform layer defines these three.
Face* font_make_face(Arena* arena, Face_Description* description, f32 scale_factor);
u32 graphics_get_texture(Vec3_i32 dim, Texture_Kind texture_kind);
b32 graphics_fill_texture(Texture_Kind texture_kind, u32 texture, Vec3_i32 p, Vec3_i32 dim, void* data);

#endif

// BOTTOM


