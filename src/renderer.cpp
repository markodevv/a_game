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

inline internal Material
CreateDefaultMaterial()
{
    return {V3(1), V3(1), V3(1), 1.0f};
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
const Color ColorYellow = {0, 255, 255, 255};
const Color ColorOrange = {120, 255, 255, 255};
const Color ColorGreen  = {0, 255, 0, 255};
const Color ColorRed    = {255, 0, 0, 255};
const Color ColorBlue   = {0, 0, 255, 255};
const Color ColorWhite   = {255, 255, 255, 255};

internal inline vec4
NormalizeColor(Color color)
{
    return {
        color.r/255.0f,
        color.g/255.0f,
        color.b/255.0f,
        color.a/255.0f,
    };
}


#define PushRenderEntry(group, type, sort_key) \
(type *)PushRenderEntry_(group, sizeof(type), RENDER_ENTRY_##type, sort_key) \


internal RenderGroup*
CreateRenderGroup(MemoryArena* arena, 
                  mat4 projection, Camera camera, 
                  Renderer2D* ren, Assets* assets, u32 capacity)
{
    RenderGroup* render_group = &ren->render_groups[ren->render_group_count++];
    
    render_group->setup.projection = projection;
    render_group->assets = assets;
    render_group->setup.camera = camera;
    
    render_group->push_buffer_capacity = capacity;
    render_group->push_buffer_base = PushMemory(arena, u8, render_group->push_buffer_capacity);
    render_group->push_buffer_size = 0;
    
    return render_group;
}


internal void 
PushBackSortElement(RenderGroup* group, u32 sort_key)
{
    SortElement* end = (SortElement*)((group->push_buffer_base + group->push_buffer_capacity) - sizeof(SortElement));
    SortElement* next_element = (end - group->sort_element_count);
    next_element->entry_offset = group->push_buffer_size;
    next_element->key = sort_key;
    
    ++group->sort_element_count;
    Assert((u8*)next_element > (group->push_buffer_base + group->push_buffer_size));
}

internal void*
PushRenderEntry_(RenderGroup* group, u32 size, RenderEntryType type, u32 sort_key)
{
    RenderEntryHeader* header = 0;
    void* result = 0;
    
    if ((group->push_buffer_size + size) < group->push_buffer_capacity)
    {
        header = (RenderEntryHeader*)(group->push_buffer_base +  
                                      group->push_buffer_size);
        header->entry_type = type;
        
        result = ((u8*)header) + sizeof(*header); 
        MemoryClear(result, size);
        
        
        PushBackSortElement(group, sort_key);
        
        group->push_buffer_size += (size + sizeof(*header));
    }
    else
    {
        Assert(false);
    }
    
    
    return result;
}


internal void
PushClearScreen(RenderGroup* group, Color color)
{
    u32 key = 0;
    ClearScreenEntry* clear_screen = PushRenderEntry(group, ClearScreenEntry, key);
    clear_screen->color = color;
}

internal void
PushQuad(RenderGroup* group, 
         vec2 position, 
         vec2 size, 
         Color color, 
         Layer layer,
         SpriteID sprite_id = WHITE_SPRITE,
         ShaderId shader_id = SHADER_ID_NORMAL)
{
    u32 key = shader_id;
    
    if (color.a != 255)
    {
        key += 100 + layer;
    }
    
    QuadEntry* entry = PushRenderEntry(group, QuadEntry, key);
    
    entry->position = V3(position, layer);
    entry->size = size;
    entry->color = color;
    entry->sprite_id = sprite_id;
    entry->shader_id = shader_id;
}


internal void
PushMesh(RenderGroup* group, 
         vec3 position, 
         vec3 scale, 
         Color color, 
         MeshEnum id,
         ShaderId shader_id = SHADER_ID_BASIC_3D)
{
    u32 key = shader_id;
    
    // TODO(Marko): Probably don't need to sort meshes 
    MeshEntry* entry = PushRenderEntry(group, MeshEntry, 0);
    Sprite* sprite = GetSprite(group->assets, id);
    
    entry->position = position;
    entry->scale = scale;
    entry->color = color;
    entry->sprite = sprite;
    entry->mesh = GetMesh(group->assets, id);
    entry->shader_id = shader_id;
}



internal void
PushTriangle(RenderGroup* group, 
             vec2 p1, 
             vec2 p2, 
             vec2 p3, 
             Color color, 
             Layer layer,
             SpriteID id = WHITE_SPRITE,
             ShaderId shader_id = SHADER_ID_NORMAL)
{
    u32 key = shader_id;
    
    if (color.a != 255)
    {
        key += 50 + layer;
    }
    
    TriangleEntry* entry = PushRenderEntry(group, TriangleEntry, key);
    Sprite* sprite = GetSprite(group->assets, id);
    
    entry->points[0] = V3(p1, layer);
    entry->points[1] = V3(p2, layer);
    entry->points[2] = V3(p3, layer);
    entry->color = color;
    entry->sprite = sprite;
    entry->shader_id = shader_id;
}

internal f32
GetTextPixelWidth(Font* font, char* text)
{
    f32 out = 0;
    
    while(*text)
    {
        u32 index = *text-32;
        out += font->char_metrics[index].xadvance;
        
        text++;
    }
    
    return out;
}

internal void
PushText(RenderGroup* render_group,
         Font* font,
         char* text,
         vec2 position,
         Layer layer)
{
    while (*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            i32 index = *text-32;
            CharInfo* info = font->char_info + index;
            
            PushQuad(render_group, position,
                     V2(info->width, info->height),
                     COLOR_WHITE, layer, info->sprite_id);
            
            position.x += info->xoffset;
        }
        
        text++;
    }
}


internal void
DEBUGDrawText(RenderGroup* render_group,
              Font* font,
              char* text,
              vec2 position,
              Layer layer,
              TextAlign align = TEXT_ALIGN_LEFT,
              Color color = {255, 255, 255, 254})
{
    if (align == TEXT_ALIGN_MIDDLE)
    {
        f32 text_width = GetTextPixelWidth(font, text);
        position.x -= (text_width / 2.0f);
    }
    
    while(*text)
    {
        if (*text >= 32 && *text <= 128)
        {
            i32 index = *text-32;
            CharMetric* cm = font->char_metrics + index;
            
            i32 round_x = IFloor((position.x + cm->xoff) + 0.5f);
            i32 round_y = IFloor((position.y - cm->yoff) + 0.5f);
            
            vec2 pos = V2(round_x, round_y);
            vec2 size = V2(cm->x1 - cm->x0,
                           -cm->y1 + cm->y0);
            
            PushQuad(render_group, pos, size, color, layer, font->sprite_ids[index]);
            
            position.x += cm->xadvance;
        }
        text++;
    }
}
