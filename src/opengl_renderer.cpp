
#include "math.h"
#define GLDECL WINAPI

#include "renderer.h"

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

global_variable const char* vertex_shader_3D =
R"(
#version 330 core
layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec3 att_normal;
layout (location = 2) in vec2 att_uv;
layout (location = 3) in vec4 att_color;
layout (location = 4) in float att_tex_id;

out vec3 frag_pos;
out vec3 normal;
out vec2 uv;
out vec4 color;

uniform mat4 u_MVP;
uniform mat4 u_normal_trans;

void main()
{
   uv = att_uv;
   color = att_color;
   frag_pos = att_pos;
   normal = mat3(u_normal_trans) * att_normal;

   gl_Position = u_MVP *  vec4(att_pos, 1.0f);
}
)";

global_variable const char* fragment_shader_3D =
R"(
#version 330 core

out vec4 frag_color;

in vec3 frag_pos;
in vec3 normal;
in vec2 uv;
in vec4 color;

uniform vec3 u_view;
uniform vec3 u_light_pos;

float ambient = 0.1f;
float specular = 0.5f;
vec4 light_color = {1.0f, 1.0f, 1.0f, 1.0f};

void main()
{
    vec3 norm = normalize(normal);
    vec4 ambient = light_color * ambient;

    vec3 light_dir = normalize(u_light_pos - frag_pos);
    vec4 diffuse = light_color * max(dot(light_dir, norm), 0.0f);

    vec3 view_dir = normalize(u_view - frag_pos);
    vec3 reflected = reflect(light_dir, norm);

    vec4 spec = light_color * pow(max(dot(reflected, view_dir), 0.0f), 32) * specular;

    frag_color = color * (diffuse + ambient + spec);
}
)";

global_variable const char* vertex_shader_2D =
R"(
#version 330 core
layout (location = 0) in vec2 att_pos;
layout (location = 1) in vec2 att_uv;
layout (location = 2) in vec4 att_color;
layout (location = 3) in float att_tex_id;

out vec2 uv;
out vec4 color;
out float tex_id;

uniform mat4 u_viewproj;

void main()
{
   gl_Position = u_viewproj * vec4(att_pos, -1.0f, 1.0f);
   uv = att_uv;
   color = att_color;
   tex_id = att_tex_id;
}
)";

global_variable const char* fragment_shader_2D =
R"(
#version 330 core

out vec4 frag_color;

in vec2 uv;
in vec4 color;
in float tex_id;

void main()
{
    frag_color = color;
}
)";

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

global_variable vec3 cube_vertices[] = {
   {-0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   {-0.5f,  0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 

   {-0.5f, -0.5f,  0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 

   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 

   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 

   {-0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f, -0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   { 0.5f, -0.5f,  0.5f}, 
   {-0.5f, -0.5f,  0.5f}, 
   {-0.5f, -0.5f, -0.5f}, 

   {-0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f, -0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   { 0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f,  0.5f}, 
   {-0.5f,  0.5f, -0.5f}, 
};

global_variable vec3 cube_normals[] =
{
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 
   { 0.0f,  0.0f,  -1.0f}, 

   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 
   { 0.0f,  0.0f,  1.0f}, 

   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 
   { -1.0f,  0.0f,  0.0f}, 

   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 
   { 1.0f,  0.0f,  0.0f}, 

   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 
   { 0.0f,  -1.0f,  0.0f}, 

   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
   { 0.0f,  1.0f,  0.0f}, 
};


internal void
opengl_compile_shaders(i32 shader_program, const char* vertex_shader, const char* fragment_shader)
{
    i32 success;
    char *info_log = (char*)malloc(sizeof(char) * 5120);

    u32 vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vs, 5120, NULL, info_log);
        DEBUG_PRINT("Failed to compile vertex shader: \n%s \n", info_log);
    }
    else
    {
        DEBUG_PRINT("Compiled Vertex Shader:\n%s\n", vertex_shader);
    }

    u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fs, 5120, NULL, info_log);
        DEBUG_PRINT("Failed to compile fragment shader: \n%s \n", info_log);
    }
    else
    {
        DEBUG_PRINT("Compiled Fragment Shader:\n%s\n", fragment_shader);
    }

    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 5120, NULL, info_log);
        DEBUG_PRINT("Failed to link shader \n%s \n", info_log);
    }
    else
    {
        DEBUG_PRINT("Successfuly linked shader program\n");
    }


    glDeleteShader(vs);
    glDeleteShader(fs);
    free(info_log);
}


internal void
opengl_init(Renderer* ren)
{
    glEnable(GL_DEPTH_TEST);
    ren->shader_program_3D = glCreateProgram();
    ren->shader_program_2D = glCreateProgram();

    opengl_compile_shaders(ren->shader_program_3D, vertex_shader_3D, fragment_shader_3D);
    opengl_compile_shaders(ren->shader_program_2D, vertex_shader_2D, fragment_shader_2D);


    glGenVertexArrays(1, &ren->VAO);

    glGenBuffers(1, &ren->VBO);

    glBindVertexArray(ren->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * MAX_VERTICES, NULL, GL_STATIC_DRAW);
    // Vertex layout
    i32 stride = sizeof(f32) * 13;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1); 

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2); 

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
    glEnableVertexAttribArray(3); 

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)(12*sizeof(f32)));
    glEnableVertexAttribArray(4); 

    glBindVertexArray(0);

    // 2D 
    ren->camera = {{0.0f, 0.0f, 200.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};

    glGenVertexArrays(1, &ren->VAO_2D);
    glGenBuffers(1, &ren->VBO_2D);

    glBindVertexArray(ren->VAO_2D);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO_2D);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData2D) * MAX_VERTICES, NULL, GL_STATIC_DRAW);

    stride = sizeof(f32) * 9;
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2*sizeof(f32)));
    glEnableVertexAttribArray(1); 

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4*sizeof(f32)));
    glEnableVertexAttribArray(2); 

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
    glEnableVertexAttribArray(3); 

    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &ren->font_texture_id);
    glBindTexture(GL_TEXTURE_2D, ren->font_texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, ren->temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindVertexArray(0);
}

internal void
opengl_draw_rectangle(Renderer* ren, vec2 position, vec2 scale, vec4 color)
{
    f32 x = position.x;
    f32 y = position.y;
    f32 w = scale.x;
    f32 h = scale.y;

    vec2 rectangle_vertices[] =
    {
        {-0.5f*w+x, -0.5f*h+y},
        {-0.5f*w+x,  0.5f*h+y},
        { 0.5f*w+x,  0.5f*h+y},

        {-0.5f*w+x, -0.5f*h+y},
        { 0.5f*w+x, -0.5f*h+y},
        { 0.5f*w+x,  0.5f*h+y},
    };


    for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
    {
        ren->vertices_2D[ren->vertex_index_2D].position = rectangle_vertices[i];
        ren->vertices_2D[ren->vertex_index_2D].color = color;
        ren->vertices_2D[ren->vertex_index_2D].uv = {};
        ren->vertices_2D[ren->vertex_index_2D].texture_id = -1.0f;
        ren->vertex_index_2D++;
    }
}

internal void
opengl_draw_cube(Renderer* ren, vec3 position, vec3 scale, vec4 color)
{
    for (sizet i = 0; i < ARRAY_COUNT(cube_vertices); i++)
    {
        ren->vertices_start[ren->vertex_index].position = cube_vertices[i];
        ren->vertices_start[ren->vertex_index].normal = cube_normals[i];
        ren->vertices_start[ren->vertex_index].color = color;
        ren->vertex_index++;
    }
}

internal inline i32
opengl_get_uniform_location(i32 shader, char* uniform)
{
    i32 loc = glGetUniformLocation(shader, uniform);
    if (loc == -1)
    {
        DEBUG_PRINT("Failed to get uniform location: %s\n", uniform);
    }
    return loc;

}

internal void
opengl_frame_end(Renderer* ren)
{
    {
        glUseProgram(ren->shader_program_3D);
        local_persist f32 light_z = 10.0f;
         
        mat4 view = camera_transform(&ren->camera);
        mat4 projection = mat4_perspective(1024.0f, 768.0f, 120.0f, 0.1f, 1000.0f);
        //mat4 projection = mat4_orthographic(1024.0f, 768.0f);
        mat4 model = mat4_scale({100, 100, 100});
        mat4 mvp = projection * view * model;
        mat4 normal_transform = mat4_transpose(mat4_inverse(model));
        light_z += 0.001f;
        f32 z = sinf(light_z) * 200.0f;
        vec3 light_pos = {300.0f, 200.0f, z};

        b8 do_transpose = true;
        i32 mvp_loc, light_loc, view_loc, normal_loc;

        mvp_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_MVP");
        glUniformMatrix4fv(mvp_loc, 1, do_transpose, (f32*)mvp.data);

        light_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_light_pos");
        glUniform3fv(light_loc, 1, (f32*)light_pos.data);

        view_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_view");
        glUniform3fv(view_loc, 1, (f32*)ren->camera.position.data);

        normal_loc =  opengl_get_uniform_location(ren->shader_program_3D, "u_normal_trans");
        glUniformMatrix4fv(normal_loc, 1, do_transpose, (f32*)normal_transform.data);

        u32 size = sizeof(VertexData) * ren->vertex_index; 

        glBindVertexArray(ren->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);

        glBufferSubData(GL_ARRAY_BUFFER, 0, size, ren->vertices_start); 
        glDrawArrays(GL_TRIANGLES, 0, ren->vertex_index);
        ren->vertex_index = 0;

    }
    {
        // NOTE(marko): 2D rendering
        glUseProgram(ren->shader_program_2D);

        mat4 view = camera_transform(&ren->camera);
        //mat4 projection = mat4_perspective(1024.0f, 768.0f, 120.0f, 0.1f, 1000.0f);
        mat4 projection = mat4_orthographic(1024.0f, 768.0f, 0.1f, 1000.0f);
        mat4 viewproj = projection;
        b8 do_transpose = true;

        i32 vp_loc = opengl_get_uniform_location(ren->shader_program_2D, "u_viewproj");
        glUniformMatrix4fv(vp_loc, 1, do_transpose, (f32*)viewproj.data);
        
        glBindVertexArray(ren->VAO_2D);
        glBindBuffer(GL_ARRAY_BUFFER, ren->VBO_2D);
        u32 size = sizeof(VertexData2D) * ren->vertex_index_2D;
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, ren->vertices_2D); 
        glDrawArrays(GL_TRIANGLES, 0, ren->vertex_index_2D);
        ren->vertex_index_2D = 0;
    }

}

