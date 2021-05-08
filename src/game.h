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
    DEBUG_PRINT("repeat count %i", button.repeat_count);
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
typedef b8 WriteEntireFileProc(char* file_name, i32 size, void* memory);
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


struct Transform
{
    vec2 position;
    vec2 rotation;
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

struct Entity
{
    u32 flags;

    Render render;
    Transform transform;
    vec2 velocity;
    vec2 acceleration;
    f32 inverse_mass;
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

    u32 tile_map[19][10];
    u32 tile_size;

    b8 is_free_camera;

    RenderSetup render_setup;

    u32 player_entity_index;
    Entity entities[1024];
    u32 num_entities;

    Renderer* renderer;
};

internal Sprite*
get_loaded_sprite(Assets* assets, SpriteHandle handle)
{
    ASSERT(handle < (i32)assets->num_sprites &&
           handle >= 0);

    return assets->sprites + handle;
}

internal SpriteHandle
create_sprite_asset(Assets* assets, u32 w, u32 h, u32 channels)
{
    Sprite sprite = {};
    sprite.data = PushMemory(&assets->arena, u8, (w*h));
    sprite.width = w;
    sprite.height = h;
    sprite.channels = channels;

    assets->sprites[assets->num_sprites] = sprite;
    ++assets->num_sprites;

    return (assets->num_sprites - 1);
}

#define MAIN_H
#endif

