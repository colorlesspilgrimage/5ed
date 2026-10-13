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
- 5ed uses `$HOME` only when it starts with `/`.
- Search order: project dir, config dir, data dir, binary dir.
- 5ed does not read `~/.5ed/`. Move files from that folder.
- `-U <dir>` replaces the config directory only. 5ed adds a `/` at the end.
- Typed text in `build.sh` is wrapped in POSIX single quotes.
- A `'` in typed text becomes `'\''`.
- `"$code"` and `"$PWD"` are quoted.
- `build.sh` uses `cd --`. An output dir that starts with `-` is not a `cd` option.
- `.5ed` string values escape `\`, `"`, newline, tab and NUL.
- `project_reprint` uses the same escapes. A value then parses back to the same bytes.
- A shell command is shell-quoted, then escaped for the string literal.
- A control character is refused. No file is written.
- A script name that holds `/` is refused.
- `build.sh` uses `cd --`. An output dir `-` becomes `./-`.
- A compound can be 64 levels deep. A deeper compound is a parse error.
- 5ed loads at most 65536 values from one file.
- These limits stop a hostile `project.5ed` (for example `a = { a };`).
- Before, such a file made 5ed crash with a stack overflow.
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
- `git diff --shortstat` shows 19 files changed, 650 insertions, 919 deletions.

## 6. Platform and rendering

- Wayland-native window (currently X11 via XWayland). Needs an alternative to GLX and XIM.
- The context is OpenGL 3.3 core. The attribute list is in `src/platform/opengl/5ed_opengl_context.h`.
- The loader is `src/platform/opengl/5ed_opengl_load.h`. Each pointer name starts with `fiveed_`.
- The 57 link warnings are gone. Release uses IPO.
- The window path is still X11 and GLX.
- Wayland, dependency trimming, and audio are unchanged.
- Remove unused system dependencies (`fontconfig` already dropped from the link line).
- Audio: remove it (item 7).

## 7. Remove audio

- Remove all audio code. No ALSA. No `dlopen` of `libasound.so.2`. No audio API.
- Normal editing writes silence. It never queues a clip.
- `music_start` and `hit_sfx` are the only calls to `def_audio_play_clip`.
- They are not bound. The command lister can still run them.
- They open `audio_test/chtulthu.wav` and `audio_test/hit.wav`. Those files are not in the tree.
- `music_stop` calls `def_audio_stop`. It is not bound.
- `src/custom/5ed_tutorial.cpp` has no audio calls. There is no bell.
- Core has no audio calls.
- Startup launches the audio thread from `main` in `src/platform/linux/linux_5ed.cpp`.
- The call is `system_thread_launch(&linux_audio_main, NULL)`. It runs before `app.init`.
- `linux_audio_main` in `src/platform/linux/linux_5ed_audio.cpp` loads `libasound.so.2`.
- It opens the PCM name `"default"`.
- `snd_pcm.hw_params_any` is the first call into `snd_pcm_hw_refine` inside libasound.
- `snd_pcm.hw_params` calls `snd_pcm_hw_refine` again.
- `chk` prints a negative code and continues. A crash on that thread kills the process.
- A null ALSA default device lets refine succeed. Any other default device hit this crash in an earlier test.
- Delete `src/platform/linux/linux_5ed_audio.cpp`.
- Delete `src/platform/linux/alsa_funcs.txt`.
- Drop `#include "platform/linux/linux_5ed_audio.cpp"` from `src/platform/linux/linux_5ed.cpp`.
- Drop `#include <dlfcn.h>` from that file. The audio thread is the only `dlopen` in the build.
- Drop `${CMAKE_DL_LIBS}` from the `5ed` link line in `CMakeLists.txt`.
- There is no `-lasound` link. `alsa/asoundlib.h` is included only by the audio file.
- In `Linux_Vars`, delete `audio_mutex`, `audio_cond`, `audio_ctx`, `audio_src_func`, `audio_dst_func` and `audio_thread`.
- In `main`, delete the `audio_mutex` init, the `audio_cond` init, and the `linux_audio_main` launch.
- `audio_cond` is never waited or signaled.
- Delete `linux_audio_main`, `linux_submit_audio`, `linux_default_mix_sources` and `linux_default_mix_destination` with the audio file.
- Delete `system_set_source_mixer` and `system_set_destination_mixer` from `src/platform/linux/linux_5ed_functions.cpp`.
- Delete the same two stubs from `src/custom/tests/5ed_project_file_stubs.cpp`.
- Delete their declarations from `src/base/5ed_system_api.h`.
- Delete `Audio_Mix_Sources_Function` and `Audio_Mix_Destination_Function` from `src/base/5ed_types.h`.
- Delete `src/custom/5ed_audio.h` and `src/custom/5ed_audio.cpp`.
- Drop `#include "custom/5ed_audio.h"` and `#include "custom/5ed_audio.cpp"` from `src/custom/5ed_default_include.cpp`.
- In `default_startup` (`src/custom/5ed_default_hooks.cpp`), delete the `def_audio_init()` call.
- `default_startup` is bound to `CoreCode_Startup` in `setup_essential_mapping`.
- `def_audio_init` only calls the two mixer setters. It does not play a clip.
- Delete these with the custom audio files: `def_audio_init`, `def_audio_play_clip`, `def_audio_is_playing`, `def_audio_stop`, `def_audio_mix_sources`, `def_audio_mix_destination`, `def_audio_begin_ticket_mutex`, `def_audio_end_ticket_mutex`, `audio_clip_from_wav_data`, `audio_clip_from_wav_FILE`, `audio_clip_from_wav_file_name`.
- `audio_clip_from_wav_file_name` has no caller.
- Delete `Audio_Control`, `Audio_Clip`, `Audio_System` and `def_audio_system`.
- In `src/custom/5ed_examples.cpp`, delete `the_music_control`, `music_start`, `music_stop` and `hit_sfx`.
- The other example commands in that file stay.
- Drop the three `COMMAND` lines from `src/custom/5ed_command_list.h` (`hit_sfx`, `music_start`, `music_stop`).
- `scripts/check-commands.sh` checks that list against the command definitions.

## 8. Quality

- Make the build warning-clean under `-Wall -Wextra` (the build currently suppresses several warnings).
- Run with ASan/UBSan and fix findings.
- Add a small regression suite that exercises buffers, undo and the lexer without a window.
- Add a GUI smoke test that drives the real window (key injection under Xvfb or a similar tool). Steps D and E of the item 1/2 plan were not run: `Alt+x setup_new_project` in an empty folder, then `load_project` on this repo. Until this exists, check them by hand.
- Add `install()` rules and a desktop file once the binary layout settles.
- CI: build Debug and Release with the current GCC.

## 9. Housekeeping

- Trim `ship_files/themes` and `UPSTREAM_CHANGES.txt` as wanted.
- Replace the tutorial content (`5ed_tutorial.cpp`, still the Handmade Seattle demo) or remove it.
- Rename `FCODER_*` include guards.
- Add a copyright line for 5ed contributors to `LICENSE` if wanted.

## 10. Open an OMP session from a hotkey

- Support binding a hotkey that opens an OMP session in a pane.
- The hotkey is user-bindable with the other key bindings (`.5ed` bindings, `ship_files/bindings.5ed`).
- Pressing the bound hotkey opens the OMP session in a pane.
