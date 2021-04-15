
#include "math.h"


global_variable const char* vertex_shader_3D =
R"(
#version 330 core
layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec3 att_normal;
layout (location = 2) in vec2 att_uv;
layout (location = 3) in float att_texid;

out vec3 frag_pos;
out vec3 normal;
out vec2 uv;
out float tex_id;

uniform mat4 u_viewproj;
uniform mat4 u_transform;
uniform mat4 u_normal_trans;

void main()
{
    vec4 frag = u_viewproj * u_transform * vec4(att_pos, 1.0f);
    uv = att_uv;
    normal = mat3(u_normal_trans) * att_normal;
    tex_id = att_texid;

    gl_Position = frag;

    frag_pos = frag.xyz;
}
)";

global_variable const char* fragment_shader_3D =
R"(
#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    bool has_texture;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 frag_color;

in vec3 frag_pos;
in vec3 normal;
in vec2 uv;
in float tex_id;

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform Material u_material;
uniform Light u_light;

uniform sampler2D u_textures[32];


void main()
{
    vec3 norm = normalize(normal);

    vec3 light_dir = normalize(u_light_pos - frag_pos);
    float diff = max(dot(light_dir, norm), 0.0f);

    vec3 view_dir = normalize(u_cam_pos - frag_pos);
    vec3 reflected = reflect(-light_dir, norm);

    float spec = pow(max(dot(reflected, view_dir), 0.0f), u_material.shininess);

    vec3 ambient = u_light.ambient * u_material.ambient;
    vec3 diffuse = u_light.diffuse * (u_material.diffuse * diff);
    vec3 specular = u_light.specular * (u_material.specular * spec);


    if (u_material.has_texture)
    {
        diffuse = u_light.diffuse * (texture(u_textures[0], uv) * diff).xyz;
        frag_color = vec4(specular + diffuse + ambient, 1.0f);
    }
    else
    {
        frag_color = vec4(diffuse + ambient + specular, 1.0f);
    }
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
   gl_Position = u_viewproj * vec4(att_pos, 0.0f, 1.0f);
   uv = att_uv;
   color = att_color;
   tex_id = att_tex_id;
}
)";

global_variable const char* fragment_shader_2D =
R"(
#version 330 core

out vec4 frag_color;

uniform sampler2D u_textures[2];

in vec2 uv;
in vec4 color;
in float tex_id;

void main()
{
    if (tex_id == 1)
    {
        float alpha = texture(u_textures[1], uv).r;
		frag_color = color * vec4(1.0f, 1.0f, 1.0f, alpha);
    }
    else if (tex_id == 0)
    {
		frag_color = texture(u_textures[0], uv);
    }
    else
    {
        frag_color = color;
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
        DEBUG_PRINT("Failed to compile vertex shader: \n%s", info_log);
    }

    u32 fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fs, 5120, NULL, info_log);
        DEBUG_PRINT("Failed to compile fragment shader: \n%s", info_log);
    }

    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 5120, NULL, info_log);
        DEBUG_PRINT("Failed to link shader \n%s", info_log);
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
        DEBUG_PRINT("Failed to get uniform location: %s\n", uniform);
    }
    return loc;

}

internal u32
opengl_load_texture(Image* image, u32 slot)
{
    u32 texture_id = 0;

    glActiveTexture(GL_TEXTURE0 + slot);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    u32 external_format = 0;
    u32 internal_format = 0;

    if (image->channels == 4)
    {
        external_format = GL_RGBA;
        internal_format = GL_RGBA8;
    }
    else if (image->channels == 3)
    {
        external_format = GL_RGB;
        internal_format = GL_RGB8;
    }
    else if (image->channels == 1)
    {
        external_format = GL_RED;
        internal_format = GL_R8;
    }
    else
    {
        ASSERT(false);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                 0, 
                 internal_format,
                 image->width,
                 image->height,
                 0,
                 external_format,
                 GL_UNSIGNED_BYTE, 
                 image->data);

    image->loaded_to_gpu = true;

    return texture_id;
}

global_variable u32 texture_slot;

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

    ren->shader_program_3D = glCreateProgram();
    ren->shader_program_2D = glCreateProgram();


    opengl_compile_shaders(ren->shader_program_3D, vertex_shader_3D, fragment_shader_3D);
    opengl_compile_shaders(ren->shader_program_2D, vertex_shader_2D, fragment_shader_2D);

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

    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2); 

    // texture id
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
    glEnableVertexAttribArray(3); 

    glBindVertexArray(0);


    // 2D 

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


    glUseProgram(ren->shader_program_2D);


    i32 sampler_loc = opengl_get_uniform_location(ren->shader_program_2D, "u_textures[0]");
    glUniform1i(sampler_loc, 0);
    sampler_loc = opengl_get_uniform_location(ren->shader_program_2D, "u_textures[1]");
    glUniform1i(sampler_loc, 1);

    glBindVertexArray(0);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


internal void
opengl_begin_frame(Renderer* ren)
{

}

internal void
set_material_uniform(u32 shader, Material* material)
{
    i32 amb, spec, diff, shin, tex;
    amb = opengl_get_uniform_location(shader, "u_material.ambient");
    spec = opengl_get_uniform_location(shader, "u_material.specular");
    diff = opengl_get_uniform_location(shader, "u_material.diffuse");
    shin = opengl_get_uniform_location(shader, "u_material.shininess");
    tex =  opengl_get_uniform_location(shader, "u_material.has_texture");

    glUniform3fv(amb, 1, material->ambient.data);
    glUniform3fv(spec, 1, material->specular.data);
    glUniform3fv(diff, 1, material->diffuse.data);
    glUniform1f(shin, material->shininess);
    glUniform1i(tex, material->has_texture);
}

internal void
set_light_uniform(u32 shader, Light* light)
{
    i32 amb, spec, diff;
    amb = opengl_get_uniform_location(shader, "u_light.ambient");
    spec = opengl_get_uniform_location(shader, "u_light.specular");
    diff = opengl_get_uniform_location(shader, "u_light.diffuse");

    glUniform3fv(amb, 1, light->ambient.data);
    glUniform3fv(spec, 1, light->specular.data);
    glUniform3fv(diff, 1, light->diffuse.data);
}

#if 0
internal void
opengl_draw_model(Renderer* ren, Model* model, vec3 position, vec3 size)
{
   
     mat4 transfrom = mat4_translate(position) *
                      mat4_scale(size);
  
     mat4 mvp = ren->projection * ren->view * transfrom;
     mat4 normal_transform = mat4_transpose(mat4_inverse(transfrom));
     b8 do_transpose = true;
  
     i32 transform_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_transform");
     glUniformMatrix4fv(transform_loc, 1, do_transpose, (f32*)transfrom.data);
  
     i32 normal_loc =  opengl_get_uniform_location(ren->shader_program_3D, "u_normal_trans");
     glUniformMatrix4fv(normal_loc, 1, do_transpose, (f32*)normal_transform.data);
  

     for (u32 i = 0; i < model->num_meshes; ++i)
     {
        Mesh* mesh = &model->meshes[i];
        set_material_uniform(ren->shader_program_3D, &mesh->material);
        if (mesh->material.has_texture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model->loaded_images[mesh->texture_index].id);
        }
        glBindVertexArray(mesh->VAO);
  
  
        if (mesh->indices)
        {
            glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, mesh->num_vertices);
        }
     }

   
}
#endif

internal void
opengl_end_frame(RenderGroup* group)
{
    Renderer* ren = group->renderer;

    u32 header_size = sizeof(RenderEntryHeader);
    for (u32 base_offset = 0; base_offset < group->push_buffer_size;)
    {
        u8* base = (group->push_buffer_base + base_offset);

        switch (((RenderEntryHeader *)base)->entry_type)
        {
            case RENDER_ENTRY_Quad:
            {
                Quad* quad = (Quad*)(base + header_size);
                draw_quad(group->renderer, quad->position, quad->size, quad->color);
                base_offset += sizeof(Quad) + header_size;
            } break;
            case RENDER_ENTRY_TexturedQuad:
            {
                TexturedQuad* quad = (TexturedQuad*)(base + header_size);
                Image* image = get_loaded_image(group->assets, quad->image);
                if (!image->loaded_to_gpu)
                {
                    image->id = opengl_load_texture(image, ren->slot);
                    image->slot = ren->slot;
                    ++ren->slot;
                }
                glActiveTexture(GL_TEXTURE0 + image->slot);
                glBindTexture(GL_TEXTURE_2D, image->id);
                draw_textured_quad(ren, image->slot, quad->position, quad->size, quad->color);
                base_offset += sizeof(TexturedQuad) + header_size;
            } break;

            default:
            {
                ASSERT(false);
            } break;
        }

    }

    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    local_persist i32 w, h;
    if (w != ren->screen_width || h != ren->screen_height)
    {
        w = ren->screen_width;
        h = ren->screen_height;
        glViewport(0, 0, w, h);
    }

    glUseProgram(ren->shader_program_2D);

    b8 do_transpose = true;

    i32 vp_loc = opengl_get_uniform_location(ren->shader_program_2D, "u_viewproj");
    glUniformMatrix4fv(vp_loc, 1, do_transpose, (f32*)group->projection.data);
    u32 size = sizeof(VertexData2D) * ren->vertex_count_2D;

    glBindVertexArray(ren->VAO_2D);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO_2D);


    glBufferSubData(GL_ARRAY_BUFFER, 0, size, ren->vertices_2D); 
    glDrawArrays(GL_TRIANGLES, 0, ren->vertex_count_2D);
    ren->vertex_count_2D = 0;
#if 0
    glEnable(GL_DEPTH_TEST);
    glUseProgram(ren->shader_program_3D);

    mat4 viewproj = ren->projection * ren->view;
    b8 do_transpose = true;

    set_light_uniform(ren->shader_program_3D, &ren->light);

    i32 light_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_light_pos");
    glUniform3fv(light_loc, 1, (f32*)ren->light_pos.data);

    i32 view_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_cam_pos");
    glUniform3fv(view_loc, 1, (f32*)ren->camera.position.data);

    i32 mvp_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_viewproj");
    glUniformMatrix4fv(mvp_loc, 1, do_transpose, (f32*)viewproj.data);

    // Model
     
    // NOTE(marko): 2D renderer

    {
    }

#endif
}

