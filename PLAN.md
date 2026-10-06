# PLAN: ROADMAP.md points 1 and 2

Branch: `feat/implement-points-1-and-2-of-roadmap-md-1006-1406`
Repo: `/home/samh/Work/5ed`

Read this whole file before you change code. You have no other context.

## 1. Goal and non-goals

### Goal

Feature prompt (exact):

> implement points 1 and 2 of ROADMAP.md (section "1. Finish the Linux-only cleanup" and section "2. Unify the base layer" in /home/samh/Work/5ed/ROADMAP.md). Do not implement points 3 and later.

The text of the two sections in `ROADMAP.md`:

Section 1, "Finish the Linux-only cleanup":
- Remove the remaining OS and compiler detection for Windows, Mac and Clang in `custom/5ed_base_types.h`, and the branches that depend on it (`OS_WINDOWS`, `OS_MAC`, `ARCH_X86`, `COMPILER_CL`).
- Remove the `OS_LINUX` conditionals once nothing else is a target.
- Drop dead config: `default_compiler_bat`, `default_flags_bat`, `.bat` build-script generation in `setup_build_bat*`, the `.win` / `.mac` keys accepted by the project parser, and `ship_files` text that mentions them.
- Remove `4ed_data.ctm` handling if unused, and other leftovers from the demo/super tiers (`FRED_INTERNAL` is the only flag still used).

Section 2, "Unify the base layer":
- `5ed_base_types.cpp` (7.3k lines) is compiled separately into the platform layer, the core and the custom layer. Define it once.
- Resolve the double definitions between core and custom (upstream's note 5) and the collision-avoidance names introduced by the rebrand (`5ed_stdio_file`, `5ed_layout_lookup`, `5ed_log_helpers`).
- Move to a layout without the flat `-I . -I custom` include namespace: `src/base`, `src/core`, `src/custom`, `src/platform`.

Each roadmap item must leave the tree building and the editor launching.

### Non-goals

- Do NOT do point 3 or later. Specifically:
  - Do not remove the `.so` split. `5ed_app.so` and `custom_5ed.so` stay, with the `dlopen` code, `CLAct_CustomDLL`, `get_version` and `init_apis`.
  - Do not change the `custom_api` / `system_api` vtable boundary.
  - Do not replace the command metadata generator, the API generators, the lexer generator or the docs system.
  - Do not change config parsers or config paths (`~/.5ed`).
  - Do not port to Wayland. Do not touch OpenGL loading, audio or warnings.
  - Do not trim themes, do not replace the tutorial text (except the one `build.bat` line named in step 1.6), do not rename `FCODER_*` include guards.
- Do not change editor behavior. The only user-visible changes are: the removed `.bat` setup commands, and the changed `build` and `setup` doc strings.
- Do not rename source files, except for the moves in step 2.1. Keep the `5ed_` prefix on file names.

## 2. Repo context

### Stack
- 5ed: a fork of the 4coder text editor. C++11 (the code does not build as C++20). Linux x86-64 only. X11/GLX, FreeType, OpenGL 2.1 compatibility.
- Compiler: GCC only (local: g++ 16.2.1). Build system: CMake 3.20 or newer (local: 4.4.3). No Clang, no MSVC.
- The code base is a set of "unity" translation units. Each `.cpp` file includes many other `.cpp` files. `function` and `internal` are macros for `static`. `global` is `static`. `external` is `extern "C"`.
- Three binaries, built from three unity roots:
  - Platform layer: `platform_linux/linux_5ed.cpp` gives the executable `5ed`.
  - Core: `5ed_app_target.cpp` gives `5ed_app.so`.
  - Custom layer: `custom/5ed_default_bindings.cpp` (it includes `custom/5ed_default_include.cpp`) gives `custom_5ed.so`.
- The executable `dlopen`s the two `.so` files at startup. Never unload.

### Current layout (before your work)
- Repo root: core files `5ed*.cpp/.h`, `CMakeLists.txt`, `README.md`, `ROADMAP.md`, `project.5ed`, `UPSTREAM_CHANGES.txt`, `LICENSE`.
- `custom/`: custom layer (about 140 files). It also holds `custom/5ed_base_types.{h,cpp}`, `custom/generated/` (checked-in generated files), `custom/languages/`, `custom/lexer_generator/`.
- `generated/`: checked-in `font_api.*`, `graphics_api.*`.
- `docs/`: API doc system (compiled into the core).
- `platform_linux/`, `platform_unix/`, `opengl/`: platform layer.
- `ship_files/`: fonts, themes, `config.5ed`, `bindings.5ed`. Copied beside the binary at build time.
- Include flags today: `-I <root> -I <root>/custom` (flat namespace). Platform also uses `-I platform_unix`.

### Install, build, run, test
```
cmake -S . -B build                          # Debug is the default
cmake --build build -j
./build/5ed
```
- Release: `cmake -S . -B build-rel -DCMAKE_BUILD_TYPE=Release && cmake --build build-rel -j`.
- Baseline: a clean Debug build takes about 11 s on `-j8`. It finishes with no error.
- Baseline run: `cd build && timeout 4 ./5ed -w 900 600 ../README.md; echo $?` prints `open: No such file or directory` (existing message, ignore it) and exit code `124` (killed by timeout, so the editor was running).
- There is NO test suite in the repo (ROADMAP point 7 adds one later). Step 4 of this plan adds one structure check script. Run it with `cmake --build build --target check-structure`.
- The build writes `5ed`, `5ed_app.so`, `custom_5ed.so` and `ship_files/*` into `build/`. It generates `build/gen/generated/command_metadata.h` and `managed_id_metadata.cpp` (command table). It does so from the custom layer with `-E -DMETA_PASS` plus `metadata_generator`.
- Developer targets (not in the default build; the `regen-*` ones rewrite checked-in files): `regen` (keycodes, system API, font API, graphics API, custom API), `regen-lexer` (do NOT run; its output differs from the checked-in lexer), `check-api-docs`.

### Conventions
- Code style: 4 spaces, `function`/`internal` return type on its own line, name on the next line, `){` on the same line. Keep it.
- File header comments use `// TOP` and `// BOTTOM`. Keep them.
- Header guard names are `FCODER_*` or `FRED_*`. Keep them (point 8 handles them).
- Commit messages and comments you add: ASD-STE100 Simplified Technical English. Short sentences, active voice.
- Do not run `regen-lexer`. Do not run formatters.
- Throwaway scripts: put them in `/tmp`. Do not commit them.

### Facts you need (checked in the repo)
- `custom/5ed_base_types.h` holds OS/compiler/arch detection (lines 7-165), the macros `OS_NAME`, `ARCH_NAME`, `COMPILER_NAME`, `CALL_CONVENTION`, `JUST_GUESS_INTS`, and all base types. It has no function prototypes. The functions are defined in `custom/5ed_base_types.cpp` (about 1000 `function` definitions, 156 `operator` overloads, 14 `global` and several `global_const` variables, 53 `#define` lines, constructors of `Scratch_Block` and `Temp_Memory_Block`).
- Users of removed macros outside `5ed_base_types.h`:
  - `OS_NAME`: `custom/5ed_prj_v1.cpp` (lines 87, 191, 336) and `custom/5ed_project_commands.cpp` (lines 673, 802, 921).
  - `COMPILER_CL`, `COMPILER_GCC`, `COMPILER_CLANG`: `custom/5ed_command_map.cpp` lines 793-826.
  - `OS_LINUX`: `opengl/5ed_opengl_defines.h` line 214 (`#if !OS_LINUX` block) and `custom/5ed_metadata_generator.cpp` line 27 (`#ifdef OS_LINUX`).
  - `CALL_CONVENTION`: `platform_linux/linux_5ed.cpp` line 577.
  - `FTECH_64_BIT`: only `CMakeLists.txt` (no source uses it).
- `4ed_data.ctm`: only `.gitignore` mentions `5ed_data.ctm`. No source uses it.
- The project file key for the OS is the string `linux`. The file `project.5ed` in the repo root uses `.linux`. Keep that key.
- Files that two or three of the unity roots include today (checked with a script on the includes of the three roots). See the table in step 2.2.
- Tools that find output paths from `__FILE__` (these break when files move): `5ed_api_definition.cpp` (lines 395-445, roots `generated/` and `custom/generated/`), `5ed_generate_keycodes.cpp` (line 182), `docs/5ed_doc_custom_api_main.cpp` (line 44), `custom/lexer_generator/5ed_lex_gen_main.cpp` (lines 3970-4010).
- `5ed_api_definition.cpp` line 192 includes `5ed_stringf.cpp` (a hidden duplicate).

## 3. Implementation steps

Work in this order. After each numbered group, build Debug and run the editor (see section 6, step A and B) before you go on. Commit points are the supervisor's job. Do not commit.

### Group 1: ROADMAP point 1 (Linux-only cleanup). Do this in the OLD layout.

**1.1 Simplify `custom/5ed_base_types.h` (top of file).**
- Delete lines 12-125 (the `_MSC_VER`, `__clang__` and `__GNUC__` detection, and the "zeroify" blocks). Keep the `SHIP_MODE` block (lines 128-133). `opengl/5ed_opengl_render.cpp` and `Assert` use it.
- Replace the deleted detection with this guard only:
  ```
  #if !defined(__GNUC__) || !defined(__gnu_linux__) || !defined(__x86_64__)
  # error 5ed supports GCC on x86-64 Linux only
  #endif
  ```
- Delete the macros `OS_*`, `ARCH_*`, `COMPILER_*`, `OS_NAME`, `ARCH_NAME`, `COMPILER_NAME`, `CALL_CONVENTION`, `JUST_GUESS_INTS`, the `_MSC_VER` blocks (`snprintf`, `__VA_OPT__`) and the `#if defined(JUST_GUESS_INTS)` int typedef branch. Keep only the `<stdint.h>` branch.
- Run `grep -rnE "OS_|ARCH_|COMPILER_|CALL_CONVENTION|JUST_GUESS_INTS" --exclude-dir=.git --exclude-dir=build --exclude=lexer_cpp.cpp --exclude=ROADMAP.md .` after the edits. The result must show only unrelated text (for example `SIGN_ARCH`-like names; check each hit).

**1.2 Fix the users of removed macros.**
- `custom/5ed_command_map.cpp` lines 793-826: keep only the GCC branch (`##__VA_ARGS__` versions). Remove the `#if COMPILER_CL`, `#elif`, `#else #error` and `#endif` lines.
- `opengl/5ed_opengl_defines.h` lines 214-217: delete the `#if !OS_LINUX ... #endif` block (it is already inactive).
- `custom/5ed_metadata_generator.cpp` lines 27-31: replace the `#ifdef OS_LINUX ... #else ... #endif` by `#include <inttypes.h>` and `#define FMTi64 PRIi64`.
- `platform_linux/linux_5ed.cpp` line 577: remove `CALL_CONVENTION` from the `GL_FUNC` typedef. Check `opengl/5ed_opengl_funcs.h` and other uses with grep.
- `OS_NAME`: add to `custom/5ed_project_commands.h` the line `#define PRJ_OS_KEY "linux"` with the comment `// The project file key for this OS. Project files use the key .linux.` Replace each `OS_NAME` in `custom/5ed_prj_v1.cpp` and `custom/5ed_project_commands.cpp` with `PRJ_OS_KEY`. Check that `5ed_project_commands.h` is included before `5ed_prj_v1.cpp` (it is: `5ed_default_include.cpp` lines 58-59).

**1.3 Remove the `.bat` support (`custom/5ed_project_commands.{h,cpp}`).**
- Delete the functions `prj_generate_bat` (cpp lines 378-413, header line 77), the commands `setup_build_bat` and `setup_build_bat_and_sh` (cpp lines 1018-1034), the struct member `Prj_Setup_Status::bat_exists`, and the flag `PrjSetupScriptFlag_Bat`. Set `PrjSetupScriptFlag_Sh = 0x2`.
- In `prj_setup_scripts`: delete `do_bat_script`, every `bat_exists` use, the block that reads `default_flags_bat` and `default_compiler_bat`, and the "batch script already exists" message.
- Fix two existing bugs that the removal exposes:
  - In `prj_file_is_setup`, the last block stores the result of `project.5ed` in `sh_exists`. Store it in `project_exists`.
  - In `prj_setup_scripts`, the shell script branch tests `!status.bat_exists`. Test `!status.sh_exists`.
  - Set `everything_exists = sh_exists && project_exists`.
- `setup_new_project` calls `prj_setup_scripts` with `PrjSetupScriptFlag_Project|PrjSetupScriptFlag_Sh`. Change its `CUSTOM_DOC` text to `Queries the user for several configuration options and initializes a new 5ed project with a build shell script.` Change the `CUSTOM_DOC` of `setup_build_sh` only if it names a batch file (it does not).
- Remove the `.win` and `.mac` keys:
  - `prj_stringize_project` (cpp line 210): `os_strings` holds only `linux`. Keep the loop shape, or replace the array by `PRJ_OS_KEY`.
  - `prj_generate_project` (cpp lines 427-505): delete `od_win`, `bf_win`, the `*.bat` pattern line, the `.win`/`.mac` lines in `load_paths` and `commands`. Write only `.linux`. Keep the output otherwise the same. Do not remove `"*.m"` (not in scope).
- Fix the doc strings in `custom/5ed_build_commands.cpp` (lines 109, 146): replace `Looks for a build.bat, build.sh, or makefile` by `Looks for a build.sh or makefile`. Check the arrays at lines 21-30: they already list only `build.sh` and `makefile`-type entries. Do not change behavior.
- `custom/5ed_tutorial.cpp` line 659: change the text to `searches for and runs a build script ('build.sh')`.

**1.4 Remove dead config text.**
- `ship_files/config.5ed` lines 82-83: delete `default_compiler_bat` and `default_flags_bat`. Keep `default_compiler_sh` and `default_flags_sh` and the comment above them.
- Run `grep -rn "default_compiler_bat\|default_flags_bat" --exclude-dir=.git --exclude-dir=build .`. Only `ROADMAP.md` may match.

**1.5 Remove demo/super-tier leftovers.**
- `.gitignore`: delete the line `5ed_data.ctm`. (Nothing in the source uses it.)
- Delete `FCODER_TRANSITION_TO`: lines 10-12 of `custom/5ed_default_include.cpp`, and the `#if FCODER_TRANSITION_TO < 4001004 ... #endif` block in `custom/5ed_clipboard.cpp` (lines 403-420). The block is active with the default value 0. It defines wrapper overloads that take `Application_Links *app`. A grep found no caller of these wrappers (`clipboard_clear(app`, `clipboard_post(app`, `clipboard_count(app`, `push_clipboard_index(app`). Delete the whole block. If the build shows a caller, change the caller to the core form without `app`.
- `CMakeLists.txt`: remove `FTECH_64_BIT`. Keep `_GNU_SOURCE` and the `FRED_INTERNAL` Debug define.
- Keep: `FRED_INTERNAL` (used in `platform_unix/unix_5ed_functions.cpp`), `SHIP_MODE`, `META_PASS`, `STATIC_LINK_API`, `DYNAMIC_LINK_API`.
- Leave `INSO_DEBUG`, `USE_LOG`, `SNIPPET_EXPANSION`, `MAC_THREADING_WRAPPER` (guard name in `platform_unix/unix_threading_wrapper.h`) unless you find them tied to a removed OS. `MAC_THREADING_WRAPPER` is only an include guard: rename it to `UNIX_THREADING_WRAPPER`.

**1.6 CMake text.** In `CMakeLists.txt`, update the comment `# 5ed_base_types.h only recognises GCC on Linux.` so it stays true. Keep the GCC and x86-64 checks.

**1.7 Docs for group 1.** In `README.md`, no change yet. At the end of group 2 you update README and ROADMAP (step 4.3).

**Group 1 check:** Debug build passes. `./build/5ed` starts. `build/gen/generated/command_metadata.h` no longer lists `setup_build_bat` and `setup_build_bat_and_sh` (`grep -c setup_build_bat build/gen/generated/command_metadata.h` prints `0`).

### Group 2: ROADMAP point 2, part A: new directory layout (still unity per target)

Goal: new layout and one include root `-I src`. Each target still includes the same `.cpp` files. Nothing is shared yet.

**2.1 Move files with `git mv`.** Create `src/base`, `src/core`, `src/custom`, `src/platform`. Use this mapping. Write a throwaway script in `/tmp` that does the moves.

First check that all base names are unique: `find . -path ./.git -prune -o -path ./build -prune -o -type f -printf '%f\n' | sort | uniq -d`. The result must be empty. (The `5ed_` names are unique. If a duplicate shows, stop and list it in the report.)

| Source | Destination |
|---|---|
| `custom/5ed_base_types.{h,cpp}` | `src/base/` |
| `custom/5ed_stringf.cpp`, `5ed_hash_functions.cpp`, `5ed_table.{h,cpp}`, `5ed_codepoint_map.cpp`, `5ed_events.{h,cpp}`, `5ed_command_map.{h,cpp}`, `5ed_string_match.{h,cpp}`, `5ed_token.{h,cpp}`, `5ed_buffer_seek_constructors.cpp`, `5ed_layout_lookup.cpp`, `5ed_log_helpers.cpp`, `5ed_doc_content_types.{h,cpp}`, `5ed_malloc_allocator.cpp`, `5ed_stdio_file.{h,cpp}`, `5ed_types.h`, `5ed_version.h`, `5ed_system_types.h`, `5ed_default_colors.h` | `src/base/` |
| `5ed_mem.cpp` (root) | `src/base/` |
| `custom/generated/*` and `generated/*` (all files) | `src/base/generated/` |
| `custom/5ed_system_allocator.cpp`, `5ed_app_links_allocator.cpp`, `5ed_system_helpers.cpp`, `5ed_profile.{h,cpp}`, `5ed_profile_static_enable.cpp`, `5ed_search_list.{h,cpp}` | `src/base/per_target/` (see step 2.2) |
| `custom/lexer_generator/`, `custom/languages/` and all other `custom/*` files (including `5ed_config_grammar.txt`, `5ed_default_colors.cpp`) | `src/custom/` (keep subfolders) |
| `platform_linux/*` | `src/platform/linux/` |
| `platform_unix/*` | `src/platform/unix/` |
| `opengl/*` | `src/platform/opengl/` |
| `docs/*` | `src/core/docs/` |
| `5ed_api_check.cpp`, `5ed_api_definition_main.cpp`, `5ed_api_parser_main.cpp`, `5ed_system_api.cpp`, `5ed_font_api.cpp`, `5ed_graphics_api.cpp`, `5ed_generate_keycodes.cpp` (generator `main` files) | `src/core/tools/` |
| `5ed_font_provider_freetype.{h,cpp}` | `src/platform/` (same level as `linux/`) |
| all other root `5ed*.cpp`, `5ed*.h` (including `5ed_app_target.cpp`, `5ed.cpp`, `5ed.h`, `5ed_font_set.*`, `5ed_font_interface.h`, `5ed_render_target.*`, `5ed_string_matching.cpp`, `5ed_api_definition.*`, `5ed_api_parser.cpp`, `5ed_api_implementation.cpp`) | `src/core/` |

Stay in the repo root: `CMakeLists.txt`, `README.md`, `ROADMAP.md`, `PLAN.md`, `LICENSE`, `UPSTREAM_CHANGES.txt`, `project.5ed`, `.gitignore`, `ship_files/`.

The collision-avoidance names (`5ed_stdio_file`, `5ed_layout_lookup`, `5ed_log_helpers`) now live in `src/base/`. They no longer collide with core names, because core files are in `src/core/`. Keep the file names. Delete the sentence about collision in any comment you find (grep for `collide`).

Dependency rules for the new layout (the check script in step 4.1 tests them):
- `src/base` includes only `base/...` and system headers.
- `src/custom` includes `base/...` and `custom/...` only.
- `src/core` includes `base/...`, `core/...`.
- `src/platform` includes `base/...`, `core/...` (for `5ed.h`, font and render headers), `platform/...`.
- No file includes a `.cpp`, `.h` or `.txt` file by a bare name or by `../`. Every project include is written as `"<layer>/<path>"`, relative to `src/`. The only exception: `#include "generated/command_metadata.h"` and `"generated/managed_id_metadata.cpp"`. These two files exist only in `build/gen/generated/`.

**2.2 Rewrite includes.** Write a throwaway python script in `/tmp`:
- Build a map from base file name to new path from the final tree (`git ls-files src`).
- For each `#include "X"` in `src/**` that names a file of the project, replace `X` with the new path relative to `src/`. Handle `generated/foo` (becomes `base/generated/foo`), `opengl/foo`, `docs/foo`, `../5ed_api_definition.h`, `5ed_doc_custom_api_global.cpp` (same folder includes), `pcg_basic.h/.c`, `alsa_funcs.txt`, `linux_icon.h`, `unix_*.h`. Leave `<...>` includes and the two build-tree includes unchanged.
- The script must report each include it cannot resolve. Fix those by hand.
- Per-target files: `src/base/per_target/` holds files that call the system or custom API through `STATIC_LINK_API` or `DYNAMIC_LINK_API` names. They need a separate compile in each binary until ROADMAP point 3. They stay included by each unity root. Do not add a README for them. Put the list in the `CMakeLists.txt` comment from step 3.3. The files are: `5ed_system_allocator.cpp`, `5ed_app_links_allocator.cpp`, `5ed_system_helpers.cpp`, `5ed_profile.cpp`, `5ed_profile_static_enable.cpp`, `5ed_search_list.cpp`, `generated/custom_api.cpp`, `generated/system_api.cpp`, `generated/font_api.cpp`, `generated/graphics_api.cpp`, `generated/lexer_cpp.cpp` (see risk 13), and `5ed_font_set.cpp` (in `src/core/`; both the core and the platform compile it). I checked each one except the lexer: they call `system_*`, `get_thread_context`, `get_core_profile_list` or `Application_Links` constructors.

**2.3 Update `CMakeLists.txt` for the new paths (still three unity roots).**
- Include dirs: `${PROJECT_SOURCE_DIR}/src` only (in `5ed_common`). Remove `${PROJECT_SOURCE_DIR}/custom` and the `platform_unix` include. The `custom_5ed` target keeps `${GENERATED_ROOT}` as a private include dir.
- The metadata step: `-Icustom` becomes `-Isrc`; input file `src/custom/5ed_default_bindings.cpp`. The `-E` command runs in the source root, so the recorded paths in `command_metadata.h` change from `custom/...` to `src/custom/...`. That is expected.
- `file(GLOB_RECURSE CUSTOM_LAYER_FILES ... src/custom/*.cpp src/custom/*.h)` and the filter regex `"/src/custom/(lexer_generator|languages)/"`.
- Update all source paths of the targets and the tools (`src/custom/5ed_metadata_generator.cpp`, `src/core/5ed_app_target.cpp`, `src/platform/linux/linux_5ed.cpp`, the `fiveed_tool(...)` lines, `regen-custom-api` argument `src/core/5ed_api_implementation.cpp`).
- Update the comment block above the developer tools: the checked-in generated files are now in `src/base/generated/`.
- `.gitignore`: update `generated/*_master_list.h` and `generated/*_constructor.cpp` to `src/base/generated/font_api_*`, `graphics_api_*` forms. Check with `git status` that the by-product files (font and graphics master list and constructor) stay ignored and that the checked-in `custom_api_master_list.h`, `custom_api_constructor.cpp`, `system_api_master_list.h`, `system_api_constructor.cpp` stay tracked. Note: those four files are tracked today under `custom/generated/`; keep them tracked.

**2.4 Fix `__FILE__`-relative output paths in the generator tools.** Each tool must write to `src/base/generated/` after the move. Change each so that it computes the `src/` folder from `__FILE__` (remove the right count of folders with `string_remove_last_folder`) and appends `base/generated/`:
- `src/core/5ed_api_definition.cpp` (function `api_definition_generate_api_includes`): the two roots `generated/` and `custom/generated/` both become `base/generated/`, relative to `src/`. `path_to_self` is now `.../src/core/`; remove one folder to get `.../src/`.
- `src/core/tools/5ed_generate_keycodes.cpp`: output `base/generated/5ed_event_codes.h` relative to `.../src/` (the file is in `src/core/tools/`; remove two folders).
- `src/core/docs/5ed_doc_custom_api_main.cpp`: the input is `base/generated/custom_api_master_list.h`. It is now in `src/core/docs/`. Remove two folders.
- `src/custom/lexer_generator/5ed_lex_gen_main.cpp`: `path_to_src` (parent of `path_to_self`) is `.../src/custom/`. The output folder becomes `.../src/base/generated/` (parent of `path_to_src`, plus `base/generated/`). Do not run `regen-lexer`. Only read the code and make the change.
- Also check `custom/languages/5ed_cpp_lexer_gen.cpp` and `5ed_cpp_lexer_test.cpp` for relative paths.

**2.5 Group 2 check.** Debug build passes. `./build/5ed` starts. Run `cmake --build build --target regen-keycodes regen-system-api regen-font-api regen-graphics-api regen-custom-api`, then `git status --short src/base/generated`. Expected: no change in the generated files. One exception: `5ed_event_codes.h` may differ in indentation (known before this work). If you see other changes, find the path bug before you go on. Then restore with `git checkout -- src/base/generated`.

### Group 3: ROADMAP point 2, part B: define the base layer once

Goal: the files in the "once" list compile exactly one time into a static library `5ed_base`. The platform, the core, the custom layer and all tools link it. No unity root includes those `.cpp` files any more.

"Once" list (all in `src/base/`; no API calls, checked with a script against the API function names):
`5ed_base_types.cpp`, `5ed_stringf.cpp`, `5ed_hash_functions.cpp`, `5ed_table.cpp`, `5ed_codepoint_map.cpp`, `5ed_events.cpp`, `5ed_command_map.cpp`, `5ed_string_match.cpp`, `5ed_token.cpp`, `5ed_buffer_seek_constructors.cpp`, `5ed_layout_lookup.cpp`, `5ed_log_helpers.cpp`, `5ed_doc_content_types.cpp`, `5ed_mem.cpp`, `5ed_malloc_allocator.cpp`, `5ed_stdio_file.cpp`.

The generated lexer `generated/lexer_cpp.cpp` is NOT in this list (risk 13).

If one of these files fails to compile alone because it needs an API call, move it to `src/base/per_target/`, add it to the list in the CMake comment, and write it in the report. Do not hide it.

**3.1 Add a base umbrella header.** Create `src/base/5ed_base.h`. It includes, in the order the unity roots use today: `base/5ed_base_types.h`, `base/5ed_version.h`, `base/5ed_table.h`, `base/5ed_events.h`, `base/5ed_types.h`, `base/5ed_doc_content_types.h`, `base/5ed_default_colors.h`, `base/generated/custom_api.h` (types only: define neither `STATIC_LINK_API` nor `DYNAMIC_LINK_API`), `base/5ed_system_types.h`, `base/generated/system_api.h` (same), `base/5ed_token.h`, `base/generated/lexer_cpp.h`, `base/5ed_string_match.h`, `base/5ed_command_map.h`, and the new prototype headers of step 3.2. Every file of the "once" list starts with `#include "base/5ed_base.h"`. Keep the include order of the unity roots for their own includes (they set `STATIC_LINK_API` or `DYNAMIC_LINK_API` between headers). Fix any order problem that the compiler shows.

**3.2 Give the base functions external linkage and prototypes.** Today every base function is `static` (via `function`) and has no prototype; the `.cpp` is included in each unity root. For each file of the "once" list:
1. Write a throwaway python script in `/tmp` that does a mechanical split. It reads a `.cpp` and scans the top level (brace depth 0). A definition starts at a line that begins with `function`, `internal` or `static`. Its header is the text up to the first `{` at depth 0. The script then:
   - removes the `function`/`internal`/`static` qualifier from the definition in the `.cpp`;
   - writes the header text (without qualifier) plus `;` as a prototype into the matching header;
   - for `global` and `global_const` variables (for example `Ii32_neg_inf`, `zero_data`, `Rf32_infinity`, `utf8_class`, `integer_symbols`, `base64`, `table_empty_slot`, `string_empty`): writes `extern <type> <name>;` or `extern const <type> <name>[...];` into the header, and in the `.cpp` removes `global`/`global_const`/`function` (the `extern` declaration from the header gives the definition external linkage; if a `const` object still has internal linkage, add `extern` before the definition);
   - moves every top-level `#define` (53 in `5ed_base_types.cpp`; the `Bind*` macros in `5ed_command_map.cpp`; the macros in `5ed_log_helpers.cpp`) and its `#include <math.h>` (the `#if C_MATH` block: delete `C_MATH`) into the header, in the original order and before the prototypes;
   - leaves member function definitions (`Scratch_Block::Scratch_Block(...)`, `Temp_Memory_Block::...`) as they are. Their declarations are already in `5ed_base_types.h`.
2. Headers to receive prototypes:
   - `5ed_base_types.cpp`: new file `src/base/5ed_base_functions.h`. Include it at the end of `5ed_base_types.h`, before the final `#endif`. Then every includer of the types header gets the prototypes.
   - `5ed_stringf.cpp`, `5ed_hash_functions.cpp`, `5ed_codepoint_map.cpp`, `5ed_buffer_seek_constructors.cpp`, `5ed_layout_lookup.cpp`, `5ed_log_helpers.cpp`, `5ed_malloc_allocator.cpp`, `5ed_mem.cpp`: create `src/base/<same name>.h` with guards `FCODER_<NAME>_H`. Add each to `5ed_base.h` if base code needs it.
   - `5ed_table.cpp`, `5ed_events.cpp`, `5ed_command_map.cpp`, `5ed_string_match.cpp`, `5ed_token.cpp`, `5ed_doc_content_types.cpp`, `5ed_stdio_file.cpp`: add the prototypes at the end of the existing header, before its `#endif`.
   - Do not touch `generated/lexer_cpp.cpp`. It stays included by each unity root (core, custom layer and the tools that use it).
3. Include guards inside the `.cpp` files (for example `FCODER_BASE_TYPES_CPP`) are no longer needed. Delete them.
4. Delete all the dead `#include`s of these `.cpp` files from the unity roots:
   - `src/custom/5ed_default_include.cpp`, `src/core/5ed_app_target.cpp`, `src/platform/linux/linux_5ed.cpp` (also the second duplicate `5ed_hash_functions.cpp` include there),
   - `src/core/5ed_api_definition.cpp` line 192 (`5ed_stringf.cpp`),
   - tool roots: `src/custom/5ed_metadata_generator.cpp`, `src/custom/lexer_generator/5ed_lex_gen_main.cpp`, `src/custom/languages/5ed_cpp_lexer_test.cpp`, `src/core/docs/5ed_doc_custom_api_main.cpp`, `src/core/tools/*.cpp`.
   - Keep the header includes. Use `#include "base/5ed_base.h"` or the old header list.
5. If a base `.cpp` uses a macro or type that only a unity root defines (for example `file_name_line_number`, `Profile*` macros, `DYNAMIC_LINK_API` globals), fix the base file so it needs only base headers. If it truly needs the root, move the file to `per_target` (see above).

**3.3 Build the library in CMake.**
```
add_library(5ed_base STATIC <the once list, src/base/...>)
target_link_libraries(5ed_base PUBLIC 5ed_common)
set_target_properties(5ed_base PROPERTIES CXX_VISIBILITY_PRESET hidden)
```
- The library is PIC (global `CMAKE_POSITION_INDEPENDENT_CODE ON`). Hidden visibility keeps each binary on its own private copy of the base objects. This matches today's behavior, where each binary has its own copy.
- Link `5ed_base` into `5ed`, `5ed_app`, `custom_5ed`, `metadata_generator` and every `fiveed_tool` target (the function `fiveed_tool` links `5ed_base` for all). Put a comment above the library: "Compiled once. The per-target files are listed below. They call the system or custom API through a different link mode in each binary. They stay compiled per binary until ROADMAP point 3." List the per-target files in that comment.
- `custom_5ed` and `5ed_app` are `SHARED` libraries that link a static library. Check that `5ed` still finds the symbols `get_version`, `init_apis` and the others that the `.so` files export. Do not change their visibility.
- Release builds: the base functions are no longer inlined into callers (small math helpers). Set `CMAKE_INTERPROCEDURAL_OPTIMIZATION` for the Release configuration with `include(CheckIPOSupported)` and `check_ipo_supported(RESULT ipo_ok)`. If the Release build then fails or the link time becomes extreme, remove the IPO setting and write this in the README under "Build".
- `-fno-threadsafe-statics` applies to the platform target only today. Check with `grep -n "static [A-Za-z_]* [a-z_]* *=" src/base/*.cpp` that no base function uses a function-local `static` with a dynamic initializer. If one exists, add the same flag to `5ed_base`.

**3.4 Clean up the per-target files.** Each unity root still includes the per-target `.cpp` files. Update their include paths only. Add this one-line comment above the first of them in each root: `// Per-target API-bound files. See CMakeLists.txt.`

**3.5 Group 3 check.** Debug and Release builds pass. `./build/5ed` starts. `cmake --build build --target check-structure` passes (step 4.1).

### Group 4: finish

**4.1 Add the check script and CMake target.** See section 4 (tests). Create `scripts/check-structure.sh`. Add to `CMakeLists.txt`:
```
add_custom_target(check-structure
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/check-structure.sh ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    DEPENDS 5ed
    VERBATIM)
```
(Not part of `ALL`.)

**4.2 Check generated files in `src/base/generated/` are not edited by hand** except for path moves. Run the `regen` check from step 2.5 again at the end.

**4.3 Update docs.**
- `README.md`: the build section (outputs unchanged), the repo layout (`src/base`, `src/core`, `src/custom`, `src/platform`, one include root `-I src`), the developer targets (paths now `src/base/generated/`; add `check-structure`), and the "Inherited problems" list. Remove problem 5 ("Weak base layer...") and renumber. Remove any text about `.bat` or Windows.
- `ROADMAP.md`: change the "Status" line to list "Linux-only cleanup (item 1), base layer and source layout (item 2)" as done. Delete sections 1 and 2, or mark each as `(done)`. Keep sections 3 to 8 unchanged. Keep the text that item 3 will merge the per-target files.
- Write the remaining per-target duplicates into ROADMAP section 3 as one new bullet: "The files listed in `CMakeLists.txt` as per-target stay compiled once per binary. Merge them here."
- Do not edit `UPSTREAM_CHANGES.txt`.

**4.4 Remove all scaffolds.** `git status` must show no stray files from `/tmp` scripts. No `PLAN.md` change by you.

## 4. Tests to write

The repo has no test framework. Do not add one (ROADMAP point 7). Add one script and one CMake target.

### 4.1 `scripts/check-structure.sh`
Bash, `set -eu`. Arguments: `$1` source root, `$2` build dir. Print one `PASS: <name>` or `FAIL: <name> ...` line per check. Exit non-zero if any check fails. Checks:

1. **no-removed-macros**: `grep -rnE "\b(OS_WINDOWS|OS_MAC|OS_LINUX|OS_NAME|ARCH_X86|ARCH_X64|ARCH_ARM(32|64)|ARCH_(32|64)BIT|ARCH_NAME|COMPILER_(CL|GCC|CLANG|NAME)|CALL_CONVENTION|JUST_GUESS_INTS|FTECH_64_BIT|FCODER_TRANSITION_TO)\b" src CMakeLists.txt ship_files` finds nothing.
2. **no-bat-or-other-os**: `grep -rniE "default_(compiler|flags)_bat|setup_build_bat|prj_generate_bat|\.bat\b|\.(win|mac) *=" src ship_files CMakeLists.txt` finds nothing. (Do not search for a bare `.win`: the X11 code uses `linuxvars.win`.) Comments in `README.md` and `ROADMAP.md` are outside the search.
3. **no-ctm**: `grep -n "ctm" .gitignore CMakeLists.txt` and `grep -rn "\.ctm" src ship_files` find nothing.
4. **layout**: the root has no `5ed*.cpp`, `5ed*.h`, `custom/`, `generated/`, `docs/`, `platform_linux/`, `platform_unix/` or `opengl/`. `src/base`, `src/core`, `src/custom`, `src/platform` exist.
5. **include-style**: every `#include "..."` in `src` either starts with `base/`, `core/`, `custom/` or `platform/`, or is one of the two build-tree includes (`generated/command_metadata.h`, `generated/managed_id_metadata.cpp`). Exception: includes inside `src/custom/lexer_generator/` of `pcg_basic.h` and `pcg_basic.c` must also use the full path `custom/lexer_generator/...`. No `../` in any project include.
6. **layer-direction**: files in `src/base` (not `per_target` and not `generated`) include only `base/`. Files in `src/custom` include only `base/` and `custom/`. Files in `src/core` include only `base/` and `core/`. Files in `src/platform` include `base/`, `core/` and `platform/`.
7. **base-cpp-not-included**: no file in `src` has `#include "base/...cpp"` for the once list (the list is in the script as an array). Only `per_target` and generated `.cpp` may be included as `.cpp`.
8. **define-once** (needs a finished build in `$2`): use `nm -C --defined-only`.
   - Take the defined global text symbols of `$2/lib5ed_base.a`. Pick the symbol names `i32_ceil32(float)`, `string_list_pushf`, `table_hash_u8`, `layout_nearest_pos_to_xy`, `log_event`. (Check the exact names with `nm -C` while you write the script; use names that exist.) Each must appear exactly once in `lib5ed_base.a`.
   - For all other object files (`find $2/CMakeFiles -name '*.o' -not -path '*5ed_base*'`), no symbol of `lib5ed_base.a` is defined (types `T t W w V v D d B b R r`). Build the symbol list once into a temp file; compare with `comm`.
   - In each of `$2/5ed`, `$2/5ed_app.so`, `$2/custom_5ed.so`, the symbol `i32_ceil32(float)` is defined exactly once. (Debug builds keep `.symtab`.)
9. **ship-files**: `ship_files/config.5ed` still defines `default_compiler_sh` and `default_flags_sh`.

Expected result before your work: checks 1 to 8 FAIL. After your work: all checks PASS.

### 4.2 Manual regression cases (no new code; run them in section 6)
- Debug and Release build.
- Editor starts and stays up.
- Project commands: `setup_new_project` makes `project.5ed` and `build.sh` only (no `build.bat`), and `project.5ed` has only `.linux` keys.
- The command lister has no `setup_build_bat` and no `setup_build_bat_and_sh`.
- `load_project` loads this repo's `project.5ed`.

## 5. Acceptance criteria

A user can see each of these.

- [ ] `cmake -S . -B build && cmake --build build -j` succeeds (Debug). No new warnings compared to the baseline (the build suppresses several; do not add suppressions).
- [ ] `cmake -S . -B build-rel -DCMAKE_BUILD_TYPE=Release && cmake --build build-rel -j` succeeds.
- [ ] `./build/5ed` opens a window, shows the 5ed UI and the default theme. It stays up until the user closes it.
- [ ] `git grep -nE "OS_WINDOWS|OS_MAC|OS_LINUX|ARCH_X86|COMPILER_CL|COMPILER_CLANG"` prints nothing outside `ROADMAP.md`.
- [ ] `src/base/5ed_base_types.h` has no OS, compiler or arch detection except one `#error` guard for non-GCC, non-Linux or non-x86-64.
- [ ] `git grep -n "default_compiler_bat\|default_flags_bat\|setup_build_bat\|prj_generate_bat"` prints nothing outside `ROADMAP.md`.
- [ ] In the editor, `Alt+x` (command lister) shows `setup_build_sh` and `setup_new_project`, and does not show `setup_build_bat` or `setup_build_bat_and_sh`.
- [ ] `setup_new_project` in an empty folder creates `project.5ed` and `<name>.sh`-style script only, with no `.bat` file. The generated `project.5ed` contains `.linux` keys and no `.win` or `.mac` key.
- [ ] `ship_files/config.5ed` has no `_bat` line.
- [ ] `.gitignore` has no `5ed_data.ctm` line.
- [ ] The repo root has no `5ed*.cpp` or `5ed*.h` file. The source is in `src/base`, `src/core`, `src/custom`, `src/platform`.
- [ ] `grep -rn '"\.\./' src` prints nothing. The compile command lines have one project include dir, `-I .../src` (plus the build `gen` dir for the custom layer). Check with `cmake --build build -- VERBOSE=1` on one file.
- [ ] `5ed_base_types.cpp` is compiled once: `grep -rn "5ed_base_types.cpp" src CMakeLists.txt` shows only the `add_library(5ed_base ...)` line.
- [ ] `cmake --build build --target check-structure` prints `PASS` for every check and exits 0.
- [ ] `cmake --build build --target regen` (or the five `regen-*` targets without `regen-lexer`) leaves `git diff --stat src/base/generated` empty. (The keycodes file can differ only in indentation; restore it.)
- [ ] `README.md` shows the new layout and no longer lists problem 5. `ROADMAP.md` marks points 1 and 2 done and keeps points 3 to 8.
- [ ] `ship_files/` contents arrive in `build/` as before (`ls build/fonts build/themes build/config.5ed build/bindings.5ed`).

## 6. Manual check script

Run these commands from `/home/samh/Work/5ed` in order. The display is `:0` (X11 or XWayland).

**A. Clean Debug build**
```
rm -rf build
cmake -S . -B build
cmake --build build -j8 2>&1 | tail -5
ls build/5ed build/5ed_app.so build/custom_5ed.so build/config.5ed build/bindings.5ed
```
Expected: the last build line is `[100%] Built target 5ed`. All five files exist.

**B. Start the editor**
```
cd build && timeout 5 ./5ed -w 900 600 ../README.md; echo "exit=$?"; cd ..
```
Expected: the window opens for 5 s. The output may include `open: No such file or directory` (existing message). The last line is `exit=124`.
A screenshot check (optional): `(cd build && ./5ed -w 900 600 ../README.md & sleep 3; import -window root /tmp/5ed-shot.png; kill %1)`. The picture shows the editor with `README.md` text.

**C. Command table (group 1)**
```
grep -c "setup_build_bat" build/gen/generated/command_metadata.h
grep -c "setup_build_sh\|setup_new_project" build/gen/generated/command_metadata.h
```
Expected: `0`, then a number greater than `0`.

**D. Project setup in an empty folder (needs the GUI)**
```
mkdir -p /tmp/5ed-demo && cd /tmp/5ed-demo && /home/samh/Work/5ed/build/5ed -w 1000 700
```
In the editor:
1. Press `Alt+x`. Type `setup_new_project`. Press Enter.
2. Answer the prompts: `Build Target:` `main.cpp`; `Output Directory:` `.` (or Enter for the default); `Binary Output:` `demo`.
3. Close the editor (`Ctrl+q`, or close the window).
Then:
```
ls /tmp/5ed-demo
grep -c "\.win\|\.mac" /tmp/5ed-demo/project.5ed
cat /tmp/5ed-demo/project.5ed
```
Expected: the folder has `project.5ed` and `build.sh`, and no `build.bat`. The `grep -c` prints `0`. `project.5ed` shows `version(2);`, `project_name = "demo";`, `.linux = ...` keys in `load_paths` and in `commands` (`.build` runs `./build.sh`, `.run` runs `./demo`), and no `*.bat` pattern. Clean up: `rm -rf /tmp/5ed-demo`.

**E. Load this project**
```
cd /home/samh/Work/5ed && ./build/5ed -w 1000 700 README.md
```
In the editor press `Alt+x`, type `load_project`, press Enter. Expected: the `*messages*` buffer shows no `Project errors:` text. `Alt+x` then `project_command_lister` lists `build`, `run` and `regen`.

**F. Release build**
```
cmake -S . -B build-rel -DCMAKE_BUILD_TYPE=Release
cmake --build build-rel -j8 2>&1 | tail -3
(cd build-rel && timeout 5 ./5ed -w 900 600 ../README.md; echo "exit=$?")
```
Expected: `[100%] Built target 5ed`, then `exit=124`. Remove `build-rel` when done.

**G. Structure check**
```
cmake --build build --target check-structure
```
Expected: every line starts with `PASS:`. The command exits 0.

**H. Generators write to the new place**
```
cmake --build build --target regen-system-api regen-font-api regen-graphics-api regen-custom-api regen-keycodes
git status --short src/base/generated
```
Expected: no output, or only `src/base/generated/5ed_event_codes.h` (indentation). Run `git checkout -- src/base/generated` after. Do NOT run `regen-lexer`.

## 7. Risks and open assumptions

1. **Size of group 3.** The split of `5ed_base_types.cpp` is mechanical, but it touches about 1000 functions, 156 operators and many macros. Use the script approach, and let the compiler find the gaps. If the script cannot handle a pattern (a function with a default argument, a function defined with a macro, a `static` local), fix that case by hand. If the split is not possible for a file, report which file and why. Do not leave a half-split tree.
2. **Release speed.** Base helpers (Vec/Rect math, string helpers) lose inlining. The plan uses IPO for Release. If IPO fails, the fallback is to accept the cost. Nobody measured the cost. `[INFERENCE]` The cost is small, because the code is not in an inner pixel loop. Check the frame feel in the Release run (step F).
3. **Per-target duplicates stay.** The files in the "per_target" list (`5ed_system_allocator.cpp`, `5ed_app_links_allocator.cpp`, `5ed_system_helpers.cpp`, `5ed_profile.cpp`, `5ed_profile_static_enable.cpp`, `5ed_search_list.cpp`, `5ed_font_set.cpp`, the four generated API `.cpp` files) call the system or custom API. Platform uses static links. The core and the custom layer use vtable pointers set at `dlopen` time. They cannot be one object while the `.so` split exists. This plan resolves them in ROADMAP point 3. This is the one place where the plan reads the roadmap line "resolve the double definitions between core and custom" as: all pure code is defined once, and the API-bound code is documented and isolated. If the supervisor wants more, point 3 must come first.
4. **Collision-avoidance names.** The plan keeps the file names `5ed_stdio_file`, `5ed_layout_lookup`, `5ed_log_helpers` and states that the new folders remove the collision. A rename to shorter names (for example `stdio_file`) is possible but breaks the `5ed_` naming that all other files use. Open assumption: no rename.
5. **`OS_NAME` replacement.** The project file key `.linux` stays, so old project files still load. The macro `PRJ_OS_KEY` replaces `OS_NAME`. If the supervisor wants the key removed (a project file without OS keys), that is a file-format change, outside points 1 and 2.
6. **`*.m` and `.mm`.** The generated project and `config.5ed` (`treat_as_code`) still mention Objective-C file types. Those are file types, not OS support. They stay.
7. **Metadata paths.** Recorded source paths in `command_metadata.h` change from `custom/...` to `src/custom/...`. If a feature shows those paths to the user, the text changes. `[INFERENCE]` No feature depends on the old form (the grep found no consumer other than the table itself). Check with `grep -rn "\.file\b\|source_name" src/custom` while you work.
8. **`regen-lexer`.** Its output differs from the checked-in lexer (known, not investigated). Group 2 changes its output path only. The change is untested on purpose.
9. **`__FILE__` paths.** The generators use `__FILE__` (absolute path under CMake). The folder counts in step 2.4 depend on the final file locations. Test with step 6.H.
10. **Hidden visibility on `5ed_base`.** This keeps a private base copy per binary. If a link error shows an undefined base symbol in an `.so`, check that the library is linked `PRIVATE` to that `.so` target and not only to the executable.
11. **Clipboard wrapper block** (step 1.5). A grep found no caller that passes `app`. If the build shows a caller anyway, change the caller. Write the result in the report.
12. **No unit tests.** The only automated check is `check-structure`. Runtime behavior is checked by hand (section 6). ROADMAP point 7 adds a real test suite later.
13. **Lexer stays per-target.** `src/base/generated/lexer_cpp.cpp` is generated. The generator emits its functions as `internal` and puts the struct `Lex_State_Cpp` in the `.cpp` file. To define it once, the generator in `src/custom/lexer_generator/5ed_lex_gen_main.cpp` (lines 3707, 3723, 3876) and the hand-written part must change too. ROADMAP point 4 plans to decide the fate of this generator, and `regen-lexer` output differs from the checked-in file for an unknown reason. So this plan leaves the lexer compiled once per binary (core and custom layer). Move it into the base library after point 4.
