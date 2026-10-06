# 5ed

A personal fork of [4coder](https://github.com/4coder-archive/4coder), the programmable text editor written by Allen Webster (2014-2022, with contributions from Casey Muratori, Alex "insofaras" Baines, Yuval Dolev and Ryan Fleury). Upstream was frozen and open sourced on 2022-05-31; this fork starts from its final commit `c38c7384` (4.1.8). Upstream's changelog is in `UPSTREAM_CHANGES.txt`.

Linux (x86-64, X11/GLX) only.

# Build

Requires g++, and the X11, Xfixes, OpenGL (GLX) and FreeType development packages (FreeType found via pkg-config).

    ./build.sh          # dev build (default), or: ./build.sh opt
    ./build/5ed

The build writes `5ed`, `5ed_app.so` and `custom_5ed.so` to `build/`, together with the contents of `ship_files/` (fonts, themes, default config and bindings).

User configuration is read from `~/.5ed/` first, then from the directory containing the binary. Config, bindings, themes and projects are `*.5ed` files.

# Inherited problems being worked on

1. Build system: multiple stages and metaprograms (command metadata extraction, API generators, lexer generator).
2. The documentation system is over-complicated and the documentation is incomplete.
3. The lexer generator is too complicated, and adding a language is hard.
4. Several layers of configuration parsers.
5. Weak base layer, duplicated between the custom layer and the core.

# License

MIT. See `LICENSE`.
