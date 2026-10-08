# 5ed

A personal fork of [4coder](https://github.com/4coder-archive/4coder), the programmable text editor written by Allen Webster (2014-2022, with contributions from Casey Muratori, Alex "insofaras" Baines, Yuval Dolev and Ryan Fleury). Upstream was frozen and open sourced on 2022-05-31; this fork starts from its final commit `c38c7384` (4.1.8). Upstream's changelog is in `UPSTREAM_CHANGES.txt`.

Linux (x86-64, X11/GLX) only.

# Build

Requires GCC (g++), CMake 3.20+, and the X11, Xfixes, OpenGL (GLX) and FreeType development packages.

    cmake -S . -B build                          # Debug by default; add -DCMAKE_BUILD_TYPE=Release for -O3
    cmake --build build -j
    ./build/5ed

Release does not use link-time optimization. A Release IPO trial printed 57 warnings. Each warning says that an OpenGL symbol changed type. The build stays without those warnings. See ROADMAP item 3.

The build writes one executable, `build/5ed`. It also copies `ship_files/` beside that executable. The copy holds fonts, themes, default config and bindings. The build does not write a core library or a custom library. The configure step removes these old libraries from an old build folder. The command table is the list `src/custom/5ed_command_list.h`.

The command line options `-d` and `-D` are gone. 5ed ignores each of them and the path after it. Files after that path open as usual. After any other unknown option, 5ed ignores all remaining arguments.

Source layout: `src/base`, `src/core`, `src/custom`, `src/platform`. The compile line uses one include root, `-I src`.

Config, bindings, themes and projects are `*.5ed` files, looked up in the loaded project's directory, then `~/.5ed/`, then the directory containing the binary.

# Customising 5ed

Edit the files in `src/custom/` and rebuild with `cmake --build build -j`. There is no plugin path and no custom library option. User config, bindings, and themes (`*.5ed` files) need no rebuild. A new command needs a function with `CUSTOM_COMMAND_SIG(name)`. Use `CUSTOM_UI_COMMAND_SIG` for a user-interface command. Add one `COMMAND` line in `src/custom/5ed_command_list.h`. Run `check-commands` to check the list.

Developer targets are not built by default. `regen` regenerates the keycode file. `regen-lexer` regenerates the C++ lexer tables. That output is the same on every run and equals the checked-in files. `check-structure` checks the source layout and the once-built base library. `check-security` checks that setup does not write through a symlink. It also checks that setup does not change a file that exists. `check-commands` checks the command list against the command definitions.

`check-tools` builds the lexer tool and the keycode tool. `check-project-file` writes a new project and parses it. It also parses this repo's `project.5ed` and `ship_files/config.5ed`. `check-command-line` parses sample command lines with the real core parser. `scripts/test-check-structure.sh` tests the `check-structure` script. `scripts/test-check-security.sh <source-root> <build-dir>` tests the `check-security` script.


# Inherited problems being worked on

1. The lexer generator stays. It now gives the same output on every run.
2. Several layers of configuration parsers.

# License

MIT. See `LICENSE`.
