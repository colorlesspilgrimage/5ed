#!/bin/bash
# Regression tests for scripts/check-commands.sh.
# Usage: scripts/test-check-commands.sh <source-root>
# Each test copies src/ to a scratch folder, changes the copy, and runs the check.
set -u

if [ "$#" -ne 1 ]; then
    echo "FAIL: usage scripts/test-check-commands.sh <source-root>"
    exit 1
fi

ROOT=$(cd "$1" && pwd)
CHECK=$ROOT/scripts/check-commands.sh
LIST=src/custom/5ed_command_list.h
fail=0
WORK=$(mktemp -d "${TMPDIR:-/tmp}/5ed-test-commands.XXXXXX")
trap 'rm -rf "$WORK"' EXIT

pass() {
    echo "PASS: $1"
}

fail_test() {
    echo "FAIL: $1"
    fail=1
}

# Make a new copy of src/ in $WORK/tree.
new_tree() {
    rm -rf "$WORK/tree"
    mkdir "$WORK/tree"
    cp -r "$ROOT/src" "$WORK/tree/src"
}

# expect_fail <name> <text>: the check must exit 1 and print <text> in a FAIL line.
expect_fail() {
    if "$CHECK" "$WORK/tree" > "$WORK/out.txt" 2>&1; then
        fail_test "$1 (check passed)"
        cat "$WORK/out.txt"
    elif ! grep -q "^FAIL: .*$2" "$WORK/out.txt"; then
        fail_test "$1 (no FAIL line with $2)"
        cat "$WORK/out.txt"
    else
        pass "$1"
    fi
}

# 1. The real tree passes.
if "$CHECK" "$ROOT" > "$WORK/out.txt" 2>&1; then
    pass "clean tree passes"
else
    fail_test "clean tree passes"
    cat "$WORK/out.txt"
fi

# 2. A list line is missing.
new_tree
grep -v '^COMMAND(undo,' "$ROOT/$LIST" > "$WORK/tree/$LIST"
expect_fail "missing list line" "missing undo"

# 3. The kind in the list is wrong.
new_tree
sed 's/^COMMAND(undo, false,/COMMAND(undo, true,/' "$ROOT/$LIST" > "$WORK/tree/$LIST"
expect_fail "wrong kind" "kind undo"

# 4. CUSTOM_DOC is back in a source file.
new_tree
printf '\nCUSTOM_DOC("x")\n' >> "$WORK/tree/src/custom/5ed_examples.cpp"
expect_fail "CUSTOM_DOC in source" "CUSTOM_DOC"

# 5. A new command with "{" on the signature line has no list line.
new_tree
printf '\nCUSTOM_COMMAND_SIG(audit_brace_command){\n}\n' >> "$WORK/tree/src/custom/5ed_examples.cpp"
expect_fail "brace on signature line" "missing audit_brace_command"

# 6. A new command with a comment after the signature has no list line.
new_tree
printf '\nCUSTOM_UI_COMMAND_SIG(audit_comment_command) // x\n{\n}\n' >> "$WORK/tree/src/custom/5ed_examples.cpp"
expect_fail "comment after signature" "missing audit_comment_command"

# 7. A name occurs twice in the list. Only the duplicate is reported.
new_tree
grep '^COMMAND(undo,' "$ROOT/$LIST" >> "$WORK/tree/$LIST"
expect_fail "duplicate list line" "duplicate undo"
if grep -q '^FAIL: extra undo' "$WORK/out.txt"; then
    fail_test "duplicate list line is not also extra"
else
    pass "duplicate list line is not also extra"
fi

# 8. A description is empty.
new_tree
sed 's/^COMMAND(undo, false, ".*")$/COMMAND(undo, false, "")/' "$ROOT/$LIST" > "$WORK/tree/$LIST"
expect_fail "empty description" "empty description undo"

# 9. A forward declaration is not a definition.
new_tree
printf '\nCUSTOM_COMMAND_SIG(audit_declared_command);\n' >> "$WORK/tree/src/custom/5ed_examples.cpp"
if "$CHECK" "$WORK/tree" > "$WORK/out.txt" 2>&1; then
    pass "declaration is not a definition"
else
    fail_test "declaration is not a definition"
    cat "$WORK/out.txt"
fi

exit "$fail"
