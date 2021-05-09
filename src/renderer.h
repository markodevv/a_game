#if !defined(RENDERER_H)
#define RENDERER_H

struct Color
{
    u8 r, g, b, a;
};

internal inline Color
COLOR(u8 r, u8 b, u8 g, u8 a)
{
    return {r, b, g, a};
}

internal inline Color
COLOR(u8 n)
{
    return {n, n, n, n};
}

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    f32 shininess;

    b8 has_texture;
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
    vec2 uv;
    Color color;
    f32 texture_slot;
};


struct Camera
{
    vec3 position;
    vec3 direction;
    vec3 up;
};

typedef i32 SpriteHandle;
struct Sprite
{
    char* name;
    void* data;
    i32 width, height, channels;
    u32 id;
    u32 slot;

    b8 loaded_to_gpu;
};

struct SubSprite
{
    SpriteHandle sprite_sheet;

    vec2 uvs[4];
};



struct Mesh
{
    VertexData* vertices;
    u32 num_vertices;

    u32* indices;
    u32 num_indices;

    i32 texture_index;

    Material material;
};

struct Model
{
    Mesh* meshes;
    u32 num_meshes;

    SpriteHandle loaded_sprites[8];
    u32 num_textures;
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


#define MAX_VERTICES 10000
#define MAX_INDICES 15000
#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6
#define NUM_ASCII 96

struct Renderer;
typedef void RendererProc(Renderer* ren);


enum RenderEntryType
{
    RENDER_ENTRY_TexturedQuadsEntry,
};

struct RenderEntryHeader
{
    RenderEntryType entry_type;
};

struct RenderSetup
{
    mat4 projection;
    Camera camera;

    SpriteHandle sprite_handles[32];
    u32 num_sprites;
};

struct TexturedQuadsEntry
{
    RenderSetup* render_setup;
    u32 vertex_offset;
    u32 index_offset;
    u32 num_quads;
};


const f32 LAYER_BACK = 1.0f;
const f32 LAYER_BACKMID = 0.75;
const f32 LAYER_MID= 0.5f;
const f32 LAYER_MIDFRONT = 0.25;

const f32 UI_LAYER_BACK = 0.20f;
const f32 UI_LAYER_BACKMID = 0.17f;
const f32 UI_LAYER_MID = 0.12f;
const f32 UI_LAYER_MIDFRONT = 0.7f;
const f32 UI_LAYER_FRONT = 0.2f;

const f32 LAYER_FRONT = 0.0f;

struct RenderGroup
{
    TexturedQuadsEntry* current_quads;
    RenderSetup* current_setup;

    struct Assets* assets;
    Renderer* renderer;
};


struct Renderer
{
    u8* push_buffer_base;
    u32 push_buffer_size;
    u32 push_buffer_max_size;

    VertexData vertices[MAX_VERTICES];
    u32 vertex_count;
    u32 indices[MAX_INDICES];
    u32 indices_count;

    i32 shader_program;
    u32 VBO;
    u32 EBO;
    u32 VAO;

    u32 slot;

    vec3 light_pos;
    Light light;

    i32 screen_width;
    i32 screen_height;

    struct Assets* assets;

    SpriteHandle white_sprite;
};

#endif
