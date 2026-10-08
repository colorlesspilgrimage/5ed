/*
5ed_default_framework_variables.cpp - Declares the global variables used by the framework for
the default 5ed behavior.
*/

// TOP


////////////////////////////////

global b32 allow_immediate_close_without_checking_for_changes = false;

global char *default_extensions[] = {
    "cpp",
    "hpp",
    "c",
    "h",
    "cc",
    "cs",
    "java",
    "rs",
    "glsl",
    "m",
};

#if !defined(AUTO_CENTER_AFTER_JUMPS)
#define AUTO_CENTER_AFTER_JUMPS true
#endif
global b32 auto_center_after_jumps = AUTO_CENTER_AFTER_JUMPS;
global u8 locked_buffer_space[256];
global String_Const_u8 locked_buffer = {};


global View_ID build_footer_panel_view_id = 0;

global u8 out_buffer_space[1024];
global u8 command_space[1024];
global char hot_directory_space[1024];

global b32 suppressing_mouse = false;

global b32 show_fps_hud = false;

// TODO(allen): REMOVE THIS!
global Heap global_heap;

enum{
    FCoderMode_Original = 0,
    FCoderMode_NotepadLike = 1,
};
global i32 fcoder_mode = FCoderMode_Original;

global ID_Pos_Jump_Location prev_location = {};

global Arena global_permanent_arena = {};

global Arena global_config_arena = {};

global char previous_isearch_query[256] = {};

global Mapping framework_mapping = {};

global Buffer_Modified_Set global_buffer_modified_set = {};

global b32 def_enable_virtual_whitespace = false;

////////////////////////////////

global b32 global_keyboard_macro_is_recording = false;
global Range_i64 global_keyboard_macro_range = {};

////////////////////////////////

global Fade_Range_List buffer_fade_ranges = {};
global Arena fade_range_arena = {};
global Fade_Range *free_fade_ranges = 0;

////////////////////////////////

global Point_Stack point_stack = {};

////////////////////////////////

global Clipboard clipboard0 = {};

// BOTTOM

