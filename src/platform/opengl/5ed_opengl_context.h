/*
 * OpenGL context attributes for 5ed.
 *
 * The list requests an OpenGL 3.3 core context.
 * The caller does not free the pointer.
 * The function has no side effect.
 */

#ifndef FIVEED_OPENGL_CONTEXT_H
#define FIVEED_OPENGL_CONTEXT_H

#include <GL/glx.h>

// Return the GLX attribute list. The array has 7 ints. It ends with 0.
inline const int *
opengl_context_attrib_list(void){
    static const int attribs[7] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    return attribs;
}

#endif
