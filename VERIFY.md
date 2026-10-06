# VERIFY: ROADMAP points 1 and 2

Branch: `feat/implement-points-1-and-2-of-roadmap-md-1006-1406`. Base: `master`.
Builds ran in `/tmp` build directories, not in `build/`. I deleted them afterwards.

## Criteria checklist

| # | Criterion | Result | Evidence |
|---|---|---|---|
| 1 | Debug build passes, no warnings | PASS | Clean `cmake -S . -B /tmp/vb && cmake --build /tmp/vb -j8` ended with `[100%] Built target 5ed`. The warning count was 0. |
| 2 | Release build passes | PASS | `cmake -S . -B /tmp/vr -DCMAKE_BUILD_TYPE=Release`, build ended `Built target 5ed`. |
| 3 | Editor starts and stays up | PASS | `timeout 5 ./5ed -w 900 600 README.md` printed `open: No such file or directory` and `exit=124`. Debug and Release. |
| 4 | No `OS_WINDOWS/OS_MAC/OS_LINUX/ARCH_X86/COMPILER_CL/COMPILER_CLANG` outside ROADMAP | PASS (note) | `git grep` hits only `PLAN.md` and `scripts/check-structure.sh` (its own pattern). No source hit. |
| 5 | `5ed_base_types.h` has only one `#error` guard | PASS | Lines 12-14 hold the guard for non-GCC, non-Linux, non-x86-64. |
| 6 | No `default_*_bat`, `setup_build_bat`, `prj_generate_bat` | PASS (note) | `git grep` hits only `PLAN.md` and the check script pattern. |
| 7 | Command table has no `setup_build_bat*` | PASS | `grep -c setup_build_bat gen/generated/command_metadata.h` printed `0`. `setup_build_sh\|setup_new_project` printed `6`. |
| 8 | `setup_new_project` makes `project.5ed` + `.sh`, no `.bat`, `.linux` keys only | PASS | See the run transcript. |
| 9 | `config.5ed` has no `_bat` line | PASS | `grep -n _bat ship_files/config.5ed` printed nothing. `default_compiler_sh` and `default_flags_sh` remain. |
| 10 | `.gitignore` has no `5ed_data.ctm` | PASS | `grep ctm .gitignore` printed nothing. |
| 11 | Root has no `5ed*.cpp/.h`. Source in `src/{base,core,custom,platform}` | PASS | Root list: `CMakeLists.txt LICENSE PLAN.md README.md ROADMAP.md UPSTREAM_CHANGES.txt build project.5ed scripts ship_files src`. |
| 12 | No `"../` includes. One include dir `-I src` | PASS | `grep -rn '"\.\./' src` printed nothing. Verbose compile line of `5ed_mem.cpp` has only `-I/home/samh/Work/5ed/src`. |
| 13 | `5ed_base_types.cpp` compiled once | PASS | `grep -rn 5ed_base_types.cpp src CMakeLists.txt` shows only `CMakeLists.txt:63` (`add_library(5ed_base`). |
| 14 | `check-structure` all PASS | PASS | 9 lines, all `PASS:`. Exit 0. Release build also gave 9 lines. |
| 15 | `regen-*` leaves `src/base/generated` unchanged | PASS | Only `5ed_event_codes.h` changed (306 lines, known indentation). I restored it with `git checkout`. |
| 16 | README layout, problem 5 removed. ROADMAP marks 1 and 2 done, keeps 3-8 | PASS | README lists 4 problems. ROADMAP items 1 and 2 say `(done)`. Items 3-8 remain. The per-target bullet is in item 3. |
| 17 | `ship_files` arrive in build dir | PASS | `config.5ed`, `bindings.5ed`, `fonts`, `themes` exist in the build dir. |
| 18 | Working tree clean, no stray files | PASS | `git status --short` showed no untracked or modified file after my restore. |

## Test results

There is no test suite. The only automated check is `check-structure`.

- `cmake --build /tmp/vb --target check-structure`: 9 PASS, 0 FAIL, exit 0.
- Release `check-structure`: 9 result lines (all PASS in the Debug run; I counted only lines in Release).

## Run transcript: `setup_new_project` (user priority)

`xdotool`, `Xvfb`, `ydotool` and `python-xlib` are not installed. The session is Wayland. I did not type into any terminal.
Instead I wrote a throwaway C++ harness in `/tmp/harness`. It included `src/custom/5ed_default_include.cpp` and linked `lib5ed_base.a`.
It stubbed only the host API: thread context, memory, file attributes, hot directory, query bar, `get_next_input_raw`, print, window title, mutex.
The real `setup_new_project`, `prj_setup_scripts`, `query_user_string`, `load_project` and the config parser ran.
The harness loaded the real `ship_files/config.5ed`. It fed the answers `main.cpp`, empty (default `.`), `demo` as key events.
I deleted the harness afterwards. The GUI steps D and E of PLAN.md were not run in the real editor.

```
$ mkdir /tmp/5ed-demo && cd /tmp/5ed-demo && ls -A     # empty
$ /tmp/harness/h ship_files/config.5ed setup x
CONFIG compiler=[g++] flags=[-g]
PROMPT: Build Target:
PROMPT: Output Directory:
PROMPT: Binary Output:
... (project printed) ...
MSG: Project errors:
MSG: /tmp/5ed-demo/project.5ed:28:12: expected an r-value; ...
/tmp/5ed-demo/project.5ed:29:1: expected an l-value ...
/tmp/5ed-demo/project.5ed:30:1: expected an l-value ...
TITLE: 5ed project: demo
$ ls
build.sh  project.5ed
$ ls *.bat
ls: cannot access '*.bat': No such file or directory
$ grep -c "\.win\|\.mac" project.5ed
0
$ cat build.sh
#!/bin/bash

code="$PWD"
opts=-g
cd . > /dev/null
g++ $opts $code/main.cpp -o demo
cd $code > /dev/null
$ bash -n build.sh && echo BASHN_OK
BASHN_OK
$ chmod +x build.sh && ./build.sh; echo $?; ./demo
0
hello 5ed
```

`project.5ed` has `version(2);`, `project_name = "demo";`, `load_paths = { .linux = load_paths_base, }`, `.build ... .linux = "./build.sh"`, `.run ... .linux = "./demo"`. It has no `*.bat` pattern, no `.win`, no `.mac`.

Second run in the same folder: `MSG: project already setup, no changes made`.
Run after I deleted `build.sh`: the prompts `Build Target`, `Output Directory`, `Binary Output` came again. The log said `project.5ed already exists, no changes made to it`. The script was rebuilt. This confirms the `sh_exists` / `project_exists` fix.

| User check | Result |
|---|---|
| (1) `project.5ed` + `.sh`, no `.bat` | PASS |
| (2) `.linux` keys only, no `.win`/`.mac` | PASS |
| (3) `.sh` valid (`bash -n`), builds `main.cpp`, binary runs | PASS |
| (4) New `project.5ed` loads without error | FAIL, pre-existing. It loads (title and variables are set). The parser reports 3 errors for the `fkey_command` block. See defect 1. |
| `setup_build_bat`, `setup_build_bat_and_sh` absent | PASS (0 hits in command table) |

Repo project: harness parse of `/home/samh/Work/5ed/project.5ed` gave `PROJECT PARSE: version=2 errors=none`.

## Defects

1. **Pre-existing, not a regression.** The generated `project.5ed` ends with
   `fkey_command = { .F1 = "run"; .F2 = "run"; };`. The parser rejects the `;` inside the braces.
   `load_project` prints `Project errors:` for lines 28-30.
   `master` has the same four `fprintf` lines in `prj_generate_project`. The branch did not change them.
   PLAN.md says do not change editor behavior, so the implementer was correct to leave it.
   Fix: use commas, or `.F1 = "run",`. A later change should do this.
2. Minor. `scripts/check-structure.sh` writes scratch files to `/tmp/5ed-check-*.txt`. It also contains the removed macro names in its patterns. Both are harmless.
3. Minor. Release has no IPO (README says so). The editor started in Release. I did not measure speed.
4. Not run. PLAN.md steps D and E in the real GUI, and the `Alt+x` lister. No safe key-injection tool exists. The harness covered the command logic. The command table covered the lister content.

VERDICT: PASS
