#if !defined(MATH_H)
#define MATH_H

#define PI_F 3.14159265359f
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#define CLAMP(a, min, max) (((a)<=(max)) ? ( ((a)<(min)) ? (min) : (a) ) : (max))

struct vec2
{
    f32 x, y;

    vec2 operator/(f32 a);
    vec2 operator+(vec2& other);
    vec2 operator-(vec2& other);
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
            f32 z;
        };
        f32 data[3];
    };

    f32 &operator[](sizet i);
    vec3 operator+(vec3& other);
    vec3 operator-(vec3& other);
    vec3 operator*(vec3& other);
    vec3 operator*(f32 a);
    vec3 operator/(f32 a);
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
    vec4 operator+(vec4& other);
    vec4 operator-(vec4& other);
};

struct mat4
{
    union
    {
        vec4 rows[4];
        f32 data[16];
    };

    vec4 &operator[](sizet i);
    mat4 operator*(mat4& other);
    vec4 operator*(vec4& other);
};

inline vec2
vec2::operator/(f32 a)
{
    return {x/a, y/a};
}

inline vec2
vec2::operator+(vec2& other)
{
    return {other.x+x, other.y+y};
}

inline vec2
vec2::operator-(vec2& other)
{
    return {x-other.x, y-other.y};
}

inline vec3
vec3::operator+(vec3& other)
{
    return {x+other.x, y+other.y, z+other.z};
}

inline vec3
vec3::operator-(vec3& other)
{
    return {x-other.x, y-other.y, z-other.z};
}

inline vec3
vec3::operator*(vec3& other)
{
    return {x*other.x, y*other.y, z*other.z};
}

inline vec3
vec3::operator*(f32 a)
{
    return {x*a, y*a, z*a};
}

inline f32 
&vec3::operator[](sizet i)
{
    return data[i];
}

inline vec3
vec3::operator/(f32 a)
{
    return {x/a, y/a, z/a};
}

inline f32
vec3_length(vec3 v)
{
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline vec3
vec3_normalized(vec3 v)
{
    f32 len = vec3_length(v);

    v.x = v.x / len;
    v.y = v.y / len;
    v.z = v.z / len;

    return v;
}


inline vec2
V2(f32 x, f32 y)
{
    return {x, y};
}

inline vec3
V3(f32 x, f32 y, f32 z)
{
    return {x, y, z};
}

inline vec3
V3(vec2 v, f32 z)
{
    return {v.x, v.y, z};
}

inline vec3
V3(f32 n)
{
    return {n, n, n};
}

inline vec4
V4(f32 x, f32 y, f32 z, f32 w)
{
    return {x, y, z ,w};
}

inline vec4
V4(vec3 v, f32 w)
{
    return {v.x, v.y, v.z, w};
}

inline vec4
V4(f32 n)
{
    return {n, n, n, n};
}

inline vec3
vec3_cross(vec3 a, vec3 b)
{
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

inline vec3
vec3_dot(vec3 a, vec3 b)
{
    return {a.x*b.x + a.y*b.y + a.z*b.z};
}

inline f32 
&vec4::operator[](sizet i)
{
    return data[i];
}

inline vec4 
vec4::operator+(vec4& other)
{
    vec4 out;
    for (sizet i = 0; i < 4; ++i)
    {
        out[i] = data[i] + other[i];
    }
    return out;
}

inline vec4 
vec4::operator-(vec4& other)
{
    vec4 out;
    for (sizet i = 0; i < 4; ++i)
    {
        out[i] = data[i] - other[i];
    }
    return out;
}

inline vec4 
&mat4::operator[](sizet i)
{
    return rows[i];
}

inline mat4 
mat4::operator*(mat4& other)
{
    mat4 out;
    for (sizet x = 0; x < 4; ++x)
    {
        for (sizet y = 0; y < 4; ++y)
        {
            f32 sum = 0.0f;
            for (sizet m = 0; m < 4; ++m)
            {
                sum += rows[x][m] * other[m][y];
            }
            out[x][y] = sum;
        }
    }
    return out;
}

inline vec4 
mat4::operator*(vec4& other)
{
    vec4 out = {};
    for (sizet x = 0; x < 4; ++x)
    {
        for (sizet y = 0; y < 1; ++y)
        {
            f32 sum = 0.0f;
            for (sizet m = 0; m < 4; ++m)
            {
                sum += rows[x][m] * other[m];
            }
            out[x] = sum;
        }
    }
    return out;
}

inline mat4
mat4_identity()
{
    mat4 out = {};
    out[0][0] = 1.0f;
    out[1][1] = 1.0f;
    out[2][2] = 1.0f;
    out[3][3] = 1.0f;

    return out;
}

inline mat4
mat4_scale(vec3 v)
{
    mat4 out = mat4_identity();
    out[0][0] = v.x;
    out[1][1] = v.y;
    out[2][2] = v.z;

    return out;
}

inline mat4
mat4_translate(vec3 v)
{
    mat4 out = mat4_identity();

    out[0][3] = v.x;
    out[1][3] = v.y;
    out[2][3] = v.z;

    return out;
}

inline mat4
mat4_rotate(f32 angle, vec3 v)
{
    f32 radians = angle * PI_F / 180;
    f32 cos = cosf(radians);
    f32 sin = sinf(radians);

    mat4 out = 
    {
        cos + v.x*v.x * (1-cos),       v.x*v.y * (1-cos) - v.z*sin,  v.x*v.z * (1-cos) + v.y*sin, 0.0f,
        v.y*v.x * (1-cos) + v.z*sin,  cos + v.y*v.y * (1-cos),       v.y*v.z * (1-cos) - v.x*sin, 0.0f,
        v.z*v.x * (1-cos) - v.y*sin,  v.z*v.y * (1-cos) + v.x*sin,  cos + v.z*v.z * (1-cos),      0.0f,
        0.0f,                            0.0f,                            0.0f,                           1.0f
    };

    return out;
}


internal inline mat4
mat4_inverse(mat4 m)
{
    f32 A2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    f32 A1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
    f32 A1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
    f32 A0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
    f32 A0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
    f32 A0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
    f32 A2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    f32 A1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
    f32 A1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
    f32 A2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
    f32 A1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
    f32 A1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    f32 A0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
    f32 A0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
    f32 A0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
    f32 A0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
    f32 A0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
    f32 A0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

    mat4 out;

    f32 det = m[0][0] * ( m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223 )
        - m[0][1] * ( m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223 )
        + m[0][2] * ( m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123 )
        - m[0][3] * ( m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123 );
    det = 1 / det;

    out[0][0] = det *   ( m[1][1] * A2323 - m[1][2] * A1323 + m[1][3] * A1223 );
    out[0][1] = det * - ( m[0][1] * A2323 - m[0][2] * A1323 + m[0][3] * A1223 );
    out[0][2] = det *   ( m[0][1] * A2313 - m[0][2] * A1313 + m[0][3] * A1213 );
    out[0][3] = det * - ( m[0][1] * A2312 - m[0][2] * A1312 + m[0][3] * A1212 );
    out[1][0] = det * - ( m[1][0] * A2323 - m[1][2] * A0323 + m[1][3] * A0223 );
    out[1][1] = det *   ( m[0][0] * A2323 - m[0][2] * A0323 + m[0][3] * A0223 );
    out[1][2] = det * - ( m[0][0] * A2313 - m[0][2] * A0313 + m[0][3] * A0213 );
    out[1][3] = det *   ( m[0][0] * A2312 - m[0][2] * A0312 + m[0][3] * A0212 );
    out[2][0] = det *   ( m[1][0] * A1323 - m[1][1] * A0323 + m[1][3] * A0123 );
    out[2][1] = det * - ( m[0][0] * A1323 - m[0][1] * A0323 + m[0][3] * A0123 );
    out[2][2] = det *   ( m[0][0] * A1313 - m[0][1] * A0313 + m[0][3] * A0113 );
    out[2][3] = det * - ( m[0][0] * A1312 - m[0][1] * A0312 + m[0][3] * A0112 );
    out[3][0] = det * - ( m[1][0] * A1223 - m[1][1] * A0223 + m[1][2] * A0123 );
    out[3][1] = det *   ( m[0][0] * A1223 - m[0][1] * A0223 + m[0][2] * A0123 );
    out[3][2] = det * - ( m[0][0] * A1213 - m[0][1] * A0213 + m[0][2] * A0113 );
    out[3][3] = det *   ( m[0][0] * A1212 - m[0][1] * A0212 + m[0][2] * A0112 );

    return out;
}


internal inline mat4
mat4_transpose(mat4 matrix)
{
    mat4 out;
    for (sizet i = 0; i < 4; ++i)
    {
        for (sizet k = 0; k < 4; ++k)
        {
            out[i][k] = matrix[k][i];
        }
    }

    return out;
}


inline f32
degrees_to_radians(f32 d) 
{
    return (d*((f32)PI_F/180));
}

inline mat4
mat4_orthographic(f32 w, f32 h)
{
    mat4 out = {
        2/w,  0.0f, 0.0f, -1.0f, 
        0.0f, 2/h,  0.0f, -1.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  1.0f
    };

    return out;
}

inline mat4
mat4_perspective(f32 w, f32 h, f32 fov, f32 n, f32 f)
{
    f32 t = tanf(degrees_to_radians(fov/2.0f));
    f32 a = w/h;

    mat4 out =
    {
        1/t,    0.0f, 0.0f,  0.0f,
        0.0f,   a/t,  0.0f,  0.0f,
        0.0f,   0.0f, (n+f)/(n-f), 2*n*f/(n-f),
        0.0f,   0.0f, -1.0f,  0.0f,
    };
    return out;
}

inline b8
point_is_inside(vec2 p, vec2 pos, vec2 scale)
{
    return (p.x >= pos.x && 
            p.x <= (pos.x + scale.x) &&
            p.y >= pos.y &&
            p.y <= (pos.y + scale.y));
}

#endif
