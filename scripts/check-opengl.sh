#!/bin/bash
set -eu

if [ "$#" -ne 1 ]; then
    echo "FAIL: usage scripts/check-opengl.sh <source-root>"
    exit 1
fi

if [ ! -d "$1" ]; then
    echo "FAIL: source root"
    exit 1
fi

if ! ROOT=$(cd -- "$1" && pwd); then
    exit 1
fi

fail() {
    echo "FAIL: $1"
    exit 1
}

# -r does not follow a symbolic link inside the tree.
if grep -r -q -F -- "GL_FUNC" "$ROOT/src" "$ROOT/CMakeLists.txt"; then
    fail "GL_FUNC"
fi

if grep -r -q -F -- "GL/glext.h" "$ROOT/src" "$ROOT/CMakeLists.txt"; then
    fail "GL/glext.h"
fi

if grep -r -q -F -- "GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB" "$ROOT/src" "$ROOT/CMakeLists.txt"; then
    fail "GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB"
fi

if grep -r -q -F -- "#version 130" "$ROOT/src" "$ROOT/CMakeLists.txt"; then
    fail "#version 130"
fi

if ! grep -q -F -- "#version 330 core" "$ROOT/src/platform/opengl/5ed_opengl_render.cpp"; then
    fail "#version 330 core"
fi

if ! grep -q -F -- "CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON" "$ROOT/CMakeLists.txt"; then
    fail "CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON"
fi

echo "PASS: opengl-source"
exit 0
