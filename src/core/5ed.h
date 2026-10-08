/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 12.12.2014
 *
 * Application Layer for 5ed
 *
 */

// TOP

#if !defined(FRED_H)
#define FRED_H

#define MAX_VIEWS 16

struct Plat_Settings{
    b8 fullscreen_window;
    
    i32 window_w;
    i32 window_h;
    i32 window_x;
    i32 window_y;
    b8 set_window_pos;
    b8 set_window_size;
    b8 maximize_window;
    
    b8 use_hinting;
    
    // -U value. It replaces the config directory only.
    char *user_directory;
};

#define App_Read_Command_Line_Sig(name) \
void *name(Thread_Context *tctx,\
String_Const_u8 current_directory,\
Plat_Settings *plat_settings,\
char ***files,   \
i32 **file_count,\
i32 argc,        \
char **argv)

typedef App_Read_Command_Line_Sig(App_Read_Command_Line);

#define App_Init_Sig(name) \
void name(Thread_Context *tctx,     \
Render_Target *target,    \
void *base_ptr,           \
String_Const_u8 current_directory)

typedef App_Init_Sig(App_Init);

#include "core/5ed_cursor_codes.h"

struct Application_Step_Result{
    Application_Mouse_Cursor mouse_cursor_type;
    b32 lctrl_lalt_is_altgr;
    b32 perform_kill;
    b32 animating;
    b32 has_new_title;
    char *title_string;
};

struct Application_Step_Input{
    b32 first_step;
    f32 dt;
    Mouse_State mouse;
    Input_List events;
    String_Const_u8 clipboard;
    b32 trying_to_kill;
};

#define App_Step_Sig(name) Application_Step_Result \
name(Thread_Context *tctx,                 \
Render_Target *target,                \
void *base_ptr,                       \
Application_Step_Input *input)

typedef App_Step_Sig(App_Step);

typedef b32 Log_Function(String_Const_u8 str);
typedef Log_Function *App_Get_Logger(void);

struct App_Functions{
    App_Get_Logger *get_logger;
    App_Read_Command_Line *read_command_line;
    App_Init *init;
    App_Step *step;
};

#define App_Get_Functions_Sig(name) App_Functions name()
typedef App_Get_Functions_Sig(App_Get_Functions);

extern "C" App_Functions app_get_functions();

#endif

// BOTTOM

