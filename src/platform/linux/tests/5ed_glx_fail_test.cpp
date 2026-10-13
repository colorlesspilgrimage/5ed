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
static void
arm_pointers(void){
    fiveed_glDebugMessageControl = (fiveed_glDebugMessageControl_Function *)1;
    fiveed_glDebugMessageCallback = (fiveed_glDebugMessageCallback_Function *)1;
    fiveed_glGenVertexArrays = (fiveed_glGenVertexArrays_Function *)1;
    fiveed_glBindVertexArray = (fiveed_glBindVertexArray_Function *)1;
    fiveed_glGenBuffers = (fiveed_glGenBuffers_Function *)1;
    fiveed_glBindBuffer = (fiveed_glBindBuffer_Function *)1;
    fiveed_glBufferData = (fiveed_glBufferData_Function *)1;
    fiveed_glBufferSubData = (fiveed_glBufferSubData_Function *)1;
    fiveed_glCreateShader = (fiveed_glCreateShader_Function *)1;
    fiveed_glShaderSource = (fiveed_glShaderSource_Function *)1;
    fiveed_glCompileShader = (fiveed_glCompileShader_Function *)1;
    fiveed_glGetShaderInfoLog = (fiveed_glGetShaderInfoLog_Function *)1;
    fiveed_glDeleteShader = (fiveed_glDeleteShader_Function *)1;
    fiveed_glCreateProgram = (fiveed_glCreateProgram_Function *)1;
    fiveed_glAttachShader = (fiveed_glAttachShader_Function *)1;
    fiveed_glLinkProgram = (fiveed_glLinkProgram_Function *)1;
    fiveed_glUseProgram = (fiveed_glUseProgram_Function *)1;
    fiveed_glGetProgramiv = (fiveed_glGetProgramiv_Function *)1;
    fiveed_glGetProgramInfoLog = (fiveed_glGetProgramInfoLog_Function *)1;
    fiveed_glValidateProgram = (fiveed_glValidateProgram_Function *)1;
    fiveed_glGetUniformLocation = (fiveed_glGetUniformLocation_Function *)1;
    fiveed_glGetAttribLocation = (fiveed_glGetAttribLocation_Function *)1;
    fiveed_glDisableVertexAttribArray = (fiveed_glDisableVertexAttribArray_Function *)1;
    fiveed_glEnableVertexAttribArray = (fiveed_glEnableVertexAttribArray_Function *)1;
    fiveed_glVertexAttribPointer = (fiveed_glVertexAttribPointer_Function *)1;
    fiveed_glVertexAttribIPointer = (fiveed_glVertexAttribIPointer_Function *)1;
    fiveed_glUniform2f = (fiveed_glUniform2f_Function *)1;
    fiveed_glUniformMatrix2fv = (fiveed_glUniformMatrix2fv_Function *)1;
    fiveed_glUniform1i = (fiveed_glUniform1i_Function *)1;
}

static int
pointers_are_clear(void){
    if (fiveed_glDebugMessageControl != 0){ return 0; }
    if (fiveed_glDebugMessageCallback != 0){ return 0; }
    if (fiveed_glGenVertexArrays != 0){ return 0; }
    if (fiveed_glBindVertexArray != 0){ return 0; }
    if (fiveed_glGenBuffers != 0){ return 0; }
    if (fiveed_glBindBuffer != 0){ return 0; }
    if (fiveed_glBufferData != 0){ return 0; }
    if (fiveed_glBufferSubData != 0){ return 0; }
    if (fiveed_glCreateShader != 0){ return 0; }
    if (fiveed_glShaderSource != 0){ return 0; }
    if (fiveed_glCompileShader != 0){ return 0; }
    if (fiveed_glGetShaderInfoLog != 0){ return 0; }
    if (fiveed_glDeleteShader != 0){ return 0; }
    if (fiveed_glCreateProgram != 0){ return 0; }
    if (fiveed_glAttachShader != 0){ return 0; }
    if (fiveed_glLinkProgram != 0){ return 0; }
    if (fiveed_glUseProgram != 0){ return 0; }
    if (fiveed_glGetProgramiv != 0){ return 0; }
    if (fiveed_glGetProgramInfoLog != 0){ return 0; }
    if (fiveed_glValidateProgram != 0){ return 0; }
    if (fiveed_glGetUniformLocation != 0){ return 0; }
    if (fiveed_glGetAttribLocation != 0){ return 0; }
    if (fiveed_glDisableVertexAttribArray != 0){ return 0; }
    if (fiveed_glEnableVertexAttribArray != 0){ return 0; }
    if (fiveed_glVertexAttribPointer != 0){ return 0; }
    if (fiveed_glVertexAttribIPointer != 0){ return 0; }
    if (fiveed_glUniform2f != 0){ return 0; }
    if (fiveed_glUniformMatrix2fv != 0){ return 0; }
    if (fiveed_glUniform1i != 0){ return 0; }
    return 1;
}

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
