#if !defined(FCODER_5ED_LAYOUT_LOOKUP_H)
#define FCODER_5ED_LAYOUT_LOOKUP_H

i64
layout_nearest_pos_to_xy(Layout_Item_List list, Vec2_f32 p);
Layout_Item*
layout_get_first_with_index(Layout_Item_List list, i64 index);
Rect_f32
layout_box_of_pos(Layout_Item_List list, i64 index);
Rect_f32
layout_padded_box_of_pos(Layout_Item_List list, i64 index);
i64
layout_get_pos_at_character(Layout_Item_List list, i64 character);
i64
layout_character_from_pos(Layout_Item_List list, i64 index);

#endif
