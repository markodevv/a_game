#if !defined(RENDERER_H)
#define RENDERER_H

struct Color
{
    u8 r, g, b, a;
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

struct CharMetric
{
   u16 x0,y0,x1,y1;
   f32 xoff,yoff,xadvance;
};

struct Font

{
    SubSprite* char_subsprites;
    CharMetric* char_metrics;
    u16 num_chars;
    i32 font_size;
    SpriteHandle font_sprite_handle;
};







struct Renderer;
typedef void RendererProc(Renderer* ren);


enum RenderEntryType
{
    RENDER_ENTRY_TexturedQuadsEntry,
    RENDER_ENTRY_QuadEntry,
};

struct RenderEntryHeader
{
    RenderEntryType entry_type;
};

struct QuadEntry
{
    vec2 position;
    vec2 size;
    Color color;
    SpriteHandle sprite_handle;
    SubSprite* subsprite;

    u32 layer;
};

struct SortElement
{
    u32 entry_offset;
    u32 key;
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



struct RenderGroup
{
    RenderSetup setup;
    Renderer* renderer;
    struct Assets* assets;

    u8* push_buffer_base;
    u32 push_buffer_size;
    u32 push_buffer_capacity;

    u32 sort_element_count;

    RenderGroup* next;
};



struct Renderer
{
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

    i32 screen_width;
    i32 screen_height;

    struct Assets* assets;

    RenderGroup* render_groups;

    SpriteHandle white_sprite;
};

#endif
