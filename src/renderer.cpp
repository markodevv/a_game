#define MAX_VERTICES 10000
#define MAX_INDICES 15000
#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6
#define NUM_ASCII 96

const u32 LAYER_BACK = 0;
const u32 LAYER_BACKMID = 1;
const u32 LAYER_MID = 2;
const u32 LAYER_MIDFRONT = 3;
const u32 LAYER_FRONT = 4;

internal inline Color
COLOR(u8 r, u8 b, u8 g, u8 a)
{
    return {r, b, g, a};
}

internal inline Color
COLOR(u8 n)
{
    return {n, n, n, n};
}

internal mat4
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
