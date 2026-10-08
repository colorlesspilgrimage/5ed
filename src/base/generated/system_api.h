#define system_error_box_sig() void system_error_box(char* msg)
#define system_get_path_sig() String_Const_u8 system_get_path(Arena* arena, System_Path_Code path_code)
#define system_get_canonical_sig() String_Const_u8 system_get_canonical(Arena* arena, String_Const_u8 name)
#define system_get_file_list_sig() File_List system_get_file_list(Arena* arena, String_Const_u8 directory)
#define system_quick_file_attributes_sig() File_Attributes system_quick_file_attributes(Arena* scratch, String_Const_u8 file_name)
#define system_load_handle_sig() b32 system_load_handle(Arena* scratch, char* file_name, Plat_Handle* out)
#define system_load_attributes_sig() File_Attributes system_load_attributes(Plat_Handle handle)
#define system_load_file_sig() b32 system_load_file(Plat_Handle handle, char* buffer, u32 size)
#define system_load_close_sig() b32 system_load_close(Plat_Handle handle)
#define system_save_file_sig() File_Attributes system_save_file(Arena* scratch, char* file_name, String_Const_u8 data)
#define system_load_library_sig() b32 system_load_library(Arena* scratch, String_Const_u8 file_name, System_Library* out)
#define system_release_library_sig() b32 system_release_library(System_Library handle)
#define system_get_proc_sig() Void_Func* system_get_proc(System_Library handle, char* proc_name)
#define system_now_time_sig() u64 system_now_time(void)
#define system_now_date_time_universal_sig() Date_Time system_now_date_time_universal(void)
#define system_local_date_time_from_universal_sig() Date_Time system_local_date_time_from_universal(Date_Time* date_time)
#define system_universal_date_time_from_local_sig() Date_Time system_universal_date_time_from_local(Date_Time* date_time)
#define system_wake_up_timer_create_sig() Plat_Handle system_wake_up_timer_create(void)
#define system_wake_up_timer_release_sig() void system_wake_up_timer_release(Plat_Handle handle)
#define system_wake_up_timer_set_sig() void system_wake_up_timer_set(Plat_Handle handle, u32 time_milliseconds)
#define system_signal_step_sig() void system_signal_step(u32 code)
#define system_sleep_sig() void system_sleep(u64 microseconds)
#define system_get_clipboard_sig() String_Const_u8 system_get_clipboard(Arena* arena, i32 index)
#define system_post_clipboard_sig() void system_post_clipboard(String_Const_u8 str, i32 index)
#define system_set_clipboard_catch_all_sig() void system_set_clipboard_catch_all(b32 enabled)
#define system_get_clipboard_catch_all_sig() b32 system_get_clipboard_catch_all(void)
#define system_cli_call_sig() b32 system_cli_call(Arena* scratch, char* path, char* script, CLI_Handles* cli_out)
#define system_cli_begin_update_sig() void system_cli_begin_update(CLI_Handles* cli)
#define system_cli_update_step_sig() b32 system_cli_update_step(CLI_Handles* cli, char* dest, u32 max, u32* amount)
#define system_cli_end_update_sig() b32 system_cli_end_update(CLI_Handles* cli)
#define system_open_color_picker_sig() void system_open_color_picker(Color_Picker* picker)
#define system_get_screen_scale_factor_sig() f32 system_get_screen_scale_factor(void)
#define system_thread_launch_sig() System_Thread system_thread_launch(Thread_Function* proc, void* ptr)
#define system_thread_join_sig() void system_thread_join(System_Thread thread)
#define system_thread_free_sig() void system_thread_free(System_Thread thread)
#define system_thread_get_id_sig() i32 system_thread_get_id(void)
#define system_acquire_global_frame_mutex_sig() void system_acquire_global_frame_mutex(Thread_Context* tctx)
#define system_release_global_frame_mutex_sig() void system_release_global_frame_mutex(Thread_Context* tctx)
#define system_mutex_make_sig() System_Mutex system_mutex_make(void)
#define system_mutex_acquire_sig() void system_mutex_acquire(System_Mutex mutex)
#define system_mutex_release_sig() void system_mutex_release(System_Mutex mutex)
#define system_mutex_free_sig() void system_mutex_free(System_Mutex mutex)
#define system_condition_variable_make_sig() System_Condition_Variable system_condition_variable_make(void)
#define system_condition_variable_wait_sig() void system_condition_variable_wait(System_Condition_Variable cv, System_Mutex mutex)
#define system_condition_variable_signal_sig() void system_condition_variable_signal(System_Condition_Variable cv)
#define system_condition_variable_free_sig() void system_condition_variable_free(System_Condition_Variable cv)
#define system_memory_allocate_sig() void* system_memory_allocate(u64 size, String_Const_u8 location)
#define system_memory_set_protection_sig() b32 system_memory_set_protection(void* ptr, u64 size, u32 flags)
#define system_memory_free_sig() void system_memory_free(void* ptr, u64 size)
#define system_memory_annotation_sig() Memory_Annotation system_memory_annotation(Arena* arena)
#define system_show_mouse_cursor_sig() void system_show_mouse_cursor(i32 show)
#define system_set_fullscreen_sig() b32 system_set_fullscreen(b32 full_screen)
#define system_is_fullscreen_sig() b32 system_is_fullscreen(void)
#define system_get_keyboard_modifiers_sig() Input_Modifier_Set system_get_keyboard_modifiers(Arena* arena)
#define system_set_key_mode_sig() void system_set_key_mode(Key_Mode mode)
#define system_set_source_mixer_sig() void system_set_source_mixer(void* ctx, Audio_Mix_Sources_Function* mix_func)
#define system_set_destination_mixer_sig() void system_set_destination_mixer(Audio_Mix_Destination_Function* mix_func)
void system_error_box(char* msg);
String_Const_u8 system_get_path(Arena* arena, System_Path_Code path_code);
String_Const_u8 system_get_canonical(Arena* arena, String_Const_u8 name);
File_List system_get_file_list(Arena* arena, String_Const_u8 directory);
File_Attributes system_quick_file_attributes(Arena* scratch, String_Const_u8 file_name);
b32 system_load_handle(Arena* scratch, char* file_name, Plat_Handle* out);
File_Attributes system_load_attributes(Plat_Handle handle);
b32 system_load_file(Plat_Handle handle, char* buffer, u32 size);
b32 system_load_close(Plat_Handle handle);
File_Attributes system_save_file(Arena* scratch, char* file_name, String_Const_u8 data);
b32 system_load_library(Arena* scratch, String_Const_u8 file_name, System_Library* out);
b32 system_release_library(System_Library handle);
Void_Func* system_get_proc(System_Library handle, char* proc_name);
u64 system_now_time(void);
Date_Time system_now_date_time_universal(void);
Date_Time system_local_date_time_from_universal(Date_Time* date_time);
Date_Time system_universal_date_time_from_local(Date_Time* date_time);
Plat_Handle system_wake_up_timer_create(void);
void system_wake_up_timer_release(Plat_Handle handle);
void system_wake_up_timer_set(Plat_Handle handle, u32 time_milliseconds);
void system_signal_step(u32 code);
void system_sleep(u64 microseconds);
String_Const_u8 system_get_clipboard(Arena* arena, i32 index);
void system_post_clipboard(String_Const_u8 str, i32 index);
void system_set_clipboard_catch_all(b32 enabled);
b32 system_get_clipboard_catch_all(void);
b32 system_cli_call(Arena* scratch, char* path, char* script, CLI_Handles* cli_out);
void system_cli_begin_update(CLI_Handles* cli);
b32 system_cli_update_step(CLI_Handles* cli, char* dest, u32 max, u32* amount);
b32 system_cli_end_update(CLI_Handles* cli);
void system_open_color_picker(Color_Picker* picker);
f32 system_get_screen_scale_factor(void);
System_Thread system_thread_launch(Thread_Function* proc, void* ptr);
void system_thread_join(System_Thread thread);
void system_thread_free(System_Thread thread);
i32 system_thread_get_id(void);
void system_acquire_global_frame_mutex(Thread_Context* tctx);
void system_release_global_frame_mutex(Thread_Context* tctx);
System_Mutex system_mutex_make(void);
void system_mutex_acquire(System_Mutex mutex);
void system_mutex_release(System_Mutex mutex);
void system_mutex_free(System_Mutex mutex);
System_Condition_Variable system_condition_variable_make(void);
void system_condition_variable_wait(System_Condition_Variable cv, System_Mutex mutex);
void system_condition_variable_signal(System_Condition_Variable cv);
void system_condition_variable_free(System_Condition_Variable cv);
void* system_memory_allocate(u64 size, String_Const_u8 location);
b32 system_memory_set_protection(void* ptr, u64 size, u32 flags);
void system_memory_free(void* ptr, u64 size);
Memory_Annotation system_memory_annotation(Arena* arena);
void system_show_mouse_cursor(i32 show);
b32 system_set_fullscreen(b32 full_screen);
b32 system_is_fullscreen(void);
Input_Modifier_Set system_get_keyboard_modifiers(Arena* arena);
void system_set_key_mode(Key_Mode mode);
void system_set_source_mixer(void* ctx, Audio_Mix_Sources_Function* mix_func);
void system_set_destination_mixer(Audio_Mix_Destination_Function* mix_func);
