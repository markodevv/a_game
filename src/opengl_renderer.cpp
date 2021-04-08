
#include "math.h"

#include "stb_truetype.h"


global_variable const char* vertex_shader_3D =
R"(
#version 330 core
layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec3 att_normal;
layout (location = 2) in vec2 att_uv;
layout (location = 3) in vec4 att_color;

out vec3 frag_pos;
out vec3 normal;
out vec2 uv;
out vec4 color;

uniform mat4 u_MVP;
uniform mat4 u_normal_trans;

void main()
{
    vec4 frag = u_MVP *  vec4(att_pos, 1.0f);
    uv = att_uv;
    color = att_color;
    normal = mat3(u_normal_trans) * att_normal;
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
in vec4 color;

uniform vec3 u_view;
uniform vec3 u_light_pos;
uniform Material u_material;
uniform Light u_light;
uniform sampler2D u_textures[32];


void main()
{
    vec3 ambient, specular, diffuse;

    vec4 obj_color = color;

    ambient = u_light.ambient * u_material.ambient;

    vec3 norm = normalize(normal);

    vec3 light_dir = normalize(u_light_pos - frag_pos);
    float diff = max(dot(light_dir, norm), 0.0f);
    diffuse = u_light.diffuse * (u_material.diffuse * diff);

    vec3 view_dir = normalize(u_view - frag_pos);
    vec3 reflected = reflect(-light_dir, norm);

    float spec = pow(max(dot(reflected, view_dir), 0.0f), u_material.shininess);
    specular = u_light.specular * (u_material.specular * spec);


    frag_color = vec4(diffuse + ambient + specular, 1.0f);
}
)";

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

uniform sampler2D u_textures[2];

in vec2 uv;
in vec4 color;
in float tex_id;

void main()
{
    if (tex_id == 0)
    {
        float alpha = texture(u_textures[0], uv).r;
		frag_color = color * vec4(1.0f, 1.0f, 1.0f, alpha);
    }
    else if (tex_id == 1)
    {
		frag_color = texture(u_textures[1], uv);
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

internal void
opengl_load_texture(Texture* texture, u32 slot, u32* texture_id)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_2D, *texture_id);

    u32 gl_channels = 0;
    u32 image_format = 0;

    if (texture->channels == 4)
    {
        gl_channels = GL_RGBA;
        image_format = GL_RGBA;
    }
    else if (texture->channels == 3)
    {
        gl_channels = GL_RGB;
        image_format = GL_RGB;
    }
    else if (texture->channels == 1)
    {
        gl_channels = GL_RED;
        image_format = GL_RED;
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
                 image_format,
                 texture->width,
                 texture->height,
                 0,
                 gl_channels,
                 GL_UNSIGNED_BYTE, 
                 texture->data);

}


internal void
opengl_init(Renderer* ren)
{
    ren->shader_program_3D = glCreateProgram();
    ren->shader_program_2D = glCreateProgram();

    opengl_compile_shaders(ren->shader_program_3D, vertex_shader_3D, fragment_shader_3D);
    opengl_compile_shaders(ren->shader_program_2D, vertex_shader_2D, fragment_shader_2D);


    glUseProgram(ren->shader_program_3D);
    glGenVertexArrays(1, &ren->VAO);

    glGenBuffers(1, &ren->VBO);

    glBindVertexArray(ren->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * MAX_VERTICES, NULL, GL_STATIC_DRAW);
    // Vertex layout
    i32 stride = sizeof(f32) * 12;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1); 

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2); 

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
    glEnableVertexAttribArray(3); 

    glBindVertexArray(0);

    if (!ren->model->loaded_to_gpu)
    {
        stride = sizeof(f32) * 12;
        for (u32 mesh_index = 0; mesh_index < ren->model->num_meshes; ++mesh_index)
        {
            Mesh* mesh = &ren->model->meshes[mesh_index];

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

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
            glEnableVertexAttribArray(0); 

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(f32)));
            glEnableVertexAttribArray(1); 

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(f32)));
            glEnableVertexAttribArray(2); 

            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
            glEnableVertexAttribArray(3); 

            glBindVertexArray(0);

            ren->model->loaded_to_gpu = 1;
        }

        char texture_uniform_name[] = {"u_textures[-]"};

        for (u32 tex_id = 0; tex_id < ren->num_textures; ++tex_id)
        {
            opengl_load_texture(&ren->loaded_textures[tex_id], 
                                ren->slot, 
                                &ren->loaded_textures[tex_id].id);;
            texture_uniform_name[11] = '0' + tex_id;
            i32 loc= opengl_get_uniform_location(ren->shader_program_3D, 
                                                 texture_uniform_name);
            glUniform1i(loc, tex_id);
            ++ren->slot;
        }

    }



    // 2D 

    glGenVertexArrays(1, &ren->VAO_2D);
    glGenBuffers(1, &ren->VBO_2D);

    glBindVertexArray(ren->VAO_2D);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO_2D);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData2D) * MAX_VERTICES, NULL, GL_STATIC_DRAW);

    stride = sizeof(f32) * 10;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); 

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1); 

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(5*sizeof(f32)));
    glEnableVertexAttribArray(2); 

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(9*sizeof(f32)));
    glEnableVertexAttribArray(3); 


    glUseProgram(ren->shader_program_2D);
    opengl_load_texture(&ren->font_texture, 0, &ren->font_texture_id);;
    DEBUG_PRINT("font texture id %i", ren->font_texture_id);


    i32 font_loc = opengl_get_uniform_location(ren->shader_program_2D, "u_textures[0]");
    glUniform1i(font_loc, 0);

    glBindVertexArray(0);


    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


internal void
opengl_begin_frame(Renderer* ren)
{
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    local_persist i32 w, h;
    if (w != ren->screen_width || h != ren->screen_height)
    {
        w = ren->screen_width;
        h = ren->screen_height;
        glViewport(0, 0, w, h);
    }

    ren->view = camera_transform(&ren->camera);
    ren->projection = mat4_perspective((f32)ren->screen_width,
                                       (f32)ren->screen_height,
                                       120.0f, 1.0f, 1000.0f);
}

internal void
set_material_uniform(u32 shader, Material* material)
{
    i32 amb, spec, diff, shin;
    amb = opengl_get_uniform_location(shader, "u_material.ambient");
    spec = opengl_get_uniform_location(shader, "u_material.specular");
    diff = opengl_get_uniform_location(shader, "u_material.diffuse");
    shin = opengl_get_uniform_location(shader, "u_material.shininess");

    glUniform3fv(amb, 1, material->ambient.data);
    glUniform3fv(spec, 1, material->specular.data);
    glUniform3fv(diff, 1, material->diffuse.data);
    glUniform1f(shin, material->shininess);
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

internal void
opengl_end_frame(Renderer* ren)
{
    {
        // NOTE(marko): 3D renderer
        glEnable(GL_DEPTH_TEST);
        glUseProgram(ren->shader_program_3D);

        //mat4 projection = mat4_orthographic(1024.0f, 768.0f);
        local_persist f32 temp = 0.0f;
        temp += 0.05f;

        mat4 model = mat4_rotate(sinf(temp) * 100, {0.0f, 1.0f, 0.0f}) * mat4_scale({100, 100, 100});
        mat4 mvp = ren->projection * ren->view * model;
        mat4 normal_transform = mat4_transpose(mat4_inverse(model));


        ren->light_pos.x = 100.0f;
        ren->light_pos.y = 200.0f;

        b8 do_transpose = true;
        i32 mvp_loc, light_loc, view_loc, normal_loc;


        mvp_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_MVP");
        glUniformMatrix4fv(mvp_loc, 1, do_transpose, (f32*)mvp.data);

        light_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_light_pos");
        glUniform3fv(light_loc, 1, (f32*)ren->light_pos.data);

        view_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_view");
        glUniform3fv(view_loc, 1, (f32*)ren->camera.position.data);

        normal_loc =  opengl_get_uniform_location(ren->shader_program_3D, "u_normal_trans");
        glUniformMatrix4fv(normal_loc, 1, do_transpose, (f32*)normal_transform.data);

        set_material_uniform(ren->shader_program_3D, &ren->material);
        set_light_uniform(ren->shader_program_3D, &ren->light);


        glBindVertexArray(ren->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);
        u32 size = sizeof(VertexData) * ren->vertex_count;
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, ren->vertices_start); 
        glDrawArrays(GL_TRIANGLES, 0, ren->vertex_count);

        ren->vertex_count = 0;

        // Model
        if (ren->model)
        {
            model = mat4_translate(V3(-200, 0, 0)) *
                    mat4_scale(V3(10, 10, 10));
            mvp = ren->projection * ren->view * model;
            normal_transform = mat4_transpose(mat4_inverse(model));

            mvp_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_MVP");
            glUniformMatrix4fv(mvp_loc, 1, do_transpose, (f32*)mvp.data);
            normal_loc =  opengl_get_uniform_location(ren->shader_program_3D, "u_normal_trans");
            glUniformMatrix4fv(normal_loc, 1, do_transpose, (f32*)normal_transform.data);

            for (u32 i = 0; i < ren->model->num_meshes; ++i)
            {
                Mesh* mesh = &ren->model->meshes[i];
                glBindVertexArray(mesh->VAO);
                glBindTexture(GL_TEXTURE_2D, mesh->texture_index);

                set_material_uniform(ren->shader_program_3D, &mesh->material);
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
    }
     
    // NOTE(marko): 2D renderer

    glDisable(GL_DEPTH_TEST);
    glUseProgram(ren->shader_program_2D);

    ren->projection = mat4_orthographic((f32)ren->screen_width, (f32)ren->screen_height);
    mat4 viewproj = ren->projection;
    b8 do_transpose = true;

    i32 vp_loc = opengl_get_uniform_location(ren->shader_program_2D, "u_viewproj");
    glUniformMatrix4fv(vp_loc, 1, do_transpose, (f32*)viewproj.data);
    u32 size = sizeof(VertexData2D) * ren->vertex_count_2D;

    glBindVertexArray(ren->VAO_2D);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO_2D);

    glBindTexture(GL_TEXTURE_2D, ren->font_texture_id);

    glBufferSubData(GL_ARRAY_BUFFER, 0, size, ren->vertices_2D); 
    glDrawArrays(GL_TRIANGLES, 0, ren->vertex_count_2D);
    ren->vertex_count_2D = 0;

}

