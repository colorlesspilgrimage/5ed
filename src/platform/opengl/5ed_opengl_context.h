/*
 * OpenGL context attributes for 5ed.
 *
 * The caller does not free this static list.
 */

#ifndef FIVEED_OPENGL_CONTEXT_H
#define FIVEED_OPENGL_CONTEXT_H

#include <GL/glx.h>

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
