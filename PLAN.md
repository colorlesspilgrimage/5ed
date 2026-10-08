# PLAN: ROADMAP item 3, "Remove the `.so` split"

Read this file fully before you start. It has all the context you need.
Write every document, comment, and commit message in ASD-STE100 (Simplified Technical English):
short sentences (20 words or fewer), active voice, one instruction per sentence, no idioms.

## 1. Goal and non-goals

### Feature prompt (exact)

> Implement the next point in ROADMAP.md. The next undone point is item 3 "Remove the `.so` split" (items 1 and 2 are marked done). Implement all bullets of item 3 as written (merge per-target files, link core+custom into the main executable, keep custom_api/system_api vtable boundary at first, remove CLAct_CustomDLL and the get_version/init_apis handshake, decide the customisation story and document it, try IPO/LTO for Release and measure it). Update ROADMAP.md to mark item 3 done with the decisions taken.

### Goal

After this work, `5ed` is ONE executable. It has no `5ed_app.so` and no `custom_5ed.so`.
The core layer, the custom layer, and the platform layer link into `build/5ed`.
No code calls `dlopen` for the core or the custom layer.

### Decisions already taken by the planner (do not reopen them)

| # | Decision | Reason |
|---|----------|--------|
| D1 | Core (`src/core/5ed_app_target.cpp`) and custom (`src/custom/5ed_default_bindings.cpp`) stay two translation units (TUs). They become CMake OBJECT libraries linked into `5ed`. | Both TUs are "unity builds" (one `.cpp` that includes many `.cpp`). A merge into one TU is not needed, and it hits name clashes. |
| D2 | The `custom_api` vtable stays in this item. | The ~180 `api(custom) function ...` definitions in `src/core/5ed_api_implementation.cpp` have file-static linkage. Direct calls need generator changes and a large edit. ROADMAP item 4 decides about the API generators. Record this in ROADMAP. |
| D3 | The `system_api` vtable is REMOVED. System functions become normal functions that the platform TU defines with external linkage. All TUs call them directly. | The merge of per-target files needs it. A file that is compiled once cannot call `system_*` through a pointer table. The platform calls `make_arena_system()` before any vtable exists. |
| D4 | The `font_api` and `graphics_api` vtables stay (core calls platform through pointers). | The prompt names only `custom_api` and `system_api`. Record in ROADMAP that item 4 or 6 may remove them. |
| D5 | Customisation story: edit `src/custom/` and rebuild. There is NO plugin path. | The custom layer is already linked to core file-static code, to a generated command table (`build/gen`), and to the API vtable, so it needs a rebuild after each core change. The old `.so` was never reloaded. Document this in `README.md` and `ROADMAP.md`. |
| D6 | `App_Functions` / `app_get_functions` stay (the platform calls `app_get_functions()` directly instead of `dlsym`). | Out of scope to remove. Keep the diff small. |
| D7 | `system_load_library`, `system_get_proc`, `system_release_library` stay in the system API. They become unused. | The list of API functions is generated. Item 4 decides. Note this in ROADMAP. |

### Non-goals

- Do not change the generated API docs system, the lexer generator, config parsers, or the keybinding format.
- Do not remove the `font_api` or `graphics_api` vtables.
- Do not remove the `custom_api` vtable.
- Do not add a plugin system.
- Do not add Wayland, XDG paths, warning cleanup, or CI. Those are other roadmap items.
- Do not change editor behavior that a user can see, except that the `-d` and `-D` options go away.

## 2. Repo context

- Path: `/home/samh/Work/5ed`. Feature branch: `feat/remove-the-so-split-roadmap-item-3-1007-1912`. Main branch: `master`.
- Stack: C++11 (compiled as C++11, do not raise it), CMake 3.20+, GCC only (GCC 16.2 on this machine), x86-64 Linux, X11/GLX, OpenGL 2.1, FreeType, Xfixes. No test framework.
- Style: unity builds. `function`, `internal`, `global` are macros for `static` (`src/base/5ed_base_types.h`). `api(x)` is an empty macro. A `.cpp` file in `src/` is often `#include`d by a target `.cpp`.
- One include root: `-I src`. Include paths look like `base/...`, `core/...`, `custom/...`, `platform/...`.
- Layer rule (checked by `scripts/check-structure.sh`): `base` includes only `base`; `custom` includes `base` and `custom`; `core` includes `base` and `core`; `platform` includes `base`, `core`, `platform`.
- Current build graph (`CMakeLists.txt`):
  - `5ed_base` STATIC library: 15 files in `src/base/*.cpp`, hidden visibility.
  - `custom_5ed` SHARED: `src/custom/5ed_default_bindings.cpp` + generated command metadata in `build/gen/generated/`.
  - `5ed_app` SHARED: `src/core/5ed_app_target.cpp`.
  - `5ed` executable: `src/platform/linux/linux_5ed.cpp`. It `dlopen`s `5ed_app.so` and `custom_5ed.so` in `main()`.
  - "Per-target" files compiled in several TUs (list in a CMake comment, section "Compiled once"):
    `src/base/per_target/{5ed_system_allocator,5ed_app_links_allocator,5ed_system_helpers,5ed_profile,5ed_profile_static_enable,5ed_search_list,5ed_command_map}.cpp`,
    `src/base/generated/{custom_api,system_api,font_api,graphics_api,lexer_cpp}.cpp`, `src/core/5ed_font_set.cpp`.
- The handshake today:
  - `src/custom/5ed_custom.cpp` defines `extern "C" get_version` and `init_apis`.
  - `src/base/5ed_types.h` has typedefs `_Get_Version_Type`, `_Init_APIs_Type`.
  - `src/core/5ed.h` has `struct Custom_API`, `Plat_Settings.custom_dll`, `Plat_Settings.custom_dll_is_strict`, and `App_Init_Sig` has the parameter `Custom_API api`.
  - `src/core/5ed.cpp`: `app_init` calls `api.init_apis(&custom_vtable, &system_vtable)`, then later calls `custom_init(&app)`. `init_command_line_settings` has `CLAct_CustomDLL` and the `-d`/`-D` options (and a `strict` variable).
  - `src/core/5ed_app_models.h`: enum `CLAct_CustomDLL`, field `Custom_API config_api` (used only at `5ed.cpp` `models->config_api = api;`).
- A test link (done by the planner on a scratch copy: core and custom as static libraries linked into `5ed`, `dlopen` replaced by direct calls) gives "multiple definition" errors ONLY for these non-static symbols:
  - `src/base/per_target/5ed_command_map.cpp` (all `mapping_*`, `map_*`, `Command_Binding::*`, `command_trigger_stringize*`),
  - `src/base/generated/lexer_cpp.cpp` (`cpp_main_keys_*`, `cpp_pp_keys_*`, `cpp_pp_directives_*`, ...),
  - `5ed_profile.cpp` (`Profile_Block::*`, `Profile_Scope_Block::*`, `profile_enable`, `profile_disable`, `profile_clear(Application_Links*)`),
  - `5ed_app_links_allocator.cpp` (`Scratch_Block::Scratch_Block(Application_Links*, ...)`),
  - `5ed_system_helpers.cpp` (`Mutex_Lock::*`).
  All other per-target code is file-static, so it is duplicated but does not clash.
- Install and build:
  ```
  cmake -S . -B build                                  # Debug (default)
  cmake -S . -B build-rel -DCMAKE_BUILD_TYPE=Release   # Release
  cmake --build build -j
  ```
- "Test" commands (no unit test framework exists; these CMake targets and scripts are the tests):
  - `cmake --build build --target check-structure` (builds `5ed` and all tools, then runs `scripts/check-structure.sh <src> <build>`).
  - `cmake --build build --target check-tools`
  - `cmake --build build --target check-project-file`
  - `cmake --build build --target check-security`
  - `scripts/test-check-structure.sh . build build-rel` (tests the check script; it appends lines to `src/custom/5ed_custom.cpp` in a temp copy, so keep that file).
- Run: `./build/5ed` (needs an X11 or XWayland display). There is NO display on the planner's machine (`DISPLAY` unset, no Xvfb, no xdotool). The implementer cannot run the GUI unless a display exists. Use `echo $DISPLAY` to check.
- Binary lookup: the editor finds fonts, themes, `config.5ed`, `bindings.5ed` next to the binary (`SystemPath_Binary`). CMake copies `ship_files/` to `build/`.
- Conventions: tabs are not used; 4 spaces. Comments in code use `// NOTE(...)` or plain text. Keep `// TOP` and `// BOTTOM` markers in source files. Keep file-top comment blocks.

## 3. Implementation steps

Do the steps in order. After EACH step the tree must configure and build (`cmake --build build -j`). Commit nothing (the supervisor commits).
Use a fresh build directory when a step changes the library list: `rm -rf build && cmake -S . -B build`.

### Step 0: Baseline numbers (scratch only, not in the repo)

1. Build Debug in `/tmp/5ed-base-debug` and Release in `/tmp/5ed-base-rel` from the unmodified tree (`git stash` is NOT needed; do this before any edit).
2. Record in `/tmp/5ed-baseline.txt`: wall time of a clean Release build (`time cmake --build /tmp/5ed-base-rel -j`), `size` of `5ed`, `5ed_app.so`, `custom_5ed.so`, and `ls -l` sizes. You use these in Step 10.
3. Run `cmake --build /tmp/5ed-base-debug --target check-structure` and confirm it passes (this is the baseline).

### Step 1: Remove `CLAct_CustomDLL` and the custom DLL settings

1. `src/core/5ed_app_models.h`: delete `CLAct_CustomDLL` from the `CLAct_*` enum. Delete the field `Custom_API config_api;` from `Models` (line ~67).
2. `src/core/5ed.cpp`, function `init_command_line_settings`: delete the `case 'd'` and `case 'D'` lines, the `case CLAct_CustomDLL:` block, and the `strict` variable (find its declaration near the top of the function; remove it only if nothing else uses it). The letters `-d` and `-D` then fall to the default `CLAct_Ignore`. Keep the `--custom` mode (`CLMode_Custom`); it passes arguments to the custom layer and is a different feature.
3. `src/core/5ed.h`: delete `char *custom_dll;` and `b8 custom_dll_is_strict;` from `Plat_Settings`.

### Step 2: Remove the version handshake and the dynamic loading

1. `src/base/5ed_types.h`: delete the two typedefs `_Get_Version_Type` and `_Init_APIs_Type` (and their `api(custom)` lines). Keep `Custom_Layer_Init_Type` and `void custom_layer_init(Application_Links *app);`. Add one declaration next to them, WITHOUT an `api(custom)` line:
   `void custom_layer_bind_api(struct API_VTable_custom *vtable);`
   Check `src/base/generated/custom_api*.h/.cpp` and `custom_api_master_list.h` do not mention `Get_Version` or `Init_APIs` (the planner found no match; confirm with `grep`). If they do, run `cmake --build build --target regen-custom-api` and review the diff.
2. `src/custom/5ed_custom.cpp`: delete `get_version` and `init_apis`. Add:
   ```
   void
   custom_layer_bind_api(API_VTable_custom *vtable){
       custom_api_read_vtable(vtable);
   }
   ```
   Keep the file (the script `scripts/test-check-structure.sh` edits it). Keep its `// TOP` and `// BOTTOM` markers.
3. `src/core/5ed.h`: delete `struct Custom_API`. Remove the `Custom_API api` parameter from `App_Init_Sig`. Change `App_Load_VTables` to take only `API_VTable_font *` and `API_VTable_graphics *` (the system vtable is removed in Step 6; do it now if you do Step 6 in the same edit, else keep the signature until Step 6).
4. `src/core/5ed.cpp`, `app_init`: delete `models->config_api = api;`. Replace the block that builds `custom_vtable`, `system_vtable`, and calls `api.init_apis(...)` with:
   ```
   API_VTable_custom custom_vtable = {};
   custom_api_fill_vtable(&custom_vtable);
   custom_layer_bind_api(&custom_vtable);
   ```
   (During Steps 2-5 the system vtable still exists: keep `system_api_fill_vtable` and `system_api_read_vtable` calls so that the custom TU still gets its system pointers. Do this by adding a second function in `5ed_custom.cpp`, `custom_layer_bind_api(API_VTable_custom*, API_VTable_system*)`, then remove the system part in Step 6. Choose the simplest path that keeps the build green.) At the `custom_init(&app);` call (line ~293) call `custom_layer_init(&app);` instead. Delete the `Assert(custom_init != 0)` line.
5. `src/platform/linux/linux_5ed.cpp`, function `main`:
   - Replace the block "NOTE(allen): load core" (the `System_Library core_library`, `search_list`, `def_search_get_full_path(... "5ed_app.so")`, `system_load_library`, `system_get_proc` code and the error boxes) with `App_Functions app = app_get_functions();`.
   - Delete the whole block "NOTE(allen): load custom layer" (all `custom_*` messages, `Custom_API custom`, `get_version`/`init_apis` lookups).
   - Change `app.init(&linuxvars.tctx, &render_target, base_ptr, curdir, custom);` to drop the last argument.
   - Declare `extern "C" App_Functions app_get_functions();` once in `src/core/5ed.h` (the definition in `5ed.cpp` is already `extern "C"`).
   - Delete `#define DLL "so"` at the top if nothing uses it (`grep -rn "DLL" src`).
   - Delete `#include "base/per_target/5ed_search_list.h"` and the `5ed_search_list.cpp` include from this file (platform does not need the search list any more). Delete the now-unused `System_Library core_library` variable.
   - Do NOT delete `system_load_library`, `system_get_proc`, `system_release_library` (decision D7).

### Step 3: New CMake graph (one executable)

Edit `CMakeLists.txt`:

1. Delete the `# per-target:` comment block and its text. Replace with a short comment that says that `5ed_base` is compiled once and is linked into the one executable.
2. Replace `add_library(custom_5ed SHARED ...)` with `add_library(5ed_custom OBJECT src/custom/5ed_default_bindings.cpp ${GENERATED_METADATA})`. Delete `PREFIX`/`SUFFIX` properties. Keep `target_include_directories(5ed_custom PRIVATE ${GENERATED_ROOT})` and `target_link_libraries(5ed_custom PRIVATE 5ed_common 5ed_base)`.
3. Replace `add_library(5ed_app SHARED ...)` with `add_library(5ed_core OBJECT src/core/5ed_app_target.cpp)`. Keep `target_link_libraries(5ed_core PRIVATE 5ed_common 5ed_base)`.
4. Link: `target_link_libraries(5ed PRIVATE 5ed_core 5ed_custom 5ed_common 5ed_base X11::X11 X11::Xfixes OpenGL::GL Freetype::Freetype ${CMAKE_DL_LIBS} m rt)`. Keep `-fno-threadsafe-statics` on the `5ed` target only. (The platform TU still needs `dl` for `libasound` in `src/platform/linux/linux_5ed_audio.cpp`; keep `${CMAKE_DL_LIBS}`.)
5. `add_dependencies(5ed 5ed_runtime_files)` only (remove `custom_5ed 5ed_app`).
6. `test_project_file` depends on `custom_5ed` for the generated command metadata. Create `add_custom_target(command_metadata DEPENDS ${GENERATED_METADATA})` after the `add_custom_command` that generates it and depend on `command_metadata` instead. Fix the comment.
7. Fix the comments at "Everything is placed beside the executable..." and the "Custom layer: custom_5ed.so" and "Core: 5ed_app.so" section headers. Remove `CMAKE_LIBRARY_OUTPUT_DIRECTORY` if no shared library is left (keep `CMAKE_RUNTIME_OUTPUT_DIRECTORY`).
8. Keep `CMAKE_POSITION_INDEPENDENT_CODE ON` for now; Step 10 may change it. If you remove it, say so in the ROADMAP notes.

At this point the link FAILS with multiple definitions (the list in section 2). Step 4 and Step 5 fix them.

### Step 4: Merge the API-free per-target files into `5ed_base` (compiled once)

1. Command map: `git mv src/base/per_target/5ed_command_map.cpp src/base/5ed_command_map.cpp`. Add includes at its top as the other `src/base/*.cpp` files do (look at `src/base/5ed_malloc_allocator.cpp` for the pattern; the file needs `base/5ed_base_types.h`, `base/5ed_types.h`, `base/5ed_table.h`, `base/5ed_events.h`, `base/5ed_command_map.h`, `base/5ed_mem.h` and what the compiler asks for). Add the file to the `5ed_base` source list in `CMakeLists.txt`. In `src/base/5ed_command_map.h` delete the comment "Each binary compiles its own copy..." and the `#pragma GCC visibility push(hidden)` and its matching `pop`. Remove the `#include "base/per_target/5ed_command_map.cpp"` lines from `src/core/5ed_app_target.cpp` and `src/custom/5ed_default_include.cpp`. Note: `mapping_init` calls `make_arena_system()`; Step 5 moves that function into `5ed_base` too. Until Step 5 is done the base archive member is not linked into tools (tools do not reference it), but the main exe needs both; do Step 4 and Step 5 in one build cycle.
2. Lexer: add `src/base/generated/lexer_cpp.cpp` to the `5ed_base` source list. Remove `#include "base/generated/lexer_cpp.cpp"` from `5ed_app_target.cpp` and `5ed_default_include.cpp`. Keep the `lexer_cpp.h` includes. The file is generated: do not edit it. If it needs extra includes, add them in a tiny wrapper or check how the unity TU provided them (it was included after all the base headers). If the file does not compile alone, create `src/base/5ed_lexer_cpp_unit.cpp` that includes the needed base headers and then `base/generated/lexer_cpp.cpp`, and list that file instead.
   Also check `tool_lexer_gen` and `regen-lexer`: they must still build (`cmake --build build --target check-tools`).

### Step 5: Merge the API-bound per-target files

These files call `system_*` or `custom_api` functions. Do Step 6 first for the `system_*` part if the build needs it, then finish here. The final state must have each file compiled in exactly ONE TU.

1. `5ed_profile_static_enable.cpp` holds only macros. `git mv` it to `src/base/5ed_profile_macros.h`. Add an include guard. Replace each `#include "base/per_target/5ed_profile_static_enable.cpp"` (in `5ed_app_target.cpp`, `5ed_default_include.cpp`) with `#include "base/5ed_profile_macros.h"` at the same place.
2. `5ed_profile.h`: `git mv src/base/per_target/5ed_profile.h src/base/5ed_profile.h`. Update all includes.
3. `5ed_profile.cpp`: it calls `system_mutex_make`, `system_now_time`, `system_thread_get_id` (system API) and, in the `Application_Links` variants, `get_thread_context` and `get_core_profile_list` (custom API, file-static in core). Split the file:
   - Move the three commands `profile_enable`, `profile_disable`, `profile_clear` (the `CUSTOM_COMMAND_SIG` blocks at the end) into `src/custom/5ed_profile_inspect.cpp` (this file is already in the custom TU, so the command metadata generator still finds them). They use `get_core_profile_list(app)`, which the custom TU has through the custom vtable.
   - Move the rest to `src/core/5ed_profile.cpp`, included ONCE, in `src/core/5ed_app_target.cpp`. Remove it from `5ed_default_include.cpp`.
   - Functions that the custom TU calls must have external linkage and a prototype in `src/base/5ed_profile.h`. Find them from the compiler errors in the custom TU ("was not declared" or "used but never defined"). Remove `function` (static) from those definitions. The class members `Profile_Block`, `Profile_Scope_Block` already have external linkage.
4. `5ed_app_links_allocator.cpp` (the `Scratch_Block(Application_Links*, ...)` constructors): `git mv` to `src/core/5ed_app_links_allocator.cpp`; include it ONCE in `5ed_app_target.cpp`; remove from `5ed_default_include.cpp`. The custom TU keeps the constructor declarations (they are in a header already) and links to the core definitions.
5. `5ed_system_helpers.cpp` (`Mutex_Lock`): `git mv` to `src/base/5ed_system_helpers.cpp`, add the include lines it needs, add it to the `5ed_base` source list, remove the includes from the target `.cpp` files. It calls `system_mutex_acquire` and `system_mutex_release` directly after Step 6.
6. `5ed_system_allocator.cpp` (`make_arena_system`, `get_base_allocator_system`): `git mv` to `src/base/5ed_system_allocator.cpp`; make its functions external (remove `internal`/`global` static where the platform, core, or custom TU uses them; keep the `global Base_Allocator base_allocator_system` as a file-static there); add prototypes in `src/base/5ed_malloc_allocator.h` (or the header that declares `make_arena_system` today; use `grep -n make_arena_system src/base/*.h`). Add it to the `5ed_base` source list. Remove the includes from `linux_5ed.cpp`, `5ed_app_target.cpp`, `5ed_default_include.cpp`. It needs `system_memory_allocate` and `system_memory_free` directly (Step 6).
   Important: the other tools link `5ed_base`. They must not pull this file in. A static archive member is used only when something references it. Run `cmake --build build --target check-tools` to confirm.
7. `5ed_search_list.{h,cpp}`: only the custom layer uses them after Step 2. `git mv` both to `src/custom/5ed_search_list.h` and `src/custom/5ed_search_list.cpp`. Fix the includes in `5ed_default_include.cpp`. Remove it from the other TUs.
8. `src/core/5ed_font_set.cpp`: include it ONCE, in `5ed_app_target.cpp` (already there). Remove the `#include "core/5ed_font_set.cpp"` from `linux_5ed.cpp`. The platform renderer (`src/platform/opengl/5ed_opengl_render.cpp`, line ~304) calls `font_set_face_from_id`. So: remove `internal` from the functions that the platform calls (only `font_set_face_from_id`; add others if the compiler asks) and add their prototypes to `src/core/5ed_font_set.h`. Keep the rest `internal`.
9. When Steps 4 and 5 are done: delete the empty directory `src/base/per_target/` (use `git mv`/`git rm`; it must not exist). Confirm: `ls src/base/per_target` fails.

### Step 6: Remove the `system_api` vtable (decision D3)

1. Generator change in `src/core/5ed_api_definition.cpp` and `src/core/5ed_api_definition.h`:
   - Add a new `API_Generation_Flag` value, for example `APIGeneration_NoVTable` (see the existing `APIGeneration_NoAPINameOnCallables` for the pattern).
   - In `generate_header`, when the flag is set: do not emit the `typedef ... _type` lines, `struct API_VTable_<name>`, or the `#if defined(STATIC_LINK_API)` / `DYNAMIC_LINK_API` guards. Emit only plain external declarations: `<ret> <name>(<params>);` (no `internal`). Keep the `<api>_<name>_sig()` macros (they are in a different part of the output; check `api_write_param_list` and the `.h` top).
   - In `generate_cpp`, when the flag is set: emit nothing, and do not create the `.cpp` file (see `api_definition_generate_api_includes`).
   - In `src/core/tools/5ed_system_api.cpp` (see `get_api_group` and how flags are passed in `5ed_api_definition_main.cpp`) pass the new flag for the `system` API only.
2. Regenerate: `cmake --build build --target regen-system-api`. Then `git rm src/base/generated/system_api.cpp` (it is empty now). Check `git diff src/base/generated/system_api.h`: only the vtable and guards go away; the `*_sig()` macros and the declarations stay (without `internal`). Do NOT run `regen-lexer`.
3. Remove every use of `API_VTable_system`, `system_api_fill_vtable`, `system_api_read_vtable`, and `DYNAMIC_LINK_API`/`STATIC_LINK_API` for system API headers:
   - `src/base/5ed_base.h` includes; `src/core/5ed_app_target.cpp`, `src/custom/5ed_default_include.cpp`, `src/platform/linux/linux_5ed.cpp` (each has `#define DYNAMIC_LINK_API` / `STATIC_LINK_API` before the include of `base/generated/system_api.h` and `.cpp`); `src/core/5ed.h` (`App_Load_VTables`); `src/core/5ed.cpp` (`app_load_vtables`, `app_init`); `src/custom/5ed_custom.cpp`; `src/platform/linux/linux_5ed.cpp` (`system_vtable`, `app.load_vtables(...)`).
   - Include `base/generated/system_api.h` once, plainly, in every TU that needs it. Drop the `#define ... _LINK_API` lines for system only. Keep them for `custom_api`, `font_api`, `graphics_api`.
4. Platform definitions: remove `internal`/`function` (static) from every `system_*` definition so it has external linkage. The definitions are in `src/platform/linux/linux_5ed_functions.cpp`, `src/platform/linux/linux_error_box.cpp`, and `src/platform/linux/linux_5ed.cpp` (find them with the list in `src/base/generated/system_api_master_list.h`; use compile and link errors as a guide). Do not touch `src/platform/unix/` (it is dead code behind `#error`).
5. Clean up the old interface: `5ed_system_types.h`, `5ed_base.h`, and any `System_Library`-based code that the removed lines used.
6. Build Debug and Release. Run `nm -C build/5ed | grep -E "system_api_(fill|read)_vtable|API_VTable_system"`: no output.

### Step 7: Scripts

Edit `scripts/check-structure.sh`:
1. Rule 6 (layer-direction): delete the `base/per_target/*` exception. Keep `base/generated/*`.
2. Rule 8 (define-once): loop only over `"$BUILD/5ed"`. Add checks that these symbols are defined exactly once in `"$BUILD/5ed"` (use `count_sym "$BUILD/5ed" TtWw "<demangled name>"`; take the exact demangled names from `nm -C build/5ed`): `mapping_init(Thread_Context*, Mapping*)`, `Mutex_Lock::Mutex_Lock(Plat_Handle)` (check the exact name: the type is `System_Mutex`), `Scratch_Block::Scratch_Block(Application_Links*)`, `make_arena_system()`, `Profile_Block::Profile_Block(Application_Links*, String_Const_u8, String_Const_u8)`, `font_set_face_from_id(Font_Set*, long)` (check the exact name).
3. Rule 10 (no-dynamic-export): delete it. Replace it with a new rule `no-shared-objects` with these checks:
   - `grep -rn "5ed_app\.so\|custom_5ed\.so\|custom_dll\|CLAct_CustomDLL\|get_version\|init_apis" src CMakeLists.txt` must give no output (README and ROADMAP are not scanned).
   - `[ ! -e src/base/per_target ]`.
   - `grep -rn "per-target" CMakeLists.txt src` gives no output.
   - `readelf -d "$BUILD/5ed"` must not show `NEEDED` for `5ed_app.so` or `custom_5ed.so`.
   - After `cmake --build`, `ls "$BUILD"/*.so` must not list `5ed_app.so` or `custom_5ed.so` (use a fresh build dir; remove the check if stale files could exist, or delete stale files in Step 9).
4. Keep the exit code rule (`exit "$fail"`).
Edit `scripts/test-check-structure.sh` only if a test breaks. Keep the final test that appends to `src/custom/5ed_custom.cpp`.

### Step 8: Docs

1. `README.md`:
   - "Build" section: say that the build writes ONE executable `build/5ed` together with the contents of `ship_files/`. Remove `5ed_app.so` and `custom_5ed.so`.
   - Replace the sentence "Release does not use link-time optimization..." with the result of Step 10.
   - Add a short section "Customising 5ed": edit the files in `src/custom/` and rebuild with `cmake --build build -j`. State that there is no plugin path and no custom library option. State that user config, bindings, and themes (`*.5ed` files) need no rebuild.
   - Mention that the command line options `-d` and `-D` are gone.
2. `ROADMAP.md`:
   - Header line "Done so far": add item 3.
   - Section "## 3. Remove the `.so` split": add " (done)" to the heading. Replace the bullets with a short list of what was done, and the decisions D1 to D7 (in STE). Include the IPO result with the measured numbers. Add the open points: `custom_api` vtable kept (item 4), `font_api`/`graphics_api` vtables kept, `system_load_library`/`system_get_proc`/`system_release_library` unused (item 4), IPO outcome (item 6 if warnings remain).
   - Keep items 1, 2 as they are, and keep the format of the other items.
3. `CMakeLists.txt` comments: make them match the new graph.

### Step 9: Clean build and the full check run

1. `rm -rf build && cmake -S . -B build && cmake --build build -j` (Debug).
2. `rm -rf build-rel && cmake -S . -B build-rel -DCMAKE_BUILD_TYPE=Release && cmake --build build-rel -j`.
3. Run all commands from section 2 under "Test commands" on both build dirs. All must pass. Remove `build-rel` at the end (it is under the repo root; `build/` is git-ignored but `build-rel` is not; either put it in `/tmp` or delete it).
4. The old `build/` directory in the repo has stale `5ed_app.so` and `custom_5ed.so`. A fresh `rm -rf build` removes them.

### Step 10: IPO/LTO trial for Release (try and measure)

1. In a scratch copy of the build (`/tmp/5ed-lto`), configure Release with IPO: add (in `CMakeLists.txt`) `include(CheckIPOSupported)`, `check_ipo_supported(RESULT FIVEED_IPO OUTPUT FIVEED_IPO_MSG)`, and for the targets `5ed`, `5ed_core`, `5ed_custom`, `5ed_base`: `set_property(TARGET <t> PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)`. Use `-DCMAKE_BUILD_TYPE=Release`. (Only Release; Debug stays without IPO.)
2. Build with `cmake --build /tmp/5ed-lto -j 2>&1 | tee /tmp/5ed-lto-build.log`. Count warnings: `grep -c warning /tmp/5ed-lto-build.log`. Save the text of the distinct warnings (the old problem: OpenGL function pointer type mismatch from `src/platform/opengl/5ed_opengl_funcs.h` and `GL_FUNC`).
3. Measure and compare with `/tmp/5ed-baseline.txt` (Step 0) and with a no-IPO Release build of the NEW tree: clean build wall time, `size build/5ed`, number of warnings. If a display exists, also measure the time from launch to the first frame (for example `perf stat`-free: run `./build/5ed -L` and read the log timestamps). If no display exists, say so in the notes. Do not invent runtime numbers.
4. Decision rule:
   - If the LTO build has ZERO new warnings and the binary starts (or, without display, links and `nm` shows `main`) then keep IPO ON for Release in `CMakeLists.txt`. Gate it with `check_ipo_supported`.
   - If warnings remain or the link fails: keep IPO OFF. Keep the existing short comment in CMake and README, and write the exact warning text and the measured numbers in ROADMAP item 3 and item 6.
5. Write the result (decision and numbers) in `README.md` and `ROADMAP.md` (Step 8).

### Step 11: Final self-check list for the implementer

- `grep -rn "dlopen\|dlsym" src` shows only `src/platform/linux/linux_5ed_audio.cpp`, `src/platform/linux/linux_5ed_functions.cpp` (`system_load_library`, `system_get_proc`), and `src/platform/unix/*` (dead code).
- `ls build` has `5ed` and no `*.so`.
- `git status` shows no stray files (`build-rel`, `/tmp` files are not in the repo).
- Do NOT commit. Do NOT push.

## 4. Tests to write

There is no unit test framework. The "tests" are the checks in `scripts/check-structure.sh` and CMake targets. Add or change these:

File `scripts/check-structure.sh` (new or changed rules; see Step 7):

| Case | Expected result |
|------|-----------------|
| Rule `no-shared-objects`, source scan for `5ed_app.so`, `custom_5ed.so`, `custom_dll`, `CLAct_CustomDLL`, `get_version`, `init_apis` in `src` and `CMakeLists.txt` | no hit, rule prints `PASS: no-shared-objects` |
| `readelf -d build/5ed` | no `NEEDED` entry for `5ed_app.so` or `custom_5ed.so` |
| `src/base/per_target` exists | rule fails with `FAIL:` line (negative case; check it by hand once with `mkdir src/base/per_target`, then remove it) |
| define-once count of `mapping_init`, `Mutex_Lock` ctor, `Scratch_Block(Application_Links*)`, `make_arena_system()`, `Profile_Block(Application_Links*,...)`, `font_set_face_from_id` in `build/5ed` | each exactly 1 |
| define-once of `i32_ceil32(float)` in `build/5ed` | exactly 1 |
| Layer direction after the moves (`src/base/5ed_profile.h`, `src/core/5ed_profile.cpp`, `src/custom/5ed_search_list.*`) | `PASS: layer-direction` |
| Rule `base-cpp-not-included` for the new base `.cpp` files | The new files `5ed_command_map.cpp`, `5ed_system_helpers.cpp`, `5ed_system_allocator.cpp` MUST be added to the `once` list in rule 7 so that nobody includes them. Result: `PASS` |

File `scripts/test-check-structure.sh`: no new cases are required. Run it on a Debug and a Release build dir. Expected: all `PASS`, exit 0.

Other checks that must still pass (they test that the build did not break):

- `cmake --build build --target check-tools` builds all 7 tool executables.
- `cmake --build build --target check-project-file` prints no `Project errors:` for the generated project, `project.5ed`, `ship_files/config.5ed` (the test includes the generated command metadata, so Step 3 item 6 matters).
- `cmake --build build --target check-security` passes.
- `cmake --build build --target regen-system-api` leaves `git diff src/base/generated/system_api.h` unchanged after a second run (the generator output is stable).

Edge cases to check by hand:

- A fresh build directory (no stale `.so`).
- Release build links with no "multiple definition" error.
- `./build/5ed -d foo.so` and `./build/5ed -D foo.so`: the options are ignored (no error box about a custom library). `foo.so` is not loaded and is not treated as a file name to open; this follows the existing `CLAct_Ignore` behavior for unknown options.

## 5. Acceptance criteria

- [ ] `cmake -S . -B build && cmake --build build -j` builds with no error and no new warning.
- [ ] `ls build` shows the executable `5ed` and NO `5ed_app.so` and NO `custom_5ed.so`.
- [ ] `./build/5ed` (with a display) opens the editor window, loads fonts and the default key bindings, and shows the `*scratch*` buffer. No error box about a missing library appears.
- [ ] Moving or deleting `build/*.so` has no effect on the editor, because no such files exist.
- [ ] The key commands work as before: open a file (`Ctrl+o`), type text, save (`Ctrl+s`), `Alt+x` shows the command lister with the command list (about 266 commands), `Alt+x` then `profile_enable` is found.
- [ ] `./build/5ed -d x.so` starts normally and does not look for `x.so`.
- [ ] The editor still reads `config.5ed`, `bindings.5ed` and themes (from the project folder, `~/.5ed/`, and the binary folder).
- [ ] `grep -rn "CLAct_CustomDLL\|get_version\|init_apis\|custom_dll" src` gives no output.
- [ ] `src/base/per_target/` does not exist. The words "per-target" do not appear in `CMakeLists.txt`.
- [ ] `cmake --build build --target check-structure` prints only `PASS:` lines.
- [ ] `scripts/test-check-structure.sh . build build-rel` prints only `PASS:` lines.
- [ ] `check-tools`, `check-project-file`, and `check-security` pass.
- [ ] `README.md` says that the build makes ONE executable and has a "Customising 5ed" section that says: edit `src/custom/` and rebuild, no plugin path.
- [ ] `ROADMAP.md` marks item 3 as done and lists the decisions D1 to D7 and the IPO result with numbers.
- [ ] The IPO trial is done. Either IPO is ON for Release with zero new warnings, or it is OFF and the warning text is in `ROADMAP.md`.

## 6. Manual check script

A display is needed for the GUI part. Check with `echo $DISPLAY`. If it is empty, run only parts A and B and report that part C was not run.

### A. Build and structure (no display needed)

```
cd /home/samh/Work/5ed
rm -rf build
cmake -S . -B build
cmake --build build -j
ls build
```
Expected: `build` lists `5ed`, `bindings.5ed`, `config.5ed`, `fonts`, `themes`, `gen`, `lib5ed_base.a`, `metadata_generator`. It lists no `5ed_app.so` and no `custom_5ed.so`.

```
readelf -d build/5ed | grep NEEDED
nm -C build/5ed | grep -cE " (T|t) (app_get_functions|custom_layer_init|custom_layer_bind_api)"
nm -C build/5ed | grep -E "get_version|init_apis|system_api_(fill|read)_vtable"
```
Expected: `NEEDED` lines list only system libraries (`libX11`, `libGL`, `libfreetype`, `libc`, ...), no `5ed_app.so`, no `custom_5ed.so`. The count is at least 3. The last command prints nothing.

```
cmake --build build --target check-structure
cmake --build build --target check-tools
cmake --build build --target check-project-file
cmake --build build --target check-security
```
Expected: each command ends with exit code 0. `check-structure` prints only lines that start with `PASS:`. Use `echo $?` after each.

```
rm -rf build-rel
cmake -S . -B build-rel -DCMAKE_BUILD_TYPE=Release
cmake --build build-rel -j
scripts/test-check-structure.sh . build build-rel
rm -rf build-rel
```
Expected: all lines `PASS:`, exit code 0.

```
cd /tmp && DISPLAY= /home/samh/Work/5ed/build/5ed ; echo "exit=$?"
```
Expected without a display: the program prints an X error or opens an error box and exits; it must NOT print "Could not load '5ed_app.so'" or "Did not find a library for the custom layer". (If the message "Could not load" appears, the old code still exists.)

### B. Source checks

```
cd /home/samh/Work/5ed
grep -rn "CLAct_CustomDLL\|get_version\|init_apis\|custom_dll\|5ed_app\.so\|custom_5ed\.so" src CMakeLists.txt
ls src/base/per_target
grep -n "5ed_app.so\|custom_5ed.so" README.md ROADMAP.md
```
Expected: the first command prints nothing. The second prints `No such file or directory`. The third prints nothing from the README. ROADMAP may name the files only in the "done" text of item 3 (it must say they were removed).

### C. GUI check (display needed, X11 or XWayland)

```
cd /home/samh/Work/5ed
./build/5ed
```
Do these actions in the window and expect these results:
1. The window opens, dark theme, with a `*scratch*` buffer. (Expect: no error box.)
2. Press `Alt+x`, type `profile_enable`, press Enter. Expect: the command runs and the lister closes (no "command not found" message in the bottom bar).
3. Press `Ctrl+o`, type `ROADMAP.md`, press Enter. Expect: the file opens and shows the text `# Roadmap`.
4. Press `Alt+x`, type `load_project`, press Enter. Expect: `project.5ed` loads (the project name `5ed` appears), no `Project errors:` text in `*messages*`.
5. Press `Ctrl+q` (or close the window). Expect: the editor exits with code 0. Check `echo $?`.

```
cd /tmp && /home/samh/Work/5ed/build/5ed -d foo.so /home/samh/Work/5ed/README.md
```
Expected: the editor opens. It does not report a missing library. `foo.so` is not loaded.

### D. IPO numbers (record in README and ROADMAP)

```
cd /home/samh/Work/5ed
rm -rf /tmp/5ed-noipo && cmake -S . -B /tmp/5ed-noipo -DCMAKE_BUILD_TYPE=Release && ( time cmake --build /tmp/5ed-noipo -j ) 2>&1 | tail -5
size /tmp/5ed-noipo/5ed
```
Compare with the same commands on the build that has IPO on (Step 10). Report: build time, `size` output, and the warning count for both.

## 7. Risks and open assumptions

- [INFERENCE] The core and custom TUs have separate file-static state. The old `.so` files had the same split, so behavior stays the same. If a `global` variable in a header is used by both TUs and it must be shared, a bug can appear only at run time. Check with the GUI check (part C).
- `custom_api` pointer functions in the custom TU still depend on the vtable that `app_init` fills. The order in `app_init` must be: fill custom vtable, `custom_layer_bind_api`, then `custom_layer_init`. A wrong order gives a crash at the first custom API call.
- `font_set.cpp` in the core TU: if the platform TU needs more than `font_set_face_from_id`, the link fails. The linker names the missing symbol. Remove `internal` for that function and add a prototype.
- A static archive (`lib5ed_base.a`) member is linked only when referenced. New base files that need `system_*` (system allocator, `Mutex_Lock`) must not be pulled into the tool executables (`tool_*`, `metadata_generator`). If a tool fails to link with `undefined reference to system_...`, a tool references one of these files. Fix the reference; do not add stubs to the tools.
- The generator change in Step 6 touches `src/core/5ed_api_definition.cpp`, which `tool_system_api`, `tool_font_api`, `tool_graphics_api`, `tool_api_parser`, `tool_api_docs` also compile. The flag must default to the old behavior for the other APIs. Verify with `git diff src/base/generated/` after `cmake --build build --target regen`: only `system_api.h` (and the removed `system_api.cpp`) may change. `regen-lexer` must NOT be run (known differing output).
- `check-api-docs` reports 6 API functions with no docs today. This is not part of this item. Do not fix it.
- IPO may produce OpenGL function pointer warnings again (known from item 1 and 2). The decision rule in Step 10 covers this. A result of "stays off" is a valid result.
- Runtime performance of IPO cannot be measured on the planner's machine (no display). The implementer must say so if the same holds. Do not state a runtime number that was not measured.
- Assumption: the `-d` and `-D` options have no user outside this repo. `CLAct_Ignore` handles them.
- Assumption: the `unix/` platform files are dead code behind `#error`. Do not edit them.
- Assumption: `scripts/test-check-structure.sh` needs the file `src/custom/5ed_custom.cpp`. If you delete that file, update the test.
- Open point for the supervisor: the plan keeps the `font_api` and `graphics_api` vtables (D4). If the user wants all vtables removed in this item, the plan grows by about one more generator flag use and the definitions in `src/platform/opengl/` and `src/platform/5ed_font_provider_freetype.cpp`.
- Open point for the supervisor: the plan keeps `custom_api` (D2). Removing it needs a change of the linkage of ~180 `api(custom) function` definitions and a change in `5ed_api_parser`.
