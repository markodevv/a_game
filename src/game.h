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
#include <stdio.h>

#define ASSERT(condition) \
    if (!(condition)) \
    { \
        *(int *)0 = 0; \
    } 

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*
#define ALERT_MSG(msg, ...) \
    printf(ANSI_COLOR_RED msg ANSI_COLOR_RESET, __VA_ARGS__)
#define WARN_MSG(msg, ...) \
    printf(ANSI_COLOR_YELLOW msg ANSI_COLOR_RESET, __VA_ARGS__)
#define NORMAL_MSG(msg, ...) \
    printf(ANSI_COLOR_GREEN msg ANSI_COLOR_RESET, __VA_ARGS__)
    */

global_variable char DEBUG_string_buffer[512];

#define ALERT_MSG(msg, ...) \
    sprintf_s(DEBUG_string_buffer, msg, __VA_ARGS__); \
    OutputDebugString(DEBUG_string_buffer)
#define WARN_MSG(msg, ...) \
    sprintf_s(DEBUG_string_buffer, msg, __VA_ARGS__); \
    OutputDebugString(DEBUG_string_buffer)
#define NORMAL_MSG(msg, ...) \
    sprintf_s(DEBUG_string_buffer, msg, __VA_ARGS__); \
    OutputDebugString(DEBUG_string_buffer)

#else

#define ASSERT(condition) 
#define ALERT_MSG(msg, ...) 
#define WARN_MSG(msg, ...)
#define NORMAL_MSG(msg, ...)

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
    f32 t_sine;
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

#define PLATFORM_API __declspec(dllexport)

#define MAIN_H
#endif
