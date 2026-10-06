/*
 * 5ed event types
 */

// TOP

#if !defined(FCODER_EVENTS_H)
#define FCODER_EVENTS_H

typedef void Custom_Command_Function(struct Application_Links *app);

typedef u32 Key_Code;
typedef u32 Mouse_Code;
typedef u32 Core_Code;
#include "base/generated/5ed_event_codes.h"

typedef u32 Input_Event_Kind;
enum{
    InputEventKind_None,
    InputEventKind_TextInsert,
    InputEventKind_KeyStroke,
    InputEventKind_KeyRelease,
    InputEventKind_MouseButton,
    InputEventKind_MouseButtonRelease,
    InputEventKind_MouseWheel,
    InputEventKind_MouseMove,
    InputEventKind_Core,
    InputEventKind_CustomFunction,
    
    InputEventKind_COUNT,
};

typedef u32 Key_Flags;
enum{
    KeyFlag_IsDeadKey = (1 << 0),
};

global_const i32 Input_MaxModifierCount = 8;

struct Input_Modifier_Set{
    Key_Code *mods;
    i32 count;
};

struct Input_Modifier_Set_Fixed{
    Key_Code mods[Input_MaxModifierCount];
    i32 count;
};

struct Input_Event{
    Input_Event_Kind kind;
    b32 virtual_event;
    union{
        struct{
            String_Const_u8 string;
            
            // used internally
            Input_Event *next_text;
            b32 blocked;
        } text;
        struct{
            Key_Code code;
            Key_Flags flags;
            Input_Modifier_Set modifiers;
            
            // used internally
            Input_Event *first_dependent_text;
        } key;
        struct{
            Mouse_Code code;
            Vec2_i32 p;
            Input_Modifier_Set modifiers;
        } mouse;
        struct{
            f32 value;
            Vec2_i32 p;
            Input_Modifier_Set modifiers;
        } mouse_wheel;
        struct{
            Vec2_i32 p;
            Input_Modifier_Set modifiers;
        } mouse_move;
        struct{
            Core_Code code;
            union{
                String_Const_u8 string;
                i32 id;
                struct{
                    String_Const_u8_Array flag_strings;
                    String_Const_u8_Array file_names;
                };
            };
        } core;
        Custom_Command_Function *custom_func;
    };
};

struct Input_Event_Node{
    Input_Event_Node *next;
    Input_Event event;
};

struct Input_List{
    Input_Event_Node *first;
    Input_Event_Node *last;
    i32 count;
};

typedef u32 Event_Property;
enum{
    EventProperty_AnyKey         = 0x0001,
    EventProperty_Escape         = 0x0002,
    EventProperty_AnyKeyRelease  = 0x0004,
    EventProperty_MouseButton    = 0x0008,
    EventProperty_MouseRelease   = 0x0010,
    EventProperty_MouseWheel     = 0x0020,
    EventProperty_MouseMove      = 0x0040,
    EventProperty_Animate        = 0x0080,
    EventProperty_ViewActivation = 0x0100,
    EventProperty_TextInsert     = 0x0200,
    EventProperty_AnyFile        = 0x0400,
    EventProperty_Startup        = 0x0800,
    EventProperty_Exit           = 0x1000,
    EventProperty_Clipboard      = 0x2000,
    EventProperty_CustomFunction = 0x4000,
};
enum{
    EventPropertyGroup_AnyKeyboardEvent =
        EventProperty_AnyKey|
        EventProperty_Escape|
        EventProperty_AnyKeyRelease|
        EventProperty_TextInsert,
    EventPropertyGroup_AnyMouseEvent =
        EventProperty_MouseButton|
        EventProperty_MouseRelease|
        EventProperty_MouseWheel|
        EventProperty_MouseMove,
    EventPropertyGroup_AnyUserInput =
        EventPropertyGroup_AnyKeyboardEvent|
        EventPropertyGroup_AnyMouseEvent,
    EventPropertyGroup_AnyCore =
        EventProperty_Animate|
        EventProperty_ViewActivation|
        EventProperty_AnyFile|
        EventProperty_Startup|
        EventProperty_Exit|
        EventProperty_Clipboard|
        EventProperty_Animate,
    EventPropertyGroup_Any =
        EventPropertyGroup_AnyUserInput|
        EventPropertyGroup_AnyCore|
        EventProperty_CustomFunction,
};


b32
has_modifier(Key_Code *mods, i32 count, Key_Code modifier);
b32
has_modifier(Input_Modifier_Set_Fixed *set, Key_Code modifier);
b32
has_modifier(Input_Modifier_Set *set, Key_Code modifier);
Input_Modifier_Set
copy_modifier_set(Arena *arena, Input_Modifier_Set_Fixed *set);
void
copy_modifier_set(Input_Modifier_Set_Fixed *dst, Input_Modifier_Set *set);
void
add_modifier(Input_Modifier_Set_Fixed *set, Key_Code mod);
void
remove_modifier(Input_Modifier_Set_Fixed *set, Key_Code mod);
void
set_modifier(Input_Modifier_Set_Fixed *set, Key_Code mod, b32 val);
Input_Modifier_Set
copy_modifier_set(Arena *arena, Input_Modifier_Set *set);
Input_Modifier_Set*
get_modifiers(Input_Event *event);
b32
has_modifier(Input_Event *event, Key_Code modifier);
b32
is_unmodified_key(Input_Event *event);
b32
is_modified(Input_Event *event);
b32
event_is_dead_key(Input_Event *event);
Input_Event
event_next_text_event(Input_Event *event);
String_Const_u8
to_writable(Input_Event *event);
b32
match_key_code(Input_Event *event, Key_Code code);
b32
match_mouse_code(Input_Event *event, Mouse_Code code);
b32
match_mouse_code_release(Input_Event *event, Mouse_Code code);
b32
match_core_code(Input_Event *event, Core_Code code);
Event_Property
get_event_properties(Input_Event *event);
Input_Event*
push_input_event(Arena *arena, Input_List *list);
Input_Event*
push_input_event(Arena *arena, Input_List *list, Input_Event *event);
Input_Event
copy_input_event(Arena *arena, Input_Event *event);
String_Const_u8
stringize_keyboard_event(Arena *arena, Input_Event *event);
Input_Event
parse_keyboard_event(Arena *arena, String_Const_u8 text);
#endif

// BOTTOM
