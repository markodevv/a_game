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

    Font font;
    f32 t_sine;
    i32 tone_hz;
    i32 tone_volume;

    u32 tile_size;

    b8 is_free_camera;

    WorldState world;

    Renderer2D renderer;
    RenderGroup *render_group;
};
