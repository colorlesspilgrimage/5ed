cmake_minimum_required(VERSION 3.20)
project(5ed LANGUAGES CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
    message(FATAL_ERROR "5ed supports x86-64 Linux only")
endif()
# 5ed_base_types.h accepts GCC on x86-64 Linux only.
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "5ed requires GCC (found ${CMAKE_CXX_COMPILER_ID})")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug (default) or Release" FORCE)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")

# The code base is C++11 and does not build as C++20 (its lerp() clashes with std::lerp).
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Everything is placed beside the executable: it finds 5ed_app.so, custom_5ed.so,
# fonts/, themes/ and config files relative to its own location.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Threads REQUIRED)
find_package(X11 REQUIRED COMPONENTS Xfixes)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

# Settings shared by every target in the project.
# One include root: paths are base/, core/, custom/ and platform/.
add_library(5ed_common INTERFACE)
target_include_directories(5ed_common INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(5ed_common INTERFACE
    _GNU_SOURCE
    $<$<CONFIG:Debug>:FRED_INTERNAL>)
target_compile_options(5ed_common INTERFACE
    -Wno-write-strings -Wno-comment -Wno-switch -Wno-null-dereference -Wno-unused-result
    -Wno-missing-declarations)
target_link_libraries(5ed_common INTERFACE Threads::Threads)

# Compiled once. The per-target files are listed below. They call the system or
# custom API through a different link mode in each binary. They stay compiled
# per binary until ROADMAP point 3.
# per-target: src/base/per_target/5ed_system_allocator.cpp
# per-target: src/base/per_target/5ed_app_links_allocator.cpp
# per-target: src/base/per_target/5ed_system_helpers.cpp
# per-target: src/base/per_target/5ed_profile.cpp
# per-target: src/base/per_target/5ed_profile_static_enable.cpp
# per-target: src/base/per_target/5ed_search_list.cpp
# per-target: src/base/generated/custom_api.cpp
# per-target: src/base/generated/system_api.cpp
# per-target: src/base/generated/font_api.cpp
# per-target: src/base/generated/graphics_api.cpp
# per-target: src/base/generated/lexer_cpp.cpp
# per-target: src/core/5ed_font_set.cpp
# per-target: src/base/per_target/5ed_command_map.cpp
add_library(5ed_base STATIC
    src/base/5ed_base_types.cpp
    src/base/5ed_stringf.cpp
    src/base/5ed_hash_functions.cpp
    src/base/5ed_table.cpp
    src/base/5ed_codepoint_map.cpp
    src/base/5ed_events.cpp
    src/base/5ed_string_match.cpp
    src/base/5ed_token.cpp
    src/base/5ed_buffer_seek_constructors.cpp
    src/base/5ed_layout_lookup.cpp
    src/base/5ed_log_helpers.cpp
    src/base/5ed_doc_content_types.cpp
    src/base/5ed_mem.cpp
    src/base/5ed_malloc_allocator.cpp
    src/base/5ed_stdio_file.cpp)
target_link_libraries(5ed_base PUBLIC 5ed_common)
set_target_properties(5ed_base PROPERTIES CXX_VISIBILITY_PRESET hidden)


################################################################################
# Custom layer: custom_5ed.so
#
# The command table (command_metadata.h) and managed ids are generated from the
# custom layer's own source: preprocess it with -DMETA_PASS, feed the result to
# metadata_generator, then compile the layer again with the generated files.
# Output goes to the build tree; paths recorded in the table are relative to
# the source root.

add_executable(metadata_generator src/custom/5ed_metadata_generator.cpp)
target_link_libraries(metadata_generator PRIVATE 5ed_common 5ed_base)

file(GLOB_RECURSE CUSTOM_LAYER_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/custom/*.cpp ${PROJECT_SOURCE_DIR}/src/custom/*.h)
list(FILTER CUSTOM_LAYER_FILES EXCLUDE REGEX "/src/custom/(lexer_generator|languages)/")

set(GENERATED_ROOT ${CMAKE_BINARY_DIR}/gen)
set(GENERATED_METADATA
    ${GENERATED_ROOT}/generated/command_metadata.h
    ${GENERATED_ROOT}/generated/managed_id_metadata.cpp)
add_custom_command(
    OUTPUT ${GENERATED_METADATA}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_ROOT}/generated
    COMMAND ${CMAKE_CXX_COMPILER} -std=c++11 -E -DMETA_PASS -Isrc
            src/custom/5ed_default_bindings.cpp -o ${GENERATED_ROOT}/command_metadata.i
    COMMAND metadata_generator -R ${GENERATED_ROOT} ${GENERATED_ROOT}/command_metadata.i
    DEPENDS metadata_generator ${CUSTOM_LAYER_FILES}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating command metadata"
    VERBATIM)

# managed_id_metadata.cpp is #included by 5ed_default_bindings.cpp, not compiled on its own.
set_source_files_properties(${GENERATED_METADATA} PROPERTIES HEADER_FILE_ONLY TRUE)
add_library(custom_5ed SHARED src/custom/5ed_default_bindings.cpp ${GENERATED_METADATA})
target_include_directories(custom_5ed PRIVATE ${GENERATED_ROOT})
target_link_libraries(custom_5ed PRIVATE 5ed_common 5ed_base)
set_target_properties(custom_5ed PROPERTIES PREFIX "" SUFFIX ".so")

################################################################################
# Core: 5ed_app.so

add_library(5ed_app SHARED src/core/5ed_app_target.cpp)
target_link_libraries(5ed_app PRIVATE 5ed_common 5ed_base)
set_target_properties(5ed_app PROPERTIES PREFIX "" SUFFIX ".so")

################################################################################
# Platform layer: 5ed

add_executable(5ed src/platform/linux/linux_5ed.cpp)
target_compile_options(5ed PRIVATE -fno-threadsafe-statics)
target_link_libraries(5ed PRIVATE 5ed_common 5ed_base
    X11::X11 X11::Xfixes OpenGL::GL Freetype::Freetype ${CMAKE_DL_LIBS} m rt)

################################################################################
# Runtime files (fonts, themes, default config and bindings) beside the binary.

add_custom_target(5ed_runtime_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            ${PROJECT_SOURCE_DIR}/ship_files ${CMAKE_BINARY_DIR}
    COMMENT "Copying ship_files"
    VERBATIM)
add_dependencies(5ed 5ed_runtime_files custom_5ed 5ed_app)

################################################################################
# Developer tools. Not part of the default build. The `regen-*` targets run a
# generator that rewrites checked-in files under src/base/generated/
# in the source tree; `regen` runs all except regen-lexer, whose output differs
# from the checked-in lexer_cpp.cpp (not investigated): run it only when the
# lexer definition changes. Do not build `regen-lexer` in parallel with a target
# that compiles the lexer. Regenerated keycodes differ from the checked-in file
# only in indentation.

function(fiveed_tool name)
    add_executable(${name} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${name} PRIVATE 5ed_common 5ed_base)
endfunction()

function(fiveed_regen name tool)
    add_custom_target(${name}
        COMMAND ${tool} ${ARGN}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS ${tool}
        VERBATIM)
endfunction()

fiveed_tool(tool_lexer_gen src/custom/languages/5ed_cpp_lexer_gen.cpp)
fiveed_tool(tool_keycodes src/core/tools/5ed_generate_keycodes.cpp)
fiveed_tool(tool_system_api src/core/tools/5ed_system_api.cpp)
fiveed_tool(tool_font_api src/core/tools/5ed_font_api.cpp)
fiveed_tool(tool_graphics_api src/core/tools/5ed_graphics_api.cpp)
fiveed_tool(tool_api_parser src/core/tools/5ed_api_parser_main.cpp)
fiveed_tool(tool_api_docs src/core/docs/5ed_doc_custom_api_main.cpp)

fiveed_regen(regen-lexer tool_lexer_gen)
fiveed_regen(regen-keycodes tool_keycodes)
fiveed_regen(regen-system-api tool_system_api)
fiveed_regen(regen-font-api tool_font_api)
fiveed_regen(regen-graphics-api tool_graphics_api)
# Generates custom_api_* from the API annotations in 5ed_api_implementation.cpp.
fiveed_regen(regen-custom-api tool_api_parser src/core/5ed_api_implementation.cpp)
fiveed_regen(check-api-docs tool_api_docs)

add_custom_target(regen)
add_dependencies(regen regen-keycodes regen-system-api regen-font-api
                       regen-graphics-api regen-custom-api)

add_custom_target(check-structure
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/check-structure.sh ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    DEPENDS 5ed
    VERBATIM)
