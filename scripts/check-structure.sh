#!/bin/bash
set -eu

if [ "$#" -ne 2 ]; then
    echo "FAIL: usage scripts/check-structure.sh <source-root> <build-dir>"
    exit 1
fi

# Make the paths absolute before the cd. A relative build dir is relative to
# the directory of the caller, not to the source root.
if ! ROOT=$(cd "$1" 2>/dev/null && pwd); then
    echo "FAIL: source root $1 does not exist"
    exit 1
fi
case "$2" in
    /*) BUILD=$2 ;;
    *) BUILD=$PWD/$2 ;;
esac
cd "$ROOT"
fail=0

# Each run uses its own scratch folder. Parallel runs do not share files.
TMP=$(mktemp -d "${TMPDIR:-/tmp}/5ed-check.XXXXXX")
trap 'rm -rf "$TMP"' EXIT

# count_sym <file> <types> <symbol>
# Print the number of defined symbols with exactly this demangled name.
# Compiler clones (for example "[clone .constprop.0]") do not count.
count_sym() {
    nm -C --defined-only "$1" | awk -v types="$2" -v sym="$3" '
        /^[0-9a-fA-F]+ [A-Za-z] / {
            type = $2
            $1 = ""; $2 = ""; sub(/^ +/, "")
            if (index(types, type) > 0 && $0 == sym) n++
        }
        END { print n + 0 }'
}

pass() {
    echo "PASS: $1"
}

fail_check() {
    echo "FAIL: $1"
    fail=1
}

# 1. no-removed-macros
if grep -rnE "\b(OS_WINDOWS|OS_MAC|OS_LINUX|OS_NAME|ARCH_X86|ARCH_X64|ARCH_ARM(32|64)|ARCH_(32|64)BIT|ARCH_NAME|COMPILER_(CL|GCC|CLANG|NAME)|CALL_CONVENTION|JUST_GUESS_INTS|FTECH_64_BIT|FCODER_TRANSITION_TO)\b" src CMakeLists.txt ship_files > "$TMP/check-1.txt"; then
    fail_check "no-removed-macros"
    cat "$TMP/check-1.txt"
else
    pass "no-removed-macros"
fi

# 2. no-bat-or-other-os
if grep -rniE "default_(compiler|flags)_bat|setup_build_bat|prj_generate_bat|\.bat\b|(^|[^A-Za-z0-9_])\.(win|mac) *=" src ship_files CMakeLists.txt > "$TMP/check-2.txt"; then
    fail_check "no-bat-or-other-os"
    cat "$TMP/check-2.txt"
else
    pass "no-bat-or-other-os"
fi

# 3. no-ctm
ctm_hit=0
if grep -n "ctm" .gitignore CMakeLists.txt > "$TMP/check-3a.txt"; then
    ctm_hit=1
fi
if grep -rn "\.ctm" src ship_files > "$TMP/check-3b.txt"; then
    ctm_hit=1
fi
if [ "$ctm_hit" -eq 0 ]; then
    pass "no-ctm"
else
    fail_check "no-ctm"
    cat "$TMP/check-3a.txt" "$TMP/check-3b.txt" 2>/dev/null || true
fi

# 4. layout
layout_ok=1
for name in custom generated docs platform_linux platform_unix opengl; do
    if [ -e "$name" ]; then
        echo "FAIL: layout still has $name"
        layout_ok=0
    fi
done
if compgen -G "5ed*.cpp" > /dev/null || compgen -G "5ed*.h" > /dev/null; then
    echo "FAIL: layout still has root 5ed sources"
    layout_ok=0
fi
for name in src/base src/core src/custom src/platform; do
    if [ ! -d "$name" ]; then
        echo "FAIL: layout missing $name"
        layout_ok=0
    fi
done
if [ "$layout_ok" -eq 1 ]; then
    pass "layout"
else
    fail=1
fi

# 5. include-style
inc_bad=0
while IFS= read -r line; do
    file=${line%%:*}
    rest=${line#*:}
    inc=$(printf '%s\n' "$rest" | sed -n 's/.*#include "\([^"]*\)".*/\1/p')
    [ -n "$inc" ] || continue
    case "$inc" in
        base/*|core/*|custom/*|platform/*) ;;
        generated/command_metadata.h|generated/managed_id_metadata.cpp) ;;
        *)
            echo "FAIL: include-style $file $inc"
            inc_bad=1
            ;;
    esac
    case "$inc" in
        *../*)
            echo "FAIL: include-style parent $file $inc"
            inc_bad=1
            ;;
    esac
done < <(grep -rn '#include "' src || true)
if [ "$inc_bad" -eq 0 ]; then
    pass "include-style"
else
    fail=1
fi

# 6. layer-direction
layer_bad=0
while IFS= read -r line; do
    file=${line%%:*}
    rest=${line#*:}
    inc=$(printf '%s\n' "$rest" | sed -n 's/.*#include "\([^"]*\)".*/\1/p')
    [ -n "$inc" ] || continue
    rel=${file#src/}
    layer=${rel%%/*}
    case "$rel" in
        base/per_target/*|base/generated/*) continue ;;
    esac
    top=${inc%%/*}
    ok=0
    case "$layer" in
        base) [ "$top" = "base" ] && ok=1 ;;
        custom) [ "$top" = "base" ] || [ "$top" = "custom" ] && ok=1 ;;
        core) [ "$top" = "base" ] || [ "$top" = "core" ] && ok=1 ;;
        platform) [ "$top" = "base" ] || [ "$top" = "core" ] || [ "$top" = "platform" ] && ok=1 ;;
    esac
    case "$inc" in
        generated/command_metadata.h|generated/managed_id_metadata.cpp) ok=1 ;;
    esac
    if [ "$ok" -eq 0 ]; then
        echo "FAIL: layer-direction $file -> $inc"
        layer_bad=1
    fi
done < <(grep -rn '#include "' src || true)
if [ "$layer_bad" -eq 0 ]; then
    pass "layer-direction"
else
    fail=1
fi

# 7. base-cpp-not-included
once=(
    5ed_base_types.cpp
    5ed_stringf.cpp
    5ed_hash_functions.cpp
    5ed_table.cpp
    5ed_codepoint_map.cpp
    5ed_events.cpp
    5ed_string_match.cpp
    5ed_token.cpp
    5ed_buffer_seek_constructors.cpp
    5ed_layout_lookup.cpp
    5ed_log_helpers.cpp
    5ed_doc_content_types.cpp
    5ed_mem.cpp
    5ed_malloc_allocator.cpp
    5ed_stdio_file.cpp
)
cpp_bad=0
for name in "${once[@]}"; do
    if grep -rn "#include \"base/${name}\"" src > "$TMP/check-7.txt"; then
        echo "FAIL: base-cpp-not-included $name"
        cat "$TMP/check-7.txt"
        cpp_bad=1
    fi
done
if [ "$cpp_bad" -eq 0 ]; then
    pass "base-cpp-not-included"
else
    fail=1
fi

# 8. define-once
if [ ! -f "$BUILD/lib5ed_base.a" ]; then
    fail_check "define-once missing $BUILD/lib5ed_base.a"
else
    nm -C --defined-only "$BUILD/lib5ed_base.a" > "$TMP/base-nm.txt"
    sym_ok=1
    for sym in \
        "i32_ceil32(float)" \
        "string_list_pushf(Arena*, List_String_Const_u8*, char*, ...)" \
        "table_hash_u8(unsigned char*, unsigned long)" \
        "layout_nearest_pos_to_xy(Layout_Item_List, Vec2_f32)" \
        "log_event(Arena*, String_Const_u8, String_Const_u8, int, int, int, int)"
    do
        count=$(count_sym "$BUILD/lib5ed_base.a" TW "$sym")
        if [ "$count" -ne 1 ]; then
            echo "FAIL: define-once archive count $count for $sym"
            sym_ok=0
        fi
    done
    awk '/^[0-9a-fA-F]+ [TWVDBR] / { $1=""; $2=""; sub(/^ +/, ""); print }' "$TMP/base-nm.txt" | grep -v '^DW\.' | sort -u > "$TMP/base-syms.txt"
    : > "$TMP/other-syms.txt"
    while IFS= read -r obj; do
        nm -C --defined-only "$obj" | awk '/^[0-9a-fA-F]+ [TtwWvVdDbBrR] / { $1=""; $2=""; sub(/^ +/, ""); print }'
    done < <(find "$BUILD/CMakeFiles" -name '*.o' -not -path "$BUILD/CMakeFiles/5ed_base.dir/*") | sort -u > "$TMP/other-syms.txt"
    if comm -12 "$TMP/base-syms.txt" "$TMP/other-syms.txt" | grep -q .; then
        echo "FAIL: define-once other objects define base symbols"
        comm -12 "$TMP/base-syms.txt" "$TMP/other-syms.txt" | head -20
        sym_ok=0
    fi
    for bin in "$BUILD/5ed" "$BUILD/5ed_app.so" "$BUILD/custom_5ed.so"; do
        count=$(count_sym "$bin" TtWw "i32_ceil32(float)")
        if [ "$count" -ne 1 ]; then
            echo "FAIL: define-once $bin i32_ceil32 count $count"
            sym_ok=0
        fi
    done
    if [ "$sym_ok" -eq 1 ]; then
        pass "define-once"
    else
        fail=1
    fi
fi

# 9. ship-files
if grep -q "default_compiler_sh" ship_files/config.5ed && grep -q "default_flags_sh" ship_files/config.5ed; then
    pass "ship-files"
else
    fail_check "ship-files"
fi

# 10. no-dynamic-export
# The .so files must not export base functions or the per-target command map
# functions. Each binary keeps a private copy. Exported copies can bind to the
# copy in the other binary.
export_bad=0
for bin in "$BUILD/5ed_app.so" "$BUILD/custom_5ed.so"; do
    if [ ! -f "$bin" ]; then
        echo "FAIL: no-dynamic-export missing $bin"
        export_bad=1
        continue
    fi
    nm -D -C --defined-only "$bin" | awk '{ $1=""; $2=""; sub(/^ +/, ""); print }' | sort -u > "$TMP/dyn-syms.txt"
    if grep -E "^(mapping_|mapping__|map_|map__|command_trigger_)" "$TMP/dyn-syms.txt" > "$TMP/dyn-bad.txt" ||
       { [ -f "$TMP/base-syms.txt" ] && comm -12 "$TMP/base-syms.txt" "$TMP/dyn-syms.txt" > "$TMP/dyn-bad.txt" && [ -s "$TMP/dyn-bad.txt" ]; }; then
        echo "FAIL: no-dynamic-export $bin exports:"
        head -20 "$TMP/dyn-bad.txt"
        export_bad=1
    fi
done
if [ "$export_bad" -eq 0 ]; then
    pass "no-dynamic-export"
else
    fail=1
fi

exit "$fail"
