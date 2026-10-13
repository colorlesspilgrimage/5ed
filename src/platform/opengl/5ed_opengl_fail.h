/*
 * OpenGL context failure cleanup for 5ed.
 *
 * Call this after context creation fails.
 * The function does not call exit.
 */

#ifndef FIVEED_OPENGL_FAIL_H
#define FIVEED_OPENGL_FAIL_H

#include <GL/glx.h>

typedef Bool fiveed_glx_make_current_fn(Display *dpy, GLXDrawable drawable, GLXContext ctx);
typedef void fiveed_glx_destroy_context_fn(Display *dpy, GLXContext ctx);

inline void
glx_fail_cleanup(Display *dpy,
                 int (*old_handler)(Display *dpy, XErrorEvent *ev),
                 GLXContext ctx,
                 int made_current,
                 fiveed_glx_make_current_fn *make_current,
                 fiveed_glx_destroy_context_fn *destroy_context){
    if (dpy != 0 && made_current != 0 && make_current != 0){
        make_current(dpy, None, 0);
    }
    if (dpy != 0 && ctx != 0 && destroy_context != 0){
        destroy_context(dpy, ctx);
    }
    XSetErrorHandler(old_handler);
}

#endif
