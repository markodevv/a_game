#pragma once

typedef struct GameMemory GameMemory;
struct GameMemory
{
    b8 is_initialized;

    void *permanent_storage;
    sizet permanent_storage_size;

    void *temporary_storage;
    sizet temporary_storage_size;

    void *debug_storage;
    sizet debug_storage_size;

    DebugState *debug;

    Platform platform;

    i32 screen_width;
    i32 screen_height;
};
typedef struct GameSoundBuffer GameSoundBuffer;
struct GameSoundBuffer
{
    i32 samples_per_sec;
    i32 sample_count;
    i16 *samples;
};
// @Print
typedef struct Render Render;
struct Render
{
    u32 sprite;
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
#define ENTITY_MAX (10000)
#define NUM_COMPONENTS (4)
typedef struct ComponentInfo ComponentInfo;
struct ComponentInfo
{
    u32 size;
    char *name;
};
typedef struct Component Component;
struct Component
{
    void *data;
};
typedef struct System System;
struct System
{
    u32 id;
    Array *entities;
    u64 signature;
    SystemFunc Update;
    HashMap entity_id_to_array_id;
    char *name;
};
typedef struct WorldState WorldState;
struct WorldState
{
    Component components[NUM_COMPONENTS];
    ComponentInfo component_infos[NUM_COMPONENTS];
    u64 entity_masks[ENTITY_MAX];
    Array *removed_ids;

    System systems[32];
    u32 num_systems;

    u32 entity_count;
    u32 component_count;

    u16 grid_width;
    u16 grid_height;

    RenderGroup *render_group;
};
typedef struct GameState GameState;
struct GameState
{
    MemoryArena arena;
    MemoryArena flush_arena;

    f32 delta_time;

    Assets assets;

    f32 t_sine;
    i32 tone_hz;
    i32 tone_volume;

    u32 tile_size;

    b8 is_free_camera;

    WorldState world;

    Renderer2D renderer;
    RenderGroup *render_group;
};
