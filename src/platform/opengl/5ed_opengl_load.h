/*
 * OpenGL entry points for 5ed.
 *
 * A global gl* data symbol makes ld warn under IPO.
 * The warning text contains type of symbol.
 * ld reports the type change from 2 to 1.
 *
 * Include this file from one translation unit.
 * Include it after GL/gl.h.
 * Include it after platform/opengl/5ed_opengl_defines.h.
 */

#ifndef FIVEED_OPENGL_LOAD_H
#define FIVEED_OPENGL_LOAD_H

#define FIVEED_GL_PTR(name, ret, args) \
    typedef ret (fiveed_##name##_Function) args; \
    static fiveed_##name##_Function *fiveed_##name = 0

#define FIVEED_GL_OPTIONAL(M) \
    M(glDebugMessageControl, void, (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled)); \
    M(glDebugMessageCallback, void, (GLDEBUGPROC callback, const void *userParam));

#define FIVEED_GL_REQUIRED(M) \
    M(glGenVertexArrays, void, (GLsizei n, GLuint *arrays)); \
    M(glBindVertexArray, void, (GLuint array)); \
    M(glGenBuffers, void, (GLsizei n, GLuint *buffers)); \
    M(glBindBuffer, void, (GLenum target, GLuint buffer)); \
    M(glBufferData, void, (GLenum target, GLsizeiptr size, const void *data, GLenum usage)); \
    M(glBufferSubData, void, (GLenum target, GLsizeiptr offset, GLsizeiptr size, const void *data)); \
    M(glCreateShader, GLuint, (GLenum type)); \
    M(glShaderSource, void, (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)); \
    M(glCompileShader, void, (GLuint shader)); \
    M(glGetShaderInfoLog, void, (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)); \
    M(glDeleteShader, void, (GLuint shader)); \
    M(glCreateProgram, GLuint, (void)); \
    M(glAttachShader, void, (GLuint program, GLuint shader)); \
    M(glLinkProgram, void, (GLuint program)); \
    M(glUseProgram, void, (GLuint program)); \
    M(glGetProgramiv, void, (GLuint program, GLenum pname, GLint *params)); \
    M(glGetProgramInfoLog, void, (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)); \
    M(glValidateProgram, void, (GLuint program)); \
    M(glGetUniformLocation, GLint, (GLuint program, const GLchar *name)); \
    M(glGetAttribLocation, GLint, (GLuint program, const GLchar *name)); \
    M(glDisableVertexAttribArray, void, (GLuint index)); \
    M(glEnableVertexAttribArray, void, (GLuint index)); \
    M(glVertexAttribPointer, void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)); \
    M(glVertexAttribIPointer, void, (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer)); \
    M(glUniform2f, void, (GLint location, GLfloat v0, GLfloat v1)); \
    M(glUniformMatrix2fv, void, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)); \
    M(glUniform1i, void, (GLint location, GLint v0));

FIVEED_GL_OPTIONAL(FIVEED_GL_PTR)
FIVEED_GL_REQUIRED(FIVEED_GL_PTR)
#undef FIVEED_GL_PTR

#define FIVEED_LOAD_REQUIRED(name, ret, args) \
    fiveed_##name = (fiveed_##name##_Function *)glXGetProcAddressARB((const GLubyte *)#name); \
    if (fiveed_##name == 0){ return false; }

#define FIVEED_LOAD_OPTIONAL(name, ret, args) \
    fiveed_##name = (fiveed_##name##_Function *)glXGetProcAddressARB((const GLubyte *)#name)

// Call this only after glXMakeCurrent. A null optional pointer is valid.
internal b32
linux_gl_load_procs(void){
    FIVEED_GL_REQUIRED(FIVEED_LOAD_REQUIRED);
    FIVEED_GL_OPTIONAL(FIVEED_LOAD_OPTIONAL);
    return true;
}

#undef FIVEED_LOAD_REQUIRED
#undef FIVEED_LOAD_OPTIONAL

// A later draw must not call a pointer for a destroyed context.
#define FIVEED_CLEAR(name, ret, args) fiveed_##name = 0

internal void
linux_gl_clear_procs(void){
    FIVEED_GL_OPTIONAL(FIVEED_CLEAR);
    FIVEED_GL_REQUIRED(FIVEED_CLEAR);
}

#undef FIVEED_CLEAR

#define glDebugMessageControl fiveed_glDebugMessageControl
#define glDebugMessageCallback fiveed_glDebugMessageCallback
#define glGenVertexArrays fiveed_glGenVertexArrays
#define glBindVertexArray fiveed_glBindVertexArray
#define glGenBuffers fiveed_glGenBuffers
#define glBindBuffer fiveed_glBindBuffer
#define glBufferData fiveed_glBufferData
#define glBufferSubData fiveed_glBufferSubData
#define glCreateShader fiveed_glCreateShader
#define glShaderSource fiveed_glShaderSource
#define glCompileShader fiveed_glCompileShader
#define glGetShaderInfoLog fiveed_glGetShaderInfoLog
#define glDeleteShader fiveed_glDeleteShader
#define glCreateProgram fiveed_glCreateProgram
#define glAttachShader fiveed_glAttachShader
#define glLinkProgram fiveed_glLinkProgram
#define glUseProgram fiveed_glUseProgram
#define glGetProgramiv fiveed_glGetProgramiv
#define glGetProgramInfoLog fiveed_glGetProgramInfoLog
#define glValidateProgram fiveed_glValidateProgram
#define glGetUniformLocation fiveed_glGetUniformLocation
#define glGetAttribLocation fiveed_glGetAttribLocation
#define glDisableVertexAttribArray fiveed_glDisableVertexAttribArray
#define glEnableVertexAttribArray fiveed_glEnableVertexAttribArray
#define glVertexAttribPointer fiveed_glVertexAttribPointer
#define glVertexAttribIPointer fiveed_glVertexAttribIPointer
#define glUniform2f fiveed_glUniform2f
#define glUniformMatrix2fv fiveed_glUniformMatrix2fv
#define glUniform1i fiveed_glUniform1i

#endif
