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

# The build writes one executable. It finds fonts/, themes/ and config files
# next to that executable.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Old builds made a core library and a custom library. The editor does not use
# them now. Remove them and their object folders from an old build folder.
foreach(old_library 5ed_app custom_5ed)
    file(REMOVE ${CMAKE_BINARY_DIR}/${old_library}.so)
    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/CMakeFiles/${old_library}.dir)
endforeach()

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

# 5ed_base is compiled once and is linked into the one executable.
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
    src/base/5ed_stdio_file.cpp
    src/base/5ed_command_map.cpp
    src/base/5ed_lexer_cpp_unit.cpp
    src/base/5ed_system_helpers.cpp
    src/base/5ed_system_allocator.cpp)
target_link_libraries(5ed_base PUBLIC 5ed_common)
set_target_properties(5ed_base PROPERTIES CXX_VISIBILITY_PRESET hidden)

################################################################################
# Custom layer. It is an object library linked into 5ed.
#
# Build the command table from the custom layer.
# Preprocess that layer with -DMETA_PASS.
# metadata_generator reads that output and writes the table.
# Compile the layer again with the generated files.
# Keep the output in the build tree.
# Paths in the table are relative to the source root.

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
add_custom_target(command_metadata DEPENDS ${GENERATED_METADATA})

# managed_id_metadata.cpp is #included by 5ed_default_bindings.cpp, not compiled on its own.
set_source_files_properties(${GENERATED_METADATA} PROPERTIES HEADER_FILE_ONLY TRUE)
add_library(5ed_custom OBJECT src/custom/5ed_default_bindings.cpp ${GENERATED_METADATA})
target_include_directories(5ed_custom PRIVATE ${GENERATED_ROOT})
target_link_libraries(5ed_custom PRIVATE 5ed_common 5ed_base)

################################################################################
# Core. It is an object library linked into 5ed.

add_library(5ed_core OBJECT src/core/5ed_app_target.cpp)
target_link_libraries(5ed_core PRIVATE 5ed_common 5ed_base)

################################################################################
# Platform layer: 5ed

add_executable(5ed src/platform/linux/linux_5ed.cpp)
target_compile_options(5ed PRIVATE -fno-threadsafe-statics)
target_link_libraries(5ed PRIVATE 5ed_core 5ed_custom 5ed_common 5ed_base
    X11::X11 X11::Xfixes OpenGL::GL Freetype::Freetype ${CMAKE_DL_LIBS} m rt)

# Release does not use link-time optimization.
# IPO warns about OpenGL function pointer types. See ROADMAP item 3 and item 6.

################################################################################
# Runtime files (fonts, themes, default config and bindings) beside the binary.

add_custom_target(5ed_runtime_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            ${PROJECT_SOURCE_DIR}/ship_files ${CMAKE_BINARY_DIR}
    COMMENT "Copying ship_files"
    VERBATIM)
add_dependencies(5ed 5ed_runtime_files)

################################################################################
# Developer tools are not part of the default build.
# regen-* rewrites checked-in files in src/base/generated/.
# regen does not run regen-lexer.
# That output differs from the checked-in lexer_cpp.cpp.
# Run regen-lexer only when the lexer definition changes.
# Do not build regen-lexer with a target that compiles the lexer.
# Regenerated keycodes differ only in indentation.

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

# Builds every generator tool. It does not run them, so no checked-in file changes.
add_custom_target(check-tools)
add_dependencies(check-tools tool_lexer_gen tool_keycodes tool_system_api tool_font_api
                             tool_graphics_api tool_api_parser tool_api_docs)

# The tools link 5ed_base too, so define-once also checks their objects.
add_custom_target(check-structure
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/check-structure.sh ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    VERBATIM)
add_dependencies(check-structure 5ed check-tools)

# Parses a new project.5ed from setup_new_project, this repo's project.5ed and
# the shipped config.5ed with the real config parser. Not part of ALL.
add_executable(test_project_file EXCLUDE_FROM_ALL
    src/custom/tests/5ed_project_file_test.cpp
    src/custom/tests/5ed_project_file_stubs.cpp)
target_include_directories(test_project_file PRIVATE ${GENERATED_ROOT})
target_link_libraries(test_project_file PRIVATE 5ed_common 5ed_base 5ed_core)
# command_metadata makes the generated command metadata that the test includes.
add_dependencies(test_project_file command_metadata)
add_custom_target(check-project-file
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/test-project
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test-project
    COMMAND test_project_file ${CMAKE_BINARY_DIR}/test-project
            ${PROJECT_SOURCE_DIR}/project.5ed ${PROJECT_SOURCE_DIR}/ship_files/config.5ed
    DEPENDS test_project_file
    VERBATIM)

# Parses command lines with the real core parser. Not part of ALL.
add_executable(test_command_line EXCLUDE_FROM_ALL
    src/core/tests/5ed_command_line_test.cpp
    src/custom/tests/5ed_project_file_stubs.cpp)
target_link_libraries(test_command_line PRIVATE 5ed_common 5ed_base 5ed_core)
add_custom_target(check-command-line
    COMMAND test_command_line
    DEPENDS test_command_line
    VERBATIM)

# Security regression checks. See scripts/check-security.sh.
add_custom_target(check-security
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/check-security.sh ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    DEPENDS 5ed_base 5ed
    VERBATIM)
