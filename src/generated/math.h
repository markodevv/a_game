typedef struct vec2 vec2;
struct vec2
{
    f32 x;
    f32 y;
};
typedef struct vec2i vec2i;
struct vec2i
{
    i32 x;
    i32 y;
};
typedef struct vec3 vec3;
struct vec3
{
    f32 x;
    f32 y;
    f32 z;
};
typedef struct vec3i vec3i;
struct vec3i
{
    i32 x;
    i32 y;
    i32 z;
};
typedef struct vec4 vec4;
struct vec4
{
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};
typedef struct mat4 mat4;
struct mat4
{
    vec4 rows[4];
};
typedef struct vec4i vec4i;
struct vec4i
{
    i32 x;
    i32 y;
    i32 z;
    i32 w;
};
