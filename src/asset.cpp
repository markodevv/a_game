///// SPRITE LOADING //////

internal Sprite*
GetSprite(Assets* assets, SpriteID id)
{
    Sprite* sprite = assets->sprites + id;
    
    if (sprite->asset_state == ASSET_LOCKED)
    {
        sprite = assets->sprites + 0;
    }
    
    return sprite;
}

struct LoadSpriteWorkData
{
    Platform* platform;
    
    MemoryTask* task;
    Assets* assets;
    SpriteEnum sprite_id;
    char* sprite_path;
};


internal void
LoadSpriteWork(void* data)
{
    LoadSpriteWorkData* sprite_data = (LoadSpriteWorkData*)(data);
    Platform* platform = sprite_data->platform;
    FileResult file = platform->ReadEntireFile(sprite_data->sprite_path);
    Sprite* sprite = 0;
    
    if (ValidFile(&file))
    {
        sprite = sprite_data->assets->sprites + (SpriteID)sprite_data->sprite_id;
        
        stbi_set_flip_vertically_on_load(1);
        sprite->data = stbi_load_from_memory((u8*)file.data,
                                             file.size,
                                             &sprite->width,
                                             &sprite->height, 
                                             &sprite->channels,
                                             0);
        
        sprite_data->assets->loaded_sprite_queue[sprite_data->assets->num_queued_sprites++] = sprite_data->sprite_id;
        sprite->asset_state = ASSET_UNLOCKED;
        sprite->name = sprite_data->sprite_path;
        
        platform->FreeFile(&file);
    }
    else
    {
        LogM("Failed to load sprite %s.\n", sprite_data->sprite_path);
    }
    
    EndMemoryTask(sprite_data->task);
    
}

internal void
LoadSprite(Platform* platform, Assets* assets, SpriteEnum sprite_enum)
{
    MemoryTask* task = BeginMemoryTask(&assets->task_queue);
    if (task)
    {
        LoadSpriteWorkData* work_data = PushMemory(&task->arena, LoadSpriteWorkData);
        work_data->task = task;
        work_data->assets = assets;
        work_data->sprite_id = sprite_enum;
        work_data->platform = platform;
        
        switch(sprite_enum)
        {
            case WHITE_SPRITE:
            {
                work_data->sprite_path = "../assets/white.png";
            } break;
            case RED_SPRITE:
            {
                work_data->sprite_path  = "../assets/red.png";
            } break;
            case GREEN_SPRITE:
            {
                work_data->sprite_path = "../assets/green.png";
            } break;
            case BLUE_SPRITE:
            {
                work_data->sprite_path = "../assets/blue.png";
            } break;
            case PINK_SPRITE:
            {
                work_data->sprite_path = "../assets/pink.png";
            } break;
            default:
            {
                LogM("No path for sprite enum %i!", sprite_enum);
            }
        }
        
        platform->PushWorkEntry(platform->work_queue, LoadSpriteWork, work_data);
    }
    
}


internal SpriteID
AddSubsprite(Assets* assets, SpriteID main_sprite)
{
    Assert(assets->num_sprites >= NUM_SPRITES);
    
    SpriteID id = (SpriteID)assets->num_sprites++;
    assets->sprites[id].type = TYPE_SUBSPRITE;
    assets->sprites[id].main_sprite = main_sprite;
    assets->sprites[id].asset_state = ASSET_UNLOCKED;
    
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
    sprite->asset_state = ASSET_UNLOCKED;
    
    subsprite->type = TYPE_SUBSPRITE;
    subsprite->main_sprite = id;
    
    return subsprite_id;
}


internal SpriteID
DEBUGCreateFontSprite(Assets* assets, u32 w, u32 h, u32 channels)
{
    Assert(assets->num_sprites >= NUM_SPRITES);
    
    SpriteID id = assets->num_sprites++;
    Sprite* sprite = assets->sprites + id;
    
    sprite->data = PushMemory(&assets->arena, u8, (w*h));
    sprite->width = w;
    sprite->height = h;
    sprite->channels = channels;
    
    sprite->type = TYPE_SPRITE;
    sprite->asset_state = ASSET_UNLOCKED;
    
    
    assets->loaded_sprite_queue[assets->num_queued_sprites++] = id;
    
    return id;
}

//////// FONT LOADING ////////////////

internal Font
LoadFontTest(Platform* platform, Assets* assets, char* font_path, i8 font_size)
{
    Font font = {};
    i32 ascent = 0, bitmap_width = 1024, bitmap_height = font_size;
    f32 xpos = 2, scale, baseline;
    FileResult font_file = platform->ReadEntireFile(font_path);
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
    platform->FreeFile(&font_file);
    
    return font;
}

////////// OBJ loading ////////////

struct OBJIndexBucket
{
    u32 index;
    vec3 key;
    
    OBJIndexBucket* next;
};


struct OBJFileInfo
{
    u32 vertex_count;
    u32 face_count;
    u32 normal_count;
    u32 uv_count;
    
    OBJIndexBucket** index_map;
};

internal OBJFileInfo
GetOBJFileInfo(MemoryArena* arena, FileResult file)
{
    OBJFileInfo result = {};
    char prefix[3];
    result.index_map = PushMemory(arena, OBJIndexBucket*, 4096);
    
    while (!IsEOF(&file))
    {
        NextWord(&file, prefix);
        
        if (StringMatch(prefix, "v"))
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
        else if (StringMatch(prefix, "f"))
        {
            result.face_count++;
        }
        NextLine(&file);
    }
    
    
    return result;
}

internal Material
OBJLoadMaterial(Platform* platform, char* mat_path)
{
    Material result = {};
    char prefix[6];
    FileResult file = platform->ReadEntireFile(mat_path);
    
    if (ValidFile(&file))
    {
        while (!IsEOF(&file))
        {
            NextWord(&file, prefix);
            
            if (StringMatch(prefix, "Ka"))
            {
                NextChar(&file);
                result.ambient = NextVec3(&file);
            }
            else if (StringMatch(prefix, "Kd"))
            {
                NextChar(&file);
                result.diffuse = NextVec3(&file);
            }
            else if (StringMatch(prefix, "Ks"))
            {
                NextChar(&file);
                result.specular = NextVec3(&file);
            }
            else if (StringMatch(prefix, "Ns"))
            {
                NextChar(&file);
                result.shininess = NextInt(&file) / 10.0f;
            }
            
            NextLine(&file);
        }
        platform->FreeFile(&file);
    }
    
    return result;
}

struct OBJFace
{
    u32 position_indices[3];
    u32 normal_indices[3];
    u32 uv_indices[3];
};

internal OBJFace 
OBJGetFace(OBJFileInfo* obj_info, FileResult file)
{
    OBJFace face = {};
    u8 index = 0;
    char c = CurrentChar(&file);
    
    while(1)
    {
        if (IsDigit(c))
        {
            //NOTE(Marko): -1 because .obj file is indexed starting from 1
            // Face = vposition/vtexture/vnormal * 3
            face.position_indices[index] = NextInt(&file) - 1;
            face.uv_indices[index] = NextInt(&file) - 1;
            face.normal_indices[index] = NextInt(&file) - 1;
            
            index++;
        }
        
        if (IsNewLineChar(CurrentChar(&file))
            || IsEOF(&file))
        {
            break;
        }
        c = NextChar(&file);
    }
    
    return face;
}

internal void
OBJPutIndex(MemoryArena* arena, OBJFileInfo* obj_info, vec3 key, u32 current_index)
{
    u32 hash = key.x * 3 + key.y * 9 + key.z + 17;
    u32 hash_index = hash % (4096-1);
    
    OBJIndexBucket** bucket = &obj_info->index_map[hash_index];
    
    
    if (!(*bucket))
    {
        *bucket = PushMemory(arena, OBJIndexBucket);
        (*bucket)->key = key;
        (*bucket)->index = current_index;
    }
    else
    {
        OBJIndexBucket* temp = PushMemory(arena, OBJIndexBucket);
        temp->next = *bucket;
        *bucket = temp;
        (*bucket)->key = key;
        (*bucket)->index = current_index;
    }
    
}

internal OBJIndexBucket*
OBJGetIndex(OBJFileInfo* obj_info, vec3 key)
{
    u32 hash = key.x * 3 + key.y * 9 + key.z + 17;
    u32 hash_index = hash % (4096-1);
    
    OBJIndexBucket* result = obj_info->index_map[hash_index];
    
    for (; result; result = result->next)
    {
        if (Vec3Equal(key, result->key))
        {
            return result;
        }
        
    }
    
    return result;
}

struct LoadOBJMeshWorkData
{
    Platform* platform;
    
    MemoryTask* task;
    Assets* assets;
    MeshEnum mesh_id;
    char* mesh_path;
};

internal void
LoadOBJMeshWork(void* param)
{
    LoadOBJMeshWorkData* work_data = (LoadOBJMeshWorkData*)param;
    Assets* assets = work_data->assets;
    Mesh* mesh = assets->meshes + work_data->mesh_id;
    Platform* platform = work_data->platform;
    
    FileResult file = platform->ReadEntireFile(work_data->mesh_path);
    
    if (ValidFile(&file))
    {
        
        OBJFileInfo obj_info = GetOBJFileInfo(&work_data->task->arena, file);
        
        mesh->vertices = (Vertex3D*)platform->Allocate(sizeof(Vertex3D) * obj_info.face_count * 3);
        mesh->indices = (u32*)platform->Allocate(sizeof(u32) * obj_info.face_count * 3);
        
        u32 pos_count = 0, normal_count = 0, uv_count = 0;
        
        vec3* obj_positions = PushMemory(&work_data->task->arena, vec3, obj_info.vertex_count);
        vec3* obj_normals = PushMemory(&work_data->task->arena, vec3, obj_info.normal_count);
        vec2* obj_uvs = PushMemory(&work_data->task->arena, vec2, obj_info.uv_count);
        
        char prefix[6];
        char* data = (char*)file.data;
        
        while(!IsEOF(&file))
        {
            NextWord(&file, prefix);
            
            if (StringMatch(prefix, "v"))
            {
                obj_positions[pos_count++] = NextVec3(&file);
            }
            else if (StringMatch(prefix, "vn"))
            {
                obj_normals[normal_count++] = NextVec3(&file);
            }
            else if (StringMatch(prefix, "vt"))
            {
                obj_uvs[uv_count++] = NextVec2(&file);
            }
            else if (StringMatch(prefix, "f"))
            {
                OBJFace face = OBJGetFace(&obj_info, file);
                
                for (u32 i = 0; i < 3; ++i)
                {
                    vec3 key = V3(face.position_indices[i],
                                  face.normal_indices[i],
                                  face.uv_indices[i]);
                    
                    OBJIndexBucket* index_bucket = OBJGetIndex(&obj_info, key);
                    
                    if (!index_bucket)
                    {
                        u32 vertex_array_index = mesh->num_vertices;
                        
                        OBJPutIndex(&work_data->task->arena, &obj_info, 
                                    key,
                                    vertex_array_index);
                        mesh->indices[mesh->num_indices++] = vertex_array_index;
                        
                        Vertex3D* vertex = mesh->vertices + mesh->num_vertices++;
                        
                        vertex->position = obj_positions[face.position_indices[i]];
                        vertex->normal = obj_normals[face.normal_indices[i]];
                        vertex->uv = obj_uvs[face.uv_indices[i]];
                        
                    }
                    else
                    {
                        Vertex3D* vertex = mesh->vertices + index_bucket->index;
                        
                        mesh->indices[mesh->num_indices++] = index_bucket->index;
                    }
                }
            }
            else if (StringMatch(prefix, "mtllib"))
            {
                u32 max_path_size = 128;
                char* material_path = PushMemory(&work_data->task->arena, char, max_path_size);
                u32 path_len = GetPathFromFilePath(material_path, max_path_size, work_data->mesh_path);
                
                char c = CurrentChar(&file);
                
                if (c == ' ')
                {
                    c = NextChar(&file);
                }
                
                for(u32 i = path_len; i < max_path_size; ++i)
                {
                    if (IsNewLineChar(c))
                    {
                        material_path[i] = '\0';
                        mesh->material = OBJLoadMaterial(platform, material_path);
                        break;
                    }
                    else if (i == max_path_size - 1)
                    {
                        LogM("Material path is longer than max path of %i\n", max_path_size);
                    }
                    material_path[i] = c;
                    c = NextChar(&file);
                }
            }
            
            NextLine(&file);
        }
        //mesh->material = CreateDefaultMaterial();
        
        platform->FreeFile(&file);
        
        assets->loaded_mesh_queue[assets->num_queued_meshes++] = work_data->mesh_id;
        
        mesh->asset_state = ASSET_UNLOCKED;
    }
    else
    {
        LogM("Failed to load OBJ file %s.\n", work_data->mesh_path);
    }
    
    EndMemoryTask(work_data->task);
}


internal void 
LoadOBJMesh(Platform* platform, Assets* assets, MeshEnum mesh_id)
{
    MemoryTask* task = BeginMemoryTask(&assets->task_queue);
    
    LoadOBJMeshWorkData* data = PushMemory(&task->arena, LoadOBJMeshWorkData);
    data->assets = assets;
    data->task = task;
    data->mesh_id = mesh_id;
    data->platform = platform;
    
    if (task)
    {
        switch(mesh_id)
        {
            case MESH_HOUSE:
            {
                data->mesh_path = "../assets/models/cottage.obj";
            } break;
            case MESH_CUBE:
            {
                data->mesh_path = "../assets/models/cube.obj";
            } break;
            case MESH_SPHERE:
            {
                data->mesh_path = "../assets/models/sphere.obj";
            } break;
        }
        
        platform->PushWorkEntry(platform->work_queue, LoadOBJMeshWork, data);
    }
}

internal Mesh*
GetMesh(Assets* assets, MeshEnum mesh_id)
{
    Mesh* mesh = assets->meshes + mesh_id;
    
    if (mesh->asset_state = ASSET_LOCKED)
    {
        mesh = assets->meshes + 0;
    }
    
    return mesh;
}

/////// ASSET INIT //////////

internal void
AssetsInit(Platform* platform, Assets* assets, MemoryArena* arena)
{
    SubArena(arena, &assets->arena, Megabytes(128));
    
    assets->task_queue = CreateTaskQueue(&assets->arena, 10);
    
    
    assets->sprites = PushMemory(&assets->arena, Sprite, 512);
    assets->meshes = PushMemory(&assets->arena, Mesh, NUM_MESHES);
    
    
    assets->loaded_sprite_queue = PushMemory(&assets->arena, SpriteID, 50);
    assets->loaded_mesh_queue = PushMemory(&assets->arena, MeshEnum, NUM_MESHES);
    
    LoadSprite(platform, assets, WHITE_SPRITE);
    LoadOBJMesh(platform, assets, MESH_CUBE);
    
    // These assets above are default assets used if assets that are
    // requested are yet not loaded or missing, so we need to wait
    // for them to ensure they are loaded.
    platform->WaitForWorkers(platform->work_queue);
    
    
    for (u32 i = 1; i < NUM_SPRITES; ++i)
    {
        LoadSprite(platform, assets, (SpriteEnum)i);
    }
    assets->num_sprites = NUM_SPRITES;
    
    for (u32 i = 1; i < NUM_MESHES; ++i)
    {
        LoadOBJMesh(platform, assets, (MeshEnum)i);
    }
    
    assets->num_meshes = NUM_MESHES;
    assets->num_sprites = NUM_SPRITES;
    
}
