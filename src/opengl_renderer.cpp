
#include "math.h"

global_variable const char* vertex_shader_2D =
R"(
#version 330 core
layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec2 att_uv;
layout (location = 2) in vec4 att_color;
layout (location = 3) in float att_tex_id;

out vec2 uv;
out vec4 color;
out float tex_id;

uniform mat4 u_viewproj;

void main()
{
   gl_Position = u_viewproj * vec4(att_pos, 1.0f);
   uv = att_uv;
   color = att_color;
   tex_id = att_tex_id;
}
)";

global_variable const char* fragment_shader_2D =
R"(
#version 330 core

out vec4 frag_color;

uniform sampler2D u_textures[9];

in vec2 uv;
in vec4 color;
in float tex_id;

void main()
{
    highp int index = int(tex_id);
    if (index == 0)
    {
        frag_color = color;
    }
    else if (index == 1)
    {
        frag_color = color * texture(u_textures[1], uv);
    }
    else if (index == 2)
    {
        frag_color = color * texture(u_textures[2], uv);
    }
    else if (index == 3)
    {
        frag_color = color * texture(u_textures[3], uv);
    }
    else if (index == 4)
    {
        frag_color = color * texture(u_textures[4], uv);
    }
}

)";



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


    glDeleteShader(vs);
    glDeleteShader(fs);
    free(info_log);
}

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


#if 0
internal void
opengl_load_model_to_gpu(Model* model)
{
    u32 stride = sizeof(f32) * 9;

    for (u32 mesh_index = 0; mesh_index < model->num_meshes; ++mesh_index)
    {
        Mesh* mesh = &model->meshes[mesh_index];

        glGenVertexArrays(1, &mesh->VAO);
        glGenBuffers(1, &mesh->VBO);

        glBindVertexArray(mesh->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

        if (mesh->indices)
        {
            glGenBuffers(1, &mesh->EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
            u32 size = mesh->num_indices * sizeof(u32);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void*)mesh->indices, GL_STATIC_DRAW);
        }

        u32 size = sizeof(VertexData) * mesh->num_vertices;
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
    for (u32 i = 0; i < model->num_textures; ++i)
    {
        opengl_load_texture(model->loaded_images + i,
                            texture_slot,
                            &model->loaded_images[i].id);
        texture_slot++;
    }

}
#endif


internal void
opengl_init(Renderer* ren)
{

    ren->shader_program = glCreateProgram();


    opengl_compile_shaders(ren->shader_program, vertex_shader_2D, fragment_shader_2D);

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
    // 2D 

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


    glUseProgram(ren->shader_program);


    char texture_uniform_name[] = {"u_textures[-]"};
    for (u32 tex_id = 0; tex_id < 9; ++tex_id)
    {
        texture_uniform_name[11] = '0' + tex_id;
        i32 loc = opengl_get_uniform_location(ren->shader_program, texture_uniform_name);
        glUniform1i(loc, tex_id);
    }
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);
}


internal void
opengl_begin_frame(Renderer* ren)
{

}


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


    // NOTE: Dumb sort

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
                        V2(1.0f,  1.0f) * quad->size + quad->position, // top right
                        V2(1.0f,  0.0f) * quad->size + quad->position, // bottom right
                        V2(0.0f,  0.0f) * quad->size + quad->position, // bottom left
                        V2(0.0f,  1.0f) * quad->size + quad->position, // top left
                    };


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
                } break;
                default:
                {
                    ASSERT(false);
                } break;
            }
        }

        glUseProgram(ren->shader_program);
        RenderSetup* setup = &render_group->setup;

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

        i32 vp_loc = opengl_get_uniform_location(ren->shader_program, "u_viewproj");
        mat4 cam_mat = camera_transform(&setup->camera);
        mat4 viewproj = setup->projection * cam_mat;
        glUniformMatrix4fv(vp_loc, 1, true, (f32*)viewproj.data);


        glBindVertexArray(ren->VAO);

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_size, ren->indices);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_size, ren->vertices);

        glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);

        render_group = render_group->next;

        ren->vertex_count = 0;
        ren->indices_count = 0;
    }
    ren->render_groups = 0;

}

