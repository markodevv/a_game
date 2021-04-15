#if !defined(RENDERER_H)
#define RENDERER_H

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
    vec3 normal;
    vec2 uv;
    f32 texture_id;
};

struct VertexData2D
{
    vec2 position;
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

typedef i32 ImageHandle;
struct Image
{
    char* name;
    void* data;
    i32 width, height, channels;
    u32 id;
    u32 slot;

    b8 loaded_to_gpu;
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

    ImageHandle loaded_images[8];
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
#define NUM_ASCII 96

struct Renderer;
typedef void RendererProc(Renderer* ren);

// push group entrys
// sort group
// send it to gpu
// gpu loads models that are not loaded and sets nececery states

enum RenderEntryType
{
    RENDER_ENTRY_TexturedQuad,
    RENDER_ENTRY_Quad,
};

struct RenderEntryHeader
{
    RenderEntryType entry_type;
};

struct Quad
{
    vec2 position;
    vec2 size;
    vec4 color;
};

struct TexturedQuad
{
    ImageHandle image;

    vec2 position;
    vec2 size;
    vec4 color;
};

struct RenderGroup
{
    mat4 projection;

    u8* push_buffer_base;
    u32 push_buffer_size;
    u32 push_buffer_max_size;

    struct Assets* assets;
    Renderer* renderer;
};


struct Renderer
{
    i32 shader_program_3D;
    u32 VBO, VAO;
    Camera camera;

    VertexData2D* vertices_2D;
    i32 shader_program_2D;
    u32 vertex_count_2D;
    u32 VBO_2D;
    u32 VAO_2D;

    mat4 view;

    u32 slot;

    vec3 light_pos;
    Light light;

    i32 screen_width;
    i32 screen_height;
};

#endif
