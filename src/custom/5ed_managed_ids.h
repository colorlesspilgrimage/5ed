#if !defined(FCODER_MANAGED_IDS_H)
#define FCODER_MANAGED_IDS_H

// The list of managed IDs. The same list declares the globals and fills them.
#define MANAGED_ID_LIST(X) \
X(colors, defcolor_bar) \
X(colors, defcolor_base) \
X(colors, defcolor_pop1) \
X(colors, defcolor_pop2) \
X(colors, defcolor_back) \
X(colors, defcolor_margin) \
X(colors, defcolor_margin_hover) \
X(colors, defcolor_margin_active) \
X(colors, defcolor_list_item) \
X(colors, defcolor_list_item_hover) \
X(colors, defcolor_list_item_active) \
X(colors, defcolor_cursor) \
X(colors, defcolor_at_cursor) \
X(colors, defcolor_highlight_cursor_line) \
X(colors, defcolor_highlight) \
X(colors, defcolor_at_highlight) \
X(colors, defcolor_mark) \
X(colors, defcolor_text_default) \
X(colors, defcolor_comment) \
X(colors, defcolor_comment_pop) \
X(colors, defcolor_keyword) \
X(colors, defcolor_str_constant) \
X(colors, defcolor_char_constant) \
X(colors, defcolor_int_constant) \
X(colors, defcolor_float_constant) \
X(colors, defcolor_bool_constant) \
X(colors, defcolor_preproc) \
X(colors, defcolor_include) \
X(colors, defcolor_special_character) \
X(colors, defcolor_ghost_character) \
X(colors, defcolor_highlight_junk) \
X(colors, defcolor_highlight_white) \
X(colors, defcolor_paste) \
X(colors, defcolor_undo) \
X(colors, defcolor_back_cycle) \
X(colors, defcolor_text_cycle) \
X(colors, defcolor_line_numbers_back) \
X(colors, defcolor_line_numbers_text) \
X(attachment, view_rewrite_loc) \
X(attachment, view_next_rewrite_loc) \
X(attachment, view_paste_index_loc) \
X(attachment, view_is_passive_loc) \
X(attachment, view_snap_mark_to_cursor) \
X(attachment, view_ui_data) \
X(attachment, view_highlight_range) \
X(attachment, view_highlight_buffer) \
X(attachment, view_render_hook) \
X(attachment, view_word_complete_menu) \
X(attachment, view_lister_loc) \
X(attachment, view_previous_buffer) \
X(attachment, buffer_map_id) \
X(attachment, buffer_eol_setting) \
X(attachment, buffer_lex_task) \
X(attachment, buffer_wrap_lines) \
X(attachment, sticky_jump_marker_handle) \
X(attachment, attachment_tokens)

#define MANAGED_ID_DECLARE(group, name) global Managed_ID name;
MANAGED_ID_LIST(MANAGED_ID_DECLARE)
#undef MANAGED_ID_DECLARE

function void
initialize_managed_ids(Application_Links *app){
#define MANAGED_ID_INIT(group, name) name = managed_id_declare(app, string_u8_litexpr(#group), string_u8_litexpr(#name));
    MANAGED_ID_LIST(MANAGED_ID_INIT)
#undef MANAGED_ID_INIT
}

#endif
