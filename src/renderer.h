#if !defined(RENDERER_H)
#define RENDERER_H

// NOTE: got this from "stb_truetype.h"
struct StbBakedChar
{
   unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
   float xoff,yoff,xadvance;
};


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
    vec2 position;
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

struct Renderer;
typedef void PlatformDrawRectangle(Renderer* ren, vec2 position, vec2 scale, vec4 color);
typedef void PlatformRendererEnd(Renderer* ren);
typedef void PlatformRendererInit(Renderer* ren);
typedef void PlatformDrawCube(Renderer* ren, vec3 position, vec3 scale, vec4 color);

#define MAX_VERTICES 10000
struct Renderer
{
    i32 shader_program_3D;
    VertexData vertices_start[MAX_VERTICES];
    u32 vertex_index;
    u32 VAO;
    u32 VBO;
    u32 light_VAO;
    Camera camera;

    VertexData2D vertices_2D[MAX_VERTICES];
    i32 shader_program_2D;
    u32 vertex_index_2D;
    u32 VBO_2D;
    u32 VAO_2D;

    
    u32 font_texture_id;
    StbBakedChar char_metrics[96];
    u8* temp_bitmap;

    PlatformRendererInit* renderer_init;
    PlatformDrawRectangle* draw_rectangle;
    PlatformRendererEnd* frame_end;
    PlatformDrawCube* draw_cube;
};

#endif
