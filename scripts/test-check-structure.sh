#!/bin/bash
# Regression tests for scripts/check-structure.sh.
# Usage: scripts/test-check-structure.sh <source-root> <build-dir>...
# Each build dir must hold a finished build (Debug or Release).
set -u

if [ "$#" -lt 2 ]; then
    echo "FAIL: usage scripts/test-check-structure.sh <source-root> <build-dir>..."
    exit 1
fi

ROOT=$(cd "$1" && pwd)
shift
CHECK=$ROOT/scripts/check-structure.sh
fail=0
WORK=$(mktemp -d "${TMPDIR:-/tmp}/5ed-test-check.XXXXXX")
trap 'rm -rf "$WORK"' EXIT

pass() {
    echo "PASS: $1"
}

fail_test() {
    echo "FAIL: $1"
    fail=1
}

for build in "$@"; do
    build=$(cd "$build" && pwd)

    # 1. A clean tree passes. Release builds have compiler clones of base
    # functions ("[clone .constprop.0]"). The clones must not count.
    if "$CHECK" "$ROOT" "$build" > "$WORK/out.txt"; then
        pass "clean tree passes ($build)"
    else
        fail_test "clean tree passes ($build)"
        cat "$WORK/out.txt"
    fi

    # 2. A relative build dir is relative to the caller, not to the source root.
    rel=$(realpath --relative-to="$WORK" "$build")
    if (cd "$WORK" && "$CHECK" "$ROOT" "$rel" > "$WORK/out.txt"); then
        pass "relative build dir ($build)"
    else
        fail_test "relative build dir ($build)"
        cat "$WORK/out.txt"
    fi

    # 3. Parallel runs do not share scratch files, and no scratch file stays.
    mkdir -p "$WORK/tmp"
    TMPDIR=$WORK/tmp "$CHECK" "$ROOT" "$build" > "$WORK/p1.txt" &
    pid1=$!
    TMPDIR=$WORK/tmp "$CHECK" "$ROOT" "$build" > "$WORK/p2.txt" &
    pid2=$!
    rc1=0; wait "$pid1" || rc1=$?
    rc2=0; wait "$pid2" || rc2=$?
    if [ "$rc1" -eq 0 ] && [ "$rc2" -eq 0 ] && [ -z "$(ls -A "$WORK/tmp")" ]; then
        pass "parallel runs ($build)"
    else
        fail_test "parallel runs ($build) rc=$rc1,$rc2 left: $(ls -A "$WORK/tmp")"
    fi
    rm -rf "$WORK/tmp"

    # 4. An old build folder has a core library, a custom library and their
    # objects. A new configure removes them, and the checks then pass.
    stale_dir=$build/CMakeFiles/5ed_app.dir
    mkdir -p "$stale_dir"
    cp "$build/CMakeFiles/5ed_base.dir/src/base/5ed_stringf.cpp.o" "$stale_dir/stale.o"
    touch "$build/5ed_app.so" "$build/custom_5ed.so"
    if cmake "$build" > "$WORK/cfg.txt" 2>&1 &&
       [ ! -e "$stale_dir" ] && [ ! -e "$build/5ed_app.so" ] && [ ! -e "$build/custom_5ed.so" ] &&
       "$CHECK" "$ROOT" "$build" > "$WORK/out.txt"; then
        pass "old libraries removed ($build)"
    else
        fail_test "old libraries removed ($build)"
        cat "$WORK/cfg.txt" "$WORK/out.txt" 2>/dev/null
    fi
    rm -rf "$stale_dir"
    rm -f "$build/5ed_app.so" "$build/custom_5ed.so"
done

# 5. Bad arguments give a FAIL line and a non-zero exit.
if "$CHECK" > "$WORK/out.txt" 2>&1; then
    fail_test "no arguments"
elif grep -q "^FAIL: usage" "$WORK/out.txt"; then
    pass "no arguments"
else
    fail_test "no arguments"
fi
if "$CHECK" "$WORK/no-such-root" "$WORK" > "$WORK/out.txt" 2>&1; then
    fail_test "missing source root"
elif grep -q "^FAIL: source root" "$WORK/out.txt"; then
    pass "missing source root"
else
    fail_test "missing source root"
fi

# 6. The checks find a bad include in a copy of the tree.
copy=$WORK/copy
mkdir -p "$copy"
cp -r "$ROOT/src" "$ROOT/scripts" "$ROOT/ship_files" "$ROOT/CMakeLists.txt" "$ROOT/.gitignore" "$copy/"
printf '#include "base/5ed_stringf.cpp"\n#include "../5ed_bad.h"\n' >> "$copy/src/custom/5ed_default_bindings.cpp"
"$CHECK" "$copy" "$WORK" > "$WORK/out.txt" 2>&1 || true
if grep -q "^FAIL: base-cpp-not-included" "$WORK/out.txt" &&
   grep -q "^FAIL: include-style" "$WORK/out.txt"; then
    pass "bad include found"
else
    fail_test "bad include found"
    cat "$WORK/out.txt"
fi

exit "$fail"
