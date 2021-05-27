#if !defined(MAIN_H)
#define MAIN_H
/* =========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */



struct FileResult
{
    void* data;
    i32 size;
};

typedef FileResult ReadEntireFileProc(char* path);
typedef void FreeEntireFileProc(void* memory);
typedef b8 WriteEntireFileProc(char* file_name, u32 size, void* memory);
typedef Model Load3DModelProc(struct Assets* assets, char* name);
typedef SpriteHandle LoadSpriteProc(Assets* assets, char* sprite_path);

typedef void RenderProc(Renderer* ren);
typedef void EndDrawFrame(Renderer* ren);


struct Platform
{
    ReadEntireFileProc* read_entire_file;
    FreeEntireFileProc* free_file_memory;
    WriteEntireFileProc* write_entire_file;

    RenderProc* init_renderer;
    EndDrawFrame* end_frame;
};


struct DebugState;
struct GameMemory
{
    b8 is_initialized;

    void* permanent_storage;
    sizet permanent_storage_size;

    void* temporary_storage;
    sizet temporary_storage_size;

#ifdef GAME_DEBUG
    DebugState* debug;
#endif
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


struct Assets
{
    MemoryArena arena;
    Model models[3];

    Sprite sprites[64];
    u32 num_sprites;
};


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

struct Rigidbody
{
    vec2 velocity;
    vec2 acceleration;

    f32 mass;
};

struct Entity
{
    u32 flags;

    Render render;

    vec2 position;
    Rigidbody rigidbody;
};


struct Particle
{
    vec2 position;
    Rigidbody rigidbody;
};

struct ParticleEmiter
{
    vec2 min_vel;
    vec2 max_vel;

    vec2 position;

    f32 drag;

    Render render;

    Particle* particles;
    u32 particle_index;
    u32 max_particles;
};

enum ComponentType
{
    COMPONENT_Transform = 0,
    COMPONENT_Render = 1,
};

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

    ParticleEmiter particle_emiter;

    u32 player_entity_index;
    Entity entities[1024];
    u32 num_entities;

    RenderSetup render_setup;
    Renderer renderer;
};

typedef void (*GameMainLoop)(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input);

#endif
