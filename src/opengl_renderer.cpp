
#include "math.h"

#include "stb_truetype.h"


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
    if (tex_id == 1)
    {
        float alpha = texture(u_textures[0], uv).r;
		frag_color = vec4(1.0f, 1.0f, 1.0f, alpha);
    }
    else if (tex_id == 2)
    {
		frag_color = texture(u_textures[1], uv);
    }
    else
    {
        frag_color = color;
    }
}

)";


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
opengl_init(Renderer* ren)
{
    glEnable(GL_DEPTH_TEST);
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
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &ren->font_texture_id);
    glBindTexture(GL_TEXTURE_2D, ren->font_texture_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, ren->font_bitmap.data);


    i32 font_loc = opengl_get_uniform_location(ren->shader_program_2D, "u_textures[0]");
    glUniform1i(font_loc, 0);

    glBindVertexArray(0);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

internal void
opengl_draw_rectangle(Renderer* ren, vec3 position, vec2 scale, vec4 color)
{

    vec3 rectangle_vertices[] =
    {
        {-0.5f, -0.5f, 0.0f},
        {-0.5f,  0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f},

        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f},
    };
    vec3 scal = to_vec3(scale);
    position = position + (scal/2);


    for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
    {
        ren->vertices_2D[ren->vertex_index_2D].position =      
                                 rectangle_vertices[i] * scal + position;
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



internal void
opengl_texture_test(Renderer* ren, vec2 position, vec2 scale, vec4 color)
{
    /*
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
    vec2 uv_coords[] =
    {
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
    };
    for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
    {
        ren->vertices_2D[ren->vertex_index_2D].position = rectangle_vertices[i];
        ren->vertices_2D[ren->vertex_index_2D].uv = uv_coords[i];
        ren->vertices_2D[ren->vertex_index_2D].color = color;
        ren->vertices_2D[ren->vertex_index_2D].texture_id = -1;
        ren->vertex_index_2D++;
    }
    */
}

internal void
opengl_draw_text(Renderer* ren, char* text, vec3 position, vec4 color)
{
    f32 z = position.z;
    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            #define IFLOOR(x) ((int) floor(x))
            stbtt_aligned_quad q;
            i32 index = *text-32;
            stbtt_bakedchar* b = ren->char_metrics + index;

            float d3d_bias = 0.0f;
            f32 ipw = 1.0f / ren->font_bitmap.width;
            f32 iph = 1.0f / ren->font_bitmap.height;
            i32 round_x = IFLOOR((position.x + b->xoff) + 0.5f);
            i32 round_y = IFLOOR((position.y - b->yoff) + 0.5f);

            q.x0 = round_x + d3d_bias;
            q.y1 = round_y + d3d_bias;
            q.x1 = round_x + b->x1 - b->x0 + d3d_bias;
            q.y0 = round_y - b->y1 + b->y0 + d3d_bias;
            q.s0 = b->x0 * ipw;
            q.t0 = b->y0 * iph;
            q.s1 = b->x1 * ipw;
            q.t1 = b->y1 * iph;

            position.x += b->xadvance;

            vec3 rectangle_vertices[] =
            {
                {q.x0, q.y0, z},
                {q.x0, q.y1, z},
                {q.x1, q.y1, z},
                {q.x0, q.y0, z},
                {q.x1, q.y1, z},
                {q.x1, q.y0, z},
            };

            vec2 uv_coords[] =
            {
                {q.s0, q.t1},
                {q.s0, q.t0},
                {q.s1, q.t0},
                {q.s0, q.t1},
                {q.s1, q.t0},
                {q.s1, q.t1},
            };

            for (sizet i = 0; i < ARRAY_COUNT(rectangle_vertices); ++i)
            {
                ren->vertices_2D[ren->vertex_index_2D].position = rectangle_vertices[i];
                ren->vertices_2D[ren->vertex_index_2D].uv = uv_coords[i];
                ren->vertices_2D[ren->vertex_index_2D].color = color;
                ren->vertices_2D[ren->vertex_index_2D].texture_id = (f32)ren->font_texture_id;
                ren->vertex_index_2D++;
            }
        }
        text++;
    }
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

}

internal void
opengl_end_frame(Renderer* ren)
{
    // NOTE(marko): 3D renderer
    glUseProgram(ren->shader_program_3D);

    mat4 view = camera_transform(&ren->camera);
    mat4 projection = mat4_perspective(1024.0f, 768.0f, 120.0f, 0.1f, 1000.0f);
    //mat4 projection = mat4_orthographic(1024.0f, 768.0f);
    mat4 model = mat4_scale({100, 100, 100});
    mat4 mvp = projection * view * model;
    mat4 normal_transform = mat4_transpose(mat4_inverse(model));

    local_persist f32 light_z = 10.0f;
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

    // NOTE(marko): 2D renderer
    glUseProgram(ren->shader_program_2D);

    view = camera_transform(&ren->camera);
    projection = mat4_orthographic(1024.0f, 768.0f);
    mat4 viewproj = projection;

    i32 vp_loc = opengl_get_uniform_location(ren->shader_program_2D, "u_viewproj");
    glUniformMatrix4fv(vp_loc, 1, do_transpose, (f32*)viewproj.data);
    size = sizeof(VertexData2D) * ren->vertex_index_2D;

    glBindVertexArray(ren->VAO_2D);
    glBindBuffer(GL_ARRAY_BUFFER, ren->VBO_2D);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, ren->vertices_2D); 
    glDrawArrays(GL_TRIANGLES, 0, ren->vertex_index_2D);
    ren->vertex_index_2D = 0;

}

