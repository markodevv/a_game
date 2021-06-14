typedef struct Color Color;
struct Color
{
u8 r;
u8 g;
u8 b;
u8 a;
};

typedef struct VertexData VertexData;
struct VertexData
{
vec3 position;
vec2 uv;
Color color;
f32 texture_slot;
};

typedef struct Camera Camera;
struct Camera
{
vec3 position;
vec3 direction;
vec3 up;
};

typedef i32 SpriteHandle;
typedef struct Sprite Sprite;
struct Sprite
{
char* name;
void* data;
i32 width;
i32 height;
i32 channels;
u32 id;
u32 slot;
b8 loaded_to_gpu;
};

typedef struct SubSprite SubSprite;
struct SubSprite
{
SpriteHandle sprite_sheet;
vec2 uvs[4];
};

typedef struct CharMetric CharMetric;
struct CharMetric
{
u16 x0;
u16 y0;
u16 x1;
u16 y1;
f32 xoff;
f32 yoff;
f32 xadvance;
};

typedef struct Font Font;
struct Font
{
SubSprite* char_subsprites;
CharMetric* char_metrics;
u16 num_chars;
u32 font_size;
SpriteHandle font_sprite_handle;
};

enum RenderEntryType 
{
RENDER_ENTRY_TexturedQuadsEntry,
RENDER_ENTRY_QuadEntry,
};

typedef struct RenderEntryHeader RenderEntryHeader;
struct RenderEntryHeader
{
RenderEntryType entry_type;
};

typedef struct QuadEntry QuadEntry;
struct QuadEntry
{
vec2 position;
vec2 size;
Color color;
SpriteHandle sprite_handle;
SubSprite* subsprite;
u32 layer;
};

typedef struct SortElement SortElement;
struct SortElement
{
u32 entry_offset;
u32 key;
};

typedef struct RenderSetup RenderSetup;
struct RenderSetup
{
mat4 projection;
Camera camera;
SpriteHandle sprite_handles[32];
u32 num_sprites;
};

typedef struct TexturedQuadsEntry TexturedQuadsEntry;
struct TexturedQuadsEntry
{
RenderSetup* render_setup;
u32 vertex_offset;
u32 index_offset;
u32 num_quads;
};

typedef struct Assets Assets;
struct Assets
{
MemoryArena arena;
Sprite sprites[64];
u32 num_sprites;
};

typedef struct RenderGroup RenderGroup;
struct RenderGroup
{
RenderSetup setup;
Assets* assets;
u8* push_buffer_base;
u32 push_buffer_size;
u32 push_buffer_capacity;
u32 sort_element_count;
RenderGroup* next;
};

typedef struct Renderer Renderer;
struct Renderer
{
VertexData vertices[10000];
u32 vertex_count;
u32 indices[15000];
u32 indices_count;
i32 shader_program;
u32 VBO;
u32 EBO;
u32 VAO;
u32 slot;
vec3 light_pos;
i32 screen_width;
i32 screen_height;
Assets* assets;
RenderGroup* render_groups;
SpriteHandle white_sprite;
};

typedef void RendererProc(Renderer* ren);
