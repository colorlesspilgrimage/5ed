#if !defined(FCODER_5ED_CODEPOINT_MAP_H)
#define FCODER_5ED_CODEPOINT_MAP_H

b32
codepoint_index_map_read(Codepoint_Index_Map *map, u32 codepoint, u16 *index_out);
u16
codepoint_index_map_count(Codepoint_Index_Map *map);
f32
font_get_glyph_advance(Face_Advance_Map *map, Face_Metrics *metrics, u32 codepoint, f32 tab_multiplier);
f32
font_get_max_glyph_advance_range(Face_Advance_Map *map, Face_Metrics *metrics,
                                 u32 codepoint_first, u32 codepoint_last,
                                 f32 tab_multiplier);
f32
font_get_average_glyph_advance_range(Face_Advance_Map *map, Face_Metrics *metrics,
                                     u32 codepoint_first, u32 codepoint_last,
                                     f32 tab_multiplier);

#endif
