#if !defined(RENDERER_H)
#define RENDERER_H
#include "stb_truetype.h"



struct VertexData
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4 color;
    f32 texture_id;
};

struct VertexData2D
{
    vec3 position;
    vec2 uv;
    vec4 color;
    f32 texture_id;
};

struct Camera
{
    vec3 position;
    vec3 direction;
    vec3 up;
};

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

struct LoadedBitmap
{
    u8* data;
    i32 width;
    i32 height;
};

#define MAX_VERTICES 100000
#define NUM_ASCII 96

struct Renderer;
typedef void RendererProc(Renderer* ren);
typedef void RendererProc(Renderer* ren);
typedef void RendererProc(Renderer* ren);

struct Renderer
{
    i32 shader_program_3D;
    VertexData vertices_start[MAX_VERTICES];
    u32 vertex_index;
    u32 VAO;
    u32 VBO;
    Camera camera;

    VertexData2D vertices_2D[MAX_VERTICES];
    i32 shader_program_2D;
    u32 vertex_index_2D;
    u32 VBO_2D;
    u32 VAO_2D;

    vec3 light_pos;

    // TODO: this should be in DebugState
    LoadedBitmap font_bitmap;
    stbtt_bakedchar char_metrics[NUM_ASCII];
    f32 font_size;
    u32 font_texture_id;
    //

    i32 screen_width;
    i32 screen_height;

    f32 light_speed;

    RendererProc* renderer_init;
    RendererProc* renderer_begin;
    RendererProc* renderer_end;
};

#endif
