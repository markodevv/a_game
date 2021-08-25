#include "generated/renderer.h"
#include "renderer.cpp"


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
OpenGLFunction(void,     glUniform1iv,              GLint location, GLsizei size, const GLint* value);
OpenGLFunction(void,     glUniform2iv,              GLint location, GLsizei size, const GLint* value);
OpenGLFunction(void,     glUniform3iv,              GLint location, GLsizei size, const GLint* value);
OpenGLFunction(void,     glUniform4iv,              GLint location, GLsizei size, const GLint* value);
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
OpenGLFunction(void,     glGetActiveUniform,	    GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
OpenGLFunction(void,     glGetActiveUniformBlockiv,	GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
OpenGLFunction(void,     glGetActiveUniformName,    GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
OpenGLFunction(void,     glActiveTexture,           GLenum texture);
OpenGLFunction(void,     glGenTextures,             GLsizei n, GLuint *textures);
OpenGLFunction(void,     glBindTexture,         	GLenum target, GLuint texture);
OpenGLFunction(void,     glPixelStorei,	            GLenum pname, GLint param);
OpenGLFunction(void,     glTexParameteri,	        GLenum target, GLenum pname, GLint param);
OpenGLFunction(void,     glTexImage2D,              GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data);
OpenGLFunction(void,     glEnable,                  GLenum cap);
OpenGLFunction(void,     glDisable,                 GLenum cap);
OpenGLFunction(void,     glBlendFunc,               GLenum sfactor, GLenum dfactor);
OpenGLFunction(void,     glDepthFunc,               GLenum func);
OpenGLFunction(void,     glDrawElements,            GLenum  	mode, GLsizei  	count, GLenum  	type, const GLvoid *  	indices);
OpenGLFunction(void,     glClearColor,              GLclampf  	red, GLclampf  	green, GLclampf  	blue, GLclampf  	alpha);
OpenGLFunction(void,     glClear,                   GLbitfield mask);
OpenGLFunction(void,     glViewport,             	GLint x, GLint y, GLsizei width, GLsizei height);
OpenGLFunction(void,     glDrawArrays,          	GLenum  	mode, GLint  	first, GLsizei  	count);



#include "shaders.glsl"


internal inline i32
OpenGLGetUniformLocation(i32 shader, char* uniform)
{
    i32 loc = glGetUniformLocation(shader, uniform);
    if (loc == -1)
    {
        LogM("Failed to get uniform location: %s\n", uniform);
    }
    return loc;
    
}

internal UniformTypeInfo
GetUniformTypeInfo(GLenum type)
{
    UniformTypeInfo result = {};
    switch(type)
    {
        case GL_FLOAT:
        {
            result = {UNIFORM_F32, sizeof(f32)};
        }break;
        case GL_FLOAT_VEC2:
        {
            result = {UNIFORM_VEC2, sizeof(vec2)};
        }break;
        case GL_FLOAT_VEC3:
        {
            result = {UNIFORM_VEC3, sizeof(vec3)};
        }break;
        case GL_FLOAT_VEC4:
        {
            result = {UNIFORM_VEC4, sizeof(vec4)};
        }break;
        case GL_SAMPLER_2D:
        {
            result = {UNIFORM_TEXTURE2D, 0};
        }break;
        case GL_FLOAT_MAT4:
        {
            result = {UNIFORM_MAT4, sizeof(mat4)};
        }break;
        default:
        {
            LogM("Unknown type uniform type %d\n", type);
        } break;
    }
    
    return result;
    
}


internal Shader
OpenGLCreateShader(MemoryArena* arena, const char* vertex_shader, const char* fragment_shader)
{
    i32 shader_program = glCreateProgram();
    
    i32 success;
    char *info_log = (char*)malloc(sizeof(char) * 5120);
    
    u32 vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    
    if (!success)
    {
        glGetShaderInfoLog(vs, 5120, NULL, info_log);
        LogM("Failed to compile vertex shader: \n%s", info_log);
    }
    
    u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    
    if (!success)
    {
        glGetShaderInfoLog(fs, 5120, NULL, info_log);
        LogM("Failed to compile fragment shader: \n%s", info_log);
    }
    
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);
    
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 5120, NULL, info_log);
        LogM("Failed to link shader \n%s", info_log);
    }
    glUseProgram(shader_program);
    
    char texture_uniform_name[] = {"u_textures[0]"};
    i32 loc = OpenGLGetUniformLocation(shader_program, texture_uniform_name);
    
    if (loc != -1)
    {
        for (u32 tex_id = 0; tex_id < 9; ++tex_id)
        {
            texture_uniform_name[11] = '0' + tex_id;
            loc = OpenGLGetUniformLocation(shader_program, texture_uniform_name);
            glUniform1i(loc, tex_id);
        }
    }
    
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    free(info_log);
    
    Shader shader = {};
    shader.bind_id = shader_program;
    i32 max_name_len = 0;
    
    glGetProgramiv(shader_program, GL_ACTIVE_UNIFORMS, &shader.num_uniforms);
    glGetProgramiv(shader_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
    
    for (i32 uniform_id= 0; uniform_id < shader.num_uniforms; ++uniform_id)
    {
        char name[128];
        // TODO: 
        // u_viewproj should be set as global uniform shared between shaders
        // using UBOs..
        
        GLenum gl_type = 0;
        i32 length;
        i32 type_size;
        
        glGetActiveUniform(shader_program,
                           uniform_id,
                           ArrayCount(name),
                           &length,
                           &type_size,
                           &gl_type,
                           name);
        
        Uniform* uniform = AddUniform(&shader, name, arena);
        
        uniform->type_info = GetUniformTypeInfo(gl_type);
        uniform->data = PushMemory(arena, u8, uniform->type_info.size);
    }
    
    return shader;
}


internal void
OpenGLLoadTexture(Sprite* sprite, u32 slot)
{
    u32 texture_id = 0;
    
    glActiveTexture(GL_TEXTURE0 + slot);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    u32 internal_format = 0;
    u32 external_format = 0;
    
    if (sprite->channels == 4)
    {
        internal_format = GL_RGBA8;
        external_format = GL_RGBA;
    }
    else if (sprite->channels == 3)
    {
        internal_format = GL_RGB8;
        external_format = GL_RGB;
    }
    else if (sprite->channels == 1)
    {
        internal_format = GL_R8;
        external_format = GL_RED;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
    }
    else
    {
        Assert(false);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0, 
                 internal_format,
                 sprite->width,
                 sprite->height,
                 0,
                 external_format,
                 GL_UNSIGNED_BYTE, 
                 sprite->data);
    
    sprite->id = texture_id;
}



internal void
OpenGLLoadMesh(Mesh* mesh)
{
    u32 stride = sizeof(f32) * 9;
    
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    
    glGenBuffers(1, &mesh->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    u32 size = mesh->num_indices * sizeof(u32);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void*)mesh->indices, GL_STATIC_DRAW);
    
    size = sizeof(Vertex3D) * mesh->num_vertices;
    glBufferData(GL_ARRAY_BUFFER, size, (void*)mesh->vertices, GL_STATIC_DRAW);
    
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 
    
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1); 
    
    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2); 
    
    // texture id
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
    glEnableVertexAttribArray(3); 
    
    glBindVertexArray(0);
    
}

Vertex3D cube_vertices[] = 
{
    {{-0.5f, -0.5f, -0.5f}, {0, 0, 1}},
    {{0.5f, -0.5f, -0.5f}, {1, 0, 0}},
    {{0.5f, 0.5f, -0.5f}, {0, 0, -1}},
    {{-0.5f, 0.5f, -0.5f}, {-1, 0, 0}},
    {{-0.5f, -0.5f, 0.5f}, {0, 1, 0}},
    {{0.5f, -0.5f, 0.5f}, {0, -1, 0}},
    {{0.5f, 0.5f, 0.5f}},
    {{-0.5f, 0.5f, 0.5f}},
};

u32 cube_indices[] =
{
    0, 1, 3, 3, 1, 2,
    1, 5, 2, 2, 5, 6,
    5, 4, 6, 6, 4, 7,
    4, 0, 7, 7, 0, 3,
    3, 2, 7, 7, 2, 6,
    4, 5, 0, 0, 5, 1
};



internal void
OpenGLInit(Renderer2D* ren)
{
    Shader* shaders = ren->assets->shaders;
    MemoryArena* arena = &ren->assets->arena;
    
    shaders[SHADER_ID_NORMAL] = OpenGLCreateShader(arena, basic_vertex_shader, basic_fragment_shader);
    shaders[SHADER_ID_HUE_QUAD] = OpenGLCreateShader(arena, basic_vertex_shader, hue_quad_shader);
    shaders[SHADER_ID_SB_QUAD] = OpenGLCreateShader(arena, basic_vertex_shader, sat_brigh_quad_shader);
    shaders[SHADER_ID_BASIC_3D] = OpenGLCreateShader(arena, vertex_shader_3D, fragment_shader_3D);
    
    ren->shader_program_3D = shaders[SHADER_ID_BASIC_3D].bind_id;
    
    
    glGenVertexArrays(1, &ren->VAO);
    glGenBuffers(1, &ren->VBO);
    glGenBuffers(1, &ren->EBO);
    
    
    glBindVertexArray(ren->VAO);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ren->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * MAX_INDICES, NULL, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_VERTICES, NULL, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ren->EBO);
    
    u32 stride = sizeof(f32) * 6 + sizeof(u8) * 4;
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1); 
    
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(5*sizeof(f32)));
    glEnableVertexAttribArray(2); 
    
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(5*sizeof(f32) + 4*(sizeof(u8))));
    glEnableVertexAttribArray(3); 
    
    
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glAlphaFunc(GL_GREATER, 0.5);
    
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glBindVertexArray(0);
    
    ren->mesh.vertices = cube_vertices,
    ren->mesh.num_vertices = ArrayCount(cube_vertices);
    
    ren->mesh.indices = cube_indices;
    ren->mesh.num_indices = ArrayCount(cube_vertices);
    
    ren->mesh.material = CreateMaterial(V3(0.2f), V3(0.1f), V3(0.5f), 1.0f);
    
    OpenGLLoadMesh(&ren->mesh);
    
    ren->light = CreateLight(V3(0, 0, -200), V3(0.3f), V3(0.5f), V3(0.1f));
    
#if 0
    char texture_uniform_name[] = {"u_textures[-]"};
    for (u32 tex_id = 0; tex_id < 9; ++tex_id)
    {
        texture_uniform_name[11] = '0' + tex_id;
        i32 loc = OpenGLGetUniformLocation(ren->shader_program_3D, texture_uniform_name);
        glUniform1i(loc, tex_id);
    }
    
    
    glUseProgram(ren->shader_program_3D);
    glGenVertexArrays(1, &ren->VAO);
    
    glGenBuffers(1, &ren->VBO);
    
    glBindVertexArray(ren->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * MAX_VERTICES, NULL, GL_STATIC_DRAW);
    // Vertex layout
    i32 stride = sizeof(f32) * 9;
    
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 
    
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1); 
    
    // uv*sizeof(f32)));
    glEnableVertexAttribArray(2); 
    
    // texture id
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
    glEnableVertexAttribArray(3); 
    
    glBindVertexArray(0);
    
    
#endif
};


// NOTE: Dumb sort
internal void
SortRenderGroup(RenderGroup* render_group)
{
    SortElement* sort_entries = SortElementStart(render_group);
    
    for(i64 i = 1; i < (i64)render_group->sort_element_count; ++i)
    {
        SortElement sort_element = sort_entries[i];
        
        i64 j = i - 1;
        while(j >= 0 && sort_entries[j].key > sort_element.key)
        {
            sort_entries[j+1] = sort_entries[j];
            --j;
        }
        sort_entries[j+1] = sort_element;
    }
}

internal void
UploadShaderUniform(u32 shader_program, Uniform* uniform)
{
    // TODO: should cache this when we load shader
    i32 location = OpenGLGetUniformLocation(shader_program, uniform->name);
    
    switch (uniform->type_info.type)
    {
        case UNIFORM_F32:
        {
            glUniform1fv(location, 1, (f32*)uniform->data);
        } break;
        case UNIFORM_VEC2:
        {
            glUniform2fv(location, 1, (f32*)uniform->data);
        } break;
        case UNIFORM_VEC3:
        {
            glUniform3fv(location, 1, (f32*)uniform->data);
        } break;
        case UNIFORM_VEC4:
        {
            glUniform4fv(location, 1, (f32*)uniform->data);
        } break;
        case UNIFORM_MAT4:
        {
            glUniformMatrix4fv(location, 1, true, (f32*)uniform->data);
        } break;
        case UNIFORM_TEXTURE2D:
        {
        } break;
        
        default:
        {
            Assert(false);
        } break;
    }
    
}

internal void
OpenGLDraw(Renderer2D* ren, RenderSetup* setup, ShaderId shader_id)
{
    Shader* shader = GetShader(ren->assets, shader_id);
    u32 shader_program = shader->bind_id;
    
    glUseProgram(shader_program);
    
    u32 num_vertices = ren->vertex_count;
    u32 num_indices = ren->indices_count;
    
    
    u32 vertices_size = num_vertices * sizeof(Vertex);
    u32 indices_size = num_indices * sizeof(u32);
    
    i32 vp_loc = OpenGLGetUniformLocation(shader_program, "u_viewproj");
    mat4 cam_mat = CameraTransform(&setup->camera);
    mat4 viewproj = setup->projection * cam_mat;
    glUniformMatrix4fv(vp_loc, 1, true, &viewproj.rows[0].x);
    
    
    Uniform* uniform = shader->uniforms;
    while (uniform)
    {
        if (!StringMatch(uniform->name, "u_viewproj"))
        {
            UploadShaderUniform(shader_program, uniform);
        }
        uniform = uniform->next;
    }
    
    
    glBindVertexArray(ren->VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ren->EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_size, ren->indices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_size, ren->vertices);
    
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
    
    ren->vertex_count = 0;
    ren->indices_count = 0;
}

internal void
OpenGLSetLightUniform(u32 shader_program, Light* light)
{
    i32 amb, spec, diff;
    amb = OpenGLGetUniformLocation(shader_program, "u_light.ambient");
    spec = OpenGLGetUniformLocation(shader_program, "u_light.specular");
    diff = OpenGLGetUniformLocation(shader_program, "u_light.diffuse");
    
    glUniform3fv(amb, 1, &light->ambient.x);
    glUniform3fv(spec, 1, &light->specular.x);
    glUniform3fv(diff, 1, &light->diffuse.x);
}


internal void
OpenGLSetMaterialUniform(u32 shader, Material* material)
{
    i32 amb, spec, diff, shin, tex;
    amb = OpenGLGetUniformLocation(shader, "u_material.ambient");
    spec = OpenGLGetUniformLocation(shader, "u_material.specular");
    diff = OpenGLGetUniformLocation(shader, "u_material.diffuse");
    shin = OpenGLGetUniformLocation(shader, "u_material.shininess");
    tex =  OpenGLGetUniformLocation(shader, "u_material.has_texture");
    
    glUniform3fv(amb, 1, &material->ambient.x);
    glUniform3fv(spec, 1, &material->specular.x);
    glUniform3fv(diff, 1, &material->diffuse.x);
    glUniform1f(shin, material->shininess);
}

internal void
OpenGLDrawMesh(Renderer2D* ren, Mesh* mesh)
{
    glUseProgram(ren->shader_program_3D);
    mat4 transform = Mat4Translate(V3(1.0f)) * Mat4Scale(V3(100.0f));
    mat4 projection = Mat4Perspective(ren->screen_width,
                                      ren->screen_height,
                                      90.0f,
                                      1.0f,
                                      1000.0f);
    
    
    mat4 view = CameraTransform(ren->camera);
    mat4 vp = projection * view;
    mat4 normal_transform = Mat4Transpose(Mat4Inverse(transform));
    b8 do_transpose = true;
    
    i32 vp_loc = OpenGLGetUniformLocation(ren->shader_program_3D, "u_viewproj");
    glUniformMatrix4fv(vp_loc, 1, do_transpose, (f32*)&vp.rows[0].x);
    
    
    i32 transform_loc = OpenGLGetUniformLocation(ren->shader_program_3D, "u_transform");
    glUniformMatrix4fv(transform_loc, 1, do_transpose, (f32*)&transform.rows[0].x);
    
    i32 normal_loc =  OpenGLGetUniformLocation(ren->shader_program_3D, "u_normal_trans");
    glUniformMatrix4fv(normal_loc, 1, do_transpose, (f32*)&normal_transform.rows[0].x);
    
    
    OpenGLSetMaterialUniform(ren->shader_program_3D, &mesh->material);
    OpenGLSetLightUniform(ren->shader_program_3D, &ren->light);
    // TODO(Marko): textures
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    
    
    glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);
}

internal void
OpenGLEndFrame(Renderer2D* ren)
{
    PROFILE_FUNCTION();
    
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    local_persist i32 w, h;
    if (w != ren->screen_width || h != ren->screen_height)
    {
        w = ren->screen_width;
        h = ren->screen_height;
        glViewport(0, 0, w, h);
    }
    
    
    // NOTE: this is a default white texture 
    // Sprite* white_sprite = GetLoadedSprite(ren->assets, ren->white_sprite);
    // if (!white_sprite->loaded_to_gpu)
    // {
    // Assert(ren->slot == 0);
    // white_sprite->id = OpenGLLoadTexture(white_sprite, ren->slot);
    // white_sprite->slot = ren->slot;
    // ++ren->slot;
    // }
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, white_sprite->id);
    
    if (ren->assets->num_queued_sprites)
    {
        for (u32 i = 0; i < ren->assets->num_queued_sprites; ++i)
        {
            SpriteHandle handle = ren->assets->loaded_sprite_queue[i];
            Sprite* sprite = GetLoadedSprite(ren->assets, handle);
            OpenGLLoadTexture(sprite, ren->slot);
            sprite->slot = ren->slot;
            ++ren->slot;
            
            glActiveTexture(GL_TEXTURE0 + sprite->slot);
            glBindTexture(GL_TEXTURE_2D, sprite->id);
        }
        ren->assets->num_queued_sprites = 0;
    }
    
    u32 header_size = sizeof(RenderEntryHeader);
    
    RenderGroup* render_group = ren->render_groups;
    ShaderId current_shader = SHADER_ID_NORMAL;
    
    
    for (u32 group_id = 0; group_id < ren->render_group_count; ++group_id)
    {
        SortRenderGroup(render_group);
        
        SortElement* sort_element = SortElementStart(render_group);
        
        for (u32 element_index = 0; 
             element_index < render_group->sort_element_count;
             ++element_index, ++sort_element)
        {
            u8* base = (render_group->push_buffer_base + sort_element->entry_offset);
            
            switch (((RenderEntryHeader *)base)->entry_type)
            {
                case RENDER_ENTRY_QuadEntry:
                {
                    QuadEntry* quad = (QuadEntry*)(base + header_size);
                    
                    // NOTE: we flush draw everything on shader change
                    if (quad->shader_id != current_shader)
                    {
                        OpenGLDraw(ren, &render_group->setup, current_shader);
                        current_shader = quad->shader_id;
                    }
                    u32 sprite_slot = 0;
                    
                    vec2 uvs[] = {
                        V2(1.0f, 1.0f),
                        V2(1.0f, 0.0f),
                        V2(0.0f, 0.0f),
                        V2(0.0f, 1.0f),
                    };
                    
                    if (quad->sprite_handle)
                    {
                        Sprite* sprite = GetLoadedSprite(ren->assets, quad->sprite_handle);
                        if (sprite->type == TYPE_SPRITE)
                        {
                            sprite_slot = sprite->slot;
                        }
                        else if (sprite->type == TYPE_SUBSPRITE)
                        {
                            sprite_slot = GetLoadedSprite(ren->assets, sprite->main_sprite)->slot;
                            uvs[0] = sprite->uvs[0];
                            uvs[1] = sprite->uvs[1];
                            uvs[2] = sprite->uvs[2];
                            uvs[3] = sprite->uvs[3];
                        }
                    }
                    
                    
                    vec3 positions[] =
                    {
                        V3(1.0f,  1.0f, 1.0f),
                        V3(1.0f,  0.0f, 1.0f),
                        V3(0.0f,  0.0f, 1.0f),
                        V3(0.0f,  1.0f, 1.0f),
                    };
                    
                    positions[0] *= V3(quad->size, 1.0f);
                    positions[1] *= V3(quad->size, 1.0f);
                    positions[2] *= V3(quad->size, 1.0f);
                    positions[3] *= V3(quad->size, 1.0f);
                    positions[0] += quad->position;
                    positions[1] += quad->position;
                    positions[2] += quad->position;
                    positions[3] += quad->position;
                    
                    u32 indices[] = {
                        0, 1, 3,
                        1, 2, 3,
                    };
                    
                    Vertex* vertex = ren->vertices + ren->vertex_count;
                    u32* index = ren->indices + ren->indices_count;
                    
                    for (u32 i = 0; i < VERTICES_PER_QUAD; ++i)
                    {
                        vertex->position = positions[i];
                        vertex->color = quad->color;
                        vertex->uv = uvs[i];
                        vertex->texture_slot = (f32)sprite_slot;
                        
                        ++vertex;
                    }
                    
                    for (u32 i = 0; i < INDICES_PER_QUAD; ++i)
                    {
                        *index = ren->vertex_count + indices[i];
                        ++index;
                    }
                    
                    ren->vertex_count += VERTICES_PER_QUAD;
                    ren->indices_count += INDICES_PER_QUAD;
                    Assert(ren->vertex_count < MAX_VERTICES &&
                           ren->indices_count < MAX_INDICES);
                } break;
                case RENDER_ENTRY_TriangleEntry:
                {
                    TriangleEntry* triangle = (TriangleEntry*)(base + header_size);
                    u32 sprite_slot = 0;
                    
                    if (triangle->shader_id != current_shader)
                    {
                        OpenGLDraw(ren, &render_group->setup, current_shader);
                        current_shader = triangle->shader_id;
                    }
                    
                    if (triangle->sprite_handle)
                    {
                        sprite_slot = GetLoadedSprite(ren->assets, triangle->sprite_handle)->slot;
                    }
                    
                    
                    Vertex* vertex = ren->vertices + ren->vertex_count;
                    u32* index = ren->indices + ren->indices_count;
                    
                    u32 indices[] = {
                        0, 1, 2,
                    };
                    
                    for (u32 i = 0; i < 3; ++i)
                    {
                        vertex->position = triangle->points[i];
                        vertex->color = triangle->color;
                        // TODO:
                        // vertex->uv = uvs[i];
                        vertex->texture_slot = (f32)sprite_slot;
                        
                        *index = ren->vertex_count + indices[i];
                        
                        ++index;
                        ++vertex;
                    }
                    
                    ren->vertex_count += 3;
                    ren->indices_count += 3;
                    
                } break;
                default:
                {
                    Assert(false);
                } break;
            }
        }
        OpenGLDraw(ren, &render_group->setup, current_shader);
        render_group->push_buffer_size = 0;
        render_group->sort_element_count = 0;
        
        render_group = ren->render_groups + (group_id + 1);
    }
    OpenGLDrawMesh(ren, &ren->mesh);
}

