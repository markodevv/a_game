#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */

#include "renderer.h"

#ifdef GAME_DEBUG
#include <stdio.h>

#define ASSERT(condition) \
    if (!(condition)) *(int *)0 = 0 

char DEBUG_string_buffer[5120];

#define DEBUG_PRINT(msg, ...) \
    sprintf_s(DEBUG_string_buffer, msg, __VA_ARGS__); \
    DEBUG_print(DEBUG_string_buffer);

#define GAME_DEBUG_PRINT(memory, msg, ...) \
    sprintf_s(DEBUG_string_buffer, msg, __VA_ARGS__); \
    memory->DEBUG_print(DEBUG_string_buffer);

#else

#define ASSERT(condition)
#define DEBUG_PRINT(msg, ...) 
#define GAME_DEBUG_PRINT(memory, msg, ...) 


#endif

#define KYLOBYTES(n) n*1024
#define MEGABYTES(n) n*1024*1024
#define GIGABYTES(n) n*1024*1024*1024


#define ARRAY_COUNT(Array) (sizeof(Array) / sizeof((Array)[0]))

struct ButtonState
{
    b8 is_down;
};

struct MouseScroll
{
    b8 scrolled;
    i32 wheel_delta;
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
    MouseScroll mouse_scroll;

};


struct DebugFileResult;
typedef void DebugPrintFunc(char* text);
typedef DebugFileResult DebugReadEntireFileFunc(char* path);
typedef void DebugFreeEntireFileFunc(void* memory);
typedef b8 DebugWriteEntireFileFunc(char* file_name, i32 size, void* memory);

#define ALLOCATE(arena, type) \
(type*)(((u8*)arena.permanent_storage) + arena.permanent_storage_index); \
ASSERT((arena.permanent_storage_size - arena.permanent_storage_index) > sizeof(type)); \
arena.permanent_storage_index += sizeof(type); 

#define ALLOCATE_ARRAY(arena, type, count) \
(type*)(((u8*)arena.permanent_storage) + (arena.permanent_storage_index)); \
ASSERT((arena.permanent_storage_size - arena.permanent_storage_index) > (sizeof(type)*count)); \
arena.permanent_storage_index += (sizeof(type)*count); 

#define ALLOCATE_TEMP(arena, type) \
(type*)(((u8*)arena.temporary_storage) + arena.temporary_storage_index); \
ASSERT((arena.temporary_storage_size - arena.temporary_storage_index) > sizeof(type)); \
arena.temporary_storage_index += (sizeof(type));

#define ALLOCATE_TEMP_ARRAY(arena, type, count) \
(type*)(((u8*)arena.temporary_storage) + (arena.temporary_storage_index)); \
ASSERT((arena.temporary_storage_size - arena.temporary_storage_index) > (sizeof(type)*count)); \
arena.temporary_storage_index += (sizeof(type)*count); 


struct GameMemory
{
    b8 is_initialized;

    void* permanent_storage;
    sizet permanent_storage_size;
    sizet permanent_storage_index;

    void* temporary_storage;
    sizet temporary_storage_size;
    sizet temporary_storage_index;

    DebugPrintFunc* DEBUG_print;
    DebugReadEntireFileFunc* DEBUG_read_entire_file;
    DebugFreeEntireFileFunc* DEBUG_free_file_memory;
    DebugWriteEntireFileFunc* DEBUG_write_entire_file;
    
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
internal void DEBUG_print(char* text);

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

