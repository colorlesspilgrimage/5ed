#!/bin/bash
# Security checks for the project setup commands, the check scripts and the
# 5ed start sequence.
# Usage: check-security.sh <source root> <build dir>
# The build dir must hold lib5ed_base.a and the 5ed executable.
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

for need in lib5ed_base.a 5ed; do
    if [ ! -f "$BUILD/$need" ]; then
        echo "FAIL: $BUILD/$need is missing. Build the project first." >&2
        exit 1
    fi
done

# Copy one function. Start at the "function" line before "<name>(".
# Stop at the first line "}".
extract_function() {
    awk -v name="$2" '
        prev ~ /^function / && index($0, name "(") == 1 { copy = 1; print prev }
        copy { print }
        copy && /^}/ { copy = 0 }
        { prev = $0 }
    ' "$1"
}

# 1. setup-no-symlink-follow
# The hot directory can be an untrusted checkout.
# A symlink must not write a file in another place.
# A file that exists must stay the same.
src="$ROOT/src/custom/5ed_project_commands.cpp"
harness="$work/prj_harness.cpp"
{
    echo '#include "base/5ed_base.h"'
    echo '#include <stdio.h>'
    echo '#include <fcntl.h>'
    echo '#include <unistd.h>'
    extract_function "$src" prj_text_is_safe
    extract_function "$src" prj_typed_fields_are_safe
    extract_function "$src" prj_shell_quote
    extract_function "$src" prj_escape_code
    extract_function "$src" prj_escape_string
    extract_function "$src" prj_create_new_file
    extract_function "$src" prj_generate_sh
    extract_function "$src" prj_generate_project
    cat <<'EOF'
int
main(int argc, char **argv){
    Arena arena = make_arena_malloc();
    String_Const_u8 dir = SCu8(argv[1]);
    String_Const_u8 script = string_u8_litexpr("build");
    String_Const_u8 compiler = {};
    String_Const_u8 code = string_u8_litexpr("main.cpp");
    String_Const_u8 od = string_u8_litexpr(".");
    String_Const_u8 bf = string_u8_litexpr("app");
    if (argc > 2){
        compiler = SCu8(argv[2]);
    }
    if (argc > 3){
        code = SCu8(argv[3]);
    }
    if (argc > 4){
        od = SCu8(argv[4]);
    }
    if (argc > 5){
        bf = SCu8(argv[5]);
    }
    String_Const_u8 empty = {};
    b32 sh = prj_generate_sh(&arena, empty, compiler, dir, script, code, od, bf);
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

    case_dir="$work/fresh"
    mkdir -p "$case_dir"
    result=$("$work/prj_harness" "$case_dir")
    if [ "$result" != "1 1" ] || ! grep -q "main.cpp" "$case_dir/build.sh" || ! grep -q "version(2);" "$case_dir/project.5ed"; then
        echo "FAIL: setup did not create build.sh and project.5ed in an empty directory"
        setup_ok=0
    fi

    # Hostile typed text must be one quoted argument. It must not run.
    quote_ok=1
    case_dir="$work/quoted"
    mkdir -p "$case_dir/o d"
    bin_name=$(printf '%s' "a'b\"c \$(touch pwned)")
    "$work/prj_harness" "$case_dir" echo "m n.cpp" "o d" "$bin_name" > /dev/null
    if ! bash -n "$case_dir/build.sh"; then
        echo "FAIL: quoted build.sh is not valid shell"
        quote_ok=0
    fi
    (cd "$case_dir" && bash build.sh) > "$work/quoted-out.txt"
    want=$(printf '%s' "m n.cpp -o a'b\"c \$(touch pwned)")
    if ! grep -F -q -- "$want" "$work/quoted-out.txt"; then
        echo "FAIL: quoted build.sh did not keep the typed text as one argument"
        quote_ok=0
    fi
    if [ -e "$case_dir/pwned" ]; then
        echo "FAIL: quoted build.sh ran the typed command"
        quote_ok=0
    fi

    # An output dir that starts with "-" is a directory, not a cd option.
    for od_name in "-P" "-"; do
        case_dir="$work/dash$od_name"
        mkdir -p "$case_dir/$od_name"
        "$work/prj_harness" "$case_dir" "sh -c pwd" main.cpp "$od_name" app > /dev/null
        got=$(cd "$case_dir" && bash build.sh 2>&1)
        if [ "$got" != "$case_dir/$od_name" ]; then
            echo "FAIL: build.sh did not cd into the output dir '$od_name' (got '$got')"
            quote_ok=0
        fi
    done

    case_dir="$work/control"
    mkdir -p "$case_dir"
    nl_name=$(printf 'a\nb')
    result=$("$work/prj_harness" "$case_dir" echo main.cpp . "$nl_name")
    if [ "$result" != "0 0" ] || [ -e "$case_dir/build.sh" ] || [ -e "$case_dir/project.5ed" ]; then
        echo "FAIL: a control character was not refused (got '$result')"
        quote_ok=0
    fi

    # "cd -L" goes to $HOME. The binary would go to the wrong folder.
    case_dir="$work/dash"
    mkdir -p "$case_dir/-L" "$case_dir/home"
    "$work/prj_harness" "$case_dir" "pwd;:" main.cpp -L app > /dev/null
    got=$(cd "$case_dir" && HOME="$case_dir/home" bash build.sh 2> /dev/null | head -n 1)
    if [ "$got" != "$case_dir/-L" ]; then
        echo "FAIL: output dir -L was read as a cd option (build ran in '$got')"
        quote_ok=0
    fi

    if [ "$setup_ok" -eq 1 ]; then
        pass "setup-no-symlink-follow"
    else
        fail_check "setup-no-symlink-follow"
    fi
    if [ "$quote_ok" -eq 1 ]; then
        pass "setup-quote-typed-text"
    else
        fail_check "setup-quote-typed-text"
    fi
fi

# 2. no-fixed-tmp-names
# A fixed name in /tmp can be a symlink from another user.
# Skip this file. Its text holds the pattern that this check finds.
: > "$work/tmp.txt"
for script in "$ROOT"/scripts/*.sh; do
    [ "$(basename "$script")" = "check-security.sh" ] && continue
    grep -nHE '/tmp/' "$script" >> "$work/tmp.txt" || true
done
if [ -s "$work/tmp.txt" ]; then
    fail_check "no-fixed-tmp-names"
    cat "$work/tmp.txt"
else
    pass "no-fixed-tmp-names"
fi

# 3. no-user-library-load
# 5ed must not load a library from $HOME/.5ed or from -d or -D.
# That folder is not a config path. A file there can be untrusted.
# DISPLAY is not set, so 5ed stops at the X11 step.
# The X11 message shows that 5ed passed the library load.
lib_src="$work/evil.cpp"
cat > "$lib_src" <<'EOF'
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
__attribute__((constructor)) static void evil_load(void){
    const char *path = getenv("EVIL_MARKER");
    if (path != 0){
        int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (fd >= 0){
            close(fd);
        }
    }
}
EOF
if ! "$CXX" -shared -fPIC -w "$lib_src" -o "$work/evil.so" 2> "$work/cc-lib.txt"; then
    fail_check "no-user-library-load (test library does not compile)"
    cat "$work/cc-lib.txt"
else
    lib_ok=1
    lib_home="$work/lib-home"
    mkdir -p "$lib_home/.5ed"
    cp "$work/evil.so" "$lib_home/.5ed/custom_5ed.so"
    cp "$work/evil.so" "$lib_home/.5ed/other.so"
    case_no=0
    for args in "" "-d other.so" "-D other.so"; do
        case_no=$((case_no + 1))
        marker="$work/marker-$case_no"
        # Word splitting of $args is intended.
        # shellcheck disable=SC2086
        (cd "$lib_home" && env -i PATH="$PATH" HOME="$lib_home" EVIL_MARKER="$marker" \
            timeout 20 "$BUILD/5ed" $args > "$work/run-$case_no.txt" 2>&1) || true
        if [ -e "$marker" ]; then
            echo "FAIL: 5ed loaded a library from $lib_home/.5ed (args: '$args')"
            lib_ok=0
        elif ! grep -q "Cannot open X11 Display" "$work/run-$case_no.txt"; then
            echo "FAIL: 5ed did not reach the X11 display step (args: '$args')"
            cat "$work/run-$case_no.txt"
            lib_ok=0
        fi
    done
    if [ "$lib_ok" -eq 1 ]; then
        pass "no-user-library-load"
    else
        fail_check "no-user-library-load"
    fi
fi

exit "$fail"
