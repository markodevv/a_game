internal void
AssetsInit(Assets* assets, MemoryArena* arena)
{
    SubArena(arena, &assets->arena, Megabytes(64));

    assets->sprites = PushMemory(&assets->arena, Sprite, 256);
    assets->loaded_sprite_queue = PushMemory(&assets->arena, SpriteHandle, 50);
}

internal SpriteHandle
LoadSprite(Platform* platform, Assets* assets, char* sprite_path)
{
    FileResult file = platform->ReadEntireFile(sprite_path);
    Sprite* sprite = assets->sprites + assets->num_sprites;

    SpriteHandle result = 0;

    if (file.data)
    {
        stbi_set_flip_vertically_on_load(1);
        sprite->data = stbi_load_from_memory((u8*)file.data,
                                                 file.size,
                                                 &sprite->width,
                                                 &sprite->height, 
                                                 &sprite->channels,
                                                 0);

        sprite->name = StringCopy(&assets->arena, sprite_path);

        platform->FreeFileMemory(file.data);
        result = assets->num_sprites++;
    }

    assets->loaded_sprite_queue[assets->num_queued_sprites++] = result;

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
