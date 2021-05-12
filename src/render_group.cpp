#define PushRenderEntry(group, type) \
(type *)push_render_entry(group, sizeof(type), RENDER_ENTRY_##type) \


internal RenderGroup 
setup_render_group(mat4 projection, Camera camera, Renderer* ren, Assets* assets)
{
    RenderGroup render_group = {};

    render_group.setup.projection = projection;
    render_group.setup.camera = camera;
    render_group.renderer = ren;
    render_group.assets = assets;



    return render_group;
}

internal void
add_sprite_to_setup(RenderSetup* render_setup, SpriteHandle sprite_handle)
{
    for (u32 i = 0; i < render_setup->num_sprites; ++i)
    {
        if (render_setup->sprite_handles[i] == sprite_handle)
            return;
    }
    ASSERT(render_setup->num_sprites < 32);
    render_setup->sprite_handles[render_setup->num_sprites] = sprite_handle;
    ++render_setup->num_sprites;
}

internal void*
push_render_entry(RenderGroup* group, u32 size, RenderEntryType type)
{
    RenderEntryHeader* header = 0;

    if ((group->renderer->push_buffer_size + size) < group->renderer->push_buffer_max_size)
    {
        header = (RenderEntryHeader*)(group->renderer->push_buffer_base +  
                                      group->renderer->push_buffer_size);
        header->entry_type = type;
        group->renderer->push_buffer_size += (size + sizeof(*header));
    }
    else
    {
       ASSERT(false);
    }


    return ((u8*)header) + sizeof(*header);
}


#if 0
internal inline void 
write_quad(RenderGroup* group,
           SpriteHandle sprite_handle, 
           vec2 positions[4],
           vec2 uvs[4],
           Color color)
{
    Sprite* sprite = get_loaded_sprite(group->assets, sprite_handle);
    Renderer* ren = group->renderer;

    VertexData* vertex = ren->vertices + ren->vertex_count;
    u32* index = ren->indices + ren->indices_count;

    for (u32 i = 0; i < VERTICES_PER_QUAD; ++i)
    {
        vertex->position = V3(positions[i], 0.0f);
        vertex->color = color;
        vertex->uv = uvs[i];
        vertex->texture_slot = (f32)sprite->slot;

        ++vertex;

    }

    for (u32 i = 0; i < INDICES_PER_QUAD; ++i)
    {
        *index = ((group->current_quads->num_quads-1) * VERTICES_PER_QUAD) + Quad_Indices[i];
        ++index;
    }


    ren->vertex_count += VERTICES_PER_QUAD;
    ren->indices_count += INDICES_PER_QUAD;
}
#endif

// internal void
// push_quad(RenderGroup* group, 
          // SpriteHandle sprite_handle,
          // vec2 positions[4], 
          // vec2 uvs[4],
          // Color color)
// {
    // TexturedQuadsEntry* quads = get_current_quads(group, 1);
    // add_sprite_to_setup(group->current_setup, sprite_handle);
// 
    // write_quad(group, sprite_handle, positions, uvs, color);
// }

internal void
push_quad(RenderGroup* group, 
          vec2 position, 
          vec2 size, 
          Color color, 
          u32 layer,
          SpriteHandle sprite_handle = 0)
{
    add_sprite_to_setup(&group->setup, sprite_handle);
    //TexturedQuadsEntry* quads = get_current_quads(group, 1);
    QuadEntry* entry = PushRenderEntry(group, QuadEntry);

    entry->position = position;
    entry->size = size;
    entry->color = color;
    entry->sprite_handle = sprite_handle;

#if 0
    Renderer* ren = group->renderer;



    vec2 positions[] =
    {
        V2(1.0f,  1.0f) * size + position, // top right
        V2(1.0f,  0.0f) * size + position, // bottom right
        V2(0.0f,  0.0f) * size + position, // bottom left
        V2(0.0f,  1.0f) * size + position, // top left
    };

    vec2 uvs[] = {
        V2(1.0f, 1.0f),
        V2(1.0f, 0.0f),
        V2(0.0f, 0.0f),
        V2(0.0f, 1.0f),
    };

    write_quad(group, sprite_handle, positions, uvs, color);
#endif
}



internal void
push_quad(RenderGroup* group, 
          SubSprite* subsprite,
          vec2 position, 
          vec2 size, 
          Color color, 
          u32 layer)
{
    add_sprite_to_setup(&group->setup, subsprite->sprite_sheet);
    //TexturedQuadsEntry* quads = get_current_quads(group, 1);
    QuadEntry* entry = PushRenderEntry(group, QuadEntry);

    entry->position = position;
    entry->size = size;
    entry->color = color;
    entry->subsprite = subsprite;

    // Sprite* sprite_sheet = get_loaded_sprite(group->assets, subsprite->sprite_sheet);
// 
    // vec2 positions[] =
    // {
        // V2(1.0f,  1.0f) * size + position, // top right
        // V2(1.0f,  0.0f) * size + position, // bottom right
        // V2(0.0f,  0.0f) * size + position, // bottom left
        // V2(0.0f,  1.0f) * size + position, // top left
    // };
// 
    // write_quad(group, subsprite->sprite_sheet, positions, subsprite->uvs, color);
}

