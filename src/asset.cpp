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
    SubArena(arena, &assets->arena, Megabytes(64));
    
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