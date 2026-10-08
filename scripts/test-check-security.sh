#!/bin/bash
# Regression tests for scripts/check-security.sh.
# Usage: scripts/test-check-security.sh <source-root> <build-dir>
# The build dir must hold a finished build.
set -u

if [ "$#" -ne 2 ]; then
    echo "FAIL: usage scripts/test-check-security.sh <source-root> <build-dir>"
    exit 1
fi

ROOT=$(cd "$1" && pwd)
BUILD=$(cd "$2" && pwd)
fail=0
WORK=$(mktemp -d "${TMPDIR:-/tmp}/5ed-test-security.XXXXXX")
trap 'rm -rf "$WORK"' EXIT

pass() {
    echo "PASS: $1"
}

fail_test() {
    echo "FAIL: $1"
    fail=1
}

# The bad text is made in two parts.
# Thus this file does not hold a fixed scratch name itself.
fixed_name="/tm""p/5ed-fixed-name.txt"

# 1. A clean tree passes the scratch-name check.
"$ROOT/scripts/check-security.sh" "$ROOT" "$BUILD" > "$WORK/out.txt" 2>&1 || true
if grep -q "^PASS: no-fixed-tmp-names" "$WORK/out.txt"; then
    pass "clean tree passes no-fixed-tmp-names"
else
    fail_test "clean tree fails no-fixed-tmp-names"
    cat "$WORK/out.txt"
fi

# 2. Each script that writes scratch files must not use a fixed name.
# A fixed name in any of these scripts must give a FAIL line.
for script in check-structure.sh check-commands.sh test-check-structure.sh test-check-security.sh; do
    copy=$WORK/copy
    rm -rf "$copy"
    mkdir -p "$copy"
    cp -r "$ROOT/src" "$ROOT/scripts" "$copy/"
    printf ': > %s\n' "$fixed_name" >> "$copy/scripts/$script"
    "$ROOT/scripts/check-security.sh" "$copy" "$BUILD" > "$WORK/out.txt" 2>&1 || true
    if grep -q "^FAIL: no-fixed-tmp-names" "$WORK/out.txt"; then
        pass "fixed scratch name in $script is found"
    else
        fail_test "fixed scratch name in $script is not found"
    fi
done

exit "$fail"
