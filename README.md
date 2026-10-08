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

Config, bindings, themes and projects are `*.5ed` files. 5ed looks in this order. First, the loaded project's directory. Next, the config directory. Next, the data directory. Last, the directory that holds the binary. The config directory is `$XDG_CONFIG_HOME/5ed` when that variable is set and starts with `/`. Otherwise it is `~/.config/5ed`. The data directory is `$XDG_DATA_HOME/5ed` when that variable is set and starts with `/`. Otherwise it is `~/.local/share/5ed`. 5ed uses `$HOME` only when it starts with `/`. 5ed does not read `~/.5ed/`. Move files from that folder. The option `-U <dir>` replaces the config directory.

All `.5ed` files use one parser and one grammar. The grammar is in `src/custom/5ed_config.h`. A `project.5ed` file needs `version(2);`. A compound can be 64 levels deep. 5ed loads at most 65536 values from one file.

`setup_new_project` and `setup_build_sh` quote typed text. They refuse a control character. They do not change a file that exists. Delete the file and run the command again. Flags and the compiler come from `config.5ed` (`default_flags_sh`, `default_compiler_sh`). Those values are written raw. They are not typed text.

# Customising 5ed

Edit the files in `src/custom/` and rebuild with `cmake --build build -j`. There is no plugin path and no custom library option. User config, bindings, and themes (`*.5ed` files) need no rebuild. A new command needs a function with `CUSTOM_COMMAND_SIG(name)`. Use `CUSTOM_UI_COMMAND_SIG` for a user-interface command. Add one `COMMAND` line in `src/custom/5ed_command_list.h`. Run `check-commands` to check the list.

Developer targets are not built by default. `regen` regenerates the keycode file. `regen-lexer` regenerates the C++ lexer tables. That output is the same on every run and equals the checked-in files. `check-structure` checks the source layout and the once-built base library. `check-security` checks that setup does not write through a symlink. It also checks that setup does not change a file that exists. It also checks that setup quotes typed text. It also checks that an output dir that starts with `-` is not a `cd` option. `check-commands` checks the command list against the command definitions.

`check-tools` builds the lexer tool and the keycode tool. `check-project-file` writes a new project and parses it. It also parses this repo's `project.5ed` and `ship_files/config.5ed`. It checks quoted hostile text and a `version(1)` file. It checks that an escaped string with a newline, a tab or a NUL parses back to the same bytes. It checks the nesting limit and a compound that refers to itself. `check-command-line` parses sample command lines with the real core parser. `scripts/test-check-structure.sh` tests the `check-structure` script. `scripts/test-check-commands.sh` tests the `check-commands` script. The `check-commands` target runs it. `scripts/test-check-security.sh <source-root> <build-dir>` tests the `check-security` script.


# Inherited problems being worked on

1. The lexer generator stays. It now gives the same output on every run.


# License

MIT. See `LICENSE`.
