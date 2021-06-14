typedef struct GameMemory GameMemory;
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

typedef struct GameSoundBuffer GameSoundBuffer;
struct GameSoundBuffer
{
i32 samples_per_sec;
i32 sample_count;
i16* samples;
};

typedef struct Render Render;
struct Render
{
SpriteHandle sprite;
vec2 size;
Color color;
};

enum EntityFlag 
{
ENTITY_FLAG_ENABLED,
};

typedef struct Rigidbody Rigidbody;
struct Rigidbody
{
vec2 velocity;
vec2 acceleration;
f32 mass;
};

typedef struct Entity Entity;
struct Entity
{
u32 flags;
Render render;
vec2 position;
Rigidbody rigidbody;
};

typedef struct Particle Particle;
struct Particle
{
vec2 position;
Rigidbody rigidbody;
};

typedef struct ParticleEmitter ParticleEmitter;
struct ParticleEmitter
{
vec2 min_vel;
vec2 max_vel;
vec2 position;
f32 drag;
u32 particle_spawn_rate;
Render render;
Particle* particles;
u32 particle_index;
u32 max_particles;
};

enum ComponentType 
{
COMPONENT_Transform,
COMPONENT_Render,
};

typedef struct GameState GameState;
struct GameState
{
MemoryArena arena;
MemoryArena transient_arena;
Assets assets;
f32 t_sine;
i32 tone_hz;
i32 tone_volume;
SubSprite hero_sprite;
SpriteHandle minotaur_sprite;
SpriteHandle hero_sprite_sheet;
SpriteHandle backgroud_sprite;
u32 tile_map[19][10];
u32 tile_size;
b8 is_free_camera;
ParticleEmitter particle_emitter;
u32 player_entity_index;
Entity entities[1024];
u32 num_entities;
RenderSetup render_setup;
Renderer renderer;
};

