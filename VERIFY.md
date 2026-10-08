# VERIFY: ROADMAP item 3, remove the `.so` split

Branch: `feat/remove-the-so-split-roadmap-item-3-1007-1912`. Commit: `ab68f73`.

## Criteria checklist

| # | Criterion | Result | Evidence |
|---|-----------|--------|----------|
| 1 | Fresh configure and build, no error, no new warning | PASS | `rm -rf build; cmake -S . -B build; cmake --build build -j` exit 0. Warning count 0. |
| 2 | `ls build` has `5ed`, no `*.so` | PASS | Listing: `5ed bindings.5ed config.5ed fonts gen lib5ed_base.a metadata_generator themes` plus CMake files. |
| 3 | Editor opens with display | NOT RUN | `DISPLAY` is unset. No GUI check was possible. |
| 4 | Moving `build/*.so` has no effect | PASS (static) | No `.so` exists. `readelf -d` shows only system libraries. |
| 5 | Key commands work (Ctrl+o, Ctrl+s, Alt+x, profile_enable) | NOT RUN | No display. |
| 6 | `-d x.so` starts and does not load `x.so` | PARTIAL | No `case 'd'`/`case 'D'` in `src/core/5ed.cpp`. Unknown options give `CLAct_Ignore`. Run test stops at `FATAL: Cannot open X11 Display!` (no display). |
| 7 | Config, bindings, themes read as before | NOT RUN | No display. Code path not changed in the diff. |
| 8 | grep for `CLAct_CustomDLL\|get_version\|init_apis\|custom_dll\|5ed_app.so\|custom_5ed.so` in `src` and `CMakeLists.txt` | PASS | No output. |
| 9 | `src/base/per_target` absent; no "per-target" word | PASS | `ls` fails. grep for `per-target` gives no output. |
| 10 | `check-structure` prints only `PASS:` | PASS | 10 PASS lines, exit 0. |
| 11 | `test-check-structure.sh . build /tmp/v-rel` all PASS | PASS | 9 PASS lines, exit 0. |
| 12 | `check-tools`, `check-project-file`, `check-security` | PASS | All exit 0. Project file: 3 PASS. Security: 2 PASS. |
| 13 | README: one executable and "Customising 5ed" | PASS | Section exists. It says edit `src/custom/`, rebuild, no plugin path. `-d`/`-D` noted as gone. |
| 14 | ROADMAP item 3 done, decisions, IPO numbers | PASS | Heading has "(done)". Decisions and numbers are listed. Item 6 note updated. |
| 15 | IPO trial done, OFF with warning text in ROADMAP | PASS | 57 warnings. Text in ROADMAP item 3 and item 6. I did not repeat the IPO build. |
| 16 | `regen-system-api` output stable | PASS | After the run, `git status` shows no change. |
| 17 | No `system_api_fill/read_vtable`, `API_VTable_system`, `get_version`, `init_apis` in binary | PASS | `nm -C` grep gives no output. |
| 18 | `app_get_functions`, `custom_layer_init`, `custom_layer_bind_api` present | PASS | `nm` count is 3. |
| 19 | `dlopen`/`dlsym` only in audio, `linux_5ed_functions.cpp`, `unix/*` | PASS | grep matches only those files. |
| 20 | Negative case `mkdir src/base/per_target` | PASS | Output: `FAIL: no-shared-objects src/base/per_target exists`. Directory removed after. |
| 21 | Release build, no multiple definition | PASS | Build in `/tmp/v-rel` exit 0, 0 warnings, 0 "multiple definition". Time 20.4 s. `size` text 1171119. |

## Test results

- `check-structure`: 10 PASS, 0 FAIL.
- `test-check-structure.sh . build /tmp/v-rel`: 9 PASS, 0 FAIL.
- `check-project-file`: 3 PASS, 0 FAIL.
- `check-security`: 2 PASS, 0 FAIL.
- `check-tools`: exit 0.
- There is no unit test framework.

## Run transcript

```
$ rm -rf build && cmake -S . -B build && cmake --build build -j     -> exit 0, 0 warnings
$ readelf -d build/5ed | grep NEEDED
 libX11.so.6 libXfixes.so.3 libfreetype.so.6 libGLX.so.0 libOpenGL.so.0
 libstdc++.so.6 libm.so.6 libgcc_s.so.1 libc.so.6
$ nm -C build/5ed | grep -cE " (T|t) (app_get_functions|custom_layer_init|custom_layer_bind_api)"
3
$ nm -C build/5ed | grep -E "get_version|init_apis|system_api_(fill|read)_vtable"
(no output)
$ cd /tmp && DISPLAY= /home/samh/Work/5ed/build/5ed ; echo exit=$?
FATAL: Cannot open X11 Display!
exit=1
$ cd /tmp && DISPLAY= /home/samh/Work/5ed/build/5ed -d foo.so ; echo exit=$?
FATAL: Cannot open X11 Display!
exit=1
```

The program does not print "Could not load '5ed_app.so'". Old loader code is gone.

## Defects found

- None in the checks that I could run.
- Gap: the GUI checks (criteria 3, 5, 6 full, 7) were not run. There was no display.
  A run-time bug from separate file-static state in the core and custom units cannot be excluded.
- Note: the implementer committed the work. PLAN.md said do not commit. The task allowed it.
- Note: PLAN.md is part of the branch diff against `master`.

VERDICT: PASS
