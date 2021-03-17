#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Creator: Marko Bisevac $
   $Notice: (C) Copyright 2021. All Rights Reserved. $
   ======================================================================== */


#ifdef GAME_DEBUG
#include <stdio.h>

#define ASSERT(condition) \
    if (!(condition)) \
    { \
        *(int *)0 = 0; \
    } 

char DEBUG_string_buffer[512];

#define DEBUG_PRINT(msg, ...) \
    sprintf_s(DEBUG_string_buffer, msg, __VA_ARGS__); \
    DEBUG_print(DEBUG_string_buffer);

#define GAME_DEBUG_PRINT(memory, msg, ...) \
    sprintf_s(DEBUG_string_buffer, msg, __VA_ARGS__); \
    memory->DEBUG_print(DEBUG_string_buffer);

#else

#define DEBUG_PRINT(msg, ...) 
#define GAME_DEBUG_PRINT(memory, msg, ...) 


#endif

#define KYLOBYTES(n) n*1024
#define MEGABYTES(n) n*1024*1024
#define GIGABYTES(n) n*1024*1024*1024


#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))

struct ButtonState
{
    b8 is_down;
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

};

#define MAX_VERTICES 10000

struct VertexData
{
    vec3 position;
    vec2 uv;
    vec4 color;
};

struct Camera
{
    vec3 position;
    vec3 direction;
    vec3 up;
};

// NOTE(marko): Move this fucntion from here
inline mat4
camera_transform(Camera* cam)
{
    vec3 f = vec3_normalized(cam->direction);
    vec3 u = vec3_normalized(cam->up);
    vec3 r = vec3_normalized(vec3_cross(cam->up, cam->direction));
    vec3 t = cam->position;

    mat4 out =
    {
        r.x,  r.y,  r.z,  -t.x,
        u.x,  u.y,  u.z,  -t.y,
        f.x,  f.y,  f.z,  -t.z,
        0.0f, 0.0f, 0.0f,  1.0f
    };

    return out;
}

struct Renderer
{
    i32 shader_program;
    VertexData vertices_start[MAX_VERTICES];
    u32 vertex_index;
    u32 VAO;
    u32 VBO;
    Camera camera;

};



struct DebugFileResult;
typedef void DebugPrintFunc(char* text);
typedef DebugFileResult DebugReadEntireFileFunc(char* path);
typedef void DebugFreeEntireFileFunc(void* memory);
typedef b8 DebugWriteEntireFileFunc(char* file_name, i32 size, void* memory);

#define PUSH_STRUCT(mem, type) \
(type*)(((u8*)mem.permanent_storage) + mem.permanent_storage_index); \
mem.permanent_storage_index += sizeof(type) \

#define PUSH_STRUCTS(mem, type, count) \
(type*)(((u8*)mem.permanent_storage) + mem.permanent_storage_index); \
mem.permanent_storage_index += sizeof(type) * count \

// TODO: temporary until i figure out what platform independent code is.
typedef void PlatformDrawRectangle(Renderer* ren, vec2 position, vec2 scale, vec4 color);
typedef void PlatformRendererEnd(Renderer* ren);
typedef void PlatformRendererInit(Renderer* ren);

struct GameMemory
{
    b8 is_initialized;

    void* permanent_storage;
    sizet permanent_storage_size;
    sizet permanent_storage_index;

    void* temporary_storage;
    sizet temporary_storage_size;

    DebugPrintFunc* DEBUG_print;
    DebugReadEntireFileFunc* DEBUG_read_entire_file;
    DebugFreeEntireFileFunc* DEBUG_free_file_memory;
    DebugWriteEntireFileFunc* DEBUG_write_entire_file;
    
    PlatformRendererInit* renderer_init;
    PlatformDrawRectangle* draw_rectange;
    PlatformRendererEnd* frame_end;
    Renderer renderer;
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

