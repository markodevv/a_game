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


struct GameMemory
{
    void* permanent_storage;
    sizet permanent_storage_size;

    void* temporary_storage;
    sizet temporary_storage_size;
};

struct GLFWwindow;
struct GameState
{
    GLFWwindow* window;
};

struct FileHandle
{
    void* data;
    sizet size;
};


#ifdef GAME_DEBUG

internal void* DEBUG_read_entire_file(const char* path);
internal void DEBUG_free_file_memory(void* memory);
internal b8 DEBUG_write_entire_file(const char* file_name, sizet size, void* memory);

#endif

internal void game_update_and_render(GameMemory* memory);

#define MAIN_H
#endif
