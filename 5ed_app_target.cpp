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

#include "5ed_base_types.h"
#include "5ed_version.h"
#include "5ed_table.h"
#include "5ed_events.h"
#include "5ed_types.h"
#include "5ed_doc_content_types.h"
#include "5ed_default_colors.h"
#define STATIC_LINK_API
#include "generated/custom_api.h"

#include "5ed_string_match.h"
#include "5ed_token.h"

#include "5ed_system_types.h"
#define DYNAMIC_LINK_API
#include "generated/system_api.h"
#include "5ed_font_interface.h"
#define DYNAMIC_LINK_API
#include "generated/graphics_api.h"
#define DYNAMIC_LINK_API
#include "generated/font_api.h"

#include "5ed_profile.h"
#include "5ed_command_map.h"

#include "5ed_render_target.h"
#include "5ed.h"
#include "5ed_buffer_model.h"
#include "5ed_coroutine.h"

#include "5ed_dynamic_variables.h"

#include "5ed_buffer_model.h"
#include "5ed_translation.h"
#include "5ed_buffer.h"
#include "5ed_history.h"
#include "5ed_file.h"

#include "5ed_working_set.h"
#include "5ed_hot_directory.h"
#include "5ed_cli.h"
#include "5ed_layout.h"
#include "5ed_view.h"
#include "5ed_edit.h"
#include "5ed_text_layout.h"
#include "5ed_font_set.h"
#include "5ed_log.h"
#include "5ed_app_models.h"

#include "generated/lexer_cpp.h"
#include "5ed_api_definition.h"
#include "docs/5ed_doc_helper.h"

////////////////////////////////

#include "5ed_base_types.cpp"
#include "5ed_layout_lookup.cpp"
#include "5ed_string_match.cpp"
#include "5ed_stringf.cpp"
#include "5ed_events.cpp"
#include "5ed_system_helpers.cpp"
#include "5ed_app_links_allocator.cpp"
#include "5ed_system_allocator.cpp"
#include "5ed_profile.cpp"
#include "5ed_profile_static_enable.cpp"
#include "5ed_hash_functions.cpp"
#include "5ed_table.cpp"
#include "5ed_log_helpers.cpp"
#include "5ed_buffer_seek_constructors.cpp"
#include "5ed_command_map.cpp"
#include "5ed_codepoint_map.cpp"

#include "generated/custom_api.cpp"
#define DYNAMIC_LINK_API
#include "generated/system_api.cpp"
#define DYNAMIC_LINK_API
#include "generated/graphics_api.cpp"
#define DYNAMIC_LINK_API
#include "generated/font_api.cpp"

#include "5ed_token.cpp"
#include "generated/lexer_cpp.cpp"

#include "5ed_api_definition.cpp"
#include "generated/custom_api_constructor.cpp"
#include "5ed_api_parser.cpp"
#include "5ed_doc_content_types.cpp"
#include "docs/5ed_doc_helper.cpp"
#include "docs/5ed_doc_custom_api.cpp"

#include "5ed_log.cpp"
#include "5ed_coroutine.cpp"
#include "5ed_mem.cpp"
#include "5ed_dynamic_variables.cpp"
#include "5ed_font_set.cpp"
#include "5ed_translation.cpp"
#include "5ed_render_target.cpp"
#include "5ed_app_models.cpp"
#include "5ed_buffer.cpp"
#include "5ed_string_matching.cpp"
#include "5ed_history.cpp"
#include "5ed_file.cpp"
#include "5ed_working_set.cpp"
#include "5ed_hot_directory.cpp"
#include "5ed_cli.cpp"
#include "5ed_layout.cpp"
#include "5ed_view.cpp"
#include "5ed_edit.cpp"
#include "5ed_text_layout.cpp"
#include "5ed_api_implementation.cpp"
#include "5ed.cpp"

// BOTTOM

