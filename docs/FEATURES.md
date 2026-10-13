# Features

Name: OpenGL 3.3 core context.

The editor draws with an OpenGL 3.3 core context on X11 and GLX.

Entry point: ./build/5ed

Use:

1. Run the Debug setup. Install the packages. Configure with g++. Build the tree.

        sudo apt-get update && sudo apt-get install -y g++ cmake pkg-config libgl-dev mesa-common-dev libx11-dev libxfixes-dev libfreetype-dev libasound2-dev && cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++ && cmake --build build -j

2. Run ./build/5ed from the build directory. Use a private HOME. Save XAUTHORITY before you change HOME. Put a null pcm in that HOME so the audio thread does not open a sound card.

3. Read the window name with xwininfo.

Check:

Run scripts/check-release-link.sh "$PWD" "$PWD/build/release".
The output contains PASS: ipo-symbol-type.

Code and tests:

src/platform/opengl/5ed_opengl_context.h
src/platform/opengl/5ed_opengl_load.h
src/platform/opengl/5ed_opengl_render.cpp
src/platform/linux/linux_5ed.cpp
src/platform/linux/tests/5ed_gl_context_attribs_test.cpp
scripts/check-opengl.sh
scripts/check-release-link.sh
