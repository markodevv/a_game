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

internal Uniform*
get_uniform(Shader* shader, char* name)
{
    Uniform* uniform = shader->uniforms;
    while (uniform)
    {
        if (string_match(name, uniform->name))
        {
            return uniform;
        }

        uniform = uniform->next;
    }

    return uniform;
}

internal Uniform*
add_uniform(Shader* shader, char* name, MemoryArena* arena)
{
    Uniform* uniform = shader->uniforms;
    if (!uniform)
    {
        shader->uniforms = PushMemory(arena, Uniform);
        shader->uniforms->name = string_copy(arena, name);
        return shader->uniforms;
    }

    while (uniform->next)
    {
        uniform = uniform->next;
    }

    uniform->next = PushMemory(arena, Uniform);
    uniform->next->name = string_copy(arena, name);

    return uniform->next;
}

// internal Uniform*
// get_uniform(Shader* shader, char* name, MemoryArena* arena = 0)
// {
    // u64 hash = 5381;
    // i32 c;
    // 
    // char* key = name;
    // while((c = *key++))
    // {
        // hash = ((hash << 5) + hash) + c;
    // }
// 
    // u16 hash_index = hash % ArrayCount(shader->uniforms);
// 
    // Uniform* uni = shader->uniforms + hash_index;
// 
    // do
    // {
        // if (uni->name && string_match(name, uni->name))
        // {
            // break;
        // }
// 
// 
        // if (arena && !(uni->next) && (uni->name))
        // {
            // PRINT("Hash collision detected!");
            // uni->next = PushMemory(arena, Uniform);
            // uni->next->name = 0;
            // uni = uni->next;
        // }
// 
        // if (arena && !(uni->name))
        // {
            // uni->name = string_copy(arena, name);
        // }
    // } while(uni->name);
// 
// 
    // return uni;
// }


#define set_shader_uniform(shader, n, value, type) \
{ \
    char* name = n; \
    Uniform* uni = get_uniform(shader, name); \
    if (uni) \
    { \
        *((type*)uni->data) = value; \
        type test = *((type*)uni->data); \
    } \
    else \
    { \
        PRINT("No uniform named [%s]", name); \
    } \
} 

internal Shader*
get_shader(Assets* assets, ShaderId shader_id)
{
    return &assets->shaders[shader_id];
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

