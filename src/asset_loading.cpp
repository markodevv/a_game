
internal SpriteHandle
load_sprite(Platform* platform, Assets* assets, char* sprite_path)
{
    FileResult file = platform->read_entire_file(sprite_path);
    Sprite* sprite = assets->sprites + assets->num_sprites;

    u32 result = 0;

    if (file.data)
    {
        stbi_set_flip_vertically_on_load(1);
        sprite->data = stbi_load_from_memory((u8*)file.data,
                                                 file.size,
                                                 &sprite->width,
                                                 &sprite->height, 
                                                 &sprite->channels,
                                                 0);

        sprite->name = string_copy(&assets->arena, sprite_path);

        platform->free_file_memory(file.data);
        result = assets->num_sprites++;
    }

    return result;
}


internal SubSprite
subsprite_from_spritesheet(Assets* assets, 
                           SpriteHandle sprite_sheet_handle, 
                           f32 x, f32 y, f32 sprite_width, f32 sprite_height)
{
    Sprite* sprite_sheet = get_loaded_sprite(assets, sprite_sheet_handle);
    SubSprite sub_sprite;
    f32 sheet_width = (f32)sprite_sheet->width;
    f32 sheet_height = (f32)sprite_sheet->height;

    sub_sprite.sprite_sheet = sprite_sheet_handle;
    sub_sprite.uvs[0] = V2((x*sprite_width)     / sheet_width, (y*sprite_height)     / sheet_height);
    sub_sprite.uvs[1] = V2((x*sprite_width)     / sheet_width, ((y+1)*sprite_height) / sheet_height);
    sub_sprite.uvs[2] = V2(((x+1)*sprite_width) / sheet_width, ((y+1)*sprite_height) / sheet_height);
    sub_sprite.uvs[3] = V2((x*sprite_width)     / sheet_width, (y*sprite_height)     / sheet_height);
    sub_sprite.uvs[4] = V2(((x+1)*sprite_width) / sheet_width, (y*sprite_height)     / sheet_height);
    sub_sprite.uvs[5] = V2(((x+1)*sprite_width) / sheet_width, ((y+1)*sprite_height) / sheet_height);

    return sub_sprite;
}



