# 5ed

A personal fork of [4coder](https://github.com/4coder-archive/4coder), the programmable text editor written by Allen Webster (2014-2022, with contributions from Casey Muratori, Alex "insofaras" Baines, Yuval Dolev and Ryan Fleury). Upstream was frozen and open sourced on 2022-05-31; this fork starts from its final commit `c38c7384` (4.1.8). Upstream's changelog is in `UPSTREAM_CHANGES.txt`.

Linux (x86-64, X11/GLX) only.

# Build

Requires GCC (g++), CMake 3.20+, and the X11, Xfixes, OpenGL (GLX) and FreeType development packages.

    cmake -S . -B build                          # Debug by default; add -DCMAKE_BUILD_TYPE=Release for -O3
    cmake --build build -j
    ./build/5ed

Release does not use link-time optimization. IPO warns about OpenGL function pointer types. The build stays without those warnings.

The build writes `5ed`, `5ed_app.so` and `custom_5ed.so` to `build/`, together with the contents of `ship_files/` (fonts, themes, default config and bindings). The command table the custom layer needs is generated during the build into `build/gen/`.

Source layout: `src/base`, `src/core`, `src/custom`, `src/platform`. The compile line uses one include root, `-I src`.

Config, bindings, themes and projects are `*.5ed` files, looked up in the loaded project's directory, then `~/.5ed/`, then the directory containing the binary.

Developer targets (not built by default) regenerate the checked-in files under `src/base/generated/`: `regen` (keycodes and the system, font, graphics and custom APIs), `regen-lexer` (C++ lexer tables; its output currently differs from the checked-in `lexer_cpp.cpp`, so only run it when the lexer definition changes), `check-api-docs` (reports undocumented Custom API functions), and `check-structure` (checks the source layout and the once-built base library).

# Inherited problems being worked on

1. Build system: multiple stages and metaprograms (command metadata extraction, API generators, lexer generator).
2. The documentation system is over-complicated and the documentation is incomplete.
3. The lexer generator is too complicated, and adding a language is hard.
4. Several layers of configuration parsers.

# License

MIT. See `LICENSE`.
