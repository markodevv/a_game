#if !defined(MATH_H)
#define MATH_H

struct vec2
{
    f32 x, y;

    vec2 operator+(vec2 other);
    vec2 operator-(vec2 other);
    vec2 operator-(f32 n);
    void operator-=(f32 n);
    void operator+=(f32 n);
    void operator+=(vec2 v);
    void operator-=(vec2 v);
    vec2 operator*(vec2 other);
    vec2 operator*(f32 a);
    void operator*=(f32 a);
    void operator/=(f32 a);
    vec2 operator/(f32 a);
};

struct vec2i
{
    i32 x, y;
};

struct vec3 
{
    union
    {
        struct
        {
            f32 x, y, z;
        };
        struct
        {
            vec2 xy;
        };
        f32 data[3];
    };

    f32 &operator[](sizet i);
    vec3 operator+(vec3 other);
    vec3 operator-(vec3 other);
    vec3 operator*(vec3 other);
    vec3 operator*(f32 a);
    vec3 operator/(f32 a);
};

struct vec3i
{
    i32 x, y, z;
};

struct vec4
{
    union
    {
        struct
        {
            f32 x, y, z, w;
        };
        struct
        {
            vec3 xyz;
        };
        f32 data[4];
    };

    f32 &operator[](sizet i);
    vec4 operator+(vec4 other);
    vec4 operator-(vec4 other);
    void operator*=(f32 n);
};

struct mat4
{
    union
    {
        vec4 rows[4];
        f32 data[16];
    };

    vec4 &operator[](sizet i);
    mat4 operator*(mat4 other);
    vec4 operator*(vec4 other);
};

struct vec4i
{
    i32 x, y, z, w;
};


#endif
