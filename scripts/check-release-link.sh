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

if ! ROOT=$(cd -- "$1" && pwd); then
    exit 1
fi

cmake -S "$ROOT" -B "$2" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build "$2" -j > "$2/release-link.log" 2>&1

count=$(grep -F -c -- "type of symbol" "$2/release-link.log" || true)
if [ -z "${count}" ]; then
    count=0
fi
if [ "${count}" -ne 0 ]; then
    echo "FAIL: ipo-symbol-type"
    exit 1
fi

echo "PASS: ipo-symbol-type"
exit 0
