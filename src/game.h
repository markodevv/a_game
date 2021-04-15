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
        };
        ButtonState buttons[9];
    };
    MouseInput mouse;
    char character;
};

inline b8
button_down(ButtonState button)
{
    return button.is_down;
}

inline b8
button_pressed(ButtonState button)
{
    return button.pressed;
}

inline b8
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
typedef ImageHandle LoadImageProc(Assets* assets, char* image_path);

typedef void RenderProc(Renderer* ren);
typedef void EndDrawFrame(RenderGroup* group);


struct Platform
{
    ReadEntireFileProc* read_entire_file;
    FreeEntireFileProc* free_file_memory;
    WriteEntireFileProc* write_entire_file;
    Load3DModelProc* load_3D_model;

    LoadImageProc* load_image;

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

    Image images[64];
    u32 num_images;
};

struct TranState
{
    MemoryArena arena;

    Assets assets;
};

struct GameState
{
    TranState trans_state;
    MemoryArena arena;
    f32 t_sine;
    i32 tone_hz;
    i32 tone_volume;

    ImageHandle minotaur_image;

    Renderer* renderer;
};

internal Image*
get_loaded_image(Assets* assets, ImageHandle handle)
{
    ASSERT(handle < (i32)assets->num_images &&
           handle >= 0);

    return assets->images + handle;
}


#define MAIN_H
#endif

// TODO LIST:
/*
-3D renderer
    -asset loading
       -material loading
       -texture loading
    -3D model render with textures
    -3D model render with material maps
    -Scenes

-Debug
    -Edit boxes
    -Scene gizmos
    -Profiler


-Collision detection

-Memory management
    -variable size allocator
*/

