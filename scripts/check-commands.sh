#!/bin/bash
set -eu
export LC_ALL=C

if [ "$#" -ne 1 ]; then
    echo "FAIL: usage scripts/check-commands.sh <source-root>"
    exit 1
fi

if ! ROOT=$(cd "$1" 2>/dev/null && pwd); then
    echo "FAIL: source root $1 does not exist"
    exit 1
fi

TMP=$(mktemp -d "${TMPDIR:-/tmp}/5ed-commands.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
fail=0

pass() {
    echo "PASS: $1"
}

fail_check() {
    echo "FAIL: $1"
    fail=1
}

# Definitions are lines that are only a command signature.
grep -rhE '^CUSTOM_(UI_)?COMMAND_SIG\([A-Za-z0-9_]+\)[[:space:]]*$' \
    "$ROOT/src/custom" --include='*.cpp' > "$TMP/def_raw.txt" || true
: > "$TMP/defs"
while IFS= read -r line; do
    [ -n "$line" ] || continue
    line=${line%$'\r'}
    if [ "${line#CUSTOM_UI_COMMAND_SIG(}" != "$line" ]; then
        kind=true
        name=${line#CUSTOM_UI_COMMAND_SIG(}
    else
        kind=false
        name=${line#CUSTOM_COMMAND_SIG(}
    fi
    name=${name%)}
    printf '%s %s\n' "$name" "$kind" >> "$TMP/defs"
done < "$TMP/def_raw.txt"
sort -o "$TMP/defs" "$TMP/defs"

# List lines are COMMAND(name, true|false, "description").
grep -E '^COMMAND\(' "$ROOT/src/custom/5ed_command_list.h" > "$TMP/list_raw.txt" || true
: > "$TMP/list"
desc_bad=0
while IFS= read -r line; do
    [ -n "$line" ] || continue
    rest=${line#COMMAND(}
    name=${rest%%,*}
    rest=${rest#*, }
    kind=${rest%%,*}
    desc=${rest#*, }
    desc=${desc%)}
    if [ "$kind" != "true" ] && [ "$kind" != "false" ]; then
        fail_check "bad kind for $name"
        continue
    fi
    if [ "$desc" = '""' ] || [ -z "$desc" ]; then
        fail_check "empty description $name"
        desc_bad=1
    fi
    printf '%s %s\n' "$name" "$kind" >> "$TMP/list"
done < "$TMP/list_raw.txt"
sort -o "$TMP/list" "$TMP/list"

cut -d' ' -f1 "$TMP/defs" > "$TMP/def_names"
cut -d' ' -f1 "$TMP/list" > "$TMP/list_names"
sort -o "$TMP/def_names" "$TMP/def_names"
sort -o "$TMP/list_names" "$TMP/list_names"

dup=$(uniq -d "$TMP/list_names" || true)
if [ -n "$dup" ]; then
    while IFS= read -r name; do
        fail_check "duplicate $name"
    done <<EOF
$dup
EOF
else
    pass "no duplicate names"
fi

missing=$(comm -23 "$TMP/def_names" "$TMP/list_names" || true)
extra=$(comm -13 "$TMP/def_names" "$TMP/list_names" || true)
if [ -n "$missing" ] || [ -n "$extra" ]; then
    if [ -n "$missing" ]; then
        while IFS= read -r name; do
            fail_check "missing $name"
        done <<EOF
$missing
EOF
    fi
    if [ -n "$extra" ]; then
        while IFS= read -r name; do
            fail_check "extra $name"
        done <<EOF
$extra
EOF
    fi
else
    def_count=$(wc -l < "$TMP/def_names")
    list_count=$(wc -l < "$TMP/list_names")
    pass "definitions match list ($def_count)"
    if [ "$def_count" != "$list_count" ]; then
        fail_check "count $def_count != $list_count"
    fi
fi

kind_bad=0
while IFS= read -r row; do
    [ -n "$row" ] || continue
    # name def_kind list_kind
    set -- $row
    if [ "$2" != "$3" ]; then
        fail_check "kind $1 definition $2 list $3"
        kind_bad=1
    fi
done < <(join "$TMP/defs" "$TMP/list")
if [ "$kind_bad" -eq 0 ]; then
    pass "kinds match"
fi

if grep -rn "CUSTOM_DOC" "$ROOT/src" > "$TMP/doc.txt"; then
    fail_check "CUSTOM_DOC remains"
    cat "$TMP/doc.txt"
else
    pass "no CUSTOM_DOC"
fi

if [ "$desc_bad" -eq 0 ]; then
    pass "descriptions are not empty"
fi

exit "$fail"
