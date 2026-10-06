#!/bin/bash
# Build the editor. Usage: ./build.sh [dev|opt]    (default: dev)
# Output: ./build/{4ed,4ed_app.so,custom_4coder.so} plus runtime files.
set -e

ROOT="$(dirname "$(readlink -f "$0")")"
BUILD="$ROOT/build"
CUSTOM="$ROOT/custom"
MODE="${1:-dev}"

case "$MODE" in
    dev) MODE_FLAGS="-g -O0 -DFRED_INTERNAL -DFRED_SUPER" ;;
    opt) MODE_FLAGS="-g -O3 -DFRED_SUPER" ;;
    *) echo "usage: $0 [dev|opt]" >&2; exit 1 ;;
esac

CXX="${CXX:-g++}"
COMMON="-std=c++11 -m64 -DFTECH_64_BIT -D_GNU_SOURCE -fPIC -pthread \
-Wno-write-strings -Wno-comment -Wno-switch -Wno-null-dereference -Wno-unused-result $MODE_FLAGS"
FT_CFLAGS="$(pkg-config --cflags freetype2)"

mkdir -p "$BUILD"
cd "$BUILD"

# 1. Custom layer: preprocess, extract command metadata, compile to a shared object.
echo "== custom_4coder.so"
META="-Wno-missing-declarations -Wno-logical-op-parentheses"
# Relative paths: the generator embeds source paths into generated/command_metadata.h.
SRC="../custom/4coder_default_bindings.cpp"
$CXX -I../custom -DMETA_PASS $COMMON $META "$SRC" -E -o 4coder_command_metadata.i
$CXX -I../custom $COMMON $META ../custom/4coder_metadata_generator.cpp -o metadata_generator
./metadata_generator -R "$CUSTOM" "$BUILD/4coder_command_metadata.i"
$CXX -I../custom $COMMON $META "$SRC" -shared -o custom_4coder.so
rm -f metadata_generator 4coder_command_metadata.i

# 2. Core.
echo "== 4ed_app.so"
$CXX -I"$ROOT" -I"$CUSTOM" $FT_CFLAGS $COMMON "$ROOT/4ed_app_target.cpp" -shared -o 4ed_app.so

# 3. Platform layer.
echo "== 4ed"
$CXX -I"$ROOT" -I"$CUSTOM" -I"$ROOT/platform_unix" $FT_CFLAGS -fno-threadsafe-statics $COMMON \
    "$ROOT/platform_linux/linux_4ed.cpp" -o 4ed \
    -lX11 -lXfixes -lGL -lfreetype -lpthread -lm -lrt -ldl

# 4. Runtime files beside the binary.
rm -rf themes fonts
cp -r "$ROOT"/ship_files/* .
echo "built $BUILD/4ed"
