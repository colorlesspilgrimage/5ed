#!/bin/bash
set -eu

if [ "$#" -ne 2 ]; then
    echo "FAIL: usage"
    exit 1
fi

if [ ! -d "$1" ]; then
    echo "FAIL: source root"
    exit 1
fi

case "$2" in
    /*) ;;
    *)
        echo "FAIL: absolute build dir"
        exit 1
        ;;
esac

# A symbolic link can point at a file outside this directory.
if [ -L "$2" ]; then
    echo "FAIL: build dir symlink"
    exit 1
fi

if [ -e "$2" ] && [ ! -d "$2" ]; then
    echo "FAIL: build dir"
    exit 1
fi

if [ -L "$2/release-link.log" ]; then
    echo "FAIL: log symlink"
    exit 1
fi

if ! ROOT=$(cd -- "$1" && pwd); then
    exit 1
fi

cmake -S "$ROOT" -B "$2" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++

if [ -L "$2" ]; then
    echo "FAIL: build dir symlink"
    exit 1
fi

log="$2/release-link.log"
if [ -L "$log" ]; then
    echo "FAIL: log symlink"
    exit 1
fi
if [ -e "$log" ] && [ ! -f "$log" ]; then
    echo "FAIL: log path"
    exit 1
fi

# Create a new file. Do not open a symbolic link.
if ! tmp=$(mktemp "$2/release-link.XXXXXX"); then
    echo "FAIL: log create"
    exit 1
fi

set +e
cmake --build "$2" -j > "$tmp" 2>&1
build_status=$?
set -e
mv -f -- "$tmp" "$log"
if [ "$build_status" -ne 0 ]; then
    exit "$build_status"
fi

count=$(grep -F -c -- "type of symbol" "$log" || true)
if [ -z "${count}" ]; then
    count=0
fi
if [ "${count}" -ne 0 ]; then
    echo "FAIL: ipo-symbol-type"
    exit 1
fi

echo "PASS: ipo-symbol-type"
exit 0
