struct GameMemory
{
b8 is_initialized;
void* permanent_storage;
sizet permanent_storage_size;
void* temporary_storage;
sizet temporary_storage_size;
DebugState* debug;
Platform platform;
i32 screen_width;
i32 screen_height;
};
struct GameSoundBuffer
{
i32 samples_per_sec;
i32 sample_count;
i16* samples;
};
struct Render
{
SpriteHandle sprite;
vec2 size;
Color color;
f32 layer;
};
struct Transform
{
vec2 position;
vec2 scale;
vec2 rotation;
};
struct Rigidbody
{
vec2 velocity;
vec2 acceleration;
f32 mass;
};
struct Particle
{
vec2 position;
Rigidbody rigidbody;
};
struct ParticleEmitter
{
vec2 min_vel;
vec2 max_vel;
f32 drag;
Render render;
u32 particle_spawn_rate;
vec2 position;
Particle* particles;
u32 particle_index;
u32 max_particles;
};
#define ENTITY_MAX 10000
typedef u64 EntityId;
struct ArchType
{
Array* type;
Array** components;
};
struct MapBucket
{
Array* components;
EntityId id;
char* name;
};
struct WorldState
{
MapBucket* entity_map;
Array* removed_ids;
u32 component_id;
};
struct GameState
{
MemoryArena arena;
MemoryArena flush_arena;
Assets assets;
f32 t_sine;
i32 tone_hz;
i32 tone_volume;
SpriteHandle hero_sprite;
SpriteHandle minotaur_sprite;
SpriteHandle hero_sprite_sheet;
SpriteHandle backgroud_sprite;
SpriteHandle goblin_sprite_sheet;
SpriteHandle goblin_sprite;
u32 tile_map[19][10];
u32 tile_size;
b8 is_free_camera;
EntityId particle_emitter;
WorldState world;
Renderer renderer;
};
typedef void (*GameMainLoopProc)(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input);
