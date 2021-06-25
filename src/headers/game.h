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

typedef struct Transform Transform;
struct Transform
{
vec2 position;
vec2 scale;
vec2 rotation;
};

typedef struct Rigidbody Rigidbody;
struct Rigidbody
{
vec2 velocity;
vec2 acceleration;
f32 mass;
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
f32 drag;
Render render;
u32 particle_spawn_rate;
vec2 position;
Particle* particles;
u32 particle_index;
u32 max_particles;
};

#define COMPONENT_Render 1<<0
#define COMPONENT_Rigidbody 1<<1
#define COMPONENT_Transform 1<<2
#define COMPONENT_ParticleEmitter 1<<3
#define ENTITY_MAX 10000
typedef u32 EntityId;
typedef struct WorldState WorldState;
struct WorldState
{
Render renders[ENTITY_MAX];
Rigidbody rigidbodys[ENTITY_MAX];
Transform transforms[ENTITY_MAX];
u32 entity_masks[ENTITY_MAX];
ParticleEmitter particle_emitters[ENTITY_MAX];
u32 num_entities;
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
EntityId particle_emitter;
WorldState* world;
Renderer renderer;
};

