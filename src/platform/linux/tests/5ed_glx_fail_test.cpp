/*
 * 5ed_glx_fail_test.cpp - Checks GLX failure cleanup and pointer clear.
 * The test does not need a display.
 * The test does not call exit on a cleanup path.
 */

#include <GL/gl.h>
#include <GL/glx.h>
#include "platform/opengl/5ed_opengl_defines.h"

typedef int b32;
#define internal static

#include "platform/opengl/5ed_opengl_load.h"
#include "platform/opengl/5ed_opengl_fail.h"

#include <stdio.h>

static int fail_count = 0;
static int call_count = 0;
static const char *call_order[4];
static GLXDrawable seen_drawable = 1;
static GLXContext seen_make_ctx = (GLXContext)1;
static GLXContext seen_destroy_ctx = 0;

static int
sentinel_handler(Display *dpy, XErrorEvent *ev){
    (void)dpy;
    (void)ev;
    return 0;
}

static int
other_handler(Display *dpy, XErrorEvent *ev){
    (void)dpy;
    (void)ev;
    return 0;
}

static int (*
current_handler(void))(Display *, XErrorEvent *){
    int (*prev)(Display *, XErrorEvent *) = XSetErrorHandler(sentinel_handler);
    XSetErrorHandler(prev);
    return prev;
}

static Bool
fake_make_current(Display *dpy, GLXDrawable drawable, GLXContext ctx){
    (void)dpy;
    seen_drawable = drawable;
    seen_make_ctx = ctx;
    if (call_count < 4){
        call_order[call_count] = "make";
        call_count += 1;
    }
    return True;
}

static void
fake_destroy_context(Display *dpy, GLXContext ctx){
    (void)dpy;
    seen_destroy_ctx = ctx;
    if (call_count < 4){
        call_order[call_count] = "destroy";
        call_count += 1;
    }
}

static void
expect(int cond, const char *name){
    if (cond){
        printf("PASS: %s\n", name);
        return;
    }
    printf("FAIL: %s\n", name);
    fail_count += 1;
}

static void
reset_calls(void){
    call_count = 0;
    seen_drawable = 1;
    seen_make_ctx = (GLXContext)1;
    seen_destroy_ctx = 0;
    call_order[0] = "";
    call_order[1] = "";
}

// Point every loaded entry at a bad address. Clear must set each one to null.
#define FIVEED_ARM(name, ret, args) fiveed_##name = (fiveed_##name##_Function *)1;

static void
arm_pointers(void){
    FIVEED_GL_OPTIONAL(FIVEED_ARM);
    FIVEED_GL_REQUIRED(FIVEED_ARM);
}

#undef FIVEED_ARM

#define FIVEED_STILL_SET(name, ret, args) if (fiveed_##name != 0){ return 0; }

static int
pointers_are_clear(void){
    FIVEED_GL_OPTIONAL(FIVEED_STILL_SET);
    FIVEED_GL_REQUIRED(FIVEED_STILL_SET);
    return 1;
}

#undef FIVEED_STILL_SET

int
main(void){
    Display *fake_dpy = (Display *)1;
    GLXContext fake_ctx = (GLXContext)2;

    XSetErrorHandler(other_handler);
    reset_calls();
    glx_fail_cleanup(0, sentinel_handler, fake_ctx, 1, fake_make_current, fake_destroy_context);
    expect(call_count == 0, "null display skips GLX");
    expect(current_handler() == sentinel_handler, "null display restores the handler");

    XSetErrorHandler(other_handler);
    reset_calls();
    glx_fail_cleanup(fake_dpy, sentinel_handler, fake_ctx, 1, fake_make_current, fake_destroy_context);
    expect(call_count == 2, "release and destroy both run");
    expect(call_order[0] != 0 && call_order[0][0] == 'm', "release runs before destroy");
    expect(call_order[1] != 0 && call_order[1][0] == 'd', "destroy runs after release");
    expect(seen_drawable == None && seen_make_ctx == 0, "release drops the current context");
    expect(seen_destroy_ctx == fake_ctx, "destroy receives the context");
    expect(current_handler() == sentinel_handler, "handler restored after GLX cleanup");

    XSetErrorHandler(other_handler);
    reset_calls();
    glx_fail_cleanup(fake_dpy, sentinel_handler, fake_ctx, 0, fake_make_current, fake_destroy_context);
    expect(call_count == 1, "destroy runs without release");
    expect(call_order[0] != 0 && call_order[0][0] == 'd', "only destroy runs");
    expect(current_handler() == sentinel_handler, "handler restored after destroy");

    XSetErrorHandler(other_handler);
    reset_calls();
    glx_fail_cleanup(fake_dpy, sentinel_handler, 0, 0, fake_make_current, fake_destroy_context);
    expect(call_count == 0, "null context skips destroy");
    expect(current_handler() == sentinel_handler, "handler restored with a null context");

    XSetErrorHandler(other_handler);
    glx_fail_cleanup(fake_dpy, sentinel_handler, fake_ctx, 1, 0, 0);
    expect(current_handler() == sentinel_handler, "null ops still restore the handler");

    XSetErrorHandler(other_handler);
    XSetErrorHandler(0);
    int (*default_handler)(Display *, XErrorEvent *) = XSetErrorHandler(sentinel_handler);
    XSetErrorHandler(other_handler);
    glx_fail_cleanup(fake_dpy, 0, 0, 0, 0, 0);
    expect(current_handler() == default_handler, "default handler restored");

    XSetErrorHandler(other_handler);
    glx_fail_cleanup(0, sentinel_handler, 0, 0, 0, 0);
    glx_fail_cleanup(0, sentinel_handler, 0, 0, 0, 0);
    expect(current_handler() == sentinel_handler, "repeated cleanup restores the handler");

    arm_pointers();
    linux_gl_clear_procs();
    expect(pointers_are_clear() != 0, "clear sets every pointer to null");
    linux_gl_clear_procs();
    expect(pointers_are_clear() != 0, "repeated clear keeps every pointer null");

    XSetErrorHandler(0);
    return fail_count == 0 ? 0 : 1;
}
