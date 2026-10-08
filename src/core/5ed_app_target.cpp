/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 13.11.2015
 *
 * Application layer build target
 *
 */

// TOP

#define REMOVE_OLD_STRING

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
#include "base/5ed_custom_api.h"

#include "base/5ed_string_match.h"
#include "base/5ed_token.h"

#include "base/5ed_system_types.h"
#include "base/5ed_system_api.h"
#include "core/5ed_font_interface.h"

#include "base/5ed_profile.h"
#include "base/5ed_command_map.h"

#include "core/5ed_render_target.h"
#include "core/5ed.h"
#include "core/5ed_buffer_model.h"
#include "core/5ed_coroutine.h"

#include "core/5ed_dynamic_variables.h"

#include "core/5ed_buffer_model.h"
#include "core/5ed_translation.h"
#include "core/5ed_buffer.h"
#include "core/5ed_history.h"
#include "core/5ed_file.h"

#include "core/5ed_working_set.h"
#include "core/5ed_hot_directory.h"
#include "core/5ed_cli.h"
#include "core/5ed_layout.h"
#include "core/5ed_view.h"
#include "core/5ed_edit.h"
#include "core/5ed_text_layout.h"
#include "core/5ed_font_set.h"
#include "core/5ed_log.h"
#include "core/5ed_app_models.h"

#include "base/generated/lexer_cpp.h"
#include "base/5ed_lexer_cpp_api.h"

////////////////////////////////

#include "core/5ed_app_links_allocator.cpp"
#include "core/5ed_profile.cpp"
#include "base/5ed_profile_macros.h"

#include "core/5ed_log.cpp"
#include "core/5ed_coroutine.cpp"
#include "core/5ed_dynamic_variables.cpp"
#include "core/5ed_font_set.cpp"
#include "core/5ed_translation.cpp"
#include "core/5ed_render_target.cpp"
#include "core/5ed_app_models.cpp"
#include "core/5ed_buffer.cpp"
#include "core/5ed_string_matching.cpp"
#include "core/5ed_history.cpp"
#include "core/5ed_file.cpp"
#include "core/5ed_working_set.cpp"
#include "core/5ed_hot_directory.cpp"
#include "core/5ed_cli.cpp"
#include "core/5ed_layout.cpp"
#include "core/5ed_view.cpp"
#include "core/5ed_edit.cpp"
#include "core/5ed_text_layout.cpp"
#include "core/5ed_api_implementation.cpp"
#include "core/5ed.cpp"

// BOTTOM

