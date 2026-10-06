#!/bin/bash
set -eu

if [ "$#" -ne 2 ]; then
    echo "FAIL: usage scripts/check-structure.sh <source-root> <build-dir>"
    exit 1
fi

ROOT=$1
BUILD=$2
cd "$ROOT"
fail=0

pass() {
    echo "PASS: $1"
}

fail_check() {
    echo "FAIL: $1"
    fail=1
}

require_no_grep() {
    name=$1
    shift
    out="/tmp/5ed-check-${name}.txt"
    if grep "$@" > "$out"; then
        fail_check "$name"
        cat "$out"
    else
        pass "$name"
    fi
}

# 1. no-removed-macros
require_no_grep no-removed-macros -rnE "\b(OS_WINDOWS|OS_MAC|OS_LINUX|OS_NAME|ARCH_X86|ARCH_X64|ARCH_ARM(32|64)|ARCH_(32|64)BIT|ARCH_NAME|COMPILER_(CL|GCC|CLANG|NAME)|CALL_CONVENTION|JUST_GUESS_INTS|FTECH_64_BIT|FCODER_TRANSITION_TO)\b" src CMakeLists.txt ship_files

# 2. no-bat-or-other-os
require_no_grep no-bat-or-other-os -rniE "default_(compiler|flags)_bat|setup_build_bat|prj_generate_bat|\.bat\b|(^|[^A-Za-z0-9_])\.(win|mac) *=" src ship_files CMakeLists.txt

# 3. no-ctm
ctm_hit=0
if grep -n "ctm" .gitignore CMakeLists.txt > /tmp/5ed-check-3a.txt; then
    ctm_hit=1
fi
if grep -rn "\.ctm" src ship_files > /tmp/5ed-check-3b.txt; then
    ctm_hit=1
fi
if [ "$ctm_hit" -eq 0 ]; then
    pass "no-ctm"
else
    fail_check "no-ctm"
    cat /tmp/5ed-check-3a.txt /tmp/5ed-check-3b.txt 2>/dev/null || true
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
    if grep -rn "#include \"base/${name}\"" src > /tmp/5ed-check-7.txt; then
        echo "FAIL: base-cpp-not-included $name"
        cat /tmp/5ed-check-7.txt
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
    nm -C --defined-only "$BUILD/lib5ed_base.a" > /tmp/5ed-base-nm.txt
    sym_ok=1
    for sym in \
        "i32_ceil32(float)" \
        "string_list_pushf(Arena*, List_String_Const_u8*, char*, ...)" \
        "table_hash_u8(unsigned char*, unsigned long)" \
        "layout_nearest_pos_to_xy(Layout_Item_List, Vec2_f32)" \
        "log_event(Arena*, String_Const_u8, String_Const_u8, int, int, int, int)"
    do
        count=$(grep -F -c "$sym" /tmp/5ed-base-nm.txt || true)
        if [ "$count" -ne 1 ]; then
            echo "FAIL: define-once archive count $count for $sym"
            sym_ok=0
        fi
    done
    awk '/^[0-9a-fA-F]+ [TWVDBR] / { $1=""; $2=""; sub(/^ +/, ""); print }' /tmp/5ed-base-nm.txt | grep -v '^DW\.' | sort -u > /tmp/5ed-base-syms.txt
    : > /tmp/5ed-other-syms.txt
    while IFS= read -r obj; do
        nm -C --defined-only "$obj" | awk '/^[0-9a-fA-F]+ [TtwWvVdDbBrR] / { $1=""; $2=""; sub(/^ +/, ""); print }'
    done < <(find "$BUILD/CMakeFiles" -name '*.o' -not -path '*5ed_base*') | sort -u > /tmp/5ed-other-syms.txt
    if comm -12 /tmp/5ed-base-syms.txt /tmp/5ed-other-syms.txt | grep -q .; then
        echo "FAIL: define-once other objects define base symbols"
        comm -12 /tmp/5ed-base-syms.txt /tmp/5ed-other-syms.txt | head -20
        sym_ok=0
    fi
    for bin in "$BUILD/5ed" "$BUILD/5ed_app.so" "$BUILD/custom_5ed.so"; do
        count=$(nm -C --defined-only "$bin" | grep -F -c "i32_ceil32(float)" || true)
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

exit "$fail"
