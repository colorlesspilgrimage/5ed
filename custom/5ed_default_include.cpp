/*
5ed_default_include.cpp - Default set of commands and setup used in 5ed.
*/

// TOP

#if !defined(FCODER_DEFAULT_INCLUDE_CPP)
#define FCODER_DEFAULT_INCLUDE_CPP

#if !defined(FCODER_TRANSITION_TO)
#define FCODER_TRANSITION_TO 0
#endif

#include <stdio.h>
#include <stdlib.h>

#include "5ed_base_types.h"
#include "5ed_version.h"
#include "5ed_table.h"
#include "5ed_events.h"
#include "5ed_types.h"
#include "5ed_doc_content_types.h"
#include "5ed_default_colors.h"
#define DYNAMIC_LINK_API
#include "generated/custom_api.h"
#include "5ed_system_types.h"
#define DYNAMIC_LINK_API
#include "generated/system_api.h"
#if !defined(META_PASS)
#include "generated/command_metadata.h"
#endif

#include "5ed_token.h"
#include "generated/lexer_cpp.h"

#include "5ed_variables.h"
#include "5ed_audio.h"
#include "5ed_profile.h"
#include "5ed_async_tasks.h"
#include "5ed_string_match.h"
#include "5ed_helper.h"
#include "5ed_delta_rule.h"
#include "5ed_layout_rule.h"
#include "5ed_code_index.h"
#include "5ed_draw.h"
#include "5ed_insertion.h"
#include "5ed_command_map.h"
#include "5ed_lister_base.h"
#include "5ed_clipboard.h"
#include "5ed_default_framework.h"
#include "5ed_config.h"
#include "5ed_auto_indent.h"
#include "5ed_search.h"
#include "5ed_build_commands.h"
#include "5ed_jumping.h"
#include "5ed_jump_sticky.h"
#include "5ed_jump_lister.h"
#include "5ed_project_commands.h"
#include "5ed_prj_v1.h"
#include "5ed_function_list.h"
#include "5ed_scope_commands.h"
#include "5ed_combined_write_commands.h"
#include "5ed_log_parser.h"
#include "5ed_profile_inspect.h"
#include "5ed_tutorial.h"
#include "5ed_search_list.h"

////////////////////////////////

#include "5ed_base_types.cpp"
#include "5ed_stringf.cpp"
#include "5ed_app_links_allocator.cpp"
#include "5ed_system_allocator.cpp"

#include "5ed_stdio_file.cpp"

#define DYNAMIC_LINK_API
#include "generated/custom_api.cpp"
#define DYNAMIC_LINK_API
#include "generated/system_api.cpp"
#include "5ed_system_helpers.cpp"
#include "5ed_layout_lookup.cpp"
#include "5ed_profile.cpp"
#include "5ed_profile_static_enable.cpp"
#include "5ed_events.cpp"
#include "5ed_custom.cpp"
#include "5ed_log_helpers.cpp"
#include "5ed_hash_functions.cpp"
#include "5ed_table.cpp"
#include "5ed_codepoint_map.cpp"
#include "5ed_async_tasks.cpp"
#include "5ed_string_match.cpp"
#include "5ed_buffer_seek_constructors.cpp"
#include "5ed_token.cpp"
#include "5ed_command_map.cpp"

#include "generated/lexer_cpp.cpp"

#include "5ed_default_map.cpp"

#include "5ed_default_framework_variables.cpp"
#include "5ed_default_colors.cpp"
#include "5ed_helper.cpp"
#include "5ed_delta_rule.cpp"
#include "5ed_layout_rule.cpp"
#include "5ed_code_index.cpp"
#include "5ed_fancy.cpp"
#include "5ed_draw.cpp"
#include "5ed_font_helper.cpp"
#include "5ed_config.cpp"
#include "5ed_dynamic_bindings.cpp"
#include "5ed_default_framework.cpp"
#include "5ed_clipboard.cpp"
#include "5ed_lister_base.cpp"
#include "5ed_base_commands.cpp"
#include "5ed_insertion.cpp"
#include "5ed_eol.cpp"
#include "5ed_lists.cpp"
#include "5ed_auto_indent.cpp"
#include "5ed_search.cpp"
#include "5ed_jumping.cpp"
#include "5ed_jump_sticky.cpp"
#include "5ed_jump_lister.cpp"
#include "5ed_code_index_listers.cpp"
#include "5ed_log_parser.cpp"
#include "5ed_keyboard_macro.cpp"
#include "5ed_cli_command.cpp"
#include "5ed_build_commands.cpp"
#include "5ed_project_commands.cpp"
#include "5ed_prj_v1.cpp"
#include "5ed_function_list.cpp"
#include "5ed_scope_commands.cpp"
#include "5ed_combined_write_commands.cpp"
#include "5ed_miblo_numbers.cpp"
#include "5ed_profile_inspect.cpp"
#include "5ed_tutorial.cpp"
#include "5ed_doc_content_types.cpp"
#include "5ed_doc_commands.cpp"
#include "5ed_docs.cpp"
#include "5ed_variables.cpp"
#include "5ed_audio.cpp"
#include "5ed_search_list.cpp"

#include "5ed_examples.cpp"

#include "5ed_default_hooks.cpp"

#endif

// BOTTOM

