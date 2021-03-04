#if !defined(MATH_H)
#define MATH_H

struct vec2
{
    f32 x, y;
};

struct vec3 
{
    union
    {
        struct
        {
            f32 x, y, z;
        };
        f32 data[3];
    };

    f32 &operator[](sizet i);
};

struct vec4
{
    union
    {
        struct
        {
            f32 x, y, z, w;
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
    };

    vec4 &operator[](sizet i);
    mat4 operator*(mat4& other);
    vec4 operator*(vec4& other);
};


inline f32 
&vec3::operator[](sizet i)
{
    return data[i];
}

inline f32
length_vec3(vec3 v)
{
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline vec3
normalized_vec3(vec3 v)
{
    f32 len = length_vec3(v);

    v.x = v.x / len;
    v.y = v.y / len;
    v.z = v.z / len;

    return v;
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
identity_mat4()
{
    mat4 out = {};
    out[0][0] = 1.0f;
    out[1][1] = 1.0f;
    out[2][2] = 1.0f;
    out[3][3] = 1.0f;

    return out;
}

inline mat4
scale_mat4(vec3 v)
{
    mat4 out = identity_mat4();
    out[0][0] = v.x;
    out[1][1] = v.y;
    out[2][2] = v.z;

    return out;
}

inline mat4
translate_mat4(vec3 v)
{
    mat4 out = identity_mat4();

    for (sizet i = 0; i < 3; ++i)
    {
        out[i][3] = v[i];
    }

    return out;
}

inline mat4
rotate_mat4(f32 angle, vec3 v)
{
    f32 radians = angle * PI / 180;
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


inline mat4
transpose_mat4(mat4 matrix)
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

inline mat4
orthographic_mat4(f32 w, f32 h)
{
    mat4 out = {
        2/w,  0.0f, 0.0f, -1.0f, 
        0.0f, 2/h,  0.0f, -1.0f,
        0.0f, 0.0f, 1.0f,  0.0f,
        0.0f, 0.0f, 0.0f,  1.0f
    };

    return out;
}
#endif
