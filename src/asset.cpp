internal void
AssetsInit(Assets* assets, MemoryArena* arena)
{
    SubArena(arena, &assets->arena, Megabytes(64));
    
    assets->sprites = PushMemory(&assets->arena, Sprite, 256);
    assets->loaded_sprite_queue = PushMemory(&assets->arena, SpriteHandle, 50);
}

internal SpriteHandle
LoadSprite(Assets* assets, char* sprite_path)
{
    FileResult file = g_Platform.ReadEntireFile(sprite_path);
    SpriteHandle result = 0;
    
    if (file.data)
    {
        result = assets->num_sprites++;
        Sprite* sprite = assets->sprites + result;
        
        stbi_set_flip_vertically_on_load(1);
        sprite->data = stbi_load_from_memory((u8*)file.data,
                                             file.size,
                                             &sprite->width,
                                             &sprite->height, 
                                             &sprite->channels,
                                             0);
        
        sprite->name = StringCopy(&assets->arena, sprite_path);
        
        g_Platform.FreeFileMemory(file.data);
        assets->loaded_sprite_queue[assets->num_queued_sprites++] = result;
    }
    
    
    
    
    return result;
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


internal SpriteHandle
AddSprite(Assets* assets)
{
    return assets->num_sprites++;
}


internal Sprite*
GetLoadedSprite(Assets* assets, SpriteHandle handle)
{
    Assert(handle < (i32)assets->num_sprites &&
           handle >= 0);
    
    return assets->sprites + handle;
}

internal SpriteHandle
SubspriteFromSprite(Assets* assets, 
                    SpriteHandle sprite_handle, 
                    f32 x, f32 y, f32 sprite_width, f32 sprite_height)
{
    Sprite* sprite = GetLoadedSprite(assets, sprite_handle);
    SpriteHandle sh = AddSprite(assets);
    Sprite* subsprite = GetLoadedSprite(assets, sh);
    
    subsprite->type = TYPE_SUBSPRITE;
    
    f32 sheet_width = (f32)sprite->width;
    f32 sheet_height = (f32)sprite->height;
    
    subsprite->main_sprite = sprite_handle;
    subsprite->uvs[0] = V2(((x+1)*sprite_width) / sheet_width, ((y+1)*sprite_height) / sheet_height);
    subsprite->uvs[1] = V2(((x+1)*sprite_width) / sheet_width, (y*sprite_height)     / sheet_height);
    subsprite->uvs[2] = V2((x*sprite_width)     / sheet_width, (y*sprite_height)     / sheet_height);
    subsprite->uvs[3] = V2((x*sprite_width)     / sheet_width, ((y+1)*sprite_height) / sheet_height);
    
    return sh;
}


internal SpriteHandle
CreateEmpthySprite(Assets* assets, u32 w, u32 h, u32 channels)
{
    Sprite sprite = {};
    SpriteHandle result;
    sprite.data = PushMemory(&assets->arena, u8, (w*h));
    sprite.width = w;
    sprite.height = h;
    sprite.channels = channels;
    
    assets->sprites[assets->num_sprites] = sprite;
    result = assets->num_sprites++;
    
    // TODO: 
    assets->loaded_sprite_queue[assets->num_queued_sprites++] = result;
    
    return result;
}

struct LoadSpriteWorkData
{
    Assets* assets;
    char* sprite_path;
};

internal void
LoadSpriteWork(void* data)
{
    LoadSpriteWorkData* work_data = (LoadSpriteWorkData*)data;
    LoadSprite(work_data->assets, work_data->sprite_path);
}
