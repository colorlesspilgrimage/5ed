#!/bin/bash
# Security checks for the project setup commands and the check scripts.
# Usage: check-security.sh <source root> <build dir>
# The build dir must hold lib5ed_base.a.
set -eu

if [ "$#" -ne 2 ]; then
    echo "usage: $0 <source root> <build dir>" >&2
    exit 2
fi

ROOT=$(cd "$1" && pwd)
BUILD=$(cd "$2" && pwd)
CXX=${CXX:-g++}
fail=0

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

pass() {
    echo "PASS: $1"
}

fail_check() {
    echo "FAIL: $1"
    fail=1
}

if [ ! -f "$BUILD/lib5ed_base.a" ]; then
    echo "FAIL: $BUILD/lib5ed_base.a is missing. Build the project first." >&2
    exit 1
fi

# Copy one function from a source file. The function starts at the line
# "function <type>" before "<name>(". It stops at the first line "}".
extract_function() {
    awk -v name="$2" '
        prev ~ /^function / && index($0, name "(") == 1 { copy = 1; print prev }
        copy { print }
        copy && /^}/ { copy = 0 }
        { prev = $0 }
    ' "$1"
}

# 1. setup-no-symlink-follow
# prj_generate_sh and prj_generate_project write build.sh and project.5ed
# in the hot directory. The hot directory can be an untrusted checkout.
# A symlink with these names must not make 5ed write a file elsewhere.
# A file that exists must not be changed.
src="$ROOT/src/custom/5ed_project_commands.cpp"
harness="$work/prj_harness.cpp"
{
    echo '#include "base/5ed_base.h"'
    echo '#include <stdio.h>'
    echo '#include <fcntl.h>'
    echo '#include <unistd.h>'
    extract_function "$src" prj_create_new_file
    extract_function "$src" prj_generate_sh
    extract_function "$src" prj_generate_project
    cat <<'EOF'
int
main(int argc, char **argv){
    Arena arena = make_arena_malloc();
    String_Const_u8 dir = SCu8(argv[1]);
    String_Const_u8 script = string_u8_litexpr("build");
    String_Const_u8 code = string_u8_litexpr("main.cpp");
    String_Const_u8 od = string_u8_litexpr(".");
    String_Const_u8 bf = string_u8_litexpr("app");
    String_Const_u8 empty = {};
    b32 sh = prj_generate_sh(&arena, empty, empty, dir, script, code, od, bf);
    b32 prj = prj_generate_project(&arena, dir, script, od, bf);
    printf("%d %d\n", (int)(sh != 0), (int)(prj != 0));
    return(0);
}
EOF
} > "$harness"

if ! "$CXX" -std=c++11 -w -D_GNU_SOURCE -I"$ROOT/src" "$harness" "$BUILD/lib5ed_base.a" -o "$work/prj_harness" 2> "$work/cc.txt"; then
    fail_check "setup-no-symlink-follow (harness does not compile)"
    cat "$work/cc.txt"
else
    setup_ok=1

    # Dangling symlinks: the targets do not exist yet.
    case_dir="$work/dangling"
    mkdir -p "$case_dir/repo" "$case_dir/outside"
    ln -s "$case_dir/outside/sh_target" "$case_dir/repo/build.sh"
    ln -s "$case_dir/outside/prj_target" "$case_dir/repo/project.5ed"
    "$work/prj_harness" "$case_dir/repo" > /dev/null
    for t in sh_target prj_target; do
        if [ -e "$case_dir/outside/$t" ]; then
            echo "FAIL: setup wrote through a symlink and created outside/$t"
            setup_ok=0
        fi
    done

    # Regular files that exist: keep their content.
    case_dir="$work/existing"
    mkdir -p "$case_dir"
    echo keep > "$case_dir/build.sh"
    echo keep > "$case_dir/project.5ed"
    "$work/prj_harness" "$case_dir" > /dev/null
    for t in build.sh project.5ed; do
        if [ "$(cat "$case_dir/$t")" != "keep" ]; then
            echo "FAIL: setup changed the file $t that exists"
            setup_ok=0
        fi
    done

    # Empty directory: setup must still create the two files.
    case_dir="$work/fresh"
    mkdir -p "$case_dir"
    result=$("$work/prj_harness" "$case_dir")
    if [ "$result" != "1 1" ] || ! grep -q "main.cpp" "$case_dir/build.sh" || ! grep -q "version(2);" "$case_dir/project.5ed"; then
        echo "FAIL: setup did not create build.sh and project.5ed in an empty directory"
        setup_ok=0
    fi

    if [ "$setup_ok" -eq 1 ]; then
        pass "setup-no-symlink-follow"
    else
        fail_check "setup-no-symlink-follow"
    fi
fi

# 2. no-fixed-tmp-names
# Scripts must not write to fixed names in /tmp. Another local user can put
# a symlink there first. Use mktemp.
if grep -nE '/tmp/' "$ROOT/scripts/check-structure.sh" > "$work/tmp.txt"; then
    fail_check "no-fixed-tmp-names"
    cat "$work/tmp.txt"
else
    pass "no-fixed-tmp-names"
fi

exit "$fail"
