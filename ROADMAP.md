# Roadmap

Status: 5ed 0.1.0 builds and runs on x86-64 Linux (X11/GLX, GCC, CMake). Done so far: upstream import, Linux-only, rebrand, CMake port, Linux-only cleanup (item 1), base layer and source layout (item 2), one executable (item 3), generated code (item 4), config (item 5). See `git log`.

Items are roughly in the order intended. Each should leave the tree building and the editor launching.

## 1. Finish the Linux-only cleanup (done)

## 2. Unify the base layer (done)


## 3. Remove the `.so` split (done)

- The build writes one executable, `build/5ed`. It does not write `5ed_app.so` or `custom_5ed.so`.
- Core and custom stay two translation units. CMake links both object libraries into `5ed`.
- The `custom_api` vtable stays. Item 4 decides about the API generators. (removed in item 4)
- The `system_api` vtable is gone. Platform functions have external linkage. Callers use them directly.
- The `font_api` and `graphics_api` vtables stay. Item 4 or item 6 may remove them. (removed in item 4)
- Customisation is an edit of `src/custom/` and a rebuild. There is no plugin path.
- `app_get_functions` stays. The platform calls it directly.
- `system_load_library`, `system_get_proc`, and `system_release_library` stay unused. Item 4 decides. (removed in item 4)
- IPO stays off. The Release IPO link printed 57 warnings and still produced `main`.
- Each warning has this form: `warning: type of symbol 'glAttachShader' changed from 2 to 1`.
- The other symbols are the `gl*` names loaded by `GL_FUNC` in `src/platform/opengl/5ed_opengl_funcs.h`.
- Old Release build time was 22.2 s. The three files were 1524888, 2938320 and 3869152 bytes.
- New Release build time with no IPO was 20.1 s. `build/5ed` was 5586040 bytes. `size` text was 1171119.
- Release IPO build time was 11.5 s. The file was 5923504 bytes. `size` text was 1072402. Warnings: 57.
- No display was available, so launch time was not measured.
- `CMAKE_POSITION_INDEPENDENT_CODE` stays on.

## 4. Simplify generated code (done)

- Command metadata is a checked-in list, `src/custom/5ed_command_list.h`, with 264 commands.
- No build step makes the list.
- `scripts/check-commands.sh` (`check-commands`) checks the list against the command definitions.
- `CUSTOM_DOC` and `CUSTOM_ID` are gone.
- Managed IDs are in `src/custom/5ed_managed_ids.h`.
- The API generators are removed.
- The vtable APIs did not need generating once item 3 landed.
- Core defines the custom API with external linkage.
- The declarations are in `src/base/5ed_custom_api.h`.
- System functions are in `src/base/5ed_system_api.h`.
- `font_make_face` and the two `graphics_*` functions are declared in `src/core/5ed_font_interface.h`.
- The platform defines those three functions.
- `app_get_functions` has no `load_vtables`.
- `system_load_library`, `system_get_proc` and `system_release_library` are removed.
- Item 3 left those three unused.
- The lexer generator stays as a developer tool (`regen-lexer`).
- `main()` seeded the random generator with `time(0)`.
- The keyword table layout changed on every run.
- The checked-in header had a hand edit (`static` on `token_cpp_kind_names`).
- The seed is fixed and the generator prints `static`.
- The checked-in output is regenerated.
- `regen-lexer` now gives no diff.
- The docs system is removed (`src/core/docs/`, `5ed_doc_*`, `check-api-docs`).
- It was 4000+ lines of Custom API text for a plugin path that no longer exists.
- It served two commands only (`custom_api_documentation`, `command_documentation`).
- It was out of date (6 missing items, many dead-call warnings).
- Command descriptions stay in the command list and show in the `command_lister`.
- A user `bindings.5ed` that names the two removed commands shows an unknown-command error.
- `check-api-docs` is dropped with the docs system.
- The 6 undocumented functions are no longer a finding.
- `git diff --shortstat master` shows 86 files changed, 2694 insertions, 13840 deletions.
- Files in `src/base/generated/` went from 18 to 3.
- The command-metadata generator run is gone.
- A clean Debug configure and build took 10.2 s before and 6.4 s after.
- Debug `build/5ed` was 3553776 bytes before and 3087584 bytes after.

## 5. Configuration (done)

- One parser path: `def_config_from_text`.
- `def_config_parse` is gone.
- The v1 project reader is gone.
- The removed files are `5ed_prj_v1.cpp`, `5ed_prj_v1.h` and `5ed_config_grammar.txt`.
- Those three files are 500 lines removed.
- This fork has no v1 project files and never wrote them.
- A project with `version(0)`, `version(1)` or no version prints an error.
- That project loads nothing. It needs `version(2);`.
- Unused typed accessors are removed.
- The theme loader and the bindings loader keep the accessors they call.
- The grammar is in `src/custom/5ed_config.h`.
- `config.5ed`, `bindings.5ed`, themes and `project.5ed` use that grammar.
- Config directory: `$XDG_CONFIG_HOME/5ed` when that value starts with `/`.
- Otherwise the config directory is `$HOME/.config/5ed`.
- Data directory: `$XDG_DATA_HOME/5ed` when that value starts with `/`.
- Otherwise the data directory is `$HOME/.local/share/5ed`.
- Search order: project dir, config dir, data dir, binary dir.
- 5ed does not read `~/.5ed/`. Move files from that folder.
- `-U <dir>` replaces the config directory only.
- Typed text in `build.sh` is wrapped in POSIX single quotes.
- A `'` in typed text becomes `'\''`.
- `"$code"` and `"$PWD"` are quoted.
- `build.sh` uses `cd --`. An output dir that starts with `-` is not a `cd` option.
- `.5ed` string values escape `\`, `"`, newline, tab and NUL.
- `project_reprint` uses the same escapes. A value then parses back to the same bytes.
- A shell command is shell-quoted, then escaped for the string literal.
- A control character is refused. No file is written.
- A script name that holds `/` is refused.
- `default_flags_sh` and `default_compiler_sh` stay raw.
- They come from `config.5ed`. They are not typed text.
- An existing `build.sh` or `project.5ed` is not changed.
- The message names the file.
- It says to delete the file and run the command again.
- Reason: the hot directory can be an untrusted checkout.
- `O_EXCL|O_NOFOLLOW` is the guard that `check-security` tests.
- `fkey_command` keeps `,` between `.F1` and `.F2`.
- Reason: the grammar needs `,` between compound elements.
- With `;`, every generated project printed `Project errors:`.
- `git diff --shortstat` shows 19 files changed, 460 insertions, 914 deletions.

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
