#include "math.h"

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
OpenGLFunction(void,     glGetActiveUniformName, GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);


#include "shaders.h"


internal inline i32
opengl_get_uniform_location(i32 shader, char* uniform)
{
    i32 loc = glGetUniformLocation(shader, uniform);
    if (loc == -1)
    {
        PRINT("Failed to get uniform location: %s\n", uniform);
    }
    return loc;

}

internal UniformTypeInfo
get_uniform_type_info(GLenum type)
{
    switch(type)
    {
        case GL_FLOAT:
        {
            return {UNIFORM_F32, sizeof(f32)};
        }break;
        case GL_FLOAT_VEC2:
        {
            return {UNIFORM_VEC2, sizeof(vec2)};
        }break;
        case GL_FLOAT_VEC3:
        {
            return {UNIFORM_VEC3, sizeof(vec3)};
        }break;
        case GL_FLOAT_VEC4:
        {
            return {UNIFORM_VEC4, sizeof(vec4)};
        }break;
        case GL_SAMPLER_2D:
        {
            return {UNIFORM_TEXTURE2D, 0};
        }break;
        case GL_FLOAT_MAT4:
        {
            return {UNIFORM_MAT4, sizeof(mat4)};
        }break;
    }

    ASSERT(false);
}


internal Shader
opengl_create_shader(MemoryArena* arena, const char* vertex_shader, const char* fragment_shader)
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
        PRINT("Failed to compile vertex shader: \n%s", info_log);
    }

    u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fs, 5120, NULL, info_log);
        PRINT("Failed to compile fragment shader: \n%s", info_log);
    }

    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 5120, NULL, info_log);
        PRINT("Failed to link shader \n%s", info_log);
    }
    glUseProgram(shader_program);

    char texture_uniform_name[] = {"u_textures[0]"};
    i32 loc = opengl_get_uniform_location(shader_program, texture_uniform_name);

    if (loc != -1)
    {
        for (u32 tex_id = 0; tex_id < 9; ++tex_id)
        {
            texture_uniform_name[11] = '0' + tex_id;
            loc = opengl_get_uniform_location(shader_program, texture_uniform_name);
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

        glGetActiveUniformName(shader.bind_id,
                               uniform_id,
                               ArrayCount(name),
                               NULL,
                               name);


        // TODO: 
        // u_viewproj should be set as global uniform shared between shaders
        // using UBOs..

        Uniform* uniform = add_uniform(&shader, name, arena);
        i32 size = 0;
        GLenum gl_type = 0;


        glGetActiveUniform(shader_program,
                           uniform_id,
                           0,
                           0,
                           0,
                           &gl_type,
                           0);

        uniform->type_info = get_uniform_type_info(gl_type);
        uniform->data = PushMemory(arena, u8, uniform->type_info.size);
    }

    return shader;
}


internal u32
opengl_load_texture(Sprite* sprite, u32 slot)
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
        ASSERT(false);
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

    sprite->loaded_to_gpu = true;

    return texture_id;
}






internal void
opengl_init(Renderer* ren)
{
    Shader* shaders = ren->assets->shaders;
    MemoryArena* arena = &ren->assets->arena;

    shaders[SHADER_ID_NORMAL] = opengl_create_shader(arena, basic_vertex_shader, basic_fragment_shader);
    shaders[SHADER_ID_COLORPICKER] = opengl_create_shader(arena, basic_vertex_shader, colorpicker_fragment_shader);
    shaders[SHADER_ID_TEST] = opengl_create_shader(arena, basic_vertex_shader, test_shader);
    

    glGenVertexArrays(1, &ren->VAO);
    glGenBuffers(1, &ren->VBO);
    glGenBuffers(1, &ren->EBO);


    glBindVertexArray(ren->VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ren->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * MAX_INDICES, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * MAX_VERTICES, NULL, GL_DYNAMIC_DRAW);

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

    glBindVertexArray(0);

#if 0
    char texture_uniform_name[] = {"u_textures[-]"};
    for (u32 tex_id = 0; tex_id < 9; ++tex_id)
    {
        texture_uniform_name[11] = '0' + tex_id;
        i32 loc = opengl_get_uniform_location(ren->shader_program_3D, texture_uniform_name);
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

    // uví*sizeof(f32)));
    glEnableVertexAttribArray(2); 

    // texture id
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
    glEnableVertexAttribArray(3); 

    glBindVertexArray(0);


#endif
}



// NOTE: Dumb sort
internal void
sort_render_group(RenderGroup* render_group)
{
    SortElement* sort_entries = sort_element_start(render_group);

    for(i32 i = 1; i < (i32)render_group->sort_element_count; ++i)
    {
        SortElement sort_element = sort_entries[i];

        i32 j = i - 1;
        while(j >= 0 && sort_entries[j].key > sort_element.key)
        {
            sort_entries[j+1] = sort_entries[j];
            --j;
        }
        sort_entries[j+1] = sort_element;
    }
}


internal void
upload_shader_uniform(u32 shader_program, Uniform* uniform)
{
    // TODO: should cache this when we load shader
    i32 location = opengl_get_uniform_location(shader_program, uniform->name);

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
            ASSERT(false);
        } break;
    }

}

internal void
opengl_draw(Renderer* ren, RenderSetup* setup, ShaderId shader_id)
{
    Shader* shader = get_shader(ren->assets, shader_id);
    u32 shader_program = shader->bind_id;

    glUseProgram(shader_program);

    for (u32 sprite_index = 0; sprite_index < setup->num_sprites; ++sprite_index)
    {
        // PRINT("sprite handle %d", setup->sprite_handles[sprite_index]);
        Sprite* sprite = get_loaded_sprite(ren->assets, 
                                           setup->sprite_handles[sprite_index]);

        if (!sprite->loaded_to_gpu)
        {
            sprite->id = opengl_load_texture(sprite, ren->slot);
            sprite->slot = ren->slot;
            ++ren->slot;
            ASSERT(ren->slot < 32);
        }
        glActiveTexture(GL_TEXTURE0 + sprite->slot);
        glBindTexture(GL_TEXTURE_2D, sprite->id);
    }

    u32 num_vertices = ren->vertex_count;
    u32 num_indices = ren->indices_count;


    u32 vertices_size = num_vertices * sizeof(VertexData);
    u32 indices_size = num_indices * sizeof(u32);

    i32 vp_loc = opengl_get_uniform_location(shader_program, "u_viewproj");
    mat4 cam_mat = camera_transform(&setup->camera);
    mat4 viewproj = setup->projection * cam_mat;
    glUniformMatrix4fv(vp_loc, 1, true, &viewproj.rows[0].x);


    Uniform* uniform = shader->uniforms;
    while (uniform)
    {
        if (!string_match(uniform->name, "u_viewproj"))
        {
            upload_shader_uniform(shader_program, uniform);
        }
        uniform = uniform->next;
    }


    glBindVertexArray(ren->VAO);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_size, ren->indices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_size, ren->vertices);

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);


    ren->vertex_count = 0;
    ren->indices_count = 0;
}

internal void
opengl_end_frame(Renderer* ren)
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    local_persist i32 w, h;
    if (w != ren->screen_width || h != ren->screen_height)
    {
        w = ren->screen_width;
        h = ren->screen_height;
        glViewport(0, 0, w, h);
    }


    //NOTE: this is a default white texture 
    Sprite* white_sprite = get_loaded_sprite(ren->assets, ren->white_sprite);
    if (!white_sprite->loaded_to_gpu)
    {
        ASSERT(ren->slot == 0);
        white_sprite->id = opengl_load_texture(white_sprite, ren->slot);
        white_sprite->slot = ren->slot;
        ++ren->slot;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, white_sprite->id);


    u32 header_size = sizeof(RenderEntryHeader);

    RenderGroup* render_group = ren->render_groups;
    ShaderId current_shader = SHADER_ID_NORMAL;


    while(render_group)
    {
        sort_render_group(render_group);

        SortElement* sort_element = sort_element_start(render_group);

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
                        opengl_draw(ren, &render_group->setup, current_shader);
                        current_shader = quad->shader_id;
                    }
                    u32 sprite_slot = 0;

                    vec2 uvs[] = {
                        V2(1.0f, 1.0f),
                        V2(1.0f, 0.0f),
                        V2(0.0f, 0.0f),
                        V2(0.0f, 1.0f),
                    };

                    if (quad->subsprite)
                    {
                        uvs[0] = quad->subsprite->uvs[0];
                        uvs[1] = quad->subsprite->uvs[1];
                        uvs[2] = quad->subsprite->uvs[2];
                        uvs[3] = quad->subsprite->uvs[3];
                        sprite_slot = get_loaded_sprite(ren->assets, quad->subsprite->sprite_sheet)->slot;
                    }
                    else if (quad->sprite_handle)
                    {
                        sprite_slot = get_loaded_sprite(ren->assets, quad->sprite_handle)->slot;
                    }


                    vec2 positions[] =
                    {
                         V2(1.0f,  1.0f),
                         V2(1.0f,  0.0f),
                         V2(0.0f,  0.0f),
                         V2(0.0f,  1.0f),
                    };

                    positions[0] *= quad->size;
                    positions[1] *= quad->size;
                    positions[2] *= quad->size;
                    positions[3] *= quad->size;
                    positions[0] += quad->position;
                    positions[1] += quad->position;
                    positions[2] += quad->position;
                    positions[3] += quad->position;

                    u32 indices[] = {
                        0, 1, 3,
                        1, 2, 3,
                    };

                    VertexData* vertex = ren->vertices + ren->vertex_count;
                    u32* index = ren->indices + ren->indices_count;

                    for (u32 i = 0; i < VERTICES_PER_QUAD; ++i)
                    {
                        vertex->position = V3(positions[i], 0.0f);
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
                    ASSERT(ren->vertex_count < MAX_VERTICES &&
                           ren->indices_count < MAX_INDICES);
                } break;
                default:
                {
                    ASSERT(false);
                } break;
            }
        }
        opengl_draw(ren, &render_group->setup, current_shader);
        render_group = render_group->next;

    }
    ren->render_groups = 0;

}

