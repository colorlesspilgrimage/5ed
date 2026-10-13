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

FIVEED_GL_PTR(glDebugMessageControl, void, (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled));
FIVEED_GL_PTR(glDebugMessageCallback, void, (GLDEBUGPROC callback, const void *userParam));
FIVEED_GL_PTR(glGenVertexArrays, void, (GLsizei n, GLuint *arrays));
FIVEED_GL_PTR(glBindVertexArray, void, (GLuint array));
FIVEED_GL_PTR(glGenBuffers, void, (GLsizei n, GLuint *buffers));
FIVEED_GL_PTR(glBindBuffer, void, (GLenum target, GLuint buffer));
FIVEED_GL_PTR(glBufferData, void, (GLenum target, GLsizeiptr size, const void *data, GLenum usage));
FIVEED_GL_PTR(glBufferSubData, void, (GLenum target, GLsizeiptr offset, GLsizeiptr size, const void *data));
FIVEED_GL_PTR(glCreateShader, GLuint, (GLenum type));
FIVEED_GL_PTR(glShaderSource, void, (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length));
FIVEED_GL_PTR(glCompileShader, void, (GLuint shader));
FIVEED_GL_PTR(glGetShaderInfoLog, void, (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog));
FIVEED_GL_PTR(glDeleteShader, void, (GLuint shader));
FIVEED_GL_PTR(glCreateProgram, GLuint, (void));
FIVEED_GL_PTR(glAttachShader, void, (GLuint program, GLuint shader));
FIVEED_GL_PTR(glLinkProgram, void, (GLuint program));
FIVEED_GL_PTR(glUseProgram, void, (GLuint program));
FIVEED_GL_PTR(glGetProgramiv, void, (GLuint program, GLenum pname, GLint *params));
FIVEED_GL_PTR(glGetProgramInfoLog, void, (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog));
FIVEED_GL_PTR(glValidateProgram, void, (GLuint program));
FIVEED_GL_PTR(glGetUniformLocation, GLint, (GLuint program, const GLchar *name));
FIVEED_GL_PTR(glGetAttribLocation, GLint, (GLuint program, const GLchar *name));
FIVEED_GL_PTR(glDisableVertexAttribArray, void, (GLuint index));
FIVEED_GL_PTR(glEnableVertexAttribArray, void, (GLuint index));
FIVEED_GL_PTR(glVertexAttribPointer, void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer));
FIVEED_GL_PTR(glVertexAttribIPointer, void, (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer));
FIVEED_GL_PTR(glUniform2f, void, (GLint location, GLfloat v0, GLfloat v1));
FIVEED_GL_PTR(glUniformMatrix2fv, void, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value));
FIVEED_GL_PTR(glUniform1i, void, (GLint location, GLint v0));

#undef FIVEED_GL_PTR

#define FIVEED_LOAD_REQUIRED(name) \
    fiveed_##name = (fiveed_##name##_Function *)glXGetProcAddressARB((const GLubyte *)#name); \
    if (fiveed_##name == 0){ return false; }

#define FIVEED_LOAD_OPTIONAL(name) \
    fiveed_##name = (fiveed_##name##_Function *)glXGetProcAddressARB((const GLubyte *)#name)

// Load each entry point. Call this only after glXMakeCurrent.
// A null optional pointer is valid. This function does not exit.
internal b32
linux_gl_load_procs(void){
    FIVEED_LOAD_REQUIRED(glGenVertexArrays);
    FIVEED_LOAD_REQUIRED(glBindVertexArray);
    FIVEED_LOAD_REQUIRED(glGenBuffers);
    FIVEED_LOAD_REQUIRED(glBindBuffer);
    FIVEED_LOAD_REQUIRED(glBufferData);
    FIVEED_LOAD_REQUIRED(glBufferSubData);
    FIVEED_LOAD_REQUIRED(glCreateShader);
    FIVEED_LOAD_REQUIRED(glShaderSource);
    FIVEED_LOAD_REQUIRED(glCompileShader);
    FIVEED_LOAD_REQUIRED(glGetShaderInfoLog);
    FIVEED_LOAD_REQUIRED(glDeleteShader);
    FIVEED_LOAD_REQUIRED(glCreateProgram);
    FIVEED_LOAD_REQUIRED(glAttachShader);
    FIVEED_LOAD_REQUIRED(glLinkProgram);
    FIVEED_LOAD_REQUIRED(glUseProgram);
    FIVEED_LOAD_REQUIRED(glGetProgramiv);
    FIVEED_LOAD_REQUIRED(glGetProgramInfoLog);
    FIVEED_LOAD_REQUIRED(glValidateProgram);
    FIVEED_LOAD_REQUIRED(glGetUniformLocation);
    FIVEED_LOAD_REQUIRED(glGetAttribLocation);
    FIVEED_LOAD_REQUIRED(glDisableVertexAttribArray);
    FIVEED_LOAD_REQUIRED(glEnableVertexAttribArray);
    FIVEED_LOAD_REQUIRED(glVertexAttribPointer);
    FIVEED_LOAD_REQUIRED(glVertexAttribIPointer);
    FIVEED_LOAD_REQUIRED(glUniform2f);
    FIVEED_LOAD_REQUIRED(glUniformMatrix2fv);
    FIVEED_LOAD_REQUIRED(glUniform1i);
    FIVEED_LOAD_OPTIONAL(glDebugMessageControl);
    FIVEED_LOAD_OPTIONAL(glDebugMessageCallback);
    return true;
}

#undef FIVEED_LOAD_REQUIRED
#undef FIVEED_LOAD_OPTIONAL

// Drop every loaded pointer. Call this when context creation fails.
// A later draw must not call a pointer for a destroyed context.
internal void
linux_gl_clear_procs(void){
#define FIVEED_CLEAR(name) fiveed_##name = 0
    FIVEED_CLEAR(glDebugMessageControl);
    FIVEED_CLEAR(glDebugMessageCallback);
    FIVEED_CLEAR(glGenVertexArrays);
    FIVEED_CLEAR(glBindVertexArray);
    FIVEED_CLEAR(glGenBuffers);
    FIVEED_CLEAR(glBindBuffer);
    FIVEED_CLEAR(glBufferData);
    FIVEED_CLEAR(glBufferSubData);
    FIVEED_CLEAR(glCreateShader);
    FIVEED_CLEAR(glShaderSource);
    FIVEED_CLEAR(glCompileShader);
    FIVEED_CLEAR(glGetShaderInfoLog);
    FIVEED_CLEAR(glDeleteShader);
    FIVEED_CLEAR(glCreateProgram);
    FIVEED_CLEAR(glAttachShader);
    FIVEED_CLEAR(glLinkProgram);
    FIVEED_CLEAR(glUseProgram);
    FIVEED_CLEAR(glGetProgramiv);
    FIVEED_CLEAR(glGetProgramInfoLog);
    FIVEED_CLEAR(glValidateProgram);
    FIVEED_CLEAR(glGetUniformLocation);
    FIVEED_CLEAR(glGetAttribLocation);
    FIVEED_CLEAR(glDisableVertexAttribArray);
    FIVEED_CLEAR(glEnableVertexAttribArray);
    FIVEED_CLEAR(glVertexAttribPointer);
    FIVEED_CLEAR(glVertexAttribIPointer);
    FIVEED_CLEAR(glUniform2f);
    FIVEED_CLEAR(glUniformMatrix2fv);
    FIVEED_CLEAR(glUniform1i);
#undef FIVEED_CLEAR
}

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
