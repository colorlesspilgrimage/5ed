/*
5ed_default_include.cpp - Default set of commands and setup used in 5ed.
*/

// TOP

#if !defined(FCODER_DEFAULT_INCLUDE_CPP)
#define FCODER_DEFAULT_INCLUDE_CPP


#include <stdio.h>
#include <stdlib.h>

#include "base/5ed_base_types.h"
#include "base/5ed_stringf.h"
#include "base/5ed_hash_functions.h"
#include "base/5ed_log_helpers.h"
#include "base/5ed_malloc_allocator.h"
#include "base/5ed_mem.h"
#include "base/5ed_version.h"
#include "base/5ed_table.h"
#include "base/5ed_events.h"
#include "base/5ed_types.h"
#include "base/5ed_codepoint_map.h"
#include "base/5ed_buffer_seek_constructors.h"
#include "base/5ed_layout_lookup.h"
#include "base/5ed_stdio_file.h"
#include "base/5ed_custom_api.h"
#include "custom/5ed_managed_ids.h"
#include "base/5ed_system_types.h"
#include "base/5ed_system_api.h"
#include "custom/5ed_command_table.h"

#include "base/5ed_token.h"
#include "base/generated/lexer_cpp.h"

#include "base/5ed_lexer_cpp_api.h"
#include "custom/5ed_variables.h"
#include "custom/5ed_audio.h"
#include "base/5ed_profile.h"
#include "custom/5ed_async_tasks.h"
#include "base/5ed_string_match.h"
#include "custom/5ed_helper.h"
#include "custom/5ed_delta_rule.h"
#include "custom/5ed_layout_rule.h"
#include "custom/5ed_code_index.h"
#include "custom/5ed_draw.h"
#include "custom/5ed_insertion.h"
#include "base/5ed_command_map.h"
#include "custom/5ed_lister_base.h"
#include "custom/5ed_clipboard.h"
#include "custom/5ed_default_framework.h"
#include "custom/5ed_config.h"
#include "custom/5ed_auto_indent.h"
#include "custom/5ed_search.h"
#include "custom/5ed_build_commands.h"
#include "custom/5ed_jumping.h"
#include "custom/5ed_jump_sticky.h"
#include "custom/5ed_jump_lister.h"
#include "custom/5ed_project_commands.h"
#include "custom/5ed_prj_v1.h"
#include "custom/5ed_function_list.h"
#include "custom/5ed_scope_commands.h"
#include "custom/5ed_combined_write_commands.h"
#include "custom/5ed_log_parser.h"
#include "custom/5ed_profile_inspect.h"
#include "custom/5ed_tutorial.h"
#include "custom/5ed_search_list.h"

////////////////////////////////



#include "base/5ed_profile_macros.h"
#include "custom/5ed_async_tasks.cpp"


#include "custom/5ed_default_map.cpp"

#include "custom/5ed_default_framework_variables.cpp"
#include "custom/5ed_default_colors.cpp"
#include "custom/5ed_helper.cpp"
#include "custom/5ed_delta_rule.cpp"
#include "custom/5ed_layout_rule.cpp"
#include "custom/5ed_code_index.cpp"
#include "custom/5ed_fancy.cpp"
#include "custom/5ed_draw.cpp"
#include "custom/5ed_font_helper.cpp"
#include "custom/5ed_config.cpp"
#include "custom/5ed_dynamic_bindings.cpp"
#include "custom/5ed_default_framework.cpp"
#include "custom/5ed_clipboard.cpp"
#include "custom/5ed_lister_base.cpp"
#include "custom/5ed_base_commands.cpp"
#include "custom/5ed_insertion.cpp"
#include "custom/5ed_eol.cpp"
#include "custom/5ed_lists.cpp"
#include "custom/5ed_auto_indent.cpp"
#include "custom/5ed_search.cpp"
#include "custom/5ed_jumping.cpp"
#include "custom/5ed_jump_sticky.cpp"
#include "custom/5ed_jump_lister.cpp"
#include "custom/5ed_code_index_listers.cpp"
#include "custom/5ed_log_parser.cpp"
#include "custom/5ed_keyboard_macro.cpp"
#include "custom/5ed_cli_command.cpp"
#include "custom/5ed_build_commands.cpp"
#include "custom/5ed_project_commands.cpp"
#include "custom/5ed_prj_v1.cpp"
#include "custom/5ed_function_list.cpp"
#include "custom/5ed_scope_commands.cpp"
#include "custom/5ed_combined_write_commands.cpp"
#include "custom/5ed_miblo_numbers.cpp"
#include "custom/5ed_profile_inspect.cpp"
#include "custom/5ed_tutorial.cpp"
#include "custom/5ed_variables.cpp"
#include "custom/5ed_audio.cpp"
#include "custom/5ed_search_list.cpp"

#include "custom/5ed_examples.cpp"

#include "custom/5ed_default_hooks.cpp"

#endif

// BOTTOM

