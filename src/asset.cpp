// SPRITE LOADING

internal Sprite*
GetSprite(Assets* assets, SpriteID id)
{
    return assets->sprites + id;
}

internal void
LoadSprite(Assets* assets, SpriteEnum sprite_enum, char* sprite_path)
{
    FileResult file = g_Platform.ReadEntireFile(sprite_path);
    
    if (file.data)
    {
        Sprite* sprite = GetSprite(assets, (SpriteID)sprite_enum);
        
        stbi_set_flip_vertically_on_load(1);
        sprite->data = stbi_load_from_memory((u8*)file.data,
                                             file.size,
                                             &sprite->width,
                                             &sprite->height, 
                                             &sprite->channels,
                                             0);
        
        sprite->name = StringCopy(&assets->arena, sprite_path);
        
        g_Platform.FreeFileMemory(file.data);
        assets->loaded_sprite_queue[assets->num_queued_sprites++] = sprite;
    }
    else
    {
        LogM("Failed to load sprite %s.\n", sprite_path);
    }
}

struct LoadSpriteWorkData
{
    Assets* assets;
    SpriteEnum sprite_id;
    char* sprite_path;
};

internal void
LoadSpriteWork(void* data)
{
    LoadSpriteWorkData* work_data = (LoadSpriteWorkData*)data;
    LoadSprite(work_data->assets, work_data->sprite_id, work_data->sprite_path);
}

internal void
AssetsInit(Assets* assets, MemoryArena* arena)
{
    SubArena(arena, &assets->arena, Megabytes(128));
    
    assets->sprites = PushMemory(&assets->arena, Sprite, 512);
    assets->loaded_sprite_queue = PushMemory(&assets->arena, Sprite*, 50);
    
    Assert(assets->num_sprites == 0);
    
    
    LoadSpriteWorkData sprites[5];
    
    sprites[0].assets = assets;
    sprites[0].sprite_id = WHITE_SPRITE;
    sprites[0].sprite_path = "../assets/white.png";
    sprites[1].assets = assets;
    sprites[1].sprite_id = RED_SPRITE;
    sprites[1].sprite_path  = "../assets/red.png";
    sprites[2].assets = assets;
    sprites[2].sprite_id = GREEN_SPRITE;
    sprites[2].sprite_path = "../assets/green.png";
    sprites[3].assets = assets;
    sprites[3].sprite_id = BLUE_SPRITE;
    sprites[3].sprite_path = "../assets/blue.png";
    sprites[4].assets = assets;
    sprites[4].sprite_id = PINK_SPRITE;
    sprites[4].sprite_path = "../assets/pink.png";
    
    for (u32 i = 0; i < ArrayCount(sprites); ++i)
    {
        g_Platform.PushWorkEntry(g_Platform.work_queue, LoadSpriteWork, &sprites[i]);
    }
    
    assets->num_sprites = NUM_SPRITES;
    
    g_Platform.WaitForWorkers(g_Platform.work_queue);
    
}


internal SpriteID
AddSubsprite(Assets* assets, SpriteID main_sprite)
{
    Assert(assets->num_sprites >= NUM_SPRITES);
    
    SpriteID id = (SpriteID)assets->num_sprites++;
    assets->sprites[id].type = TYPE_SUBSPRITE;
    assets->sprites[id].main_sprite = main_sprite;
    
    return id;
}

internal SpriteID
SubspriteFromSprite(Assets* assets, 
                    SpriteID id,
                    f32 x, f32 y, f32 w, f32 h)
{
    Sprite* sprite = GetSprite(assets, id);
    SpriteID subsprite_id  = AddSubsprite(assets, id);
    
    Sprite* subsprite = GetSprite(assets, subsprite_id);
    
    f32 sw = (f32)sprite->width;
    f32 sh = (f32)sprite->height;
    
    subsprite->uvs[0] = V2((x + w) / sw, (y + h) / sh);
    subsprite->uvs[1] = V2((x + w) / sw,  y / sh);
    subsprite->uvs[2] = V2( x / sw,       y / sh);
    subsprite->uvs[3] = V2( x / sw,      (y + h) / sh);
    
    return subsprite_id;
}

internal SpriteID
SubspriteFromSpriteC(Assets* assets, 
                     SpriteID id, 
                     i32 x, i32 y, f32 sprite_width, f32 sprite_height)
{
    Sprite* sprite = GetSprite(assets, id);
    SpriteID subsprite_id  = AddSubsprite(assets, id);
    
    Sprite* subsprite = GetSprite(assets, subsprite_id);
    
    f32 sheet_width = (f32)sprite->width;
    f32 sheet_height = (f32)sprite->height;
    
    subsprite->uvs[0] = V2(((x+1)*sprite_width) / sheet_width, ((y+1)*sprite_height) / sheet_height);
    subsprite->uvs[1] = V2(((x+1)*sprite_width) / sheet_width, (y*sprite_height)     / sheet_height);
    subsprite->uvs[2] = V2((x*sprite_width)     / sheet_width, (y*sprite_height)     / sheet_height);
    subsprite->uvs[3] = V2((x*sprite_width)     / sheet_width, ((y+1)*sprite_height) / sheet_height);
    
    subsprite->type = TYPE_SUBSPRITE;
    subsprite->main_sprite = id;
    
    return subsprite_id;
}


internal SpriteID
DEBUGCreateFontSprite(Assets* assets, u32 w, u32 h, u32 channels)
{
    Assert(assets->num_sprites >= NUM_SPRITES);
    SpriteID id = assets->num_sprites++;
    Sprite* sprite = GetSprite(assets, id);
    
    sprite->data = PushMemory(&assets->arena, u8, (w*h));
    sprite->width = w;
    sprite->height = h;
    sprite->channels = channels;
    
    sprite->type = TYPE_SPRITE;
    
    assets->loaded_sprite_queue[assets->num_queued_sprites++] = sprite;
    
    return id;
}

//////// FONT LOADING ////////////////

internal Font
LoadFontTest(Assets* assets, char* font_path, i8 font_size)
{
    Font font = {};
    i32 ascent = 0, bitmap_width = 1024, bitmap_height = font_size;
    f32 xpos = 2, scale, baseline;
    FileResult font_file = g_Platform.ReadEntireFile(font_path);
    font.char_info = PushMemory(&assets->arena, CharInfo, NUM_ASCII);
    
    /* Initialize font */
    
    stbtt_fontinfo font_info;
    if (!stbtt_InitFont(&font_info, (u8*)font_file.data, 0))
    {
        LogM("Stb init font failed for %s\n", font_path);
        return font;
    }
    
    SpriteID font_id = DEBUGCreateFontSprite(assets, bitmap_width, bitmap_height, 1);
    Sprite* font_sprite = GetSprite(assets, font_id);
    
    scale = stbtt_ScaleForPixelHeight(&font_info, font_size);
    stbtt_GetFontVMetrics(&font_info, &ascent, 0, 0);
    
    baseline = (i32)(ascent * scale);
    u8* bitmap = (u8*)font_sprite->data;
    
    for (u8 i = 0; i < NUM_ASCII; ++i)
    {
        char c = 32 + i;
        i32 advance,lsb,x0,y0,x1,y1;
        float x_shift = xpos - (f32)floor(xpos);
        CharInfo* char_info = font.char_info + i;
        
        stbtt_GetCodepointHMetrics(&font_info, c, &advance, &lsb);
        stbtt_GetCodepointBitmapBoxSubpixel(&font_info, c, scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
        char_info->xoffset = advance * scale;
        char_info->width = x1 - x0 + (x_shift);
        char_info->height = font_size;
        i32 bitmap_offset = (xpos + x0);
        
        f32 w = x1-x0;
        
        char_info->sprite_id = AddSubsprite(assets, font_id);
        Sprite* char_sprite = GetSprite(assets, char_info->sprite_id);
        
        char_sprite->uvs[0] = V2((xpos+w) / bitmap_width,   0);
        char_sprite->uvs[1] = V2((xpos+w) / bitmap_width,   1);
        char_sprite->uvs[2] = V2((xpos)   / bitmap_width,   1);
        char_sprite->uvs[3] = V2((xpos)   / bitmap_width,   0);
        
        stbtt_MakeCodepointBitmapSubpixel(&font_info, bitmap + bitmap_offset, x1-x0, y1-y0, bitmap_width, scale, scale, x_shift, 0, c);
        xpos += (advance * scale);
        
    }
    
    stbi_write_png("STB.png", bitmap_width, bitmap_height, 1, font_sprite->data, bitmap_width);
    g_Platform.FreeFileMemory(font_file.data);
    
    return font;
}

////////// OBJ loading ////////////

struct OBJIndexBucket
{
    u32 index;
    vec3 position;
    vec3 normal;
    vec2 uv;
    
    OBJIndexBucket* next;
};

struct OBJFileInfo
{
    u32 vertex_count;
    u32 face_count;
    u32 normal_count;
    u32 uv_count;
    
    u8* data;
    u32 cursor;
    
    OBJIndexBucket** index_map;
};

internal bool 
IsNewLineChar(char c)
{
    return c == '\n' || c == '\n\r';
}

internal bool 
IsWhiteSpace(char c)
{
    return c == ' ';
}

internal OBJFileInfo
GetOBJFileInfo(MemoryArena* arena, FileResult file)
{
    OBJFileInfo result = {};
    char prefix[3];
    result.data = (u8*)file.data;
    u8* data = (u8*)file.data;
    result.index_map = PushMemory(arena, OBJIndexBucket*, 4096);
    
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
    return c >= 48 && c <= 57 || c == '.' || c == '-';
}

internal char
OBJNextChar(OBJFileInfo* obj_info)
{
    return obj_info->data[++obj_info->cursor];
}

internal char
OBJCurrentChar(OBJFileInfo* obj_info)
{
    return obj_info->data[obj_info->cursor];
}

internal b8
IsEOFChar(char c)
{
    return (c == '\0');
}

internal void
OBJSkipWhiteSpace(OBJFileInfo* obj_info)
{
    while (IsWhiteSpace(OBJCurrentChar(obj_info)))
    {
        OBJNextChar(obj_info);
    }
}

internal u32
OBJNextInt(OBJFileInfo* obj_info)
{
    u32 result = 0;
    char string[32];
    u8 index = 0;
    
    char c = OBJCurrentChar(obj_info);
    Assert(IsDigit(c));
    
    while(IsDigit(c))
    {
        string[index++] = c;
        c = OBJNextChar(obj_info);
    }
    
    string[index] = '\0';
    result = ToInt(string);
    
    if (c == '/')
    {
        OBJNextChar(obj_info);
    }
    
    return result;
    
}

internal f32
OBJNextFloat(OBJFileInfo* obj_info)
{
    f32 result = 0;
    char string[32];
    u8 index = 0;
    
    char c = OBJCurrentChar(obj_info);
    Assert(IsDigit(c));
    
    while(IsDigit(c))
    {
        string[index++] = c;
        c = OBJNextChar(obj_info);
    }
    
    string[index] = '\0';
    result = ToFloat(string);
    
    if (c == '/')
    {
        OBJNextChar(obj_info);
    }
    
    return result;
    
}



internal vec3
OBJNextVec3(OBJFileInfo* obj_info)
{
    vec3 out = {};
    u8 vertex_axis = 0;
    
    while(1)
    {
        char c = OBJNextChar(obj_info);
        
        if (IsDigit(c))
        {
            Vec3At(out, vertex_axis++) = OBJNextFloat(obj_info);
        }
        
        c = OBJCurrentChar(obj_info);
        
        if (IsNewLineChar(c))
        {
            break;
        }
    }
    
    return out;
}

internal vec2
OBJNextVec2(OBJFileInfo* obj_info)
{
    vec2 out = {};
    u8 vertex_axis = 0;
    
    while(1)
    {
        char c = OBJNextChar(obj_info);
        
        if (IsDigit(c))
        {
            Vec2At(out, vertex_axis++) = OBJNextFloat(obj_info);
        }
        
        c = OBJCurrentChar(obj_info);
        
        if (IsNewLineChar(c))
        {
            break;
        }
    }
    
    return out;
}

struct OBJFace
{
    u32 position_indices[3];
    u32 normal_indices[3];
    u32 uv_indices[3];
};

internal OBJFace 
OBJGetFace(OBJFileInfo* obj_info)
{
    OBJFace face = {};
    u8 index = 0;
    
    char c = OBJCurrentChar(obj_info);
    while(1)
    {
        if (IsDigit(c))
        {
            //NOTE(Marko): -1 because .obj file is indexed starting from 1
            // Face = vposition/vtexture/vnormal * 3
            face.position_indices[index] = OBJNextInt(obj_info) - 1;
            face.uv_indices[index] = OBJNextInt(obj_info) - 1;
            face.normal_indices[index] = OBJNextInt(obj_info) - 1;
            
            index++;
        }
        
        if (IsNewLineChar(OBJCurrentChar(obj_info))
            || IsEOFChar(OBJCurrentChar(obj_info)))
        {
            break;
        }
        c = OBJNextChar(obj_info);
    }
    
    return face;
}

internal void
OBJSkipLine(OBJFileInfo* obj_info)
{
    char c = OBJCurrentChar(obj_info);
    while(1)
    {
        if (IsEOFChar(c) || IsNewLineChar(c))
        {
            OBJNextChar(obj_info);
            break;
        }
        c = OBJNextChar(obj_info);
    }
}

internal void
OBJPutIndex(MemoryArena* arena, OBJFileInfo* obj_info, vec3 pos, vec3 norm, vec2 uv, u32 current_index)
{
    u32 hash = 
        pos.x * 3 + pos.y * 9 + pos.z + 17 + 
        norm.x * 3 + norm.y * 9 + norm.z + 17 +
        uv.x * 3 + uv.y * 9;
    u32 hash_index = hash % (4096-1);
    
    OBJIndexBucket** bucket = &obj_info->index_map[hash_index];
    
    
    if (!(*bucket))
    {
        *bucket = PushMemory(arena, OBJIndexBucket);
        (*bucket)->position = pos;
        (*bucket)->uv = uv;
        (*bucket)->normal = norm;
        (*bucket)->index = current_index;
    }
    else
    {
        OBJIndexBucket* temp = PushMemory(arena, OBJIndexBucket);
        temp->next = *bucket;
        *bucket = temp;
        (*bucket)->position = pos;
        (*bucket)->uv = uv;
        (*bucket)->normal = norm;
    }
    
}

internal OBJIndexBucket*
OBJGetIndex(OBJFileInfo* obj_info, vec3 pos, vec3 norm, vec2 uv)
{
    u32 hash = 
        pos.x * 3 + pos.y * 9 + pos.z + 17 + 
        norm.x * 3 + norm.y * 9 + norm.z + 17 +
        uv.x * 3 + uv.y * 9;
    
    u32 hash_index = hash % (4096-1);
    
    OBJIndexBucket* result = obj_info->index_map[hash_index];
    
    for (; result; result = result->next)
    {
        if (Vec3Equal(pos, result->position) &&
            Vec2Equal(uv, result->uv) &&
            Vec3Equal(norm, result->normal))
        {
            return result;
        }
        
    }
    
    return result;
}

internal Mesh
LoadOBJModel(Platform* platform, Assets* assets, char* model_path)
{
    Mesh result = {};
    FileResult file = platform->ReadEntireFile(model_path);
    OBJFileInfo obj_info = GetOBJFileInfo(&assets->arena, file);
    
    result.vertices = PushMemory(&assets->arena, Vertex3D, obj_info.face_count * 3);
    result.indices = PushMemory(&assets->arena, u32, obj_info.face_count * 3);
    
    TemporaryArena temp_memory = BeginTemporaryMemory(&assets->arena);
    
    u32 pos_count = 0, normal_count = 0, uv_count = 0;
    
    vec3* obj_positions = PushMemory(&assets->arena, vec3, obj_info.vertex_count);
    vec3* obj_normals = PushMemory(&assets->arena, vec3, obj_info.normal_count);
    vec2* obj_uvs = PushMemory(&assets->arena, vec2, obj_info.uv_count);
    
    char prefix[3];
    char* data = (char*)file.data;
    
    while(!IsEOFChar(OBJCurrentChar(&obj_info)))
    {
        prefix[0] = OBJCurrentChar(&obj_info);
        prefix[1] = OBJNextChar(&obj_info);
        prefix[2] = '\0';
        
        if (StringMatch(prefix, "v "))
        {
            obj_positions[pos_count++] = OBJNextVec3(&obj_info);
        }
        else if (StringMatch(prefix, "vn"))
        {
            obj_normals[normal_count++] = OBJNextVec3(&obj_info);
        }
        else if (StringMatch(prefix, "vt"))
        {
            obj_uvs[uv_count++] = OBJNextVec2(&obj_info);
        }
        else if (StringMatch(prefix, "f "))
        {
            OBJFace face = OBJGetFace(&obj_info);
            
            for (u32 i = 0; i < 3; ++i)
            {
                vec3 vertex_pos = obj_positions[face.position_indices[i]];
                vec3 vertex_norm = obj_normals[face.normal_indices[i]];
                vec2 vertex_uv = obj_uvs[face.uv_indices[i]];
                
                OBJIndexBucket* index_bucket = OBJGetIndex(&obj_info, vertex_pos, vertex_norm, vertex_uv);
                
                if (!index_bucket)
                {
                    u32 vertex_array_index = result.num_vertices;
                    OBJPutIndex(&assets->arena, &obj_info, 
                                vertex_pos, vertex_norm, vertex_uv, 
                                vertex_array_index);
                    result.indices[result.num_indices++] = vertex_array_index;
                    
                    Vertex3D* vertex = result.vertices + result.num_vertices;
                    vertex->position = vertex_pos;
                    vertex->normal = vertex_norm;
                    vertex->uv = vertex_uv;
                    
                    vertex++;
                    result.num_vertices++;
                    
                    LogM("Position ");
                    Log(&vertex->position);
                    LogM("Normal ");
                    Log(&vertex->normal);
                }
                else
                {
                    result.indices[result.num_indices++] = index_bucket->index;
                }
            }
            
            
        }
        
        OBJSkipLine(&obj_info);
    }
    
    
    LogM("Num vertices %d", result.num_vertices);
    /*
        if (result.num_indices == obj_info.face_count * 3)
        {
            result.num_indices = 0;
        }
    */
    
    EndTemporaryMemory(&temp_memory);
    platform->FreeFileMemory(file.data);
    
    return result;
}

