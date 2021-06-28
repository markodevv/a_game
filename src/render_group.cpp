#define PushRenderEntry(group, type, sort_key) \
(type *)push_render_entry(group, sizeof(type), RENDER_ENTRY_##type, sort_key) \


internal RenderGroup*
setup_render_group(MemoryArena* arena, mat4 projection, Renderer* ren, Assets* assets)
{
    RenderGroup* render_group;

    if (!ren->render_groups)
    {
        ren->render_groups = PushMemory(arena, RenderGroup);
        *ren->render_groups = {};
        render_group = ren->render_groups;
    }
    else
    {
        render_group = ren->render_groups;
        while(render_group->next)
        {
            render_group = render_group->next;
        }
        render_group->next = PushMemory(arena, RenderGroup);
        render_group = render_group->next;
        *render_group = {};
    }

    render_group->setup.projection = projection;
    render_group->assets = assets;

    render_group->push_buffer_capacity = Megabytes(10);
    render_group->push_buffer_base = PushMemory(arena, u8, render_group->push_buffer_capacity);
    render_group->push_buffer_size = 0;

    ASSERT(arena->temp_arena_count);

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


internal void 
push_back_sort_element(RenderGroup* group, u32 sort_key)
{
    SortElement* end = (SortElement*)((group->push_buffer_base + group->push_buffer_capacity) - sizeof(SortElement));
    SortElement* next_element = (end - group->sort_element_count);
    next_element->entry_offset = group->push_buffer_size;
    next_element->key = sort_key;

    ++group->sort_element_count;
    ASSERT((u8*)next_element > (group->push_buffer_base + group->push_buffer_size));
}

internal SortElement*
sort_element_start(RenderGroup* group)
{
    SortElement* end = (SortElement*)((group->push_buffer_base + 
                                       group->push_buffer_capacity) - 
                                       (sizeof(SortElement)));
    SortElement* start = end - (group->sort_element_count - 1);

    return start;
}

internal void*
push_render_entry(RenderGroup* group, u32 size, RenderEntryType type, u32 sort_key)
{
    RenderEntryHeader* header = 0;
    void* result = 0;

    if ((group->push_buffer_size + size) < group->push_buffer_capacity)
    {
        header = (RenderEntryHeader*)(group->push_buffer_base +  
                                      group->push_buffer_size);
        header->entry_type = type;

        result = ((u8*)header) + sizeof(*header); 
        memory_clear(result, size);


        push_back_sort_element(group, sort_key);

        group->push_buffer_size += (size + sizeof(*header));
    }
    else
    {
       ASSERT(false);
    }


    return result;
}



internal void
push_quad(RenderGroup* group, 
          vec2 position, 
          vec2 size, 
          Color color, 
          f32 layer,
          SpriteHandle sprite_handle = 0,
          ShaderId shader_id = SHADER_ID_NORMAL)
{
    add_sprite_to_setup(&group->setup, sprite_handle);
    u32 key = shader_id;

    if (color.a != 255)
    {
        key += 100 + layer;
    }

    QuadEntry* entry = PushRenderEntry(group, QuadEntry, key);



    entry->position = V3(position, layer);
    entry->size = size;
    entry->color = color;
    entry->sprite_handle = sprite_handle;
    entry->shader_id = shader_id;
}



internal void
push_quad(RenderGroup* group, 
          SubSprite* subsprite,
          vec2 position, 
          vec2 size, 
          Color color, 
          f32 layer,
          ShaderId shader_id = SHADER_ID_NORMAL)
{
    ASSERT(subsprite);

    add_sprite_to_setup(&group->setup, subsprite->sprite_sheet);
    u32 key = shader_id;

    if (color.a != 255)
    {
        key += 100 + layer;
    }
    QuadEntry* entry = PushRenderEntry(group, QuadEntry, key);

    entry->position = V3(position, layer);
    entry->size = size;
    entry->color = color;
    entry->subsprite = subsprite;

}

internal void
push_triangle(RenderGroup* group, 
              vec2 p1, 
              vec2 p2, 
              vec2 p3, 
              Color color, 
              f32 layer,
              SpriteHandle sprite_handle = 0,
              ShaderId shader_id = SHADER_ID_NORMAL)
{
    add_sprite_to_setup(&group->setup, sprite_handle);
    u32 key = shader_id;

    if (color.a != 255)
    {
        key += 50 + layer;
    }

    TriangleEntry* entry = PushRenderEntry(group, TriangleEntry, key);

    entry->points[0] = V3(p1, layer);
    entry->points[1] = V3(p2, layer);
    entry->points[2] = V3(p3, layer);
    entry->color = color;
    entry->sprite_handle = sprite_handle;
    entry->shader_id = shader_id;
}
