/*
5ed_command_map.h - Command management types
*/

// TOP

#if !defined(FCODER_CODEPOINT_MAP_H)
#define FCODER_CODEPOINT_MAP_H

#include <stdarg.h>
typedef i64 Command_Map_ID;

struct Command_Trigger{
    Command_Trigger *next;
    Input_Event_Kind kind;
    u32 sub_code;
    Input_Modifier_Set mods;
};

struct Command_Trigger_List{
    Command_Trigger *first;
    Command_Trigger *last;
};

struct Command_Binding{
    union{
        Custom_Command_Function *custom;
        char *name;
    };
    
    Command_Binding();
    Command_Binding(Custom_Command_Function *c);
    Command_Binding(char *n);
    
    operator Custom_Command_Function*();
    operator char*();
};

struct Command_Modified_Binding{
    Command_Modified_Binding *next;
    SNode order_node;
    Input_Modifier_Set mods;
    Command_Binding binding;
};

struct Command_Binding_List{
    Command_Binding_List *next;
    SNode *first;
    SNode *last;
    i32 count;
};

struct Command_Map{
    Command_Map *next;
    Command_Map *prev;
    Command_Map_ID id;
    Command_Map_ID parent;
    Command_Binding text_input_command;
    Arena node_arena;
    Table_u64_u64 event_code_to_binding_list;
    Table_u64_u64 cmd_to_binding_trigger;
    Command_Modified_Binding *binding_first;
    Command_Modified_Binding *binding_last;
    Command_Binding_List *list_first;
    Command_Binding_List *list_last;
    
    struct Binding_Unit *real_beginning;
};

struct Mapping{
    Arena node_arena;
    Heap heap;
    Base_Allocator heap_wrapper;
    Table_u64_u64 id_to_map;
    Command_Map_ID id_counter;
    Command_Map *first_map;
    Command_Map *last_map;
    Command_Map *free_maps;
    Command_Modified_Binding *free_bindings;
    Command_Binding_List *free_lists;
};

typedef i32 Binding_Match_Rule;
enum{
    BindingMatchRule_Strict,
    BindingMatchRule_Loose,
};

struct Map_Event_Breakdown{
    Input_Modifier_Set *mod_set;
    u64 key;
    Key_Code skip_self_mod;
};


#if !defined(MAP_METADATA_ONLY)
#define MAP_METADATA_ONLY 0
#endif
#if MAP_METADATA_ONLY
#define BindingGetPtr(b) ((b).name)
#else
#define BindingGetPtr(b) ((b).custom)
#endif
#if MAP_METADATA_ONLY
# define BindFWrap_(F) stringify(F)
#else
# define BindFWrap_(F) F
#endif
#define MappingScope() Mapping *m = 0; Command_Map *map = 0
#define SelectMapping(N) m = (N)
#define SelectMap(ID) map = mapping_get_or_make_map(m, (ID))
#define ParentMap(ID) map_set_parent(m, map, (ID))
#define BindTextInput(F) map_set_binding_text_input(map, BindFWrap_(F))
#define Bind(F, K, ...) \
map_set_binding_l(m, map, BindFWrap_(F), InputEventKind_KeyStroke, (K), ##__VA_ARGS__, 0)
#define BindRelease(F, K, ...) \
map_set_binding_l(m, map, BindFWrap_(F), InputEventKind_KeyRelease, (K), ##__VA_ARGS__, 0)
#define BindMouse(F, K, ...) \
map_set_binding_l(m, map, BindFWrap_(F), InputEventKind_MouseButton, (K), ##__VA_ARGS__, 0)
#define BindMouseRelease(F, K, ...) \
map_set_binding_l(m, map, BindFWrap_(F), InputEventKind_MouseButtonRelease, (K), ##__VA_ARGS__, 0)
#define BindMouseWheel(F, ...) \
map_set_binding_l(m, map, BindFWrap_(F), InputEventKind_MouseWheel, 0, ##__VA_ARGS__, 0)
#define BindMouseMove(F, ...) \
map_set_binding_l(m, map, BindFWrap_(F), InputEventKind_MouseMove, 0, ##__VA_ARGS__, 0)
#define BindCore(F, K, ...) \
map_set_binding_l(m, map, BindFWrap_(F), InputEventKind_Core, (K), ##__VA_ARGS__, 0)


u64
mapping__key(Input_Event_Kind kind, u32 sub_code);
Command_Map*
mapping__alloc_map(Mapping *mapping);
void
mapping__free_map(Mapping *mapping, Command_Map *map);
Command_Modified_Binding*
mapping__alloc_modified_binding(Mapping *mapping);
void
mapping__free_modified_binding(Mapping *mapping, Command_Modified_Binding *binding);
Command_Binding_List*
mapping__alloc_binding_list(Mapping *mapping);
void
mapping__free_binding_list(Mapping *mapping, Command_Binding_List *binding_list);
Command_Binding_List*
map__get_list(Command_Map *map, u64 key);
Command_Binding_List*
map__get_or_make_list(Mapping *mapping, Command_Map *map, u64 key);
void
mapping_init(Thread_Context *tctx, Mapping *mapping);
void
mapping_release(Thread_Context *tctx, Mapping *mapping);
void
map__init(Mapping *mapping, Command_Map *map, Command_Map_ID id);
Command_Map*
mapping_get_map(Mapping *mapping, Command_Map_ID id);
Command_Map_ID
mapping_validate_id(Mapping *mapping, Command_Map_ID id);
Command_Map*
mapping_get_or_make_map(Mapping *mapping, Command_Map_ID id);
void
mapping_release_map(Mapping *mapping, Command_Map *map);
b32
map_strict_match(Input_Modifier_Set *binding_mod_set, Input_Modifier_Set *event_mod_set, Key_Code skip_self_mod);
b32
map_loose_match(Input_Modifier_Set *binding_mod_set, Input_Modifier_Set *event_mod_set);
Map_Event_Breakdown
map_get_event_breakdown(Input_Event *event);
Command_Binding
map_get_binding_non_recursive(Command_Map *map, Input_Event *event, Binding_Match_Rule rule);
Command_Binding
map_get_binding_non_recursive(Command_Map *map, Input_Event *event);
Command_Binding
map_get_binding_recursive(Mapping *mapping, Command_Map *map, Input_Event *event, Binding_Match_Rule rule);
Command_Binding
map_get_binding_recursive(Mapping *mapping, Command_Map *map, Input_Event *event);
void
map_set_parent(Command_Map *map, Command_Map *parent);
void
map_null_parent(Command_Map *map);
void
map__command_add_trigger(Command_Map *map, Command_Binding binding, Command_Trigger *trigger);
Input_Event
map_trigger_as_event(Command_Trigger *trigger);
Command_Trigger_List
map_get_triggers_non_recursive(Mapping *mapping, Command_Map *map, Command_Binding binding);
Command_Trigger_List
map_get_triggers_non_recursive(Command_Map *map, Command_Binding binding);
Command_Trigger_List
map_get_triggers_recursive(Arena *arena, Mapping *mapping, Command_Map *map, Command_Binding binding);
Command_Binding_List*
map_get_binding_list_on_key(Command_Map *map, Key_Code code);
Command_Binding_List*
map_get_binding_list_on_mouse_button(Command_Map *map, Mouse_Code code);
Command_Binding_List*
map_get_binding_list_on_core(Command_Map *map, Core_Code code);
void
map_set_binding(Mapping *mapping, Command_Map *map, Command_Binding binding, u32 code1, u32 code2, Input_Modifier_Set *mods);
void
map_set_binding_key(Mapping *mapping, Command_Map *map, Command_Binding binding, Key_Code code, Input_Modifier_Set *modifiers);
void
map_set_binding_mouse(Mapping *mapping, Command_Map *map, Command_Binding binding, Mouse_Code code, Input_Modifier_Set *modifiers);
void
map_set_binding_core(Mapping *mapping, Command_Map *map, Command_Binding binding, Core_Code code, Input_Modifier_Set *modifiers);
void
map_set_binding_text_input(Command_Map *map, Command_Binding binding);
Command_Binding_List*
map_get_binding_list_on_key(Mapping *mapping, Command_Map_ID map_id, Key_Code code);
Command_Binding
map_get_binding_non_recursive(Mapping *mapping, Command_Map_ID map_id, Input_Event *event);
Command_Binding
map_get_binding_recursive(Mapping *mapping, Command_Map_ID map_id, Input_Event *event);
Command_Trigger_List
map_get_triggers_non_recursive(Mapping *mapping, Command_Map_ID map_id, Command_Binding binding);
Command_Trigger_List
map_get_triggers_recursive(Arena *arena, Mapping *mapping, Command_Map_ID map_id, Command_Binding binding);
void
map_set_parent(Mapping *mapping, Command_Map_ID map_id, Command_Map_ID parent_id);
void
map_set_parent(Mapping *mapping, Command_Map *map, Command_Map_ID parent_id);
void
map_null_parent(Mapping *mapping, Command_Map_ID map_id);
void
map_set_binding(Mapping *mapping, Command_Map_ID map_id, Command_Binding binding,
                u32 code1, u32 code2, Input_Modifier_Set *modifiers);
void
map_set_binding_key(Mapping *mapping, Command_Map_ID map_id, Command_Binding binding,
                    Key_Code code, Input_Modifier_Set *modifiers);
void
map_set_binding_mouse(Mapping *mapping, Command_Map_ID map_id, Command_Binding binding,
                      Mouse_Code code, Input_Modifier_Set *modifiers);
void
map_set_binding_core(Mapping *mapping, Command_Map_ID map_id, Command_Binding binding,
                     Core_Code code, Input_Modifier_Set *modifiers);
void
map_set_binding_text_input(Mapping *mapping, Command_Map_ID map_id, Command_Binding binding);
void
command_trigger_stringize_mods(Arena *arena, List_String_Const_u8 *list, Input_Modifier_Set *modifiers);
void
command_trigger_stringize(Arena *arena, List_String_Const_u8 *list, Command_Trigger *trigger);
void
map_set_binding_lv(Mapping *mapping, Command_Map *map,
                   Command_Binding binding, u32 code1, u32 code2, va_list args);
#if MAP_METADATA_ONLY
void
map_set_binding_l(Mapping *mapping, Command_Map *map, char *name, u32 code1, u32 code2, ...);
#else
void
map_set_binding_l(Mapping *mapping, Command_Map *map, Custom_Command_Function *custom, u32 code1, u32 code2, ...);
#endif

#endif

// BOTTOM
