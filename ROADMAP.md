# Roadmap

Status: 5ed 0.1.0 builds and runs on x86-64 Linux (X11/GLX, GCC, CMake). Done so far: upstream import, Linux-only, rebrand, CMake port. See `git log`.

Items are roughly in the order intended. Each should leave the tree building and the editor launching.

## 1. Finish the Linux-only cleanup

- Remove the remaining OS and compiler detection for Windows, Mac and Clang in `custom/5ed_base_types.h`, and the branches that depend on it (`OS_WINDOWS`, `OS_MAC`, `ARCH_X86`, `COMPILER_CL`).
- Remove the `OS_LINUX` conditionals once nothing else is a target.
- Drop dead config: `default_compiler_bat`, `default_flags_bat`, `.bat` build-script generation in `setup_build_bat*`, the `.win` / `.mac` keys accepted by the project parser, and `ship_files` text that mentions them.
- Remove `4ed_data.ctm` handling if unused, and other leftovers from the demo/super tiers (`FRED_INTERNAL` is the only flag still used).

## 2. Unify the base layer

- `5ed_base_types.cpp` (7.3k lines) is compiled separately into the platform layer, the core and the custom layer. Define it once.
- Resolve the double definitions between core and custom (upstream's note 5) and the collision-avoidance names introduced by the rebrand (`5ed_stdio_file`, `5ed_layout_lookup`, `5ed_log_helpers`).
- Move to a layout without the flat `-I . -I custom` include namespace: `src/base`, `src/core`, `src/custom`, `src/platform`.

## 3. Remove the `.so` split

- `5ed_app.so` and `custom_5ed.so` are each `dlopen`ed once at startup and never unloaded or reloaded (`platform_linux/linux_5ed.cpp`), so there is no hot reload to preserve.
- Link core and custom layer into the main executable. Keep the `custom_api` / `system_api` vtable boundary at first and remove it afterwards if nothing needs it.
- Remove the custom-DLL command line option (`CLAct_CustomDLL` in `5ed.cpp`) and the version handshake (`get_version`, `init_apis`).
- Decide on the user customisation story: edit `custom/` and rebuild, or keep a plugin path.

## 4. Simplify generated code

- Command metadata: replace the preprocess-and-parse step (`5ed_metadata_generator.cpp`) with something simpler, for example an explicit registration table.
- API generators (`5ed_api_definition*.cpp`, `5ed_api_parser*.cpp`, `5ed_system_api.cpp`, `5ed_font_api.cpp`, `5ed_graphics_api.cpp`): decide whether the vtable APIs still need generating once item 3 lands.
- Lexer generator (`custom/lexer_generator`, 4k lines): keep as is, replace, or hand-write the C++ lexer. Find out why regenerated `lexer_cpp.cpp` differs from the checked-in one before trusting `regen-lexer`.
- Docs system (`docs/`, `5ed_doc_*`, `check-api-docs`): compiled into the core today. Decide whether to keep it.

## 5. Configuration

- Collapse the layers of config parsers (`5ed_config.cpp`, the `.5ed` bindings/theme format, `project.5ed`, `5ed_config_grammar.txt`) into one.
- Use XDG paths (`~/.config/5ed`) instead of `~/.5ed`, and `$XDG_DATA_HOME` for data.

## 6. Platform and rendering

- Wayland-native window (currently X11 via XWayland). Needs an alternative to GLX and XIM.
- Replace the OpenGL 2.1 compatibility context and `glext` function loading.
- Remove unused system dependencies (`fontconfig` already dropped from the link line).
- Audio (`platform_linux/linux_5ed_audio.cpp`, ALSA via `dlopen`): keep or remove.

## 7. Quality

- Make the build warning-clean under `-Wall -Wextra` (the build currently suppresses several warnings).
- Run with ASan/UBSan and fix findings.
- Add a small regression suite that exercises buffers, undo and the lexer without a window.
- Add `install()` rules and a desktop file once the binary layout settles.
- CI: build Debug and Release with the current GCC.

## 8. Housekeeping

- Trim `ship_files/themes` and `UPSTREAM_CHANGES.txt` as wanted.
- Replace the tutorial content (`5ed_tutorial.cpp`, still the Handmade Seattle demo) or remove it.
- Rename `FCODER_*` include guards.
- Add a copyright line for 5ed contributors to `LICENSE` if wanted.
