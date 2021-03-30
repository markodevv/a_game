#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */

#include "debug.h"

#define KYLOBYTES(n) n*1024
#define MEGABYTES(n) n*1024*1024
#define GIGABYTES(n) n*1024*1024*1024

#define ARRAY_COUNT(Array) (sizeof(Array) / sizeof((Array)[0]))

internal inline u32
string_length(char* string)
{
    u32 out = 0;
    while(string)
    {
        out++;
    }

    return out;
}

struct ButtonState
{
    b8 is_down;
};

struct MouseInput
{
    vec2 position;
    i32 wheel_delta;
    b8 moved;
    ButtonState left_button;
    ButtonState right_button;
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
        };
        ButtonState buttons[6];
    };
    MouseInput mouse;
};

inline b8
button_pressed(ButtonState button)
{
    return button.is_down;
}

inline b8
button_released(ButtonState button)
{
    return !button.is_down;
}

struct DebugFileResult;
typedef DebugFileResult DebugReadEntireFileFunc(char* path);
typedef void DebugFreeEntireFileFunc(void* memory);
typedef b8 DebugWriteEntireFileFunc(char* file_name, i32 size, void* memory);


struct MemoryArena
{
    sizet used;
    sizet size;
    u8* base;
};

struct UiItemId
{
    void* id;
};

struct DebugState
{
    MemoryArena arena;
    f32 game_fps;

    UiItemId hot_item;
    UiItemId active_item;
};

#define ALLOCATE(arena, type, ...)  (type *)push_memory(arena, sizeof(type), __VA_ARGS__)

inline void*
push_memory(MemoryArena* arena, sizet type_size, sizet count = 1)
{
    ASSERT((arena->size - arena->used) > (type_size * count));
    u8* out = arena->base + arena->used;
    arena->used += type_size * count;

    return out;
}

inline void
init_arena(MemoryArena* arena, sizet size, void* base)
{
    arena->used = 0;
    arena->size = size;
    arena->base = (u8*)base;
}

inline void
sub_arena(MemoryArena* main, MemoryArena* sub, sizet size)
{
    u8* base = ALLOCATE(main, u8, size);
    init_arena(sub, size, base);
}

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

    DebugState* debug;
#endif

    i32 screen_width;
    i32 screen_height;
    RenderCommands render_commands;
};


struct GameSoundBuffer
{
    i32 samples_per_sec;
    i32 sample_count;
    i16* samples;
};

struct GameState
{
    f32 t_sine;
    i32 tone_hz;
    i32 tone_volume;

    MemoryArena arena;
    Renderer renderer;
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
-3D batched renderer
-collision detection
-asset loading
-memory management
*/

