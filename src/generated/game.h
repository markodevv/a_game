#pragma once

// @Print
typedef struct Render Render;
struct Render
{
    // @NoPrint
    SpriteID sprite;
    Color color;
    f32 layer;
};
// @Print
typedef struct Transform Transform;
struct Transform
{
    vec2 position;
    vec2 scale;
    vec2 rotation;
};
// @Print
typedef struct Rigidbody Rigidbody;
struct Rigidbody
{
    vec2 velocity;
    vec2 acceleration;

    f32 mass;
};
// @Print
typedef struct Particle Particle;
struct Particle
{
    vec2 position;
    vec2 velocity;
    vec2 acceleration;
    vec2 size;
};
typedef struct ParticleEmitter ParticleEmitter;
struct ParticleEmitter
{
    vec2 min_vel;
    vec2 max_vel;

    f32 drag;

    Color color;
    vec2 size;

    u32 particle_spawn_rate;

    vec2 position;

    Particle *particles;
    u32 particle_index;
    u32 max_particles;
};
typedef struct Entity Entity;
struct Entity
{
    vec3 position;
    vec3 rotation;
    vec3 scale;

    SpriteID spriteID;
    MeshEnum meshID;
    Color color;
    Layer layer;

    vec2 velocity;
    vec2 acceleration;
    f32 mass;

    u32 flags;
};
enum
{
    RENDER = (1<<0),
    PHYSICS = (1<<1),
};
typedef unsigned int EntityFlag;
#define ENTITY_MAX (16384)
typedef struct WorldState WorldState;
struct WorldState
{
    u16 grid_width;
    u16 grid_height;

    RenderGroup *render_group;

    Entity entities[ENTITY_MAX];
    u32 entity_count;
};
typedef struct GameState GameState;
struct GameState
{
    MemoryArena arena;
    MemoryArena flush_arena;

    f32 delta_time;

    Assets assets;

    Font font;
    f32 t_sine;
    i32 tone_hz;
    i32 tone_volume;

    u32 tile_size;

    b32 is_free_camera;

    WorldState world;

    Renderer2D *renderer;
    RenderGroup *render_group;
};
