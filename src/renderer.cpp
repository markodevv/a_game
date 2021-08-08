#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6
#define NUM_ASCII 96

const u32 LAYER_BACK = 0;
const u32 LAYER_BACKMID = 1;
const u32 LAYER_MID = 2;
const u32 LAYER_MIDFRONT = 3;
const u32 LAYER_FRONT = 4;

internal inline Color
NewColor(u8 r, u8 b, u8 g, u8 a)
{
    return {r, b, g, a};
}

internal inline Color
NewColor(u8 n)
{
    return {n, n, n, n};
}

internal Uniform*
GetUnform(Shader* shader, char* name)
{
    Uniform* uniform = shader->uniforms;
    while (uniform)
    {
        if (StringMatch(name, uniform->name))
        {
            return uniform;
        }
        
        uniform = uniform->next;
    }
    
    return uniform;
}

internal Uniform*
AddUniform(Shader* shader, char* name, MemoryArena* arena)
{
    Uniform* uniform = shader->uniforms;
    if (!uniform)
    {
        shader->uniforms = PushMemory(arena, Uniform);
        shader->uniforms->name = StringCopy(arena, name);
        return shader->uniforms;
    }
    
    while (uniform->next)
    {
        uniform = uniform->next;
    }
    
    uniform->next = PushMemory(arena, Uniform);
    uniform->next->name = StringCopy(arena, name);
    
    return uniform->next;
}

// 

#define SetShaderUniform(shader, n, value, type) \
{ \
char* name = n; \
Uniform* uni = GetUnform(shader, name); \
if (uni) \
{ \
*((type*)uni->data) = value; \
type test = *((type*)uni->data); \
} \
else \
{ \
Print("No uniform named [%s]", name); \
} \
} 

internal Shader*
GetShader(Assets* assets, ShaderId shader_id)
{
    return &assets->shaders[shader_id];
}

internal Camera
CreateCamera(vec3 up, vec3 direction, vec3 position)
{
    Camera result;
    result.up = up;
    result.direction = direction;
    result.position = position;
    return result;
}

internal mat4
CameraTransform(Camera* cam)
{
    vec3 f = Vec3Normalized(cam->direction);
    vec3 u = Vec3Normalized(cam->up);
    vec3 r = Vec3Normalized(Vec3Cross(cam->up, cam->direction));
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

