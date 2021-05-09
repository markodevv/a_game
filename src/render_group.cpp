#define PushRenderEntry(group, type) \
(type *)push_render_entry(group, sizeof(type), RENDER_ENTRY_##type) \

u32 Quad_Indices[] = {
    0, 1, 3,
    1, 2, 3,
};

internal RenderGroup 
render_group_begin(RenderSetup* render_setup, Renderer* ren, Assets* assets)
{
    RenderGroup render_group = {};

    render_group.current_setup = render_setup;
    render_group.assets = assets;
    render_group.renderer = ren;

    ren->assets = assets;

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

    group->current_quads = 0;

    return ((u8*)header) + sizeof(*header);
}

internal TexturedQuadsEntry*
get_current_quads(RenderGroup* group, u32 num_quads)
{
    TexturedQuadsEntry* quads = group->current_quads;
    if (!quads)
    {
        quads = PushRenderEntry(group, TexturedQuadsEntry);
        *quads = {};
        quads->vertex_offset = group->renderer->vertex_count;
        quads->index_offset = group->renderer->indices_count;
        quads->render_setup = group->current_setup;

        group->current_quads = quads;
    }
    quads->num_quads += num_quads;

    return quads;
}


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
        *index = ren->vertex_count + Quad_Indices[i];
        ++index;
    }


    ren->vertex_count += VERTICES_PER_QUAD;
    ren->indices_count += INDICES_PER_QUAD;
}

internal void
push_quad(RenderGroup* group, 
          SpriteHandle sprite_handle,
          vec2 positions[4], 
          vec2 uvs[4],
          Color color)
{
    TexturedQuadsEntry* quads = get_current_quads(group, 1);

    write_quad(group, sprite_handle, positions, uvs, color);
}

internal void
push_quad(RenderGroup* group, 
          SpriteHandle sprite_handle,
          vec2 position, 
          vec2 size, 
          Color color, 
          f32 layer)
{
    TexturedQuadsEntry* quads = get_current_quads(group, 1);
    Renderer* ren = group->renderer;

    add_sprite_to_setup(group->current_setup, sprite_handle);


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
}



internal void
push_quad(RenderGroup* group, 
          SubSprite* subsprite,
          vec2 position, 
          vec2 size, 
          Color color, 
          f32 layer)
{
    TexturedQuadsEntry* quads = get_current_quads(group, 1);
    Renderer* ren = group->renderer;

    add_sprite_to_setup(group->current_setup, subsprite->sprite_sheet);
    Sprite* sprite_sheet = get_loaded_sprite(group->assets, subsprite->sprite_sheet);

    vec2 positions[] =
    {
        V2(1.0f,  1.0f) * size + position, // top right
        V2(1.0f,  0.0f) * size + position, // bottom right
        V2(0.0f,  0.0f) * size + position, // bottom left
        V2(0.0f,  1.0f) * size + position, // top left
    };

    write_quad(group, subsprite->sprite_sheet, positions, subsprite->uvs, color);
}

internal void
push_quad(RenderGroup* group, vec2 position, vec2 size, Color color, f32 layer)
{
    push_quad(group, 0, position, size, color, layer);
}
