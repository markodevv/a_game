struct Color
{
u8 r;
u8 g;
u8 b;
u8 a;
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
typedef i32 SpriteHandle;;
enum SpriteType 
{
TYPE_SPRITE,
TYPE_SUBSPRITE,
};

struct Sprite
{
union 
{
struct 
{
char* name;
void* data;
i32 width;
i32 height;
i32 channels;
};
struct 
{
SpriteHandle main_sprite;
vec2 uvs[4];
};
};
u32 id;
u32 slot;
SpriteType type;
};
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
struct Font
{
SpriteHandle* sprite_handles;
CharMetric* char_metrics;
u16 num_chars;
u32 font_size;
SpriteHandle font_sprite_handle;
};
enum RenderEntryType 
{
RENDER_ENTRY_QuadEntry,
RENDER_ENTRY_TriangleEntry,
};

struct RenderEntryHeader
{
RenderEntryType entry_type;
};
enum ShaderId 
{
SHADER_ID_NORMAL,
SHADER_ID_HUE_QUAD,
SHADER_ID_SB_QUAD,
NUM_SHADERS,
};

struct QuadEntry
{
vec3 position;
vec2 size;
Color color;
SpriteHandle sprite_handle;
ShaderId shader_id;
};
struct TriangleEntry
{
vec3 points[3];
Color color;
SpriteHandle sprite_handle;
ShaderId shader_id;
};
struct SortElement
{
u32 entry_offset;
u32 key;
};
struct RenderSetup
{
mat4 projection;
};
enum UniformType 
{
UNIFORM_F32,
UNIFORM_F64,
UNIFORM_I32,
UNIFORM_U32,
UNIFORM_VEC2,
UNIFORM_VEC3,
UNIFORM_VEC4,
UNIFORM_MAT4,
UNIFORM_TEXTURE2D,
};

struct UniformTypeInfo
{
UniformType type;
u32 size;
};
struct Uniform
{
UniformTypeInfo type_info;
void* data;
char* name;
Uniform* next;
};
struct Shader
{
u32 bind_id;
Uniform* uniforms;
i32 num_uniforms;
};
struct Assets
{
MemoryArena arena;
SpriteHandle* loaded_sprite_queue;
u32 num_queued_sprites;
Sprite* sprites;
u32 num_sprites;
Shader shaders[NUM_SHADERS];
};
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
#define MAX_VERTICES 100000
#define MAX_INDICES 150000
struct Renderer
{
VertexData vertices[MAX_VERTICES];
u32 vertex_count;
u32 indices[MAX_INDICES];
u32 indices_count;
u32 VBO;
u32 EBO;
u32 VAO;
Camera camera;
u32 slot;
vec3 light_pos;
i32 screen_width;
i32 screen_height;
Assets* assets;
RenderGroup* render_groups;
SpriteHandle white_sprite;
};
typedef void RendererProc(Renderer* ren);;
