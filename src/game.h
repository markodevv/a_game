#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */



#define ARRAY_COUNT(Array) (sizeof(Array) / sizeof((Array)[0]))

#define KYLOBYTES(n) n*1024
#define MEGABYTES(n) n*1024*1024
#define GIGABYTES(n) n*1024*1024*1024


internal inline u32
string_length(char* string)
{
    u32 len = 0;
    while(string[len])
    {
        len++;
    }

    return len;
}

internal inline char*
string_copy(MemoryArena* arena, char* str)
{
    u32 len = string_length(str);

    char* out = PushMemory(arena, char, len+1);
    out[len] = '\0';

    for (sizet i = 0; i < len; ++i)
    {
        out[i] = str[i];
    }

    return out;
}

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


struct DebugFileResult;
typedef DebugFileResult DebugReadEntireFileFunc(char* path);
typedef void DebugFreeEntireFileFunc(void* memory);
typedef b8 DebugWriteEntireFileFunc(char* file_name, i32 size, void* memory);
typedef Model* DebugLoad3DModel(MemoryArena* arena, char* name);


internal void
end_temporary_memory(TemporaryArena* temp_arena)
{
    MemoryArena *arena = temp_arena->arena;
    ASSERT(arena->used >= temp_arena->used);
    arena->used = temp_arena->used;
}

struct DebugState;
struct GameMemory
{
    b8 is_initialized;

    void* permanent_storage;
    sizet permanent_storage_size;

    void* temporary_storage;
    sizet temporary_storage_size;

#ifdef GAME_DEBUG
    DebugReadEntireFileFunc* DEBUG_read_entire_file;
    DebugFreeEntireFileFunc* DEBUG_free_file_memory;
    DebugWriteEntireFileFunc* DEBUG_write_entire_file;
    DebugLoad3DModel* DEBUG_load_3D_model;
    DebugState* debug;
#endif

    i32 screen_width;
    i32 screen_height;
    RendererProc* renderer_init;
    RendererProc* renderer_begin;
    RendererProc* renderer_end;
};


struct GameSoundBuffer
{
    i32 samples_per_sec;
    i32 sample_count;
    i16* samples;
};

struct GameState
{
    MemoryArena arena;
    MemoryArena temporary_arena;
    f32 t_sine;
    i32 tone_hz;
    i32 tone_volume;

    Renderer* renderer;
};


#ifdef GAME_DEBUG

struct DebugFileResult
{
    void* data;
    i32 size;
};

internal DebugFileResult DEBUG_read_entire_file(char* path);
internal void DEBUG_free_file_memory(void* memory);
internal b8 DEBUG_write_entire_file(char* file_name, i32 size, void* memory);

#endif

#define PLATFORM_API __declspec(dllexport)

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

