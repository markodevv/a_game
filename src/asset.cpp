internal void
AssetsInit(Assets* assets, MemoryArena* arena)
{
    SubArena(arena, &assets->arena, Megabytes(64));
    
    assets->sprites = PushMemory(&assets->arena, Sprite, 512);
    assets->loaded_sprite_queue = PushMemory(&assets->arena, SpriteID, 50);
    assets->runtime_sprite_queue = PushMemory(&assets->arena, RuntimeSpriteID, 50);
}

struct OBJFileInfo
{
    u32 vertex_count;
    u32 face_count;
    u32 normal_count;
    u32 uv_count;
};

internal bool 
IsNewLineChar(char c)
{
    return c == '\n' || c == '\n\r';
}

internal OBJFileInfo
GetOBJFileInfo(FileResult file)
{
    OBJFileInfo result = {};
    char prefix[3];
    u8* data = (u8*)file.data;
    while(*data)
    {
        prefix[0] = *data++;
        prefix[1] = *data;
        prefix[2] = '\0';
        
        if (StringMatch(prefix, "v "))
        {
            result.vertex_count++;
        }
        else if (StringMatch(prefix, "vn"))
        {
            result.normal_count++;
        }
        else if (StringMatch(prefix, "vt"))
        {
            result.uv_count++;
        }
        else if (StringMatch(prefix, "f "))
        {
            result.face_count++;
        }
        
        while (*data && !IsNewLineChar(*data))
        {
            data++;
        }
        
        if (*data)
        {
            data++;
        }
    }
    
    return result;
}

internal bool
IsDigit(char c)
{
    return c >= 48 && c <= 57 || c == '.';
}


internal vec3
GetVec3(char* text)
{
    vec3 out = {};
    char number_str[50];
    u8 vertex_axis = 0;
    
    while(1)
    {
        i32 sign = 1;
        
        if (*text == '-')
        {
            sign = -1;
            text++;
        }
        if (IsDigit(*text))
        {
            i32 number_str_index = 0;
            while(IsDigit(*text))
            {
                number_str[number_str_index] = *text;
                number_str_index++;
                text++;
            }
            number_str[number_str_index] = '\0';
            Vec3At(out, vertex_axis++) = (f32)(Atof(number_str) * sign);
            number_str_index = 0;
        }
        if (IsNewLineChar(*text))
        {
            break;
        }
        text++;
    }
    
    return out;
}

internal Mesh
LoadOBJModel(Platform* platform, Assets* assets, char* model_path)
{
    Mesh result = {};
    FileResult file = platform->ReadEntireFile(model_path);
    OBJFileInfo obj_info = GetOBJFileInfo(file);
    
    result.vertices = PushMemory(&assets->arena, Vertex3D, obj_info.vertex_count);
    result.indices = PushMemory(&assets->arena, u32, obj_info.face_count * 3);
    result.vertices = PushMemory(&assets->arena, Vertex3D, obj_info.vertex_count);
    
    
    char prefix[3];
    char* data = (char*)file.data;
    
    while(*data)
    {
        prefix[0] = *data++;
        prefix[1] = *data;
        prefix[2] = '\0';
        
        if (StringMatch(prefix, "v "))
        {
            vec3 v = GetVec3(data);
        }
        else if (StringMatch(prefix, "vn"))
        {
            vec3 v = GetVec3(data);
        }
        else if (StringMatch(prefix, "vt"))
        {
            vec3 v = GetVec3(data);
        }
        else if (StringMatch(prefix, "f "))
        {
        }
        
        while (*data && !IsNewLineChar(*data))
        {
            data++;
        }
        
        if (*data)
        {
            data++;
        }
    }
    
    return result;
}


internal Sprite*
GetLoadedSprite(Assets* assets, SpriteID id)
{
    return assets->sprites + id;
}

internal Sprite*
GetRuntimeSprite(Assets* assets, RuntimeSpriteID id)
{
    return assets->sprites + (id + NUM_SPRITES);
}

internal RuntimeSpriteID
AddSubsprite(Assets* assets)
{
    RuntimeSpriteID id = assets->num_runtime_sprites++;
    assets->sprites[id + NUM_SPRITES].type = TYPE_SUBSPRITE;
    return assets->num_runtime_sprites++;
}

internal RuntimeSpriteID
SubspriteFromSprite(Assets* assets, 
                    SpriteID id, 
                    f32 x, f32 y, f32 sprite_width, f32 sprite_height)
{
    Sprite* sprite = GetLoadedSprite(assets, id);
    RuntimeSpriteID runtime_id  = AddSubsprite(assets);
    Sprite* subsprite = GetRuntimeSprite(assets, runtime_id);
    
    f32 sheet_width = (f32)sprite->width;
    f32 sheet_height = (f32)sprite->height;
    
    subsprite->main_sprite = id;
    subsprite->uvs[0] = V2(((x+1)*sprite_width) / sheet_width, ((y+1)*sprite_height) / sheet_height);
    subsprite->uvs[1] = V2(((x+1)*sprite_width) / sheet_width, (y*sprite_height)     / sheet_height);
    subsprite->uvs[2] = V2((x*sprite_width)     / sheet_width, (y*sprite_height)     / sheet_height);
    subsprite->uvs[3] = V2((x*sprite_width)     / sheet_width, ((y+1)*sprite_height) / sheet_height);
    
    subsprite->type = TYPE_SUBSPRITE;
    
    return runtime_id;
}


internal RuntimeSpriteID
CreateEmpthySprite(Assets* assets, u32 w, u32 h, u32 channels)
{
    RuntimeSpriteID id =  assets->num_runtime_sprites++;
    Sprite* sprite = GetRuntimeSprite(assets, id);
    
    sprite->data = PushMemory(&assets->arena, u8, (w*h));
    sprite->width = w;
    sprite->height = h;
    sprite->channels = channels;
    
    sprite->type = TYPE_SPRITE;
    
    assets->runtime_sprite_queue[assets->num_runtime_queued_sprites++] = id;
    
    return id;
}

internal void
LoadSprite(Assets* assets, SpriteID id, char* sprite_path)
{
    FileResult file = g_Platform.ReadEntireFile(sprite_path);
    
    if (file.data)
    {
        Sprite* sprite = assets->sprites + id;
        
        stbi_set_flip_vertically_on_load(1);
        sprite->data = stbi_load_from_memory((u8*)file.data,
                                             file.size,
                                             &sprite->width,
                                             &sprite->height, 
                                             &sprite->channels,
                                             0);
        
        sprite->name = StringCopy(&assets->arena, sprite_path);
        
        g_Platform.FreeFileMemory(file.data);
        assets->loaded_sprite_queue[assets->num_queued_sprites++] = id;
    }
    else
    {
        LogM("Failed to load sprite %s.\n", sprite_path);
    }
}

struct LoadSpriteWorkData
{
    Assets* assets;
    SpriteID sprite_id;
    char* sprite_path;
};

internal void
LoadSpriteWork(void* data)
{
    LoadSpriteWorkData* work_data = (LoadSpriteWorkData*)data;
    LoadSprite(work_data->assets, work_data->sprite_id, work_data->sprite_path);
}
