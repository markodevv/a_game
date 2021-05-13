#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */



struct ButtonState
{
    b8 is_down;
    b8 released;
    b8 pressed;

    u8 repeat_count;
};

struct MouseInput
{
    vec2 position;
    i32 wheel_delta;
    b8 moved;
};

struct GameInput
{
    union
    {
        struct
        {
            ButtonState move_left;
            ButtonState move_right;
            ButtonState move_up;
            ButtonState move_down;
            ButtonState move_forward;
            ButtonState move_back;
            ButtonState pause_button;
            ButtonState left_mouse_button;
            ButtonState right_mouse_button;
            ButtonState backspace;
            ButtonState escape;
        };
        ButtonState buttons[11];
    };
    MouseInput mouse;
    char character;
};

internal inline b8
button_down(ButtonState button)
{
    return button.is_down;
}

internal inline b8
button_repeat(ButtonState button)
{
    PRINT("repeat count %i", button.repeat_count);
    return (button.repeat_count > 0);
}

internal inline b8
button_pressed(ButtonState button)
{
    return button.pressed;
}

internal inline b8
button_released(ButtonState button)
{
    return button.released;
}


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

enum ModelId
{
    M_ID_ZOMBIE0,
    M_ID_ZOMBIE1,
    M_ID_DUMMY,
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
    vec2 scale;
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

    f32 inverse_mass;
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

struct ParticleSystem
{
    f32 age_min;
    f32 age_max;

    vec2 min_vel;
    vec2 max_vel;

    vec2 position;

    f32 drag;

    Particle* particles;
    u32 particle_index;
    u32 max_particles;
};

struct TranState
{
    MemoryArena arena;

    Assets assets;
};

enum ComponentType
{
    COMPONENT_Transform = 0,
    COMPONENT_Render = 1,
};

struct GameState
{
    TranState trans_state;
    MemoryArena arena;
    f32 t_sine;
    i32 tone_hz;
    i32 tone_volume;

    SpriteHandle minotaur_sprite;
    SubSprite hero_sprite;
    SpriteHandle hero_sprite_sheet;
    SpriteHandle backgroud_sprite;

    u32 tile_map[19][10];
    u32 tile_size;

    b8 is_free_camera;


    u32 player_entity_index;
    Entity entities[1024];
    u32 num_entities;

    ParticleSystem particle_system;

    RenderSetup render_setup;
    Renderer* renderer;
};

#define MAIN_H
#endif

