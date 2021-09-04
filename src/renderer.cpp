#define VERTICES_PER_QUAD 4
#define INDICES_PER_QUAD 6

const Layer LAYER_BACK = 0;
const Layer LAYER_BACKMID = 1;
const Layer LAYER_MID = 2;
const Layer LAYER_MIDFRONT = 3;
const Layer LAYER_FRONT = 4;
const Color COLOR_WHITE = {255, 255, 255, 255};

internal Color
RandomColor()
{
    return {
        RandomRangeU8(0, 255),
        RandomRangeU8(0, 255), 
        RandomRangeU8(0, 255), 
        255
    };
}

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
LogM("No uniform named [%s]", name); \
} \
} 

internal Shader*
GetShader(Assets* assets, ShaderId shader_id)
{
    return &assets->shaders[shader_id];
}

inline internal Material
CreateMaterial(vec3 ambient, vec3 diffuse, vec3 specular, f32 shininess)
{
    return {ambient, diffuse, specular, shininess};
}

inline internal Light
CreateLight(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular)
{
    return {ambient, diffuse, specular, position};
}

inline internal Camera
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


internal SortElement*
SortElementStart(RenderGroup* group)
{
    SortElement* end = (SortElement*)((group->push_buffer_base + 
                                       group->push_buffer_capacity) - 
                                      (sizeof(SortElement)));
    SortElement* start = end - (group->sort_element_count - 1);
    
    return start;
}
