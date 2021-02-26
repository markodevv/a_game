#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */
#define internal static 
#define global_variable static 
#define local_persist static

#include <stdint.h>
#include <stddef.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef size_t sizet;


#ifdef GAME_DEBUG

#define ASSERT(condition, message) \
    if (!(condition)) { \
        printf(message); \
        printf("\n"); \
        assert(condition); \
    } 
#else

#define ASSERT(condifiton, message) 

#endif  

#define KYLOBYTES(n) n*1024
#define MEGABYTES(n) n*1024*1024
#define GIGABYTES(n) n*1024*1024*1024

#define PI 3.14159265359f

struct ButtonState
{
    b8 pressed;
};

struct GameInput
{
    ButtonState move_left;
    ButtonState move_right;
    ButtonState move_up;
    ButtonState move_down;
};

struct GameMemory
{
    b8 is_initialized;
    void* permanent_storage;
    sizet permanent_storage_size;

    void* temporary_storage;
    sizet temporary_storage_size;
};


struct GameSoundBuffer
{
    i32 samples_per_sec;
    i32 sample_count;
    i16* samples;
};

struct GameState
{
    i32 tone_hz;
    i32 tone_volume;
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

#define WIN32_DECLSPEC __declspec(dllexport)


/*
WIN32_DECLSPEC void game_update(f32 delta_time, GameMemory* memory, GameSoundBuffer* game_sound, GameInput* input);
WIN32_DECLSPEC void game_render();
*/

#define MAIN_H
#endif
