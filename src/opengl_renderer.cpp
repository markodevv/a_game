
#include "math.h"
#define GLDECL WINAPI

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
    LOAD_GL_FUNCTION(void,      UniformMatrix4fv,        GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
    LOAD_GL_FUNCTION(void,      UseProgram,              GLuint program) \
    LOAD_GL_FUNCTION(void,      VertexAttribPointer,     GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) \
    LOAD_GL_FUNCTION(void,      GenVertexArrays,         GLsizei n, GLuint *arrays) \
    LOAD_GL_FUNCTION(void,      BindVertexArray,         GLuint array) \
    LOAD_GL_FUNCTION(void,      DeleteShader,            GLuint shader) \
    LOAD_GL_FUNCTION(void,      GetProgramInfoLog,      GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    LOAD_GL_FUNCTION(void,      GetProgramiv,            GLuint program, GLenum pname, GLint *params) \


#define LOAD_GL_FUNCTION(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); extern name##proc * gl##name;
GL_FUNCTION_LIST
#undef LOAD_GL_FUNCTION



global_variable const char* vertex_shader_source =
R"(
#version 330 core
layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec2 att_uv;
layout (location = 2) in vec4 att_color;

out vec2 uv;
out vec4 color;

uniform mat4 MVP;

void main()
{
   gl_Position = MVP * vec4(att_pos.xyz, 1.0f);
   uv = att_uv;
   color = att_color;
}
)";

global_variable const char* fragment_shader_source =
R"(
#version 330 core

out vec4 frag_color;

in vec2 uv;
in vec4 color;


void main()
{
    frag_color = color;
}
)";

internal void
opengl_create_shaders(i32 shader_program)
{
    i32 success;
    char info_log[512];

    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        DEBUG_PRINT("Failed to compile vertex shader: \n%s \n", info_log);
    }

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        DEBUG_PRINT("Failed to compile fragment shader: \n%s \n", info_log);
    }

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        DEBUG_PRINT("Failed to link shader \n%s \n", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

// TODO: remove this globals



internal void
opengl_init(Renderer* ren)
{
    ren->shader_program = glCreateProgram();
    opengl_create_shaders(ren->shader_program);
    glUseProgram(ren->shader_program);

    glGenVertexArrays(1, &ren->VAO);
    glBindVertexArray(ren->VBO);

    glGenBuffers(1, &ren->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * MAX_VERTICES, NULL, GL_STATIC_DRAW);

    // Vertex layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(f32), (void*)0);
    // Enable the vertex atribute;
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(f32), (void*)(3 * sizeof(f32)));
    // Enable the vertex atribute;
    glEnableVertexAttribArray(1); 

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(f32), (void*)(5 * sizeof(f32)));
    // Enable the vertex atribute;
    glEnableVertexAttribArray(2); 

    ren->camera = {{}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};
}

internal void
opengl_draw_rectangle(Renderer* ren, vec2 position, vec2 scale, vec4 color)
{
    f32 x = position.x;
    f32 y = position.y;
    f32 w = scale.x;
    f32 h = scale.y;

    vec3 rectangle_positions[] =
    {
        {-0.5f*w+x, -0.5f*h+y, 0.0f},
        {-0.5f*w+x,  0.5f*h+y, 0.0f},
        {0.5f*w+x,  0.5f*h+y, 0.0f},

        {-0.5f*w+x, -0.5f*h+y, 0.0f},
        {0.5f*w+x, -0.5f*h+y, 0.0f},
        {0.5f*w+x,  0.5f*h+y, 0.0f},
    };


    for (sizet i = 0; i < ARRAY_COUNT(rectangle_positions); ++i)
    {
        ren->vertices_start[ren->vertex_index].position = rectangle_positions[i];
        ren->vertices_start[ren->vertex_index].color = color;
        ren->vertex_index++;
    }
}

internal void
opengl_frame_end(Renderer* ren)
{
    mat4 view = camera_transform(&ren->camera);
    mat4 projection = mat4_orthographic(1024.0f, 768.0f);
    mat4 mvp = projection * view;
    i32 mvp_loc = glGetUniformLocation(ren->shader_program, "MVP");
    glUniformMatrix4fv(mvp_loc, 1, GL_TRUE, (f32*)mvp.data);

    u32 size = sizeof(VertexData) * ren->vertex_index; 
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, ren->vertices_start); 
    glDrawArrays(GL_TRIANGLES, 0, ren->vertex_index);

    ren->vertex_index = 0;
}

