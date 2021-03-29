#if !defined(OPENGL_RENDERER_H)
#define OPENGL_RENDERER_H

/*
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#define GL_ARRAY_BUFFER                   0x8892 // Acquired from:
#define GL_ARRAY_BUFFER_BINDING           0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_LINK_STATUS                    0x8B82
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_VERTEX_SHADER                  0x8B31


#define GL_FUNCTION_LIST \
    LOAD_GL_FUNCTION(void,      AttachShader,            GLuint program, GLuint shader) \
    LOAD_GL_FUNCTION(void,      BindBuffer,              GLenum target, GLuint buffer) \
    LOAD_GL_FUNCTION(void,      BindFramebuffer,         GLenum target, GLuint framebuffer) \
    LOAD_GL_FUNCTION(void,      BufferData,              GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) \
    LOAD_GL_FUNCTION(void,      BufferSubData,           GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data) \
    LOAD_GL_FUNCTION(GLenum,    CheckFramebufferStatus,  GLenum target) \
    LOAD_GL_FUNCTION(void,      ClearBufferfv,           GLenum buffer, GLint drawbuffer, const GLfloat * value) \
    LOAD_GL_FUNCTION(void,      CompileShader,           GLuint shader) \
    LOAD_GL_FUNCTION(GLuint,    CreateProgram,           void) \
    LOAD_GL_FUNCTION(GLuint,    CreateShader,            GLenum type) \
    LOAD_GL_FUNCTION(void,      DeleteBuffers,           GLsizei n, const GLuint *buffers) \
    LOAD_GL_FUNCTION(void,      DeleteFramebuffers,      GLsizei n, const GLuint *framebuffers) \
    LOAD_GL_FUNCTION(void,      EnableVertexAttribArray, GLuint index) \
    LOAD_GL_FUNCTION(void,      DrawBuffers,             GLsizei n, const GLenum *bufs) \
    LOAD_GL_FUNCTION(void,      FramebufferTexture2D,    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) \
    LOAD_GL_FUNCTION(void,      GenBuffers,              GLsizei n, GLuint *buffers) \
    LOAD_GL_FUNCTION(void,      GenFramebuffers,         GLsizei n, GLuint * framebuffers) \
    LOAD_GL_FUNCTION(GLint,     GetAttribLocation,       GLuint program, const GLchar *name) \
    LOAD_GL_FUNCTION(void,      GetShaderInfoLog,        GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    LOAD_GL_FUNCTION(void,      GetShaderiv,             GLuint shader, GLenum pname, GLint *params) \
    LOAD_GL_FUNCTION(GLint,     GetUniformLocation,      GLuint program, const GLchar *name) \
    LOAD_GL_FUNCTION(void,      LinkProgram,             GLuint program) \
    LOAD_GL_FUNCTION(void,      ShaderSource,            GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) \
    LOAD_GL_FUNCTION(void,      Uniform1i,               GLint location, GLint v0) \
    LOAD_GL_FUNCTION(void,      Uniform1f,               GLint location, GLfloat v0) \
    LOAD_GL_FUNCTION(void,      Uniform2f,               GLint location, GLfloat v0, GLfloat v1) \
    LOAD_GL_FUNCTION(void,      Uniform4f,               GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) \
    LOAD_GL_FUNCTION(void,      Uniform1fv,              GLint location, GLsizei size, const GLfloat* value) \
    LOAD_GL_FUNCTION(void,      Uniform2fv,              GLint location, GLsizei size, const GLfloat* value) \
    LOAD_GL_FUNCTION(void,      Uniform3fv,              GLint location, GLsizei size, const GLfloat* value) \
    LOAD_GL_FUNCTION(void,      Uniform4fv,              GLint location, GLsizei size, const GLfloat* value) \
    LOAD_GL_FUNCTION(void,      UniformMatrix4fv,        GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
    LOAD_GL_FUNCTION(void,      UseProgram,              GLuint program) \
    LOAD_GL_FUNCTION(void,      VertexAttribPointer,     GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) \
    LOAD_GL_FUNCTION(void,      GenVertexArrays,         GLsizei n, GLuint *arrays) \
    LOAD_GL_FUNCTION(void,      BindVertexArray,         GLuint array) \
    LOAD_GL_FUNCTION(void,      DeleteShader,            GLuint shader) \
    LOAD_GL_FUNCTION(void,      GetProgramInfoLog,       GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    LOAD_GL_FUNCTION(void,      GetProgramiv,            GLuint program, GLenum pname, GLint *params) \
    LOAD_GL_FUNCTION(void,      ActiveTexture,           GLenum texture) \




#define LOAD_GL_FUNCTION(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); extern name##proc * gl##name;
GL_FUNCTION_LIST
#undef LOAD_GL_FUNCTION
*/
#define GL_ARRAY_BUFFER                   0x8892 // Acquired from:
#define GL_ARRAY_BUFFER_BINDING           0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_LINK_STATUS                    0x8B82
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_VERTEX_SHADER                  0x8B31

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
#define GLDECL 
#define OpenGLFunction(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); name##proc * ##name

OpenGLFunction(void,     glAttachShader,            GLuint program, GLuint shader);
OpenGLFunction(void,     glBindBuffer,              GLenum target, GLuint buffer);
OpenGLFunction(void,     glBindFramebuffer,         GLenum target, GLuint framebuffer);
OpenGLFunction(void,     glBufferData,              GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
OpenGLFunction(void,     glBufferSubData,           GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);
OpenGLFunction(GLenum,   glCheckFramebufferStatus,  GLenum target);
OpenGLFunction(void,     glClearBufferfv,           GLenum buffer, GLint drawbuffer, const GLfloat * value);
OpenGLFunction(void,     glCompileShader,           GLuint shader);
OpenGLFunction(GLuint,   glCreateProgram,           void);
OpenGLFunction(GLuint,   glCreateShader,            GLenum type);
OpenGLFunction(void,     glDeleteBuffers,           GLsizei n, const GLuint *buffers);
OpenGLFunction(void,     glDeleteFramebuffers,      GLsizei n, const GLuint *framebuffers);
OpenGLFunction(void,     glEnableVertexAttribArray, GLuint index);
OpenGLFunction(void,     glDrawBuffers,             GLsizei n, const GLenum *bufs);
OpenGLFunction(void,     glFramebufferTexture2D,    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
OpenGLFunction(void,     glGenBuffers,              GLsizei n, GLuint *buffers);
OpenGLFunction(void,     glGenFramebuffers,         GLsizei n, GLuint * framebuffers);
OpenGLFunction(GLint,    glGetAttribLocation,       GLuint program, const GLchar *name);
OpenGLFunction(void,     glGetShaderInfoLog,        GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
OpenGLFunction(void,     glGetShaderiv,             GLuint shader, GLenum pname, GLint *params);
OpenGLFunction(GLint,    glGetUniformLocation,      GLuint program, const GLchar *name);
OpenGLFunction(void,     glLinkProgram,             GLuint program);
OpenGLFunction(void,     glShaderSource,            GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length);
OpenGLFunction(void,     glUniform1i,               GLint location, GLint v0);
OpenGLFunction(void,     glUniform1f,               GLint location, GLfloat v0);
OpenGLFunction(void,     glUniform2f,               GLint location, GLfloat v0, GLfloat v1);
OpenGLFunction(void,     glUniform4f,               GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
OpenGLFunction(void,     glUniform1fv,              GLint location, GLsizei size, const GLfloat* value);
OpenGLFunction(void,     glUniform2fv,              GLint location, GLsizei size, const GLfloat* value);
OpenGLFunction(void,     glUniform3fv,              GLint location, GLsizei size, const GLfloat* value);
OpenGLFunction(void,     glUniform4fv,              GLint location, GLsizei size, const GLfloat* value);
OpenGLFunction(void,     glUniformMatrix4fv,        GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
OpenGLFunction(void,     glUseProgram,              GLuint program);
OpenGLFunction(void,     glVertexAttribPointer,     GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
OpenGLFunction(void,     glGenVertexArrays,         GLsizei n, GLuint *arrays);
OpenGLFunction(void,     glBindVertexArray,         GLuint array);
OpenGLFunction(void,     glDeleteShader,            GLuint shader);
OpenGLFunction(void,     glGetProgramInfoLog,       GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
OpenGLFunction(void,     glGetProgramiv,            GLuint program, GLenum pname, GLint *params);
OpenGLFunction(void,     glActiveTexture,           GLenum texture);

#endif
