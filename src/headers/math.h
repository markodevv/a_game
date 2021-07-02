struct vec2
{
f32 x;
f32 y;
f32& operator[](sizet i);
};
struct vec2i
{
i32 x;
i32 y;
};
struct vec3
{
f32 x;
f32 y;
f32 z;
f32& operator[](sizet i);
};
struct vec3i
{
i32 x;
i32 y;
i32 z;
};
struct vec4
{
f32 x;
f32 y;
f32 z;
f32 w;
f32& operator[](sizet i);
};
struct mat4
{
vec4 rows[4];
vec4& operator[](sizet i);
};
struct vec4i
{
i32 x;
i32 y;
i32 z;
i32 w;
};
