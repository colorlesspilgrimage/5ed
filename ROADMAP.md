# Roadmap

Status: 5ed 0.1.0 builds and runs on x86-64 Linux (X11/GLX, GCC, CMake). Done so far: upstream import, Linux-only, rebrand, CMake port, Linux-only cleanup (item 1), base layer and source layout (item 2). See `git log`.

Items are roughly in the order intended. Each should leave the tree building and the editor launching.

## 1. Finish the Linux-only cleanup (done)

## 2. Unify the base layer (done)


## 3. Remove the `.so` split

- `5ed_app.so` and `custom_5ed.so` are each `dlopen`ed once at startup and never unloaded or reloaded (`platform_linux/linux_5ed.cpp`), so there is no hot reload to preserve.
- The files listed in `CMakeLists.txt` as per-target stay compiled once per binary. Merge them here.
- Link core and custom layer into the main executable. Keep the `custom_api` / `system_api` vtable boundary at first and remove it afterwards if nothing needs it.
- Remove the custom-DLL command line option (`CLAct_CustomDLL` in `5ed.cpp`) and the version handshake (`get_version`, `init_apis`).
- Decide on the user customisation story: edit `custom/` and rebuild, or keep a plugin path.
- Once core and custom are one target, try IPO/LTO for Release again and measure it. It is off now because it triggered OpenGL function-pointer warnings, so `lib5ed_base.a` helpers are not inlined across the library. If the warnings remain, finish it in item 6.

## 4. Simplify generated code

- Command metadata: replace the preprocess-and-parse step (`5ed_metadata_generator.cpp`) with something simpler, for example an explicit registration table.
- API generators (`5ed_api_definition*.cpp`, `5ed_api_parser*.cpp`, `5ed_system_api.cpp`, `5ed_font_api.cpp`, `5ed_graphics_api.cpp`): decide whether the vtable APIs still need generating once item 3 lands.
- Lexer generator (`custom/lexer_generator`, 4k lines): keep as is, replace, or hand-write the C++ lexer. Find out why regenerated `lexer_cpp.cpp` differs from the checked-in one before trusting `regen-lexer`.
- Docs system (`docs/`, `5ed_doc_*`, `check-api-docs`): compiled into the core today. Decide whether to keep it.
- `check-api-docs` reports 6 API functions with no documentation. Document them, or drop the check if the docs system is removed.

## 5. Configuration

- Collapse the layers of config parsers (`5ed_config.cpp`, the `.5ed` bindings/theme format, `project.5ed`, `5ed_config_grammar.txt`) into one.
- Use XDG paths (`~/.config/5ed`) instead of `~/.5ed`, and `$XDG_DATA_HOME` for data.
- Project generation (`setup_new_project`, `setup_build_sh`, `prj_generate_project`): quote or escape user-typed text written to `build.sh` and `project.5ed`. It is written raw today (same as upstream).
- Decide the overwrite rule for `setup_new_project`. Files are now created with `O_EXCL|O_NOFOLLOW`, so an existing `build.sh` or `project.5ed` is left untouched and a "could not create" message is printed. Add an explicit confirm-and-overwrite path, or keep and document the current rule.
- Decide whether to keep the `fkey_command` block fix (`,` instead of `;` for `.F1`/`.F2`). It changes the generated `project.5ed` and goes past the Linux-only cleanup. It also fixes the `Project errors:` that `load_project` reported on every generated project.

## 6. Platform and rendering

- Wayland-native window (currently X11 via XWayland). Needs an alternative to GLX and XIM.
- Replace the OpenGL 2.1 compatibility context and `glext` function loading.
- When `glext` loading is replaced, remove the OpenGL function-pointer warnings so IPO can be on for Release (see item 3).
- Remove unused system dependencies (`fontconfig` already dropped from the link line).
- Audio (`platform_linux/linux_5ed_audio.cpp`, ALSA via `dlopen`): keep or remove.

## 7. Quality

- Make the build warning-clean under `-Wall -Wextra` (the build currently suppresses several warnings).
- Run with ASan/UBSan and fix findings.
- Add a small regression suite that exercises buffers, undo and the lexer without a window.
- Add a GUI smoke test that drives the real window (key injection under Xvfb or a similar tool). Steps D and E of the item 1/2 plan were not run: `Alt+x setup_new_project` in an empty folder, then `load_project` on this repo. Until this exists, check them by hand.
- Add `install()` rules and a desktop file once the binary layout settles.
- CI: build Debug and Release with the current GCC.

## 8. Housekeeping

- Trim `ship_files/themes` and `UPSTREAM_CHANGES.txt` as wanted.
- Replace the tutorial content (`5ed_tutorial.cpp`, still the Handmade Seattle demo) or remove it.
- Rename `FCODER_*` include guards.
- Add a copyright line for 5ed contributors to `LICENSE` if wanted.
