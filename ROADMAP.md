# Roadmap

Status: 5ed 0.1.0 builds and runs on x86-64 Linux (X11/GLX, GCC, CMake). Done so far: upstream import, Linux-only, rebrand, CMake port, Linux-only cleanup (item 1), base layer and source layout (item 2), one executable (item 3). See `git log`.

Items are roughly in the order intended. Each should leave the tree building and the editor launching.

## 1. Finish the Linux-only cleanup (done)

## 2. Unify the base layer (done)


## 3. Remove the `.so` split (done)

- The build writes one executable, `build/5ed`. It does not write `5ed_app.so` or `custom_5ed.so`.
- Core and custom stay two translation units. CMake links both object libraries into `5ed`.
- The `custom_api` vtable stays. Item 4 decides about the API generators.
- The `system_api` vtable is gone. Platform functions have external linkage. Callers use them directly.
- The `font_api` and `graphics_api` vtables stay. Item 4 or item 6 may remove them.
- Customisation is an edit of `src/custom/` and a rebuild. There is no plugin path.
- `app_get_functions` stays. The platform calls it directly.
- `system_load_library`, `system_get_proc`, and `system_release_library` stay unused. Item 4 decides.
- IPO stays off. The Release IPO link printed 57 warnings and still produced `main`.
- Each warning has this form: `warning: type of symbol 'glAttachShader' changed from 2 to 1`.
- The other symbols are the `gl*` names loaded by `GL_FUNC` in `src/platform/opengl/5ed_opengl_funcs.h`.
- Old Release build time was 22.2 s. The three files were 1524888, 2938320 and 3869152 bytes.
- New Release build time with no IPO was 20.1 s. `build/5ed` was 5586040 bytes. `size` text was 1171119.
- Release IPO build time was 11.5 s. The file was 5923504 bytes. `size` text was 1072402. Warnings: 57.
- No display was available, so launch time was not measured.
- `CMAKE_POSITION_INDEPENDENT_CODE` stays on.

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
- When `glext` loading is replaced, remove the OpenGL function-pointer warnings so IPO can be on for Release (see item 3). The 2026-10-07 trial kept IPO off. The link printed 57 warnings. Each warning has this form: `warning: type of symbol 'glAttachShader' changed from 2 to 1`. The names are the `GL_FUNC` symbols in `src/platform/opengl/5ed_opengl_funcs.h`.
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

## 9. Open an OMP session from a hotkey

- Support binding a hotkey that opens an OMP session in a pane.
- The hotkey is user-bindable with the other key bindings (`.5ed` bindings, `ship_files/bindings.5ed`).
- Pressing the bound hotkey opens the OMP session in a pane.
