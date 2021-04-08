#if !defined(RENDERER_H)
#define RENDERER_H
#include "stb_truetype.h"


struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    f32 shininess;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct VertexData
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4 color;
};

struct VertexData2D
{
    vec3 position;
    vec2 uv;
    vec4 color;
    f32 texture_id;
};


struct Camera
{
    vec3 position;
    vec3 direction;
    vec3 up;
};

struct Texture
{
    char* name;
    void* data;
    i32 width, height, channels;
    u32 id;
};

struct Mesh
{
    VertexData* vertices;
    u32 num_vertices;

    u32* indices;
    u32 num_indices;

    u32 texture_index;

    Material material;

    u32 VAO, VBO, EBO;
};

struct Model
{
    b8 loaded_to_gpu;
    Mesh* meshes;
    u32 num_meshes;
};

inline mat4
camera_transform(Camera* cam)
{
    vec3 f = vec3_normalized(cam->direction);
    vec3 u = vec3_normalized(cam->up);
    vec3 r = vec3_normalized(vec3_cross(cam->up, cam->direction));
    vec3 t = cam->position;

    mat4 out =
    {
        r.x,  r.y,  r.z,  -t.x,
        u.x,  u.y,  u.z,  -t.y,
        f.x,  f.y,  f.z,  -t.z,
        0.0f, 0.0f, 0.0f,  1.0f
    };

    return out;
}

struct LoadedBitmap
{
    u8* data;
    i32 width;
    i32 height;
};

#define MAX_VERTICES 10000
#define NUM_ASCII 96

struct Renderer;
typedef void RendererProc(Renderer* ren);
typedef void RendererProc(Renderer* ren);
typedef void RendererProc(Renderer* ren);

struct RenderGroup
{
    u32 num_vertieces;
    VertexData *vertices;
    mat4 transform;

    RenderGroup* next_group;
};


struct Renderer
{
    MemoryArena arena;

    RenderGroup* render_groups;

    i32 shader_program_3D;
    VertexData vertices_start[MAX_VERTICES];
    u32 vertex_count;
    u32 VAO;
    u32 VBO;
    Camera camera;

    VertexData2D vertices_2D[MAX_VERTICES];
    i32 shader_program_2D;
    u32 vertex_count_2D;
    u32 VBO_2D;
    u32 VAO_2D;

    mat4 projection;
    mat4 view;

    u32 slot;

    vec3 light_pos;
    Material material;
    Light light;

    Material jade;

    Texture loaded_textures[32];
    u32 num_textures;

    // TODO: this should be in DebugState
    Texture font_texture;
    stbtt_bakedchar char_metrics[NUM_ASCII];
    f32 font_size;
    u32 font_texture_id;
    f32 x_advance;
    //
    Model* model;

    i32 screen_width;
    i32 screen_height;

    f32 light_speed;

    RendererProc* renderer_init;
    RendererProc* renderer_begin;
    RendererProc* renderer_end;
};

internal void
push_render_group(Renderer* ren, u32 num_vertieces)
{
    RenderGroup* render_group = PushMemory(&ren->arena, RenderGroup);
    render_group->vertices = PushMemory(&ren->arena, VertexData, num_vertieces);
    render_group->next_group = 0;
     
    if (!ren->render_groups)
    {
        ren->render_groups = render_group;
    }
    else
    {
        RenderGroup* node = ren->render_groups;
        while(node->next_group)
        {
            node = node->next_group;
        }
        node->next_group = render_group;
    }

}
#endif
