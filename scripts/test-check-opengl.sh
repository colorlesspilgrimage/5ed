#!/bin/bash
# Edge tests for the OpenGL check scripts.
# Usage: scripts/test-check-opengl.sh <source-root>
set -eu

if [ "$#" -ne 1 ]; then
    echo "FAIL: usage scripts/test-check-opengl.sh <source-root>"
    exit 1
fi

if [ ! -d "$1" ]; then
    echo "FAIL: source root"
    exit 1
fi

ROOT=$(cd -- "$1" && pwd)
fail=0
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

pass() {
    echo "PASS: $1"
}

fail_test() {
    echo "FAIL: $1"
    fail=1
}

run_capture() {
    set +e
    run_out=$("$@" 2>&1)
    run_status=$?
    set -e
}

# A clean copy has the two strings the source check requires.
make_clean_tree() {
    tree=$1
    mkdir -p "$tree/src/platform/opengl"
    printf '%s\n' '#version 330 core' > "$tree/src/platform/opengl/5ed_opengl_render.cpp"
    printf '%s\n' 'set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)' > "$tree/CMakeLists.txt"
}

# --- check-opengl.sh arguments ---

run_capture "$ROOT/scripts/check-opengl.sh"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: usage '; then
    pass "opengl check rejects no arguments"
else
    fail_test "opengl check rejects no arguments"
    printf '%s\n' "$run_out"
fi

run_capture "$ROOT/scripts/check-opengl.sh" "$WORK/missing-root"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: source root$'; then
    pass "opengl check rejects a missing source root"
else
    fail_test "opengl check rejects a missing source root"
    printf '%s\n' "$run_out"
fi

run_capture "$ROOT/scripts/check-opengl.sh" "$ROOT/CMakeLists.txt"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: source root$'; then
    pass "opengl check rejects a file as the source root"
else
    fail_test "opengl check rejects a file as the source root"
    printf '%s\n' "$run_out"
fi

# --- A real forbidden string still fails. ---

bad_tree=$WORK/bad-gl-func
make_clean_tree "$bad_tree"
printf '%s\n' 'GL_FUNC' > "$bad_tree/src/bad.h"
run_capture "$ROOT/scripts/check-opengl.sh" "$bad_tree"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: GL_FUNC$'; then
    pass "opengl check finds GL_FUNC in a real file"
else
    fail_test "opengl check finds GL_FUNC in a real file"
    printf '%s\n' "$run_out"
fi

ver_tree=$WORK/bad-version
make_clean_tree "$ver_tree"
printf '%s\n' '#version 130' >> "$ver_tree/src/platform/opengl/5ed_opengl_render.cpp"
run_capture "$ROOT/scripts/check-opengl.sh" "$ver_tree"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: #version 130$'; then
    pass "opengl check finds version 130"
else
    fail_test "opengl check finds version 130"
    printf '%s\n' "$run_out"
fi

# --- Do not follow a symbolic link out of the tree. CWE-59. ---

link_tree=$WORK/link-tree
make_clean_tree "$link_tree"
outside=$WORK/outside
mkdir -p "$outside"
printf '%s\n' 'GL_FUNC' > "$outside/evil.h"
ln -s "$outside" "$link_tree/src/escape"
run_capture "$ROOT/scripts/check-opengl.sh" "$link_tree"
if [ "$run_status" -eq 0 ] && printf '%s\n' "$run_out" | grep -q '^PASS: opengl-source$'; then
    pass "opengl check does not follow a directory link"
else
    fail_test "opengl check does not follow a directory link"
    printf '%s\n' "$run_out"
fi

# --- check-release-link.sh arguments ---

run_capture "$ROOT/scripts/check-release-link.sh"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: usage$'; then
    pass "release check rejects no arguments"
else
    fail_test "release check rejects no arguments"
    printf '%s\n' "$run_out"
fi

run_capture "$ROOT/scripts/check-release-link.sh" "$ROOT"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: usage$'; then
    pass "release check rejects one argument"
else
    fail_test "release check rejects one argument"
    printf '%s\n' "$run_out"
fi

run_capture "$ROOT/scripts/check-release-link.sh" "$WORK/missing-root" "/build"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: source root$'; then
    pass "release check rejects a missing source root"
else
    fail_test "release check rejects a missing source root"
    printf '%s\n' "$run_out"
fi

run_capture "$ROOT/scripts/check-release-link.sh" "$ROOT" "build/release"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: absolute build dir$'; then
    pass "release check rejects a relative build dir"
else
    fail_test "release check rejects a relative build dir"
    printf '%s\n' "$run_out"
fi

# A stub cmake keeps this test off the network and off a real build.
stub=$WORK/stub
mkdir -p "$stub"
cat > "$stub/cmake" << 'EOF'
#!/bin/bash
if [ "$1" = "--build" ]; then
    printf '%s\n' "build-ok"
fi
exit 0
EOF
chmod +x "$stub/cmake"

# --- A build directory that is a symbolic link. CWE-59. ---

real_build=$WORK/real-build
mkdir -p "$real_build"
link_build=$WORK/link-parent/link-build
mkdir -p "$WORK/link-parent"
ln -s "$real_build" "$link_build"
run_capture env PATH="$stub:$PATH" "$ROOT/scripts/check-release-link.sh" "$ROOT" "$link_build"
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: build dir symlink$'; then
    pass "release check rejects a build dir symlink"
else
    fail_test "release check rejects a build dir symlink"
    printf '%s\n' "$run_out"
fi
if [ -e "$real_build/CMakeCache.txt" ] || [ -e "$real_build/release-link.log" ]; then
    fail_test "release check wrote through a build dir symlink"
else
    pass "release check did not write through a build dir symlink"
fi

# --- The log path must not follow a symbolic link. CWE-61. ---

secret=$WORK/secret.txt
printf '%s\n' "SECRET" > "$secret"
log_build=$WORK/log-build
mkdir -p "$log_build"
ln -s "$secret" "$log_build/release-link.log"
run_capture env PATH="$stub:$PATH" "$ROOT/scripts/check-release-link.sh" "$ROOT" "$log_build"
secret_body=$(cat "$secret")
if [ "$secret_body" = "SECRET" ]; then
    pass "release log symlink left the target unchanged"
else
    fail_test "release log symlink left the target unchanged"
    printf '%s\n' "secret now: $secret_body"
fi
if [ "$run_status" -eq 1 ] && printf '%s\n' "$run_out" | grep -q '^FAIL: log symlink$'; then
    pass "release check rejects a log symlink"
else
    fail_test "release check rejects a log symlink"
    printf '%s\n' "$run_out"
fi

# --- Failure cleanup is present in the platform file. CWE-459. ---

platform="$ROOT/src/platform/linux/linux_5ed.cpp"
cleanup_count=$(grep -c "glx_fail_cleanup" "$platform" || true)
if [ "$cleanup_count" -eq 3 ]; then
    pass "context failure calls cleanup three times"
else
    fail_test "context failure calls cleanup three times"
    printf '%s\n' "count: $cleanup_count"
fi

if grep -q "linux_gl_clear_procs" "$platform"; then
    pass "context failure clears loaded pointers"
else
    fail_test "context failure clears loaded pointers"
fi

if grep -q "glx_ctx_error = false" "$platform"; then
    pass "context create clears the error flag"
else
    fail_test "context create clears the error flag"
fi

exit "$fail"
