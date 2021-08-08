#define PI_F 3.14159265359f
#define Min(a,b) (((a)<(b)) ? (a) : (b))
#define Max(a,b) (((a)>(b)) ? (a) : (b))
#define Clamp(a, min, max) (((a)<=(max)) ? ( ((a)<(min)) ? (min) : (a) ) : (max))
#define Clamp_V3(v, min, max) clamp_vec3(v, min, max)
#define Abs(x) ((x)<0)?(-x):(x)
#define Abs_V3(v) abs_vec3(v)
#define Fmod(a, b) (f32)fmod((double)a, (double)b)
#define Fmod_V3(v, x) fmod_vec3(v, x)
#define IFloor(x) ((int) floor(x))

internal vec3 
fmod_vec3(vec3 v, f32 x)
{
    vec3 result = {};
    result.x = (f32)fmod((double)v.x, (double)x);
    result.y = (f32)fmod((double)v.y, (double)x);
    result.z = (f32)fmod((double)v.z, (double)x);
    
    return result;
}

internal vec3 
abs_vec3(vec3 v)
{
    v.x = Abs(v.x);
    v.y = Abs(v.y);
    v.z = Abs(v.z);
    
    return v;
}

internal vec3
clamp_vec3(vec3 v, f32 min, f32 max)
{
    v.x = Clamp(v.x, min, max);
    v.y = Clamp(v.y, min, max);
    v.z = Clamp(v.z, min, max);
    
    return v;
}

internal vec3
vec3_mix(vec3 v1, vec3 v2, f32 s)
{
    vec3 result = {};
    result.x =  v1.x*(1.0f - s)+v2.x*s;
    result.y =  v1.y*(1.0f - s)+v2.y*s;
    result.z =  v1.z*(1.0f - s)+v2.z*s;
    
    return result;
}

internal u64
Log2(u64 n)
{
    return log2(n);
}


internal vec2
V2(f32 x, f32 y)
{
    return {x, y};
}

internal vec2i
V2I(i32 x, i32 y)
{
    return {x, y};
}

internal vec3
V3(f32 x, f32 y, f32 z)
{
    return {x, y, z};
}

internal vec3i
V3I(i32 x, i32 y, i32 z)
{
    return {x, y, z};
}

internal vec2
V2(f32 a)
{
    return {a, a};
}

internal vec3
V3(vec2 v, f32 z)
{
    return {v.x, v.y, z};
}

internal vec3
V3(f32 n)
{
    return {n, n, n};
}

internal vec3 
Vec3Up()
{
    return V3(0.0f, 1.0f, 0.0f);
}

internal vec3 
Vec3Forward()
{
    return V3(0.0f, 0.0f, 1.0f);
}


internal vec4
V4(f32 x, f32 y, f32 z, f32 w)
{
    return {x, y, z ,w};
}

internal vec4
V4(vec3 v, f32 w)
{
    return {v.x, v.y, v.z, w};
}

internal vec4
V4(f32 n)
{
    return {n, n, n, n};
}

void 
operator+=(vec2& v1, vec2 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
}

void 
operator-=(vec2& v1, vec2 v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
}

void 
operator*=(vec2& v1, vec2 v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
}

vec2 
operator*(const vec2& v, f32 n)
{
    return {v.x*n, v.y*n};
}

vec2
operator/(vec2& v, f32 a)
{
    return {v.x/a, v.y/a};
}

vec2 
operator*(vec2& v1, vec2 v2)
{
    return V2(v1.x*v2.x, v1.y*v2.y);
}

vec2
operator+(vec2& v1, vec2 v2)
{
    return {v1.x+v2.x, v1.y+v2.y};
}

vec2
operator-(vec2& v, f32 n)
{
    return {v.x-n, v.y-n};
}

vec2
operator-(vec2& v1, vec2 v2)
{
    return {v1.x-v2.x, v1.y-v2.y};
}

void 
operator-=(vec2& v, f32 n)
{
    v.x -= n;
    v.y -= n;
}

void 
operator*=(vec2& v, f32 n)
{
    v.x *= n;
    v.y *= n;
}

void
operator/=(vec2& v, f32 n)
{
    v.x /= n;
    v.y /= n;
}

void 
operator+=(vec2& v, f32 n)
{
    v.x += n;
    v.y += n;
}

void 
operator*=(vec3& v1, vec3 v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
}

void 
operator+=(vec3& v1, vec3 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
}

vec3 
operator*(const vec3& v, f32 n)
{
    return {v.x*n, v.y*n, v.z*n};
}

vec3 
operator*(const vec3& v1, const vec3& v2)
{
    return {v1.x*v2.x, v1.y*v2.y, v1.z*v2.z};
}

f32
SquareRoot(f32 of)
{
    return sqrtf(of);
}

f32
Vec2Length(vec2 v)
{
    return SquareRoot(v.x*v.x + v.y*v.y);
}

f32
Vec2LengthSquared(vec2 v)
{
    return (v.x*v.x + v.y*v.y);
}

vec2
Vec2Normalized(vec2 v)
{
    f32 len = Vec2Length(v);
    return {v.x/len, v.y/len};
}

f32
Vec2Distance(vec2 a, vec2 b)
{
    return Vec2Length(b - a);
}

f32
Vec2DistanceSquared(vec2 a, vec2 b)
{
    return Vec2Length(b - a);
}

vec3
operator+(vec3& v1, vec3 v2)
{
    return {v1.x+v2.x, v1.y+v2.y, v1.z+v2.z};
}

vec3
operator+(const vec3& v1, f32 x)
{
    return {v1.x+x, v1.y+x, v1.z+x};
}

vec3
operator-(vec3& v1, vec3 v2)
{
    return {v1.x-v2.x, v1.y-v2.y, v1.z-v2.z};
}

vec3
operator-(const vec3& v1, f32 x)
{
    return {v1.x-x, v1.y-x, v1.z-x};
}

vec3
operator*(vec3& v1, vec3 v2)
{
    return {v1.x*v2.x, v1.y*v2.y, v1.z*v2.z};
}

vec3
operator*(vec3& v, f32 n)
{
    return {v.x*n, v.y*n, v.z*n};
}


vec3
operator/(vec3& v, f32 n)
{
    return {v.x/n, v.y/n, v.z/n};
}

f32
Vec3Length(vec3 v)
{
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

vec3
Vec3Normalized(vec3 v)
{
    f32 len = Vec3Length(v);
    
    v.x = v.x / len;
    v.y = v.y / len;
    v.z = v.z / len;
    
    return v;
}


vec3
Vec3Cross(vec3 a, vec3 b)
{
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

vec3
Vec3Dot(vec3 a, vec3 b)
{
    return {a.x*b.x + a.y*b.y + a.z*b.z};
}


vec4 
operator+(vec4& v1, vec4 v2)
{
    vec4 out;
    out.x = v1.x + v2.x;
    out.y = v1.y + v2.y;
    out.z = v1.z + v2.z;
    out.w = v1.w + v2.w;
    
    return out;
}

void 
operator*=(vec4& v,  f32 n)
{
    v.x *= n;
    v.y *= n;
    v.z *= n;
    v.w *= n;
}

vec4 
operator-(vec4& v1, vec4 v2)
{
    vec4 out;
    out.x = v1.x - v2.x;
    out.y = v1.y - v2.y;
    out.z = v1.z - v2.z;
    out.w = v1.w - v2.w;
    
    return out;
}


mat4 
operator*(mat4& m1, mat4 m2)
{
    mat4 out;
    for (sizet x = 0; x < 4; ++x)
    {
        for (sizet y = 0; y < 4; ++y)
        {
            f32 sum = 0.0f;
            for (sizet m = 0; m < 4; ++m)
            {
                sum += m1[x][m] * m2[m][y];
            }
            out[x][y] = sum;
        }
    }
    return out;
}

vec4 
operator*(mat4 mat, vec4 other)
{
    vec4 out = {};
    for (sizet x = 0; x < 4; ++x)
    {
        for (sizet y = 0; y < 1; ++y)
        {
            f32 sum = 0.0f;
            for (sizet m = 0; m < 4; ++m)
            {
                sum += mat[x][m] * other[m];
            }
            out[x] = sum;
        }
    }
    return out;
}

mat4
Mat4Identity()
{
    mat4 out = {};
    out[0][0] = 1.0f;
    out[1][1] = 1.0f;
    out[2][2] = 1.0f;
    out[3][3] = 1.0f;
    
    return out;
}

mat4
Mat4Scale(vec3 v)
{
    mat4 out = Mat4Identity();
    out[0][0] = v.x;
    out[1][1] = v.y;
    out[2][2] = v.z;
    
    return out;
}

mat4
Mat4Translate(vec3 v)
{
    mat4 out = Mat4Identity();
    
    out[0][3] = v.x;
    out[1][3] = v.y;
    out[2][3] = v.z;
    
    return out;
}

mat4
Mat4Rotate(f32 angle, vec3 v)
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


internal mat4
Mat4Inverse(mat4 m)
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


internal mat4
Mat4Transpose(mat4 matrix)
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


internal f32
ToRadians(f32 d) 
{
    return (d*((f32)PI_F/180));
}

internal mat4
Mat4Orthographic(f32 w, f32 h)
{
    
    f32 f = 10000.0f;
    f32 n = 0.0f;
    
    mat4 out = {
        2/w,  0.0f, 0.0f, -1.0f, 
        0.0f, 2/h,  0.0f, -1.0f,
        0.0f, 0.0f, -2/(f-n),  -(f-n)/(f+n),
        0.0f, 0.0f, 0.0f,  1.0f
    };
    
    return out;
}

internal mat4
Mat4Perspective(f32 w, f32 h, f32 fov, f32 n, f32 f)
{
    f32 t = tanf(ToRadians(fov/2.0f));
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

internal b8
PointInsideRect(vec2 p, vec2 pos, vec2 scale)
{
    return (p.x >= pos.x && 
            p.x <= (pos.x + scale.x) &&
            p.y >= pos.y &&
            p.y <= (pos.y + scale.y));
}

f32&
vec2::operator[](sizet i)
{
    return (&(x))[i];
}

f32&
vec3::operator[](sizet i)
{
    return (&(x))[i];
}
f32&
vec4::operator[](sizet i)
{
    return (&(x))[i];
}

vec4&
mat4::operator[](sizet i)
{
    return rows[i];
}
